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

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud.h"

__QCLOUD_STATIC__ void event_reply_destroy(qcloud_event_reply_t *reply)
{
    qcloud_list_del(&reply->list);
    osal_free(reply);
}

__QCLOUD_STATIC__ void event_reply_list_destroy(qcloud_event_client_t *client)
{
    qcloud_list_t *curr, *next;
    qcloud_event_reply_t *reply;

    if (qcloud_list_empty(&client->reply_list)) {
        return;
    }

    osal_mutex_lock(client->reply_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->reply_list) {
        reply = QCLOUD_LIST_ENTRY(curr, qcloud_event_reply_t, list);
        event_reply_destroy(reply);
    }

    osal_mutex_unlock(client->reply_list_lock);
}

__QCLOUD_STATIC__ void event_reply_handler(void *client, mqtt_incoming_msg_t *message, void *private_data)
{
    QCLOUD_POINTER_SANITY_CHECK_RTN(message);
    QCLOUD_POINTER_SANITY_CHECK_RTN(private_data);

    int32_t return_code;
    char *client_token = NULL, *status = NULL;
    qcloud_list_t *curr, *next;
    qcloud_event_client_t *event_client;
    qcloud_event_reply_t *reply;

    event_client = (qcloud_event_client_t *)private_data;

    QCLOUD_LOG_I("msg recved, topic: %.*s, payload: %.*s", message->topic_len, message->topic, message->payload_len, (char *) message->payload);

    // 解析事件回复中的clientToken
    if (!shadow_json_client_token_parse((char *)message->payload, &client_token)) {
        QCLOUD_LOG_E("client token parse fail!");
        return;
    }

    // 解析事件回复中的处理结果
    if (!event_json_return_code_parse((char *)message->payload, &return_code)) {
        return;
    }

    if (!event_json_status_parse((char *)message->payload, &status)) {
        QCLOUD_LOG_D("no status return");
    }

    QCLOUD_LOG_D("event token:%s code:%d status:%s", client_token, return_code, status);

    if (qcloud_list_empty(&event_client->reply_list)) {
        return;
    }

    osal_mutex_lock(event_client->reply_list_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &event_client->reply_list) {
        reply = QCLOUD_LIST_ENTRY(curr, qcloud_event_reply_t, list);

        if (osal_timer_is_expired(&reply->timer)) {
            QCLOUD_LOG_E("event[%s] timeout", reply->client_token);
            event_reply_destroy(reply);
            continue;
        }

        if (strcmp(client_token, reply->client_token) == 0) { // client_token matches
            if (reply->handler) {
                reply->handler(client, message);
            }
            event_reply_destroy(reply);
        }
    }

    osal_mutex_unlock(event_client->reply_list_lock);
}

__QCLOUD_STATIC__ qcloud_event_reply_t *event_reply_construct(qcloud_event_client_t *client,
                                                                                uint32_t timeout_ms,
                                                                                event_reply_handler_fn_t handler)
{
    qcloud_err_t rc;
    int rc_snprintf;
    qcloud_event_reply_t *event_reply;
    qcloud_shadow_client_t *shadow_client;

    shadow_client = client->shadow_client;

    event_reply = (qcloud_event_reply_t *)osal_malloc(sizeof(qcloud_event_reply_t));
    if (!event_reply) {
        return NULL;
    }

    event_reply->handler = handler;

    osal_timer_init(&event_reply->timer);
    osal_timer_countdown_ms(&event_reply->timer, timeout_ms);

	memset(event_reply->client_token, 0, sizeof(event_reply->client_token));

    ++shadow_client->token_num;

	rc_snprintf = osal_snprintf(event_reply->client_token, QCLOUD_EVENT_TOKEN_MAX, "%s-%u",
                                    shadow_client->device_product_id, shadow_client->token_num);
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, QCLOUD_EVENT_TOKEN_MAX);
    if (rc != QCLOUD_ERR_SUCCESS) {
        return NULL;
    }

    qcloud_list_init(&event_reply->list);

    qcloud_list_add(&event_reply->list, &client->reply_list);

    return event_reply;
}

