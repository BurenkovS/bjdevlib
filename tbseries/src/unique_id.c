 /*
 * BJ Devices Travel Box series midi controller library
 * @file	unique_id.h
 * 
 * @brief	Read unique ID of controller. ID is available only in gen2 devices via 1-wire from DS2411 IC.
 *			see details on BJ Devices WIKI : 
 *			http://bjdevices.com/wiki/doku.php?id=difference_between_new_and_old_versions_of_tb_series_devices   
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "pinout.h"
#include "onewire.h"
#include "crc8.h"


#include <stdint.h>
#include <stdbool.h>

void initUniqId()
{
	ow_set_bus(&SERIAL_NIMBER_IC_IN,&SERIAL_NIMBER_IC_PORT,&SERIAL_NIMBER_IC_DDR,SERIAL_NIMBER_IC_PIN);
}

bool uniqIdGet(uint8_t* buffer)
{
	ow_byte_wr(OWI_READ_ROM);

	uint8_t i;
	for (i = 0; i < 8; ++i)
		buffer[i] = ow_byte_rd();

	if(buffer[OW_ROMCODE_SIZE-1] == crc8(buffer, OW_ROMCODE_SIZE-1))
		return true;
	else
		return false;
}
