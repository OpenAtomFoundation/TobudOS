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

#ifndef QCLOUD_IOT_EXPORT_DATA_TEMPLATE_H_
#define QCLOUD_IOT_EXPORT_DATA_TEMPLATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_export_mqtt.h"
#include "qcloud_iot_export_method.h"

#define  MAX_CONTORL_REPLY_STATUS_LEN		64		   // max len of status within control reply msg 

/**
 * @brief Data type of template
 */

#define TYPE_TEMPLATE_INT    	JINT32
#define TYPE_TEMPLATE_ENUM    	JINT32
#define TYPE_TEMPLATE_FLOAT  	JFLOAT
#define TYPE_TEMPLATE_BOOL   	JINT8
#define TYPE_TEMPLATE_STRING 	JSTRING
#define TYPE_TEMPLATE_TIME 		JUINT32
#define TYPE_TEMPLATE_JOBJECT 	JOBJECT

typedef int32_t   TYPE_DEF_TEMPLATE_INT;
typedef int32_t   TYPE_DEF_TEMPLATE_ENUM;
typedef float     TYPE_DEF_TEMPLATE_FLOAT;
typedef char      TYPE_DEF_TEMPLATE_BOOL;
typedef char      TYPE_DEF_TEMPLATE_STRING;
typedef uint32_t  TYPE_DEF_TEMPLATE_TIME;
typedef void *    TYPE_DEF_TEMPLATE_OBJECT;

#ifdef EVENT_POST_ENABLED					//enable event function of data_template

#define TYPE_STR_INFO			"info"
#define TYPE_STR_ALERT			"alert"
#define TYPE_STR_FAULT			"fault"

/*If defined,event's timesamp should be accurate UTC timestamp in millisecond  */
#define  EVENT_TIMESTAMP_USED			   

#define  FLAG_EVENT0 			(1U<<0)
#define  FLAG_EVENT1			(1U<<1)
#define  FLAG_EVENT2			(1U<<2)
#define  FLAG_EVENT3			(1U<<3)
#define  FLAG_EVENT4 			(1U<<4)
#define  FLAG_EVENT5			(1U<<5)
#define  FLAG_EVENT6			(1U<<6)
#define  FLAG_EVENT7			(1U<<7)
#define  FLAG_EVENT8 			(1U<<8)
#define  FLAG_EVENT9			(1U<<9)

#define  ALL_EVENTS_MASK		(0xFFFFFFFF)


typedef enum {
	eEVENT_INFO,
	eEVENT_ALERT,    
    eEVENT_FAULT, 
}eEventType;

typedef struct  _sEvent_{
	char 	 *event_name;		 // event name
	char 	 *type;			 	 // event type
    uint32_t timestamp;			 // event timestamp
	uint8_t eventDataNum;		 // number of event properties
    DeviceProperty *pEventData;  // event properties
} sEvent;

#endif

/* The structure of data_template init parameters */
typedef struct {
    /* device info */
    char                        *product_id;            	// product ID
	char						*device_name;			 	// device name
	
#ifdef AUTH_MODE_CERT
	char						*cert_file; 			 	// cert file path
	char						*key_file;				 	// key file path
#else
	char						*device_secret; 		 	// device secret
#endif

	uint32_t					command_timeout;			// timeout value (unit: ms) for MQTT connect/pub/sub/yield
	uint32_t					keep_alive_interval_ms; 	// MQTT keep alive time interval in millisecond

	uint8_t 					clean_session;				// flag of clean session, 1 clean, 0 not clean

	uint8_t 					auto_connect_enable;		// flag of auto reconnection, 1 is enable and recommended

    MQTTEventHandler            event_handle;             	// event callback
	
} TemplateInitParams;

#ifdef AUTH_MODE_CERT
    #define DEFAULT_TEMPLATE_INIT_PARAMS { NULL, NULL, NULL, NULL, 2000, 240 * 1000, 1, 1, {0}}
#else
    #define DEFAULT_TEMPLATE_INIT_PARAMS { NULL, NULL, NULL, 2000, 240 * 1000, 1, 1, {0}}
#endif


typedef enum _eReplyCode_{
		eDEAL_SUCCESS = 0,
		eDEAL_FAIL = -1,
}eReplyCode;

/**
 * @brief control msg reply parameter
 */
