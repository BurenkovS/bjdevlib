/*
 * BJ Devices Travel Box series midi controller library
 * @file	axefx.h
 * 
 * @brief	Fractal Audio AxeFx (Standard, Ultra, II, II XL, II XL+) SysEx communication functions
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */ 


#ifndef axefx_h_
#define axefx_h_

#include <stdint.h>
#include <stdbool.h>

#define FRACTAL_AUDIO_MANF_ID 0x000174

//model ID
typedef enum AxeFxModelId
{
	AXEFX_STANDARD_MODEL	= 0x00//AXEFX standard code
	,AXEFX_ULTRA_MODEL		= 0x01//AXEFX ultra code
	,AXEFX_2_MODEL			= 0x03//AXEFX2 code
	,AXEFX_2_XL_MODEL		= 0x06//AXEFX2 XL code
	,AXEFX_2_XL_PLUS_MODEL	= 0x07//AXEFX2 XL+ code	
	
}AxeFxModelId;


//supportable function ID
typedef enum AxeFxFunctionId
{
	AXEFX_GET_SET_PARAMETER			= 0x02
	,AXEFX_GET_SET_MODIFIER			= 0x07
	,AXEFX_GET_FIRMWARE_VERSION		= 0x08
	,AXEFX_TUNER_INFO				= 0x0D
	,AXEFX_GET_PRESET_EFFECT_BLOCKS_AND_CC_AND_BYPASS_STATE = 0x0E
	,AXEFX_GET_PRESET_NAME			= 0x0F
	,AXEFX_TEMPO_BEAT				= 0x10
	,AXEFX_GET_SET_BLOCK_XY			= 0x11
	,AXEFX_GET_PRESET_NUMBER		= 0x14
	,AXEFX_GET_ROUTING_GRID_LAYOUT	= 0x20
	,AXEFX_FRONT_PANEL_CHANGE_DETECTED = 0x21
	,AXEFX_LOOPER_STATUS_ENABLE		= 0x23
	,AXEFX_SET_SCENE_NUMBER			= 0x29
	,AXEFX_MULTIPURPOSE_RESPONSE	= 0x64
	
}AxeFxFunctionId;

//looper state bits
typedef enum AxeFxLooperBit
{
	AXEFX_LOOPER_BIT_RECORD = 0
	,AXEFX_LOOPER_BIT_PLAY
	,AXEFX_LOOPER_BIT_ONCE
	,AXEFX_LOOPER_BIT_OVERDUB
	,AXEFX_LOOPER_BIT_REVERSE
	,AXEFX_LOOPER_BIT_HALF
	,AXEFX_LOOPER_BIT_UNDO
	
}AxeFxLooperBit;

/*
 * MIDI_GET_PRESET_EFFECT_BLOCKS_AND_CC_AND_BYPASS_STATE response format
 * response consist of several blocks of following structure  
 */
typedef struct AxeFxEffectBlockState
{
	bool isEnabled_;		//true = Enabled, false = Bypassed
	bool isX_;				//X/Y state : true = X, false = Y
	uint8_t iaCcNumber_;	//CC number assigned to this effect
	uint8_t iaXYCcNumber_;  //XY switch CC number assigned to this effect
	uint8_t effectId_;		//Effect ID
}AxeFxEffectBlockState;

/*
 * AXEFX_TUNER_INFO response format
 * response consist of several blocks of following structure  
 */
typedef struct AxeFxEffectTunerInfo
{
	uint8_t note_;			//Note (0=A; 1=Bb; 2=B; etc.)
	uint8_t stringNumber_;	//String number (0=high E string; 5=low E string)
	uint8_t tuneData_;		//Tuner data (value of 63 indicates 'perfectly in tune')
}AxeFxEffectTunerInfo;

/*
 * AXEFX_LOOPER_STATUS_ENABLE response format
 * response consist of several blocks of following structure  
 */
typedef struct AxeFxLooperInfo
{
	uint8_t status;      //Looper Status Bits (.status & AXEFX_LOOPER_BIT_RECORD)
	uint8_t position;    //Looper Position (range: 0 to 99)
}AxeFxLooperInfo;

/*
 * AXEFX_MULTIPURPOSE_RESPONSE response format
 * response consist of several blocks of following structure
 */
typedef struct AxeFxMultipurposeResponseInfo
{
	uint8_t functionId; //Function ID
	uint8_t code;       //Response code
}AxeFxMultipurposeResponseInfo;

/*
 * @brief	Send function request to axe fx. If request contain any payload expect then functionId,
 *			user should prepare it by himself and pass as function parameter.
 * @param	modelId -		AxeFx model ID. See AxeFxModelId enum
 * @param	functionId -	AxeFx function ID. See AxeFxFunctionId enum
 * @param	*payload -		payload data pointer. If payload don't needed, pass NULL
 * @param	payloadLength -	payload length
 */ 
