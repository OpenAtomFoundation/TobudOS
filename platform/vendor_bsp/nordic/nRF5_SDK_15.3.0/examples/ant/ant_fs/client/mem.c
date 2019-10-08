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
#include "mem.h"
#include <string.h>

#define NRF_LOG_MODULE_NAME MEM

#if MEM_WRITE_LOG_ENABLED
#define NRF_LOG_LEVEL       MEM_WRITE_LOG_LEVEL
#define NRF_LOG_INFO_COLOR  MEM_WRITE_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR MEM_WRITE_DEBUG_COLOR
#else //MEM_WRITE_LOG_ENABLED
#define NRF_LOG_LEVEL       0
#endif //MEM_WRITE_LOG_ENABLED
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define MEM_DIR_SIZE          9u                     /**< Example directory size. */
#define MEM_DIR_INVALID_INDEX 0xFFFFu                /**< Defined invalid index value. */

// Sample constant directory structure.
typedef struct
{
    antfs_dir_header_t header;                       /**< Directory header. */
    antfs_dir_struct_t directory_file[MEM_DIR_SIZE]; /**< Array of directory entry structures. */
} directory_file_t;

// Sample constant directory.
static const directory_file_t m_directory =
{
    {
        ANTFS_DIR_STRUCT_VERSION,                             // Version 1, length of each subsequent entry = 16 bytes, system time not used.
        sizeof(antfs_dir_struct_t),
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        {9, 0,        4, 0, 0, 0xA0, 0x00000000, 0x000296BC}, // Index 9, data type 0, identifier 4, read and erase, 0 bytes in length.
        {7, 1,        0, 0, 0, 0x60, 0x00000000, 0x00007E71}, // Index 7, data type 1, identifier 0, write and erase, 0 bytes in length.
        {8, 1,        1, 0, 0, 0x60, 0x00000000, 0x00007E73}, // Index 8, data type 1, identifier 1, write and erase, 0 bytes in length.
        {1, 0x80,     1, 0, 0, 0x80, 0x000000DA, 0x00007E63}, // Index 1, data type 128, identifier 1, read and write, 218 bytes in length.
        {2, 0x80,     2, 0, 0, 0xC0, 0x0000014E, 0x00007E65}, // Index 2, data type 128, identifier 2, read and write, 334 bytes in length.
        {3, 0x80,     3, 0, 0, 0xC0, 0x0000037B, 0x00007E67}, // Index 3, data type 128, identifier 3, read and write, 891 bytes in length.
        {4, 0x80,     3, 0, 0, 0xC0, 0x0000037B, 0x00007E6A}, // Index 4, data type 128, identifier 3, read and write, 891 bytes in length.
        {5, 0x80,     3, 0, 0, 0xF0, 0x0001D4C0, 0x00007E6C}, // Index 5, data type 128, identifier 3, read, write, erase, archive, 120000 bytes in length.
        {6, 0x80,  0x0A, 0, 0, 0x80, 0x00000000, 0x00007E6F}  // Index 6, data type 128, identifier 10, read-only, 0 bytes in length.
    }
};


/**@brief Function for getting an array index of a particular directory structure matching the
 *        requested file index.
 *
 * @param[in] index The file index identifier used for lookup.
 *
 * @return Upon success the array index, otherwise MEM_DIR_INVALID_INDEX.
 */
static uint32_t index_lookup(uint32_t index)
{
    if (index)
    {
        uint32_t idx;
        for (idx = 0; idx < MEM_DIR_SIZE; idx++)
        {
            if (m_directory.directory_file[idx].data_file_index == index)
            {
                return idx;
            }
        }

    }

    return MEM_DIR_INVALID_INDEX;
}


bool mem_file_write(uint16_t index, uint32_t offset, const void * p_data, uint32_t size)
{
    if ((index < MEM_DIR_SIZE + 1u) && (index > 0))
    {
        const uint32_t array_index = index_lookup(index);
        if (array_index != MEM_DIR_INVALID_INDEX)
        {
            NRF_LOG_HEXDUMP_INFO(p_data, size);
            return true;
        }
        else
        {
            return false;
        }

    }

    return false;
}


void mem_file_read(uint16_t index, uint32_t offset, void * p_data, uint32_t size)
{
    if (index == 0)
    {
        // Directory.

        uint8_t * p_directory = (uint8_t*) &m_directory;
        memcpy((uint8_t*)p_data, p_directory + offset, size);
    }
    else
    {
        // Fake data (no actual files stored in memory for this reference design).

        uint32_t idx;
        uint8_t* p_data_access = (uint8_t*)p_data;
        for (idx = 0; idx < size; idx++)
        {
            p_data_access[idx] = ((uint8_t)(offset + idx));
        }
    }
}


bool mem_file_erase(uint16_t index)
{
    if ((index < MEM_DIR_SIZE + 1u) && (index > 0))
    {
        const uint32_t array_index = index_lookup(index);
        if (array_index != MEM_DIR_INVALID_INDEX)
        {
            // Erase file. This function is implementation specific. There are no actual files in
            // memory for this reference design, so there is nothing to erase.
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}


bool mem_file_info_get(uint16_t index, antfs_dir_struct_t * p_file_info)
{
    if (index < (MEM_DIR_SIZE + 1u))
    {
        if (index == 0)
        {
            // Requested directory.

            // Set can download flag.
            p_file_info->general_flags = 0x80u;

            p_file_info->file_size_in_bytes =
                // Header + directory structures.
                (uint32_t)(MEM_DIR_SIZE + 1u) * sizeof(antfs_dir_struct_t);

            // Directory is index 0
            p_file_info->data_file_index = 0;
        }
        else
        {
            // Requested a file.

            const uint32_t array_index = index_lookup(index);
            memcpy(p_file_info,
                   &m_directory.directory_file[array_index],
                   sizeof(antfs_dir_struct_t));
        }

        return true;
    }

    return false;
}
