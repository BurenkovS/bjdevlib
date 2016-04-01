/*
 * BJ Devices Travel Box series midi controller library
 * @file	kpa.h
 * 
 * @brief	Kemper Profiling Amplifier communication functions
			  
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */ 


#ifndef kpa_h_
#define kpa_h_

#include <stdint.h>
#include <stdbool.h>

#define KEMPER_AMPS_MANF_ID 0x002033
//only Kemper Profiling Amplifier is supported
#define KPA_PRODUCT_TYPE 0x02
#define KPA_DEVICE_ID_OMNI 0x7F
#define KPA_PARAMETER_INSTANCE 0x00


//KpaParamAddress is native parameter address, as it describe in "Profiler Midi Parameter Documentation.pdf"
//Consist from 7bit MSB and 7bit LSB parts and will be send to KPA as two bytes with 0 in most significant bit 
//MSB is page address (0-0x7F), LSB is parameter offset in page (0-0x7F)
//For example parameter "Stomp D Rotary Switch" have sequence number 0x1A9E and will be send as 0x35 and 0x1E
//So valid KpaParamAddress for parameter "Stomp D Rotary Switch" is 0x351E
//All KPA_PARAM_* defines is KpaParamAddress 
typedef uint16_t KpaParamAddress;
//The same, but for extended address. 
//In this case address encoded in 5 bytes, so using uint64_t(3 most significant bytes unused)
typedef uint64_t KpaParamExtAddress;

/*
 * @brief	Send Single Parameter Change function to KPA
 * @param	controller -	NRPN address
 * @param	value -			14-bit NRPN value
 */ 
void kpaSendSingleParameterChange(KpaParamAddress controllerAddr, uint16_t value);


/*
 * @brief	Send Multi Parameter Change  function to KPA
 *			To change a whole bunch of parameters you can send multiple
 *			values for a whole range of parameters
 * @param	controller -	NRPN starting address
 * @param	value -			pointer to 14-bit NRPN values. 
 *							each next value will applied to the next NRPN address(up to 64 values)
 * @param	num -			numbers of values 
 */ 
void kpaSendMultiParameterChange(KpaParamAddress controllerAddr, uint16_t* value, uint8_t num);

/*
 * @brief	Send String Parameter Change function to KPA
 * @param	controller -	NRPN address
 * @param	str -			A 7-bit values representing an ASCII null byte terminated string.
 */ 
void kpaSendStringParameterChange(KpaParamAddress controllerAddr, const char* str);

/*
 * @brief	Send BLOB Parameter Change function to KPA
 * @param	controller -	NRPN address(BLOB controller)
 * @param	startIffset -	14-bit start offset
 * @param	content	-		N times a 7-bit value where N is the number given in the size attribute
 * @param	size -			14-bit size
 */ 
void kpaSendBLOBChange(KpaParamAddress controllerAddr, uint16_t startOffset, uint8_t* content, uint16_t size);

/*
 * @brief	Send Extended Parameter Change function to KPA
 * @param	controller -	NRPN starting address
 * @param	value -			pointer to 32-bit NRPN values.
 *							each next value will applied to the next NRPN address(up to 64 values)
 * @param	num -			numbers of values
 */ 
void kpaSendExtendedParameterChange(KpaParamExtAddress controllerAddr, uint32_t* value, uint8_t num);

/*
 * @brief	Send Extended String Parameter Change function to KPA
 * @param	controller -	NRPN address
 * @param	str -			A 7-bit values representing an ASCII null byte terminated string.
 */ 
void kpaSendExtendedStringParameterChange(KpaParamExtAddress controllerAddr, char* str);

/*
 * @brief	Send Request Single Parameter function to KPA
 * @param	controller -	14-bit NRPN address
 */ 
void kpaSendRequestSingleParameterValue(KpaParamAddress controllerAddr);

/*
 * @brief	Send Request Multi Parameter function to KPA. Notice that there is
 *			no size attribute defined. The response does cover all parameter 
 *			of the requested unit. Expect up to 128 values
 * @param	controller -	14-bit NRPN address
 */ 
void kpaSendRequestMultiParameterValues(KpaParamAddress controllerAddr);

/*
 * @brief	Send Request String Parameter function to KPA
 * @param	controller -	14-bit NRPN address
 */ 
void kpaSendRequestStringParameter(KpaParamAddress controllerAddr);

/*
 * @brief	Send Request Extended String Parameter function to KPA
 * @param	controller -	31-bit NRPN address
 */ 
void kpaSendRequestExtendedStringParameter(KpaParamExtAddress controllerAddr);

/*
 * @brief	Send Request Parameter Value as Rendered String function to KPA.
 *			Retrieve the string representation for a specific value of a parameter. 
 * @param	controller -	14-bit NRPN address
 * @param	value -			14-bit NRPN value
 */ 
void kpaSendRequestParameterValueAsRenderedString(KpaParamAddress controllerAddr, uint16_t value);

/*
 * @brief	Send "beacon" message.
 *			"beacon" is a periodic signaling command to keep the KPA sending relevant changes
 * @param	setNum -	set of parameters to be sent back.
 *						0 - disables the set, 1..n  - selects a specific set.
 *						1 - This set is designed to allow clients to reflect the On/Off
 *						states of the KPA Stomp Slots as well as the currently selected
 *						program in performance mode.
 *						2 - This set is designed to allow clients to reflect the On/Off
 *						states of the KPA Stomp Slots as well as the currently selected
 *						program in performance mode. Additionally it provides the rig name
 *						and the short strings for stomp types.
 *						Every sets also contain tap event, tuner and current mode information
 * @param	flags -		bits that represent specific options for the command or set. see KPA_BEACON_FLAG_... macro
 * @param	timeLease -	time Lease in units of 2 seconds. 1..126  - 2 to 252 seconds, 127 - forever 
 *						KPA stops sending parameter changes after the time given in the field t is over.
 *						To keep the KPA sending updates, the beacon message just needs to be sent again 
 *						within the time interval given in the previously sent beacon message.
 *
 */ 
void kpaSendBeacon(uint8_t setNum, uint8_t flags, uint8_t timeLease);


//parsing functions

/*
 * @brief	Get function code from SysEx message
 */
