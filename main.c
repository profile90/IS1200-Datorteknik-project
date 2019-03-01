
#include <stdlib.h>
#include <pic32mx.h>
#include "driver/OLED_I2C.h"
#include "defines.h"

/*
 * PUT IN GLOBAL DEFINE
 */

#define UP 0x8
#define DOWN 0x4 
#define LEFT 0x2
#define RIGHT 0x1
#define HEIGHT 64
#define WIDTH 128
#define MISSILEMAX 20
typedef struct point {
    short x;
    short y;
} point;

typedef struct cursor{
    point p;
    point op;
} cursor;

short gx = 64;
short gy = 32;
short gox, goy;

typedef struct missile {
    short sx, sy;
    short dx, dy;
    short k, d, p;
    point progress[128];
    short shot;
    short exploded;
} missile;

float absf(float x) {
    if(x < 0) {
        return (x * -1);
    }
    else {
        return x;
    }
}

float pow(float x, float y) {
    short i;
    float result = x;
    for(i = 1; i < y; i++) {
        result *= result;
    }
    return result;
}

float sqrt(float x) {
    float x0 = x;
    float x1;
    for(; ;) {
        x1 = x0 - (x0 * x0 - x)/(2 * x0);
        float delta = (x1 - x0)/ x0;
        if(delta < .000001 && delta > -.000001) {
            return x1;
        }
        x0 = x1;
    }
}


void shoot(missile * m, short sx, short sy, short dx, short dy) {
    m->sx = sx;
    m->sy = sy;
    m->dx = dx;
    m->dy = dy;
    m->k  = (sy - dy) / (sx - dx);
    m->d  = (short)sqrt(absf(pow((sx - dx), 2) + pow((sy - dy), 2)));
    m->shot = 1;
    m->p = 0;
    m->exploded = 0;

}

void explode(missile * m, short x0, short y0, short radius)
{
    short k, l;
    for( k = -radius; k <= radius; k++) {
        for(l = -radius; l <= radius; l++) {
            if(k*k + l*l <= radius*radius) {
                OLED_setPixel(x0 + l, y0 + k);
            }
        }
    }

}

void removecircle(missile * m, short x0, short y0, short radius) {
    short k, l;
    for( k = -radius; k <= radius; k++) {
        for(l = -radius; l <= radius; l++) {
            if(k*k + l*l <= radius*radius) {
                OLED_clrPixel(x0 + l, y0 + k);
            }
        }
    }
}

void clrmissile(missile * m) {
    if(m->exploded) {
        while(m->p >= 0) {
            OLED_clrPixel(m->progress[m->p].x, m->progress[m->p].y);
            m->p--;
        }
        removecircle(m, m->dx, m->dy, 5);
        }
}

void resetMissile(missile *m) {
    m->sx = 0;
    m->sy = 0;
    m->dx = 0;
    m->dy = 0;
    m->k  = 0;
    m->d  = 0;
    m->shot = 0;
    m->p = 0;
    m->exploded = 0;
}

void missileUpdate(missile * m) {
    if(m->shot) {
        if(m->p >= m->d || m->p >= 128) {
            m->shot = 0;
            explode(m, m->dx, m->dy, 5);
            m->exploded = 1;
        }

        if( m->p != 0) {
            if(m->sx < m->dx) {
                m->progress[m->p].x = m->progress[m->p - 1].x + 1;
            }

            else if(m->sx > m->dx) {
                m->progress[m->p].x = m->progress[m->p - 1].x - 1;
            }

            else {
                if(m->sy < m->dy) {
                    m->progress[m->p].y = m->progress[m->p - 1].y  - 1;
                }
                else {
                     m->progress[m->p].y = m->progress[m->p - 1].y  + 1;
                }
            }
            
         m->progress[m->p].y = (m->progress[m->p - 1].x - m->sx) * m->k + m->sy;
        
        }

        else {
            m->progress[m->p].x = m->sx;
            m->progress[m->p].y = m->sy;
        }

        OLED_setPixel(m->progress[m->p].x, m->progress[m->p].y);
        m->p++;
    }
}


missile ms[20];
short timeoutcount = 0;

void ISRHANDLER() {
    short i = 0;


    /*for(i = 0; i < MISSILEMAX; i++) {
        if(ms[i].shot) {
            missileUpdate(&ms[i]);
            if(ms[i].exploded) {
                clrmissile(&m[i]);
            }
        }
    }*/

  if((IFS(0) & 0x100)) {
    IFSCLR(0) = 0x100;
    if(timeoutcount == 10){
        if(ms[0].shot) {
            missileUpdate(&ms[0]);
        }
      timeoutcount = 0;
    }
    timeoutcount++;
  } 
}

#define TMR2PERIOD ((80000000 / 256) / 10)
#if TMR2PERIOD > 0xffff
#error "Timer period is too big."
#endif

