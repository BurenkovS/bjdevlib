/*
 * BJ Devices Travel Box series midi controller library
 * @file	bjdevlib_tb.c
 * 
 * @brief	Common include for library
			  
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "bjdevlib_tb.h"
#include <avr/interrupt.h>

void initBjDevLib()
{

#ifdef LOG_ENABLED
	initUart1(19200);
	LOG(SEV_INFO, "Log enabled"); 
#endif
	initButtons();
	initTimer();
	initMidi();
	initLed();
	initExpression();
#if defined (TB_6P_DEVICE) || defined(TB_11P_DEVICE)
	initPedalLed();
#endif
	sei();
} 