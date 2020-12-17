/*
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "ble_qiot_log.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define HEX_DUMP_BYTE_PER_LINE 16

e_ble_qiot_log_level g_log_level = BLE_QIOT_LOG_LEVEL_INFO;

void ble_qiot_set_log_level(e_ble_qiot_log_level level)
{
    g_log_level = level;
    return;
}

#if (0 == BLE_QIOT_USER_DEFINE_HEDUMP)
void ble_qiot_log_hex(e_ble_qiot_log_level level, const char *hex_name, const char *data, uint32_t data_len)
{
    char buf[HEX_DUMP_BYTE_PER_LINE * 5] = {0};
    int  line_count = 0, line = 0, byte = 0, rest = 0, start_byte = 0;

    if (g_log_level < level) return;

    line_count = data_len / HEX_DUMP_BYTE_PER_LINE;
    if (data_len % HEX_DUMP_BYTE_PER_LINE) {
        line_count += 1;
    }

    ble_qiot_log_raw("\r\nble qiot dump: %s, length: %d\r\n", hex_name, data_len);
    ble_qiot_log_raw(" 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15\r\n");
    ble_qiot_log_raw("===============================================\r\n");
    for (line = 0; line < line_count; line++) {
        start_byte = line * HEX_DUMP_BYTE_PER_LINE;
        if (data_len - start_byte < HEX_DUMP_BYTE_PER_LINE) {
            rest = data_len % HEX_DUMP_BYTE_PER_LINE;
        } else {
            rest = HEX_DUMP_BYTE_PER_LINE;
        }

        for (byte = 0; byte < HEX_DUMP_BYTE_PER_LINE; byte++) {
            if (byte < rest) {
                sprintf(&buf[byte * 3], "%02X ", data[start_byte + byte]);
            } else {
                sprintf(&buf[byte * 3], "   ");
            }
        }

        sprintf(&buf[HEX_DUMP_BYTE_PER_LINE * 3], "| ");
        for (byte = 0; byte < rest; byte++) {
            if (data[start_byte + byte] >= ' ' && data[start_byte + byte] <= '~') {
                buf[HEX_DUMP_BYTE_PER_LINE * 3 + 2 + byte] = data[start_byte + byte];
            } else {
                buf[HEX_DUMP_BYTE_PER_LINE * 3 + 2 + byte] = '.';
            }
        }
        sprintf(&buf[HEX_DUMP_BYTE_PER_LINE * 3 + 2 + rest], "\r\n");

        ble_qiot_log_raw("%s", buf);  // do not use printf(buf), that cause '%' transfer next character
    }
    ble_qiot_log_raw("\r\n");
}
#endif // BLE_QIOT_USER_DEFINE_HEDUMP

#ifdef __cplusplus
}
#endif
