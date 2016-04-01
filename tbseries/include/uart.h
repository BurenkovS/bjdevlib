/*
 * BJ Devices Travel Box series midi controller library
 * @file	uart.h
 * 
 * @brief	USART low level functions				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */


#ifndef uart_h_
#define uart_h_

#include <stdint.h>
#include <stdbool.h>

#ifndef RXB8
#define RXB8 1
#endif

#ifndef TXB8
#define TXB8 0
#endif

#ifndef UPE
#define UPE 2
#endif

#ifndef DOR
#define DOR 3
#endif

#ifndef FE
#define FE 4
#endif

#ifndef UDRE
#define UDRE 5
#endif

#ifndef RXC
#define RXC 7
#endif

#define FRAMING_ERROR		(1<<FE)
#define PARITY_ERROR		(1<<UPE)
#define DATA_OVERRUN		(1<<DOR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE			(1<<RXC)

// USART0 Receiver buffer
// User can redefine this value
#ifndef RX_BUFFER_SIZE0
#	define RX_BUFFER_SIZE0 256
#endif

/*
 * @brief	USART0 init as midi port: baud 31250, 8 data, 1 stop, no parity, async mode
 */
void initUart0AsMidi();

/*
 * @brief	Send byte to USART0
 */
void uart0PutChar(uint8_t data);

/*
 * @brief	Get char from USART0. Blocking call
 */
uint8_t uart0GetChar();

/*
 * @brief	Check if USART0 input buffer is empty
 */
bool uart0IsBufferEmpty();

/*
 * @brief	Check if USART0 input buffer is overflow
 * @param	resetOverflowFlag - reset overflow flag
 */
bool uart0IsBufferOvefflow(bool resetOverflowFlag);

/*
 * @brief	USART1 init with variable baudrate
 *			8 data, 1 stop, no parity, async mode	
 * @param	baudrate - required baudrate
 */
void initUart1(uint32_t baudrate);

/*
 * @brief	Send byte to USART1
 */
void uart1PutChar(uint8_t data);

/*
 * @brief	Send null-terminated string to USART1
 */
void uart1PutString(char* str);

/*
 * @brief	Send null-terminated string from PRG_MEM to USART1
 */
void uart1PutStringP(const char* str);

#endif /* uart_h_ */