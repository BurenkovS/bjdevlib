/*
 * BJ Devices Travel Box series midi controller library
 * @file	expression.h
 * 
 * @brief	Expression pedal functions				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#ifndef expression_h_
#define expression_h_

#include "adc.h"
#include <stdint.h>

typedef enum PedalNumber
{
	EXP_PEDAL1 = 0
	,EXP_PEDAL2
	,EXP_PEDAL_ONBOARD	
}PedalNumber;

/*
 * @brief	Initialization of expression pedals driver 
 */
void initExpression();

/*
 * @brief	Get expression pedal position. Max position is 127, min - 0
 */
uint8_t expGetPedalPosition(PedalNumber pedalNumber);

/*
 * @brief	Register callback will invoked if any pedal change position
 */
void expRegisterPedalChangePositionCallback(void (*callback)(PedalNumber pedalNumber, uint8_t position));

/*
 * @brief	Register callback will invoked if any pedal change position
 */
void expRegisterPedalChangePositionCallback(void (*callback)(PedalNumber pedalNumber, uint8_t position));


/*
 * @brief	REad all pedals positions, and invoke registered callback if any pedals was turn. Should be invoked in main loop
 */
void expProcess();

#endif /* expression_h_ */