void timeINIT() {
  T2CON = 0x70;
  PR2 = TMR2PERIOD;
  TMR2 = 0;
  T2CONSET = 0x8000; /* Start the timer*/
}

void btnINIT() {
    TRISDSET = 0x103;
    TRISFSET = 0x2;
}

short readBtn() {
    while(1) {
            short up   = (PORTD >> 5) & 0x8;
            short left = (PORTD) & 0x2;
            short right = (PORTD) & 0x1;
            short down = (PORTF << 1) & 0x4;

            return up | left | right | down;
    }    
}

void stickINIT() {
    /*  AD1PCFG-config
     *  bit  2 ~(0x1): set PCFG-pin (a0 (AN2)) 2 to 0 
     *  bit  4 ~(0x1): set PCFG-pin (a1 (AN4)) 4 to 0
     */    
    AD1PCFG  = ~((1 << 2) | (1 << 4)); 
    
    /*  TRISBSET-config
     *  bit  2  (0x1): set a0 (AN2) to input
     *  bit  4  (0x1): set a1 (AN4)to input
     */    
    TRISBSET = (1 << 2) | (1 << 4);
    
    /*  AD1CON1-config
     *  bit 10-8 (0x4): output is 32-bit integer 
     *  bit  7-5 (0x7): set conversion trigger to auto
     */
    AD1CON1  = (0x4 << 8) | (0x7 << 5); 
    
    /*  AD1CON2-config
     *  bit 31-0 (0x0): disable control register 2
     */
    AD1CON2  = 0;
    
    /*  AD1CON3-config
     *  bit 15 (0x1): conversion clock source (ADC internal RC clock)
     */
    AD1CON3 |= (0x1 << 15);

    /*  AD1CON1-config
     *  bit 15 (0x1): start ADC
     */
    AD1CON1 |= (0x1 << 15);             
}



short getStickInput(bool stick) {
    if(stick == XSTICK) {
        /*  AD1CHS-config
         *  bit 16-19 (0x2): set pin a0 (AN2) to input (MUX A settings)
         */ 
        AD1CHS = (0x2 << 16);
    }

    if(stick == YSTICK) {
        /*  AD1CHS-config
         *  bit 16-19 (0x4): set pin a1 (AN4) to input (MUX A settings)
         */ 
        AD1CHS = (0x4 << 16);
    }

    /*  AD1CON1-config
     *  bit 1 (0x1): start samlping ADC
     */ 
    AD1CON1 |= (0x1 << 1);
    
    /*  AD1CON1-check
     *  bit 1 (0x1): check if started
     */
    while(!(AD1CON1  & (0x1 << 1)));
    /*  AD1CON1-check
     *  bit 1 (0x1): check if sample is done
     */
    while(!(AD1CON1  & 0x1));
    /*  ADC1BUF0-check
     *  bit 1 (0x1): check if sample is done
     */
    return ADC1BUF0;
}


void stickInput(short stick) {
    short magnitude;

    if(stick == YSTICK) {
        magnitude = getStickInput(YSTICK);
        goy = gy;
        if(magnitude > STICKUPPERLIMIT) {
            gy--;
        } 
        if(magnitude < STICKLOWERLIMIT) {
            gy++;
        }
    }

    if(stick == XSTICK) {
        magnitude = getStickInput(XSTICK);
        gox = gx;
        if(magnitude > STICKUPPERLIMIT) {
            gx++;
        } 
        if(magnitude < STICKLOWERLIMIT) {
            gx--;
        }
    }
}


short main() {
    timeINIT();
    btnINIT();
    stickINIT();
    OLED_start();

    IPCSET(2) = 0x31; //Sets IPC02 to interrupt priority and subpriority to max priority, set makes sure that other bits dont change
    IECSET(0) = 0x100; //Sets bit 8 in IEC(0) to 1

    enable_interrupt();
    
    short sx, sy;
    short dx, dy;
    short next = 0;

    short mi = 0;
    

    while(1) {
        stickInput(XSTICK);
        stickInput(YSTICK); 

        if((readBtn() & UP) == 0){ //up
        }
        
        if((readBtn() & DOWN) == 0){ //down
            if(ms[0].exploded) {
                clrmissile(&ms[0]);
                resetMissile(&ms[0]);
            }
        }
        
        if((readBtn() & LEFT) == 0){ //left
           if(next) {
            dx = gx;
            dy = gy;
            shoot(&ms[0], sx, sy, dx, dy);
                
            next = 0;
           }
        }
        
        if((readBtn() & RIGHT) == 0){ //right
            if(!next) {
                sx = gx;
                sy = gy;
                next = 1;
            }
            else 
            {
                
            }
        }

        if(!OLED_boundsCheck(gx, gy)) {
                gx = 64;
                gy = 32;
        }

        OLED_clrPixel(gox, goy);
        OLED_setPixel(gx, gy);
  
        OLED_refresh();

    }
    
    return 0;
}