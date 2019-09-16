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

static char incoming_msg_from_cloud[QCLOUD_SHADOW_JSON_BUF_MAX];

__QCLOUD_STATIC__ void shadow_glue_json_node_insert(char *json_doc, char *json_node, int pos)
{
    int i, n;
    int len = strlen(json_doc);
    int nlen = strlen(json_node);

    for (i = len - 1; i >= pos; --i) {
        *(json_doc + i + nlen) = *(json_doc + i);
    }

    for (n = 0; n < nlen; n++) {
        *(json_doc + pos + n) = *json_node++;
    }

    *(json_doc + len + nlen) = 0;
}

/**
 * @brief 根据RequestParams、Method来给json填入type字段的值
 */
__QCLOUD_STATIC__ qcloud_err_t shadow_glue_json_request_method_set(char *json_doc, size_t json_doc_size, qcloud_shadow_req_method_t requst_method)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);

    char *type = NULL;
    char json_node[64] = {0};
    size_t json_len, size_remaining, json_node_len;

    switch (requst_method) {
        case QCLOUD_SHADOW_REQUEST_METHOD_GET:
            type = SHADOW_OPERATION_GET;
            break;

        case QCLOUD_SHADOW_REQUEST_METHOD_UPDATE:
            type = SHADOW_OPERATION_UPDATE;
            break;
    }

    json_len = strlen(json_doc);
    size_remaining = json_doc_size - json_len;

    osal_snprintf(json_node, 64, "\"type\":\"%s\", ", type);
    json_node_len = strlen(json_node);

    if (json_node_len >= size_remaining - 1) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    shadow_glue_json_node_insert(json_doc, json_node, 1);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ void shadow_glue_request_destroy(qcloud_shadow_request_t *request)
{
    qcloud_list_del(&request->list);
    osal_free(request);
}

/**
 * @brief 执行设备影子操作的回调函数
 */
__QCLOUD_STATIC__ void shadow_glue_operation_request_do_handle(qcloud_shadow_client_t *client, const char *client_token, const char *method_type)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK_RTN(client);
    QCLOUD_POINTER_SANITY_CHECK_RTN(client_token);
    QCLOUD_POINTER_SANITY_CHECK_RTN(method_type);

    char *delta = NULL;
    int16_t result_code;
    qcloud_list_t *curr, *next;
    qcloud_shadow_request_t *request = NULL;
    qcloud_shadow_req_state_t req_state = QCLOUD_SHADOW_REQUEST_STATE_NONE;

    if (qcloud_list_empty(&client->request_list)) {
        return;
    }

    osal_mutex_lock(client->global_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->request_list) {
        request = QCLOUD_LIST_ENTRY(curr, qcloud_shadow_request_t, list);

        if (strcmp(request->client_token, client_token) != 0) {
            continue;
        }

        // ͨ�� payload ����� result ��ȷ����Ӧ�Ĳ����Ƿ�ɹ�
        // ��result = 0ʱ��payload��Ϊ�գ�result��0ʱ������updateʧ��
        if (!shadow_json_operation_result_code_parse(incoming_msg_from_cloud, &result_code)) {
        	QCLOUD_LOG_E("parse result code failed.");
            shadow_glue_request_destroy(request);
            continue;
        }

        req_state = (result_code == 0 ? QCLOUD_SHADOW_REQUEST_STATE_ACCEPTED : QCLOUD_SHADOW_REQUEST_STATE_REJECTED);

        if ((strcmp(method_type, SHADOW_OPERATION_GET) == 0 && req_state == QCLOUD_SHADOW_REQUEST_STATE_ACCEPTED) ||
            (strcmp(method_type, SHADOW_OPERATION_UPDATE) == 0 && req_state == QCLOUD_SHADOW_REQUEST_STATE_REJECTED)) {
            if (shadow_json_operation_delta_get(incoming_msg_from_cloud, &delta)) {
                shadow_glue_delta_handle(client, delta);
                osal_free(delta);
            }
        }

        if (request->handler) {
            request->handler(client, request->method, req_state, incoming_msg_from_cloud, request->context);
        }

        shadow_glue_request_destroy(request);
        --client->request_list_counter;
    }

    osal_mutex_unlock(client->global_lock);

    QCLOUD_FUNC_EXIT;
}

/**
 * @brief 文档操作请求结果的回调函数
 * 客户端先订阅 $shadow/operation/result/{ProductId}/{DeviceName}, 收到该topic的消息则会调用该回调函数
 * 在这个回调函数中, 解析出各个设备影子文档操作的结果
 */
