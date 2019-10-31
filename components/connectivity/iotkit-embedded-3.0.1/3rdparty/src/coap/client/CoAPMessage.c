/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_coap_internal.h"
#include "Cloud_CoAPExport.h"
#include "CoAPSerialize.h"
#include "CoAPDeserialize.h"
#include "Cloud_CoAPPlatform.h"


#define COAPAckMsg(header) \
    ((header.code == COAP_MSG_CODE_EMPTY_MESSAGE) \
     &&(header.type == COAP_MESSAGE_TYPE_ACK))

#define Cloud_CoAPRespMsg(header)\
    ((header.code >= 0x40) && (header.code < 0xc0))

#define Cloud_CoAPPingMsg(header)\
    ((header.code == COAP_MSG_CODE_EMPTY_MESSAGE)\
     && (header.type == COAP_MESSAGE_TYPE_CON))

#define Cloud_CoAPRstMsg(header)\
    (header.type == COAP_MESSAGE_TYPE_RST)

#define Cloud_CoAPCONRespMsg(header)\
    ((header.code == COAP_MSG_CODE_205_CONTENT) \
     && (header.type == COAP_MESSAGE_TYPE_CON))

#define Cloud_CoAPReqMsg(header)\
    ((1 <= header.code) && (32 > header.code))


#define COAP_CUR_VERSION        1
#define COAP_WAIT_TIME_MS       2000
#define COAP_MAX_MESSAGE_ID     65535
#define COAP_MAX_RETRY_COUNT    4
#define COAP_ACK_TIMEOUT        2
#define COAP_ACK_RANDOM_FACTOR  1
#define COAP_MAX_TRANSMISSION_SPAN   10

unsigned short Cloud_CoAPMessageId_gen(Cloud_CoAPContext *context)
{
    unsigned short msg_id = 0;
    msg_id = ((COAP_MAX_MESSAGE_ID == context->message_id)  ? 1 : context->message_id++);
    return msg_id;
}

int Cloud_CoAPMessageHandler_set(Cloud_CoAPMessage *message, Cloud_CoAPRespMsgHandler resp)
{
    if (NULL == message) {
        return COAP_ERROR_NULL;
    }
    message->resp = resp;
    return COAP_SUCCESS;
}

static int Cloud_CoAPMessageList_add(Cloud_CoAPContext *context, Cloud_CoAPMessage *message, int len)
{
    Cloud_CoAPSendNode *node = NULL;
    node = coap_malloc(sizeof(Cloud_CoAPSendNode));

    if (NULL != node) {
        node->acked        = 0;
        node->user         = message->user;
        node->msgid        = message->header.msgid;
        node->resp = message->resp;
        node->msglen       = len;
        node->timeout_val   = COAP_ACK_TIMEOUT * COAP_ACK_RANDOM_FACTOR;

        if (COAP_MESSAGE_TYPE_CON == message->header.type) {
            node->timeout       = node->timeout_val;
            node->retrans_count = 0;
        } else {
            node->timeout       = COAP_MAX_TRANSMISSION_SPAN;
            node->retrans_count = COAP_MAX_RETRY_COUNT;
        }
        node->tokenlen     = message->header.tokenlen;
        memcpy(node->token, message->token, message->header.tokenlen);
        node->message      = (unsigned char *)coap_malloc(len);
        if (NULL != node->message) {
            memcpy(node->message, context->sendbuf, len);
        }

        if (&context->list.count >= &context->list.maxcount) {
            coap_free(node);
            return -1;
        } else {
            list_add_tail(&node->sendlist, &context->list.sendlist);
            context->list.count ++;
            return 0;
        }
    } else {
        return -1;
    }
}

