
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
    int x;
    int y;
} point;

typedef struct cursor{
    point p;
    point op;
} cursor;

int gx = 64;
int gy = 32;
int gox, goy;

typedef struct missile {
    int sx, sy;
    int dx, dy;
    int k, d, p;
    point progress[128];
    int shot;
    int exploded;
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
    int i;
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


void shoot(missile * m, int sx, int sy, int dx, int dy) {
    m->sx = sx;
    m->sy = sy;
    m->dx = dx;
    m->dy = dy;
    m->k  = (sy - dy) / (sx - dx);
    m->d  = (int)sqrt(absf(pow((sx - dx), 2) + pow((sy - dy), 2)));
    m->shot = 1;
    m->p = 0;
    m->exploded = 0;

}

void explode(missile * m, int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int xchange = 1 - (radius << 1);    // 1 - radius * 2
    int ychange = 0;
    int error = 0;

    while(x0 >= y0) {
        int i;
        for(i = x0 - x; i <= x0 + x; i++) {
            OLED_setPixel(i, y0 + y);
            OLED_setPixel(i, y0 - y);
        }
        int j;
        for(j = x0 - x; j <= x0 + x; j++) {
            OLED_setPixel(i, y0 + x);
            OLED_setPixel(i, y0 - x);
        }

        y++;
        error += ychange;
        ychange += 2;

        if(((error << 1) + xchange) > 0) {
            x--;
            error += xchange;
            xchange += 2;
        }
    }

}

void removecircle(missile * m, int x0, int y0, int radius) {
    int x = radius;
    int y = 0;
    int xchange = 1 - (radius << 1);    // 1 - radius * 2
    int ychange = 0;
    int error = 0;

    while(x0 >= y0) {
        int i;
        for(i = x0 - x; i <= x0 + x; i++) {
            OLED_clrPixel(i, y0 + y);
            OLED_setPixel(i, y0 - y);
        }
        int j;
        for(j = x0 - x; j <= x0 + x; j++) {
            OLED_clrPixel(i, y0 + x);
            OLED_clrPixel(i, y0 - x);
        }

        y++;
        error += ychange;
        ychange += 2;

        if(((error << 1) + xchange) > 0) {
            x--;
            error += xchange;
            xchange += 2;
        }
    }
}

void clrmissile(missile * m) {
    if(m->exploded) {
        while(m->p >= 0) {
            OLED_clrPixel(m->progress[m->p].x, m->progress[m->p].y);
            m->p--;
        }
        int r = 1;
        while(r <= 3) {
            removecircle(m, m->dx, m->dy, r);
            r++;
        }
    }
}

void missileUpdate(missile * m) {
    if(m->shot) {
        if(m->p < m->d) {
            int x  = m->sx;
            int y  = m->sy;

            float derror = absf((m->sy - m->dy)/(m->sx - m->dx));
            float error  = 0;
            
            if(m->sx < m->dx){
                x++;
            }
            else{
                x--;
            }
            y = m->k * x;
            error = derror;
            while(error > 0.5){
                y += (m->dy > m->sy?1 : -1);
                error -= 1.0;
            }
            m->progress[m->p].x = x;
            m->progress[m->p].y = y;
            m->p++;
            OLED_setPixel(x, y);
        }
        if(m->p == m->d) {
            int r = 1;
            while(r <= 3) {
                 explode(m, m->dx, m->dy, r);
            }
            m->exploded = 1;
        }
    }
}


missile ms[20];


void ISR() {
    int i = 0;


    for(i = 0; i < MISSILEMAX; i++) {
        if(ms[i].shot) {
            missileUpdate(&ms[i]);
            if(ms[i].exploded) {
                clrmissile(&m[i]);
            }
        }
    }
}

void btnINIT() {
    TRISDSET = 0x103;
    TRISFSET = 0x2;
}

int readBtn() {
    while(1) {
            int up   = (PORTD >> 5) & 0x8;
            int left = (PORTD) & 0x2;
            int right = (PORTD) & 0x1;
            int down = (PORTF << 1) & 0x4;

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



int getStickInput(bool stick) {
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


void stickInput(int stick) {
    int magnitude;

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


int main() {
    btnINIT();
    stickINIT();
    OLED_start();
    
    int sx, sy;
    int dx, dy;
    int next = 0;

    int mi = 0;
    

    while(1) {
        stickInput(XSTICK);
        stickInput(YSTICK); 

        if((readBtn() & UP) == 0){ //up
        }
        
        if((readBtn() & DOWN) == 0){ //down
        }
        
        if((readBtn() & LEFT) == 0){ //left
        }
        
        if((readBtn() & RIGHT) == 0){ //right
            if(!next) {
                sx = gx;
                sy = gy;
                next = 1;
            }
            else 
            {
                dx = gx;
                dy = gy;
                shoot(&ms[mi], sx, sy, dx, dy);
                mi++;
                if(mi == 20) {
                    mi = 0;
                }
                next = 0;
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