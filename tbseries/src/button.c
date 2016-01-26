/*
 * BJ Devices Travel Box series midi controller library
 * @file	button.c
 * 
 * @brief	Process footswitch buttons and configuration keyboard buttons 
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */


#include "button.h"
#include "pinout.h"
#include "portio.h"
#include "timer.h"
#include <stdint.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#ifdef TB_12_DEVICE

const ioPort buttonsArray[FOOT_BUTTONS_NUM + CONF_BUTTONS_NUM] PROGMEM = {
	{ &KEY_1_PORT, KEY_1_PIN },
	{ &KEY_2_PORT, KEY_2_PIN },
	{ &KEY_3_PORT, KEY_3_PIN },
	{ &KEY_4_PORT, KEY_4_PIN },
	{ &KEY_5_PORT, KEY_5_PIN },
	{ &KEY_6_PORT, KEY_6_PIN },
	{ &KEY_7_PORT, KEY_7_PIN },
	{ &KEY_8_PORT, KEY_8_PIN },
	{ &KEY_9_PORT, KEY_9_PIN },
	{ &KEY_10_PORT, KEY_10_PIN },
	{ &KEY_11_PORT, KEY_11_PIN },
	{ &KEY_12_PORT, KEY_12_PIN },

	{ &KEY_INC_PORT, KEY_INC_PIN },
	{ &KEY_DEC_PORT, KEY_DEC_PIN },
	{ &KEY_UP_PORT, KEY_UP_PIN },
	{ &KEY_DOWN_PORT, KEY_DOWN_PIN },
	{ &KEY_LOAD_PORT, KEY_LOAD_PIN },
	{ &KEY_SETUP_PORT, KEY_SETUP_PIN }};

#elif defined TB_5_DEVICE
 
//port connection is differ on various models
const ioPort buttonsArray[FOOT_BUTTONS_NUM + CONF_BUTTONS_NUM] PROGMEM = {
	 { &KEY_1_PORT, KEY_1_PIN },
	 { &KEY_2_PORT, KEY_2_PIN },
	 { &KEY_3_PORT, KEY_3_PIN },
	 { &KEY_6_PORT, KEY_6_PIN },
	 { &KEY_4_PORT, KEY_4_PIN },

	 { &KEY_INC_PORT, KEY_INC_PIN },
	 { &KEY_DEC_PORT, KEY_DEC_PIN },
	 { &KEY_UP_PORT, KEY_UP_PIN },
	 { &KEY_DOWN_PORT, KEY_DOWN_PIN },
	 { &KEY_LOAD_PORT, KEY_LOAD_PIN },
	 { &KEY_SETUP_PORT, KEY_SETUP_PIN }};
	 
#endif


#define DEBOUNCE_DELAY_MS	2
#define KEY_ACTIVE			0
#define SLOW_AUTOREPEATS	3

#define SLOW_AUTO_TIME		10
#define FAST_AUTO_TIME		5
#define FIRST_AUTO_TIME		20
#define HOLD_ON_TIME		25

uint8_t getButtonState(uint8_t buttonNum)
{
    ioPort tmpPort;
	tmpPort.pin_ = pgm_read_byte(&(buttonsArray[buttonNum].pin_));
	tmpPort.portReg_ = (volatile uint8_t*)pgm_read_word(&(buttonsArray[buttonNum].portReg_));
	
	if(inputGet(&tmpPort) == KEY_ACTIVE)
	{
		_delay_ms(DEBOUNCE_DELAY_MS);
			return inputGet(&tmpPort);
	}
	
	return !KEY_ACTIVE;
}


static ButtonActionType buttonsLastAction[FOOT_BUTTONS_NUM + CONF_BUTTONS_NUM];
static uint32_t buttonsLastTime[FOOT_BUTTONS_NUM + CONF_BUTTONS_NUM];
static uint32_t autorepeatCounter[FOOT_BUTTONS_NUM + CONF_BUTTONS_NUM];

