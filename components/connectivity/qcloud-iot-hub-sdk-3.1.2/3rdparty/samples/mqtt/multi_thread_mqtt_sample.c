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
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"

/*
 * Notes for using SDK in multi-thread programing
 * 1. IOT_MQTT_Yield, IOT_MQTT_Construct and IOT_MQTT_Destroy are NOT thread-safe, only calling them in the same thread
 * 2. IOT_MQTT_Publish, IOT_MQTT_Subscribe and IOT_MQTT_Unsubscribe are thread-safe, and be executed in multi-threads simultaneously
 * 3. IOT_MQTT_Yield is the only entry to read from socket and should not be hand up for long
 */

#if defined(__linux__)
/*
 * This sample test MQTT multithread performance in Linux pthread runtime. Please note the data format is not JSON
 */

#ifdef AUTH_MODE_CERT
    static char sg_cert_file[PATH_MAX + 1];      // full path of device cert file
    static char sg_key_file[PATH_MAX + 1];       // full path of device key file
#endif


static DeviceInfo sg_devInfo;


#define MAX_SIZE_OF_TOPIC_CONTENT 100
#define MAX_PUB_THREAD_COUNT 5
#define PUBLISH_COUNT 10
#define THREAD_SLEEP_INTERVAL_USEC 1000000
#define CONNECT_MAX_ATTEMPT_COUNT 3
#define RX_RECEIVE_PERCENTAGE 99.0f

static bool sg_terminate_yield_thread;
static bool sg_terminate_subUnsub_thread;

static unsigned int sg_countArray[MAX_PUB_THREAD_COUNT][PUBLISH_COUNT];	// record the times when msg from subscribed topic is received 
static unsigned int sg_rxMsgBufferTooBigCounter;                        // record the times when msg is oversize
static unsigned int sg_rxUnexpectedNumberCounter;                       // record the times when unexpected msg is received
static unsigned int sg_rePublishCount;									// record the times of re-publish
static unsigned int sg_wrongYieldCount;									// record the times of Yield failure
static unsigned int sg_threadStatus[MAX_PUB_THREAD_COUNT];				// record the status of all the threads 
static char sg_integeration_test_topic[MAX_SIZE_OF_TOPIC_CONTENT];		// topic for sub/pub


typedef struct ThreadData {
	int threadId;
	void *client;
} ThreadData;


