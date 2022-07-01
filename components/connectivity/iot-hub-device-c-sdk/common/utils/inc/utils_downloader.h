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
 * @file utils_downloader.h
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

#ifndef IOT_HUB_DEVICE_C_SDK_COMMON_UTILS_INC_UTILS_DOWNLOADER_H_
#define IOT_HUB_DEVICE_C_SDK_COMMON_UTILS_INC_UTILS_DOWNLOADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/**
 * @brief Downloader status.
 *
 */
typedef enum {
    UTILS_DOWNLOADER_STATUS_SUCCESS = 0,
    UTILS_DOWNLOADER_STATUS_BREAK_POINT_FAILED,
    UTILS_DOWNLOADER_STATUS_DATA_DOWNLOAD_FAILED,
    UTILS_DOWNLOADER_STATUS_NETWORK_FAILED
} UtilsDownloaderStatus;

/**
 * @brief Downloader function.
 *
 */
typedef struct {
    // memory
    void* (*downloader_malloc)(size_t len); /**< user malloc */
    void (*downloader_free)(void* val);     /**< user free */

    // break point
    int (*break_point_init)(void* usr_data);    /**< init break point, read from flash or file */
    void (*break_point_deinit)(void* usr_data); /**< deinit break point */
    int (*break_point_set)(void* usr_data);     /**< set break point structure */
    int (*break_point_save)(void* usr_data);    /**< save break point in flash or file */
    int (*break_point_check)(void* usr_data);   /**< check break point valid */
    int (*break_point_restore)(void* usr_data); /**< restore break point */

    // data download
    int (*data_download_init)(void* usr_data);    /**< init data download, such as http connect */
    void (*data_download_deinit)(void* usr_data); /**< deinit data download, such as http disconnect */

    int (*data_download_is_over)(void* usr_data); /**< check if download finish */
    int (*data_download_recv)(void* usr_data);    /**< recv data, such as http recv */
    int (*data_download_save)(void* usr_data);    /**< save data, such as write firmware to flash */
    int (*data_download_finish)(void* usr_data, UtilsDownloaderStatus status); /**< process result */
} UtilsDownloaderFunction;

/**
 * @brief Init downloader.
 *
 * @param[in] func download function should implement
 * @param[in] usr_data user data using in function
 * @return pointer to downloader
 */
void* utils_downloader_init(UtilsDownloaderFunction func, void* usr_data);

/**
 * @brief Process download using function.
 *
 * @param[in,out] handle pointer to downloader
 * @return -1 for fail,  others see data_download_finish
 */
int utils_downloader_process(void* handle);

/**
 * @brief Deinit downloader.
 *
 * @param[in,out] handle pointer to downloader
 */
void utils_downloader_deinit(void* handle);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_COMMON_UTILS_INC_UTILS_DOWNLOADER_H_
