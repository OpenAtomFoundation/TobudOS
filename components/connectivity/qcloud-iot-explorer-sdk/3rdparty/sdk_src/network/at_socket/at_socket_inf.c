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

#include <stdio.h>
#include <string.h>
#include <stdint.h> 

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

#include "utils_param_check.h" 
#include "at_socket_inf.h"

/** The global array of available at */
static at_socket_ctx_t at_socket_ctxs[MAX_AT_SOCKET_NUM];

/**at socket operation mutex */
static void *sg_at_socket_mutex;

/**at device driver ops*/
static at_device_op_t *sg_at_device_ops = NULL;

#define MAX_RECV_PKT_PER_CHAIN		(10)

static at_device_op_t * _at_device_op_get(void)
{
	return sg_at_device_ops;   
}

static int _at_socket_ctx_free(at_socket_ctx_t *pCtx)
{
	POINTER_SANITY_CHECK(pCtx, QCLOUD_ERR_INVAL);
	
	pCtx->fd 	   	= UNUSED_SOCKET;
	pCtx->net_type 	= eNET_DEFAULT;

	if(pCtx->recvpkt_list){
		list_destroy(pCtx->recvpkt_list);
		pCtx->recvpkt_list = NULL;
	}
	
	if(pCtx->recv_lock){
		HAL_MutexDestroy(pCtx->recv_lock);
		pCtx->recv_lock = NULL;
	}
		
	return QCLOUD_RET_SUCCESS;
}

static at_socket_ctx_t *_at_socket_ctx_alloc(void)
{
	int i;

	for (i = 0; i < MAX_AT_SOCKET_NUM; i++) {
		if (at_socket_ctxs[i].state == eSOCKET_CLOSED) {	
			
			at_socket_ctxs[i].net_type 		  = eNET_DEFAULT;		
			at_socket_ctxs[i].send_timeout_ms = AT_SOCKET_SEND_TIMEOUT_MS;
			at_socket_ctxs[i].recv_timeout_ms = AT_SOCKET_RECV_TIMEOUT_MS;	
			at_socket_ctxs[i].dev_op		  = _at_device_op_get();		
			
			at_socket_ctxs[i].recv_lock = HAL_MutexCreate();
			if(NULL == at_socket_ctxs[i].recv_lock){
		    	Log_e("create recv lock fail");	    	
				goto exit;
			}
			at_socket_ctxs[i].recvpkt_list = list_new();
			if(NULL != at_socket_ctxs[i].recvpkt_list){
		       at_socket_ctxs[i].recvpkt_list->free = HAL_Free;
		    }else {
		    	Log_e("no memory to allocate recvpkt_list");	    	
				goto exit;
		    }
	
			at_socket_ctxs[i].state			  = eSOCKET_ALLOCED;
			return &at_socket_ctxs[i];
		}
	}

exit:

	if(i < MAX_AT_SOCKET_NUM){
		_at_socket_ctx_free(&at_socket_ctxs[i]);
	}
			
	return NULL;
}

static at_socket_ctx_t *_at_socket_find(int fd)
{
	int i;
	
	for (i = 0; i < MAX_AT_SOCKET_NUM; i++) {
		if (at_socket_ctxs[i].fd == fd) {	
			return &at_socket_ctxs[i];
		}
	}

	return NULL;
}

/* get a block to the AT socket receive list*/
static int _at_recvpkt_put(List *rlist, const char *ptr, size_t length)
{
    at_recv_pkt *pkt = NULL;

	if(rlist->len > MAX_RECV_PKT_PER_CHAIN){
		Log_e("Too many recv packets wait for read");
		HAL_Free(pkt);
		return QCLOUD_ERR_FAILURE;
	}

    pkt = (at_recv_pkt *) HAL_Malloc(sizeof(struct at_recv_pkt));
    if (pkt == NULL){
        Log_e("No memory for receive packet table!");	
        return QCLOUD_ERR_FAILURE;
    }

    pkt->bfsz_totle = length;
    pkt->bfsz_index = 0;
    pkt->buff = (char *) ptr;

	ListNode *node = list_node_new(pkt);
    if (NULL == node) {
        Log_e("run list_node_new is error!");
		HAL_Free(pkt);
        return QCLOUD_ERR_FAILURE;
    }

    list_rpush(rlist, node);
	
    return length;
}

