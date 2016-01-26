/*
 * BJ Devices Travel Box series midi controller library
 * @file	timer.c
 * 
 * @brief	Provide timer ticks since last reset.
 *			one tick is approx 32.75ms				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */ 

#include "timer.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint32_t ticks;

void initTimer()
{
    TCCR0 = 0x07;
    TCNT0 = 0x00;
    OCR0 = 0x00;
	TIMSK |= (1<<0);
    ticks = 0;
}

uint32_t getTicks()
{
	uint32_t tmp;
	    
	cli();
	tmp = ticks;
	sei();

    return tmp;
}

ISR(TIMER0_OVF_vect)
{
	 ++ticks;
}