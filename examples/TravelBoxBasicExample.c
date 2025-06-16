/*
 * BJ Devices Travel Box series midi controller library
 * @file	TravelBoxBasicExample.c
 * 
 * @brief	Library examples				
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

//Assume we want to make 6 button in bottom row as preset change buttons(from button 1 to button 6)
//And button from 7 to button 12 is stompbox controllers
//Define preset numbers for  button 1 - 6. Any value you want ;)
uint8_t presetNumbers[6] = {0x23, 0x14, 0x66, 0x2, 0x0, 0x5};

//last active preset button number
uint8_t presetButtonNumber = 0;
	
//Define CC number for stompboxes
//For example using default values from Kemper Profiler manual
//CC numbers for dedicated IA
#define CC_STOMP_A      17 //Stomp A
#define CC_STOMP_B      18 //Stomp B
#define CC_STOMP_C      19 //Stomp C
#define CC_STOMP_D      20 //Stomp D
#define CC_DELAY        26 //Delay
#define CC_REVERB       28 //Reverb

uint8_t stompCCNumbers[6] = {CC_STOMP_A,CC_STOMP_B, CC_STOMP_C, CC_STOMP_D, CC_DELAY, CC_REVERB};

//Actual value will send to midi. 
//Usually values 0..63 - bypass, value 64-127 - active effect
//All stompboxes bypass by default
#define STOMP_OFF	0x00
#define STOMP_ON	0x7F
#define STOMP_ON_MIN_VAL	0x40
uint8_t stompActualValue[6] = {STOMP_OFF, STOMP_OFF, STOMP_OFF, STOMP_OFF, STOMP_OFF, STOMP_OFF};

void updateLeds()
{
		uint8_t i;
		//first clear all LEDS.
		//It is recommend to send data to leds
		//only after all changes will be done
		//to avoid flickering. Or 
		//parameter "send" of lEDs function is response for this
		ledSetColorAll(COLOR_BLACK, false);
		
		//Green led show stompbox state
		for (i = 0; i < sizeof(stompCCNumbers); ++i)
		{
			if(stompActualValue[i] >= STOMP_ON_MIN_VAL)
				ledSetColor(i + 6, COLOR_GREEN, false);//Active stomp is green led. Stomps leds numbers starts from 6
		}
		
		//Active preset is red led
		ledSetColor(presetButtonNumber, COLOR_RED, true);//now all changes done and we can send prepared data to leds
}

void updateScreen()
{
	uint8_t presetToPrint[3] = {0x20,0x20,0x20};
	
	//Usually guitar sound processors display preset numbers starting from 1, but internal number is still 0
	//Convert integer to string 
	itoa(presetNumbers[presetButtonNumber] + 1, (char*)presetToPrint, 10);
	
	//set cursor to preset number start position
	lcd_gotoxy(9, 0);
	
	//print string
	lcd_string(presetToPrint, sizeof(presetToPrint));
}	

void processPresetSwitching(uint8_t buttonNum)
{
	//Update last active preset button number
	presetButtonNumber = buttonNum;

	//Send Program change midi message. It is usually used for preset switching
	midiSendProgramChange(presetNumbers[presetButtonNumber], MIDI_CHANNEL);
	
	//Update LEDs according new state of preset
	updateLeds();
	
	//Update screen according new state of preset
	updateScreen();
}

void processStompboxSwitching(uint8_t buttonNum)
{
	//invert stompbox state
	if(stompActualValue[buttonNum] < STOMP_ON_MIN_VAL)
		stompActualValue[buttonNum] = STOMP_ON;
	else
		stompActualValue[buttonNum] = STOMP_OFF;
	
	//Send Control change midi message. It is usually used for fxs switching
	midiSendControlChange(stompCCNumbers[buttonNum], stompActualValue[buttonNum], MIDI_CHANNEL);
	
	//Update LEDs according new state of preset
	updateLeds();
}


//buttons process function example
void processButtonEvent(ButtonEvent buttonEvent) 
{
	//Preset switching performed only on button 1 - button 6
	//and only on BUTTON_PUSH event.
	if(buttonEvent.buttonNum_ < 6 && buttonEvent.actionType_ == BUTTON_PUSH)
		processPresetSwitching(buttonEvent.buttonNum_);
		
	//Stomps switching performed only on button 7 - button 12. 
	//Button 13-18 is configuration keyboard buttons, must be filtered out
	//Also response only on BUTTON_PUSH event.
	else if(buttonEvent.buttonNum_ >= 6 && buttonEvent.buttonNum_ <= 11 && buttonEvent.actionType_ == BUTTON_PUSH)
		processStompboxSwitching(buttonEvent.buttonNum_ - 6);
	
}

//example of midi callback - Control Change message handler
void controlChangeHandler(uint8_t channel, uint8_t ccNum, uint8_t ccVal)
{
	uint8_t i;
	if(channel != MIDI_CHANNEL)
		return;
		
	for(i = 0; i < 6; ++i)
	{
		if(stompCCNumbers[i] == ccNum)//if ccNum is present in buttons setting
			stompActualValue[i] = ccVal;//change stompbox state		
	}
	updateLeds();//and update leds
}

//Program change handler
void programChangeHandler(uint8_t prgNum)
{
	uint8_t i;
	
	for(i = 0; i < 6; ++i)
	{
		if(presetNumbers[i] == prgNum)//if such preset is assign to button switch corresponding led on
		{
			presetButtonNumber = i;
			break;
		}
	}
	//Update LEDs according new state of preset
	updateLeds();
		
	//Update screen according new state of preset
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
	
	//register midi callback
	//you can use callbacks for receiving message or parse it manually
	//In this case we use callback for control change and manual parsing for program change(see main())
	midiRegisterControlChangeCallback(controlChangeHandler);
	
	//put preset number to screen
	lcd_string((uint8_t*)"Preset # ", sizeof("Preset # "));
	
	updateLeds();
	updateScreen();

	//Create variable to store buttons events
	ButtonEvent lastButtonEvent;
	
	while(1)
    {
		//check buttons
		lastButtonEvent = getButtonLastEvent();
		//BUTTON_NO_EVENT if no any buttons action, so do nothing in this case, else process event
		if(lastButtonEvent.actionType_ != BUTTON_NO_EVENT)
			processButtonEvent(lastButtonEvent);
			
		//check midi
		//manual pacing example. We will parse program change message here
		//midiRead() return true if any valid message received
		//if you want using only callbacks for input messages handling,
		//you should invoke midiRead() in main cycle but don't need to check return value
		if(midiRead())
		{
			//check if message is "program change" and it received on expected midi channel
			if(midiGetMessageType() == PC_STATUS && midiGetChannelNumber() == MIDI_CHANNEL)
				programChangeHandler(midiGetProgramNumber());
		} 
    }
}
