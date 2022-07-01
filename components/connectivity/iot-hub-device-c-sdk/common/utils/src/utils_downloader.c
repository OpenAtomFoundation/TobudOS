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
 * @file utils_downloader.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-10-18
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-10-18 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include "utils_downloader.h"

/**
 * @brief Downloader.
 *
 */
typedef struct {
    UtilsDownloaderFunction func;
    void*                   usr_data;
} UtilsDownloader;

/**
 * @brief Init downloader.
 *
 * @param[in] func download function should implement
 * @param[in] usr_data user data using in function
 * @return pointer to downloader
 */
void* utils_downloader_init(UtilsDownloaderFunction func, void* usr_data)
{
    if (!func.downloader_malloc) {
        return NULL;
    }

    UtilsDownloader* handle = func.downloader_malloc(sizeof(UtilsDownloader));

    if (!handle) {
        return NULL;
    }

    handle->func     = func;
    handle->usr_data = usr_data;
    return handle;
}

/**
 * @brief Process download using function.
 *
 * @param[in,out] handle pointer to downloader
 * @return -1 for fail,  others see data_download_finish
 */
int utils_downloader_process(void* handle)
{
    int rc = -1;

    UtilsDownloader*      downloader = handle;
    UtilsDownloaderStatus status     = UTILS_DOWNLOADER_STATUS_SUCCESS;

    if (!handle) {
        return -1;
    }

    rc = downloader->func.break_point_init(downloader->usr_data);
    if (rc) {
        status = UTILS_DOWNLOADER_STATUS_BREAK_POINT_FAILED;
        goto exit;
    }

    // if check ok, restore from break point, otherwise set break point
    rc = downloader->func.break_point_check(downloader->usr_data)
             ? downloader->func.break_point_set(downloader->usr_data)
             : downloader->func.break_point_restore(downloader->usr_data);
    if (rc) {
        status = UTILS_DOWNLOADER_STATUS_BREAK_POINT_FAILED;
        goto exit;
    }

    rc = downloader->func.data_download_init(downloader->usr_data);
    if (rc) {
        status = UTILS_DOWNLOADER_STATUS_DATA_DOWNLOAD_FAILED;
        goto exit;
    }

    while (!downloader->func.data_download_is_over(downloader->usr_data)) {
        rc = downloader->func.data_download_recv(downloader->usr_data);
        if (rc < 0) {
            status = UTILS_DOWNLOADER_STATUS_NETWORK_FAILED;
            goto exit;
        }

        rc = downloader->func.data_download_save(downloader->usr_data);
        if (rc) {
            status = UTILS_DOWNLOADER_STATUS_DATA_DOWNLOAD_FAILED;
            goto exit;
        }

        rc = downloader->func.break_point_save(downloader->usr_data);
        if (rc) {
            status = UTILS_DOWNLOADER_STATUS_BREAK_POINT_FAILED;
            goto exit;
        }
    }

exit:
    rc = downloader->func.data_download_finish(downloader->usr_data, status);
    downloader->func.break_point_deinit(downloader->usr_data);
    downloader->func.data_download_deinit(downloader->usr_data);
    return rc;
}

/**
 * @brief Deinit downloader.
 *
 * @param[in,out] handle pointer to downloader
 */
void utils_downloader_deinit(void* handle)
{
    UtilsDownloader* downloader = handle;

    if (!handle) {
        return;
    }
    downloader->func.downloader_free(handle);
}
