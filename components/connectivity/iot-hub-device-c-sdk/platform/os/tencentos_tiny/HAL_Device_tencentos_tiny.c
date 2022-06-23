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
 * @file HAL_Device_tencentos_tiny.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2022-01-24
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-01-24 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcloud_iot_platform.h"

#include "qcloud_iot_params_check.h"

/**
 * @brief product Id
 *
 */
static char sg_product_id[MAX_SIZE_OF_PRODUCT_ID + 1] = "PRODUCT_ID";

/**
 * @brief device name
 *
 */
static char sg_device_name[MAX_SIZE_OF_DEVICE_NAME + 1] = "DEVICE_NAME";

/**
 * @brief device secret of PSK device
 *
 */
static char sg_device_secret[MAX_SIZE_OF_DEVICE_SECRET + 1] = "IOT_PSK";

/**
 * @brief Copy device info from src to dst
 *
 * @param[out] dst dst to copy
 * @param[in] src srt to be copied
 * @param[in] max_len max_len to be copy
 * @return @see IotReturnCode
 */
static int device_info_copy(void *pdst, void *psrc, uint8_t max_len)
{
    if (strlen(psrc) > max_len) {
        return QCLOUD_ERR_FAILURE;
    }
    memset(pdst, '\0', max_len);
    strncpy(pdst, psrc, max_len);
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief Save device info
 *
 * @param[in] device_info @see DeviceInfo
 * @return @see IotReturnCode
 */
int HAL_SetDevInfo(DeviceInfo *device_info)
{
    POINTER_SANITY_CHECK(device_info, QCLOUD_ERR_DEV_INFO);
    int rc;
    rc = device_info_copy(sg_product_id, device_info->product_id, MAX_SIZE_OF_PRODUCT_ID);            // set product ID
    rc |= device_info_copy(sg_device_name, device_info->device_name, MAX_SIZE_OF_DEVICE_NAME);        // set dev name
    rc |= device_info_copy(sg_device_secret, device_info->device_secret, MAX_SIZE_OF_DEVICE_SECRET);  // set dev secret
    if (rc) {
        Log_e("Set device info err");
        rc = QCLOUD_ERR_DEV_INFO;
    }
    return rc;
}

/**
 * @brief Get device info
 *
 * @param[in] device_info @see DeviceInfo
 * @return @see IotReturnCode
 */
int HAL_GetDevInfo(DeviceInfo *device_info)
{
    POINTER_SANITY_CHECK(device_info, QCLOUD_ERR_DEV_INFO);
    int ret;
    memset(device_info, '\0', sizeof(DeviceInfo));
    ret = device_info_copy(device_info->product_id, sg_product_id, MAX_SIZE_OF_PRODUCT_ID);            // get product ID
    ret |= device_info_copy(device_info->device_name, sg_device_name, MAX_SIZE_OF_DEVICE_NAME);        // get dev name
    ret |= device_info_copy(device_info->device_secret, sg_device_secret, MAX_SIZE_OF_DEVICE_SECRET);  // get dev secret
    if (QCLOUD_RET_SUCCESS != ret) {
        Log_e("Get device info err");
        ret = QCLOUD_ERR_DEV_INFO;
    }
    return ret;
}
