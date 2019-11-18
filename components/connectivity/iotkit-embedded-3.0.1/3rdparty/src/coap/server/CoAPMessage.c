/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */





#include <stdio.h>
#include "CoAPExport.h"
#include "CoAPSerialize.h"
#include "CoAPDeserialize.h"
#include "CoAPResource.h"
#include "CoAPObserve.h"
#include "CoAPPlatform.h"
#include "CoAPInternal.h"
#include "iotx_coap_internal.h"

#define COAPAckMsg(header) \
    ((header.code == COAP_MSG_CODE_EMPTY_MESSAGE) \
     &&(header.type == COAP_MESSAGE_TYPE_ACK))

#define CoAPRespMsg(header)\
    ((header.code >= 0x40) && (header.code < 0xc0))

#define CoAPPingMsg(header)\
    ((header.code == COAP_MSG_CODE_EMPTY_MESSAGE)\
     && (header.type == COAP_MESSAGE_TYPE_CON))

#define CoAPResetMsg(header)\
    (header.type == COAP_MESSAGE_TYPE_RST)

#define CoAPCONRespMsg(header)\
    ((header.code == COAP_MSG_CODE_205_CONTENT) \
     && (header.type == COAP_MESSAGE_TYPE_CON))

#define CoAPReqMsg(header)\
    ((1 <= header.code) && (32 > header.code))


#define NOKEEP 0
#define KEEPING 1
#define TOREMOVEKEEP 2
#define COAP_CUR_VERSION        1
#define COAP_MAX_MESSAGE_ID     65535
#define COAP_MAX_RETRY_COUNT    8
#define COAP_ACK_TIMEOUT        600
#define COAP_ACK_RANDOM_FACTOR  1

unsigned short CoAPMessageId_gen(CoAPContext *context)
{
    unsigned short msg_id = 0;
    CoAPIntContext *ctx = NULL;
    if (!context) {
        return msg_id;
    }
    ctx = (CoAPIntContext *)context;
    HAL_MutexLock(ctx->mutex);
    msg_id = ((COAP_MAX_MESSAGE_ID == ctx->message_id)  ? (ctx->message_id = 1) : ctx->message_id++);
    HAL_MutexUnlock(ctx->mutex);
    return msg_id;
}

int CoAPMessageHandler_set(CoAPMessage *message, CoAPSendMsgHandler handler)
{
    if (NULL == message) {
        return COAP_ERROR_NULL;
    }
    message->handler = handler;
    return COAP_SUCCESS;
}

static int CoAPMessageList_add(CoAPContext *context, NetworkAddr *remote,
                               CoAPMessage *message, unsigned char *buffer, int len)
{
    CoAPIntContext *ctx = (CoAPIntContext *)context;
    CoAPSendNode *node = NULL;
    uint64_t tick ;
    node = coap_malloc(sizeof(CoAPSendNode));

    if (NULL != node) {
        memset(node, 0x00, sizeof(CoAPSendNode));
        node->acked        = 0;
        node->user         = message->user;
        node->header       = message->header;
        node->handler      = message->handler;
        node->msglen       = len;
        node->message      = buffer;
        node->timeout_val   = COAP_ACK_TIMEOUT * COAP_ACK_RANDOM_FACTOR;
        memcpy(&node->remote, remote, sizeof(NetworkAddr));
        if (platform_is_multicast((const char *)remote->addr) || 1 == message->keep) {
            COAP_FLOW("The message %d need keep", message->header.msgid);
            node->keep = KEEPING;
        } else {
            node->keep = NOKEEP;
        }

        tick = HAL_UptimeMs ();

        if (COAP_MESSAGE_TYPE_CON == message->header.type) {
            node->timeout = node->timeout_val + tick;
            node->retrans_count = COAP_MAX_RETRY_COUNT;
        } else {
            node->timeout = node->timeout_val * 4 + tick;
            node->retrans_count = 0;
        }

        memcpy(node->token, message->token, message->header.tokenlen);

        HAL_MutexLock(ctx->sendlist.list_mutex);
        if (ctx->sendlist.count >= ctx->sendlist.maxcount) {
            HAL_MutexUnlock(ctx->sendlist.list_mutex);
            coap_free(node);
            COAP_INFO("The send list is full");
            return COAP_ERROR_DATA_SIZE;
        } else {
            list_add_tail(&node->sendlist, &ctx->sendlist.list);
            ctx->sendlist.count ++;
            HAL_MutexUnlock(ctx->sendlist.list_mutex);
            return COAP_SUCCESS;
        }
    } else {
        return COAP_ERROR_NULL;
    }
}

