/*
 * BJ Devices Travel Box series midi controller library
 * @file	uart.c
 * 
 * @brief	USART low level functions				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "uart.h"

#include <avr/interrupt.h>

static uint8_t rxBuffer0[RX_BUFFER_SIZE0];

#if RX_BUFFER_SIZE0 <= 256
static uint8_t rxWrIndex0;
static uint8_t rxRdIndex0;
static uint8_t rxCounter0;

#else
static uint16_t rxWrIndex0;
static uint16_t rxRdIndex0;
static uint16_t rxCounter0;

#endif // RX_BUFFER_SIZE0 <= 256

// This flag is set on USART0 Receiver buffer overflow
static bool rxBufferOverflow0;

// USART0 Receiver interrupt service routine
ISR(USART0_RX_vect)
{
	uint8_t status = UCSR0A;
	uint8_t data = UDR0;
	
	if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
	{
		rxBuffer0[rxWrIndex0++]=data;
		
		#if RX_BUFFER_SIZE0 == 256
		// special case for receiver buffer size=256
		if (++rxCounter0 == 0)
			rxBufferOverflow0 = true;
		
		#else
		if (rxWrIndex0 == RX_BUFFER_SIZE0)
			rxWrIndex0 = 0;
			
		if (++rxCounter0 == RX_BUFFER_SIZE0)
		{
			rxCounter0 = 0;
			rxBufferOverflow0 = true;
		}
		#endif //RX_BUFFER_SIZE0 == 256
	}
}

void initUart0AsMidi()
{
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART0 Receiver: On
	// USART0 Transmitter: On
	// USART0 Mode: Asynchronous
	// USART0 Baud Rate: 31250
	UCSR0A=0x00;
	UCSR0B=0x98;
	UCSR0C=0x06;
	UBRR0H=0x00;
	UBRR0L=0x0F;
}

void uart0PutChar(uint8_t data)
{
	while ((UCSR0A & DATA_REGISTER_EMPTY) == 0);
	UDR0 = data;
}

uint8_t uart0GetChar()
{	
	uint8_t data;
	while (rxCounter0==0);
	data = rxBuffer0[rxRdIndex0++];
	
	#if RX_BUFFER_SIZE0 != 256
	if (rxRdIndex0 == RX_BUFFER_SIZE0) 
		rxRdIndex0=0;
	#endif
	
	cli();
	--rxCounter0;
	sei();
	return data;
}

bool uart0IsBufferEmpty()
{
	return (rxCounter0 == 0);
}

bool uart0IsBufferOvefflow(bool resetOverflowFlag)
{
	bool ret = rxBufferOverflow0;
	
	if(resetOverflowFlag)
		rxBufferOverflow0 = false;
		
	return ret;
}