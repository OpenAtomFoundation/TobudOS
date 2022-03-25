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
 * @file HAL_Device_linux.c
 * @brief Get and set device info
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-31
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-31 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-09 <td>1.1     <td>fancyxu   <td>fix code standard of IotReturnCode
 * </table>
 */

#include "qcloud_iot_platform.h"

/**
 * @brief  Enable this macro (also control by cmake) to use static string buffer to store device info.To use specific
 * storing methods like files/flash, disable this macro and implement dedicated methods.
 *
 */
#ifdef DEBUG_DEV_INFO_USED

/**
 * @brief product Id
 *
 */
static char sg_product_id[MAX_SIZE_OF_PRODUCT_ID + 1] = "FWR8PGACUS";

/**
 * @brief device name
 *
 */
static char sg_device_name[MAX_SIZE_OF_DEVICE_NAME + 1] = "dev001";

#ifdef DEV_DYN_REG_ENABLED
/**
 * @brief product secret for device dynamic Registration
 *
 */
static char sg_product_secret[MAX_SIZE_OF_PRODUCT_SECRET + 1] = "YOUR_PRODUCT_SECRET";
#endif

#ifdef AUTH_MODE_CERT

/**
 * @brief public cert file name of certificate device
 *
 */
static char sg_device_cert_file_name[MAX_SIZE_OF_DEVICE_CERT_FILE_NAME + 1] = "YOUR_DEVICE_NAME_cert.crt";

/**
 * @brief private key file name of certificate device
 *
 */
static char sg_device_privatekey_file_name[MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME + 1] = "YOUR_DEVICE_NAME_private.key";
#else
/**
 * @brief device secret of PSK device
 *
 */
static char sg_device_secret[MAX_SIZE_OF_DEVICE_SECRET + 1] = "XIBjgofTv/QEQTlRTDQnGg==";
#endif

#endif

/**
 * @brief Copy device info from src to dst
 *
 * @param[out] dst dst to copy
 * @param[in] src srt to be copied
 * @param[in] max_len max_len to be copy
 * @return @see IotReturnCode
 */
static int device_info_copy(void *dst, void *src, uint8_t max_len)
{
    if (strlen(src) > max_len) {
        return QCLOUD_ERR_FAILURE;
    }
    memset(dst, '\0', max_len);
    strncpy(dst, src, max_len);
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief Save device info
 *
 * @param[in] dev_info device info to be saved
 * @return @see IotReturnCode
 */
int HAL_SetDevInfo(void *dev_info)
{
    POINTER_SANITY_CHECK(dev_info, QCLOUD_ERR_DEV_INFO);
    int         rc;
    DeviceInfo *device_info = (DeviceInfo *)dev_info;

#ifdef DEBUG_DEV_INFO_USED
    rc = device_info_copy(sg_product_id, device_info->product_id, MAX_SIZE_OF_PRODUCT_ID);      // set product ID
    rc |= device_info_copy(sg_device_name, device_info->device_name, MAX_SIZE_OF_DEVICE_NAME);  // set dev name

#ifdef DEV_DYN_REG_ENABLE
    rc |= device_info_copy(sg_product_secret, device_info->product_secret, MAX_SIZE_OF_PRODUCT_SECRET);
#endif

#ifdef AUTH_MODE_CERT
    rc |= device_info_copy(sg_device_cert_file_name, device_info->dev_cert_file_name,
                           MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);  // set dev cert file name
    rc |= device_info_copy(sg_device_privatekey_file_name, device_info->dev_key_file_name,
                           MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME);  // set dev key file name
#else
    rc |= device_info_copy(sg_device_secret, device_info->device_secret, MAX_SIZE_OF_DEVICE_SECRET);  // set dev secret
#endif

#else
    rc = iot_save_devinfo_to_json_file(device_info);
#endif

    if (rc) {
        Log_e("Set device info err");
        rc = QCLOUD_ERR_DEV_INFO;
    }
    return rc;
}

/**
 * @brief Get device info
 *
 * @param[in] dev_info buffer to save device info
 * @return @see IotReturnCode
 */
int HAL_GetDevInfo(void *dev_info)
{
    POINTER_SANITY_CHECK(dev_info, QCLOUD_ERR_DEV_INFO);

    int         rc;
    DeviceInfo *device_info = (DeviceInfo *)dev_info;
    memset((char *)device_info, '\0', sizeof(DeviceInfo));

#ifdef DEBUG_DEV_INFO_USED
    rc = device_info_copy(device_info->product_id, sg_product_id, MAX_SIZE_OF_PRODUCT_ID);      // get product ID
    rc |= device_info_copy(device_info->device_name, sg_device_name, MAX_SIZE_OF_DEVICE_NAME);  // get dev name

#ifdef DEV_DYN_REG_ENABLED
    rc |=
        device_info_copy(device_info->product_secret, sg_product_secret, MAX_SIZE_OF_PRODUCT_SECRET);  // get product ID
#endif

#ifdef AUTH_MODE_CERT
    rc |= device_info_copy(device_info->dev_cert_file_name, sg_device_cert_file_name,
                           MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);  // get dev cert file name
    rc |= device_info_copy(device_info->dev_key_file_name, sg_device_privatekey_file_name,
                           MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME);  // get dev key file name
#else
    rc |= device_info_copy(device_info->device_secret, sg_device_secret, MAX_SIZE_OF_DEVICE_SECRET);  // get dev secret
#endif

#else
    // get devinfo from file
    rc = HAL_GetDevInfoFromFile(sg_device_info_file, device_info);
#endif

    if (rc) {
        Log_e("Get device info err");
        rc = QCLOUD_ERR_DEV_INFO;
    }
    return rc;
}
