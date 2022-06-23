/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2022 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file HAL_File_tencentos_tiny.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-04-07
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-04-07 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <stdio.h>
#include <stdlib.h>

#include "qcloud_iot_platform.h"

/**
 * @brief Functions for saving file into NVS(files/FLASH)
 * @param[in] filename file path name
 * @param[in] buf source need write buffer
 * @param[in] write_len length of file to write
 * @return length of data save when success, or 0 for failure
 */
size_t HAL_File_Write(const char *filename, const void *buf, size_t write_len, size_t offset)
{
    return write_len;
}

/**
 * @brief Functions for reading file from NVS(files/FLASH)
 * @param[in] filename file path name
 * @param[in] buf destination log buffer
 * @param[in] read_len length to read
 * @return length of data read when success, or 0 for failure
 */
size_t HAL_File_Read(const char *filename, void *buf, size_t read_len, size_t offset)
{
    return 0;
}

/**
 * @brief Functions for deleting file in NVS(files/FLASH).
 * @param[in] filename file path name
 * @return 0 when success
 */
int HAL_File_Del(const char *filename)
{
    return 0;
}

/**
 * @brief Functions for reading the size of file in NVS(files/FLASH).
 * @param[in] filename file path name
 * @return 0 when nothing exist
 */
size_t HAL_File_GetSize(const char *filename)
{
    return 0;
}
