/*
 * BJ Devices Travel Box series midi controller library
 * @file	TravelBoxAxeFxExample.c
 * 
 * @brief	AxeFx example. Reading IA state and preset names from AxeFx
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */ 

//common include for all library
#include "bjdevlib_tb.h"
//vendor library
#include "axefx.h"

//third-party LCD library
//you can use any other library.
#include "lcd_tb.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h> 

//define default midi channel
#define MIDI_CHANNEL 0

//define AxeFx model. See list of models in axefx.h enum AxeFxModelId
#define MY_AXEFX_MODEL AXEFX_2_XL_MODEL

//Assume we want to make 6 button in bottom row as preset change buttons(from button 1 to button 6)
//And button from 7 to button 12 is stompbox controllers
//Define preset numbers for  button 1 - 6. Any value you want ;)
const char presetNumbers[6] = {0x23, 0x14, 0x66, 0x2, 0x0, 0x5};
	
#define	PRESET_NAME_MAX_SIZE 16 //as we have 16-chars dispay, set 16 as max preset name length
const char presetNameToPrint[PRESET_NAME_MAX_SIZE] = "Name not found";//String for peint preset name. Default - "Name not found"

//last active preset button number
uint8_t presetButtonNumber = 0;
	
//Define CC number for stompboxes
//CC numbers for dedicated IA. Numbers may differ, check settings!
#define CC_DELAY        47
#define CC_REVERB       83
#define CC_CHORUS       41
#define CC_DRIVE	    49
#define CC_COMP			43
#define CC_FLANGER	    00

uint8_t stompCCNumbers[6] = {CC_DELAY, CC_REVERB, CC_CHORUS, CC_DRIVE, CC_COMP, CC_FLANGER};

//Actual value will send to midi. 
//Usually values 0..63 - bypass, value 64-127 - active effect
//All stompboxes bypass by default
#define STOMP_OFF	0x00
#define STOMP_ON	0x7F
#define STOMP_ON_MIN_VAL	0x40//if value higher or equal this value, we assume effect is enable
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
	//Usually guitar sound processors display preset numbers starting from 1, but internal number is still 0
	//Convert integer to string 
	LCDWriteIntXY(9, 0, presetNumbers[presetButtonNumber] + 1, 3);
	
	//clear bottom string
	LCDWriteStringXY(0, 1, "                ");
	//print name
	LCDWriteStringXY(0, 1, (const char*)presetNameToPrint);
}	

void processPresetSwitching(uint8_t buttonNum)
{
	//Update last active preset button number
	presetButtonNumber = buttonNum;

	//Send Program change midi message. It is usually used for preset switching
	midiSendProgramChange(presetNumbers[presetButtonNumber], MIDI_CHANNEL);
	
	//To get all LED in valid state we should send request for IA states in new preset
	//AXEFX_GET_PRESET_EFFECT_BLOCKS_AND_CC_AND_BYPASS_STATE have no additional payload, so parameter 3 is NULL
	axefxSendFunctionRequest(MY_AXEFX_MODEL, AXEFX_GET_PRESET_EFFECT_BLOCKS_AND_CC_AND_BYPASS_STATE, NULL, 0);
	
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

//implement method for IA states processing
void parseIaStates(uint8_t* sysexData)
{
	uint8_t i;
	uint8_t j;
	//Get total effect blocks in sysex message
	uint8_t totalEffectsInMessage = axefxGetEffectBlockStateNumber(sysexData);
	
	//temp variable to store single block state
	AxeFxEffectBlockState state; 
	
	for(i = 0; i < totalEffectsInMessage; ++i)
	{
		//check no error during parsing 
		if(axefxGetSingleEffectBlockState(&state, i, sysexData));
		{
			//looking for same CC numbers in internal array to set valid IA state. 
			for(j = 0; j < 6; ++j)
			{
				if(stompCCNumbers[j] == state.iaCcNumber_)//if found - set state
					stompActualValue[j] = state.isEnabled_ ? STOMP_ON : STOMP_OFF;
			}
		}
	}
}

//implement method for IA states processing
void parsePresetName(uint8_t* sysexData)
{
	axefxGetPresetName((char*)presetNameToPrint, PRESET_NAME_MAX_SIZE, sysexData);
}

//create callback for income sysex messages from axefx
void sysExCallback(uint16_t length)
{
	//get pointer to last sysex message payload data
	uint8_t* sysexData = midiGetLastSysExData();
	
	//check if SysEx from Axe Fx. If not do nothing. Also can check model ID here
	if(!axeFxCheckFractalManufId(sysexData))
		return;
	
	AxeFxFunctionId function = axeFxGetFunctionId(sysexData);
	
	switch(function)
	{
		case AXEFX_GET_PRESET_EFFECT_BLOCKS_AND_CC_AND_BYPASS_STATE :
			parseIaStates(sysexData);//parse IA state and set internal states
			axefxSendFunctionRequest(MY_AXEFX_MODEL, AXEFX_GET_PRESET_NAME, NULL, 0);//now we can send new preset name request
			updateLeds();//update LEDs with actual IA states
			break;
		
		case AXEFX_GET_PRESET_NAME :
			parsePresetName(sysexData);
			updateScreen();
			
		default: break;//in this example all others message drops
	}
}

int main(void)
{
	//Library initialization
	initBjDevLib();
	
	//third-party LCD library initialization
	//third-party LCD library initialization
	LCDInit(LS_ULINE);
	LcdHideCursor();
	
	//register midi callback for SysEx messages
	midiRegisterSysExCallback(sysExCallback);
	
	//put  "Preset # " to screen. It is a static title
	LCDWriteStringXY(0, 0, "Preset # ");
	
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
			
		midiRead();
    }
}
