/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2012
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *    1) Redistributions of source code must retain the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer.
 *
 *    2) Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 *    3) Neither the name of Garmin nor the names of its
 *       contributors may be used to endorse or promote products
 *       derived from this software without specific prior
 *       written permission.
 *
 * The following actions are prohibited:
 *
 *    1) Redistribution of source code containing the ANT+ Network
 *       Key. The ANT+ Network Key is available to ANT+ Adopters.
 *       Please refer to http://thisisant.com to become an ANT+
 *       Adopter and access the key. 
 *
 *    2) Reverse engineering, decompilation, and/or disassembly of
 *       software provided in binary form under this license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE HEREBY
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; DAMAGE TO ANY DEVICE, LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE. SOME STATES DO NOT ALLOW 
 * THE EXCLUSION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO THE
 * ABOVE LIMITATIONS MAY NOT APPLY TO YOU.
 *
 */
/**@file
 * @brief The memory simulator interface.
 * This file is based on implementation originally made in August 2012 by Garmin Canada Inc. 
 * (former Dynastream Innovations Inc.)
 * @defgroup ant_fs_client_main ANT-FS client device simulator
 * @{
 * @ingroup nrf_ant_fs_client
 *
 * @brief The ANT-FS client device simulator.
 *
 */

#ifndef MEM_H__
#define MEM_H__

#include <stdint.h>
#include <stdbool.h>
#include "antfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief Function for writing data to file system.
 *
 * @param[in] index        The file index.
 * @param[in] offset       The write data offset.
 * @param[in] p_data       The data to be written.
 * @param[in] size         The number of bytes to be written.
 *
 * @retval true Operation success.
 * @retval true Operation failure.
 */
bool mem_file_write(uint16_t index, uint32_t offset, const void * p_data, uint32_t size);

/**@brief Function for reading data from file system.
 *
 * @param[in] index        The file index, 0 for directory.
 * @param[in] offset       The read data offset.
 * @param[out] p_data      The buffer where data is read.
 * @param[in] size         The number of bytes to read.
 */
void mem_file_read(uint16_t index, uint32_t offset, void * p_data, uint32_t size);

/**@brief Function for erasing file from file system.
 *
 * @param[in] index        The file index.
 *
 * @retval true Operation success.
 * @retval true Operation failure.
 */
bool mem_file_erase(uint16_t index);

/**@brief Function for retrieving file information from directory.
 *
 * @param[in] index        The file index, which information to retrieve.
 * @param[out] p_file_info The container where information is read.
 *
 * @retval true Operation success.
 * @retval true Operation failure.
 */
bool mem_file_info_get(uint16_t index, antfs_dir_struct_t * p_file_info);


#ifdef __cplusplus
}
#endif

#endif  // MEM_H__

/**
 *@}
 **/
