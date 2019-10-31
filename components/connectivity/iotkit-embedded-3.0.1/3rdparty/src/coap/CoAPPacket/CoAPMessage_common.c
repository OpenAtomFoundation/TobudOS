/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */





#include <stdio.h>
#include "iotx_coap_internal.h"
#include "CoAPSerialize.h"
#include "CoAPDeserialize.h"
#if 0
#include "CoAPResource.h"
#include "CoAPObserve.h"
#include "CoAPInternal.h"
#endif
#include "CoAPPlatform.h"

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


#define COAP_CUR_VERSION        1
#define COAP_WAIT_TIME_MS       2000
#define COAP_MAX_MESSAGE_ID     65535
#define COAP_MAX_RETRY_COUNT    4
#define COAP_ACK_TIMEOUT        2
#define COAP_ACK_RANDOM_FACTOR  1
#define COAP_MAX_TRANSMISSION_SPAN   10

int CoAPStrOption_add(CoAPMessage *message, unsigned short optnum, unsigned char *data, unsigned short datalen)
{
    unsigned char *ptr = NULL;
    if (COAP_MSG_MAX_OPTION_NUM <= message->optcount) {
        return COAP_ERROR_INVALID_PARAM;
    }

    message->options[message->optcount].num = optnum - message->optdelta;
    message->options[message->optcount].len = datalen;
    ptr = (unsigned char *)coap_malloc(datalen);
    if (NULL == ptr) {
        return COAP_ERROR_MALLOC;
    }
    memset(ptr, 0x00, datalen);
    memcpy(ptr, data, datalen);
    message->options[message->optcount].val = ptr;
    message->optdelta = optnum;
    message->optcount ++;

    return COAP_SUCCESS;

}

int CoAPStrOption_get(CoAPMessage *message, unsigned short optnum, unsigned char *data, unsigned short *datalen)
{
    unsigned char index = 0;

    for (index = 0; index < message->optcount; index++) {
        if (message->options[index].num == optnum) {
            if (*datalen >= message->options[index].len) {
                memcpy(data, message->options[index].val, message->options[index].len);
                *datalen = message->options[index].len;
                return COAP_SUCCESS;
            } else {
                return COAP_ERROR_INVALID_LENGTH;
            }
        }
    }

    return COAP_ERROR_NOT_FOUND;

}


int CoAPUintOption_add(CoAPMessage *message, unsigned short  optnum, unsigned int data)
{
    unsigned char *ptr = NULL;
    if (COAP_MSG_MAX_OPTION_NUM <= message->optcount) {
        return COAP_ERROR_INVALID_PARAM;
    }
    message->options[message->optcount].num = optnum - message->optdelta;

    if (0 == data) {
        message->options[message->optcount].len = 0;
    } else if (255 >= data) {
        message->options[message->optcount].len = 1;
        ptr = (unsigned char *)coap_malloc(1);
        if (NULL != ptr) {
            *ptr = (unsigned char)data;
        }
    } else if (65535 >= data) {
        message->options[message->optcount].len = 2;
        ptr  = (unsigned char *)coap_malloc(2);
        if (NULL != ptr) {
            *ptr     = (unsigned char)((data & 0xFF00) >> 8);
            *(ptr + 1) = (unsigned char)(data & 0x00FF);
        }
    } else {
        message->options[message->optcount].len = 4;
        ptr   = (unsigned char *)coap_malloc(4);
        if (NULL != ptr) {
            *ptr     = (unsigned char)((data & 0xFF000000) >> 24);
            *(ptr + 1) = (unsigned char)((data & 0x00FF0000) >> 16);
            *(ptr + 2) = (unsigned char)((data & 0x0000FF00) >> 8);
            *(ptr + 3) = (unsigned char)(data & 0x000000FF);
        }
    }
    message->options[message->optcount].val = ptr;
    message->optdelta = optnum;
    message->optcount   += 1;

    return COAP_SUCCESS;
}

int CoAPUintOption_get(CoAPMessage *message,
                       unsigned short  optnum,
                       unsigned int *data)
{

    unsigned char index = 0;

    for (index = 0; index < message->optcount; index++) {
        if (message->options[index].num == optnum) {
            int byte = 0;
            switch (message->options[index].len) {
                case 1:
                    *data |= message->options[index].val[byte++];
                    break;
                case 2:
                    *data |= (message->options[index].val[byte++] << 8);
                    *data |= message->options[index].val[byte++];
                    break;
                case 3:
                    *data |= (message->options[index].val[byte++] << 16);
                    *data |= (message->options[index].val[byte++] << 8);
                    *data |= message->options[index].val[byte++];
                    break;
                case 4:
                    *data |= (message->options[index].val[byte++] << 24);
                    *data |= (message->options[index].val[byte++] << 16);
                    *data |= (message->options[index].val[byte++] << 8);
                    *data |= message->options[index].val[byte++];
                    break;
                default:
                    *data = 0;
                    break;
            }
            return COAP_SUCCESS;
        }
    }

    return COAP_ERROR_NOT_FOUND;
}


