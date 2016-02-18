/*
 * BJ Devices Travel Box series midi controller library
 * @file	axefx.c
 * 
 * @brief	Fractal Audio AxeFx (Standard, Ultra, II, II XL, II XL+) SysEx communication functions
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */ 

#include "axefx.h"
#include "midi.h"

#include <avr/pgmspace.h>
#include <string.h>


//offsets necessary data in SysEx
static const uint8_t modelIdOffsetBytes PROGMEM = 4;
static const uint8_t functionIdOffsetBytes PROGMEM = 5;
static const uint8_t fuctionPayloadOffsetBytes PROGMEM = 6;

//other constants
static const uint8_t effectBlockSize PROGMEM = 5;

static uint8_t getChecksum(uint8_t* array, uint16_t size)
{
	uint16_t i;
	uint8_t ckecksum = 0;
	
	ckecksum ^= SYSEX_STATUS;
	ckecksum ^= (uint8_t)FRACTAL_AUDIO_MANF_ID;
	ckecksum ^= (uint8_t)((uint32_t)FRACTAL_AUDIO_MANF_ID >> 8);
	ckecksum ^= (uint8_t)((uint32_t)FRACTAL_AUDIO_MANF_ID >> 16);
	
	for (i = 0; i < size; ++i)
		ckecksum ^= *(array + i);
	
	ckecksum &= 0x7f;
	
	return ckecksum; 
}

void axefxSendFunctionRequest(AxeFxModelId modelId, AxeFxFunctionId functionId, uint8_t* payload, uint16_t payloadLength)
{
	uint16_t messageSize = 3 + payloadLength;//total length is payloadLength + 1 byte modelId + 1 byte functionId + 1 byte checksumm
	uint8_t messageToSend[messageSize];
	//fill message
	messageToSend[0] = (uint8_t)modelId;
	messageToSend[1] = (uint8_t)functionId;
	if(payload != NULL && payloadLength != 0)
		memcpy((void*)(messageToSend + 2), payload, payloadLength);
		
	messageToSend[messageSize - 1] = getChecksum(messageToSend, messageSize - 1);//last byte is a checksum	
	midiSendSysExManfId(FRACTAL_AUDIO_MANF_ID, messageSize, messageToSend);
}

void axefxParseTunerInfo(AxeFxEffectTunerInfo* tunerInfo, uint8_t* sysEx)
{
	memcpy(tunerInfo, sysEx + pgm_read_byte(&fuctionPayloadOffsetBytes), sizeof(AxeFxEffectTunerInfo));
}

bool axeFxCheckFractalManufId(uint8_t* sysEx)
{
	return (FRACTAL_AUDIO_MANF_ID == midiGetSysExManufacturerId(sysEx));
}

AxeFxModelId axeFxGetModelId(uint8_t* sysEx)
{
	return (AxeFxModelId)(*(sysEx + pgm_read_byte(&modelIdOffsetBytes)));
}

AxeFxFunctionId axeFxGetFunctionId(uint8_t* sysEx)
{
	return (AxeFxFunctionId)(*(sysEx + pgm_read_byte(&functionIdOffsetBytes)));
}

uint8_t axefxGetEffectBlockStateNumber(uint8_t* sysEx)
{
	return (midiGetSysExLength(sysEx) - pgm_read_byte(&fuctionPayloadOffsetBytes)) / pgm_read_byte(&effectBlockSize);
}

static void fillEffectBlockStructFromGen2(AxeFxEffectBlockState* structToFill, uint8_t* blockInSysEx)
{
	structToFill->isEnabled_ = ((blockInSysEx[0] & 0x01) == 0x00) ? false : true;
	structToFill->isX_ = ((blockInSysEx[0] & 0x02) == 0x00) ? false : true;
	structToFill->iaCcNumber_ = ((blockInSysEx[1] >> 1) | blockInSysEx[2] << 6) & 0x7F;
	structToFill->effectId_ = ((blockInSysEx[1] >> 3) | blockInSysEx[2] << 4) & 0x7F;
}

static void fillEffectBlockStructFromGen1(AxeFxEffectBlockState* structToFill,uint8_t* blockInSysEx)
{
		structToFill->isEnabled_ = (blockInSysEx[4] == 0x00) ? false : true;
		structToFill->isX_ = false;//always false in gen1
		structToFill->iaCcNumber_ = blockInSysEx[2] | (blockInSysEx[3] << 4);
		structToFill->effectId_ = blockInSysEx[0] | (blockInSysEx[1] << 4);
}

uint8_t totalBlocks;
bool axefxGetSingleEffectBlockState(AxeFxEffectBlockState* blockState, uint8_t blockNum, uint8_t* sysEx)
{
	totalBlocks = axefxGetEffectBlockStateNumber(sysEx);
	
	if(blockNum > totalBlocks)//wrong block number is requested
		return false;
	
	AxeFxModelId model = axeFxGetModelId(sysEx);
	
	uint16_t blockOffset = pgm_read_byte(&fuctionPayloadOffsetBytes) + pgm_read_byte(&effectBlockSize)*blockNum;
	
	if(model == AXEFX_STANDARD_MODEL || model == AXEFX_ULTRA_MODEL)//check gen1 processor
		fillEffectBlockStructFromGen1(blockState, sysEx + blockOffset);
	else//if gen 2
		fillEffectBlockStructFromGen2(blockState, sysEx  + blockOffset);

	return true;	
}

uint8_t axefxGetAllEffectBlockState(AxeFxEffectBlockState* blockStates, uint8_t* sysEx)
{
	uint8_t totalBlocks = axefxGetEffectBlockStateNumber(sysEx);
	uint8_t i;
	for(i = 0; i < totalBlocks; ++i)
		axefxGetSingleEffectBlockState(blockStates + i, i, sysEx);
		
	return totalBlocks;
}

void axefxGetPresetName(char* name, uint8_t maxSize, uint8_t* sysEx)
{
	strncpy(name, (char*)(sysEx + pgm_read_byte(&fuctionPayloadOffsetBytes)), maxSize);
	*(name + maxSize) = '\0';
}