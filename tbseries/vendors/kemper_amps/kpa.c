/*
 * BJ Devices Travel Box series midi controller library
 * @file	kpa.c
 * 
 * @brief	Kemper Profiling Amplifier communication functions
			  
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "kpa.h"
#include "midi.h"
#include <avr/pgmspace.h>
#include <string.h>

//fill first 6 bytes of message
static void fillMessageCommonPart(uint8_t* messageToSend, uint8_t function, KpaParamAddress controllerAddr)
{
		messageToSend[0] = (uint8_t)KPA_PRODUCT_TYPE;
		messageToSend[1] = (uint8_t)KPA_DEVICE_ID_OMNI;

		//function
		messageToSend[2] = (uint8_t)function;
		messageToSend[3] = (uint8_t)KPA_PARAMETER_INSTANCE;
		
		//address
		messageToSend[4] = (uint8_t)(controllerAddr >> 8);
		messageToSend[5] = (uint8_t)controllerAddr;
}

//fill first 9 bytes of extended message
static void fillExtMessageCommonPart(uint8_t* messageToSend, uint8_t function, KpaParamExtAddress controllerAddr)
{
	messageToSend[0] = (uint8_t)KPA_PRODUCT_TYPE;
	messageToSend[1] = (uint8_t)KPA_DEVICE_ID_OMNI;

	//function
	messageToSend[2] = (uint8_t)function;
	messageToSend[3] = (uint8_t)KPA_PARAMETER_INSTANCE;
	
	//address
	messageToSend[4] = (uint8_t)(controllerAddr >> 32);
	messageToSend[5] = (uint8_t)(controllerAddr >> 24);
	messageToSend[6] = (uint8_t)(controllerAddr >> 16);
	messageToSend[7] = (uint8_t)(controllerAddr >> 8);
	messageToSend[8] = (uint8_t)controllerAddr;
}

static const uint8_t singleParamChangeMessSize PROGMEM = 8;
void kpaSendSingleParameterChange(KpaParamAddress controllerAddr, uint16_t value)
{
	size_t baseMessLength = pgm_read_byte(&singleParamChangeMessSize);
	uint8_t messageToSend[baseMessLength];

	//fill first 6 bytes
	fillMessageCommonPart(messageToSend, KPA_FUNCTION_SINGLE_PARAMETER_CHANGE, controllerAddr);
	//value
	messageToSend[6] = ((uint8_t)(value >> 7)) & 0x7F;
	messageToSend[7] = ((uint8_t)value) & 0x7F;
		
	midiSendSysExManfId((uint32_t)KEMPER_AMPS_MANF_ID, baseMessLength, messageToSend);
}

static const uint8_t multiParamChangeMessSize PROGMEM = 6;
void kpaSendMultiParameterChange(KpaParamAddress controllerAddr, uint16_t* value, uint8_t num)
{
	size_t baseMessLength = pgm_read_byte(&multiParamChangeMessSize);
	uint8_t messageToSend[baseMessLength + num*sizeof(uint16_t)];
	uint8_t i;
	uint16_t tmpValue;
	fillMessageCommonPart(messageToSend, KPA_FUNCTION_MULTI_PARAMETER_CHANGE, controllerAddr);
	
	//value
	for(i = 0; i < num; ++i)
	{
		tmpValue = *(value + i);
		messageToSend[baseMessLength + i*sizeof(uint16_t)] = ((uint8_t)(tmpValue >> 7)) & 0x7F;
		messageToSend[baseMessLength + 1 + i*sizeof(uint16_t)] = ((uint8_t)tmpValue) & 0x7F;
	}
	
	midiSendSysExManfId((uint32_t)KEMPER_AMPS_MANF_ID, pgm_read_byte(&multiParamChangeMessSize) + num*sizeof(uint16_t), messageToSend);	
}

static const uint8_t stringParamChangeMessSize PROGMEM = 6;
void kpaSendStringParameterChange(KpaParamAddress controllerAddr, const char* str)
{
	size_t strLength = strlen(str);
	size_t baseMessLength = pgm_read_byte(&stringParamChangeMessSize);
	uint8_t messageToSend[baseMessLength + strLength + 1];

	fillMessageCommonPart(messageToSend, KPA_FUNCTION_STRING_PARAMETER_CHANGE, controllerAddr);

	memcpy(messageToSend + baseMessLength, str, strLength);
	messageToSend[baseMessLength + strLength] = 0;
	midiSendSysExManfId((uint32_t)KEMPER_AMPS_MANF_ID, baseMessLength + strLength + 1, messageToSend);
}

static const uint8_t blobParamChangeMessSize PROGMEM = 10;
void kpaSendBLOBChange(KpaParamAddress controllerAddr, uint16_t startOffset, uint8_t* content, uint16_t size)
{
	const size_t baseMessLength = pgm_read_byte(&blobParamChangeMessSize);
	uint8_t messageToSend[baseMessLength + size];
	
	fillMessageCommonPart(messageToSend, KPA_FUNCTION_BLOB, controllerAddr);

	//offset
	messageToSend[6] = ((uint8_t)(startOffset >> 7)) & 0x7F;
	messageToSend[7] = ((uint8_t)startOffset) & 0x7F;
		
	//size
	messageToSend[8] = ((uint8_t)(size >> 7)) & 0x7F;
	messageToSend[9] = ((uint8_t)size) & 0x7F;
	
	//data
	memcpy(messageToSend + baseMessLength, content, size);
	midiSendSysExManfId((uint32_t)KEMPER_AMPS_MANF_ID, baseMessLength + size, messageToSend);
}

static const uint8_t extParamChangeMessSize PROGMEM = 9;
void kpaSendExtendedParameterChange(KpaParamExtAddress controllerAddr, uint32_t* value, uint8_t num)
{
	const size_t baseMessLength = pgm_read_byte(&extParamChangeMessSize);
	uint8_t messageToSend[baseMessLength + num*5];
	uint8_t i;
	//fill message
	fillExtMessageCommonPart(messageToSend, KPA_FUNCTION_EXTENDED_PARAMETER_CHANGE, controllerAddr);
	
	//value
	uint32_t tmpValue;

	for(i = 0; i < num; ++i)
	{
		tmpValue = *(value + i);
		messageToSend[baseMessLength + i*5] = ((uint8_t)(tmpValue >> 28)) & 0x0F;
		messageToSend[baseMessLength + 1 + i*5] = ((uint8_t)(tmpValue >> 21)) & 0x7F;
		messageToSend[baseMessLength + 2 + i*5] = ((uint8_t)(tmpValue >> 14)) & 0x7F;
		messageToSend[baseMessLength + 3 + i*5] = ((uint8_t)(tmpValue >> 7)) & 0x7F;
		messageToSend[baseMessLength + 4 + i*5] = ((uint8_t)tmpValue) & 0x7F;
	}
	midiSendSysExManfId((uint32_t)KEMPER_AMPS_MANF_ID, baseMessLength + num*5, messageToSend);	
}

static const uint8_t extStringParamChangeMessSize PROGMEM = 9;
void kpaSendExtendedStringParameterChange(KpaParamExtAddress controllerAddr, char* str)
{
	size_t baseMessLength = pgm_read_byte(&extStringParamChangeMessSize);
	size_t strLength = strlen(str);
	uint8_t messageToSend[baseMessLength + strLength + 1];

	fillExtMessageCommonPart(messageToSend, KPA_FUNCTION_EXTENDED_STRING_PARAMETER_CHANGE, controllerAddr);

	memcpy(messageToSend + baseMessLength, str, strLength);
	messageToSend[baseMessLength + strLength] = 0;
	midiSendSysExManfId((uint32_t)KEMPER_AMPS_MANF_ID, baseMessLength + strLength + 1, messageToSend);
}

static const uint8_t rqSingleParameterValueMessSize PROGMEM = 6;
void kpaSendRequestSingleParameterValue(KpaParamAddress controllerAddr)
{
	const size_t baseMessLength = pgm_read_byte(&rqSingleParameterValueMessSize);
	uint8_t messageToSend[baseMessLength];
	fillMessageCommonPart(messageToSend, KPA_FUNCTION_REQUEST_SINGLE_PARAMETER_VALUE, controllerAddr);
	midiSendSysExManfId((uint32_t)KEMPER_AMPS_MANF_ID, baseMessLength, messageToSend);	
}

static const uint8_t rqMultiParameterValueMessSize PROGMEM = 6;
void kpaSendRequestMultiParameterValues(KpaParamAddress controllerAddr)
{
	const size_t baseMessLength = pgm_read_byte(&rqMultiParameterValueMessSize);
	uint8_t messageToSend[baseMessLength];
	fillMessageCommonPart(messageToSend, KPA_FUNCTION_REQUEST_MULTI_PARAMETER_VALUES, controllerAddr);
	midiSendSysExManfId((uint32_t)KEMPER_AMPS_MANF_ID, baseMessLength, messageToSend);	
}

static const uint8_t rqStringParameterValueMessSize PROGMEM = 6;
void kpaSendRequestStringParameter(KpaParamAddress controllerAddr)
{
	const size_t baseMessLength = pgm_read_byte(&rqStringParameterValueMessSize);
	uint8_t messageToSend[baseMessLength];
	fillMessageCommonPart(messageToSend, KPA_FUNCTION_REQUEST_STRING_PARAMETER, controllerAddr);
	midiSendSysExManfId((uint32_t)KEMPER_AMPS_MANF_ID, baseMessLength, messageToSend);	
}

static const uint8_t rqExtStringParameterValueMessSize PROGMEM = 9;
void kpaSendRequestExtendedStringParameter(KpaParamExtAddress controllerAddr)
{
	const size_t baseMessLength = pgm_read_byte(&rqExtStringParameterValueMessSize);
	uint8_t messageToSend[baseMessLength];
	fillExtMessageCommonPart(messageToSend, KPA_FUNCTION_REQUEST_EXTENDED_STRING_PARAMETER, controllerAddr);
	midiSendSysExManfId((uint32_t)KEMPER_AMPS_MANF_ID, baseMessLength, messageToSend);	
}

static const uint8_t rqParameterValueAsStringMessSize PROGMEM = 8;
void kpaSendRequestParameterValueAsRenderedString(KpaParamAddress controllerAddr, uint16_t value)
{
	const size_t baseMessLength = pgm_read_byte(&rqParameterValueAsStringMessSize);
	uint8_t messageToSend[baseMessLength];
	fillMessageCommonPart(messageToSend, KPA_FUNCTION_REQUEST_PARAMETER_VALUE_AS_RENDERED_STRING, controllerAddr);
	
	messageToSend[6] = ((uint8_t)(value >> 7)) & 0x7F;
	messageToSend[7] = ((uint8_t)value) & 0x7F;
		
	midiSendSysExManfId((uint32_t)KEMPER_AMPS_MANF_ID, baseMessLength, messageToSend);	
}

static const uint8_t beaconMessSize PROGMEM = 8;
void kpaSendBeacon(uint8_t setNum, uint8_t flags, uint8_t timeLease)
{
		const size_t baseMessLength = pgm_read_byte(&beaconMessSize);
		uint8_t messageToSend[baseMessLength];
		
		//KPA_FUNCTION_SYS_COMMUNICATION function have special 1 bytes parameter address
		//Don't want to implement separate helper for this single case
		messageToSend[0] = (uint8_t)KPA_PRODUCT_TYPE;
		messageToSend[1] = (uint8_t)KPA_DEVICE_ID_OMNI;

		//function
		messageToSend[2] = (uint8_t)KPA_FUNCTION_SYS_COMMUNICATION;
		messageToSend[3] = (uint8_t)KPA_PARAMETER_INSTANCE;
				
		//address
		messageToSend[4] = (uint8_t)KPA_PARAM_BEACON;
				
		messageToSend[5] = setNum;
		messageToSend[6] = flags;
		messageToSend[7] = timeLease;
		
		midiSendSysExManfId((uint32_t)KEMPER_AMPS_MANF_ID, baseMessLength, messageToSend);
}

static const uint8_t functionCodeOffset PROGMEM = 6;
uint8_t kpaGetFunction(uint8_t* sysEx)
{
	return *(sysEx + pgm_read_byte(&functionCodeOffset));
}

static const uint8_t paramAddrOffset PROGMEM = 8;
KpaParamAddress kpaGetParamAddress(uint8_t* sysEx)
{
	return (*(sysEx + pgm_read_byte(&paramAddrOffset)) << 8) | *(sysEx + pgm_read_byte(&paramAddrOffset) + 1); 
}

KpaParamExtAddress kpaGetParamExtAddress(uint8_t* sysEx)
{
	uint8_t i;
	KpaParamExtAddress retVal = 0;
	for(i = 0; i < 5; ++i)
	{
		retVal |= *(sysEx + pgm_read_byte(&paramAddrOffset) + i);
		if (i !=4)
			retVal <<= 8;
	}
	
	return retVal;
}

static const uint8_t paramValueOffset PROGMEM = 10;
uint16_t kpaGetSingleParameterValue(uint8_t* sysEx)
{
	return (*(sysEx + pgm_read_byte(&paramValueOffset)) << 7) | *(sysEx + pgm_read_byte(&paramValueOffset) + 1); 
}

bool kpaGetMultiParameterValue(uint8_t valueNum, uint16_t* userBuffer, uint8_t* sysEx)
{
	uint16_t offset = pgm_read_byte(&paramValueOffset) + valueNum*2;
	if (offset <= (midiGetSysExLength(sysEx) - 2)) 
	{
		*userBuffer = (*(sysEx + offset) << 7) | *(sysEx + offset + 1);
		return true;
	}
	return false;
}

uint16_t kpaGetAllMultiParameterValues(uint16_t* userBuffer, uint8_t* sysEx)
{
	uint16_t i = 0;
	while(kpaGetMultiParameterValue(i, userBuffer + i, sysEx))
		++i;

	return i;
}

void kpaGetStringParameter(char* str, uint8_t maxSize, uint8_t* sysEx)
{
	strncpy(str, (char*)(sysEx + pgm_read_byte(&paramValueOffset)), maxSize);
	*(str + maxSize - 1) = '\0';
}

//Extended parameters
static const uint8_t paramExtValueOffset PROGMEM = 13;
bool kpaGetMultiExtParameterValue(uint8_t valueNum, uint32_t* userBuffer, uint8_t* sysEx)
{
	uint16_t offset = pgm_read_byte(&paramExtValueOffset) + valueNum*5;
	uint8_t i = 0;

	if (offset <= (midiGetSysExLength(sysEx) - 5))
	{
		for(i = 0; i < 5; ++i)
			*userBuffer |=  (uint32_t)(*(sysEx + offset + i)) << ((4-i)*7);

		return true;
	}
	return false;
}

uint16_t kpaGetAllMultiExtParameterValues(uint32_t* userBuffer, uint8_t* sysEx)
{
	uint16_t i = 0;
	while(kpaGetMultiExtParameterValue(i, userBuffer + i, sysEx))
		++i;

	return i;
}

void kpaGetExtStringParameter(char* str, uint8_t maxSize, uint8_t* sysEx)
{
	strncpy(str, (char*)(sysEx + pgm_read_byte(&paramExtValueOffset)), maxSize);
	*(str + maxSize - 1) = '\0';
}

static const uint8_t activeSensingCounterValueOffset PROGMEM = 9;
uint8_t kpaGetActiveSensingCounter(uint8_t* sysEx)
{
	return *(sysEx + pgm_read_byte(&activeSensingCounterValueOffset));
}


