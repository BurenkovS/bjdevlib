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
	initButtons();
	initTimer();
	initMidi();
	initLed();
	sei();
} 