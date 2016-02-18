/*
 * BJ Devices Travel Box series midi controller library
 * @file	midi.h
 * 
 * @brief	Send and receive midi messages. Running status is not supported in this version,
 *			running status messages will ignored!!				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */ 


#ifndef midi_h_
#define midi_h_

#include <stdint.h>
#include <stdbool.h>

/*
 * @brief	UART and maintenance initialization 
 */
void initMidi();

/*
 * @brief	Send program change midi message
 * @param	progNum - midi program number
 * @param	chanNum - midi channel  
 */
void midiSendProgramChange(uint8_t progNum, uint8_t chanNum);

/*
 * @brief	Send control change midi message
 * @param	ctrlNum - midi continuous controller number
 * @param	val - controller value
 * @param	chanNum - midi channel   
 */
void midiSendControlChange(uint8_t ctrlNum, uint8_t val, uint8_t chanNum);

/*
 * @brief	Send system exclusive midi message.  
 *			Start byte (F0 hex) and an end byte (F7 hex) will automatically included and should not passed in payload 
 * @param	length - data length
 * @param	data - payload  
 */
void midiSendSysEx(uint16_t length, uint8_t* data);

/*
 * @brief	Send system exclusive midi message with manufacturer id.  
 *			Start byte (F0 hex) and an end byte (F7 hex) will automatically included and should not passed in payload 
 * @param	manfId - 3 byte format manufacturer id
 *			Most significant byte [31:25] will ignored
 *			[24:16] - manufacturer id byte 1
 *			[15:8] - manufacturer id byte 2
 *			[7:0] - manufacturer id byte 3
 * @param	length - data length without manufacturer id
 * @param	data - payload  without manufacturer id
 */
void midiSendSysExManfId(uint32_t manfId, uint16_t length, uint8_t* data);

/*
 * @brief	Read UART buffer and check midi messages. 
 *			Should be invoked in main loop. Running status is not supported in this version,
 *			running status messages will ignored!!
 * @return	true if valid midi message received
 */
bool midiRead();

/*
 * @brief	Register callback will invoked if control change message received
 */
void midiRegisterControlChangeCallback(void (*callback)(uint8_t channel, uint8_t ccNum, uint8_t ccVal));

/*
 * @brief	Register callback will invoked if program change message received
 */
void midiRegisterProgramChangeCallback(void (*callback)(uint8_t channel, uint8_t program));

/*
 * @brief	Register callback will invoked if system exclusive message received
 */
void midiRegisterSysExCallback(void (*callback)(uint16_t length));

/*
 * @brief	Register callback will invoked if active sense message received
 */
void midiRegisterActiveSenseCallback(void (*callback)(void));

/*
 * @return	Midi channel number in last input midi message
*/
uint8_t midiGetChannelNumber(); 

/*
 * @return	Midi message type
*/
uint8_t midiGetMessageType(); 

/*
 * @return	Midi program number in last input program change midi message
*/
uint8_t midiGetProgramNumber();

/*
 * @return	Midi continuous controller number in last input midi control change message
*/
uint8_t midiGetControllerNumber();

/*
 * @return	Midi continuous controller value in last input midi control change message
*/
uint8_t midiGetControllerValue();

/*
 * @return	System exclusive payload length from any SysEx message
*/
uint16_t midiGetSysExLength(uint8_t* sysEx);

/*
 * @return	System exclusive payload length from last SysEx message
*/
uint16_t midiGetLastSysExLength();

/*
 * @return	Pointer to system exclusive payload data
*/
uint8_t* midiGetLastSysExData();

/*
 * @return	3 byte format manufacturer id from SysEx message. 
 * 			Most significant byte in return value is always 0,
 *			[24:16] - manufacturer id byte 1
 *			[15:8] - manufacturer id byte 2
 *			[7:0] - manufacturer id byte 3
 */
uint32_t midiGetSysExManufacturerId(uint8_t* sysEx);


//Midi statuses
#define UNKNOWN_STATUS	0x00 //Unknown
#define PC_STATUS		0xC0 //Program change midi message status
#define CC_STATUS		0xB0 //Control change midi message status
#define SYSEX_STATUS	0xF0 //System exclusive
#define ACTIVE_SENSE	0xE0 //Active sense

#define SYSEX_END		0xF7 //End of System exclusive message

//midi buffer size in bytes
#define MIDI_BUFFER_SIZE	256
 

#endif /* midi_h_ */