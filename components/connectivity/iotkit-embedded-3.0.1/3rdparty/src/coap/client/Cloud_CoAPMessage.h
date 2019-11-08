/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */




#include "Cloud_CoAPExport.h"

#ifndef __COAP_HANDLE_MSG_H__
#define __COAP_HANDLE_MSG_H__

int Cloud_CoAPStrOption_add(Cloud_CoAPMessage *message, unsigned short optnum,
            unsigned char *data, unsigned short datalen);


int Cloud_CoAPUintOption_add(Cloud_CoAPMessage *message, unsigned short  optnum,
            unsigned int data);

unsigned short Cloud_CoAPMessageId_gen(Cloud_CoAPContext *context);

int Cloud_CoAPMessageId_set(Cloud_CoAPMessage *message, unsigned short msgid);

int Cloud_CoAPMessageType_set(Cloud_CoAPMessage *message, unsigned char type);

int Cloud_CoAPMessageCode_set(Cloud_CoAPMessage *message, Cloud_CoAPMessageCode code);

int Cloud_CoAPMessageToken_set(Cloud_CoAPMessage *message, unsigned char *token,
        unsigned char tokenlen);

int Cloud_CoAPMessageUserData_set(Cloud_CoAPMessage *message, void *userdata);

int Cloud_CoAPMessagePayload_set(Cloud_CoAPMessage *message, unsigned char *payload,
        unsigned short payloadlen);

int Cloud_CoAPMessageHandler_set(Cloud_CoAPMessage *message, Cloud_CoAPRespMsgHandler handler);

int Cloud_CoAPMessage_init(Cloud_CoAPMessage *message);

int Cloud_CoAPMessage_destory(Cloud_CoAPMessage *message);

int Cloud_CoAPMessage_send(Cloud_CoAPContext *context, Cloud_CoAPMessage *message);

int Cloud_CoAPMessage_recv(Cloud_CoAPContext *context, unsigned int timeout, int readcount);

int Cloud_CoAPMessage_cycle(Cloud_CoAPContext *context);



#endif