typedef struct _sReplyPara {
    uint32_t  	 timeout_ms;         						      // request timeout time, unit:ms   
    eReplyCode   code;    							  		  	  // reply code. 0:success, ~0:failed
    char      	 status_msg[MAX_CONTORL_REPLY_STATUS_LEN];       //  reply message

} sReplyPara;


/**
 * @brief Define property status in data template
 */
typedef enum _eDataState_{
    eNOCHANGE = 0,
	eCHANGED = 1,	
} eDataState;

/**
 * @brief Define data point in data template
 */
typedef struct {
    DeviceProperty data_property;
    eDataState state;
} sDataPoint;


/**
 * @brief Create data_template client and connect to MQTT server
 *
 * @param pParams data_template init parameters
 *
 * @return a valid data_template client handle when success, or NULL otherwise
 */ 
void* IOT_Template_Construct(TemplateInitParams *pParams);

/**
 * @brief Publish MQTT message
 *
 * @param pClient       handle to data_template client 
 * @param topicName     MQTT topic name
 * @param pParams       publish parameters
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */ 
int IOT_Template_Publish(void *handle, char *topicName, PublishParams *pParams);


/**
 * @brief Subscribe MQTT message
 *
 * @param pClient       handle to data_template client 
 * @param topicFilter   MQTT topic filter
 * @param pParams       subscribe parameters
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */ 
int IOT_Template_Subscribe(void *handle, char *topicFilter, SubscribeParams *pParams);

/**
 * @brief Unsubscribe MQTT message
 *
 * @param pClient       handle to data_template client 
 * @param topicFilter   MQTT topic filter
 *
 * @return packet id (>=0) when success, or err code (<0) for failure
 */
int IOT_Template_Unsubscribe(void *handle, char *topicFilter);

/**
 * @brief Check if MQTT data_template is connected
 *
 * @param pClient       handle to data_template client 
 * @return true= connected, false = unconnected
 */ 
bool IOT_Template_IsConnected(void *handle);

/**
 * @brief Close connection and destroy MQTT data_template client
 *
 * @param pClient    pointer of handle to data_template client
 *
 * @return QCLOUD_RET_SUCCESS for success, or err code for failure
 */ 
int IOT_Template_Destroy(void *handle);

/**
 * @brief Check connection and keep alive state, read/handle MQTT message in synchronized way
 *
 * @param pClient    handle to data_template client
 * @param timeout_ms timeout value (unit: ms) for this operation
 *
 * @return QCLOUD_RET_SUCCESS when success, or err code for failure
 */ 
int IOT_Template_Yield(void *handle, uint32_t timeout_ms);
 
/**
 * @brief Register device property
 *
 * @param pClient           handle to data_template client
 * @param pProperty         reference to device property
 * @param callback          callback when property changes
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure
 */ 
int IOT_Template_Register_Property(void *handle, DeviceProperty *pProperty, OnPropRegCallback callback);
 
/**
 * @brief UnRegister device property
 *
 * @param pClient           handle to data_template client
 * @param pProperty         reference to device property
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure
 */ 
int IOT_Template_UnRegister_Property(void *handle, DeviceProperty *pProperty);

#ifdef ACTION_ENABLED
int IOT_Template_Register_Action(void *handle, DeviceAction *pAction, OnActionHandleCallback callback);

int IOT_Template_UnRegister_Action(void *handle, DeviceAction *pAction);
#endif


/**
 * @brief Add reported fields array in JSON document, don't overwrite
 *
 * @param pClient       handle to data_template client
 * @param jsonBuffer    string buffer to store JSON document
 * @param sizeOfBuffer  size of string buffer
 * @param count         number of properties
 * @param pDeviceProperties         array of properties
 * @return              QCLOUD_RET_SUCCESS when success, or err code for failure
 */ 
int IOT_Template_JSON_ConstructReportArray(void *handle, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count, DeviceProperty *pDeviceProperties[]); 


/**
 * @brief report data_template data in asynchronized way
 *
 * @param pClient           handle to data_template client
 * @param pJsonDoc          source JSON document for report
 * @param sizeOfBuffer      length of JSON document
 * @param callback          callback when response arrive
 * @param userContext       user data for callback
 * @param timeout_ms        timeout value for this operation (unit: ms)
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure
 */ 
int IOT_Template_Report(void *handle, char *pJsonDoc, size_t sizeOfBuffer, OnReplyCallback callback, void *userContext, uint32_t timeout_ms);

