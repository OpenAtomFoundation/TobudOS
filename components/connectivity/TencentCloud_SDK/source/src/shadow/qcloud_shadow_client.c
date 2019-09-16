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

__QCLOUD_STATIC__ qcloud_err_t shadow_client_construct(qcloud_shadow_client_t *client,
                                                                        mqtt_event_handler_fn_t handler,
                                                                        shadow_type_t shadow_type,
                                                                        qcloud_device_t *device)
{
    int topic_len = 0;

    client->shadow_type             = shadow_type;
    client->event_handler.handler   = handler;

    client->version                 = 0;
    client->token_num               = 0;
    client->sync_state              = QCLOUD_SHADOW_SYNC_STATE_NONE;

    client->request_list_counter    = 0;
    qcloud_list_init(&client->request_list);

    qcloud_list_init(&client->property_list);

    memset(client->request_topic_subscribe, 0, sizeof(client->request_topic_subscribe));

    if (shadow_type == SHADOW_TYPE_TEMPLATE) {
        topic_len = osal_snprintf(client->request_topic_subscribe, QCLOUD_MQTT_TOPIC_SIZE_MAX,
                                            "$template/operation/result/%s/%s",
                                            device->product_id, device->device_name);
    } else {
        topic_len = osal_snprintf(client->request_topic_subscribe, QCLOUD_MQTT_TOPIC_SIZE_MAX,
                                            "$shadow/operation/result/%s/%s",
                                            device->product_id, device->device_name);
    }

    if (topic_len < 0 || topic_len >= QCLOUD_MQTT_TOPIC_SIZE_MAX) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    memset(client->request_topic_publish, 0, sizeof(client->request_topic_publish));

    if (shadow_type == SHADOW_TYPE_TEMPLATE) {
        topic_len = osal_snprintf(client->request_topic_publish, QCLOUD_MQTT_TOPIC_SIZE_MAX,
                                            "$template/operation/%s/%s",
                                            device->product_id, device->device_name);
    } else {
        topic_len = osal_snprintf(client->request_topic_publish, QCLOUD_MQTT_TOPIC_SIZE_MAX,
                                            "$shadow/operation/%s/%s",
                                            device->product_id, device->device_name);
    }

    if (topic_len < 0 || topic_len >= QCLOUD_MQTT_TOPIC_SIZE_MAX) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    strncpy(client->device_product_id, device->product_id, sizeof(client->device_product_id));
    client->device_product_id[QCLOUD_DEVICE_PRODUCT_ID_MAX] = '\0';

    client->global_lock = osal_mutex_create();
    if (!client->global_lock) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ void shadow_incoming_msg_handler(void *client, void *context, mqtt_event_t *event)
{
	uint16_t packet_id;
    mqtt_incoming_msg_t *mqtt_msg;
    qcloud_shadow_client_t *shadow_client = NULL;

    shadow_client = (qcloud_shadow_client_t *)context;

	switch (event->type) {
		case MQTT_EVENT_SUBCRIBE_SUCCESS:
            packet_id = *(uint16_t *)event->message;
			QCLOUD_LOG_D("subscribe success, packet id=%u", (uint32_t)packet_id);
            shadow_client->sync_state = QCLOUD_SHADOW_SYNC_STATE_SUCCESS;
			break;

		case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            packet_id = *(uint16_t *)event->message;
			QCLOUD_LOG_D("subscribe wait ack timeout, packet id=%u", (uint32_t)packet_id);
			shadow_client->sync_state = QCLOUD_SHADOW_SYNC_STATE_TIMEOUT;
			break;

		case MQTT_EVENT_SUBCRIBE_NACK:
            packet_id = *(uint16_t *)event->message;
			QCLOUD_LOG_D("subscribe nack, packet id=%u", (uint32_t)packet_id);
			shadow_client->sync_state = QCLOUD_SHADOW_SYNC_STATE_NACK;
			break;

		case MQTT_EVENT_PUBLISH_RECVEIVED:
            mqtt_msg = (mqtt_incoming_msg_t *)event->message;
			QCLOUD_LOG_D("topic arrived without handler: topic=%.*s, payload=%.*s",
					 mqtt_msg->topic_len,
					 mqtt_msg->topic,
					 mqtt_msg->payload_len,
					 mqtt_msg->payload);
			break;

		default:
			break;
	}

	if (shadow_client->event_handler.handler) {
		shadow_client->event_handler.handler(shadow_client, shadow_client->event_handler.context, event);
	}
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_create(qcloud_shadow_client_t *client,
                                                                            qcloud_device_t *device,
                                                                            mqtt_event_handler_fn_t handler,
                                                                            shadow_type_t shadow_type)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(device, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;

    memset(client, 0, sizeof(qcloud_shadow_client_t));

    rc = qcloud_mqtt_client_create(&client->mqtt_client, device, shadow_incoming_msg_handler, (void *)client, QCLOUD_AUTO_CONN_STATE_ENABLED);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

    rc = qcloud_mqtt_connect_opt_create(&client->mqtt_connect_opt, device, MQTT_VERSION_3_1_1, 240, MQTT_CLEAN_SESSION_STATE_ENABLED);
    if (rc != QCLOUD_ERR_SUCCESS) {
        goto errout;
    }

    rc = shadow_client_construct(client, handler, shadow_type, device);
    if (rc != QCLOUD_ERR_SUCCESS) {
        goto errout;
    }

    rc = qcloud_mqtt_client_connect(&client->mqtt_client, &client->mqtt_connect_opt);
    if (rc != QCLOUD_ERR_SUCCESS) {
        goto errout;
    }

    rc = shadow_glue_operation_request_subscribe(client);
    if (rc != QCLOUD_ERR_SUCCESS) {
        goto errout;
    }

    client->sync_state = QCLOUD_SHADOW_SYNC_STATE_PENDACK;

    while (client->sync_state == QCLOUD_SHADOW_SYNC_STATE_PENDACK) {
        qcloud_shadow_client_yield(client, 100);
    }

    if (client->sync_state != QCLOUD_SHADOW_SYNC_STATE_SUCCESS) {
        goto errout;
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);

errout:
    qcloud_mqtt_client_destroy(&client->mqtt_client);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_yield(qcloud_shadow_client_t *client, uint32_t timeout)
{
    QCLOUD_FUNC_ENTRY;

	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
	QCLOUD_NUMBERIC_SANITY_CHECK(timeout, QCLOUD_ERR_INVAL);

    shadow_glue_request_list_scan(client);

    QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_client_yield(&client->mqtt_client, &client->mqtt_connect_opt, timeout));
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_publish(qcloud_shadow_client_t *client, char *topic, mqtt_publish_opt_t *publish_opt)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(topic, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(publish_opt, QCLOUD_ERR_INVAL);

	QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_publish(&client->mqtt_client, topic, publish_opt));
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_subscribe(qcloud_shadow_client_t *client, const char *topic_filter, mqtt_subscribe_opt_t *subscribe_opt)
{
	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(topic_filter, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(subscribe_opt, QCLOUD_ERR_INVAL);

	QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_subscribe(&client->mqtt_client, topic_filter, subscribe_opt));
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_unsubscribe(qcloud_shadow_client_t *client, const char *topic_filter)

{
	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(topic_filter, QCLOUD_ERR_INVAL);

	QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_unsubscribe(&client->mqtt_client, topic_filter));
}

__QCLOUD_API__ int qcloud_shadow_client_is_connected(qcloud_shadow_client_t *client)
{
	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

	return qcloud_mqtt_client_is_connected(&client->mqtt_client);
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_destroy(qcloud_shadow_client_t *client)
{
	QCLOUD_FUNC_ENTRY;

	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

	shadow_glue_operation_request_unsubscribe(client);

	shadow_glue_property_list_destroy(client);
    shadow_glue_request_list_destroy(client);

	qcloud_mqtt_client_destroy(&client->mqtt_client);

    if (client->global_lock) {
        osal_mutex_destroy(client->global_lock);
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS)
}

__QCLOUD_STATIC__ int shadow_device_property_is_exist(qcloud_shadow_client_t *client, shadow_dev_property_t *that_dev_property)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_list_t *curr, *next;
    qcloud_shadow_prop_info_t *property_info = NULL;
    shadow_dev_property_t *this_dev_property = NULL;

    if (qcloud_list_empty(&client->property_list)) {
        return QCLOUD_FALSE;
    }

    osal_mutex_lock(client->global_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->property_list) {
        property_info       = QCLOUD_LIST_ENTRY(curr, qcloud_shadow_prop_info_t, list);
        this_dev_property   = property_info->dev_property;

        if (strcmp(this_dev_property->key, that_dev_property->key) != 0 ||
            this_dev_property->type != that_dev_property->type) {
            continue;
        }

        osal_mutex_unlock(client->global_lock);
        return QCLOUD_TRUE;
    }

    osal_mutex_unlock(client->global_lock);

    return QCLOUD_FALSE;
}

__QCLOUD_STATIC__ qcloud_err_t shadow_device_property_do_register(qcloud_shadow_client_t *client,
                                                                                            shadow_dev_property_t *dev_property,
                                                                                            shadow_property_delta_handler_fn_t handler)
{
    qcloud_shadow_prop_info_t *property_info = NULL;

    property_info = (qcloud_shadow_prop_info_t *)osal_malloc(sizeof(qcloud_shadow_prop_info_t));
    if (!property_info) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    qcloud_list_init(&property_info->list);
    property_info->dev_property = dev_property;
    property_info->handler      = handler;
    qcloud_list_add(&property_info->list, &client->property_list);
    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_device_property_register(qcloud_shadow_client_t *client,
                                                                                                shadow_dev_property_t *dev_property,
                                                                                                shadow_property_delta_handler_fn_t handler)
{
    QCLOUD_FUNC_ENTRY;

	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(dev_property, QCLOUD_ERR_INVAL);

    if (!qcloud_mqtt_client_is_connected(&client->mqtt_client)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

	if (shadow_device_property_is_exist(client, dev_property)) {
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SHADOW_PROPERTY_EXIST);
    }

    QCLOUD_FUNC_EXIT_RC(shadow_device_property_do_register(client, dev_property, handler));
}

__QCLOUD_STATIC__ qcloud_err_t shadow_device_property_do_unregister(qcloud_shadow_client_t *client, shadow_dev_property_t *that_dev_property)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_list_t *curr, *next;
    qcloud_shadow_prop_info_t *property_info = NULL;
    shadow_dev_property_t *this_dev_property = NULL;

    if (qcloud_list_empty(&client->property_list)) {
        return QCLOUD_ERR_FAILURE;
    }

    osal_mutex_lock(client->global_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->property_list) {
        property_info = QCLOUD_LIST_ENTRY(curr, qcloud_shadow_prop_info_t, list);
        this_dev_property = property_info->dev_property;

        if (strcmp(this_dev_property->key, that_dev_property->key) != 0 ||
            this_dev_property->type != that_dev_property->type) {
            continue;
        }

        qcloud_list_del(&property_info->list);
        osal_free(property_info);

        osal_mutex_unlock(client->global_lock);
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

    osal_mutex_unlock(client->global_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_device_property_unregister(qcloud_shadow_client_t *client, shadow_dev_property_t *dev_property)
{
	QCLOUD_FUNC_ENTRY;

	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(dev_property, QCLOUD_ERR_INVAL);

	if (!qcloud_mqtt_client_is_connected(&client->mqtt_client)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

	if (!shadow_device_property_is_exist(client, dev_property)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SHADOW_NOT_PROPERTY_EXIST);
    }

	QCLOUD_FUNC_EXIT_RC(shadow_device_property_do_unregister(client, dev_property));
}

__QCLOUD_STATIC__ void shadow_req_state_update_handler(void *client, qcloud_shadow_req_method_t method, qcloud_shadow_req_state_t req_state, const char *json_doc, void *context)
{
	QCLOUD_LOG_D("request state=%d", req_state);

    if (json_doc) {
        QCLOUD_LOG_D("json doc=%s", json_doc);
    } else {
        QCLOUD_LOG_D("json doc NULL");
    }

    *((qcloud_shadow_req_state_t *)context) = req_state;
}

__QCLOUD_STATIC__ qcloud_err_t shadow_request_state2errno(qcloud_shadow_req_state_t state)
{
    switch (state) {
        case QCLOUD_SHADOW_REQUEST_STATE_ACCEPTED:
            return QCLOUD_ERR_SUCCESS;

        case QCLOUD_SHADOW_REQUEST_STATE_TIMEOUT:
            return QCLOUD_ERR_SHADOW_UPDATE_TIMEOUT;

        case QCLOUD_SHADOW_REQUEST_STATE_REJECTED:
            return QCLOUD_ERR_SHADOW_UPDATE_REJECTED;
    }

    return QCLOUD_ERR_INVAL;
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_update_async(qcloud_shadow_client_t *client,
                                                                                    char *json_doc,
                                                                                    size_t json_doc_size,
                                                                                    shadow_requset_handler_fn_t handler,
                                                                                    void *context,
                                                                                    uint32_t timeout)
{
    QCLOUD_FUNC_ENTRY;

	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);
	QCLOUD_NUMBERIC_SANITY_CHECK(timeout, QCLOUD_ERR_INVAL);

    qcloud_shadow_req_opt_t request_opt;

	if (!qcloud_mqtt_client_is_connected(&client->mqtt_client)) {
		QCLOUD_LOG_E("mqtt disconnected");
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
	}

	// 如果没有之前没有订阅$shadow/operation/result成功，再一次订阅
	if (client->sync_state != QCLOUD_SHADOW_SYNC_STATE_SUCCESS) {
		shadow_glue_operation_request_subscribe(client);
	}

	QCLOUD_LOG_D("update request docment: %s", json_doc);

    request_opt.method  = QCLOUD_SHADOW_REQUEST_METHOD_UPDATE;
    request_opt.handler = handler;
    request_opt.context = context;
    request_opt.timeout = timeout; // in seconds

	QCLOUD_FUNC_EXIT_RC(shadow_glue_request_post(client, &request_opt, json_doc, json_doc_size));
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_update_sync(qcloud_shadow_client_t *client, char *json_doc, size_t json_doc_size, uint32_t timeout)
{
    QCLOUD_FUNC_ENTRY;

	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);
	QCLOUD_NUMBERIC_SANITY_CHECK(timeout, QCLOUD_ERR_INVAL);

	qcloud_err_t rc = QCLOUD_ERR_SUCCESS;
    qcloud_shadow_req_state_t req_state = QCLOUD_SHADOW_REQUEST_STATE_NONE;

	if (!qcloud_mqtt_client_is_connected(&client->mqtt_client)) {
		QCLOUD_LOG_E("shadow disconnected");
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
	}

	rc = qcloud_shadow_client_update_async(client, json_doc, json_doc_size, shadow_req_state_update_handler, &req_state, timeout);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

	while (req_state == QCLOUD_SHADOW_REQUEST_STATE_NONE) {
        qcloud_shadow_client_yield(client, 200);
    }

    return shadow_request_state2errno(req_state);
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_get_async(qcloud_shadow_client_t *client,
                                                                                shadow_requset_handler_fn_t handler,
                                                                                void *context,
                                                                                uint32_t timeout)
{
    QCLOUD_FUNC_ENTRY;

	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(handler, QCLOUD_ERR_INVAL);
	QCLOUD_NUMBERIC_SANITY_CHECK(timeout, QCLOUD_ERR_INVAL);

	qcloud_err_t rc;
    qcloud_shadow_req_opt_t request_opt;
    char request_json_buffer[QCLOUD_SHADOW_JSON_WITH_CLIENT_TOKEN_MAX];

	if (!qcloud_mqtt_client_is_connected(&client->mqtt_client)) {
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
	}

	// 如果没有之前没有订阅$shadow/operation/result成功，再一次订阅
	if (client->sync_state != QCLOUD_SHADOW_SYNC_STATE_SUCCESS) {
        shadow_glue_operation_request_subscribe(client);
	}

    ++client->token_num;
	rc = shadow_json_empty_doc_build(request_json_buffer, client->token_num, client->device_product_id);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);
	QCLOUD_LOG_D("get request document: %s", request_json_buffer);

    request_opt.method  = QCLOUD_SHADOW_REQUEST_METHOD_GET;
    request_opt.handler = handler;
    request_opt.context = context;
    request_opt.timeout = timeout; // in seconds

	QCLOUD_FUNC_EXIT_RC(shadow_glue_request_post(client, &request_opt, request_json_buffer, sizeof(request_json_buffer)));
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_get_sync(qcloud_shadow_client_t *client, uint32_t timeout)
{
    QCLOUD_FUNC_ENTRY;

	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
	QCLOUD_NUMBERIC_SANITY_CHECK(timeout, QCLOUD_ERR_INVAL);

	qcloud_err_t rc = QCLOUD_ERR_SUCCESS;
    qcloud_shadow_req_state_t req_state = QCLOUD_SHADOW_REQUEST_STATE_NONE;

	if (!qcloud_mqtt_client_is_connected(&client->mqtt_client)) {
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
	}

	rc = qcloud_shadow_client_get_async(client, shadow_req_state_update_handler, &req_state, timeout);
	QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

	while (req_state == QCLOUD_SHADOW_REQUEST_STATE_NONE) {
        qcloud_shadow_client_yield(client, 200);
    }

    QCLOUD_FUNC_EXIT_RC(shadow_request_state2errno(req_state));
}

/**
 * @brief 初始化一个JSON文档
 *
 * 本函数主要是为JSON文档添加state字段, 即 "{\"state\":{", 所以在生成JSON文档时, 请先调用该方法
 *
 * @param jsonBuffer   为存储JSON文档准备的字符串缓冲区
 * @param sizeOfBuffer  缓冲区大小
 * @return              返回QCLOUD_ERR_SUCCESS, 表示初始化成功
 */
__QCLOUD_STATIC__ qcloud_err_t shadow_jsondoc_init(qcloud_shadow_client_t *client,
                                                                char *json_doc,
                                                                size_t json_doc_size,
                                                                int is_overwrite)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);

    int rc = 0;

    if (is_overwrite) {
    	rc = osal_snprintf(json_doc, json_doc_size, "{\"version\":%d, \"overwriteUpdate\":true, \"state\":{", client->version);
    } else {
    	rc = osal_snprintf(json_doc, json_doc_size, "{\"version\":%d, \"state\":{", client->version);
    }

    QCLOUD_FUNC_EXIT_RC(shadow_json_snprintf_rc2errno(rc, json_doc_size));
}

/**
 * @brief 在JSON文档中添加结尾部分的内容, 包括clientToken字段、version字段
 *
 * @param jsonBuffer    为存储JSON文档准备的字符串缓冲区
 * @param sizeOfBuffer   缓冲区大小
 * @return               返回QCLOUD_ERR_SUCCESS, 表示成功
 */
__QCLOUD_STATIC__ qcloud_err_t shadow_jsondoc_finalize(qcloud_shadow_client_t *client, char *json_doc, size_t json_doc_size)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);

    int rc_snprintf;
	qcloud_err_t rc;
	size_t remain_size = 0;

	if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
	}

	rc_snprintf = osal_snprintf(json_doc + strlen(json_doc) - 1, remain_size, "}, \"%s\":\"", SHADOW_FIELD_CLIENT_TOKEN);
	rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

	if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
	}

    ++client->token_num;
	rc = shadow_json_client_token_generate(json_doc + strlen(json_doc), remain_size, client->token_num, client->device_product_id);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

	if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
		return QCLOUD_ERR_JSON_BUFFER_TOO_SHORT;
	}

	rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"}");
    QCLOUD_FUNC_EXIT_RC(shadow_json_snprintf_rc2errno(rc_snprintf, remain_size));
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_report_construct(qcloud_shadow_client_t *client,
                                                                                            char *json_doc,
                                                                                            size_t json_doc_size,
                                                                                            int count, ...)
{
	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    va_list args;
    size_t remain_size = 0;
    int i = 0, rc_snprintf = 0;
    shadow_dev_property_t *dev_property;

    rc = shadow_jsondoc_init(client, json_doc, json_doc_size, QCLOUD_FALSE);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("json init failed: %d", rc);
		QCLOUD_FUNC_EXIT_RC(rc);
	}

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"reported\":{");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    va_start(args, count);

    for (i = 0; i < count; ++i) {
        dev_property = va_arg(args, shadow_dev_property_t *);
        if (!dev_property || !dev_property->key) {
            va_end(args);
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
        }

        rc = shadow_json_node_add(json_doc, remain_size, dev_property->key, dev_property->data, dev_property->type);
        if (rc != QCLOUD_ERR_SUCCESS) {
            va_end(args);
            QCLOUD_FUNC_EXIT_RC(rc);
        }
    }

    va_end(args);

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SHORT;
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc) - 1, remain_size, "},");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("json add report failed: %d", rc);
		QCLOUD_FUNC_EXIT_RC(rc);
	}

	rc = shadow_jsondoc_finalize(client, json_doc, json_doc_size);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("json finalize failed: %d", rc);
	}

	QCLOUD_FUNC_EXIT_RC(rc);
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_report_construct_array(qcloud_shadow_client_t *client,
                                                                                                    char *json_doc,
                                                                                                    size_t json_doc_size,
                                                                                                    int count,
                                                                                                    shadow_dev_property_t *dev_propertys[])
{
	QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);
	QCLOUD_POINTER_SANITY_CHECK(dev_propertys, QCLOUD_ERR_INVAL);

	qcloud_err_t rc;
    int8_t i = 0, rc_snprintf = 0;
    size_t remain_size = 0;
    shadow_dev_property_t *dev_property;

    rc = shadow_jsondoc_init(client, json_doc, json_doc_size, QCLOUD_FALSE);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("json init failed: %d", rc);
		QCLOUD_FUNC_EXIT_RC(rc);
	}

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"reported\":{");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    for (i = 0; i < count; ++i) {
		dev_property = dev_propertys[i];
        if (!dev_property || !dev_property->key) {
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
        }

        rc = shadow_json_node_add(json_doc, remain_size, dev_property->key, dev_property->data, dev_property->type);
        QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);
    }

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SHORT;
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc) - 1, remain_size, "},");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("json add report failed: %d", rc);
		return rc;
	}

	rc = shadow_jsondoc_finalize(client, json_doc, json_doc_size);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("json finalize failed: %d", rc);
	}

	return rc;
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_overwrite_report_construct(qcloud_shadow_client_t *client,
                                                                                                            char *json_doc,
                                                                                                            size_t json_doc_size,
                                                                                                            int count, ...)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    va_list args;
    size_t remain_size = 0;
    int rc_snprintf = 0, i = 0;
    shadow_dev_property_t *dev_property;

    rc = shadow_jsondoc_init(client, json_doc, json_doc_size, QCLOUD_TRUE);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("json init failed: %d", rc);
		QCLOUD_FUNC_EXIT_RC(rc);
	}

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"reported\":{");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    va_start(args, count);

    for (i = 0; i < count; ++i) {
        dev_property = va_arg(args, shadow_dev_property_t *);
        if (!dev_property || !dev_property->key) {
            va_end(args);
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
        }

        rc = shadow_json_node_add(json_doc, remain_size, dev_property->key, dev_property->data, dev_property->type);
        if (rc != QCLOUD_ERR_SUCCESS) {
            va_end(args);
            QCLOUD_FUNC_EXIT_RC(rc);
        }
    }

    va_end(args);

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SHORT;
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc) - 1, remain_size, "},");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("shadow json add report failed: %d", rc);
		QCLOUD_FUNC_EXIT_RC(rc);
	}

	rc = shadow_jsondoc_finalize(client, json_doc, json_doc_size);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("shadow json finalize failed: %d", rc);
	}

	QCLOUD_FUNC_EXIT_RC(rc);
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_report_with_desire_null_construct(qcloud_shadow_client_t *client,
                                                                                                                        char *json_doc,
                                                                                                                        size_t json_doc_size,
                                                                                                                        int count, ...)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);

	qcloud_err_t rc;
    va_list args;
    size_t remain_size = 0;
    int rc_snprintf = 0, i = 0;
    shadow_dev_property_t *dev_property;

    rc = shadow_jsondoc_init(client, json_doc, json_doc_size, QCLOUD_FALSE);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("json init failed: %d", rc);
		QCLOUD_FUNC_EXIT_RC(rc);
	}

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"reported\":{");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    va_start(args, count);

    for (i = 0; i < count; ++i) {
        dev_property = va_arg(args, shadow_dev_property_t *);

        if (!dev_property || !dev_property->key) {
            va_end(args);
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
        }

        rc = shadow_json_node_add(json_doc, remain_size, dev_property->key, dev_property->data, dev_property->type);
        if (rc != QCLOUD_ERR_SUCCESS) {
            va_end(args);
            QCLOUD_FUNC_EXIT_RC(rc);
        }
    }

    va_end(args);

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        return QCLOUD_ERR_JSON_BUFFER_TOO_SHORT;
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc) - 1, remain_size, "},");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("shadow json add report failed: %d", rc);
		QCLOUD_FUNC_EXIT_RC(rc);
	}

	rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"desired\": null ");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

	QCLOUD_FUNC_EXIT_RC(shadow_jsondoc_finalize(client, json_doc, json_doc_size));
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_desire_null_construct(qcloud_shadow_client_t *client,
                                                                                                    char *json_doc,
                                                                                                    size_t json_doc_size)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    size_t remain_size = 0;
    int rc_snprintf = 0;

    rc = shadow_jsondoc_init(client, json_doc, json_doc_size, QCLOUD_FALSE);
    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_LOG_E("json init failed: %d", rc);
        QCLOUD_FUNC_EXIT_RC(rc);
    }

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"desired\": null ");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    QCLOUD_FUNC_EXIT_RC(shadow_jsondoc_finalize(client, json_doc, json_doc_size));
}

