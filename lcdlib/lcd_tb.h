/******************************************************************************

                          eXtreme Electronics xAPI(TM)
						  ----------------------------
xAPI is a Powerful but easy to use C library to program the xBoard(TM)
series of AVR development board. The library has support for commonly use tasks
like:-

*LCD interfacing
*MUXED 7 segment displays.
*Remote Control
*Serial Communication
*DC Motor Controls with Speed Control
*Analog Measurement for Sensor Interface.
*Temperature Measurement.
*I2C Communication.
*EEPROM Interface
*Real Time Clock (RTC Interface)

The APIs are highly documented and easy to use even by a beginner.

Versions:
	1.0 - 	Basic Lib
	2.0 - 	Added Custom Char Support, Much flexible connection.
			4 line LCD support.

For More Info Log On to
www.eXtremeElectronics.co.in

Copyright 2008-2009 eXtreme Electronics India

                                    LCD Core
						           ----------
This module is used for interfacing with Standard Alpha Numeric LCD Modules.
For More information please see supplied tutorials and videos.

                                     NOTICE
									--------
NO PART OF THIS WORK CAN BE COPIED, DISTRIBUTED OR PUBLISHED WITHOUT A
WRITTEN PERMISSION FROM EXTREME ELECTRONICS INDIA. THE LIBRARY, NOR ANY PART
OF IT CAN BE USED IN COMMERCIAL APPLICATIONS. IT IS INTENDED TO BE USED FOR
HOBBY, LEARNING AND EDUCATIONAL PURPOSE ONLY. IF YOU WANT TO USE THEM IN
COMMERCIAL APPLICATION PLEASE WRITE TO THE AUTHOR.


WRITTEN BY:
AVINASH GUPTA
me@avinashgupta.com

*******************************************************************************/


#include <avr/io.h>

#include <util/delay.h>

#include "myutils.h"

#ifndef _LCD_H
#define _LCD_H



/*_________________________________________________________________________________________*/

/************************************************
	LCD CONNECTIONS for BJ Devices TB series
*************************************************/

#define LCD_DATA C			//for TB series, Port PC4-PC7 are connected to LCD D4-D7
#define LCD_DATA_POS 4

#define LCD_E C 			//Enable/strobe signal
#define LCD_E_POS	PC2		//Position of enable in above port

#define LCD_RS C			//RS SIGNAL
#define LCD_RS_POS PC0

#define LCD_RW C			//RW SIGNAL
#define LCD_RW_POS PC1

/***********************************************

LCD Type Selection
Uncomment Just one of them

************************************************/

//#define LCD_TYPE_202	//For 20 Chars by 2 lines

//#define LCD_TYPE_204	//For 20 Chars by 4 lines

#define LCD_TYPE_162	//For 16 Chars by 2 lines

//#define LCD_TYPE_164	//For 16 Chars by 4 lines


//************************************************




//************************************************

#define LS_BLINK 0B00000001
#define LS_ULINE 0B00000010
#define LS_NONE	 0B00000000



/***************************************************
			F U N C T I O N S
****************************************************/



void LCDInit(uint8_t style);
void LCDWriteString(const char *msg);
void LCDWriteInt(int val,unsigned int field_length);
void LCDGotoXY(uint8_t x,uint8_t y);

//Low level
void LCDByte(uint8_t,uint8_t);
#define LCDCmd(c) (LCDByte(c,0))
#define LCDData(d) (LCDByte(d,1))

void LCDBusyLoop();





/***************************************************
			F U N C T I O N S     E N D
****************************************************/


/***************************************************
	M A C R O S
***************************************************/
#define LCDClear() LCDCmd(0b00000001)
#define LCDHome() LCDCmd(0b00000010)
#define LcdShowCursor() LCDCmd(0x0E)
#define LcdHideCursor() LCDCmd(0x0C)

#define LCDWriteStringXY(x,y,msg) do{	\
 LCDGotoXY(x,y);						\
 LCDWriteString(msg);					\
} while(0)								\

#define LCDWriteIntXY(x,y,val,fl) do{	\
 LCDGotoXY(x,y);						\
 LCDWriteInt(val,fl);					\
}while(0)								\
/***************************************************/




/*_________________________________________________________________________________________*/
#endif