/**
 * @brief report data_template data in synchronized way
 *
 * @param pClient           handle to data_template client
 * @param pJsonDoc          source JSON document for report
 * @param sizeOfBuffer      length of JSON document
 * @param timeout_ms        timeout value for this operation (unit: ms)
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure

 */ 
int IOT_Template_Report_Sync(void *handle, char *pJsonDoc, size_t sizeOfBuffer, uint32_t timeout_ms);

/**
 * @brief Get data_template state from server in asynchronized way.
 * Generally it's a way to sync data_template data during offline
 *
 * @param pClient           handle to data_template client
 * @param callback          callback when response arrive
 * @param userContext       user data for callback
 * @param timeout_ms        timeout value for this operation (unit: ms)
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure
 */  
int IOT_Template_GetStatus(void *handle, OnReplyCallback callback, void *userContext, uint32_t timeout_ms);
 
/**
 * @brief Get Get data_template state from server in asynchronized way
 *
 * @param pClient           handle to data_template client
 * @param timeout_ms        timeout value for this operation (unit: ms)
 * @return                  QCLOUD_RET_SUCCESS when success, or err code for failure
 */  
int IOT_Template_GetStatus_sync(void *handle, uint32_t timeout_ms);


/**
 * @brief  clear the control msg when IOT_Template_GetStatus get control msg 
 * @param pClient		  handle to data_template client
 * @param pClientToken	  correspond to the clientToken of control msg 
 * @return				  QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_Template_ClearControl(void *handle, char *pClientToken, OnReplyCallback callback, uint32_t timeout_ms); 

/**
 * @brief  reply to the control msg 
 * @param pClient		  handle to data_template client
 * @param pJsonDoc	  	  data buffer for reply
 * @param sizeOfBuffer    length of data buffer
 * @param replyPara       reply info
 * @return				  QCLOUD_RET_SUCCESS when success, or err code for failure
 */ 
int IOT_Template_ControlReply(void *handle, char *pJsonDoc, size_t sizeOfBuffer, sReplyPara *replyPara);

/**
 * @brief  construct system information in json format
 * @param pClient		   handle to data_template client
 * @param jsonBuffer	   data buffer for construct
 * @param sizeOfBuffer	   length of data buffer
 * @param pPlatInfo		   pointer of platform info, compulsory
 * @param pSelfInfo		   pointer of self-define info, option 
 * @return				  QCLOUD_RET_SUCCESS when success, or err code for failure

 *package format for system info report
* {
*  "method": "report_info",
*  "clientToken": "client-token1618",
*  "params": {
*  "module_hardinfo": "esp8266",	//if module used, compulsory
*  "module_softinfo":  "APP_2.0.0", //compulsory
*  "fw_ver":       "FW_1.0.0",		//compulsory
*  "imei":       "123456789",		//option
*  "lat":        "22.546015"		//latitude,option
*  "lon":        "113.941125",		//longitude,option
*  "device_label": {
*    "append_info": "your self define info" //self-define information,option
* ...
*   }
* }
*/
int IOT_Template_JSON_ConstructSysInfo(void *handle, char *jsonBuffer, size_t sizeOfBuffer, DeviceProperty *pPlatInfo, DeviceProperty *pSelfInfo); 
 
/**
 * @brief report system information in asynchronized way
 *
 * @param pClient			handle to data_template client
 * @param pJsonDoc			source JSON document for report
 * @param sizeOfBuffer		length of JSON document
 * @param callback			callback when response arrive
 * @param userContext		user data for callback
 * @param timeout_ms		timeout value for this operation (unit: ms)
 * @return					QCLOUD_RET_SUCCESS when success, or err code for failure
 */ 
int IOT_Template_Report_SysInfo(void *handle, char *pJsonDoc, size_t sizeOfBuffer, OnReplyCallback callback, void *userContext, uint32_t timeout_ms);

	 
/**
 * @brief report data_template data in synchronized way
 *
 * @param pClient			handle to data_template client
 * @param pJsonDoc			source JSON document for report
 * @param sizeOfBuffer		length of JSON document
 * @param callback			callback when response arrive
 * @param userContext		user data for callback
 * @param timeout_ms		timeout value for this operation (unit: ms)
 * @return					QCLOUD_RET_SUCCESS when success, or err code for failure
 */
