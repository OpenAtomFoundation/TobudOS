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
 * @file ota_firmware_save.c
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

#include "ota_firmware_save.h"

#include "utils_log.h"

#define OTA_FILE_PATH             "./app_ota_fw.bin"
#define OTA_BREAK_POINT_FILE_PATH "./break_point.dat"

/**
 * @brief Read ota break point from file.
 *
 * @param[out] buf data to read
 * @param[in] buf_len data buffer len
 * @return > 0 for read len, -1 for fail
 */
int ota_break_point_read(uint8_t *buf, uint32_t buf_len)
{
    FILE *fp = fopen(OTA_BREAK_POINT_FILE_PATH, "rb");
    if (!fp) {
        fp = fopen(OTA_BREAK_POINT_FILE_PATH, "wb+");
        if (!fp) {
            Log_e("create file failed");
            return -1;
        }
    }

    int ret = fread(buf, 1, buf_len, fp);
    fclose(fp);
    return ret;
}

/**
 * @brief Write ota break point to file.
 *
 * @param[in] data break point data to write
 * @param[in] data_len data length
 * @return 0 for success
 */
int ota_break_point_write(const uint8_t *data, uint32_t data_len)
{
    FILE *fp = fopen(OTA_BREAK_POINT_FILE_PATH, "wb+");
    if (!fp) {
        Log_e("open file failed");
        return -1;
    }
    fwrite(data, 1, data_len, fp);
    fclose(fp);
    return 0;
}

/**
 * @brief Read firmware from file.
 *
 * @param[out] buf data to read
 * @param[in] buf_len data buffer len
 * @param[in] offset offset of file
 * @return > 0 for read len, -1 for fail
 */
int ota_firmware_read(uint8_t *buf, uint32_t buf_len, uint32_t offset)
{
    int   ret = -1;
    FILE *fp  = fopen(OTA_FILE_PATH, "rb");
    if (!fp) {
        fp = fopen(OTA_FILE_PATH, "wb+");
        if (!fp) {
            Log_e("create file failed");
            return -1;
        }
    }
    ret = fseek(fp, offset, SEEK_SET);
    if (!ret) {
        ret = fread(buf, 1, buf_len, fp);
    }
    fclose(fp);
    return ret;
}

/**
 * @brief Write firmware to file.
 *
 * @param[in] data firmware data to write
 * @param[in] data_len data length
 * @param[in] offset offset of file
 * @return 0 for success
 */
int ota_firmware_write(uint8_t *data, uint32_t data_len, uint32_t offset)
{
    int   ret = -1;
    FILE *fp  = fopen(OTA_FILE_PATH, "rb+");
    if (!fp) {
        fp = fopen(OTA_FILE_PATH, "wb+");
        if (!fp) {
            Log_e("create file failed");
            return -1;
        }
    }
    ret = fseek(fp, offset, SEEK_SET);
    if (!ret) {
        fwrite(data, 1, data_len, fp);
    }
    fclose(fp);
    return 0;
}

/**
 * @brief Finish write firmware.
 *
 * @param[in] total_len total length of firmware
 * @return 0 for success
 */
int ota_firmware_finish(uint32_t total_len)
{
    return 0;
}
