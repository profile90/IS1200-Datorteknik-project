#ifndef HEADER_OLEDI2C
#define HEADER_OLEDI2C

/********* Hardware Specific Defines ***********/
#define bitmapbyte(x) bitmap[x]

#define PROGMEM
#define bitmapdatatype unsigned char*

#define SDA	18					// A4 (Remeber to set the jumper correctly)
#define SCL	19					// A5 (Remeber to set the jumper correctly)

#ifndef TWI_FREQ
	#define TWI_FREQ 400000L

#endif


#define _I2CCON_ON 15
#define _I2CCON_STREN 6
#define _I2CSTAT_BCL 10
#define _I2CCON_SEN 0
#define _I2CSTAT_IWCOL 7
#define _I2CSTAT_TRSTAT 14
#define _I2CSTAT_ACKSTAT 15
#define _I2CCON_PEN 2



#define SH1106_ADDR		0x3C

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

#define MSBFIRST 1


#define SH1106_COMMAND			0x00
#define SH1106_DATA			    0xC0
#define SH1106_DATA_CONTINUE	0x40

#define RST_NOT_IN_USE	255



/******** SH1106 Commandset ************************
    Derivate of SSD1306 Commandset, see: 
        https://github.com/rogerclarkmelbourne/Arduino_STM32/blob/master/STM32F1/libraries/OLEDI2C/OLEDI2C.h

    With notes and remarks from Axel Pettersson & Samuel Larsson, see:
        https://github.com/StelFoog/IS1200-Datorteknik/blob/master/Project/driver/OLEDI2C.h
    
****************************************************/

// Fundamental Commands
#define SH1106_SET_CONTRAST_CONTROL					0x81
#define SH1106_DISPLAY_ALL_ON_RESUME					0xA4
#define SH1106_DISPLAY_ALL_ON							0xA5
#define SH1106_NORMAL_DISPLAY							0xA6
#define SH1106_INVERT_DISPLAY							0xA7
#define SH1106_DISPLAY_OFF								0xAE
#define SH1106_DISPLAY_ON								0xAF
#define SH1106_NOP										0xE3
// Scrolling Commands
#define SH1106_HORIZONTAL_SCROLL_RIGHT					0x26
#define SH1106_HORIZONTAL_SCROLL_LEFT					0x27
#define SH1106_HORIZONTAL_SCROLL_VERTICAL_AND_RIGHT	    0x29
#define SH1106_HORIZONTAL_SCROLL_VERTICAL_AND_LEFT		0x2A
#define SH1106_DEACTIVATE_SCROLL						0x2E
#define SH1106_ACTIVATE_SCROLL							0x2F
#define SH1106_SET_VERTICAL_SCROLL_AREA				    0xA3
// Addressing Setting Commands
#define SH1106_SET_LOWER_COLUMN					    	0x02     // NOTE(): Starts at 0x00 in SSD1306 Commandset
#define SH1106_SET_HIGHER_COLUMN						0x10
#define SH1106_SETSTARTPAGE                             0xB0     // NOTE(): For some reason page start at 0xB0
#define SH1106_MEMORY_ADDR_MODE						    0x20
#define SH1106_SET_COLUMN_ADDR							0x21
#define SH1106_SET_PAGE_ADDR							0x22
// Hardware Configuration Commands
#define SH1106_SET_START_LINE							0x40
#define SH1106_SET_SEGMENT_REMAP						0xA0
#define SH1106_SET_MULTIPLEX_RATIO						0xA8
#define SH1106_COM_SCAN_DIR_INC						    0xC0
#define SH1106_COM_SCAN_DIR_DEC						    0xC8
#define SH1106_SET_DISPLAY_OFFSET						0xD3
#define SH1106_SET_COM_PINS							    0xDA
#define SH1106_CHARGE_PUMP								0x8D
// Timing & Driving Scheme Setting Commands
#define SH1106_SET_DISPLAY_CLOCK_DIV_RATIO				0xD5
#define SH1106_SET_PRECHARGE_PERIOD					    0xD9
#define SH1106_SET_VCOM_DESELECT						0xDB


#define BUFFERSIZE 1024
#include <stdint.h>


uint8_t			scrbuf[1024];

void	HW__initTWI();
void	HW__sendTWIstart();
void	HW__sendTWIstop();
void	HW__sendTWIcommand(uint8_t value);

void	OLED_refresh();
void	OLED_start();
void	OLED_setBrightness(uint8_t value);
void    OLED_fill();
void	OLED_clear();
void	OLED_invert(char mode);
void	OLED_setPixel(uint16_t x, uint16_t y);
void	OLED_clrPixel(uint16_t x, uint16_t y);

#endif
