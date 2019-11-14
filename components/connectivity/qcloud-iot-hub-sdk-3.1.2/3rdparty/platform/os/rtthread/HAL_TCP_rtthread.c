/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <string.h>
//#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include <rtthread.h>


#include "qcloud_iot_import.h"
#include "qcloud_iot_export_log.h"
#include "qcloud_iot_export_error.h"


static uint64_t rtthread_get_time_ms(void)
{
    return HAL_GetTimeMs();
}

static uint64_t rtthread_time_left(uint64_t t_end, uint64_t t_now)
{
    uint64_t t_left;

    if (t_end > t_now) {
        t_left = t_end - t_now;
    } else {
        t_left = 0;
    }

    return t_left;
}

uintptr_t HAL_TCP_Connect(const char *host, uint16_t port)
{
	int ret;
	struct addrinfo hints, *addr_list, *cur;
    int fd = 0;

    char port_str[6];
    HAL_Snprintf(port_str, 6, "%d", port);

    memset(&hints, 0x00, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    Log_d("establish tcp connection with server(host=%s port=%s)", host, port_str);

    if (getaddrinfo(host, port_str, &hints, &addr_list) != 0) {
        perror("getaddrinfo error");
        return 0;
    }

    for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
        if (cur->ai_family != AF_INET) {
            perror("socket type error");
            ret = -1;
            continue;
        }
    	fd = (int) socket( cur->ai_family, cur->ai_socktype, cur->ai_protocol );
		if( fd < 0 )
		{
			ret = 0;
			continue;
		}

		if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0)
		{
			ret = fd;
			break;
		}

		close( fd );
		ret = 0;
    }

    if (0 == ret) {
        Log_e("fail to establish tcp");
    } else {
        Log_d("success to establish tcp, fd=%d", ret);
    }

    freeaddrinfo(addr_list);

    return (uintptr_t)ret;
}	


int HAL_TCP_Disconnect(uintptr_t fd)
{
    int rc;

    rc = close((int) fd);
    if (0 != rc) {
        perror("closesocket error");
        return -1;
    }

    return 0;
}


int HAL_TCP_Write(uintptr_t fd, const unsigned char *buf, uint32_t len, uint32_t timeout_ms, size_t *written_len)
{
    int ret;
    uint32_t len_sent;
    uint64_t t_end, t_left;
    fd_set sets;

    t_end = rtthread_get_time_ms() + timeout_ms;
    len_sent = 0;
    ret = 1; /* send one time if timeout_ms is value 0 */

    do {
		t_left = rtthread_time_left(t_end, rtthread_get_time_ms());
	    if (0 != t_left) {
	        struct timeval timeout;

	        FD_ZERO(&sets);
	        FD_SET(fd, &sets);

	        timeout.tv_sec = t_left / 1000;
	        timeout.tv_usec = (t_left % 1000) * 1000;

	        ret = select(fd + 1, NULL, &sets, NULL, &timeout);
	        if (ret > 0) {
	            if (0 == FD_ISSET(fd, &sets)) {
	                Log_e("Should NOT arrive");
	                /* If timeout in next loop, it will not sent any data */
	                ret = 0;
	                continue;
	            }
	        } else if (0 == ret) {
	            Log_e("select-write timeout %d", (int)fd);
	            break;
	        } else {
	            if (EINTR == errno) {
	                Log_e("EINTR be caught");
	                continue;
	            }

                perror("select-write fail");
                break;
            }
        }
        else {
        	ret = QCLOUD_ERR_SSL_WRITE_TIMEOUT;
        }

	    if (ret > 0) {
	        ret = send(fd, buf + len_sent, len - len_sent, 0);
	        if (ret > 0) {
	            len_sent += ret;
	        } else if (0 == ret) {
	            Log_e("No data be sent");
	        } else {
	            if (EINTR == errno) {
	                Log_e("EINTR be caught");
	                continue;
	            }

				ret = QCLOUD_ERR_TCP_WRITE_FAIL;
	            perror("send fail");
	            break;
	        }
	    }
	} while ((len_sent < len) && (rtthread_time_left(t_end, rtthread_get_time_ms()) > 0));

    *written_len = (size_t)len_sent;

    return len_sent > 0 ? QCLOUD_RET_SUCCESS : ret;
}


int HAL_TCP_Read(uintptr_t fd, unsigned char *buf, uint32_t len, uint32_t timeout_ms, size_t *read_len)
{
    int ret, err_code;
    uint32_t len_recv;
    uint64_t t_end, t_left;
    fd_set sets;
    struct timeval timeout;


	//Log_d("readTimeout %dms,readlen %d", timeout_ms, *read_len);
	HAL_SleepMs(1);  //wihout delay,read seems to be blocked

    t_end = rtthread_get_time_ms() + timeout_ms;
    len_recv = 0;
    err_code = 0;


    do {
	    t_left = rtthread_time_left(t_end, rtthread_get_time_ms());
	    if (0 == t_left) {
			err_code = QCLOUD_ERR_MQTT_NOTHING_TO_READ;
	        break;
	    }
	    FD_ZERO(&sets);
	    FD_SET(fd, &sets);

	    timeout.tv_sec = t_left / 1000;
	    timeout.tv_usec = (t_left % 1000) * 1000;

	    ret = select(fd + 1, &sets, NULL, NULL, &timeout);
	    if (ret > 0) {
	        ret = recv(fd, buf + len_recv, len - len_recv, 0);
	        if (ret > 0) {
	            len_recv += ret;
	        } else if (0 == ret) {
	            perror("connection is closed");
	            err_code = -1;
	            break;
	        } else {
	            if (EINTR == errno) {
	                Log_e("EINTR be caught");
	                continue;
	            }
	            perror("recv fail");
	            err_code = -2;
	            break;
	        }
	    } else if (0 == ret) {
	    	err_code = QCLOUD_ERR_MQTT_NOTHING_TO_READ;
	        break;
	    } else {
	        perror("select-recv fail");
	        err_code = -2;
	        break;
	    }
	}while ((len_recv < len));

    *read_len = (size_t)len_recv;

    return (0 != len_recv) ? 0 : err_code;
}