void axefxSendFunctionRequest(AxeFxModelId modelId, AxeFxFunctionId functionId, uint8_t* payload, uint16_t payloadLength);


/*
 * @brief	Parse SysEx message and fill tunerInfo structure, if SysEx is valid AXEFX_TUNER_INFO message
 * @param	*tunerInfo -	pointer to tunerInfo structure which will contain tuner info  
 * @param	*sysEx -		pointer to SysEx message to parse 
 */ 
void axefxParseTunerInfo(AxeFxEffectTunerInfo* tunerInfo, uint8_t* sysEx);

/*
 * @brief	Parse SysEx message and fill looperInfo structure, if SysEx is valid AXEFX_LOOPER_STATUS_ENABLE message
 * @param	*looperInfo -	pointer to looperInfo structure which will contain tuner info
 * @param	*sysEx -		pointer to SysEx message to parse
 */
void axefxParseLooperInfo(AxeFxLooperInfo *looperInfo, uint8_t *sysEx);

/*
 * @brief	Parse SysEx message and fill responseInfo structure, if SysEx is valid AXEFX_MULTIPURPOSE_RESPONSE message
 * @param	*responseInfo -	pointer to tunerInfo structure which will contain response info
 * @param	*sysEx -		pointer to SysEx message to parse
 */
void axefxParseMultipurposeResponseInfo(AxeFxMultipurposeResponseInfo *responseInfo, uint8_t *sysEx);

/*
 * @brief	Check if SysEx have Fractal Audio manufacturer id (0x00 0x01 0x74)
 * @param	*sysEx -		pointer to SysEx message to parse 
 * @return	true if SysEx have Fractal Audio manufacturer id, else false
 */ 
bool axeFxCheckFractalManufId(uint8_t* sysEx);

/*
 * @brief	Get model ID from SysEx
 * @param	*sysEx -	pointer to SysEx message to parse 
 */ 
AxeFxModelId axeFxGetModelId(uint8_t* sysEx);

/*
 * @brief	Get function ID from SysEx
 * @param	*sysEx -	pointer to SysEx message to parse 
 */ 
AxeFxFunctionId axeFxGetFunctionId(uint8_t* sysEx);


//following functions using for process AXEFX_GET_PRESET_EFFECT_BLOCKS_AND_CC_AND_BYPASS_STATE message
/*
 * @brief	Get numbers of effect blocks (5-byte chunks) in message
 * @param	*sysEx -	pointer to SysEx message to parse 
 */ 
uint8_t axefxGetEffectBlockStateNumber(uint8_t* sysEx);

/*
 * @brief	Parse single effect block (5-byte chunk) from message. 
 *			Function does not perform any validation of message structure
 *			or function ID,	it is a user responsibility
 * @param	*blockState -	pointer to AxeFxEffectBlockState structure to fill data
 * @param	blockNum -		chunk sequence number in the message
 * @param	*sysEx -		pointer to SysEx message to parse
 * @return	true if blockNum exists in this SysEx message  
 */ 
bool axefxGetSingleEffectBlockState(AxeFxEffectBlockState* blockState, uint8_t blockNum, uint8_t* sysEx);

/*
 * @brief	Parse all effect blocks (5-byte chunks) from message. 
 *			Function does not perform any validation of message structure
 *			or function ID,	it is a user responsibility
 * @param	*blockState -	pointer to begin of array of AxeFxEffectBlockState structures to fill data.
 *							User must take care about array size, it must be larger or equal than maximum effects in preset
 * @param	*sysEx -		pointer to SysEx message to parse
 * @return	number of blocks was parsed  
 */ 
uint8_t axefxGetAllEffectBlockState(AxeFxEffectBlockState* blockStates, uint8_t* sysEx);


//following functions using for process AXEFX_GET_PRESET_NAME message

/*
 * @brief	Copy preset name from SysEx to user buffer. The behaviour is same as strncpy(), 
			but null symbol will always added in the last position of user buffer in any case  
 * @param	*name -		pointer to user buffer
 * @param	maxSize -	maximum preset name length. Character above this value will dropped
 * @param	*sysEx -	pointer to SysEx message to parse
 */ 
void axefxGetPresetName(char* name, uint8_t maxSize, uint8_t* sysEx);


//following functions using for process AXEFX_SET_SCENE_NUMBER message

/*
 * @brief	Get scene number from SysEx
 * @param	*sysEx -	pointer to SysEx message to parse
 */
uint8_t axefxGetSceneNumber(uint8_t *sysEx);


//following functions using for process AXEFX_GET_PRESET_NUMBER message

/*
 * @brief	Get preset number from SysEx
 * @param	*sysEx -	pointer to SysEx message to parse
 */
uint16_t axefxGetPresetNumber(uint8_t *sysEx);

#endif /* axefx_h_ */