__QCLOUD_STATIC__ qcloud_err_t event_json_init(qcloud_event_client_t *client,
                                                            char *json_doc,
                                                            size_t json_doc_size,
                                                            int event_count,
                                                            event_reply_handler_fn_t handler,
                                                            uint32_t reply_timeout_ms)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);

	int rc_snprintf = 0;
    qcloud_event_reply_t *event_reply = NULL;

    event_reply = event_reply_construct(client, reply_timeout_ms, handler);
    QCLOUD_FUNC_EXIT_RC_IF(event_reply, NULL, QCLOUD_ERR_INVAL);

	memset(json_doc, 0, json_doc_size);

	if (event_count > 1) {
		rc_snprintf = osal_snprintf(json_doc, json_doc_size, "{\"method\":\"%s\", \"clientToken\":\"%s\", ", \
																		EVENT_FIELD_POSTS, event_reply->client_token);
	} else {
		rc_snprintf = osal_snprintf(json_doc, json_doc_size, "{\"method\":\"%s\", \"clientToken\":\"%s\", ", \
																		EVENT_FIELD_POST, event_reply->client_token);
	}

    QCLOUD_FUNC_EXIT_RC(shadow_json_snprintf_rc2errno(rc_snprintf, json_doc_size));
 }

__QCLOUD_STATIC__ qcloud_err_t event_multi_json_construct(char *json_doc,
                                                                                size_t json_doc_size,
                                                                                size_t remain_size,
                                                                                int event_count,
                                                                                qcloud_event_t *events[])
{
    int i = 0, j = 0;
    qcloud_err_t rc;
    int rc_snprintf;
    qcloud_event_t *event;
    shadow_dev_property_t *dev_property;

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"events\":[");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    for (i = 0; i < event_count; ++i) {
        event = events[i];
        if (!event) {
            QCLOUD_LOG_E("%dth/%d null event", i, event_count);
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
        }

        rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size,
                                        "{\"eventId\":\"%s\", \"type\":\"%s\", \"timestamp\":%u000, \"params\":{",\
                                        event->event_name, event->type, event->timestamp);
        rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
        QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

        if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
        }

        for (j = 0; j < event->event_payload_count; ++j) {
            dev_property = &event->event_payload[i];
            if (!dev_property || !dev_property->key) {
                QCLOUD_LOG_E("%dth/%d null event property data", i, event->event_payload_count);
                QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
            }

            rc = event_json_node_add(json_doc, remain_size, dev_property->key, dev_property->data, dev_property->type);
            QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);
        }

        if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
        }

        rc_snprintf = osal_snprintf(json_doc + strlen(json_doc)-1, remain_size, "}}," );
        rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
        QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);
    }

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc) - 1, remain_size, "]");
    QCLOUD_FUNC_EXIT_RC(shadow_json_snprintf_rc2errno(rc_snprintf, remain_size));
}

__QCLOUD_STATIC__ qcloud_err_t event_single_json_construct(char *json_doc,
                                                                                size_t json_doc_size,
                                                                                size_t remain_size,
                                                                                int event_count,
                                                                                qcloud_event_t *events[])
{
    int i = 0;
    qcloud_err_t rc;
    int rc_snprintf;
    qcloud_event_t *event;
    shadow_dev_property_t *dev_property;

    event = events[0];
    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size,
                                    "\"eventId\":\"%s\", \"type\":\"%s\", \"timestamp\":%u000, \"params\":{",\
                                    event->event_name, event->type, event->timestamp);

    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    for (i = 0; i < event->event_payload_count; ++i) {
        dev_property = &event->event_payload[i];
        if (!dev_property || !dev_property->key) {
            QCLOUD_LOG_E("%dth/%d null event property data", i, event->event_payload_count);
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
        }

        rc = event_json_node_add(json_doc, remain_size, dev_property->key, dev_property->data, dev_property->type);
        QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);
    }

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc) - 1, remain_size, "}" );
    QCLOUD_FUNC_EXIT_RC(shadow_json_snprintf_rc2errno(rc_snprintf, remain_size));
}