uint8_t kpaGetFunction(uint8_t* sysEx);

/*
 * @brief	Get KpaParamAddress from any SysEx message
 */
KpaParamAddress kpaGetParamAddress(uint8_t* sysEx);

/*
 * @brief	Get KpaParamExtAddress from any SysEx message
 */
KpaParamExtAddress kpaGetParamExtAddress(uint8_t* sysEx);

/*
 * @brief	Get single parameter value from SysEx message.
 *			Note the corresponding parameter address is not extended
 */
uint16_t kpaGetSingleParameterValue(uint8_t* sysEx);

/*
 * @brief	Copy all values from SysEx message(KPA_FUNCTION_MULTI_PARAMETER_CHANGE)
 *			Note the corresponding parameter address is not extended
 * @param	*userBuffer -	pointer to user buffer for copy. Expect up to 128 values
 * @param	*sysEx -		pointer to SysEx message. Input parameter
 * @return	total number of parameters in message
 */
uint16_t kpaGetAllMultiParameterValues(uint16_t* userBuffer, uint8_t* sysEx);

/*
 * @brief	Copy singe value of several values from SysEx message(KPA_FUNCTION_MULTI_PARAMETER_CHANGE)
 *			Note the corresponding parameter address is not extended
 * @param	valueNum -		sequence number of requested value
 * @param	*userBuffer -	pointer to user variable for copy.
 * @param	*sysEx -		pointer to SysEx message. Input parameter
 * @return	true if value blockNum exists in this SysEx message 
 */
bool kpaGetMultiParameterValue(uint8_t valueNum, uint16_t* userBuffer, uint8_t* sysEx);

/*
 * @brief	Copy all values from SysEx message(KPA_FUNCTION_EXTENDED_PARAMETER_CHANGE)
 *			Note the corresponding parameter address is extended
 * @param	*userBuffer -	pointer to user buffer for copy. Expect up to 128 values
 * @param	*sysEx -		pointer to SysEx message. Input parameter
 * @return	total number of parameters in message
 */
uint16_t kpaGetAllMultiExtParameterValues(uint32_t* userBuffer, uint8_t* sysEx);

/*
 * @brief	Copy singe value of several values from SysEx message(KPA_FUNCTION_EXTENDED_PARAMETER_CHANGE)
 *			Note the corresponding parameter address is extended!
 * @param	valueNum -		sequence number of requested value
 * @param	*userBuffer -	pointer to user variable for copy.
 * @param	*sysEx -		pointer to SysEx message. Input parameter
 * @return	true if value blockNum exists in this SysEx message 
 */
bool kpaGetMultiExtParameterValue(uint8_t valueNum, uint32_t* userBuffer, uint8_t* sysEx);

/*
 * @brief	Copy string from SysEx to user buffer. The behaivour is same as strncpy(), 
 *			but null symbol will always added in the last position of user buffer in any case;
 *			Note the corresponding parameter address is NOT extended  
 * @param	*str -		pointer to user buffer
 * @param	maxSize -	maximum preset name length. Character above this value will dropped
 * @param	*sysEx -	pointer to SysEx message to parse
 */
void kpaGetStringParameter(char* str, uint8_t maxSize, uint8_t* sysEx);

/*
 * @brief	Copy extended string from SysEx to user buffer. The behaivour is same as strncpy(), 
 *			but null symbol will always added in the last position of user buffer in any case;
 *			Note the corresponding parameter address is extended!  
 * @param	*str -		pointer to user buffer
 * @param	maxSize -	maximum preset name length. Character above this value will dropped
 * @param	*sysEx -	pointer to SysEx message to parse
 */
void kpaGetExtStringParameter(char* str, uint8_t maxSize, uint8_t* sysEx);

/*
 * @brief	Return counter field from "active sensing" signal (SysEx message) 
 *			To make the communication fool proof against the MIDI cabling,
 *			the KPA will send a SYSEX string back that allows the detection 
 *			of missing packets or broken cables. It is being sent every 500ms
 *			as long as the time lease is valid(see kpaSendBeacon function).
 *			It incorporates an increasing 7-bit counter. At 127 the counter wraps to 0.
 *			This mechanism can be suppressed by setting the “NOCTR” bit 
 *			in the beacon message. If the client side detects a missing sensing message,
 *			the INIT bit could be set again to request the full set again
 * @return	7-bit counter from  "active sensing" message
 */
uint8_t kpaGetActiveSensingCounter(uint8_t* sysEx);


//Beacon flags
#define KPA_BEACON_FLAG_INIT        0x01//If the set should be initially sent after enabling the bidirectional mode
#define KPA_BEACON_FLAG_SYSEX       0x02//If the set should send NRPN CC (0) or use SYSEX (1)
#define KPA_BEACON_FLAG_ECHO        0x04//If set, parameters are sent back even if they were set via MIDI IN
#define KPA_BEACON_FLAG_NOFE        0x08//if set, the KPA will stop sending 0xFE when protocol is initiated
#define KPA_BEACON_FLAG_NOCTR       0x10//if set, the KPA will not send back the periodic KPA_PARAM_ACTIVE_SENSING_SIGNAL
#define KPA_BEACON_FLAG_TUNEMODE    0x20//if set, the Tuning information is only sent in Tuner Mode, otherwise it’s being sent all the time


//KPA functions code
#define KPA_FUNCTION_SINGLE_PARAMETER_CHANGE					0x01
#define KPA_FUNCTION_MULTI_PARAMETER_CHANGE						0x02 
#define KPA_FUNCTION_STRING_PARAMETER_CHANGE					0x03 
#define KPA_FUNCTION_BLOB										0x04 
#define KPA_FUNCTION_EXTENDED_PARAMETER_CHANGE					0x06 
#define KPA_FUNCTION_EXTENDED_STRING_PARAMETER_CHANGE			0x07 
#define KPA_FUNCTION_REQUEST_SINGLE_PARAMETER_VALUE				0x41 
#define KPA_FUNCTION_REQUEST_MULTI_PARAMETER_VALUES				0x42 
#define KPA_FUNCTION_REQUEST_STRING_PARAMETER					0x43 
#define KPA_FUNCTION_REQUEST_EXTENDED_STRING_PARAMETER			0x47 
#define KPA_FUNCTION_REQUEST_PARAMETER_VALUE_AS_RENDERED_STRING 0x7C
//it used for "beacon" and "active sensing signal" only  
#define KPA_FUNCTION_SYS_COMMUNICATION 0x7E 


