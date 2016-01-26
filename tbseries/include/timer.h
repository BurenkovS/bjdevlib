/*
 * BJ Devices Travel Box series midi controller library
 * @file	timer.h
 * 
 * @brief	provide timer ticks since last reset
			  
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */ 


#ifndef timer_h_
#define timer_h_

#include <stdint.h>
/*
 * @brief Timer initialization
 */
void initTimer();

/*
 * @brief get timer ticks since last reset
 */
uint32_t getTicks();

#include <stdint.h>




#endif /* timer_h_ */