__QCLOUD_STATIC__ qcloud_err_t event_json_construct(qcloud_event_client_t *client,
                                                                    char *json_doc,
                                                                    size_t json_doc_size,
                                                                    int event_count,
                                                                    qcloud_event_t *events[],
                                                                    event_reply_handler_fn_t handler,
                                                                    uint32_t reply_timeout_ms)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(events, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    int rc_snprintf = 0;
    size_t remain_size = 0;

    rc = event_json_init(client, json_doc, json_doc_size, event_count, handler, reply_timeout_ms);
    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_LOG_E("event json init failed: %d", rc);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    if (event_count > 1) { //多个事件
        event_multi_json_construct(json_doc, json_doc_size, remain_size, event_count, events);
    } else { //单个事件
        event_single_json_construct(json_doc, json_doc_size, remain_size, event_count, events);
    }

    // finish json
    if ((remain_size = json_doc_size - strlen(json_doc)) < 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "}");
    QCLOUD_FUNC_EXIT_RC(shadow_json_snprintf_rc2errno(rc_snprintf, remain_size));
}

__QCLOUD_STATIC__ qcloud_err_t event_publish(qcloud_event_client_t *client, char *json_doc)
{
	QCLOUD_FUNC_ENTRY;

    mqtt_publish_opt_t publish_opt;
    qcloud_shadow_client_t *shadow_client = NULL;

    shadow_client = client->shadow_client;

    memset(&publish_opt, 0, sizeof(mqtt_publish_opt_t));
    publish_opt.qos         = MQTT_QOS0;
	publish_opt.payload     = json_doc;
	publish_opt.payload_len = strlen(json_doc);

	QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_client_publish(&shadow_client->mqtt_client, client->up_topic, &publish_opt));
}

__QCLOUD_STATIC__ qcloud_err_t event_subscribe(qcloud_event_client_t *client)
{
    mqtt_subscribe_opt_t subscribe_opt;
    qcloud_shadow_client_t *shadow_client = NULL;

    shadow_client = client->shadow_client;

    subscribe_opt.qos               = MQTT_QOS0;
    subscribe_opt.private_data      = (void *)client;
    subscribe_opt.message_handler   = event_reply_handler;

    QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_client_subscribe(&shadow_client->mqtt_client, client->down_topic, &subscribe_opt));
}

__QCLOUD_STATIC__ qcloud_err_t event_unsubscribe(qcloud_event_client_t *client)
{
    qcloud_shadow_client_t *shadow_client = NULL;

    shadow_client = client->shadow_client;

    QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_client_unsubscribe(&shadow_client->mqtt_client, client->down_topic));
}

