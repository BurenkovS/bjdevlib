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
#include "lcd_lib.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

//define default midi channel
#define MIDI_CHANNEL 0

//Control change numbers for expression pedal 1 and expression pedal 2
const uint8_t pedalCCNumbers[2] = {0x1, 0x3};

//Current pedal positions
uint8_t pedalPositions[2] = {0x0, 0x0};
	
void updateScreen()
{
	uint8_t pedal1ToPrint[3] = {0x20,0x20,0x20};
	uint8_t pedal2ToPrint[3] = {0x20,0x20,0x20};
	
	itoa(pedalPositions[0], (char*)pedal1ToPrint, 10);
	itoa(pedalPositions[1], (char*)pedal2ToPrint, 10);	
	
	lcd_gotoxy(9, 0);
	//print string
	lcd_string(pedal1ToPrint, 3);

	lcd_gotoxy(9, 1);
	//print string
	lcd_string(pedal2ToPrint, 3);


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
	lcd_init();
	_delay_ms(10);
	lcd_clr();
	lcd_home();
	
	//register pedal change position callback
	//you can use callback, which will invoke only then pedals was turn or check pedal position manually,
	//compare it with previous value and process changes
	expRegisterPedalChangePositionCallback(expPedalsCallback);
	
	lcd_string((uint8_t*)"Pedal 1 : ", sizeof("Pedal 1 : "));
	lcd_gotoxy(0,1);
	lcd_string((uint8_t*)"Pedal 2 : ", sizeof("Pedal 2 : "));
	
	//manual read current pedal position
	pedalPositions[0] = expGetPedalPosition(EXP_PEDAL1);
	pedalPositions[1] = expGetPedalPosition(EXP_PEDAL2);
	
	//print current pedal positions
	updateScreen();

	while(1)
    {
		//continuously reading all pedals and run callback, if any changes detected
		expProcess();
    }
}