__QCLOUD_STATIC__ void shadow_glue_operation_result_handler(void *client, mqtt_incoming_msg_t *message, void *private_data)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK_RTN(client);
    QCLOUD_POINTER_SANITY_CHECK_RTN(message);

	int cloud_rcv_len;
	uint32_t version = 0;
	char *client_token = NULL, *method_type = NULL, *delta = NULL;
    qcloud_mqtt_client_t *mqtt_client = NULL;
    qcloud_shadow_client_t *shadow_client = NULL;

    mqtt_client     = (qcloud_mqtt_client_t *)client;
    shadow_client   = (qcloud_shadow_client_t*)mqtt_client->event_handler.context;

    if (!message->topic || message->topic_len <= 0) {
        return;
    }

    if (message->payload_len > QCLOUD_SHADOW_JSON_BUF_MAX) {
        QCLOUD_LOG_E("received len exceeds limit");
        goto out;
    }

    cloud_rcv_len = QCLOUD_MIN(QCLOUD_SHADOW_JSON_BUF_MAX - 1, message->payload_len);
    memcpy(incoming_msg_from_cloud, message->payload, cloud_rcv_len + 1);
    incoming_msg_from_cloud[cloud_rcv_len] = '\0'; // json_parse relies on a string

    // 解析shadow result topic消息类型
    if (!shadow_json_operation_type_parse(incoming_msg_from_cloud, &method_type)) {
        QCLOUD_LOG_E("fail to parse type!");
        goto out;
    }

    // 非delta消息的push，一定由设备端触发，找到设备段对应的client_token
    if (strcmp(method_type, SHADOW_OPERATION_DELTA) != 0 &&
        !shadow_json_client_token_parse(incoming_msg_from_cloud, &client_token)) {
		QCLOUD_LOG_E("fail to parse client token! json=%s", incoming_msg_from_cloud);
		goto out;
    }

    // 获取shadow push消息version，如果比本地的version则修改本地version，比本地可能是由于服务器回滚或出错
	if (shadow_json_version_parse(incoming_msg_from_cloud, &version) &&
        version > shadow_client->version) {
        shadow_client->version = version;
    }

    if (strcmp(method_type, SHADOW_OPERATION_DELTA) == 0) {
        if (shadow_json_delta_parse(incoming_msg_from_cloud, &delta)) {
			QCLOUD_LOG_D("delta: %s", delta);
        	shadow_glue_delta_handle(shadow_client, delta);
        }

        goto out;
    }

    if (shadow_client) {
        shadow_glue_operation_request_do_handle(shadow_client, client_token, method_type);
    }

out:
    if (!method_type) {
        osal_free(method_type);
    }

    if (!client_token) {
        osal_free(client_token);
    }

    if (!delta) {
        osal_free(delta);
    }
}

/**
 * @brief 发布文档请求到物联云
 *
 * @param client                   Qcloud_IoT_Client对象
 * @param method                    文档操作方式
 * @param pJsonDoc                  等待发送的文档
 * @return 返回QCLOUD_ERR_SUCCESS, 表示发布文档请求成功
 */
__QCLOUD_STATIC__ qcloud_err_t shadow_glue_operation_request_publish(qcloud_shadow_client_t *client,
                                                                                                char *json_doc)
{
    QCLOUD_FUNC_ENTRY;

    mqtt_publish_opt_t publish_opt;

    memset(&publish_opt, 0, sizeof(mqtt_publish_opt_t));
    publish_opt.qos         = MQTT_QOS0;
    publish_opt.payload     = (void *)json_doc;
    publish_opt.payload_len = strlen(json_doc);

    QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_client_publish(&client->mqtt_client, client->request_topic_publish, &publish_opt));
}

__QCLOUD_INTERNAL__ qcloud_err_t shadow_glue_operation_request_subscribe(qcloud_shadow_client_t *client)
{
    mqtt_subscribe_opt_t subscribe_opt;

    subscribe_opt.message_handler   = shadow_glue_operation_result_handler;
    subscribe_opt.private_data      = NULL;
    subscribe_opt.qos               = MQTT_QOS0;

    QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_client_subscribe(&client->mqtt_client, client->request_topic_subscribe, &subscribe_opt));
}

/**
 * @brief 取消订阅topic: $shadow/operation/result/{ProductId}/{DeviceName}
 */
__QCLOUD_INTERNAL__ qcloud_err_t shadow_glue_operation_request_unsubscribe(qcloud_shadow_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_client_unsubscribe(&client->mqtt_client, client->request_topic_subscribe));
}

