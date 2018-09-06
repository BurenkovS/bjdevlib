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
	uint32_t i;
	
	uart0PutChar(SYSEX_STATUS);
	
	for (i = 0; i < length; ++i)
	{
		uart0PutChar(*(data + i));	
	}
	
	uart0PutChar(0xF7);
}

void midiSendSysExManfId(uint32_t manfId, uint16_t length, uint8_t* data)
{
	uint32_t i;
	
	uart0PutChar(SYSEX_STATUS);
	
	uart0PutChar((manfId >> 16) & 0x7F);
	uart0PutChar((manfId >> 8) & 0x7F);
	uart0PutChar(manfId & 0x7F);
	
	for (i = 0; i < length; ++i)
	{
		uart0PutChar(*(data + i));
	}
	uart0PutChar(0xF7);
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

bool parse(uint8_t data)
{
	if(midiInRxCnt == 0)
	{
		if(!(data & 0x80))
			return false;//TODO add running status support here
		
		uint8_t statusWoChannel = (data < 0xF0) ? (data & 0xF0) : data;//Save all byte in case of realtime message
		switch (statusWoChannel)
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
				lastStatus = data;
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
		//MSB must be 0 inside the message if it is not end of SysEx message
		if ((data & 0x80) && (data != SYSEX_END))
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
					//lastStatus = UNKNOWN_STATUS;
					return true;
				}
			break;
			
			case SYSEX_STATUS :
				midiBuffer[midiInRxCnt++] = data;
				if(data == SYSEX_END)//End of sys ex
				{
					lastSysExLength = midiInRxCnt;
					midiInRxCnt = 0;
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
	if(ccCallback && lastStatus == CC_STATUS)
		(*ccCallback)(midiGetChannelNumber(), midiGetControllerNumber(), midiGetControllerValue());
		
	if(pcCallback && lastStatus == PC_STATUS)
		(*pcCallback)(midiGetChannelNumber(), midiGetProgramNumber());
		
	if(sysExCallback && lastStatus == SYSEX_STATUS)
		(*sysExCallback)(lastSysExLength);
		
	if(activeSenseCallback && lastStatus == ACTIVE_SENSE)
		(*activeSenseCallback)();
}

bool midiRead()
{
	while(!uart0IsBufferEmpty())
	{
		if(parse(uart0GetChar()))
		{
			runCallbacks();
			return true;
		}
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

uint16_t midiGetSysExLength(uint8_t* sysEx)
{
	uint16_t length = 0;
	
	while(*(sysEx + length) != SYSEX_END)
		++length;
		
	return length;
}

uint16_t midiGetLastSysExLength()
{
	return lastSysExLength;
}

uint8_t* midiGetLastSysExData()
{
	return midiBuffer;
}

uint8_t midiGetMessageType()
{
	return (midiBuffer[0] & 0xF0);
}

uint32_t midiGetSysExManufacturerId(uint8_t* sysEx)
{
	uint32_t ret = 0;
	ret = ((uint32_t)sysEx[1] << 16) | ((uint32_t)sysEx[2] << 8) | sysEx[3]; 
	return ret;
}