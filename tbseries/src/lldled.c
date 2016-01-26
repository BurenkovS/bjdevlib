/*
 * BJ Devices Travel Box series midi controller library
 * @file	lldled.c
 * 
 * @brief	Low level driver for LEDs via shift registers
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "lldled.h"
#include "pinout.h"

#include <avr/pgmspace.h>
#include <stdint.h>

void lldLedSend(RegsChainDescriptor* interface)
{
	uint8_t i;
	uint8_t j;

	outputClear(interface->clk_);//clock falling edge
	outputClear(interface->oe_);//set OE to low
	
	for(i = 0; i < interface->regsNum_; i++)
	{
		for (j = 0; j < 8; j++)
		{
			outputClear(interface->clk_);//clock falling edge
			outputSetToVal(interface->data_, (0x01 & (interface->buffer_[(interface->regsNum_ - 1) - i] >> (7-j)))^LED_ON);
			outputSet(interface->clk_);//clock rising edge
		}
	}
	outputSet(interface->oe_);//set OE to high
}

void lldLedSetVal(RegsChainDescriptor* interface, uint8_t num, uint8_t val)
{
	uint8_t regNum;//shift reg number
	uint8_t positionInReg;//Position if required led number inside current register
	
	uint8_t pinNum = pgm_read_byte(&((interface->ledNumTable_)[num]));
	regNum = pinNum >> 3;//replace division by shift. Get 8-bit shift register number
	positionInReg = pinNum - (regNum << 3);

	(interface->buffer_)[regNum] = ((interface->buffer_)[regNum] & ~(0x1 << positionInReg)) | (val ^ LED_ON) << positionInReg;
}
