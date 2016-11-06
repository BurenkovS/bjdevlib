/*
 * TravelBoxReadUniqueId.c
 *
 * Created: 05.11.2016 15:47:42
 *  Author: Admin
 */ 


#include "unique_id.h"
#include "lcd_tb.h"
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

uint64_t swapByteOrder(uint64_t value)
{
	uint64_t uvalue = (uint64_t)value;
	uint64_t swapped = ( ((0x00000000000000FF) & (uvalue >> 56))
	| ((0x000000000000FF00) & (uvalue >> 40))
	| ((0x0000000000FF0000) & (uvalue >> 24))
	| ((0x00000000FF000000) & (uvalue >> 8))
	| ((0x000000FF00000000) & (uvalue << 8))
	| ((0x0000FF0000000000) & (uvalue << 24))
	| ((0x00FF000000000000) & (uvalue << 40))
	| ((0xFF00000000000000) & (uvalue << 56)));
	return (uint64_t)swapped;
}

//this can be defined in project settings, makefile or provided through -D compiler option in command line
#define GLOBALY_DEFINED_USER_ID 0x0102030405060708

#define ID_BYTES 8//DS2411 ID contain 8 bytes. 7 bytes data + 1 byte CRC
uint8_t idBuffer[ID_BYTES];//Buffer for data from DS2411
char idStr[ID_BYTES*2+1];//Buffer for string representation of ID

 int main(void)
 {
	uint8_t i;
	
	//LCD initialization
	LCDInit(LS_ULINE);

	//1-wire interface initialization
	initUniqId();
	
	//small delay... just in case :)
	_delay_ms(10);

	//read unique ID from DS2411. If successfully - print in on screen
	if(uniqIdGet(idBuffer))
	{
		for(i = 0; i < ID_BYTES; ++i)
			sprintf(idStr+(i*2), "%02x", idBuffer[i]);

		LCDWriteString(idStr);
	}
	//If error during read - print error message
	//Read error may occur when DS2411 is not available. Gen1 devices not have DS2411  
	else
	{
		LCDWriteString("ID not available");
		return 0;//If you don't want your firmware works on gen 1 devices, just halt execution here
	}

	uint64_t userId = GLOBALY_DEFINED_USER_ID;

	//compare current ID with user-provided ID
	//This example use memcmp for comparison. So I should swap bytes in userId variable for correct comparison
	userId = swapByteOrder(userId);
	if (0 != memcmp((void*)idBuffer, (void*)&userId, ID_BYTES))
	{
		LCDWriteStringXY(0,1, "ID not matched!");
		return 0;//break execution
	}

	LCDWriteStringXY(0,1, "ID matched!");//ID matched, start firmware
	//Start to implement your firmware behavior here
 }