void CoAPMessageToken_dump(unsigned char *token, unsigned char tokenlen)
{
    int index = 0, count = 0;
    int total = 2 * COAP_MSG_MAX_TOKEN_LEN;
    char   buff[2 * COAP_MSG_MAX_TOKEN_LEN + 1] = {0}, *ptr = NULL;

    ptr = buff;
    for (index = 0; index < tokenlen; index++) {
        count = HAL_Snprintf(ptr, total, "%02X", token[index]);
        ptr += count;
        total -= count;
    }

    COAP_FLOW("Token Len   : %d", tokenlen);
    COAP_FLOW("Token       : %s", buff);
}

void CoAPMessage_dump(NetworkAddr *remote, CoAPMessage *message)
{
    int ret = COAP_SUCCESS;
    unsigned int ctype;
    unsigned char code, msgclass, detail;

    if (NULL == remote || NULL == message) {
        return;
    }
    code = (unsigned char)message->header.code;
    msgclass = code >> 5;
    detail = code & 0x1F;

    COAP_FLOW("*********Message Info**********");
    COAP_FLOW("Version     : %d", message->header.version);
    COAP_FLOW("Code        : %d.%02d(0x%x)", msgclass, detail, code);
    COAP_FLOW("Type        : 0x%x", message->header.type);
    COAP_FLOW("Msgid       : %d", message->header.msgid);
    COAP_FLOW("Option      : %d", message->optcount);
    COAP_FLOW("Payload Len : %d", message->payloadlen);

    CoAPMessageToken_dump(message->token, message->header.tokenlen);
    COAP_FLOW("Remote      : %s:%d", remote->addr, remote->port);
    ret = CoAPUintOption_get(message, COAP_OPTION_CONTENT_FORMAT, &ctype);
    if (COAP_SUCCESS == ret && NULL != message->payload
        && (COAP_CT_APP_OCTET_STREAM != ctype && COAP_CT_APP_CBOR != ctype)) {
        /* COAP_FLOW("Payload     : %s", message->payload); */
    }

    COAP_FLOW("********************************");

}

int CoAPMessage_send(CoAPContext *context, NetworkAddr *remote, CoAPMessage *message)
{
    int   ret              = COAP_SUCCESS;
    unsigned short msglen  = 0;
    unsigned char  *buff   = NULL;
    unsigned short readlen = 0;
    CoAPIntContext *ctx    = NULL;

    if (NULL == message || NULL == context) {
        return (COAP_ERROR_INVALID_PARAM);
    }

    ctx = (CoAPIntContext *)context;
    msglen = CoAPSerialize_MessageLength(message);
    if (COAP_MSG_MAX_PDU_LEN < msglen) {
        COAP_INFO("The message length %d is too loog", msglen);
        return COAP_ERROR_DATA_SIZE;
    }

    buff = (unsigned char *)coap_malloc(msglen);
    if (NULL == buff) {
        COAP_INFO("Malloc memory failed");
        return COAP_ERROR_NULL;
    }
    memset(buff, 0x00, msglen);
    msglen = CoAPSerialize_Message(message, buff, msglen);

#ifndef COAP_OBSERVE_CLIENT_DISABLE
    CoAPObsClient_delete(ctx, message);
#endif
    readlen = CoAPNetwork_write(ctx->p_network, remote,
                                buff, (unsigned int)msglen, ctx->waittime);
    if (msglen == readlen) {/*Send message success*/
        if (CoAPReqMsg(message->header) || CoAPCONRespMsg(message->header)) {
            COAP_FLOW("The message id %d len %d send success, add to the list",
                      message->header.msgid, msglen);
            ret = CoAPMessageList_add(ctx, remote, message, buff, msglen);
            if (COAP_SUCCESS != ret) {
                coap_free(buff);
                COAP_ERR("Add the message %d to list failed", message->header.msgid);
                return ret;
            }
        } else {
            coap_free(buff);
            COAP_FLOW("The message %d isn't CON msg, needless to be retransmitted",
                      message->header.msgid);
        }
    } else {
        coap_free(buff);
        COAP_ERR("CoAP transport write failed, send message %d return %d", message->header.msgid, ret);
        return COAP_ERROR_WRITE_FAILED;
    }

    CoAPMessage_dump(remote, message);
    return COAP_SUCCESS;
}