int Cloud_CoAPMessage_send(Cloud_CoAPContext *context, Cloud_CoAPMessage *message)
{
    unsigned int   ret            = COAP_SUCCESS;
    unsigned short msglen         = 0;

    if (NULL == message || NULL == context) {
        return (COAP_ERROR_INVALID_PARAM);
    }

    /* TODO: get the message length */
    /* msglen = CoAPSerialize_MessageLength(message); */
    msglen = CoAPSerialize_MessageLength(message);
    if (COAP_MSG_MAX_PDU_LEN < msglen) {
        COAP_INFO("The message length %d is too loog", msglen);
        return COAP_ERROR_DATA_SIZE;
    }

    memset(context->sendbuf, 0x00, COAP_MSG_MAX_PDU_LEN);
    msglen = CoAPSerialize_Message(message, context->sendbuf, COAP_MSG_MAX_PDU_LEN);
    COAP_DEBUG("----The message length %d-----", msglen);


    ret = Cloud_CoAPNetwork_write(&context->network, context->sendbuf, (unsigned int)msglen);
    if (COAP_SUCCESS == ret) {
        if (Cloud_CoAPReqMsg(message->header) || Cloud_CoAPCONRespMsg(message->header)) {
            COAP_DEBUG("Add message id %d len %d to the list",
                       message->header.msgid, msglen);
            Cloud_CoAPMessageList_add(context, message, msglen);
        } else {
            COAP_DEBUG("The message doesn't need to be retransmitted");
        }
    } else {
        COAP_ERR("CoAP transport write failed, return %d", ret);
    }

    return ret;
}


static int Cloud_CoAPAckMessage_handle(Cloud_CoAPContext *context, Cloud_CoAPMessage *message)
{
    Cloud_CoAPSendNode *node = NULL;

    list_for_each_entry(node, &context->list.sendlist, sendlist, Cloud_CoAPSendNode) {
        if (node->msgid == message->header.msgid) {
            node->acked = 1;
            return COAP_SUCCESS;
        }
    }

    return COAP_SUCCESS;
}

static int Cloud_CoAPAckMessage_send(Cloud_CoAPContext *context, unsigned short msgid)
{
    Cloud_CoAPMessage message;
    CoAPMessage_init(&message);
    CoAPMessageId_set(&message, msgid);
    return Cloud_CoAPMessage_send(context, &message);
}

static int Cloud_CoAPRespMessage_handle(Cloud_CoAPContext *context, Cloud_CoAPMessage *message)
{
    Cloud_CoAPSendNode *node = NULL;

    if (COAP_MESSAGE_TYPE_CON == message->header.type) {
        Cloud_CoAPAckMessage_send(context, message->header.msgid);
    }


    list_for_each_entry(node, &context->list.sendlist, sendlist, Cloud_CoAPSendNode) {
        if (0 != node->tokenlen && node->tokenlen == message->header.tokenlen
            && 0 == memcmp(node->token, message->token, message->header.tokenlen)) {

#ifdef INFRA_LOG_NETWORK_PAYLOAD
            COAP_DEBUG("Find the node by token");
            COAP_INFO("Downstream Payload:");
            iotx_facility_json_print((const char *)message->payload, LOG_INFO_LEVEL, '<');
#endif
            message->user  = node->user;
            if (COAP_MSG_CODE_400_BAD_REQUEST <= message->header.code) {
                /* TODO:i */
                if (NULL != context->notifier) {
                    /* context->notifier(message->header.code, message); */
                }
            }

            if (NULL != node->resp) {
                node->resp(node->user, message);
            }
            COAP_DEBUG("Remove the message id %d from list", node->msgid);
            list_del_init(&node->sendlist);
            context->list.count--;
            if (NULL != node->message) {
                coap_free(node->message);
            }
            coap_free(node);
            node = NULL;
            return COAP_SUCCESS;
        }
    }
    return COAP_ERROR_NOT_FOUND;
}