void initButtons()
{
	uint8_t i;
	ioPort tmpPort;
	for (i = 0; i < FOOT_BUTTONS_NUM + CONF_BUTTONS_NUM; ++i)
	{
		tmpPort.pin_ = pgm_read_byte(&(buttonsArray[i].pin_));
		tmpPort.portReg_ = (volatile uint8_t*)pgm_read_word(&(buttonsArray[i].portReg_));
		
		initInput(&tmpPort, 1);
		buttonsLastAction[i] = BUTTON_RELEASE; 
		buttonsLastTime[0] = 0;
		autorepeatCounter[0] = 0;
	}
}

ButtonActionType getButtonActionType(uint8_t buttonNum)
{
    uint8_t autorepeatTime = SLOW_AUTO_TIME;
	uint8_t buttonState = getButtonState(buttonNum);
    
	switch(buttonsLastAction[buttonNum])
	{
		case BUTTON_RELEASE:
			if(buttonState == KEY_ACTIVE)
			{
				buttonsLastAction[buttonNum] = BUTTON_PUSH;
				buttonsLastTime[buttonNum] = getTicks();//store push time
				return BUTTON_PUSH;
			}
			break;
			
		case BUTTON_PUSH:
		    if(buttonState != KEY_ACTIVE)
		    {
			    buttonsLastAction[buttonNum] = BUTTON_RELEASE;
			    return BUTTON_RELEASE;
		    }
		        
		    //if button hold on
		    if(getTicks() > (buttonsLastTime[buttonNum] + HOLD_ON_TIME))
		    {
			    buttonsLastTime[buttonNum] = getTicks();//renew time
			    buttonsLastAction[buttonNum] = BUTTON_HOLDON;
			    return BUTTON_HOLDON;
		    }
			break;
		
		case BUTTON_HOLDON:
			if(buttonState != KEY_ACTIVE)
			{
                buttonsLastAction[buttonNum] = BUTTON_RELEASE;
                return BUTTON_RELEASE;							
			}
			
			if(getTicks() > (buttonsLastTime[buttonNum] + FIRST_AUTO_TIME))
			{
				buttonsLastTime[buttonNum] = getTicks();//renew time
				buttonsLastAction[buttonNum] = BUTTON_REPAEATED_PUSH;
				return BUTTON_REPAEATED_PUSH;				
			}
			break;
		
		case BUTTON_REPAEATED_PUSH:
			if(autorepeatCounter[buttonNum] < SLOW_AUTOREPEATS)//slow autorepeat
				autorepeatTime = SLOW_AUTO_TIME;
			else
				autorepeatTime = FAST_AUTO_TIME;
			            
			if(buttonState != KEY_ACTIVE)
			{
				buttonsLastAction[buttonNum] = BUTTON_RELEASE;
				return BUTTON_RELEASE;
			}
						
			else if(getTicks() > (buttonsLastTime[buttonNum] + autorepeatTime))
			{
				buttonsLastTime[buttonNum] = getTicks();//renew time
				if(autorepeatCounter[buttonNum] < SLOW_AUTOREPEATS)
					autorepeatCounter[buttonNum] += 1;
					
				return BUTTON_REPAEATED_PUSH;
			}
			break;
			
		default: break;
	}
	return BUTTON_NO_EVENT;
}


ButtonEvent getButtonLastEvent()
{
    uint8_t i;
    ButtonEvent buttonEvent;
    buttonEvent.actionType_ = BUTTON_NO_EVENT;
    buttonEvent.buttonNum_ = 0;

    for(i = 0; i < FOOT_BUTTONS_NUM + CONF_BUTTONS_NUM; ++i)
    {
	    buttonEvent.actionType_ = getButtonActionType(i);
	    if(buttonEvent.actionType_ != BUTTON_NO_EVENT)
	    {
		    buttonEvent.buttonNum_ = i;
		    break;
	    }
    }
    return  buttonEvent;	
}