//Parameters addresses. MSB is a page address, LSB is parameter offset. 
//See "Profiler Midi Parameter Documentation.pdf"
//RIG (ADDRESS PAGE 4)
#define KPA_PARAM_RIG_TEMPO			0x0400
#define KPA_PARAM_RIG_VOLUME		0x0401
#define KPA_PARAM_RIG_TEMPO_ENABLE	0x0402
//INPUT (ADDRESS PAGE 9)
#define KPA_PARAM_INPUT_NOISE_GATE_INTENSITY	0x0903
#define KPA_PARAM_INPUT_CLEAN_SENSE				0x0904
#define KPA_PARAM_INPUT_DISTORTION_SENSE		0x0904
//AMPLIFIER (ADDRESS PAGE 10)
#define KPA_PARAM_AMPLIFIER_ON_OFF			0x0A02
#define KPA_PARAM_AMPLIFIER_GAIN			0x0A04
#define KPA_PARAM_AMPLIFIER_DEFINITION		0x0A06
#define KPA_PARAM_AMPLIFIER_CLARITY			0x0A07
#define KPA_PARAM_AMPLIFIER_POWER_SAGGING	0x0A08
#define KPA_PARAM_AMPLIFIER_PICK			0x0A09
#define KPA_PARAM_AMPLIFIER_COMPRESSOR		0x0A0A
#define KPA_PARAM_AMPLIFIER_TUBE_SHAPE		0x0A0B
#define KPA_PARAM_AMPLIFIER_TUBE_BIAS		0x0A0C
#define KPA_PARAM_AMPLIFIER_DIRECT_MIX		0x0A0F
//EQUALIZER (ADDRESS PAGE 11)
#define KPA_PARAM_EQUALIZER_ON_OFF		0x0B02
#define KPA_PARAM_EQUALIZER_BASS		0x0B04
#define KPA_PARAM_EQUALIZER_MIDDLE		0x0B05
#define KPA_PARAM_EQUALIZER_TREBLE		0x0B06	
#define KPA_PARAM_EQUALIZER_PRESENCE	0x0B07
//CABINET (ADDRESS PAGE 12)
#define KPA_PARAM_CABINET_ON_OFF		0x0C02
#define KPA_PARAM_CABINET_VOLUME		0x0C03
#define KPA_PARAM_CABINET_HIGH_SHIFT	0x0C04
#define KPA_PARAM_CABINET_LOW_SHIFT		0x0C05
#define KPA_PARAM_CABINET_CHARACTER		0x0C06
//STOMP A (ADDRESS PAGE 50)
#define KPA_PARAM_STOMP_A_TYPE								0x3200
#define KPA_PARAM_STOMP_A_ON_OFF							0x3203
#define KPA_PARAM_STOMP_A_MIX								0x3204
#define KPA_PARAM_STOMP_A_VOLUME							0x3206
#define KPA_PARAM_STOMP_A_STEREO							0x3207
#define KPA_PARAM_STOMP_A_WAH_MANUAL						0x3208
#define KPA_PARAM_STOMP_A_WAH_PEAK							0x3209
#define KPA_PARAM_STOMP_A_WAH_RANGE							0x320A
#define KPA_PARAM_STOMP_A_WAH_PEDAL_MODE					0x320C
#define KPA_PARAM_STOMP_A_WAH_TOUCH_ATTACK					0x320D
#define KPA_PARAM_STOMP_A_WAH_TOUCH_RELEASE					0x320E
#define KPA_PARAM_STOMP_A_WAH_TOUCH_BOOST					0x320F
#define KPA_PARAM_STOMP_A_DISTORTION_SHAPER_DRIVE			0x3210
#define KPA_PARAM_STOMP_A_DISTORTION_BOOSTER_TONE			0x3211
#define KPA_PARAM_STOMP_A_COMPRESSOR_GATE_INTENSITY			0x3212
#define KPA_PARAM_STOMP_A_COMPRESSOR_ATTACK					0x3213
#define KPA_PARAM_STOMP_A_MODULATION_RATE					0x3214
#define KPA_PARAM_STOMP_A_MODULATION_DEPTH					0x3215
#define KPA_PARAM_STOMP_A_MODULATION_FEEDBACK				0x3216
#define KPA_PARAM_STOMP_A_MODULATION_CROSSOVER				0x3217
#define KPA_PARAM_STOMP_A_MODULATION_HYPERCHORUS_AMOUNT		0x3218
#define KPA_PARAM_STOMP_A_MODULATION_MANUAL					0x3219
#define KPA_PARAM_STOMP_A_MODULATION_PHASER_PEAK_SPREAD		0x321A
#define KPA_PARAM_STOMP_A_MODULATION_PHASER_STAGES			0x321B
#define KPA_PARAM_STOMP_A_ROTARY_SPEED_SLOW_FAST			0x321E	
#define KPA_PARAM_STOMP_A_ROTARY_DISTANCE					0x321F
#define KPA_PARAM_STOMP_A_ROTARY_BALANCE					0x3220
#define KPA_PARAM_STOMP_A_COMPRESSOR_SQUASH					0x3221
#define KPA_PARAM_STOMP_A_GRAPHIC_EQ_BAND_1					0x3222
#define KPA_PARAM_STOMP_A_GRAPHIC_EQ_BAND_2					0x3223
#define KPA_PARAM_STOMP_A_GRAPHIC_EQ_BAND_3					0x3224
#define KPA_PARAM_STOMP_A_GRAPHIC_EQ_BAND_4					0x3225
#define KPA_PARAM_STOMP_A_GRAPHIC_EQ_BAND_5					0x3226
#define KPA_PARAM_STOMP_A_GRAPHIC_EQ_BAND_6					0x3227
#define KPA_PARAM_STOMP_A_GRAPHIC_EQ_BAND_7					0x3228
#define KPA_PARAM_STOMP_A_GRAPHIC_EQ_BAND_8					0x3229
#define KPA_PARAM_STOMP_A_PARAMETRIC_EQ_LOW_GAIN			0x322A
#define KPA_PARAM_STOMP_A_PARAMETRIC_EQ_LOW_FREQUENCY		0x322B
#define KPA_PARAM_STOMP_A_PARAMETRIC_EQ_HIGH_GAIN			0x322C
#define KPA_PARAM_STOMP_A_PARAMETRIC_EQ_HIGH_FREQUENCY		0x322D
#define KPA_PARAM_STOMP_A_PARAMETRIC_EQ_PEAK_GAIN			0x322E
#define KPA_PARAM_STOMP_A_PARAMETRIC_EQ_PEAK_FREQUENCY		0x322F
#define KPA_PARAM_STOMP_A_PARAMETRIC_EQ_PEAK_Q_FACTOR		0x3230
#define KPA_PARAM_STOMP_A_PARAMETRIC_EQ_PEAK_GAIN_2			0x3231
#define KPA_PARAM_STOMP_A_PARAMETRIC_EQ_PEAK_FREQUENCY_2	0x3232
#define KPA_PARAM_STOMP_A_PARAMETRIC_EQ_PEAK_Q_FACTOR_2		0x3233
#define KPA_PARAM_STOMP_A_WAH_PEAK_RANGE					0x3234
//STOMP B (ADDRESS PAGE 51)
#define KPA_PARAM_STOMP_B_TYPE								0x3300
#define KPA_PARAM_STOMP_B_ON_OFF							0x3303
#define KPA_PARAM_STOMP_B_MIX								0x3304
#define KPA_PARAM_STOMP_B_VOLUME							0x3306
#define KPA_PARAM_STOMP_B_STEREO							0x3307
#define KPA_PARAM_STOMP_B_WAH_MANUAL						0x3308
#define KPA_PARAM_STOMP_B_WAH_PEAK							0x3309
#define KPA_PARAM_STOMP_B_WAH_RANGE							0x330A
#define KPA_PARAM_STOMP_B_WAH_PEDAL_MODE					0x330C
#define KPA_PARAM_STOMP_B_WAH_TOUCH_ATTACK					0x330D
#define KPA_PARAM_STOMP_B_WAH_TOUCH_RELEASE					0x330E
#define KPA_PARAM_STOMP_B_WAH_TOUCH_BOOST					0x330F
#define KPA_PARAM_STOMP_B_DISTORTION_SHAPER_DRIVE			0x3310
#define KPA_PARAM_STOMP_B_DISTORTION_BOOSTER_TONE			0x3311
#define KPA_PARAM_STOMP_B_COMPRESSOR_GATE_INTENSITY			0x3312
#define KPA_PARAM_STOMP_B_COMPRESSOR_ATTACK					0x3313
#define KPA_PARAM_STOMP_B_MODULATION_RATE					0x3314
#define KPA_PARAM_STOMP_B_MODULATION_DEPTH					0x3315
#define KPA_PARAM_STOMP_B_MODULATION_FEEDBACK				0x3316
#define KPA_PARAM_STOMP_B_MODULATION_CROSSOVER				0x3317
#define KPA_PARAM_STOMP_B_MODULATION_HYPERCHORUS_AMOUNT		0x3318
#define KPA_PARAM_STOMP_B_MODULATION_MANUAL					0x3319
#define KPA_PARAM_STOMP_B_MODULATION_PHASER_PEAK_SPREAD		0x331A
#define KPA_PARAM_STOMP_B_MODULATION_PHASER_STAGES			0x331B
#define KPA_PARAM_STOMP_B_ROTARY_SPEED_SLOW_FAST			0x331E
#define KPA_PARAM_STOMP_B_ROTARY_DISTANCE					0x331F
#define KPA_PARAM_STOMP_B_ROTARY_BALANCE					0x3320
#define KPA_PARAM_STOMP_B_COMPRESSOR_SQUASH					0x3321
#define KPA_PARAM_STOMP_B_GRAPHIC_EQ_BAND_1					0x3322
#define KPA_PARAM_STOMP_B_GRAPHIC_EQ_BAND_2					0x3323
#define KPA_PARAM_STOMP_B_GRAPHIC_EQ_BAND_3					0x3324
#define KPA_PARAM_STOMP_B_GRAPHIC_EQ_BAND_4					0x3325
#define KPA_PARAM_STOMP_B_GRAPHIC_EQ_BAND_5					0x3326
#define KPA_PARAM_STOMP_B_GRAPHIC_EQ_BAND_6					0x3327
#define KPA_PARAM_STOMP_B_GRAPHIC_EQ_BAND_7					0x3328
#define KPA_PARAM_STOMP_B_GRAPHIC_EQ_BAND_8					0x3329
#define KPA_PARAM_STOMP_B_PARAMETRIC_EQ_LOW_GAIN			0x332A
#define KPA_PARAM_STOMP_B_PARAMETRIC_EQ_LOW_FREQUENCY		0x332B
#define KPA_PARAM_STOMP_B_PARAMETRIC_EQ_HIGH_GAIN			0x332C
#define KPA_PARAM_STOMP_B_PARAMETRIC_EQ_HIGH_FREQUENCY		0x332D
#define KPA_PARAM_STOMP_B_PARAMETRIC_EQ_PEAK_GAIN			0x332E
#define KPA_PARAM_STOMP_B_PARAMETRIC_EQ_PEAK_FREQUENCY		0x332F
#define KPA_PARAM_STOMP_B_PARAMETRIC_EQ_PEAK_Q_FACTOR		0x3330
#define KPA_PARAM_STOMP_B_PARAMETRIC_EQ_PEAK_GAIN_2			0x3331
#define KPA_PARAM_STOMP_B_PARAMETRIC_EQ_PEAK_FREQUENCY_2	0x3332
#define KPA_PARAM_STOMP_B_PARAMETRIC_EQ_PEAK_Q_FACTOR_2		0x3333
#define KPA_PARAM_STOMP_B_WAH_PEAK_RANGE					0x3334
//STOMP C (ADDRESS PAGE 52)
#define KPA_PARAM_STOMP_C_TYPE								0x3400
#define KPA_PARAM_STOMP_C_ON_OFF							0x3403
#define KPA_PARAM_STOMP_C_MIX								0x3404
#define KPA_PARAM_STOMP_C_VOLUME							0x3406
#define KPA_PARAM_STOMP_C_STEREO							0x3407
#define KPA_PARAM_STOMP_C_WAH_MANUAL						0x3408
#define KPA_PARAM_STOMP_C_WAH_PEAK							0x3409
#define KPA_PARAM_STOMP_C_WAH_RANGE							0x340A
#define KPA_PARAM_STOMP_C_WAH_PEDAL_MODE					0x340C
#define KPA_PARAM_STOMP_C_WAH_TOUCH_ATTACK					0x340D
#define KPA_PARAM_STOMP_C_WAH_TOUCH_RELEASE					0x340E
#define KPA_PARAM_STOMP_C_WAH_TOUCH_BOOST					0x340F
#define KPA_PARAM_STOMP_C_DISTORTION_SHAPER_DRIVE			0x3410
#define KPA_PARAM_STOMP_C_DISTORTION_BOOSTER_TONE			0x3411
#define KPA_PARAM_STOMP_C_COMPRESSOR_GATE_INTENSITY			0x3412
#define KPA_PARAM_STOMP_C_COMPRESSOR_ATTACK					0x3413
#define KPA_PARAM_STOMP_C_MODULATION_RATE					0x3414
#define KPA_PARAM_STOMP_C_MODULATION_DEPTH					0x3415
#define KPA_PARAM_STOMP_C_MODULATION_FEEDBACK				0x3416
#define KPA_PARAM_STOMP_C_MODULATION_CROSSOVER				0x3417
#define KPA_PARAM_STOMP_C_MODULATION_HYPERCHORUS_AMOUNT		0x3418
#define KPA_PARAM_STOMP_C_MODULATION_MANUAL					0x3419
#define KPA_PARAM_STOMP_C_MODULATION_PHASER_PEAK_SPREAD		0x341A
#define KPA_PARAM_STOMP_C_MODULATION_PHASER_STAGES			0x341B
#define KPA_PARAM_STOMP_C_ROTARY_SPEED_SLOW_FAST			0x341E
#define KPA_PARAM_STOMP_C_ROTARY_DISTANCE					0x341F
#define KPA_PARAM_STOMP_C_ROTARY_BALANCE					0x3420
#define KPA_PARAM_STOMP_C_COMPRESSOR_SQUASH					0x3421
#define KPA_PARAM_STOMP_C_GRAPHIC_EQ_BAND_1					0x3422
#define KPA_PARAM_STOMP_C_GRAPHIC_EQ_BAND_2					0x3423
#define KPA_PARAM_STOMP_C_GRAPHIC_EQ_BAND_3					0x3424
#define KPA_PARAM_STOMP_C_GRAPHIC_EQ_BAND_4					0x3425
#define KPA_PARAM_STOMP_C_GRAPHIC_EQ_BAND_5					0x3426
#define KPA_PARAM_STOMP_C_GRAPHIC_EQ_BAND_6					0x3427
#define KPA_PARAM_STOMP_C_GRAPHIC_EQ_BAND_7					0x3428
#define KPA_PARAM_STOMP_C_GRAPHIC_EQ_BAND_8					0x3429
#define KPA_PARAM_STOMP_C_PARAMETRIC_EQ_LOW_GAIN			0x342A
#define KPA_PARAM_STOMP_C_PARAMETRIC_EQ_LOW_FREQUENCY		0x342B
#define KPA_PARAM_STOMP_C_PARAMETRIC_EQ_HIGH_GAIN			0x342C
#define KPA_PARAM_STOMP_C_PARAMETRIC_EQ_HIGH_FREQUENCY		0x342D
#define KPA_PARAM_STOMP_C_PARAMETRIC_EQ_PEAK_GAIN			0x342E
#define KPA_PARAM_STOMP_C_PARAMETRIC_EQ_PEAK_FREQUENCY		0x342F
#define KPA_PARAM_STOMP_C_PARAMETRIC_EQ_PEAK_Q_FACTOR		0x3430
#define KPA_PARAM_STOMP_C_PARAMETRIC_EQ_PEAK_GAIN_2			0x3431
#define KPA_PARAM_STOMP_C_PARAMETRIC_EQ_PEAK_FREQUENCY_2	0x3432
#define KPA_PARAM_STOMP_C_PARAMETRIC_EQ_PEAK_Q_FACTOR_2		0x3433
#define KPA_PARAM_STOMP_C_WAH_PEAK_RANGE					0x3434
//STOMP D (ADDRESS PAGE 53)
#define KPA_PARAM_STOMP_D_TYPE								0x3500
#define KPA_PARAM_STOMP_D_ON_OFF							0x3503
#define KPA_PARAM_STOMP_D_MIX								0x3504
#define KPA_PARAM_STOMP_D_VOLUME							0x3506
#define KPA_PARAM_STOMP_D_STEREO							0x3507
#define KPA_PARAM_STOMP_D_WAH_MANUAL						0x3508
#define KPA_PARAM_STOMP_D_WAH_PEAK							0x3509
#define KPA_PARAM_STOMP_D_WAH_RANGE							0x350A
#define KPA_PARAM_STOMP_D_WAH_PEDAL_MODE					0x350C
#define KPA_PARAM_STOMP_D_WAH_TOUCH_ATTACK					0x350D
#define KPA_PARAM_STOMP_D_WAH_TOUCH_RELEASE					0x350E
#define KPA_PARAM_STOMP_D_WAH_TOUCH_BOOST					0x350F
#define KPA_PARAM_STOMP_D_DISTORTION_SHAPER_DRIVE			0x3510
#define KPA_PARAM_STOMP_D_DISTORTION_BOOSTER_TONE			0x3511
#define KPA_PARAM_STOMP_D_COMPRESSOR_GATE_INTENSITY			0x3512
#define KPA_PARAM_STOMP_D_COMPRESSOR_ATTACK					0x3513
#define KPA_PARAM_STOMP_D_MODULATION_RATE					0x3514
#define KPA_PARAM_STOMP_D_MODULATION_DEPTH					0x3515
#define KPA_PARAM_STOMP_D_MODULATION_FEEDBACK				0x3516
#define KPA_PARAM_STOMP_D_MODULATION_CROSSOVER				0x3517
#define KPA_PARAM_STOMP_D_MODULATION_HYPERCHORUS_AMOUNT		0x3518
#define KPA_PARAM_STOMP_D_MODULATION_MANUAL					0x3519
#define KPA_PARAM_STOMP_D_MODULATION_PHASER_PEAK_SPREAD		0x351A
#define KPA_PARAM_STOMP_D_MODULATION_PHASER_STAGES			0x351B
#define KPA_PARAM_STOMP_D_ROTARY_SPEED_SLOW_FAST			0x351E
#define KPA_PARAM_STOMP_D_ROTARY_DISTANCE					0x351F
#define KPA_PARAM_STOMP_D_ROTARY_BALANCE					0x3520
#define KPA_PARAM_STOMP_D_COMPRESSOR_SQUASH					0x3521
#define KPA_PARAM_STOMP_D_GRAPHIC_EQ_BAND_1					0x3522
#define KPA_PARAM_STOMP_D_GRAPHIC_EQ_BAND_2					0x3523
#define KPA_PARAM_STOMP_D_GRAPHIC_EQ_BAND_3					0x3524
#define KPA_PARAM_STOMP_D_GRAPHIC_EQ_BAND_4					0x3525
#define KPA_PARAM_STOMP_D_GRAPHIC_EQ_BAND_5					0x3526
#define KPA_PARAM_STOMP_D_GRAPHIC_EQ_BAND_6					0x3527
#define KPA_PARAM_STOMP_D_GRAPHIC_EQ_BAND_7					0x3528
#define KPA_PARAM_STOMP_D_GRAPHIC_EQ_BAND_8					0x3529
#define KPA_PARAM_STOMP_D_PARAMETRIC_EQ_LOW_GAIN			0x352A
#define KPA_PARAM_STOMP_D_PARAMETRIC_EQ_LOW_FREQUENCY		0x352B
#define KPA_PARAM_STOMP_D_PARAMETRIC_EQ_HIGH_GAIN			0x352C
#define KPA_PARAM_STOMP_D_PARAMETRIC_EQ_HIGH_FREQUENCY		0x352D
#define KPA_PARAM_STOMP_D_PARAMETRIC_EQ_PEAK_GAIN			0x352E
#define KPA_PARAM_STOMP_D_PARAMETRIC_EQ_PEAK_FREQUENCY		0x352F
#define KPA_PARAM_STOMP_D_PARAMETRIC_EQ_PEAK_Q_FACTOR		0x3530
#define KPA_PARAM_STOMP_D_PARAMETRIC_EQ_PEAK_GAIN_2			0x3531
#define KPA_PARAM_STOMP_D_PARAMETRIC_EQ_PEAK_FREQUENCY_2	0x3532
#define KPA_PARAM_STOMP_D_PARAMETRIC_EQ_PEAK_Q_FACTOR_2		0x3533
#define KPA_PARAM_STOMP_D_WAH_PEAK_RANGE					0x3534
//STOMP X (ADDRESS PAGE 56)
#define KPA_PARAM_STOMP_X_TYPE								0x3800
#define KPA_PARAM_STOMP_X_ON_OFF							0x3803
#define KPA_PARAM_STOMP_X_MIX								0x3804
#define KPA_PARAM_STOMP_X_VOLUME							0x3806
#define KPA_PARAM_STOMP_X_STEREO							0x3807
#define KPA_PARAM_STOMP_X_WAH_MANUAL						0x3808
#define KPA_PARAM_STOMP_X_WAH_PEAK							0x3809
#define KPA_PARAM_STOMP_X_WAH_RANGE							0x380A
#define KPA_PARAM_STOMP_X_WAH_PEDAL_MODE					0x380C
#define KPA_PARAM_STOMP_X_WAH_TOUCH_ATTACK					0x380D
#define KPA_PARAM_STOMP_X_WAH_TOUCH_RELEASE					0x380E
#define KPA_PARAM_STOMP_X_WAH_TOUCH_BOOST					0x380F
#define KPA_PARAM_STOMP_X_DISTORTION_SHAPER_DRIVE			0x3810
#define KPA_PARAM_STOMP_X_DISTORTION_BOOSTER_TONE			0x3811
#define KPA_PARAM_STOMP_X_COMPRESSOR_GATE_INTENSITY			0x3812
#define KPA_PARAM_STOMP_X_COMPRESSOR_ATTACK					0x3813
#define KPA_PARAM_STOMP_X_MODULATION_RATE					0x3814
#define KPA_PARAM_STOMP_X_MODULATION_DEPTH					0x3815
#define KPA_PARAM_STOMP_X_MODULATION_FEEDBACK				0x3816
#define KPA_PARAM_STOMP_X_MODULATION_CROSSOVER				0x3817
#define KPA_PARAM_STOMP_X_MODULATION_HYPERCHORUS_AMOUNT		0x3818
#define KPA_PARAM_STOMP_X_MODULATION_MANUAL					0x3819
#define KPA_PARAM_STOMP_X_MODULATION_PHASER_PEAK_SPREAD		0x381A
#define KPA_PARAM_STOMP_X_MODULATION_PHASER_STAGES			0x381B
#define KPA_PARAM_STOMP_X_ROTARY_SPEED_SLOW_FAST			0x381E
#define KPA_PARAM_STOMP_X_ROTARY_DISTANCE					0x381F
#define KPA_PARAM_STOMP_X_ROTARY_BALANCE					0x3820
#define KPA_PARAM_STOMP_X_COMPRESSOR_SQUASH					0x3821
#define KPA_PARAM_STOMP_X_GRAPHIC_EQ_BAND_1					0x3822
#define KPA_PARAM_STOMP_X_GRAPHIC_EQ_BAND_2					0x3823
#define KPA_PARAM_STOMP_X_GRAPHIC_EQ_BAND_3					0x3824
#define KPA_PARAM_STOMP_X_GRAPHIC_EQ_BAND_4					0x3825
#define KPA_PARAM_STOMP_X_GRAPHIC_EQ_BAND_5					0x3826
#define KPA_PARAM_STOMP_X_GRAPHIC_EQ_BAND_6					0x3827
#define KPA_PARAM_STOMP_X_GRAPHIC_EQ_BAND_7					0x3828
#define KPA_PARAM_STOMP_X_GRAPHIC_EQ_BAND_8					0x3829
#define KPA_PARAM_STOMP_X_PARAMETRIC_EQ_LOW_GAIN			0x382A
#define KPA_PARAM_STOMP_X_PARAMETRIC_EQ_LOW_FREQUENCY		0x382B
#define KPA_PARAM_STOMP_X_PARAMETRIC_EQ_HIGH_GAIN			0x382C
#define KPA_PARAM_STOMP_X_PARAMETRIC_EQ_HIGH_FREQUENCY		0x382D
#define KPA_PARAM_STOMP_X_PARAMETRIC_EQ_PEAK_GAIN			0x382E
#define KPA_PARAM_STOMP_X_PARAMETRIC_EQ_PEAK_FREQUENCY		0x382F
#define KPA_PARAM_STOMP_X_PARAMETRIC_EQ_PEAK_Q_FACTOR		0x3830
#define KPA_PARAM_STOMP_X_PARAMETRIC_EQ_PEAK_GAIN_2			0x3831
#define KPA_PARAM_STOMP_X_PARAMETRIC_EQ_PEAK_FREQUENCY_2	0x3832
#define KPA_PARAM_STOMP_X_PARAMETRIC_EQ_PEAK_Q_FACTOR_2		0x3833
#define KPA_PARAM_STOMP_X_WAH_PEAK_RANGE					0x3834
//STOMP MOD (ADDRESS PAGE 58)
#define KPA_PARAM_STOMP_MOD_TYPE							0x3A00
#define KPA_PARAM_STOMP_MOD_ON_OFF							0x3A03
#define KPA_PARAM_STOMP_MOD_MIX								0x3A04
#define KPA_PARAM_STOMP_MOD_VOLUME							0x3A06
#define KPA_PARAM_STOMP_MOD_STEREO							0x3A07
#define KPA_PARAM_STOMP_MOD_WAH_MANUAL						0x3A08
#define KPA_PARAM_STOMP_MOD_WAH_PEAK						0x3A09
#define KPA_PARAM_STOMP_MOD_WAH_RANGE						0x3A0A
#define KPA_PARAM_STOMP_MOD_WAH_PEDAL_MODE					0x3A0C
#define KPA_PARAM_STOMP_MOD_WAH_TOUCH_ATTACK				0x3A0D
#define KPA_PARAM_STOMP_MOD_WAH_TOUCH_RELEASE				0x3A0E
#define KPA_PARAM_STOMP_MOD_WAH_TOUCH_BOOST					0x3A0F
#define KPA_PARAM_STOMP_MOD_DISTORTION_SHAPER_DRIVE			0x3A10
#define KPA_PARAM_STOMP_MOD_DISTORTION_BOOSTER_TONE			0x3A11
#define KPA_PARAM_STOMP_MOD_COMPRESSOR_GATE_INTENSITY		0x3A12
#define KPA_PARAM_STOMP_MOD_COMPRESSOR_ATTACK				0x3A13
#define KPA_PARAM_STOMP_MOD_MODULATION_RATE					0x3A14
#define KPA_PARAM_STOMP_MOD_MODULATION_DEPTH				0x3A15
#define KPA_PARAM_STOMP_MOD_MODULATION_FEEDBACK				0x3A16
#define KPA_PARAM_STOMP_MOD_MODULATION_CROSSOVER			0x3A17
#define KPA_PARAM_STOMP_MOD_MODULATION_HYPERCHORUS_AMOUNT	0x3A18
#define KPA_PARAM_STOMP_MOD_MODULATION_MANUAL				0x3A19
#define KPA_PARAM_STOMP_MOD_MODULATION_PHASER_PEAK_SPREAD	0x3A1A
#define KPA_PARAM_STOMP_MOD_MODULATION_PHASER_STAGES		0x3A1B
#define KPA_PARAM_STOMP_MOD_ROTARY_SPEED_SLOW_FAST			0x3A1E
#define KPA_PARAM_STOMP_MOD_ROTARY_DISTANCE					0x3A1F
#define KPA_PARAM_STOMP_MOD_ROTARY_BALANCE					0x3A20
#define KPA_PARAM_STOMP_MOD_COMPRESSOR_SQUASH				0x3A21
#define KPA_PARAM_STOMP_MOD_GRAPHIC_EQ_BAND_1				0x3A22
#define KPA_PARAM_STOMP_MOD_GRAPHIC_EQ_BAND_2				0x3A23
#define KPA_PARAM_STOMP_MOD_GRAPHIC_EQ_BAND_3				0x3A24
#define KPA_PARAM_STOMP_MOD_GRAPHIC_EQ_BAND_4				0x3A25
#define KPA_PARAM_STOMP_MOD_GRAPHIC_EQ_BAND_5				0x3A26
#define KPA_PARAM_STOMP_MOD_GRAPHIC_EQ_BAND_6				0x3A27
#define KPA_PARAM_STOMP_MOD_GRAPHIC_EQ_BAND_7				0x3A28
#define KPA_PARAM_STOMP_MOD_GRAPHIC_EQ_BAND_8				0x3A29
#define KPA_PARAM_STOMP_MOD_PARAMETRIC_EQ_LOW_GAIN			0x3A2A
#define KPA_PARAM_STOMP_MOD_PARAMETRIC_EQ_LOW_FREQUENCY		0x3A2B
#define KPA_PARAM_STOMP_MOD_PARAMETRIC_EQ_HIGH_GAIN			0x3A2C
#define KPA_PARAM_STOMP_MOD_PARAMETRIC_EQ_HIGH_FREQUENCY	0x3A2D
#define KPA_PARAM_STOMP_MOD_PARAMETRIC_EQ_PEAK_GAIN			0x3A2E
#define KPA_PARAM_STOMP_MOD_PARAMETRIC_EQ_PEAK_FREQUENCY	0x3A2F
#define KPA_PARAM_STOMP_MOD_PARAMETRIC_EQ_PEAK_Q_FACTOR		0x3A30
#define KPA_PARAM_STOMP_MOD_PARAMETRIC_EQ_PEAK_GAIN_2		0x3A31
#define KPA_PARAM_STOMP_MOD_PARAMETRIC_EQ_PEAK_FREQUENCY_2	0x3A32
#define KPA_PARAM_STOMP_MOD_PARAMETRIC_EQ_PEAK_Q_FACTOR_2	0x3A33
#define KPA_PARAM_STOMP_MOD_WAH_PEAK_RANGE					0x3A34
//DELAY (ADDRESS PAGE 74)
#define KPA_PARAM_DELAY_TYPE				0x4A00
#define KPA_PARAM_DELAY_ON_OFF_CUTS_TAIL	0x4A02
#define KPA_PARAM_DELAY_MIX					0x4A03
#define KPA_PARAM_DELAY_VOLUME				0x4A04
#define KPA_PARAM_DELAY_TIME				0x4A05
#define KPA_PARAM_DELAY_RATIO				0x4A06
#define KPA_PARAM_DELAY_CLOCK_LEFT			0x4A07
#define KPA_PARAM_DELAY_CLOCK_RIGHT			0x4A08
#define KPA_PARAM_DELAY_FEEDBACK			0x4A09
#define KPA_PARAM_DELAY_BANDWIDTH			0x4A0A
#define KPA_PARAM_DELAY_CENTER_FREQUENCY	0x4A0B
#define KPA_PARAM_DELAY_MODULATION			0x4A0C
#define KPA_PARAM_DELAY_ON_OFF_KEEPS_TAIL	0x4A0D
//REVERB (ADDRESS PAGE 75)
#define KPA_PARAM_REVERB_TYPE				0x4B00
#define KPA_PARAM_REVERB_ON_OFF_CUTS_TAIL	0x4B02
#define KPA_PARAM_REVERB_MIX				0x4B03
#define KPA_PARAM_REVERB_VOLUME				0x4B04
#define KPA_PARAM_REVERB_DEL_REV_BALANCE	0x4B05
#define KPA_PARAM_REVERB_TIME				0x4B06
#define KPA_PARAM_REVERB_DAMPING			0x4B07
#define KPA_PARAM_REVERB_BANDWIDTH			0x4B08
#define KPA_PARAM_REVERB_CENTER_FREQUENCY	0x4B09
#define KPA_PARAM_REVERB_PRE_DELAY			0x4B0A
#define KPA_PARAM_REVERB_ON_OFF_KEEPS_TAIL	0x4B0B
//SYSTEM / GLOBAL (ADDRESS PAGE 127)
#define KPA_PARAM_SYSTEM_GLOBAL_MAIN_OUTPUT_VOLUME			0x7F00 
#define KPA_PARAM_SYSTEM_GLOBAL_HEADPHONE_OUTPUT_VOLUME		0x7F01
#define KPA_PARAM_SYSTEM_GLOBAL_MONITOR_OUTPUT_VOLUME		0x7F02
#define KPA_PARAM_SYSTEM_GLOBAL_DIRECT_OUTPUT_VOLUME		0x7F03
#define KPA_PARAM_SYSTEM_GLOBAL_S_PDIF_INPUT_ENABLE			0x7F0B
#define KPA_PARAM_SYSTEM_GLOBAL_MAIN_OUTPUT_EQ_BASS			0x7F0C
#define KPA_PARAM_SYSTEM_GLOBAL_MAIN_OUTPUT_EQ_MIDDLE		0x7F0D
#define KPA_PARAM_SYSTEM_GLOBAL_MAIN_OUTPUT_EQ_TREBLE		0x7F0E
#define KPA_PARAM_SYSTEM_GLOBAL_MAIN_OUTPUT_EQ_PRESENCE		0x7F0F
#define KPA_PARAM_SYSTEM_GLOBAL_MONITOR_OUTPUT_EQ_BASS		0x7F11
#define KPA_PARAM_SYSTEM_GLOBAL_MONITOR_OUTPUT_EQ_MIDDLE	0x7F12
#define KPA_PARAM_SYSTEM_GLOBAL_MONITOR_OUTPUT_EQ_TREBLE	0x7F13
#define KPA_PARAM_SYSTEM_GLOBAL_MONITOR_OUTPUT_EQ_PRESENCE	0x7F14

