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

#include "ble_qiot_llsync_device.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ble_qiot_common.h"
#include "ble_qiot_config.h"
#include "ble_qiot_export.h"
#include "ble_qiot_hmac.h"
#include "ble_qiot_import.h"
#include "ble_qiot_log.h"
#include "ble_qiot_param_check.h"
#include "ble_qiot_utils_base64.h"
#include "ble_qiot_md5.h"

#define BLE_GET_EXPIRATION_TIME(_cur_time) ((_cur_time) + BLE_EXPIRATION_TIME)

static ble_core_data             sg_core_data;                // ble data storage in flash
static ble_device_info           sg_device_info;              // device info storage in flash
static e_llsync_bind_state       sg_llsync_bind_state;        // llsync bind state in used
static e_llsync_connection_state sg_llsync_connection_state;  // llsync connection state in used
static e_ble_connection_state    sg_ble_connection_state;     // ble connection state in used

static int memchk(const uint8_t *buf, int len)
{
    int i = 0;

    for (i = 0; i < len; i++) {
        if (buf[i] != 0xFF) {
            return 1;
        }
    }

    return 0;
}

ble_qiot_ret_status_t ble_init_flash_data(void)
{
    if (sizeof(sg_core_data) !=
        ble_read_flash(BLE_QIOT_RECORD_FLASH_ADDR, (char *)&sg_core_data, sizeof(sg_core_data))) {
        ble_qiot_log_e("read flash error");
        return BLE_QIOT_RS_ERR_FLASH;
    }
    if (0 == memchk((const uint8_t *)&sg_core_data, sizeof(sg_core_data))) {
        memset(&sg_core_data, 0, sizeof(sg_core_data));
    }

    if (0 != ble_get_mac(sg_device_info.mac)) {
        ble_qiot_log_e("get mac error");
        return BLE_QIOT_RS_ERR_FLASH;
    }
    if (0 != ble_get_product_id(sg_device_info.product_id)) {
        ble_qiot_log_e("get product id error");
        return BLE_QIOT_RS_ERR_FLASH;
    }
    if (0 != ble_get_psk(sg_device_info.psk)) {
        ble_qiot_log_e("get secret key error");
        return BLE_QIOT_RS_ERR_FLASH;
    }
    if (0 == ble_get_device_name(sg_device_info.device_name)) {
        ble_qiot_log_e("get device name error");
        return BLE_QIOT_RS_ERR_FLASH;
    }
    llsync_bind_state_set((e_llsync_bind_state)sg_core_data.bind_state);

    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "core_data", (char *)&sg_core_data, sizeof(sg_core_data));
    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "device_info", (char *)&sg_device_info, sizeof(sg_device_info));

    return BLE_QIOT_RS_OK;
}

static ble_qiot_ret_status_t ble_write_core_data(ble_core_data *core_data)
{
    memcpy(&sg_core_data, core_data, sizeof(ble_core_data));
    if (sizeof(ble_core_data) !=
        ble_write_flash(BLE_QIOT_RECORD_FLASH_ADDR, (char *)&sg_core_data, sizeof(ble_core_data))) {
        ble_qiot_log_e("write core failed");
        return BLE_QIOT_RS_ERR_FLASH;
    }

    return BLE_QIOT_RS_OK;
}

void llsync_bind_state_set(e_llsync_bind_state new_state)
{
    ble_qiot_log_d("bind state: %d ---> %d", sg_llsync_bind_state, new_state);
    sg_llsync_bind_state = new_state;
}

e_llsync_bind_state llsync_bind_state_get(void)
{
    return sg_llsync_bind_state;
}

void llsync_connection_state_set(e_llsync_connection_state new_state)
{
    ble_qiot_log_d("connection state: %d ---> %d", sg_llsync_connection_state, new_state);
    sg_llsync_connection_state = new_state;
}

e_llsync_connection_state llsync_connection_state_get(void)
{
    return sg_llsync_connection_state;
}

void ble_connection_state_set(e_ble_connection_state new_state)
{
    sg_ble_connection_state = new_state;
}

e_ble_connection_state ble_connection_state_get(void)
{
    return sg_ble_connection_state;
}

bool ble_is_connected(void)
{
    return sg_ble_connection_state == E_BLE_CONNECTED;
}

bool llsync_is_connected(void)
{
    return sg_llsync_connection_state == E_LLSYNC_CONNECTED;
}

