/*
 * Copyright (c) 2019-2021 Tencent Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#ifdef AT_TCP_ENABLED

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "network_interface.h"
#include "utils_param_check.h"
#include "utils_timer.h"
#include "at_socket_inf.h"

int network_at_tcp_init(Network *pNetwork)
{
	int rc;
	
	/*at device init entry: at_client init, device driver register to at_socket*/
	rc = at_device_init();
	if(QCLOUD_RET_SUCCESS != rc)
	{
		Log_e("at device init fail,rc:%d",rc);
		return rc;
	}
    
	/*do after at device init*/
	rc = at_socket_init();
	if(QCLOUD_RET_SUCCESS != rc)
	{
		Log_e("at socket init fail,rc:%d",rc);
	}

	return rc;
}

int network_at_tcp_connect(Network *pNetwork)
{
	POINTER_SANITY_CHECK(pNetwork, QCLOUD_ERR_INVAL);	
	
	 int fd = at_socket_connect(pNetwork->host, pNetwork->port, eNET_TCP);

	 if (fd < 0) {
		 Log_e("fail to connect with TCP server: %s:%u", pNetwork->host, pNetwork->port);
		 pNetwork->handle = AT_NO_CONNECTED_FD;
		 return -1;
	 }
	 else {
		 Log_d("connected with TCP server: %s:%u", pNetwork->host, pNetwork->port);
		 pNetwork->handle = fd;
		 return 0;
	 }
}

int network_at_tcp_read(Network *pNetwork, unsigned char *data, size_t datalen, uint32_t timeout_ms, size_t *read_len)
{
	int ret, err_code;
	uint32_t len_recv;
	Timer timer;

	InitTimer(&timer);
    countdown_ms(&timer, timeout_ms);

	len_recv = 0;
	err_code = 0;

	do {
		if (expired(&timer)) {
			err_code = QCLOUD_ERR_TCP_READ_TIMEOUT;
			break;
		}

		ret = at_socket_recv(pNetwork->handle, data + len_recv, datalen - len_recv);

		if (ret > 0) {
			len_recv += ret;
		}
		else if (ret == 0) {
			err_code = QCLOUD_ERR_TCP_NOTHING_TO_READ;
		}
		else {//ret < 0
			Log_e("recv fail\n");
			err_code = QCLOUD_ERR_TCP_READ_FAIL;
			break;
		}

	} while ((len_recv < datalen));

	if (err_code == QCLOUD_ERR_TCP_READ_TIMEOUT && len_recv == 0) {
		err_code = QCLOUD_ERR_TCP_NOTHING_TO_READ;
	}

	*read_len = len_recv;

	return (datalen == len_recv) ? QCLOUD_RET_SUCCESS : err_code;
}

int network_at_tcp_write(Network *pNetwork, unsigned char *data, size_t datalen, uint32_t timeout_ms, size_t *written_len)
{
	int ret;
	uint32_t len_sent;
	Timer timer;
	int net_err = 0;

    InitTimer(&timer);
    countdown_ms(&timer, timeout_ms);

	len_sent = 0;
	ret = 1; /* send one time if timeout_ms is value 0 */

	do {
		ret = at_socket_send(pNetwork->handle, data + len_sent, datalen - len_sent);

		if (ret > 0) {
			len_sent += ret;
		}
		else if (0 == ret) {
			Log_e("No data be sent\n");
		}
		else {
			Log_e("send fail, ret:%d\n", ret);
			net_err = 1;
			break;
		}
	} while (!net_err && (len_sent < datalen) && (!expired(&timer)));

	*written_len = (size_t)len_sent;

	return (len_sent > 0 && net_err == 0) ? QCLOUD_RET_SUCCESS : QCLOUD_ERR_TCP_WRITE_FAIL;
}

void network_at_tcp_disconnect(Network *pNetwork)
{
    int rc;

    rc = at_socket_close((int)pNetwork->handle);
    if (QCLOUD_RET_SUCCESS != rc) {
        Log_e("socket close error\n");
    }

    return ;
}

#endif