/* get a block from AT socket receive list */
static int _at_recvpkt_get(List *pkt_list, char *buff, size_t len)
{
	ListIterator *iter;
	ListNode *node = NULL;
	at_recv_pkt *pkt;
	size_t readlen = 0, page_len = 0;
	POINTER_SANITY_CHECK(buff, QCLOUD_ERR_INVAL);
	
    if (pkt_list->len) {
		iter = list_iterator_new(pkt_list, LIST_HEAD);
        if (NULL == iter) {
			Log_e("new listiterator fail");
			return QCLOUD_ERR_TCP_READ_FAIL;
        }

	   /*traverse recv pktlist*/	
       do{
            node = list_iterator_next(iter);			
            if (!node) {
               	break;
            }

			/*get recv packet*/
			pkt = (at_recv_pkt *)(node->val);
			if(!pkt){
				Log_e("pkt is invalid!");
				list_remove(pkt_list, node);
				continue;
			}
			
			page_len = pkt->bfsz_totle - pkt->bfsz_index;
			if(page_len >= (len - readlen)){
				memcpy(buff  + readlen, pkt->buff + pkt->bfsz_index, (len - readlen));
				pkt->bfsz_index += len - readlen;
				readlen = len;
				break;			
			}else{
				memcpy(buff + readlen, pkt->buff + pkt->bfsz_index, page_len);
				readlen += page_len;
				
				/*delete pkt after read*/
				HAL_Free(pkt->buff);
				list_remove(pkt_list, node);
			}		
        }while(1);

        list_iterator_destroy(iter);
    }

	return readlen;
}

static void _at_socket_recv_cb(int fd, at_socket_evt_t event, char *buff, size_t bfsz)
{
	POINTER_SANITY_CHECK_RTN(buff);
	at_socket_ctx_t *pAtSocket;
	
    if(event == AT_SOCKET_EVT_RECV){
		HAL_MutexLock(sg_at_socket_mutex);	
		pAtSocket = _at_socket_find(fd + MAX_AT_SOCKET_NUM);	
		if(_at_recvpkt_put(pAtSocket->recvpkt_list, buff, bfsz) < 0){
			Log_e("put recv package to list fail");
			HAL_Free(buff);
		}
		HAL_MutexUnlock(sg_at_socket_mutex);
	}
}

static void _at_socket_closed_cb(int fd, at_socket_evt_t event, char *buff, size_t bfsz)
{
	//fancyxu
	at_socket_ctx_t *pAtSocket;
	pAtSocket = _at_socket_find(fd + MAX_AT_SOCKET_NUM);
	
    if(event == AT_SOCKET_EVT_CLOSED) {
		HAL_MutexLock(sg_at_socket_mutex);	
		pAtSocket->state = eSOCKET_CLOSED;
		_at_socket_ctx_free(pAtSocket);
		HAL_MutexUnlock(sg_at_socket_mutex);
	}
}

int at_device_op_register(at_device_op_t *device_op)
{
	int rc;
	
	if(NULL == sg_at_device_ops){
		sg_at_device_ops = device_op;
		rc = QCLOUD_RET_SUCCESS;
	}else{
		Log_e("pre device op already register");
		rc = QCLOUD_ERR_FAILURE;
	}

	return rc;     
}

int at_socket_init(void)
{
	int i;
	int rc = QCLOUD_RET_SUCCESS;

	for(i = 0; i < MAX_AT_SOCKET_NUM; i++){
		at_socket_ctxs[i].fd     = UNUSED_SOCKET;
		at_socket_ctxs[i].state  = eSOCKET_CLOSED;
		at_socket_ctxs[i].dev_op = NULL;
		at_socket_ctxs[i].recvpkt_list = NULL;		
	}

	sg_at_socket_mutex = HAL_MutexCreate();
	if (sg_at_socket_mutex == NULL) {
		Log_e("create sg_at_socket_mutex fail \n");
		rc = QCLOUD_ERR_FAILURE;
	}

	if(NULL != sg_at_device_ops){
		if(QCLOUD_RET_SUCCESS == sg_at_device_ops->init()){
			Log_d("at device %s init success", (NULL == sg_at_device_ops->deviceName)?"noname":sg_at_device_ops->deviceName);
			sg_at_device_ops->set_event_cb(AT_SOCKET_EVT_RECV, _at_socket_recv_cb);
			sg_at_device_ops->set_event_cb(AT_SOCKET_EVT_CLOSED, _at_socket_closed_cb);
		}else{
			Log_e("at device %s init fail", (NULL == sg_at_device_ops->deviceName)?"noname":sg_at_device_ops->deviceName);
		}
	}

	return rc;  
}

int at_socket_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
	at_device_op_t *at_op = _at_device_op_get();	
	POINTER_SANITY_CHECK(at_op, QCLOUD_ERR_INVAL);	
	POINTER_SANITY_CHECK(at_op->parse_domain, QCLOUD_ERR_INVAL);

	return at_op->parse_domain(host_name, host_ip, host_ip_len);
}

int at_socket_get_local_mac(char *macbuff, size_t bufflen)
{
	at_device_op_t *at_op = _at_device_op_get();
	POINTER_SANITY_CHECK(at_op, QCLOUD_ERR_INVAL);	
	POINTER_SANITY_CHECK(at_op->get_local_mac, QCLOUD_ERR_INVAL);
	
	return at_op->get_local_mac(macbuff, bufflen);
}

