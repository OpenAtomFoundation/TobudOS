/**
 * @file qcloud_iot_http_signed.h
 * @author {hubert} ({hubertxxu@tencent.com})
 * @brief
 * @version 1.0
 * @date 2022-01-17
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
 * 2022-01-17		1.0			hubertxxu		first commit
 * </table>
 */
#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_HTTP_SIGNED_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_HTTP_SIGNED_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_common.h"

typedef struct {
    const char *host;
    const char *uri;
    char       *secretkey;
    uint32_t    recv_timeout_ms;
    bool        need_recv;
} HttpSignedParams;

/**
 * @brief post message and recv response
 *
 * @param params @see HttpSignedParams
 * @param request_buf   request buffer
 * @param request_buf_len request buffer length
 * @param response_buf   response buffer if need recv
 * @param response_buf_len response buffer length if need recv
 * @return int 0 for success. others @see IotReturnCode
 */
int IOT_HTTP_Signed_Request(HttpSignedParams *params, const char *request_buf, size_t request_buf_len,
                            uint8_t *response_buf, int response_buf_len);

#ifdef __cplusplus
}
#endif
#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_HTTP_SIGNED_H_
