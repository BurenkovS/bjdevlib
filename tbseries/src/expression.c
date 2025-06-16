/*
 * BJ Devices Travel Box series midi controller library
 * @file	expression.c
 * 
 * @brief	Expression pedal functions				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "expression.h"
#include "adc.h"
#include "pinout.h"
#include "portio.h"
#include "log.h"
#include <avr/pgmspace.h>

#define MAX_PEDALS 3

const ioPort pedalsArray[MAX_PEDALS] PROGMEM = {
	{ &EXP_P1_PORT, EXP_P1_PIN },
	{ &EXP_P2_PORT, EXP_P2_PIN },
	{ &EXP_P_OB_PORT, EXP_P_OB_PIN }};

#define EXP_P1_ADC_CHAN EXP_P1_PIN
#define EXP_P2_ADC_CHAN EXP_P2_PIN
#define EXP_P_ONBPAR_ADC_CHAN EXP_P_OB_PIN

static const uint8_t adcChanArray[MAX_PEDALS] = {EXP_P1_ADC_CHAN, EXP_P2_ADC_CHAN, EXP_P_ONBPAR_ADC_CHAN};
static uint8_t pedalsPrevValue[MAX_PEDALS] = {0, 0, 0};


//Use hysteresis function to avoid bouncing on the boundary of values
static uint8_t direction[3] = {0,0,0};
static const uint8_t threshold = 1;
static uint16_t lastVal[3] = {0,0,0};
	
uint8_t hysteresis(uint8_t input, uint8_t channel)
{
	if (direction[channel] == 0)//go down
	{
		if ((uint16_t)input > lastVal[channel] + threshold)
		{
			direction[channel] = 1;//go up
			lastVal[channel] = (uint16_t)input;
			return input;
		}
		else if((uint16_t)input < lastVal[channel])
		{
			lastVal[channel] = (uint16_t)input;
			return input;
		}
		else
		{
			return lastVal[channel];
		}
	}
	else//go up
	{
		if ((uint16_t)input < lastVal[channel] - threshold)
		{
			direction[channel] = 0;//go down
			lastVal[channel] = (uint16_t)input;
			return input;
		}
		else if(input >= (uint16_t)lastVal[channel])
		{
			lastVal[channel] = (uint16_t)input;
			return input;
		}
		else
		{
			return lastVal[channel];
		}
	}
}
	
void initExpression()
{
	uint8_t i;
	ioPort tmpPort;
	
	initAdc();
	
	for(i = 0; i < MAX_PEDALS; ++i)
	{	
		tmpPort.pin_ = pgm_read_byte(&(pedalsArray[i].pin_));
		tmpPort.portReg_ = (volatile uint8_t*)pgm_read_word(&(pedalsArray[i].portReg_));
		initInput(&tmpPort, 1);
	}
	
}

uint8_t expGetPedalPosition(PedalNumber pedalNumber)
{
	//hysteresis working with values from 0 to 255, but pedal position in range from 0 to 127
	return (hysteresis(adcRead8MsbBit(adcChanArray[pedalNumber]), pedalNumber)) >> 1;
}

//callback
static void (*posCallback)(PedalNumber, uint8_t);

void expRegisterPedalChangePositionCallback(void (*callback)(PedalNumber pedalNumber, uint8_t position))
{
	posCallback = callback;
}

void expProcess()
{
	if(!posCallback)
		return;

	uint8_t i;
	uint8_t position;
	for(i = 0; i < MAX_PEDALS; ++i)
	{
		position = expGetPedalPosition(i);
		if(position != pedalsPrevValue[i])
		{
			pedalsPrevValue[i] = position;
			(*posCallback)((PedalNumber)i, position);
		}
	}
}
