/*
 * BJ Devices Travel Box series midi controller library
 * @file	adc.c
 * 
 * @brief	ADC low level functions				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "adc.h"
#include <util/delay.h>
#include <avr/io.h>



void initAdc()
{
	ADMUX=ADC_VREF_TYPE & 0xff;
	ADCSRA=0x87;	
}

uint8_t adcRead8MsbBit(uint8_t chanNum)
{
	ADMUX=chanNum | (ADC_VREF_TYPE & 0xff);
	// Delay needed for the stabilization of the ADC input voltage
	_delay_us(10);
	// Start the AD conversion
	ADCSRA|=0x40;
	// Wait for the AD conversion to complete
	while ((ADCSRA & 0x10)==0);
	ADCSRA|=0x10;
	return ADCH;
}