void event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg) {
	MQTTMessage* mqtt_messge = (MQTTMessage*)msg->msg;
	uintptr_t packet_id = (uintptr_t)msg->msg;

	switch(msg->event_type) {
		case MQTT_EVENT_UNDEF:
			Log_i("undefined event occur.");
			break;

		case MQTT_EVENT_DISCONNECT:
			Log_i("MQTT disconnect.");
			break;

		case MQTT_EVENT_RECONNECT:
			Log_i("MQTT reconnect.");
			break;

		case MQTT_EVENT_PUBLISH_RECVEIVED:
			Log_i("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
					  mqtt_messge->topic_len,
					  mqtt_messge->ptopic,
					  mqtt_messge->payload_len,
					  mqtt_messge->payload);
			break;
		case MQTT_EVENT_SUBCRIBE_SUCCESS:
			Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);			
			break;

		case MQTT_EVENT_SUBCRIBE_TIMEOUT:
			Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_SUBCRIBE_NACK:
			Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_UNSUBCRIBE_SUCCESS:
			Log_i("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
			Log_i("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_UNSUBCRIBE_NACK:
			Log_i("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_SUCCESS:
			Log_i("publish success, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_TIMEOUT:
			Log_i("publish timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_NACK:
			Log_i("publish nack, packet-id=%u", (unsigned int)packet_id);
			break;
		default:
			Log_i("Should NOT arrive here.");
			break;
	}
}

static int _setup_connect_init_params(MQTTInitParams* initParams)
{
	int ret;
	
	ret = HAL_GetDevInfo((void *)&sg_devInfo);	
	if(QCLOUD_RET_SUCCESS != ret){
		return ret;
	}
	
	initParams->device_name = sg_devInfo.device_name;
	initParams->product_id = sg_devInfo.product_id;

#ifdef AUTH_MODE_CERT
	char certs_dir[PATH_MAX + 1] = "certs";
	char current_path[PATH_MAX + 1];
	char *cwd = getcwd(current_path, sizeof(current_path));
	if (cwd == NULL)
	{
		Log_e("getcwd return NULL");
		return QCLOUD_ERR_FAILURE;
	}
	sprintf(sg_cert_file, "%s/%s/%s", current_path, certs_dir, sg_devInfo.dev_cert_file_name);
	sprintf(sg_key_file, "%s/%s/%s", current_path, certs_dir, sg_devInfo.dev_key_file_name);

	initParams->cert_file = sg_cert_file;
	initParams->key_file = sg_key_file;
#else
	initParams->device_secret = sg_devInfo.device_secret;
#endif

    memset(sg_integeration_test_topic, 0, MAX_SIZE_OF_TOPIC_CONTENT);	
	sprintf(sg_integeration_test_topic, "%s/%s/data", sg_devInfo.product_id, sg_devInfo.device_name);

	initParams->command_timeout = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
	initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;

	initParams->auto_connect_enable = 1;
	initParams->event_handle.h_fp = event_handler;

    return QCLOUD_RET_SUCCESS;
}

/**
 * self-defined format, not JSON
 */
static void _mqtt_message_handler(void *pClient, MQTTMessage *message, void *userData) 
{
	if (message == NULL) {
		return;
	}

	if(MAX_SIZE_OF_TOPIC_CONTENT >= message->payload_len) {
		/* parsing payload */
		char tempBuf[MAX_SIZE_OF_TOPIC_CONTENT + 1] = {0};
		unsigned int tempRow = 0, tempCol = 0;
        char *temp = NULL;		
        
		HAL_Snprintf(tempBuf, message->payload_len + 1, "%s", (char *)message->payload);
		Log_d("Message received : %s", tempBuf);
		temp = strtok(tempBuf, " ,:");
		if(NULL == temp) {
			return;
		}
		temp = strtok(NULL, " ,:");
		if(NULL == temp) {
			return;
		}
		tempRow = atoi(temp);
		temp = strtok(NULL, " ,:");
		if(NULL == temp) {
			return;
		}
		temp = strtok(NULL, " ,:");
		if(NULL == temp) {
			return;
		}
		tempCol = atoi(temp);

		if(((tempRow - 1) < MAX_PUB_THREAD_COUNT) && (tempCol < PUBLISH_COUNT)) {
			sg_countArray[tempRow - 1][tempCol]++;
		} else {
			Log_e(" Unexpected Thread : %d, Message : %d ", tempRow, tempCol);
			sg_rxUnexpectedNumberCounter++;
		}
	} else {
		sg_rxMsgBufferTooBigCounter++;
	}
}

/**
 * yield test thread runner
 */
static void *_mqtt_yield_thread_runner(void *ptr) {
	int rc = QCLOUD_RET_SUCCESS;
	void *pClient = ptr;
    
	while(false == sg_terminate_yield_thread) {

        rc = IOT_MQTT_Yield(pClient, 200);

        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            usleep(THREAD_SLEEP_INTERVAL_USEC);
            continue;
        }
        else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED){
            Log_e("Yield thread exit with error: %d", rc);
            break;
        }
        usleep(THREAD_SLEEP_INTERVAL_USEC);
	}
	return NULL;
}

/**
 * subscribe/unsubscribe test thread runner
 * subscribed and unsubscribe
 */
static void *_mqtt_sub_unsub_thread_runner(void *ptr) {
	int rc = QCLOUD_RET_SUCCESS;
	void *pClient = ptr;
	char testTopic[128];
	HAL_Snprintf(testTopic, 128, "%s/%s/control", sg_devInfo.product_id, sg_devInfo.device_name);

	while(QCLOUD_RET_SUCCESS == rc && false == sg_terminate_subUnsub_thread) {
		do {
			usleep(THREAD_SLEEP_INTERVAL_USEC);
			SubscribeParams sub_params = DEFAULT_SUB_PARAMS;
			sub_params.qos = QOS1;
			sub_params.on_message_handler = _mqtt_message_handler;
			rc = IOT_MQTT_Subscribe(pClient, testTopic, &sub_params);

		} while(QCLOUD_ERR_MQTT_NO_CONN == rc || QCLOUD_ERR_MQTT_REQUEST_TIMEOUT == rc);

		if(rc < 0) {
			Log_e("Subscribe failed. Ret : %d ", rc);
		}

        sleep(1);
        
		do {
			usleep(THREAD_SLEEP_INTERVAL_USEC);
			rc = IOT_MQTT_Unsubscribe(pClient, testTopic);
		} while(QCLOUD_ERR_MQTT_NO_CONN == rc|| QCLOUD_ERR_MQTT_REQUEST_TIMEOUT == rc);

		if(rc < 0) {
			Log_e("Unsubscribe failed. Returned : %d ", rc);
		}
	}
	return NULL;
}

/**
 * do subscribe in the thread
 */
static int _mqtt_subscribe_to_test_topic(void *pClient)
{
	SubscribeParams sub_params = DEFAULT_SUB_PARAMS;
    sub_params.on_message_handler = _mqtt_message_handler;
    sub_params.qos = QOS1;
	return IOT_MQTT_Subscribe(pClient, sg_integeration_test_topic, &sub_params);
}

/**
 * do publish in the thread
 * loop for PUBLISH_COUNT times
 * If publish failed in 1st time, do it again
 */
static void *_mqtt_publish_thread_runner(void *ptr) {
	int itr = 0;
	char topic_content[MAX_SIZE_OF_TOPIC_CONTENT + 1] = {0};

	PublishParams params;
	int rc = QCLOUD_RET_SUCCESS;
	ThreadData *threadData = (ThreadData *) ptr;
	void *pClient = threadData->client;
	int threadId = threadData->threadId;

    int sleep_us = THREAD_SLEEP_INTERVAL_USEC;
	for(itr = 0; itr < PUBLISH_COUNT; itr++) {
        int size = HAL_Snprintf(topic_content, sizeof(topic_content), "Thread : %d, Msg : %d", threadId, itr);
    	if (size < 0 || size > sizeof(topic_content) - 1) {
    		Log_e("payload content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_content));
    		return 0;
    	}		

		params.payload = (void *) topic_content;
		params.payload_len = strlen(topic_content);
		params.qos = QOS1;
		Log_d("Msg being published: %s", topic_content);

		do {
			rc = IOT_MQTT_Publish(pClient, sg_integeration_test_topic, &params);
			usleep(sleep_us);
		} while(QCLOUD_ERR_MQTT_NO_CONN == rc || QCLOUD_ERR_MQTT_REQUEST_TIMEOUT == rc);
		
		// 1st publish failed, re-publish and update sg_rePublishCount
		if(rc < 0) {
			Log_e("Failed attempt 1 Publishing Thread : %d, Msg : %d, cs : %d ", threadId, itr, rc);
			do {
				rc = IOT_MQTT_Publish(pClient, sg_integeration_test_topic, &params);
				usleep(sleep_us);
			} while(QCLOUD_ERR_MQTT_NO_CONN == rc);
			sg_rePublishCount++;
			if(rc < 0) {
				Log_e("Failed attempt 2 Publishing Thread : %d, Msg : %d, cs : %d Second Attempt ", threadId, itr, rc);
			}
		}
	}
	sg_threadStatus[threadId - 1] = 1;
	return 0;
}

/**
 * thread safety test
 */
static int _mqtt_multi_thread_test(void* client)
{
	pthread_t publish_thread[MAX_PUB_THREAD_COUNT];	// for all publish thread
	pthread_t yield_thread;							// yield thread
	pthread_t sub_unsub_thread;						// sub/unsub thread
	int threadId[MAX_PUB_THREAD_COUNT];				// thread id
	int pubThreadReturn[MAX_PUB_THREAD_COUNT];		// to save return value when creating pub threads
	float percentOfRxMsg = 0.0;						// record the success percentage of every pub send and recv
	int finishedThreadCount = 0;					// record the number of finished publish threads
	int rxMsgCount = 0;         					// record the times of successfull subscribe
	ThreadData threadData[MAX_PUB_THREAD_COUNT];	// thread data to wrap client threadId for publish thread
    
	int rc = QCLOUD_RET_SUCCESS;
	int test_result = 0;	
    int i = 0 , j = 0;

    // init the global variables    
	sg_terminate_yield_thread = false;
	sg_rxMsgBufferTooBigCounter = 0;
	sg_rxUnexpectedNumberCounter = 0;
	sg_rePublishCount = 0;
	sg_wrongYieldCount = 0;

    if (client == NULL) {
        Log_e("MQTT client is invalid!");
        return QCLOUD_ERR_FAILURE;
    }    

    /* Firstly create the thread for IOT_MQTT_Yield() so we can read and handle MQTT packet */
    rc = pthread_create(&yield_thread, NULL, _mqtt_yield_thread_runner, client);
    if (rc < 0) {
        Log_e("Create Yield thread failed. err: %d - %s", rc, strerror(rc));
        return rc;
    }

    /* create a thread to test subscribe and unsubscribe another topic */
	rc = pthread_create(&sub_unsub_thread, NULL, _mqtt_sub_unsub_thread_runner, client);
    if (rc < 0) {
        Log_e("Create Sub_unsub thread failed. err: %d - %s", rc, strerror(rc));
        return rc;
    }
    
    /* subscribe the same test topic as publish threads */
	rc = _mqtt_subscribe_to_test_topic(client);
    if (rc < 0) {
        Log_e("Client subscribe failed: %d", rc);
        return rc;
    }    
    
    /* setup the thread info for pub-threads */
    for(j = 0; j < MAX_PUB_THREAD_COUNT; j++) {
		threadId[j] = j + 1;	    // self defined thread ID: 1 - MAX_PUB_THREAD_COUNT
		sg_threadStatus[j] = 0;	    // thread status flag
		for(i = 0; i < PUBLISH_COUNT; i++) {
			sg_countArray[j][i] = 0;
		}
	}

	/* create multi threads to test IOT_MQTT_Publish() */
	for(i = 0; i < MAX_PUB_THREAD_COUNT; i++) {
		threadData[i].client = client;
		threadData[i].threadId = threadId[i];
		pubThreadReturn[i] = pthread_create(&publish_thread[i], NULL, _mqtt_publish_thread_runner,
											&threadData[i]);
											
		if (pubThreadReturn[i] < 0) {
            Log_e("Create publish thread(ID: %d) failed. err: %d - %s", 
                        threadId[i], pubThreadReturn[i], strerror(pubThreadReturn[i]));
        }
	}
    
	/* wait for all pub-threads to finish their jobs */
	do {
		finishedThreadCount = 0;
		for(i = 0; i < MAX_PUB_THREAD_COUNT; i++) { 
			finishedThreadCount += sg_threadStatus[i];
		}
		Log_i(">>>>>>>>Finished thread count : %d", finishedThreadCount);
		sleep(1);
	} while(finishedThreadCount < MAX_PUB_THREAD_COUNT);

	Log_i("Publishing is finished");

	sg_terminate_yield_thread = true;
	sg_terminate_subUnsub_thread = true;

	/* Allow time for yield_thread and sub_sunsub thread to exit */
    
    /* Not using pthread_join because all threads should have terminated gracefully at this point. If they haven't,
         * which should not be possible, something below will fail. */
	sleep(1);
	
	/* Calculating Test Results */
	for(i = 0; i < PUBLISH_COUNT; i++) {
		for(j = 0; j < MAX_PUB_THREAD_COUNT; j++) {
			if(sg_countArray[j][i] > 0) {
				rxMsgCount++;
			}
		}
	}
    percentOfRxMsg = (float) rxMsgCount * 100 / (PUBLISH_COUNT * MAX_PUB_THREAD_COUNT);
    
	printf("\n\nMQTT Multi-thread Test Result : \n");	
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	if(RX_RECEIVE_PERCENTAGE <= percentOfRxMsg &&
		0 == sg_rxMsgBufferTooBigCounter &&
		0 == sg_rxUnexpectedNumberCounter &&
	    0 == sg_wrongYieldCount)
	{	
		// test success
		printf("Success! PercentOfRxMsg: %f %%\n", percentOfRxMsg);
		printf("Published Messages: %d , Received Messages: %d \n", PUBLISH_COUNT * MAX_PUB_THREAD_COUNT, rxMsgCount);
		printf("QoS 1 re publish count %u\n", sg_rePublishCount);
		printf("Yield count without error during callback %u\n", sg_wrongYieldCount);
		test_result = 0;
	} else {
		// test fail
		printf("\nFailure! PercentOfRxMsg: %f %%\n", percentOfRxMsg);
        printf("Published Messages: %d , Received Messages: %d \n", PUBLISH_COUNT * MAX_PUB_THREAD_COUNT, rxMsgCount);
		printf("\"Received message was too big than anything sent\" count: %u\n", sg_rxMsgBufferTooBigCounter);
		printf("\"The number received is out of the range\" count: %u\n", sg_rxUnexpectedNumberCounter);
		printf("Yield count without error during callback %u\n", sg_wrongYieldCount);
		test_result = -1;
	}	
    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");
    
	return test_result;
}


int main(int argc, char **argv) {
    int rc;
    
    //Init log level
    IOT_Log_Set_Level(eLOG_DEBUG);

    //Avoid broken pipe crash
    signal(SIGPIPE, SIG_IGN);

    //Init connection
    MQTTInitParams init_params = DEFAULT_MQTTINIT_PARAMS;
    rc = _setup_connect_init_params(&init_params);
	if (rc != QCLOUD_RET_SUCCESS) {
		return rc;
    }

    //MQTT client connect
	void * client = IOT_MQTT_Construct(&init_params);
    if (client != NULL) {
        Log_i("Cloud Device Construct Success");
    } else {
        Log_e("Cloud Device Construct Failed");
        return QCLOUD_ERR_FAILURE;
    }

    //Start rock & roll
    rc = _mqtt_multi_thread_test(client);
	if(0 != rc) {
        Log_e("MQTT multi-thread test FAILED! RC: %d", rc);
    } else {
        Log_i("MQTT multi-thread test SUCCESS");
    }

    //Finish and destroy
    rc = IOT_MQTT_Destroy(&client);
    return rc;
}

#endif