int at_socket_get_local_ip(char *ip, size_t iplen, char *gw, size_t gwlen, char *mask, size_t masklen)
{
	at_device_op_t *at_op = _at_device_op_get();
	POINTER_SANITY_CHECK(at_op, QCLOUD_ERR_INVAL);	
	POINTER_SANITY_CHECK(at_op->get_local_ip, QCLOUD_ERR_INVAL);
	
	return at_op->get_local_ip(ip, iplen, gw, gwlen, mask, masklen);
}

int at_socket_connect(const char *host, uint16_t port, eNetProto eProto)
{	
	at_socket_ctx_t *pAtSocket;
	int fd;

	HAL_MutexLock(sg_at_socket_mutex);
	pAtSocket = _at_socket_ctx_alloc();
	HAL_MutexUnlock(sg_at_socket_mutex);
	
	if((NULL == pAtSocket) || (NULL == pAtSocket->dev_op)|| (NULL == pAtSocket->dev_op->connect)){
		Log_e("alloc socket fail");
		return QCLOUD_ERR_FAILURE;
	}

	fd = pAtSocket->dev_op->connect(host, port, eProto);
	if(fd < 0){
		Log_e("dev_op connect fail,pls check at device driver!");
		_at_socket_ctx_free(pAtSocket);	
	}else{		
		pAtSocket->fd = fd + MAX_AT_SOCKET_NUM;
		pAtSocket->state = 	eSOCKET_CONNECTED;
	}
	
	return pAtSocket->fd;
}

int at_socket_close(int fd)
{	
	at_socket_ctx_t *pAtSocket;
	
	
	pAtSocket = _at_socket_find(fd);
	if(NULL==pAtSocket) { //server close the connection
		Log_e("socket was closed");
		return QCLOUD_ERR_TCP_PEER_SHUTDOWN;
	}
	
	int rc;
	if((eSOCKET_CONNECTED == pAtSocket->state) && (NULL != pAtSocket->dev_op) && (NULL != pAtSocket->dev_op->close)) {
		rc = pAtSocket->dev_op->close(pAtSocket->fd - MAX_AT_SOCKET_NUM);
	} else {
		rc = QCLOUD_ERR_FAILURE;
	}		
	return rc;
}

int at_socket_send(int fd, const void *buf, size_t len)
{
	at_socket_ctx_t *pAtSocket;
	
	pAtSocket = _at_socket_find(fd);
	POINTER_SANITY_CHECK(pAtSocket, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(pAtSocket->dev_op, QCLOUD_ERR_INVAL);	
	POINTER_SANITY_CHECK(pAtSocket->dev_op->send, QCLOUD_ERR_INVAL);
	
	if(pAtSocket->state != eSOCKET_CONNECTED){
		Log_e("socket was closed");
		return QCLOUD_ERR_TCP_PEER_SHUTDOWN;
	}else{
		return  pAtSocket->dev_op->send(fd - MAX_AT_SOCKET_NUM, buf, len);
	}		
}

int at_socket_recv(int fd, void *buf, size_t len)
{
	at_socket_ctx_t *pAtSocket;
	size_t	recv_len;
	
	pAtSocket = _at_socket_find(fd);
	POINTER_SANITY_CHECK(pAtSocket, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(pAtSocket->dev_op, QCLOUD_ERR_INVAL);	
	POINTER_SANITY_CHECK(pAtSocket->dev_op->recv_timeout, QCLOUD_ERR_INVAL);

	if(pAtSocket->state != eSOCKET_CONNECTED){
		Log_e("socket was closed");
		return QCLOUD_ERR_TCP_READ_FAIL;
	}else{
		HAL_MutexLock(pAtSocket->recv_lock);

		//call at device recv driver for os and nonos
		if(pAtSocket->recvpkt_list->len == 0) {
			if(pAtSocket->dev_op->recv_timeout(fd - MAX_AT_SOCKET_NUM, buf, len, pAtSocket->recv_timeout_ms) != QCLOUD_RET_SUCCESS) {
				Log_e("at device recv err"); //do not return yet
			}
		}

		/* receive packet list last transmission of remaining data */		
		recv_len = _at_recvpkt_get(pAtSocket->recvpkt_list, (char *)buf, len);
		HAL_MutexUnlock(pAtSocket->recv_lock);
	}	

	return recv_len;
}

int at_socket_recv_timeout(int fd, void *buf, size_t len, uint32_t timeout)
{
	at_socket_ctx_t *pAtSocket;
	
	pAtSocket = _at_socket_find(fd);
	POINTER_SANITY_CHECK(pAtSocket, QCLOUD_ERR_INVAL);
	POINTER_SANITY_CHECK(pAtSocket->dev_op, QCLOUD_ERR_INVAL);	
	POINTER_SANITY_CHECK(pAtSocket->dev_op->recv_timeout, QCLOUD_ERR_INVAL);

	if(pAtSocket->state != eSOCKET_CONNECTED){
		Log_e("socket was closed");
		return QCLOUD_ERR_TCP_PEER_SHUTDOWN;
	}else{
		return  pAtSocket->dev_op->recv_timeout(fd - MAX_AT_SOCKET_NUM, buf, len, timeout);
	}		
}
