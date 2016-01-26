/*
 * BJ Devices Travel Box series midi controller library
 * @file	button.h
 * 
 * @brief	Process footswitch buttons and configuration keyboard buttons 
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2015
 */ 


#ifndef button_h_
#define button_h_

#include <stdint.h>

typedef enum ButtonActionType//Action type
{
	BUTTON_NO_EVENT = 0,
	BUTTON_PUSH,			//push button
	BUTTON_RELEASE,			//release button
	BUTTON_HOLDON,			//button holds during some time
	BUTTON_REPAEATED_PUSH,	//repeated button push event
}ButtonActionType;

typedef struct
{
	ButtonActionType actionType_;
	uint8_t buttonNum_;				//Button numbers starts from 0 value. Button 0 is marked as "1" on device panel  
}ButtonEvent;

/*
 * @brief	Buttons initialization
 */
void initButtons();


/*
 * @brief	Return last button event - button number and action type
 */
ButtonEvent getButtonLastEvent();

#endif /* button_h_ */