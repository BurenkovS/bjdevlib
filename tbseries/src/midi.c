/*
 * BJ Devices Travel Box series midi controller library
 * @file	midi.c
 * 
 * @brief	Send and receive midi messages				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "midi.h"
#include "uart.h"

#include <stdint.h>

static uint8_t midiBuffer[MIDI_BUFFER_SIZE];

void initMidi()
{
	initUart0AsMidi();	
}

void midiSendProgramChange(uint8_t progNum, uint8_t chanNum)
{
	uart0PutChar(PC_STATUS | (0x0F & chanNum));
	uart0PutChar(0x7F & progNum);
}

void midiSendControlChange(uint8_t ctrlNum, uint8_t val, uint8_t chanNum)
{
	uart0PutChar(CC_STATUS | (0x0F & chanNum));
	uart0PutChar(0x7F & ctrlNum);
	uart0PutChar(0x7F & val);
}

void midiSendSysEx(uint16_t length, uint8_t* data)
{
	
}

uint8_t getMessageLength(uint8_t messageType)
{
	switch(messageType)
	{
		case PC_STATUS :
		return 2;
		break;
		
		case CC_STATUS :
		return 3;
		break;
		
		case ACTIVE_SENSE :
		return 1;
		break;
		
		default:
		return 0;//return 0 if length is unknown
		break;
	}

}
static uint16_t midiInRxCnt = 0;
static uint8_t lastStatus = UNKNOWN_STATUS;
static uint16_t lastSysExLength = 0;

bool parce(uint8_t data)
{
	if(midiInRxCnt == 0)
	{
		if(!(data & 0x80))
		return false;//TODO add running status support here
		
		switch (data & 0xF0)
		{
			case PC_STATUS :
			case CC_STATUS	:
			case SYSEX_STATUS :
			midiBuffer[midiInRxCnt++] = data;
			lastStatus = data & 0xF0;
			return false;
			break;

			case ACTIVE_SENSE :
			midiBuffer[midiInRxCnt] = data;
			lastStatus = data & 0xF0;
			return true;
			break;
			
			default:
			lastStatus = UNKNOWN_STATUS;
			return false;
			break;
		}
	}
	else
	{
		if (data & 0x80) //MSB must be 0 inside the message, error if not
		{
			midiInRxCnt = 0;//start waiting new valid message
			lastStatus = UNKNOWN_STATUS;
			return false;
		}
		
		switch (lastStatus)
		{
			case PC_STATUS :
			case CC_STATUS	:
			midiBuffer[midiInRxCnt++] = data;
			if(midiInRxCnt == getMessageLength(lastStatus))//if end of message reached
			{
				midiInRxCnt = 0;
				lastStatus = UNKNOWN_STATUS;
				return true;
			}
			break;
			
			case SYSEX_STATUS :
			midiBuffer[midiInRxCnt++] = data;
			if(data == 0x7F)//End of sys ex
			{
				lastSysExLength = midiInRxCnt;
				midiInRxCnt = 0;
				lastStatus = UNKNOWN_STATUS;
				return true;
			}
			break;

			default:
			break;
		}
		return false;
	}
}


//callbacks
static void (*ccCallback)(uint8_t, uint8_t, uint8_t);
static void (*pcCallback)(uint8_t, uint8_t);
static void (*sysExCallback)(uint16_t);
static void (*activeSenseCallback)(void);

void runCallbacks() 
{
	if(ccCallback)
		(*ccCallback)(midiGetChannelNumber(), midiGetControllerNumber(), midiGetControllerValue());
		
	if(pcCallback)
		(*pcCallback)(midiGetChannelNumber(), midiGetProgramNumber());
		
	if(sysExCallback)
		(*sysExCallback)(lastSysExLength);
		
	if(activeSenseCallback)
		(*activeSenseCallback)();
}

bool midiRead()
{
	if(uart0IsBufferEmpty())
		return false;
		
	if(parce(uart0GetChar()))
	{
		runCallbacks();
		return true;
	}
	return false;
}

void midiRegisterControlChangeCallback(void (*callback)(uint8_t channel, uint8_t ccNum, uint8_t ccVal))
{
	ccCallback = callback;
}

void midiRegisterProgramChangeCallback(void (*callback)(uint8_t channel, uint8_t program))
{
	pcCallback = callback;
}

void midiRegisterSysExCallback(void (*callback)(uint16_t length))
{
	sysExCallback = callback;
}

void midiRegisterActiveSenseCallback(void (*callback)(void))
{
	activeSenseCallback = callback;
}


uint8_t midiGetChannelNumber()
{
	return (midiBuffer[0] & 0x0F);
} 

uint8_t midiGetProgramNumber()
{
	return midiBuffer[1];
}

uint8_t midiGetControllerNumber()
{
	return midiBuffer[1];
}

uint8_t midiGetControllerValue()
{
	return midiBuffer[2];
}

uint8_t midiGetSysExLength()
{
	return lastSysExLength;
}

uint8_t* midiGetSysExData()
{
	return midiBuffer;
}

uint8_t midiGetMessageType()
{
	return (midiBuffer[0] & 0xF0);
}