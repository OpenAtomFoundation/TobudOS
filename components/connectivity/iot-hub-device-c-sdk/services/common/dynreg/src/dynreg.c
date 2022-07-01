/**
 * @file dynreg.c
 * @author {hubert} ({hubertxxu@tencent.com})
 * @brief
 * @version 1.0
 * @date 2022-01-26
 *
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
 * @par Change Log:
 * <table>
 * Date				Version		Author			Description
 * 2022-01-26		1.0			hubertxxu		first commit
 * </table>
 */

#include "qcloud_iot_common.h"
#include "utils_json.h"
#include "utils_base64.h"
#include "mbedtls/aes.h"

#define DYN_RESPONSE_BUFF_LEN (256)
#define DECODE_BUFF_LEN       (256)
#define UTILS_AES_BLOCK_LEN   (16)

/**
 * @brief aes cbc handle
 *
 * @return int 0 is success other is error.
 */
static int _aes_cbc(uint8_t *input_data, uint32_t input_data_len, uint8_t *output_data, uint32_t output_data_len,
                    DeviceInfo *device_info)
{
    int     i;
    uint8_t iv[16]                       = {0};
    char    key[UTILS_AES_BLOCK_LEN + 1] = {0};
    for (i = 0; i < 16; i++) {
        iv[i] = '0';
    }
    strncpy(key, device_info->product_secret, UTILS_AES_BLOCK_LEN);
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_dec(&aes_ctx, (const unsigned char *)key, 128);
    if (mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, input_data_len, iv, (unsigned char *)input_data,
                              (unsigned char *)output_data)) {
        return QCLOUD_ERR_FAILURE;
    }
    mbedtls_aes_free(&aes_ctx);
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief parse device register response buffer
 *
 * @param response_buf
 * @param response_len
 * @return int 0 success other error
 */
static int _parse_response_result(char *response_buf, int response_len, DeviceInfo *device_info)
{
    int            rc = 0;
    UtilsJsonValue value;
    char           base64_decode_buf[DECODE_BUFF_LEN] = {0};
    size_t         olen                               = 0;

    // 1. find payload
    rc = utils_json_value_get("Response.Payload", strlen("Response.Payload"), response_buf, response_len, &value);
    if (rc) {
        Log_e("can not find payload. json : %s", response_buf);
        goto exit;
    }

    // 2. base64 decode
    rc = utils_base64decode((uint8_t *)base64_decode_buf, DECODE_BUFF_LEN, &olen, (const uint8_t *)value.value,
                            value.value_len);
    if (rc) {
        Log_e("base64 decode error.");
        goto exit;
    }

    // 3. aes cbc
    olen = olen + (16 - olen % 16);
    rc   = _aes_cbc((uint8_t *)base64_decode_buf, (uint32_t)olen, (uint8_t *)base64_decode_buf, DECODE_BUFF_LEN,
                    device_info);
    if (rc) {
        Log_e("aes cbc error. ");
        goto exit;
    }

    // 4. find psk
    uint32_t encryption_type = 0;
    rc = utils_json_get_uint32("encryptionType", strlen("encryptionType"), base64_decode_buf, DECODE_BUFF_LEN,
                               &encryption_type);
    if (rc) {
        Log_e("parse encryptionType error. json : %s", base64_decode_buf);
        goto exit;
    }
    rc = utils_json_value_get("psk", strlen("psk"), base64_decode_buf, DECODE_BUFF_LEN, &value);
    if (rc) {
        Log_e("parse psk error. json : %s", base64_decode_buf);
        goto exit;
    }

    // 5. copy psk to device info
    strncpy(device_info->device_secret, value.value, value.value_len);

exit:
    return rc;
}

/**
 * @brief dynreg device, get device secret or device cert file and private key file from iot platform
 *
 * @param[in] params @see DeviceInfo
 * @return 0 is success other is failed
 */
int IOT_DynReg_Device(DeviceInfo *device_info)
{
    int rc = 0;
    POINTER_SANITY_CHECK(device_info, QCLOUD_ERR_INVAL);

    if (strlen(device_info->product_secret) < UTILS_AES_BLOCK_LEN) {
        Log_e("product key illegal");
        rc = QCLOUD_ERR_FAILURE;
        goto exit;
    }

    char dynreg_buf[DYN_RESPONSE_BUFF_LEN] = {0};

    /* constructor dynreg http body */
    int request_body_len =
        HAL_Snprintf(dynreg_buf, DYN_RESPONSE_BUFF_LEN, "{\"ProductId\":\"%s\",\"DeviceName\":\"%s\"}",
                     device_info->product_id, device_info->device_name);

    HttpSignedParams params = {
        .host            = DYN_REG_SERVER_URL,
        .uri             = DYN_REG_URI_PATH,
        .need_recv       = true,
        .recv_timeout_ms = 2000,
        .secretkey       = device_info->product_secret,
    };
    rc = IOT_HTTP_SignedRequest(&params, dynreg_buf, request_body_len, (uint8_t *)dynreg_buf, DYN_RESPONSE_BUFF_LEN);
    if (rc < 0) {
        goto exit;
    }
    rc = _parse_response_result(dynreg_buf, rc, device_info);
exit:
    return rc;
}