int CoAPOption_present(CoAPMessage *message, unsigned short option)
{
    unsigned char index = 0;


    for (index = 0; index < message->optcount; index++) {
        if (message->options[index].num == option) {
            return COAP_SUCCESS;
        }
    }
    return COAP_ERROR_NOT_FOUND;
}

int CoAPMessageId_set(CoAPMessage *message, unsigned short msgid)
{
    if (NULL == message) {
        return COAP_ERROR_NULL;
    }
    message->header.msgid = msgid;
    return COAP_SUCCESS;
}

int CoAPMessageType_set(CoAPMessage *message, unsigned char type)
{
    if (NULL == message) {
        return COAP_ERROR_NULL;
    }
    if (COAP_MESSAGE_TYPE_CON != type && COAP_MESSAGE_TYPE_NON != type
        && COAP_MESSAGE_TYPE_ACK != type && COAP_MESSAGE_TYPE_RST != type) {
        return COAP_ERROR_INVALID_PARAM;
    }

    message->header.type = type;
    return COAP_SUCCESS;
}

int CoAPMessageCode_set(CoAPMessage *message, CoAPMessageCode code)
{
    if (NULL == message) {
        return COAP_ERROR_NULL;
    }
    message->header.code  = code;
    return COAP_SUCCESS;
}

int CoAPMessageCode_get(CoAPMessage *message, CoAPMessageCode *code)
{
    if (NULL == message || NULL == code) {
        return COAP_ERROR_NULL;
    }
    *code = message->header.code;
    return COAP_SUCCESS;
}

int CoAPMessageToken_set(CoAPMessage *message, unsigned char *token,
                         unsigned char tokenlen)
{
    if (NULL == message || NULL == token) {
        return COAP_ERROR_NULL;
    }
    if (COAP_MSG_MAX_TOKEN_LEN < tokenlen) {
        return COAP_ERROR_INVALID_LENGTH;
    }
    memcpy(message->token, token, tokenlen);
    message->header.tokenlen = tokenlen;

    return COAP_SUCCESS;
}

int CoAPMessageUserData_set(CoAPMessage *message, void *userdata)
{
    if (NULL == message || NULL == userdata) {
        return COAP_ERROR_NULL;
    }
    message->user = userdata;
    return COAP_SUCCESS;
}

int CoAPMessageKeep_Set(CoAPMessage *message, int keep)
{
    if (NULL == message || keep < 0) {
        return COAP_ERROR_NULL;
    }
    message->keep = keep;
    return COAP_SUCCESS;
}

int CoAPMessagePayload_set(CoAPMessage *message, unsigned char *payload,
                           unsigned short payloadlen)
{
    if (NULL == message || (0 < payloadlen && NULL == payload)) {
        return COAP_ERROR_NULL;
    }
    message->payload = payload;
    message->payloadlen = payloadlen;

    return COAP_SUCCESS;
}

int CoAPMessage_init(CoAPMessage *message)
{
    int count = 0;

    if (NULL == message) {
        return COAP_ERROR_NULL;
    }
    memset(message, 0x00, sizeof(CoAPMessage));
    message->header.version    = COAP_CUR_VERSION;
    message->header.type       = COAP_MESSAGE_TYPE_ACK;
    message->header.tokenlen   = 0;
    message->header.code       = COAP_MSG_CODE_EMPTY_MESSAGE;
    message->header.msgid      = 0;
    message->payload           = NULL;
    message->payloadlen        = 0;
    message->optcount          = 0;
    message->optdelta          = 0;
    message->handler           = NULL;
    message->keep              = 0;
    for (count = 0; count < COAP_MSG_MAX_OPTION_NUM; count++) {
        message->options[count].len = 0;
        message->options[count].num = 0;
        message->options[count].val = NULL;
    }

    return COAP_SUCCESS;
}

int CoAPMessage_destory(CoAPMessage *message)
{
    int count = 0;
    if (NULL == message) {
        return COAP_ERROR_NULL;
    }

    for (count = 0; count < COAP_MSG_MAX_OPTION_NUM; count++) {
        if (NULL != message->options[count].val) {
            coap_free(message->options[count].val);
            message->options[count].val = NULL;
        }
    }

    return COAP_SUCCESS;
}