int CoAPMessage_cancel(CoAPContext *context, CoAPMessage *message)
{
    CoAPSendNode *node = NULL, *next = NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    if (NULL == context || NULL == message) {
        return COAP_ERROR_NULL;
    }


    HAL_MutexLock(ctx->sendlist.list_mutex);
    list_for_each_entry_safe(node, next, &ctx->sendlist.list, sendlist, CoAPSendNode) {
        if (node->header.msgid == message->header.msgid) {
            list_del(&node->sendlist);
            ctx->sendlist.count--;
            COAP_INFO("Cancel message %d from list, cur count %d",
                      node->header.msgid, ctx->sendlist.count);
            coap_free(node->message);
            coap_free(node);
        }
    }
    HAL_MutexUnlock(ctx->sendlist.list_mutex);
    return COAP_SUCCESS;
}

int CoAPMessageId_cancel(CoAPContext *context, unsigned short msgid)
{
    CoAPSendNode *node = NULL, *next = NULL;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    if (NULL == context || NULL == ctx->sendlist.list_mutex) {
        return COAP_ERROR_NULL;
    }

    HAL_MutexLock(ctx->sendlist.list_mutex);
    list_for_each_entry_safe(node, next, &ctx->sendlist.list, sendlist, CoAPSendNode) {
        if (NULL != node) {
            if (node->header.msgid == msgid) {
                list_del(&node->sendlist);
                ctx->sendlist.count--;
                COAP_FLOW("Cancel message %d from list, cur count %d",
                          node->header.msgid, ctx->sendlist.count);
                coap_free(node->message);
                coap_free(node);
            }
        }
    }
    HAL_MutexUnlock(ctx->sendlist.list_mutex);

    return COAP_SUCCESS;
}

static int CoAPAckMessage_handle(CoAPContext *context, CoAPMessage *message)
{
    CoAPSendNode *node = NULL, *next;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    HAL_MutexLock(ctx->sendlist.list_mutex);
    list_for_each_entry_safe(node, next, &ctx->sendlist.list, sendlist, CoAPSendNode) {
        if (node->header.msgid == message->header.msgid) {
            CoAPSendMsgHandler handler = node->handler;
            void *user_data = node->user;
            NetworkAddr remote = {0};
            memcpy(&remote, &node->remote, sizeof(remote));
            node->acked = 1;
            if (CoAPRespMsg(node->header)) { /* CON response message */
                list_del(&node->sendlist);
                coap_free(node->message);
                coap_free(node);
                ctx->sendlist.count --;
                COAP_DEBUG("The CON response message %d receive ACK, remove it", message->header.msgid);
            }
            if (handler) handler(ctx, COAP_RECV_RESP_SUC, user_data, &remote, NULL);
            HAL_MutexUnlock(ctx->sendlist.list_mutex);
            return COAP_SUCCESS;
        }
    }
    HAL_MutexUnlock(ctx->sendlist.list_mutex);

    return COAP_SUCCESS;
}

static int CoAPAckMessage_send(CoAPContext *context, NetworkAddr *remote, unsigned short msgid)
{
    int ret   = COAP_SUCCESS;
    CoAPMessage message;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    CoAPMessage_init(&message);
    CoAPMessageId_set(&message, msgid);
    COAP_DEBUG("Send Ack Response Message");
    ret = CoAPMessage_send(ctx, remote, &message);
    CoAPMessage_destory(&message);
    return ret;
}

