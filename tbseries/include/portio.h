/*
 * BJ Devices Travel Box series midi controller library
 * @file	portio.h
 * 
 * @brief	functions for working with IO ports
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */ 

#ifndef portio_h_
#define portio_h_

#include <stdint.h>


/*
 * I/O port descriptor
 */
typedef struct ioPort{
	volatile uint8_t* portReg_;
	uint8_t pin_;
}ioPort;

/*
 * @brief	Init port as output
 * @param	out	- pointer to I/O port descriptor which will assign to phy pin
 * @param	level - default level
 */
void initOutput(const ioPort *out, uint8_t level);

/*
 * @brief	Set port to high
 */
void outputSet(const ioPort *out);

/*
 * @brief	Set port to low
 */
void outputClear(const ioPort *out);

/*
 * @brief	Set port to value
 */
void outputSetToVal(const ioPort *out, uint8_t val);

/*
 * @brief	Init port as input
 * @param	input	pointer to I/O port descriptor which will assign to phy pin
 * @param	pullup	pullup on/off
 */
void initInput(const ioPort *input, uint8_t pullup);

/*
 * @brief	Get value from input port
 */
uint8_t inputGet(const ioPort *out);
//uint8_t inputGet(int8_t pin, volatile uint8_t* port);

#endif /* portio_h_ */