#define KPA_PARAM_FLOORBOARD_NAME	0x7F7F
#define KPA_PARAM_FIRMWARE_VERSION	0x7F7E
#define KPA_PARAM_EXT_DEVICE_NAME	0x00000C1A40
#define KPA_PARAM_EXT_DEVICE_OWNER	0x00000C1A41

//Others parameters from set1/set2
#define KPA_PARAM_TAP_EVENT						0x7C00
#define KPA_PARAM_CURRENT_TUNING				0x7C0F
#define KPA_PARAM_CURRENT_NOTE					0x7D54
#define KPA_PARAM_SELECTED_MAIN_MODE			0x7F7D // (0=BROWSE/1=PERFORM)
#define KPA_PARAM_SELECTED_MODE_CHICKEN_HEAD	0x7F7E
#define KPA_PARAM_EXT_SLOT_1_ENABLED			0x00010000
#define KPA_PARAM_EXT_SLOT_2_ENABLED			0x00010001
#define KPA_PARAM_EXT_SLOT_3_ENABLED			0x00010002
#define KPA_PARAM_EXT_SLOT_4_ENABLED			0x00010003
#define KPA_PARAM_EXT_SLOT_5_ENABLED			0x00010004

#define KPA_PARAM_RIG_NAME					0x0001
#define KPA_PARAM_EXT_PERFORMANCE_NAME		0x00010000
#define KPA_PARAM_EXT_SLOT_1_NAME			0x00010001
#define KPA_PARAM_EXT_SLOT_2_NAME			0x00010002
#define KPA_PARAM_EXT_SLOT_3_NAME			0x00010003
#define KPA_PARAM_EXT_SLOT_4_NAME			0x00010004
#define KPA_PARAM_EXT_SLOT_5_NAME			0x00010005

//parameters address for KPA_FUNCTION_SYS_COMMUNICATION function
#define KPA_PARAM_BEACON					0x40
#define KPA_PARAM_ACTIVE_SENSING_SIGNAL		0x7F


#endif /* kpa_h_ */