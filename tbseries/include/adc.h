/*
 * BJ Devices Travel Box series midi controller library
 * @file	adc.h
 * 
 * @brief	ADC low level functions				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#ifndef adc_h_
#define adc_h_

#include <stdint.h>

#define ADC_VREF_TYPE 0x60

/*
 * @brief	Default ADC initialization. Voltage reference is VCC, clock 62,5 KHz
 */
void initAdc();

/*
 * @brief	Read the 8 most significant bits of the AD conversion result
 */
uint8_t adcRead8MsbBit(uint8_t chanNum);



#endif /* adc_h_ */