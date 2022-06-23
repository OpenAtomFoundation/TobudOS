/**
 * @file at_module_ota.c
 * @author {hubert} ({hubertxxu@tencent.com})
 * @brief
 * @version 1.0
 * @date 2022-06-07
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
 * 2022-06-07		1.0			hubertxxu		first commit
 * </table>
 */
#include "at_module.h"

/**
 * @brief OTA info handle.
 *
 */
static QcloudIotOtaFwInfo sg_iot_ota_info_handle;

/**
 * @brief Get ota info handle.
 *
 * @return @see QcloudIotOtaFwInfo
 */
QcloudIotOtaFwInfo *qcloud_iot_get_ota_info_handle(void)
{
    return &sg_iot_ota_info_handle;
}

/**
 * @brief Init ota && report mcu & at version.
 *
 * @param[in] version mcu version.
 * @return 0 for success
 */
int IOT_OTA_Init(const char *version)
{
    sg_iot_ota_info_handle.get_fw_info_sem = HAL_SemaphoreCreate();
    if (!sg_iot_ota_info_handle.get_fw_info_sem) {
        Log_e("create sem fail.");
        return QCLOUD_ERR_FAILURE;
    }
    module_set_ota_urc();
    // TODO: get at module version and change cls state.
    return module_ota_set_fw_version(IOT_OTA_CLS_STATE_REPORT_MCU, version);
}

/**
 * @brief Deinit ota.
 *
 */
void IOT_OTA_Deinit(void)
{
    HAL_SemaphoreDestroy(sg_iot_ota_info_handle.get_fw_info_sem);
}

/**
 * @brief Read fw info from at module.
 *
 * @param[out] version mcu fw version
 * @param[out] fw_size mcu fw size
 * @param[out] md5  mcu fw md5
 * @param[in] timeout_ms timeout
 * @return 0 for success
 */
int IOT_OTA_ReadFwInfo(char **version, uint32_t *fw_size, char **md5, uint32_t timeout_ms)
{
    int rc = 0;

    rc = module_ota_get_fw_info();
    if (rc) {
        return rc;
    }

    rc = HAL_SemaphoreWait(sg_iot_ota_info_handle.get_fw_info_sem, timeout_ms);
    if (rc) {
        *version = NULL;
        *fw_size = 0;
        *md5     = NULL;
        return QCLOUD_ERR_FAILURE;
    }

    *version = sg_iot_ota_info_handle.version;
    *fw_size = sg_iot_ota_info_handle.fw_size;
    *md5     = sg_iot_ota_info_handle.md5;
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief Read fw data from at module.
 *
 * @param[out] fw_data fw data
 * @param[out] fw_data_len fw data length
 * @param[in] timeout_ms timeout
 * @return 0 for success
 */
int IOT_OTA_ReadFWData(uint8_t *fw_data, uint32_t *fw_data_len, uint32_t timeout_ms)
{
    return module_ota_read_fw_data(fw_data, fw_data_len, timeout_ms);
}