// [1byte bind state] + [6 bytes mac] + [8bytes identify string]/[10 bytes product id]
int ble_get_my_broadcast_data(char *out_buf, int buf_len)
{
    POINTER_SANITY_CHECK(out_buf, BLE_QIOT_RS_ERR_PARA);
    BUFF_LEN_SANITY_CHECK(buf_len, BLE_BIND_IDENTIFY_STR_LEN + BLE_QIOT_MAC_LEN + 1, BLE_QIOT_RS_ERR_PARA);

    int ret_len = 0;

    out_buf[ret_len] = sg_llsync_bind_state & LLSYNC_BIND_STATE_MASK;
    out_buf[ret_len] |= BLE_QIOT_LLSYNC_PROTOCOL_VERSION << 4;
    ret_len++;

    if (E_LLSYNC_BIND_SUCC == (sg_llsync_bind_state & LLSYNC_BIND_STATE_MASK)) {
        uint8_t md5_in_buf[128]              = {0};
        uint8_t md5_in_len                   = 0;
        uint8_t md5_out_buf[MD5_DIGEST_SIZE] = {0};

        // 1 bytes state + 8 bytes device identify_str + 8 bytes identify_str
        memcpy((char *)md5_in_buf, sg_device_info.product_id, sizeof(sg_device_info.product_id));
        md5_in_len += sizeof(sg_device_info.product_id);
        memcpy((char *)md5_in_buf + md5_in_len, sg_device_info.device_name, strlen(sg_device_info.device_name));
        md5_in_len += strlen(sg_device_info.device_name);
        utils_md5(md5_in_buf, md5_in_len, md5_out_buf);
        for (int i = 0; i < MD5_DIGEST_SIZE / 2; i++) {
            out_buf[i + ret_len] = md5_out_buf[i] ^ md5_out_buf[i + MD5_DIGEST_SIZE / 2];
        }
        ret_len += MD5_DIGEST_SIZE / 2;
        memcpy(out_buf + ret_len, sg_core_data.identify_str, sizeof(sg_core_data.identify_str));
        ret_len += sizeof(sg_core_data.identify_str);
    } else {
        // 1 bytes state + 6 bytes mac + 10 bytes product id
        memcpy(out_buf + ret_len, sg_device_info.mac, BLE_QIOT_MAC_LEN);
        ret_len += BLE_QIOT_MAC_LEN;
        memcpy(out_buf + ret_len, sg_device_info.product_id, sizeof(sg_device_info.product_id));
        ret_len += sizeof(sg_device_info.product_id);
    }
    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "broadcast", out_buf, ret_len);

    return ret_len;
}

