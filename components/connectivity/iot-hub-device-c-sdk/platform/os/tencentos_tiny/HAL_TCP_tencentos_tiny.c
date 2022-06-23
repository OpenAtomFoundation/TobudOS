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
 * @file HAL_TCP_module.c
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
#include <string.h>
#include <errno.h>

#include "qcloud_iot_common.h"

#include "tos_k.h"
#include "sal_module_wrapper.h"

/**
 * @brief TCP connect in linux
 *
 * @param[in] host host to connect
 * @param[out] port port to connect
 * @return socket fd
 */
int HAL_TCP_Connect(const char *host, const char *port)
{
    int fd;
    Log_i("osal_tcp_connect entry, host=%s port=%d(%s)", host, port, port);
    fd = tos_sal_module_connect(host, port, TOS_SAL_PROTO_TCP);
    if (fd < 0) {
        Log_i("net connect fail\n\r");
        return -1;
    }
    return fd;
}

/**
 * @brief TCP disconnect
 *
 * @param[in] fd socket fd
 * @return 0 for success
 */
int HAL_TCP_Disconnect(int fd)
{
    (void)tos_sal_module_close(fd);
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief TCP write
 *
 * @param[in] fd socket fd
 * @param[in] buf buf to write
 * @param[in] len buf len
 * @param[in] timeout_ms timeout
 * @param[out] written_len data written length
 * @return @see IotReturnCode
 */
int HAL_TCP_Write(int fd, const unsigned char *buf, uint32_t len, uint32_t timeout_ms, size_t *written_len)
{
    int ret;

    ret = tos_sal_module_send(fd, buf, len);

    if (ret < 0) {
        return QCLOUD_ERR_TCP_WRITE_FAIL;
    }

    (*(int *)written_len) = ret;
    return QCLOUD_RET_SUCCESS;
}

/**
 * @brief TCP read.
 *
 * @param[in] fd socket fd
 * @param[out] buf buffer to save read data
 * @param[in] len buffer len
 * @param[in] timeout_ms timeout
 * @param[out] read_len length of data read
 * @return @see IotReturnCode
 */
int HAL_TCP_Read(int fd, unsigned char *buf, uint32_t len, uint32_t timeout_ms, size_t *read_len)
{
    int ret;

    ret = tos_sal_module_recv_timeout(fd, buf, len, timeout_ms);

    if (ret < 0) {
        return QCLOUD_ERR_TCP_READ_FAIL;
    }

    if (ret == 0) {
        return QCLOUD_ERR_TCP_NOTHING_TO_READ;
    }

    (*(int *)read_len) = ret;
    return QCLOUD_RET_SUCCESS;
}
