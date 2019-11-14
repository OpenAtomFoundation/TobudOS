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

#ifndef _DATA_TEMPLATE_EVENT_H_
#define _DATA_TEMPLATE_EVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

#define NAME_MAX_LEN			(32)
#define TYPE_MAX_LEN			(32)
#define EVENT_TOKEN_MAX_LEN		(32)
#define SIGLE_EVENT			    (1) 
#define MUTLTI_EVENTS		    (2)


#define MAX_EVENT_WAIT_REPLY    (10) 
#define EVENT_MAX_DATA_NUM		(255)

#define POST_EVENT				"event_post"
#define POST_EVENTS				"events_post"
#define REPLY_EVENT				"event_reply"

		
/**
 * @brief event's method, post and reply
 */
typedef enum {
    eEVENT_POST,    
    eEVENT_REPLY, 
} eEventMethod;


typedef enum _eEventDealType_{
	eDEAL_REPLY_CB = 0,
	eDEAL_EXPIRED = 1,	
}eEventDealType;

typedef struct _sReply_{
    char       client_token[EVENT_TOKEN_MAX_LEN];               // clientToken for this event reply
    void       *user_context;                                   // user context
    Timer      timer;                                           // timer for request timeout

    OnEventReplyCallback      callback;                         // callback for this event reply
} sEventReply;


#ifdef __cplusplus
}
#endif
#endif //_DATA_TEMPLATE_EVENT_H_