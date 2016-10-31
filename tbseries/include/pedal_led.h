/*
 * BJ Devices Travel Box series midi controller library
 * @file	pedal_led.h
 * 
 * @brief	Build-in expression pedal LEDs control functions
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */


#ifndef PEDAL_LED_H_
#define PEDAL_LED_H_

#include "pinout.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum PedalLedColor
{
		PEDAL_COLOR_NO
        ,PEDAL_COLOR_R
        ,PEDAL_COLOR_G
        ,PEDAL_COLOR_B
        ,PEDAL_COLOR_RG
        ,PEDAL_COLOR_RB
        ,PEDAL_COLOR_GB
        ,PEDAL_COLOR_RGB
} PedalLedColor;


/*High level functions*/

/*
 * Init pedal shift registers interface
 */
void initPedalLed();


/*
 * @brief	Set single LED color in internal buffer and optionally send it to shift register
 * @param	ledNum -	number if led. 
 * @param	color -		color to set. PEDAL_COLOR_NO is led off.
 * @param	send -		send data to shift resister after color set. It is recommend to set
 *						color of all LEDs before send data to avoid flickering  
 *
 * Led numbers(top view of pedal):
 *      0       8
 *      1       9
 *      2       10
 *      3       11
 *      4       12
 *      5       13
 *      6       14
 *      7       15 
 */
void ledSetPedalColor(uint8_t ledNum, PedalLedColor color, bool send);


/*
 * @brief	Set all LEDs color in internal buffer and optionally send it to shift register
 * @param	color -		color to set. PEDAL_COLOR_NO is led off.
 * @param	send -		send data to shift resister after color set. It is recommend to set
 *						color of all LEDs before send data to avoid flickering  
 */
void ledSetPedalColorAll(PedalLedColor color, bool send);



#endif /* PEDAL_LED_H_ */