static void Cloud_CoAPMessage_handle(Cloud_CoAPContext *context,
                                     unsigned char     *buf,
                                     unsigned short      datalen)
{
    int    ret  = COAP_SUCCESS;
    Cloud_CoAPMessage     message;
    unsigned char code, msgclass, detail;
    memset(&message, 0x00, sizeof(Cloud_CoAPMessage));

    ret = CoAPDeserialize_Message(&message, buf, datalen);
    code = (unsigned char)message.header.code;
    msgclass = code >> 5;
    detail = code & 0x1F;

    COAP_DEBUG("Version     : %d", message.header.version);
    COAP_DEBUG("Code        : %d.%02d(0x%x)", msgclass, detail, code);
    COAP_DEBUG("Type        : 0x%x", message.header.type);
    COAP_DEBUG("Msgid       : %d", message.header.msgid);
    COAP_DEBUG("Option      : %d", message.optcount);
    COAP_DEBUG("Payload Len : %d", message.payloadlen);

    msgclass = msgclass;
    detail = detail;

    if (COAP_SUCCESS != ret) {
        if (NULL != context->notifier) {
            /* TODO: */
            /* context->notifier(context, event); */
        }
    }

    if (COAPAckMsg(message.header)) {
        COAP_DEBUG("Receive CoAP ACK Message,ID %d", message.header.msgid);
        Cloud_CoAPAckMessage_handle(context, &message);

    } else if (Cloud_CoAPRespMsg(message.header)) {
        COAP_DEBUG("Receive CoAP Response Message,ID %d", message.header.msgid);
        Cloud_CoAPRespMessage_handle(context, &message);
    }
}

int Cloud_CoAPMessage_recv(Cloud_CoAPContext *context, unsigned int timeout, int readcount)
{
    int len = 0;
    int count = readcount;

    while (1) {
        len = Cloud_CoAPNetwork_read(&context->network, context->recvbuf,
                                     COAP_MSG_MAX_PDU_LEN, timeout);
        if (len > 0) {
            if (0 == readcount) {
                Cloud_CoAPMessage_handle(context, context->recvbuf, len);
            } else {
                count--;
                Cloud_CoAPMessage_handle(context, context->recvbuf, len);
                if (0 == count) {
                    return len;
                }
            }
        } else {
            return 0;
        }
    }
}

int Cloud_CoAPMessage_cycle(Cloud_CoAPContext *context)
{
    unsigned int ret = 0;
    Cloud_CoAPSendNode *node = NULL, *next = NULL;
    Cloud_CoAPMessage_recv(context, context->waittime, 0);

    list_for_each_entry_safe(node, next, &context->list.sendlist, sendlist, Cloud_CoAPSendNode) {
        if (NULL != node) {
            if (node->timeout == 0) {
                if (node->retrans_count < COAP_MAX_RETRY_COUNT && (0 == node->acked)) {
                    node->timeout     = node->timeout_val * 2;
                    node->timeout_val = node->timeout;
                    node->retrans_count++;
                    COAP_DEBUG("Retansmit the message id %d len %d", node->msgid, node->msglen);
                    ret = Cloud_CoAPNetwork_write(&context->network, node->message, node->msglen);
                    if (ret != COAP_SUCCESS) {
                        if (NULL != context->notifier) {
                            /* TODO: */
                            /* context->notifier(context, event); */
                        }
                    }
                }

                if ((node->timeout > COAP_MAX_TRANSMISSION_SPAN) ||
                    (node->retrans_count >= COAP_MAX_RETRY_COUNT)) {
                    if (NULL != context->notifier) {
                        /* TODO: */
                        /* context->notifier(context, event); */
                    }

                    /*Remove the node from the list*/
                    list_del_init(&node->sendlist);
                    context->list.count--;
                    COAP_INFO("Retransmit timeout,remove the message id %d count %d",
                              node->msgid, context->list.count);
                    coap_free(node->message);
                    coap_free(node);
                }
            } else {
                node->timeout--;
            }
        }
    }
    return COAP_SUCCESS;
}

