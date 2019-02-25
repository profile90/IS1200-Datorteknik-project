#include "OLED_I2C.h"
#include "pic32mx.h"  /* Declarations of system-specific addresses etc */
#include "font.h"

/*  NOTICE:
 *
 *  Adapted from https://github.com/rogerclarkmelbourne/Arduino_STM32/ and 
 *  
 *
 *  for use with PIC32 and the C-programming language
 * 
 */
inline void OLED__waitForIdleBus() { while (I2C1CON & 0x1f) {} }

void _initTWI()
{
	uint32_t	tpgd;
	IFSCLR(0) = 0xE0000000;									// Clear Interrupt Flag
	IECCLR(0) = 0xE0000000;									// Disable Interrupt
	I2C1CONCLR = (1 << _I2CCON_ON);							// Disable I2C interface
	I2C1BRG = 0x0C2; 									    // Set speed
	I2C1ADD = SH1106_ADDR;									// Set I2C device address
	I2C1CONSET = (1 << _I2CCON_ON) | (1 << _I2CCON_STREN);	// Enable I2C Interface
}


void _sendTWIcommand(uint8_t value) {
    OLED__waitForIdleBus();								// Wait for I2C bus to be Idle before starting
    I2C1CONSET = (1 << _I2CCON_SEN);					// Send start condition
    
    if (I2C1STAT & (1 << _I2CSTAT_BCL)) { return; }		// Check if there is a bus collision
    while (I2C1CON & (1 << _I2CCON_SEN)) {}				// Wait for start condition to finish
    
    I2C1TRN = (SSD1306_ADDR<<1);						// Send device Write address
    while (I2C1STAT & (1 << _I2CSTAT_IWCOL))			// Check if there is a Write collision
    {
        I2C1STATCLR = (1 << _I2CSTAT_IWCOL);			// Clear Write collision flag
        I2C1TRN = (SSD1306_ADDR<<1);					// Retry send device Write address
    }
    
    while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
    while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
    
    I2C1TRN = SSD1306_DATA_CONTINUE;					// Send the command for continous data
    while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
    while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
}



void OLED__start() {

    _sendTWIcommand(SH1106_DISPLAY_OFF);
    _sendTWIcommand(SH1106_SET_DISPLAY_CLOCK_DIV_RATIO);
        _sendTWIcommand(0x80);
    _sendTWIcommand(SH1106_SET_MULTIPLEX_RATIO);
        _sendTWIcommand(0x3F);
    _sendTWIcommand(SH1106_SET_DISPLAY_OFFSET);
        _sendTWIcommand(0x0);
    _sendTWIcommand(SH1106_SET_START_LINE | 0x0);
    _sendTWIcommand(SH1106_CHARGE_PUMP);
    	_sendTWIcommand(0x14);
    _sendTWIcommand(SH1106_MEMORY_ADDR_MODE);
        _sendTWIcommand(0x00); // Horizontal addressing mode 
    _sendTWIcommand(SH1106_SET_SEGMENT_REMAP | 0x1);
    _sendTWIcommand(SH1106_COM_SCAN_DIR_DEC);
    _sendTWIcommand(SH1106_SET_COM_PINS);
        _sendTWIcommand(0x12);
    _sendTWIcommand(SH1106_SET_CONTRAST_CONTROL);
	    _sendTWIcommand(0xCF);
    _sendTWIcommand(SH1106_SET_PRECHARGE_PERIOD);
	    _sendTWIcommand(0xF1);
    _sendTWIcommand(SH1106_SET_VCOM_DESELECT);
        _sendTWIcommand(0x40);
    _sendTWIcommand(SH1106_DISPLAY_ALL_ON_RESUME);
    _sendTWIcommand(SH1106_NORMAL_DISPLAY);
	_sendTWIcommand(SH1106_DISPLAY_ON);

    OLED_clear();
    OLED_refresh();
}


void	OLED__refresh() {
    _waitForIdleBus();
    I2C1CONSET = (1 << _I2CON_SEN);
    
    if(I2C1STAT & (1 << _I2STAT_BCL)) { return; }
    while(I2C1STAT & (1 << _I2STAT_SEN)) {};
    I2C1TRN = (SH1106_ADDR << 1);										

    while(I2C1STAT & (1 << _I2CSTAT_IWCOL)){
        I2C1STATCLR = (1 << _I2CSTAT_IWCOL);
        I2C1TRN     = (SH1106_ADDR << 1)
    }
    while(I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}
    while(I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}

    I2C1TRN = SH1106_DATA_CONTINUE;
    while(I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}
    while(I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}



    for(int i = 0; i < BUFFERSIZE; i++) {
        
    }
}


void    OLED__fill() {

}
void	OLED__clear() {

}
void	OLED__invert(char mode) {

}
void	OLED__setPixel(uint16_t x, uint16_t y) {

}
void	OLED__clrPixel(uint16_t x, uint16_t y) {

}