int ble_bind_get_authcode(const char *bind_data, uint16_t data_len, char *out_buf, uint16_t buf_len)
{
    POINTER_SANITY_CHECK(bind_data, BLE_QIOT_RS_ERR_PARA);
    BUFF_LEN_SANITY_CHECK(data_len, (int32_t)sizeof(ble_bind_data), BLE_QIOT_RS_ERR_PARA);
    POINTER_SANITY_CHECK(out_buf, BLE_QIOT_RS_ERR_PARA);
    BUFF_LEN_SANITY_CHECK(buf_len, SHA1_DIGEST_SIZE + BLE_QIOT_DEVICE_NAME_LEN, BLE_QIOT_RS_ERR_PARA);

    char    out_sign[SHA1_DIGEST_SIZE]       = {0};
    char    sign_info[80]                    = {0};
    int     sign_info_len                    = 0;
    int     time_expiration                  = 0;
    int     nonce                            = 0;
    int     ret_len                          = 0;
    uint8_t secret[BLE_QIOT_PSK_LEN / 4 * 3] = {0};
    int     secret_len                       = 0;

    // if the pointer "char *bind_data" is not aligned with 4 byte, in some cpu convert it to
    // pointer "ble_bind_data *" work correctly, but some cpu will get wrong value, or cause
    // other "Unexpected Error". Here copy it to a local variable make sure aligned with 4 byte.
    ble_bind_data bind_data_aligned;
    memcpy(&bind_data_aligned, bind_data, sizeof(ble_bind_data));

    nonce           = NTOHL(bind_data_aligned.nonce);
    time_expiration = BLE_GET_EXPIRATION_TIME(NTOHL(bind_data_aligned.timestamp));

    // [10 bytes product_id] + [x bytes device name] + ';' + [4 bytes nonce] + ';' + [4 bytes timestamp]
    memcpy(sign_info, sg_device_info.product_id, sizeof(sg_device_info.product_id));
    sign_info_len += sizeof(sg_device_info.product_id);
    memcpy(sign_info + sign_info_len, sg_device_info.device_name, strlen(sg_device_info.device_name));
    sign_info_len += strlen(sg_device_info.device_name);
    snprintf(sign_info + sign_info_len, sizeof(sign_info) - sign_info_len, ";%u", nonce);
    sign_info_len += strlen(sign_info + sign_info_len);
    snprintf(sign_info + sign_info_len, sizeof(sign_info) - sign_info_len, ";%u", time_expiration);
    sign_info_len += strlen(sign_info + sign_info_len);

    qcloud_iot_utils_base64decode(secret, sizeof(secret), (size_t *)&secret_len,
                                  (const unsigned char *)sg_device_info.psk, sizeof(sg_device_info.psk));
    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "bind sign in", sign_info, sign_info_len);
    utils_hmac_sha1((const char *)sign_info, sign_info_len, out_sign, (const char *)secret, secret_len);
    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "bind sign out", out_sign, sizeof(out_sign));

    // return [SHA1_DIGEST_SIZE bytes sign] + [x bytes device_name]
    memset(out_buf, 0, buf_len);
    memcpy(out_buf, out_sign, SHA1_DIGEST_SIZE);
    ret_len += SHA1_DIGEST_SIZE;

    memcpy(out_buf + ret_len, sg_device_info.device_name, strlen(sg_device_info.device_name));
    ret_len += strlen(sg_device_info.device_name);
    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "bind auth code", out_buf, ret_len);

    return ret_len;
}

ble_qiot_ret_status_t ble_bind_write_result(const char *result, uint16_t len)
{
    POINTER_SANITY_CHECK(result, BLE_QIOT_RS_ERR_PARA);
    BUFF_LEN_SANITY_CHECK(len, (int32_t)sizeof(ble_core_data), BLE_QIOT_RS_ERR_PARA);

    ble_core_data *bind_result = (ble_core_data *)result;

    llsync_bind_state_set((e_llsync_bind_state)bind_result->bind_state);
    return ble_write_core_data(bind_result);
}

ble_qiot_ret_status_t ble_unbind_write_result(void)
{
    ble_core_data bind_result;

    llsync_connection_state_set(E_LLSYNC_DISCONNECTED);
    llsync_bind_state_set(E_LLSYNC_BIND_IDLE);
    memset(&bind_result, 0, sizeof(bind_result));
    return ble_write_core_data(&bind_result);
}

