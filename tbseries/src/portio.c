/*
 * BJ Devices Travel Box series midi controller library
 * @file	portio.c
 * 
 * @brief	Functions for working with IO ports
			  
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "portio.h"

void initOutput(const ioPort *out, uint8_t level)
{
	//set DIR register to output
	(*((out->portReg_)-1)) |= (1 << out->pin_);
	   
	if (level)
	   (*(out->portReg_)) |= (1 << out->pin_);
   else
	   (*(out->portReg_)) &= ~(1 << out->pin_);
}

void outputSet(const ioPort *out)
{
	(*(out->portReg_)) |= (1 << out->pin_);
}

void outputClear(const ioPort *out)
{
	(*(out->portReg_)) &= ~(1 << out->pin_); 
}

void outputSetToVal(const ioPort *out, uint8_t val)
{
	if(val)
		outputSet(out);
	else
		outputClear(out);
}

//void initInput(uint8_t pin, volatile uint8_t* port, uint8_t pullup)
void initInput(const ioPort *input, uint8_t pullup)
{
	//set DIR register to input
	(*((input->portReg_) - 1)) &= ~(1 << input->pin_);
	
	if (pullup)
		(*(input->portReg_)) |= (1 << input->pin_);
	else
		(*(input->portReg_)) &= ~(1 << input->pin_);
}

uint8_t inputGet(const ioPort *out)
{
	return (*((out->portReg_) - 2)) & (1 << out->pin_);
}
