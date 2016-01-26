/*
 * BJ Devices Travel Box series midi controller library
 * @file	lldled.h
 * 
 * @brief	Low level driver for LEDs via shift registers
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */


#ifndef lldled_h_
#define lldled_h_

#include "portio.h"

#include <stdint.h>

#define LED_ON  1//Active led value
#define LED_OFF !LED_ON//

/*
 * Interface descriptor.
 * Device with pedals have several shift registers chains with different chain length
 */
typedef struct RegsChainDescriptor
{
    uint8_t ledsNum_;	//physical leds num. For example one bi-colored LED have psychical two LEDs in single case
    uint8_t regsNum_;
    const ioPort* clk_;
    const ioPort* data_;
    const ioPort* oe_;		
    uint8_t* buffer_;
    uint8_t* ledNumTable_;
}RegsChainDescriptor;

/*
 * @brief	Send data to shift register 
 */
void lldLedSend(RegsChainDescriptor* interface);

/*
 * @brief	Set single LED. Data is not send to shift register
 * @param	num -	number if physical led
 * @param	val -	LED value
 */
void lldLedSetVal(RegsChainDescriptor* interface, uint8_t num, uint8_t val);


#endif /* lldled_h_ */