
#include <stdlib.h>
#include <pic32mx.h>
#include "driver/OLED_I2C.h"
#include "defines.h"

/*
 * PUT IN GLOBAL DEFINE
 */


uint16_t x = 64;
uint16_t y = 32;
uint16_t oy, ox;


void btnINIT() {
    TRISDSET = (1 << 8) | (1 << 0) | (1 << 1);
    TRISFSET = (1 << 1);
}

void readBtn() {

    
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
        oy = y;
        if(magnitude > STICKUPPERLIMIT) {
            y--;
        } 
        if(magnitude < STICKLOWERLIMIT) {
            y++;
        }
    }

    if(stick == XSTICK) {
        magnitude = getStickInput(XSTICK);
        ox = x;
        if(magnitude > STICKUPPERLIMIT) {
            x++;
        } 
        if(magnitude < STICKLOWERLIMIT) {
            x--;
        }
    }
}



int main() {
    stickINIT();
    OLED_start();
    
    
    while(1) {
        stickInput(XSTICK);
        stickInput(YSTICK);
        if(!OLED_boundsCheck(x, y)) {
                x = 64;
                y = 32;
        }

        OLED_clrPixel(ox, oy);
        OLED_setPixel(x, y);
  
        OLED_refresh();

    }
    
    return 0;
}