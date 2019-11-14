/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "mqtt_client.h"

/**
  * Determines the length of the MQTT subscribe packet that would be produced using the supplied parameters
  * @param count the number of topic filter strings in topicFilters
  * @param topicFilters the array of topic filter strings to be used in the publish
  * @return the length of buffer needed to contain the serialized version of the packet
  */
static uint32_t _get_subscribe_packet_rem_len(uint32_t count, char **topicFilters) {
    size_t i;
    size_t len = 2; /* packetid */

    for (i = 0; i < count; ++i) {
        len += 2 + strlen(*topicFilters + i) + 1; /* length + topic + req_qos */
    }

    return (uint32_t) len;
}

/**
  * Serializes the supplied subscribe data into the supplied buffer, ready for sending
  * @param buf the buffer into which the packet will be serialized
  * @param buf_len the length in bytes of the supplied bufferr
  * @param dup integer - the MQTT dup flag
  * @param packet_id integer - the MQTT packet identifier
  * @param count - number of members in the topicFilters and reqQos arrays
  * @param topicFilters - array of topic filter names
  * @param requestedQoSs - array of requested QoS
  * @return the length of the serialized data.  <= 0 indicates error
  */
static int _serialize_subscribe_packet(unsigned char *buf, size_t buf_len, uint8_t dup, uint16_t packet_id, uint32_t count,
                                char **topicFilters, QoS *requestedQoSs, uint32_t *serialized_len) {
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(serialized_len, QCLOUD_ERR_INVAL);

    unsigned char *ptr = buf;
    unsigned char header = 0;
    uint32_t rem_len = 0;
    uint32_t i = 0;
    int rc;

    // remaining length of SUBSCRIBE packet = packet type(2 byte) + count * (remaining length(2 byte) + topicLen + qos(1 byte))
    rem_len = _get_subscribe_packet_rem_len(count, topicFilters);
    if (get_mqtt_packet_len(rem_len) > buf_len) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }
    // init header
    rc = mqtt_init_packet_header(&header, SUBSCRIBE, QOS1, dup, 0);
    if (QCLOUD_RET_SUCCESS != rc) {
        IOT_FUNC_EXIT_RC(rc);
    }
    // 1st byte in fixed header
    mqtt_write_char(&ptr, header);
    // remaining length
    ptr += mqtt_write_packet_rem_len(ptr, rem_len);
    // variable header
    mqtt_write_uint_16(&ptr, packet_id);
    // payload
    for (i = 0; i < count; ++i) {
        mqtt_write_utf8_string(&ptr, *topicFilters + i);
        mqtt_write_char(&ptr, (unsigned char) requestedQoSs[i]);
    }

    *serialized_len = (uint32_t) (ptr - buf);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

int qcloud_iot_mqtt_subscribe(Qcloud_IoT_Client *pClient, char *topicFilter, SubscribeParams *pParams) {

    IOT_FUNC_ENTRY;
    int rc;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pParams, QCLOUD_ERR_INVAL);
    // POINTER_SANITY_CHECK(pParams->on_message_handler, QCLOUD_ERR_INVAL);
    STRING_PTR_SANITY_CHECK(topicFilter, QCLOUD_ERR_INVAL);

    Timer timer;
    uint32_t len = 0;
    uint16_t packet_id = 0;

    ListNode *node = NULL;
    
    size_t topicLen = strlen(topicFilter);
    if (topicLen > MAX_SIZE_OF_CLOUD_TOPIC) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MAX_TOPIC_LENGTH);
    }

    if (pParams->qos == QOS2) {
        Log_e("QoS2 is not supported currently");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_QOS_NOT_SUPPORT);
    }
    
    if (!get_client_conn_state(pClient)) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN)
    }

    /* topic filter should be valid in the whole sub life */
    char *topic_filter_stored = HAL_Malloc(topicLen + 1);
    if (topic_filter_stored == NULL) {
        Log_e("malloc failed");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }
    
    strcpy(topic_filter_stored, topicFilter);
    topic_filter_stored[topicLen] = 0;
    
    InitTimer(&timer);
    countdown_ms(&timer, pClient->command_timeout_ms);

    HAL_MutexLock(pClient->lock_write_buf);
    packet_id = get_next_packet_id(pClient);
    Log_d("topicName=%s|packet_id=%d", topic_filter_stored, packet_id);

    rc = _serialize_subscribe_packet(pClient->write_buf, pClient->write_buf_size, 0, packet_id, 1, &topic_filter_stored,
                                     &pParams->qos, &len);
    if (QCLOUD_RET_SUCCESS != rc) {
    	HAL_MutexUnlock(pClient->lock_write_buf);
    	HAL_Free(topic_filter_stored);
        IOT_FUNC_EXIT_RC(rc);
    }

    /* add node into sub ack wait list */
    SubTopicHandle sub_handle;
    sub_handle.topic_filter = topic_filter_stored;
    sub_handle.message_handler = pParams->on_message_handler;
    sub_handle.sub_event_handler = pParams->on_sub_event_handler;
    sub_handle.qos = pParams->qos;
    sub_handle.handler_user_data = pParams->user_data;

    rc = push_sub_info_to(pClient, len, (unsigned int)packet_id, SUBSCRIBE, &sub_handle, &node);
    if (QCLOUD_RET_SUCCESS != rc) {
        Log_e("push publish into to pubInfolist failed!");
        HAL_MutexUnlock(pClient->lock_write_buf);
        HAL_Free(topic_filter_stored);
        IOT_FUNC_EXIT_RC(rc);
    }
    
    // send SUBSCRIBE packet
    rc = send_mqtt_packet(pClient, len, &timer);
    if (QCLOUD_RET_SUCCESS != rc) {
        HAL_MutexLock(pClient->lock_list_sub);
        list_remove(pClient->list_sub_wait_ack, node);
        HAL_MutexUnlock(pClient->lock_list_sub);

    	HAL_MutexUnlock(pClient->lock_write_buf);
    	HAL_Free(topic_filter_stored);
        IOT_FUNC_EXIT_RC(rc);
    }

    HAL_MutexUnlock(pClient->lock_write_buf);

    IOT_FUNC_EXIT_RC(packet_id);
}

int qcloud_iot_mqtt_resubscribe(Qcloud_IoT_Client *pClient) {
    IOT_FUNC_ENTRY;
    int rc;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    uint32_t itr = 0;
    char *topic = NULL;
    SubscribeParams temp_param;

    if (NULL == pClient) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    if (!get_client_conn_state(pClient)) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    for (itr = 0; itr < MAX_MESSAGE_HANDLERS; itr++) {
        topic = (char *) pClient->sub_handles[itr].topic_filter;
        if (topic == NULL) {
            continue;
        }
        temp_param.on_message_handler = pClient->sub_handles[itr].message_handler;
        temp_param.on_sub_event_handler = pClient->sub_handles[itr].sub_event_handler;
        temp_param.qos = pClient->sub_handles[itr].qos;
        temp_param.user_data = pClient->sub_handles[itr].handler_user_data;

        rc = qcloud_iot_mqtt_subscribe(pClient, topic, &temp_param);
        if (rc < 0) {
        	Log_e("resubscribe failed %d, topic: %s", rc, topic);
            IOT_FUNC_EXIT_RC(rc);
        }
    }

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

#ifdef __cplusplus
}
#endif