static int CoAPRestMessage_send(CoAPContext *context, NetworkAddr *remote, unsigned short msgid)
{
    int ret   = COAP_SUCCESS;
    CoAPMessage message;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    CoAPMessage_init(&message);
    CoAPMessageType_set(&message, COAP_MESSAGE_TYPE_RST);
    CoAPMessageId_set(&message, msgid);
    COAP_DEBUG("Send Rest Pong Message");
    ret = CoAPMessage_send(ctx, remote, &message);
    CoAPMessage_destory(&message);
    return ret;
}

static int CoAPErrRespMessage_send(CoAPContext *context, NetworkAddr *remote, CoAPMessage *message,
                                   unsigned char err_code)
{
    CoAPMessage response;
    int ret   = COAP_SUCCESS;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    CoAPMessage_init(&response);
    CoAPMessageCode_set(&response, err_code);
    CoAPMessageId_set(&response, message->header.msgid);
    CoAPMessageToken_set(&response, message->token, message->header.tokenlen);
    if (COAP_MESSAGE_TYPE_CON == message->header.type) {
        CoAPMessageType_set(&response, COAP_MESSAGE_TYPE_ACK);
    } else {
        CoAPMessageType_set(&response, message->header.type);
    }
    COAP_FLOW("Send Error Response Message");
    ret = CoAPMessage_send(ctx, remote, &response);
    CoAPMessage_destory(&response);
    return ret;
}

static int CoAPRespMessage_handle(CoAPContext *context, NetworkAddr *remote, CoAPMessage *message)
{
    char                found = 0;
    CoAPSendNode       *node = NULL, *next = NULL;
    CoAPIntContext     *ctx = (CoAPIntContext *)context;

    if (COAP_MESSAGE_TYPE_CON == message->header.type) {
        CoAPAckMessage_send(ctx, remote, message->header.msgid);
    }

    HAL_MutexLock(ctx->sendlist.list_mutex);
    list_for_each_entry_safe(node, next, &ctx->sendlist.list, sendlist, CoAPSendNode) {
        if (0 != node->header.tokenlen && node->header.tokenlen == message->header.tokenlen
            && 0 == memcmp(node->token, message->token, message->header.tokenlen)) {
            if (!node->keep) {
                list_del(&node->sendlist);
                ctx->sendlist.count--;
                COAP_FLOW("Remove the message id %d from list", node->header.msgid);
            } else {
                COAP_FLOW("Find the message id %d, It need keep", node->header.msgid);
            }
            found = 1;

            break;
        }
    }

    if (found && NULL != node) {
        message->user  = node->user;
        /* TODO: comment it */
        /*
        if (COAP_MSG_CODE_400_BAD_REQUEST <= message->header.code) {
            if (NULL != ctx->notifier) {
                ctx->notifier(message->header.code, remote, message);
            }
        }
        */
        if (NULL != node->handler) {
            CoAPSendMsgHandler handler = node->handler;
#ifndef COAP_OBSERVE_CLIENT_DISABLE
            CoAPObsClient_add(ctx, message, remote, node);
#endif
            HAL_MutexUnlock(ctx->sendlist.list_mutex);
            COAP_FLOW("Call the response message callback %p", handler);
            handler(ctx, COAP_REQUEST_SUCCESS, message->user, remote, message);
        } else {
            HAL_MutexUnlock(ctx->sendlist.list_mutex);
        }

        if (!node->keep) {
            if (NULL != node->message) {
                coap_free(node->message);
            }
            coap_free(node);
            COAP_DEBUG("The message needless keep, free it");
        }
    } else {
        HAL_MutexUnlock(ctx->sendlist.list_mutex);
#ifndef COAP_OBSERVE_CLIENT_DISABLE
        CoAPObsClient_add(ctx, message, remote, NULL);
#endif
    }
    return COAP_ERROR_NOT_FOUND;
}

#define PACKET_INTERVAL_THRE_MS     800
#define PACKET_TRIGGER_NUM          100

static int CoAPRequestMessage_ack_send(CoAPContext *context, NetworkAddr *remote, unsigned short msgid)
{
    int ret   = COAP_SUCCESS;
    CoAPMessage message;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    CoAPMessage_init(&message);
    CoAPMessageId_set(&message, msgid);
    COAP_INFO("Send Ack Response Message: %d", msgid);
    ret = CoAPMessage_send(ctx, remote, &message);
    CoAPMessage_destory(&message);
    return ret;
}