int IOT_Template_Report_SysInfo_Sync(void *handle, char *pJsonDoc, size_t sizeOfBuffer, uint32_t timeout_ms);


#ifdef EVENT_POST_ENABLED
/**
 * @brief callback of event reply
 *
 * @param client    handle to data_template client
 * @param msg		event reply msg
 *
 */
typedef void (*OnEventReplyCallback)(void *client, MQTTMessage *msg);

 
/**
 * @brief set events flag when events occured
 *
 * @param client    handle to data_template client
 * @param flag		event flags for set. per bit of 32bits represent one event. 
 *
 */ 
void IOT_Event_setFlag(void *client, uint32_t flag);


/**
 * @brief  clear events flag after events dealed
 *
 * @param client    handle to data_template client
 * @param flag		event flags for clear. per bit of 32bits represent one event. 
 *
 */ 
void IOT_Event_clearFlag(void *client, uint32_t flag);


/**
 * @brief get events flag setted
 *
 * @param client    handle to data_template client
 * @return			events flag status
 *
 */  
uint32_t IOT_Event_getFlag(void *client);

/**
 * @brief init event function of data_template client
 *
 * @param c    handle to data_template client
 */
int IOT_Event_Init(void *c);


/**
 * @brief handle event wait for reply timeout
 * 
 * @param client   handle to data_template client
 */
void handle_template_expired_event(void *client);


/**
 * @brief post event to cloud, SDK construct event json package
 * @param pClient 		  handle to data_template client
 * @param pJsonDoc	  	  data buffer for event post
 * @param sizeOfBuffer    length of data buffer
 * @param event_count     event counts to post
 * @param pEventArry	  pointer of events array to post
 * @param replyCb	      callback when event reply received
 * @return @see IoT_Error_Code	  
 */
int IOT_Post_Event(void *pClient, char *pJsonDoc, size_t sizeOfBuffer, uint8_t event_count, sEvent *pEventArry[], OnEventReplyCallback replyCb);                                            

/**
 * @brief post event to cloud, user input raw event data
 * @param pClient handle to data_template client
 * @param pJsonDoc    data buffer for event post
 * @param sizeOfBuffer    length of data buffer
 * @param pEventMsg       user input event raw data in json format
 *  event package format:
 *	 {"method": "event_post",
 *		"clientToken": "123",
 *		"version": "1.0",
 *		"eventId": "PowerAlarm",
 *		"type": "fatal",
 *		"timestamp": 1212121221,
 *		"params": {
 *			"Voltage": 2.8,
 *			"Percent": 20
 *		}
 *	}
 
 * 	pEventMsg for example
 *  single event:
 *	 {
 *		 "eventId": "PowerAlarm",
 *		 "type": "fatal",
 *		 "timestamp": 1212121221,
 *		 "params": {
 *			 "Voltage": 2.8,
 *			 "Percent": 20
 *		 }
 *	 }
 *
 *  multi event:
 *	 {
 *		 "eventId": "PowerAlarm",
 *		 "type": "fatal",
 *		 "timestamp": 1212121221,
 *		 "params": {
 *			 "Voltage": 2.8,
 *			 "Percent": 20
 *		 }
 *	 },
 *	 {
 *		 "name": "PowerAlarm",
 *		 "type": "fatal",
 *		 "timestamp": 1212121223,
 *		 "params": {
 *			 "Voltage": 2.1,
 *			 "Percent": 10
 *		 }
 *	 },
 *   ....
 *
 * @param replyCb	  event callback when event reply received
 * @return @see IoT_Error_Code	  
 */
int IOT_Post_Event_Raw(void *pClient, char *pJsonDoc, size_t sizeOfBuffer, char *pEventMsg, OnEventReplyCallback replyCb);            

#endif

#ifdef ACTION_ENABLED
/**
* @brief  reply to the action msg 
* @param pClient		  handle to data_template client
* @param pClientToken	  correspond to the clientToken of action msg 
* @param pJsonDoc	  	  data buffer for reply
* @param sizeOfBuffer     length of data buffer
* @param pAction 		  pointer of action 	
* @param replyPara        action reply info
* @return				  QCLOUD_RET_SUCCESS when success, or err code for failure
*/ 

int IOT_ACTION_REPLY(void *pClient, const char *pClientToken, char *pJsonDoc, size_t sizeOfBuffer, DeviceAction *pAction, sReplyPara *replyPara);
#endif

#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_TEMPLATE_H_ */
