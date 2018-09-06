/*
 * BJ Devices Travel Box series midi controller library
 * @file	TravelBoxKpaExample.c
 * 
 * @brief	KPA example. Reading IA state and rig names from Kemper Profiling Amplifier
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

//common include for all library
#include "bjdevlib_tb.h"
//vendor library
#include "kpa.h"

//third-party LCD library
//you can use any other library.
#include "lcd_tb.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

//define default midi channel
#define MIDI_CHANNEL 0

//Assume we want to make 6 button in bottom row as preset change buttons(from button 1 to button 6)
//And button from 7 to button 12 is stompbox controllers
//Define preset numbers for  button 1 - 6. Any value you want ;)
const char presetNumbers[6] = {0x23, 0x14, 0x66, 0x2, 0x0, 0x5};

//last active preset button number
uint8_t presetButtonNumber = 0;

//Kpa connection status
bool kpaConnected = false;
//Kpa state
uint16_t kpaMode = 0;//Kemper main mode.  (0=BROWSE/1=PERFORM)

//Kpa rig and perfomance name
#define	NAME_MAX_SIZE 16 //as we have 16-chars display, set 16 as max preset name length
char kpaRigName[NAME_MAX_SIZE] = "No rig";
char kpaPerformanceName[NAME_MAX_SIZE] = "No perfom";

//Define parameter numbers for which are response for stompboxes ON/OFF state
KpaParamAddress stompParamNumbers[6] = {KPA_PARAM_STOMP_A_ON_OFF
										,KPA_PARAM_STOMP_B_ON_OFF
										,KPA_PARAM_STOMP_C_ON_OFF
										,KPA_PARAM_STOMP_D_ON_OFF
										,KPA_PARAM_REVERB_ON_OFF_CUTS_TAIL
										,KPA_PARAM_DELAY_ON_OFF_CUTS_TAIL};

//In KPA parameter value 0 correspond to stomp off, 1 -  stomp on
//parameter takes two bytes 
#define STOMP_OFF	0x0000
#define STOMP_ON	0x0001

uint16_t stompActualValue[6] = {STOMP_OFF, STOMP_OFF, STOMP_OFF, STOMP_OFF, STOMP_OFF, STOMP_OFF};

void updateLeds()
{
	uint8_t i;
	//first clear all LEDS.
	//It is recommend to send data to LEDs only after all changes will be done to avoid flickering.
	//parameter "send" of LEDs function is response for this
	ledSetColorAll(COLOR_BLACK, false);
	
	//Green led show stompbox active state
	for (i = 0; i < sizeof(stompParamNumbers)/sizeof(stompParamNumbers[0]); ++i)
	{
		if(stompActualValue[i] == STOMP_ON)
			ledSetColor(i + 6, COLOR_GREEN, false);//Active stomp is green led. Stomps leds numbers starts from 6
	}
	
	//Active preset is red led
	ledSetColor(presetButtonNumber, COLOR_RED, true);//now all changes done and we can send prepared data to leds
}

void updateScreen()
{

	//Usually guitar sound processors display preset numbers starting from 1, but internal number is still 0
	//Print number
	LCDWriteIntXY(9, 0, presetNumbers[presetButtonNumber] + 1, 3);
	
	//clear bottom string
	LCDWriteStringXY(0, 1, "                ");
	//print string
	if(kpaMode == 0)
		//Browse mode. Show rig name
		LCDWriteStringXY(0, 1, (const char*)kpaRigName);
	else
		//Performance mode. Show performance name
		LCDWriteStringXY(0, 1, (const char*)kpaPerformanceName);
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
	if(stompActualValue[buttonNum] == STOMP_OFF)
		stompActualValue[buttonNum] = STOMP_ON;
	else
		stompActualValue[buttonNum] = STOMP_OFF;

	//Send message to change stomp state
	kpaSendSingleParameterChange(stompParamNumbers[buttonNum], (uint16_t)stompActualValue[buttonNum]);
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

//implement helper for IA states processing
void parseIaStates(KpaParamAddress addr, uint8_t* sysexData)
{
	uint8_t i;
	for(i = 0; i < 6; ++i)
	{
		if(addr == stompParamNumbers[i])//find IA address in array
			stompActualValue[i] = kpaGetSingleParameterValue(sysexData);//set corresponding IA to valid state
	}
	updateLeds();
}

//create callback for income sysex messages from KPA
void sysExCallback(uint16_t length)
{
	uint8_t* sysEx;//pointer to SysEx payload data
	sysEx = midiGetLastSysExData();
	//get function code
	uint8_t function = kpaGetFunction(sysEx);
	
	//address from message
	KpaParamAddress addr;
	switch(function)
	{
		case KPA_FUNCTION_SINGLE_PARAMETER_CHANGE : //KPA sends IA states and main mode as single parameter change
			addr = kpaGetParamAddress(sysEx);
			switch (addr)
			{
				case KPA_PARAM_SELECTED_MAIN_MODE : //parse main mode 
					kpaMode = kpaGetSingleParameterValue(sysEx);
					updateScreen();
					break;
				
				case KPA_PARAM_STOMP_A_ON_OFF : //parse all stomboxes state	
				case KPA_PARAM_STOMP_B_ON_OFF :
				case KPA_PARAM_STOMP_C_ON_OFF :
				case KPA_PARAM_STOMP_D_ON_OFF :
				case KPA_PARAM_REVERB_ON_OFF_CUTS_TAIL :
				case KPA_PARAM_DELAY_ON_OFF_CUTS_TAIL :
					parseIaStates(addr, sysEx);
				break;
				
				default: break;
			}
		break;
		
		case KPA_FUNCTION_STRING_PARAMETER_CHANGE : //Rig Name passed as STRING_PARAMETER_CHANGE
			if(kpaGetParamAddress(sysEx) == KPA_PARAM_RIG_NAME)
			{
				kpaGetStringParameter(kpaRigName, NAME_MAX_SIZE, sysEx);//Fill rig name. We don't need more than 16 chars due the display limitation
				LOG(SEV_TRACE, "Rig name :%s", kpaRigName); 
				updateScreen();
			}
		break;
		
		case KPA_FUNCTION_EXTENDED_STRING_PARAMETER_CHANGE : //Performance Name passed as EXTENDED_STRING_PARAMETER_CHANGE
			if(kpaGetParamExtAddress(sysEx) == KPA_PARAM_EXT_PERFORMANCE_NAME)
			{
				kpaGetExtStringParameter(kpaPerformanceName, NAME_MAX_SIZE, sysEx);//Fill performance name. We don't need more than 16 chars due the display limitation
				LOG(SEV_TRACE, "Perf name :%s", kpaPerformanceName); 
				updateScreen();
			}
		break;
	}
}

//connection initialization
void initConnectionToKpa()
{
	if(kpaConnected)
		return;
	LCDWriteStringXY(0, 1, "KPA Connected");
	_delay_ms(500);
	kpaConnected = true;
	//If you want to KPA show a popup with the your floorboard name when the first beacon message received
	//you should send string change request with KPA_PARAM_FLOORBOARD_NAME address and board name
	kpaSendStringParameterChange(KPA_PARAM_FLOORBOARD_NAME, "My first midi board");
	
	_delay_ms(100);
	//Then "beacon" message should be send to KPA to enable sending changes.
	//For details see kpaSendBeacon(...) function description in kpa.h
	
	//First prepare flags. See flags description in kpa.h
	uint8_t beaconFlags = KPA_BEACON_FLAG_INIT
						| KPA_BEACON_FLAG_SYSEX //library works only with SysEx
						//In this simple example we will not process communication issue,
						//so we disable to send FE ans sensing by set KPA_BEACON_FLAG_NOFE and KPA_BEACON_FLAG_NOCTR.
						//But it is not good practice in real life.
						//In real life you should to keep track of the connection is good
						//by monitoring "Sensing" message which comes about every 500ms
						//as long as the time lease is valid
						| KPA_BEACON_FLAG_NOFE
						| KPA_BEACON_FLAG_NOCTR
						| KPA_BEACON_FLAG_TUNEMODE;//tuner will send only in tuner mode
	
	kpaSendBeacon(2			//Set number. I want to use SET2, its contain performance and rig names inter alia.
				,beaconFlags//Flags
				,127);		//Send changes forever. Possible it is overhead, but it is a simplest way to communication, no need to resend beacon
	LOG(SEV_TRACE, "Send beacon"); 
	
	updateScreen();
}

const char stringSev[] PROGMEM = "TRACE: ";

int main(void)
{
	//Library initialization
	initBjDevLib();
	
	//third-party LCD library initialization
	LCDInit(LS_ULINE);
	LcdHideCursor();
	
	//KPA send MIDI Active Sensing real time message 0xFE.
	//As soon as we can see it, we able to establish bi-directorial connection with KPA
	midiRegisterActiveSenseCallback(initConnectionToKpa);
	
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