__QCLOUD_STATIC__ qcloud_err_t event_client_construct(qcloud_event_client_t *client,
                                                                        qcloud_shadow_client_t *shadow_client,
                                                                        qcloud_device_t *device)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(shadow_client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(device, QCLOUD_ERR_INVAL);

    int size;

    size = osal_snprintf(client->down_topic, QCLOUD_MQTT_TOPIC_SIZE_MAX, "$thing/down/event/%s/%s", device->product_id, device->device_name);
    if (size < 0 || size > QCLOUD_MQTT_TOPIC_SIZE_MAX - 1) {
        QCLOUD_LOG_E("topic size overflow, %d", size);
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    size = osal_snprintf(client->up_topic, QCLOUD_MQTT_TOPIC_SIZE_MAX, "$thing/up/event/%s/%s", device->product_id, device->device_name);
    if (size < 0 || size > QCLOUD_MQTT_TOPIC_SIZE_MAX - 1) {
        QCLOUD_LOG_E("topic size overflow, %d", size);
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    if ((client->reply_list_lock = osal_mutex_create()) == NULL) {
        QCLOUD_LOG_E("write buf lock failed.");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    qcloud_list_init(&client->reply_list);

    client->shadow_client = shadow_client;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ void event_incoming_msg_handler(void *client, void *context, mqtt_event_t *event)
{
	uint16_t packet_id;
    qcloud_event_client_t *event_client = NULL;

    event_client = (qcloud_event_client_t *)context;

	switch (event->type) {
		case MQTT_EVENT_SUBCRIBE_SUCCESS:
            packet_id = *(uint16_t *)event->message;
			QCLOUD_LOG_D("subscribe success, packet id=%u", (uint32_t)packet_id);
            event_client->sync_state = QCLOUD_EVENT_SYNC_STATE_SUCCESS;
			break;

		case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            packet_id = *(uint16_t *)event->message;
			QCLOUD_LOG_D("subscribe wait ack timeout, packet id=%u", (uint32_t)packet_id);
			event_client->sync_state = QCLOUD_EVENT_SYNC_STATE_TIMEOUT;
			break;

		case MQTT_EVENT_SUBCRIBE_NACK:
            packet_id = *(uint16_t *)event->message;
			QCLOUD_LOG_D("subscribe nack, packet id=%u", (uint32_t)packet_id);
			event_client->sync_state = QCLOUD_EVENT_SYNC_STATE_NACK;
			break;

		default:
			break;
	}
}

__QCLOUD_API__ qcloud_err_t qcloud_event_client_create(qcloud_event_client_t *client,
                                                                            qcloud_shadow_client_t *shadow_client,
                                                                            qcloud_device_t *device)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(shadow_client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(device, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    void *shadow_context = NULL;
    mqtt_event_handler_fn_t shadow_handler = NULL;

    if (!qcloud_shadow_client_is_connected(shadow_client)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    rc = event_client_construct(client, shadow_client, device);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    rc = event_subscribe(client);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    shadow_context = shadow_client->event_handler.context;
    shadow_handler = shadow_client->event_handler.handler;

    shadow_client->event_handler.context = (void *)client;;
    shadow_client->event_handler.handler = event_incoming_msg_handler;

    client->sync_state = QCLOUD_EVENT_SYNC_STATE_PENDACK;

    while (client->sync_state == QCLOUD_EVENT_SYNC_STATE_PENDACK) {
        qcloud_shadow_client_yield(client->shadow_client, 100);
    }

    shadow_client->event_handler.context = shadow_context;
    shadow_client->event_handler.handler = shadow_handler;

    if (client->sync_state != QCLOUD_SHADOW_SYNC_STATE_SUCCESS) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }
    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_API__ qcloud_err_t qcloud_event_client_destroy(qcloud_event_client_t *client)
{
	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

	event_unsubscribe(client);

	event_reply_list_destroy(client);

    if (client->reply_list_lock) {
        osal_mutex_destroy(client->reply_list_lock);
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
}

__QCLOUD_API__ qcloud_err_t qcloud_event_client_post(qcloud_event_client_t *client,
                                                                        char *json_doc,
                                                                        size_t json_doc_size,
                                                                        int event_count,
                                                                        qcloud_event_t *events[],
                                                                        event_reply_handler_fn_t handler)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(events, QCLOUD_ERR_INVAL);

	qcloud_err_t rc;

	rc = event_json_construct(client, json_doc, json_doc_size, event_count, events, handler, 5000);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("construct event json fail, %d", rc);
		QCLOUD_FUNC_EXIT_RC(rc);
	}

	rc = event_publish(client, json_doc);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("publish event to cloud fail, %d",rc);
	}

	QCLOUD_FUNC_EXIT_RC(rc);
}

__QCLOUD_API__ qcloud_err_t qcloud_event_client_post_raw(qcloud_event_client_t *client,
                                                                                char *json_doc,
                                                                                size_t json_doc_size,
                                                                                char *event_msg,
                                                                                event_reply_handler_fn_t handler)
{
	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(event_msg, QCLOUD_ERR_INVAL);

	qcloud_err_t rc;
    int rc_snprintf;
	size_t remain_size = 0;

	rc = event_json_init(client, json_doc, json_doc_size, 2, handler, 5000);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("event json init failed: %d", rc);
		QCLOUD_FUNC_EXIT_RC(rc);
	}

	if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
	}

	rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"events\":[%s]}", event_msg);
	rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

	QCLOUD_LOG_D("json doc: %s", json_doc);

	rc = event_publish(client, json_doc);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("publish event raw fail, %d",rc);
	}

	QCLOUD_FUNC_EXIT_RC(rc);
}

#ifdef __cplusplus
}
#endif

