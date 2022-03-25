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
 * @file HAL_TCP_linux.c
 * @brief Linux tcp api
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-31
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-31 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2021-07-09 <td>1.1     <td>fancyxu   <td>refactor for support tls, change port to str format
 * </table>
 */

#include "sal_module_wrapper.h"
#include "qcloud_iot_platform.h"

/**
 * @brief TCP connect in linux
 *
 * @param[in] host host to connect
 * @param[out] port port to connect
 * @return socket fd
 */
int HAL_TCP_Connect(const char *host, const char *port)
{
    int rc;
    int fd = 0;
    char host_ip[32];
    
    rc = tos_sal_module_parse_domain(host, host_ip, 32);
    if (rc < 0) {
        return QCLOUD_ERR_TCP_UNKNOWN_HOST;
    }
    
    fd = tos_sal_module_connect(host_ip, port, TOS_SAL_PROTO_TCP);
    if (fd < 0) {
        return QCLOUD_ERR_TCP_CONNECT;
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
    tos_sal_module_close(fd);

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
int HAL_TCP_Write(int fd, const uint8_t *buf, uint32_t len, uint32_t timeout_ms, size_t *written_len)
{
    int ret = 0;
    
    ret = tos_sal_module_send(fd, buf, len);
    if (ret < 0) {
        ret = QCLOUD_ERR_TCP_WRITE_FAIL;
        return -1;
    }
   
    *(int *)written_len = ret;
    // We always know hom much should write.
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
int HAL_TCP_Read(int fd, uint8_t *buf, uint32_t len, uint32_t timeout_ms, size_t *read_len)
{
    int ret;

    ret = tos_sal_module_recv_timeout(fd, buf, len, timeout_ms);
    if (ret < 0) {
        return QCLOUD_ERR_TCP_READ_FAIL;
    } else if (ret == 0) {
        return QCLOUD_ERR_TCP_NOTHING_TO_READ;
    }
    
    *(int *)read_len = ret;

    // We always don't know hom much should read.
    return QCLOUD_RET_SUCCESS;
}