int ble_conn_get_authcode(const char *conn_data, uint16_t data_len, char *out_buf, uint16_t buf_len)
{
    POINTER_SANITY_CHECK(conn_data, BLE_QIOT_RS_ERR_PARA);
    BUFF_LEN_SANITY_CHECK(data_len, (int32_t)sizeof(ble_conn_data), BLE_QIOT_RS_ERR_PARA);
    POINTER_SANITY_CHECK(out_buf, BLE_QIOT_RS_ERR_PARA);
    BUFF_LEN_SANITY_CHECK(buf_len, SHA1_DIGEST_SIZE + BLE_QIOT_DEVICE_NAME_LEN, BLE_QIOT_RS_ERR_PARA);

    char sign_info[64]              = {0};
    char out_sign[SHA1_DIGEST_SIZE] = {0};
    int  sign_info_len              = 0;
    int  timestamp                  = 0;
    int  ret_len                    = 0;

    // if the pointer "char *bind_data" is not aligned with 4 byte, in some cpu convert it to
    // pointer "ble_bind_data *" work correctly, but some cpu will get wrong value, or cause
    // other "Unexpected Error". Here copy it to a local variable make sure aligned with 4 byte.
    ble_conn_data conn_data_aligned;
    memcpy(&conn_data_aligned, conn_data, sizeof(ble_conn_data));

    // valid sign
    timestamp = NTOHL(conn_data_aligned.timestamp);
    snprintf(sign_info + sign_info_len, sizeof(sign_info) - sign_info_len, "%d", timestamp);
    sign_info_len += strlen(sign_info + sign_info_len);
    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "valid sign in", sign_info, sign_info_len);
    utils_hmac_sha1(sign_info, sign_info_len, out_sign, sg_core_data.local_psk, sizeof(sg_core_data.local_psk));
    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "valid sign out", out_sign, SHA1_DIGEST_SIZE);
    if (0 != memcmp(&conn_data_aligned.sign_info, out_sign, SHA1_DIGEST_SIZE)) {
        ble_qiot_log_e("invalid connect sign");
        return BLE_QIOT_RS_VALID_SIGN_ERR;
    }

    // calc sign
    memset(sign_info, 0, sizeof(sign_info));
    sign_info_len = 0;

    // expiration time + product id + device name
    timestamp = BLE_GET_EXPIRATION_TIME(NTOHL(conn_data_aligned.timestamp));
    snprintf(sign_info + sign_info_len, sizeof(sign_info) - sign_info_len, "%d", timestamp);
    sign_info_len += strlen(sign_info + sign_info_len);
    memcpy(sign_info + sign_info_len, sg_device_info.product_id, sizeof(sg_device_info.product_id));
    sign_info_len += sizeof(sg_device_info.product_id);
    memcpy(sign_info + sign_info_len, sg_device_info.device_name, strlen(sg_device_info.device_name));
    sign_info_len += strlen(sg_device_info.device_name);

    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "conn sign in", sign_info, sign_info_len);
    utils_hmac_sha1(sign_info, sign_info_len, out_sign, sg_core_data.local_psk, sizeof(sg_core_data.local_psk));
    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "conn sign out", out_sign, sizeof(out_sign));

    // return authcode
    memset(out_buf, 0, buf_len);
    memcpy(out_buf, out_sign, SHA1_DIGEST_SIZE);
    ret_len += SHA1_DIGEST_SIZE;
    memcpy(out_buf + ret_len, sg_device_info.device_name, strlen(sg_device_info.device_name));
    ret_len += strlen(sg_device_info.device_name);
    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "conn auth code", out_buf, ret_len);

    return ret_len;
}

int ble_unbind_get_authcode(const char *unbind_data, uint16_t data_len, char *out_buf, uint16_t buf_len)
{
    POINTER_SANITY_CHECK(unbind_data, BLE_QIOT_RS_ERR_PARA);
    BUFF_LEN_SANITY_CHECK(data_len, (int32_t)sizeof(ble_unbind_data), BLE_QIOT_RS_ERR_PARA);
    POINTER_SANITY_CHECK(out_buf, BLE_QIOT_RS_ERR_PARA);
    BUFF_LEN_SANITY_CHECK(buf_len, SHA1_DIGEST_SIZE, BLE_QIOT_RS_ERR_PARA);

    char sign_info[32]              = {0};
    char out_sign[SHA1_DIGEST_SIZE] = {0};
    int  sign_info_len              = 0;
    int  ret_len                    = 0;

    // valid sign
    memcpy(sign_info, BLE_UNBIND_REQUEST_STR, BLE_UNBIND_REQUEST_STR_LEN);
    sign_info_len += BLE_UNBIND_REQUEST_STR_LEN;
    utils_hmac_sha1(sign_info, sign_info_len, out_sign, sg_core_data.local_psk, sizeof(sg_core_data.local_psk));
    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "valid sign out", out_sign, SHA1_DIGEST_SIZE);

    if (0 != memcmp(((ble_unbind_data *)unbind_data)->sign_info, out_sign, SHA1_DIGEST_SIZE)) {
        ble_qiot_log_e("invalid unbind sign");
        return BLE_QIOT_RS_VALID_SIGN_ERR;
    }

    // calc sign
    memset(sign_info, 0, sizeof(sign_info));
    sign_info_len = 0;

    memcpy(sign_info, BLE_UNBIND_RESPONSE, strlen(BLE_UNBIND_RESPONSE));
    sign_info_len += BLE_UNBIND_RESPONSE_STR_LEN;
    utils_hmac_sha1(sign_info, sign_info_len, out_sign, sg_core_data.local_psk, sizeof(sg_core_data.local_psk));
    ble_qiot_log_hex(BLE_QIOT_LOG_LEVEL_INFO, "unbind auth code", out_sign, SHA1_DIGEST_SIZE);

    memset(out_buf, 0, buf_len);
    memcpy(out_buf, out_sign, sizeof(out_sign));
    ret_len += sizeof(out_sign);

    return ret_len;
}

#ifdef __cplusplus
}
#endif