__QCLOUD_API__ qcloud_err_t qcloud_shadow_client_desire_construct(qcloud_shadow_client_t *client,
                                                                                            char *json_doc,
                                                                                            size_t json_doc_size,
                                                                                            int count, ...)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;
    va_list args;
    int i = 0, rc_snprintf = 0;
    size_t remain_size = 0;
    shadow_dev_property_t *dev_property;

	rc = shadow_jsondoc_init(client, json_doc, json_doc_size, QCLOUD_FALSE);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("shadow json init failed: %d", rc);
		QCLOUD_FUNC_EXIT_RC(rc);
	}

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc), remain_size, "\"desired\":{");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    va_start(args, count);

    for (i = 0; i < count; ++i) {
        dev_property = va_arg(args, shadow_dev_property_t *);
        if (!dev_property || !dev_property->key) {
            va_end(args);
            QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
        }

        rc = shadow_json_node_add(json_doc, remain_size, dev_property->key, dev_property->data, dev_property->type);
        if (rc != QCLOUD_ERR_SUCCESS) {
			va_end(args);
            QCLOUD_FUNC_EXIT_RC(rc);
        }
    }

    va_end(args);

    if ((remain_size = json_doc_size - strlen(json_doc)) <= 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_JSON_BUFFER_TOO_SHORT);
    }

    rc_snprintf = osal_snprintf(json_doc + strlen(json_doc) - 1, remain_size, "},");
    rc = shadow_json_snprintf_rc2errno(rc_snprintf, remain_size);
	if (rc != QCLOUD_ERR_SUCCESS) {
		QCLOUD_LOG_E("json add desired failed: %d", rc);
		QCLOUD_FUNC_EXIT_RC(rc);
	}

	QCLOUD_FUNC_EXIT_RC(shadow_jsondoc_finalize(client, json_doc, json_doc_size));
}

#ifdef __cplusplus
}
#endif

