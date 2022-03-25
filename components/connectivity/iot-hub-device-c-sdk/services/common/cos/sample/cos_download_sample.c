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
 * @file cos_download_sample.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-10-27
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-10-27 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcloud_iot_cos.h"

#include "utils_log.h"

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
static int sg_main_exit = 0;

#ifdef __linux__

#include <signal.h>
#include <pthread.h>
#include <unistd.h>

static void _main_exit(int sig)
{
    Log_e("demo exit by signal:%d\n", sig);
    sg_main_exit = 1;
}
#endif

int main(int argc, char **argv)
{
#ifdef __linux__
    signal(SIGINT, _main_exit);
#endif

    int rc = 0;

    // init log level
    LogHandleFunc func = {0};

    func.log_malloc               = HAL_Malloc;
    func.log_free                 = HAL_Free;
    func.log_get_current_time_str = HAL_Timer_Current;
    func.log_printf               = HAL_Printf;
    utils_log_init(func, LOG_LEVEL_DEBUG, 2048);

    IotCosDownloadParams connect_params = {
        .url              = "http://localhost",  // your cos url
        .file_size        = 0,                   // your cos file size
        .offset           = 0,
        .is_fragmentation = false,
        .is_https_enabled = false,
    };

    uint8_t buf[1024];

    void *handle = IOT_COS_DownloadInit(&connect_params);
    if (!handle) {
        goto exit;
    }

    while (!IOT_COS_DownloadIsFinished(handle)) {
        rc = IOT_COS_DownloadFetch(handle, buf, sizeof(buf), 5000);
        if (rc < 0) {
            break;
        }
    }
    IOT_COS_DownloadDeinit(handle);
exit:
    utils_log_deinit();
    return rc;
}
