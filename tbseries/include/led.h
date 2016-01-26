/*
 * BJ Devices Travel Box series midi controller library
 * @file	led.h
 * 
 * @brief	Control bi-colored LEDs near the foot buttons
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */ 


#ifndef led_h_
#define led_h_

#include "pinout.h"
#include "portio.h"

#include <stdint.h>
#include <stdbool.h>


typedef enum LedColor
{
	COLOR_BLACK = 0	//all LEDs off
	,COLOR_RED
	,COLOR_GREEN
	,COLOR_YELLOW	//mix red and yellow
}LedColor;

/*
 * @brief	LEDs initialization
 */
void initLed();

/*
 * @brief	Set single LED color in internal buffer and optionally send it to shift register
 * @param	ledNum -	number if led. Corresponds nearest button number.
 *						LEDs numbers starts from 0 value. LED 0 is marked as "1" on device panel
 * @param	color -		color to set. COLOR_BLACK is led off, COLOR_YELLOW is mix red and yellow.
 * @param	send -		send data to shift resister after color set. It is recommend to set
 *						color of all LEDs before send data to avoid flickering  
 */
void ledSetColor(uint8_t ledNum, LedColor color, bool send);

/*
 * @brief	Set all LEDs color in internal buffer and optionally send it to shift register
 * @param	color -		color to set. COLOR_BLACK is led off, COLOR_YELLOW is mix red and yellow.
 * @param	send -		send data to shift resister after color set. It is recommend to set
 *						color of all LEDs before send data to avoid flickering  
 */
void ledSetColorAll(LedColor color, bool send);

/*
 * @brief	Send data from internal buffer to shift register
 */
void ledSend();


#endif /* led_h_ */