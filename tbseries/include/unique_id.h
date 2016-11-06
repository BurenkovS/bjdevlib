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


#ifndef UNIQUEID_H_
#define UNIQUEID_H_

#include <stdint.h>
#include <stdbool.h>

/*
 * @brief	Initialization of 1-wire interface
 */
void initUniqId();

/*
 * @brief	Read ID from DS2411
 * @param 	*buffer - pointer to buffer for read data. 8 bytes is required
 * @return	true if ID was successfully read and CRC matched, false otherwise
 */
bool uniqIdGet(uint8_t* buffer);



#endif /* UNIQUEID_H_ */