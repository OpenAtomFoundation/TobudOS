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
  * Determines the length of the MQTT unsubscribe packet that would be produced using the supplied parameters
  * @param count the number of topic filter strings in topicFilters
  * @param topicFilters the array of topic filter strings to be used in the publish
  * @return the length of buffer needed to contain the serialized version of the packet
  */
static uint32_t _get_unsubscribe_packet_rem_len(uint32_t count, char **topicFilters) {
    size_t i;
    size_t len = 2; /* packetid */

    for (i = 0; i < count; ++i) {
        len += 2 + strlen(*topicFilters + i); /* length + topic*/
    }

    return (uint32_t) len;
}

/**
  * Serializes the supplied unsubscribe data into the supplied buffer, ready for sending
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buf_len the length in bytes of the data in the supplied buffer
  * @param dup integer - the MQTT dup flag
  * @param packet_id integer - the MQTT packet identifier
  * @param count - number of members in the topicFilters array
  * @param topicFilters - array of topic filter names
  * @param serialized_len - the length of the serialized data
  * @return int indicating function execution status
  */
static int _serialize_unsubscribe_packet(unsigned char *buf, size_t buf_len,
                                         uint8_t dup, uint16_t packet_id,
                                         uint32_t count, char **topicFilters,
                                         uint32_t *serialized_len) {
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(serialized_len, QCLOUD_ERR_INVAL);

    unsigned char *ptr = buf;
    unsigned char header = 0;
    uint32_t rem_len = 0;
    uint32_t i = 0;
    int rc;

    rem_len = _get_unsubscribe_packet_rem_len(count, topicFilters);
    if (get_mqtt_packet_len(rem_len) > buf_len) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    rc = mqtt_init_packet_header(&header, UNSUBSCRIBE, QOS1, dup, 0);
    if (QCLOUD_RET_SUCCESS != rc) {
        IOT_FUNC_EXIT_RC(rc);
    }
    mqtt_write_char(&ptr, header); /* write header */

    ptr += mqtt_write_packet_rem_len(ptr, rem_len); /* write remaining length */

    mqtt_write_uint_16(&ptr, packet_id);

    for (i = 0; i < count; ++i) {
        mqtt_write_utf8_string(&ptr, *topicFilters + i);
    }

    *serialized_len = (uint32_t) (ptr - buf);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

int qcloud_iot_mqtt_unsubscribe(Qcloud_IoT_Client *pClient, char *topicFilter) {
    IOT_FUNC_ENTRY;
    int rc;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);
    STRING_PTR_SANITY_CHECK(topicFilter, QCLOUD_ERR_INVAL);

    int i = 0;
    Timer timer;
    uint32_t len = 0;
    uint16_t packet_id = 0;
    bool suber_exists = false;

    ListNode *node = NULL;

    size_t topicLen = strlen(topicFilter);
    if (topicLen > MAX_SIZE_OF_CLOUD_TOPIC) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MAX_TOPIC_LENGTH);
    }
    
    /* Remove from message handler array */
    HAL_MutexLock(pClient->lock_generic);
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i) {        
        if ((pClient->sub_handles[i].topic_filter != NULL && !strcmp(pClient->sub_handles[i].topic_filter, topicFilter))
            || strstr(topicFilter,"/#") != NULL || strstr(topicFilter,"/+") != NULL) {            
            /* notify this event to topic subscriber */
            if (NULL != pClient->sub_handles[i].sub_event_handler)
                pClient->sub_handles[i].sub_event_handler(
                    pClient, MQTT_EVENT_UNSUBSCRIBE, pClient->sub_handles[i].handler_user_data);

            /* Free the topic filter malloced in qcloud_iot_mqtt_subscribe */
            HAL_Free((void *)pClient->sub_handles[i].topic_filter);
            pClient->sub_handles[i].topic_filter = NULL;

            /* We don't want to break here, if the same topic is registered
             * with 2 callbacks. Unlikely scenario */
            suber_exists = true;
        }
    }
    HAL_MutexUnlock(pClient->lock_generic);

    if (suber_exists == false) {
        Log_e("subscription does not exists: %s", topicFilter);
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_UNSUB_FAIL);
    }

    if (!get_client_conn_state(pClient)) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
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
    rc = _serialize_unsubscribe_packet(pClient->write_buf, pClient->write_buf_size, 0, packet_id, 1, &topic_filter_stored,
                                       &len);
    if (QCLOUD_RET_SUCCESS != rc) {
    	HAL_MutexUnlock(pClient->lock_write_buf);
    	HAL_Free(topic_filter_stored);
        IOT_FUNC_EXIT_RC(rc);
    }

    SubTopicHandle sub_handle;
    sub_handle.topic_filter = topic_filter_stored;
    sub_handle.sub_event_handler = NULL;
    sub_handle.message_handler = NULL;
    sub_handle.handler_user_data = NULL;

    rc = push_sub_info_to(pClient, len, (unsigned int)packet_id, UNSUBSCRIBE, &sub_handle, &node);
    if (QCLOUD_RET_SUCCESS != rc) {
        Log_e("push publish into to pubInfolist failed: %d", rc);
        HAL_MutexUnlock(pClient->lock_write_buf);
        HAL_Free(topic_filter_stored);
        IOT_FUNC_EXIT_RC(rc);
    }

    /* send the unsubscribe packet */
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

#ifdef __cplusplus
}
#endif
