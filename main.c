
#include <stdlib.h>
#include <pic32mx.h>
#include "driver/OLED_I2C.h"
#include "defines.h"
#include "maths.h"
#include "missile.h"
#include "main.h"


void ISRHANDLER() {
  if((IFS(0) & 0x100)) {
    int i = 0;
    int j = 0;
    if(timeoutcount == 100) {
        for(i = 0; i < MISSILEMAX; i++) {
            missileHandle(&ms[i]);
            missileHandle(&en[i]);
            for(j = 0; j < MISSILEMAX; j++) {
                if(ms[i].shot && en[j].shot) {
                    if(collision(&ms[i], &en[j])) {
                        ms[i].exploded = 1;
                        ms[i].shot = 0;
                        en[j].exploded = 1;
                        en[j].shot = 0;
                    } 
                }
            }
        }
        timeoutcount = 0;
    }

    enmissilecounter++;
    timeoutcount++;
    IFSCLR(0) = 0x100;
  } 
}


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
            uint16_t up   = (PORTD >> 5) & 0x8;
            uint16_t left = (PORTD) & 0x2;
            uint16_t right = (PORTD) & 0x1;
            uint16_t down = (PORTF << 1) & 0x4;

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
            gy -= 2;
        } 
        if(magnitude < STICKLOWERLIMIT) {
            gy += 2;
        }
    }

    if(stick == XSTICK) {
        magnitude = getStickInput(XSTICK);
        gox = gx;
        if(magnitude > STICKUPPERLIMIT) {
            gx += 2;
        } 
        if(magnitude < STICKLOWERLIMIT) {
            gx -= 2;
        }
    }
}

void cursorINIT() {
    c.co[0].x = gx;
    c.co[0].y = gy;
    
    c.co[1].x = gx;
    c.co[1].y = gy + 1;

    c.co[2].x = gx;
    c.co[2].y = gy - 1;

    c.co[3].x = gx + 1;
    c.co[3].y = gy;

    c.co[4].x = gx - 1;
    c.co[4].y = gy;
}
    

short main() {
    timeINIT();
    btnINIT();
    stickINIT();
    OLED_start();

    cursorINIT();

    IPCSET(2) = 0x31; //Sets IPC02 to interrupt priority and subpriority to max priority, set makes sure that other bits dont change
    IECSET(0) = 0x100; //Sets bit 8 in IEC(0) to 1

    enable_interrupt();
    
    short dx, dy;
    short next = 0;

    while(1) {
        stickInput(XSTICK);
        stickInput(YSTICK); 
        cursorINIT();
        if(mi >= MISSILEMAX) {
            mi = 0;
        }
        if(ei >= MISSILEMAX) {
            ei = 0;
        }

        if((readBtn() & UP) == 0){ //up

        }
        
        if((readBtn() & DOWN) == 0){ //down
            if(!ms[mi].shot){
                shoot(&ms[mi], 64, 59, gx, gy);
            }
            mi++;
        }
        
        if((readBtn() & LEFT) == 0){ //left
            if(!ms[mi].shot){
                shoot(&ms[mi], 32, 59, gx, gy);
            }
            mi++;
        }
        
        if((readBtn() & RIGHT) == 0){ //right
            if(!ms[mi].shot){
                shoot(&ms[mi], 96, 59, gx, gy);
            }
            mi++;
        }

        if(!OLED_boundsCheck(gx, gy)) {
            gx = 64;
            gy = 32;
        }

        prev = TMR2 & 0x7f;
       

        if(enmissilecounter >= 9000) {
            short dx;
            short dy = 59;
            switch((prev & 0x7)) {
                case 0:
                case 1:
                    dx = 32;
                    break;
                case 2:
                    dx = 64;
                    break;
                case 3:
                    dx = 96;
                    break;
            }
            shoot(&en[ei], prev, 4, dx, dy);
            ei++;
            enmissilecounter = 0;
        }

        
        OLED_clrPixel(gox, goy);
        OLED_clrPixel(gox + 1, goy);
        OLED_clrPixel(gox - 1, goy);
        OLED_clrPixel(gox, goy + 1);
        OLED_clrPixel(gox, goy - 1);
        int i = 0;
        for(i = 0; i < 5; i++)
            OLED_setPixel(c.co[i].x, c.co[i].y);
        OLED_refresh();

    }
    
    return 0;
}
