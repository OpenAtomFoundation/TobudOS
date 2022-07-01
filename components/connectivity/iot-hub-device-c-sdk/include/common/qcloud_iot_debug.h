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
 * @file qcloud_iot_debug.h
 * @brief iot debug & upload debug
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-28
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-28 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_COMMON_QCLOUD_IOT_DEBUG_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_COMMON_QCLOUD_IOT_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "utils_log.h"
#include "qcloud_iot_config.h"
#include "qcloud_iot_platform.h"

#ifdef IOT_DEBUG
#define IOT_FUNC_ENTRY                                             \
    {                                                              \
        Log_d("FUNC_ENTRY:   %s L#%d \n", __FUNCTION__, __LINE__); \
    }
#define IOT_FUNC_EXIT                                             \
    {                                                             \
        Log_d("FUNC_EXIT:   %s L#%d \n", __FUNCTION__, __LINE__); \
        return;                                                   \
    }
#define IOT_FUNC_EXIT_RC(x)                                                          \
    {                                                                                \
        Log_d("FUNC_EXIT:   %s L#%d Return Code : %d \n", __FUNCTION__, __LINE__, x; \
        return x;                                                                    \
    }
#else
#define IOT_FUNC_ENTRY
#define IOT_FUNC_EXIT \
    {                 \
        return;       \
    }
#define IOT_FUNC_EXIT_RC(x) \
    {                       \
        return x;           \
    }
#endif


#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_COMMON_QCLOUD_IOT_DEBUG_H_