static int CoAPRequestMessage_handle(CoAPContext *context, NetworkAddr *remote, CoAPMessage *message)
{
    int             index = 0;
    int             ret   = COAP_SUCCESS;
    CoAPResource   *resource = NULL;
    unsigned char   path[COAP_MSG_MAX_PATH_LEN] = {0};
    unsigned char  *tmp = path;
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    COAP_FLOW("CoAPRequestMessage_handle: %p", ctx);
    /* TODO: if need only one callback */
    for (index = 0; index < message->optcount; index++) {
        if (COAP_OPTION_URI_PATH == message->options[index].num) {
            if ((COAP_MSG_MAX_PATH_LEN - 1) >= (tmp - path + message->options[index].len)) {
                *tmp = '/';
                tmp += 1;
                strncpy((char *)tmp, (const char *)message->options[index].val, message->options[index].len);
                tmp += message->options[index].len;
            }
        }
    }
    if (strcmp("/sys/device/info/notify", (const char *)path)) {
        COAP_DEBUG("Request path is %s", path);
    }

    resource = CoAPResourceByPath_get(ctx, (char *)path);
    if (NULL != resource) {
        if (NULL != resource->callback) {
            if (((resource->permission) & (1 << ((message->header.code) - 1))) > 0) {
                if (message->header.type == COAP_MESSAGE_TYPE_CON) {
                    CoAPRequestMessage_ack_send(ctx, remote, message->header.msgid);
                }
                resource->callback(ctx, (char *)path, remote, message);
            } else {
                COAP_FLOW("The resource %s isn't allowed", resource->path);
                ret = CoAPErrRespMessage_send(ctx, remote, message, COAP_MSG_CODE_405_METHOD_NOT_ALLOWED);
            }
        } else {
            COAP_FLOW("The resource %s handler isn't exist", resource->path);
            ret = CoAPErrRespMessage_send(ctx, remote, message, COAP_MSG_CODE_405_METHOD_NOT_ALLOWED);
        }
    } else {
        COAP_FLOW("The resource %s isn't found", path);
        ret = CoAPErrRespMessage_send(ctx, remote, message, COAP_MSG_CODE_404_NOT_FOUND);
    }

    return ret;
}


static void CoAPMessage_handle(CoAPContext *context,
                               NetworkAddr       *remote,
                               unsigned char     *buf,
                               unsigned short     datalen)
{
    int                 ret  = COAP_SUCCESS;
    CoAPMessage         message;
    CoAPIntContext     *ctx = (CoAPIntContext *)context;

    COAP_FLOW("CoAPMessage_handle: %p", ctx);
    memset(&message, 0x00, sizeof(CoAPMessage));

    ret = CoAPDeserialize_Message(&message, buf, datalen);
    if (COAP_SUCCESS != ret) {
        if (NULL != ctx->notifier) {
            /* TODO: */
            /* context->notifier(context, event); */
        }
    }

    COAP_FLOW("--------Receive a Message------");
    CoAPMessage_dump(remote, &message);

    if (COAPAckMsg(message.header) || CoAPResetMsg(message.header)) {
        /* TODO: implement handle client observe */

        /* TODO: if need call response callback */
        CoAPAckMessage_handle(ctx, &message);

    } else if (CoAPRespMsg(message.header)) {
        CoAPRespMessage_handle(ctx, remote, &message);
    } else if (CoAPPingMsg(message.header)) {
        CoAPRestMessage_send(ctx, remote, message.header.msgid);
    } else if (CoAPReqMsg(message.header)) {
        CoAPRequestMessage_handle(ctx, remote, &message);
    } else {
        COAP_INFO("Weird packet,drop it");
    }

}

