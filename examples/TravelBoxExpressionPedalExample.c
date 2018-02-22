/*
 * BJ Devices Travel Box series midi controller library
 * @file	TravelBoxExpressionPedalExample.c
 * 
 * @brief	Expression pedals example. Read pedal position and send CC message. Position will display on LCD				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */ 


#include "bjdevlib_tb.h"

//third-party LCD library
//you can use any other library.
#include "lcd_tb.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

//define default midi channel
#define MIDI_CHANNEL 0

//Control change numbers for expression pedal 1 and expression pedal 2
const uint8_t pedalCCNumbers[3] = {0x1, 0x3, 0x7};

//Current pedal positions
uint8_t pedalPositions[3] = {0x0, 0x0, 0x0};
	
void updateScreen()
{
	LCDWriteIntXY(3, 0, pedalPositions[0], 3);
	LCDWriteIntXY(11, 0, pedalPositions[1], 3);
	LCDWriteIntXY(3, 1, pedalPositions[2], 3);
}	

void expPedalsCallback(PedalNumber pedalNumber, uint8_t pedalPosition)
{
	//send control change message
	midiSendControlChange(pedalCCNumbers[(uint8_t)pedalNumber], pedalPosition, MIDI_CHANNEL);
	
	//update local variable to print current value
	pedalPositions[(uint8_t)pedalNumber] = pedalPosition;
	updateScreen();
}

int main(void)
{
	//Library initialization
	initBjDevLib();
	
	//third-party LCD library initialization
	//third-party LCD library initialization
	LCDInit(LS_ULINE);
	LcdHideCursor();
	
	//register pedal change position callback
	//you can use callback, which will invoke only then pedals was turn or check pedal position manually,
	//compare it with previous value and process changes
	expRegisterPedalChangePositionCallback(expPedalsCallback);
	
	LCDWriteStringXY(0, 0,"P1: ");
	LCDWriteStringXY(8, 0,"P2: ");
	LCDWriteStringXY(0, 1,"P3: ");
	
	//manual read current pedal position
	pedalPositions[0] = expGetPedalPosition(EXP_PEDAL1);
	pedalPositions[1] = expGetPedalPosition(EXP_PEDAL2);
	pedalPositions[3] = expGetPedalPosition(EXP_PEDAL_ONBOARD);
	
	//print current pedal positions
	updateScreen();

	while(1)
    {
		//continuously reading all pedals and run callback, if any changes detected
		expProcess();
    }
}
