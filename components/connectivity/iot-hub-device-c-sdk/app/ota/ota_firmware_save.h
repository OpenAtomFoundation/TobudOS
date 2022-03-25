/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
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
 * @file ota_firmware_save.h
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-10-20
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-10-20 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_APP_OTA_OTA_FIRMWARE_SAVE_H_
#define IOT_HUB_DEVICE_C_SDK_APP_OTA_OTA_FIRMWARE_SAVE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

/**
 * @brief Read ota break point from file.
 *
 * @param[out] buf data to read
 * @param[in] buf_len data buffer len
 * @return > 0 for read len, -1 for fail
 */
int ota_break_point_read(uint8_t *buf, uint32_t buf_len);

/**
 * @brief Write ota break point to file.
 *
 * @param[in] data break point data to write
 * @param[in] data_len data length
 * @return 0 for success
 */
int ota_break_point_write(const uint8_t *data, uint32_t data_len);

/**
 * @brief Read firmware from file.
 *
 * @param[out] buf data to read
 * @param[in] buf_len data buffer len
 * @param[in] offset offset of file
 * @return > 0 for read len, -1 for fail
 */
int ota_firmware_read(uint8_t *buf, uint32_t buf_len, uint32_t offset);

/**
 * @brief Write firmware to file.
 *
 * @param[in] data firmware data to write
 * @param[in] data_len data length
 * @param[in] offset offset of file
 * @return 0 for success
 */
int ota_firmware_write(uint8_t *data, uint32_t data_len, uint32_t offset);

/**
 * @brief Finish write firmware.
 *
 * @param[in] total_len total length of firmware
 * @return 0 for success
 */
int ota_firmware_finish(uint32_t total_len);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_APP_OTA_OTA_FIRMWARE_SAVE_H_