int CoAPMessage_process(CoAPContext *context, unsigned int timeout)
{
    int len = 0;
    NetworkAddr remote;
    char ip_addr[17] = {0};
    CoAPIntContext *ctx = (CoAPIntContext *)context;

    if (NULL == context) {
        return COAP_ERROR_NULL;
    }

    HAL_Wifi_Get_IP(ip_addr, NULL);

    while (1) {
        memset(&remote, 0x00, sizeof(NetworkAddr));
        memset(ctx->recvbuf, 0x00, COAP_MSG_MAX_PDU_LEN);
        len = CoAPNetwork_read(ctx->p_network,
                               &remote,
                               ctx->recvbuf,
                               COAP_MSG_MAX_PDU_LEN, timeout);
        if (strncmp((const char *)ip_addr, (const char *)remote.addr, sizeof(ip_addr)) == 0) /* drop the packet from itself*/
            continue;
        if (len > 0) {
            CoAPMessage_handle(ctx, &remote, ctx->recvbuf, len);
        } else {
            return len;
        }
    }
}

static void Check_timeout (void *context)
{
    CoAPIntContext *ctx = (CoAPIntContext *)context;
    CoAPSendNode *node = NULL, *next = NULL, *timeout_node = NULL;
    uint64_t tick = HAL_UptimeMs ();
    do {
        timeout_node = NULL;
        HAL_MutexLock(ctx->sendlist.list_mutex);
        list_for_each_entry_safe(node, next, &ctx->sendlist.list, sendlist, CoAPSendNode) {

            if (node->keep != NOKEEP) {
                continue;
            }
            if ((node->retrans_count > 0) || (node->timeout >= tick)) {
                continue;
            }

            /*Remove the node from the list*/
            list_del_init(&node->sendlist);
            ctx->sendlist.count--;
            COAP_INFO("Retransmit timeout,remove the message id %d count %d",
                              node->header.msgid, ctx->sendlist.count);
            #ifndef COAP_OBSERVE_SERVER_DISABLE
                CoapObsServerAll_delete(ctx, &node->remote);
            #endif
            timeout_node = node;
            break;
        }
        HAL_MutexUnlock(ctx->sendlist.list_mutex);

        if (timeout_node) {
            if(NULL != timeout_node->handler){
                timeout_node->handler(ctx, COAP_RECV_RESP_TIMEOUT, timeout_node->user, &timeout_node->remote, NULL);
            }
            coap_free(timeout_node->message);
            coap_free(timeout_node);
        }
    } while (timeout_node);
}

static void Retansmit (void *context)
{
    CoAPIntContext *ctx = (CoAPIntContext *)context;
    CoAPSendNode *node = NULL, *next = NULL;
    unsigned int ret = 0;

    uint64_t tick = HAL_UptimeMs (); 
    HAL_MutexLock(ctx->sendlist.list_mutex);
    list_for_each_entry_safe(node, next, &ctx->sendlist.list, sendlist, CoAPSendNode) {
        if (NULL == node || node->timeout > tick ) {
            continue;
        }    

        if (node->retrans_count > 0) {
            /*If has received ack message, don't resend the message*/
            if(0 == node->acked){
                COAP_DEBUG("Retansmit the message id %d len %d", node->header.msgid, node->msglen);
                ret = CoAPNetwork_write(ctx->p_network, &node->remote, node->message, node->msglen, ctx->waittime);
                if (ret != COAP_SUCCESS) {
                }    
            }
            node->timeout_val = node->timeout_val * 3 / 2;
            -- node->retrans_count;
            if (node->retrans_count == 0) {
                node->timeout = tick + COAP_ACK_TIMEOUT;
            } else {
                node->timeout = tick + node->timeout_val;
            }

            COAP_FLOW("node->timeout_val = %d , node->timeout=%d ,tick=%d", node->timeout_val,node->timeout,tick);
        }
    }
    HAL_MutexUnlock(ctx->sendlist.list_mutex);
}

extern void *coap_yield_mutex;

int CoAPMessage_cycle(CoAPContext *context)
{
    int res = 0;

    CoAPIntContext *ctx = (CoAPIntContext *)context;

    if (NULL == context) {
        return COAP_ERROR_NULL;
    }

    if (coap_yield_mutex != NULL) {
        HAL_MutexLock(coap_yield_mutex);
    }

    res = CoAPMessage_process(ctx, ctx->waittime);
    Retansmit (ctx);
    Check_timeout (ctx);

    if (coap_yield_mutex != NULL) {
        HAL_MutexUnlock(coap_yield_mutex);
    }

    if (res < 0) {
        HAL_SleepMs(20);
    }

    return res;
}