__QCLOUD_INTERNAL__ qcloud_err_t shadow_glue_request_list_scan(qcloud_shadow_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    qcloud_list_t *curr, *next;
    qcloud_shadow_request_t *request;

    if (qcloud_list_empty(&client->request_list)) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
    }

    osal_mutex_lock(client->global_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->request_list) {
        request = QCLOUD_LIST_ENTRY(curr, qcloud_shadow_request_t, list);

        // check whether the ack is timeout
        if (!osal_timer_is_expired(&request->timer)) {
            continue;
        }

        if (request->handler) {
            request->handler(client, request->method, QCLOUD_SHADOW_REQUEST_STATE_TIMEOUT, incoming_msg_from_cloud, request->context);
        }

        shadow_glue_request_destroy(request);
        --client->request_list_counter;
    }

    osal_mutex_unlock(client->global_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
 * @brief 将设备影子文档的操作请求保存在列表中
 */
__QCLOUD_STATIC__ qcloud_err_t shadow_glue_request_record(qcloud_shadow_client_t *client,
                                                                                const char *client_token,
                                                                                qcloud_shadow_req_opt_t *request_opt)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_shadow_request_t *request = NULL;

    if (client->request_list_counter >= QCLOUD_SHADOW_REQUEST_PENDING_MAX) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_MAX_APPENDING_REQUEST);
    }

    request = (qcloud_shadow_request_t *)osal_malloc(sizeof(qcloud_shadow_request_t));
    if (!request) {
        QCLOUD_LOG_E("malloc failed!");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    request->handler    = request_opt->handler;
    request->context    = request_opt->context;
    request->method     = request_opt->method;
    strncpy(request->client_token, client_token, QCLOUD_SHADOW_CLIENT_TOKEN_MAX);

    osal_timer_init(&request->timer);
    osal_timer_countdown(&request->timer, request_opt->timeout);

    qcloud_list_init(&request->list);

    osal_mutex_lock(client->global_lock);
    qcloud_list_add(&request->list, &client->request_list);
    ++client->request_list_counter;
    osal_mutex_unlock(client->global_lock);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_INTERNAL__ qcloud_err_t shadow_glue_request_post(qcloud_shadow_client_t *client,
                                                                            qcloud_shadow_req_opt_t *request_opt,
                                                                            char *json_doc,
                                                                            size_t json_doc_size)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(request_opt, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(json_doc, QCLOUD_ERR_INVAL);

    qcloud_err_t rc = QCLOUD_ERR_SUCCESS;
    char *client_token = NULL;

    // 解析文档中的clientToken, 如果解析失败, 直接返回错误
    if (!shadow_json_client_token_parse(json_doc, &client_token)) {
        QCLOUD_LOG_E("fail to parse client token!");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    rc = shadow_glue_json_request_method_set(json_doc, json_doc_size, request_opt->method);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(rc, QCLOUD_ERR_SUCCESS, rc);

    // 相应的 operation topic 订阅成功或已经订阅
    rc = shadow_glue_operation_request_publish(client, json_doc);
    if (rc == QCLOUD_ERR_SUCCESS) {
        rc = shadow_glue_request_record(client, client_token, request_opt);
    }

    osal_free(client_token);

    QCLOUD_FUNC_EXIT_RC(rc);
}

/**
 * @brief 处理注册属性的回调函数
 * 当订阅的$shadow/operation/result/{ProductId}/{DeviceName}返回消息时，
 * 若对应的type为delta, 则执行该函数
 *
 */
__QCLOUD_INTERNAL__ void shadow_glue_delta_handle(qcloud_shadow_client_t *client, char *delta)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_list_t *curr, *next;
    qcloud_shadow_prop_info_t *property_info = NULL;
    shadow_dev_property_t *dev_property = NULL;

    if (qcloud_list_empty(&client->property_list)) {
        return;
    }

    osal_mutex_lock(client->global_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->property_list) {
        property_info   = QCLOUD_LIST_ENTRY(curr, qcloud_shadow_prop_info_t, list);
        dev_property    = property_info->dev_property;

        if (shadow_json_value_update(delta, dev_property)) {
            property_info->handler(client, delta, strlen(delta), dev_property);
        }
    }

    osal_mutex_unlock(client->global_lock);

    QCLOUD_FUNC_EXIT;
}

__QCLOUD_INTERNAL__ void shadow_glue_property_list_destroy(qcloud_shadow_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_list_t *curr, *next;
    qcloud_shadow_prop_info_t *property_info = NULL;

    if (qcloud_list_empty(&client->property_list)) {
        return;
    }

    osal_mutex_lock(client->global_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->property_list) {
        property_info = QCLOUD_LIST_ENTRY(curr, qcloud_shadow_prop_info_t, list);

        qcloud_list_del(&property_info->list);
        osal_free(property_info);
    }

    osal_mutex_unlock(client->global_lock);

    QCLOUD_FUNC_EXIT;
}

__QCLOUD_INTERNAL__ void shadow_glue_request_list_destroy(qcloud_shadow_client_t *client)
{
    QCLOUD_FUNC_ENTRY;

    qcloud_list_t *curr, *next;
    qcloud_shadow_request_t *request = NULL;

    if (qcloud_list_empty(&client->request_list)) {
        return;
    }

    osal_mutex_lock(client->global_lock);

    QCLOUD_LIST_FOR_EACH_SAFE(curr, next, &client->request_list) {
        request = QCLOUD_LIST_ENTRY(curr, qcloud_shadow_request_t, list);

        qcloud_list_del(&request->list);
        osal_free(request);
    }

    osal_mutex_unlock(client->global_lock);

    QCLOUD_FUNC_EXIT;
}

#ifdef __cplusplus
}
#endif

