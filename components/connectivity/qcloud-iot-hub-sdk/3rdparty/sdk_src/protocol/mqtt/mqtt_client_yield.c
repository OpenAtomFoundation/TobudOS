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
 *    Allan Stockdill-Mander/Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "log_upload.h"
#include "mqtt_client.h"
#include "qcloud_iot_import.h"

static uint32_t _get_random_interval(void)
{
    srand((unsigned)HAL_GetTimeMs());
    /* range: 1000 - 2000 ms, in 10ms unit */
    return (rand() % 100 + 100) * 10;
}

static void _iot_disconnect_callback(Qcloud_IoT_Client *pClient)
{
    if (NULL != pClient->event_handle.h_fp) {
        MQTTEventMsg msg;
        msg.event_type = MQTT_EVENT_DISCONNECT;
        msg.msg        = NULL;

        pClient->event_handle.h_fp(pClient, pClient->event_handle.context, &msg);
    }
}

static void _reconnect_callback(Qcloud_IoT_Client *pClient)
{
    if (NULL != pClient->event_handle.h_fp) {
        MQTTEventMsg msg;
        msg.event_type = MQTT_EVENT_RECONNECT;
        msg.msg        = NULL;

        pClient->event_handle.h_fp(pClient, pClient->event_handle.context, &msg);
    }
}

/**
 * @brief handle exceptional disconnection
 *
 * @param pClient
 * @return
 */
static int _handle_disconnect(Qcloud_IoT_Client *pClient)
{
    IOT_FUNC_ENTRY;
    int rc;

    if (0 == get_client_conn_state(pClient)) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    rc = qcloud_iot_mqtt_disconnect(pClient);
    // disconnect network stack by force
    if (rc != QCLOUD_RET_SUCCESS) {
        pClient->network_stack.disconnect(&(pClient->network_stack));
        set_client_conn_state(pClient, NOTCONNECTED);
    }

    Log_e("disconnect MQTT for some reasons..");

    _iot_disconnect_callback(pClient);

    // exceptional disconnection
    pClient->was_manually_disconnected = 0;
    IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
}

/**
 * @brief handle reconnect
 *
 * @param pClient
 * @return
 */
static int _handle_reconnect(Qcloud_IoT_Client *pClient)
{
    IOT_FUNC_ENTRY;

    int8_t isPhysicalLayerConnected = 1;
    int    rc                       = QCLOUD_RET_MQTT_RECONNECTED;

    // reconnect control by delay timer (increase interval exponentially )
    if (!expired(&(pClient->reconnect_delay_timer))) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT);
    }

    if (NULL != pClient->network_stack.is_connected) {
        isPhysicalLayerConnected =
            (int8_t)pClient->network_stack.is_connected(&(pClient->network_stack));  // always return 1
    }

    if (isPhysicalLayerConnected) {
        rc = qcloud_iot_mqtt_attempt_reconnect(pClient);
        if (rc == QCLOUD_RET_MQTT_RECONNECTED) {
            Log_e("attempt to reconnect success.");
            _reconnect_callback(pClient);
#ifdef LOG_UPLOAD
            if (is_log_uploader_init()) {
                int log_level;
                if (qcloud_get_log_level(pClient, &log_level) < 0) {
                    Log_e("client get log topic failed: %d", rc);
                }
            }
#endif
            IOT_FUNC_EXIT_RC(rc);
        } else {
            Log_e("attempt to reconnect failed, errCode: %d", rc);
            rc = QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT;
        }
    }

    pClient->current_reconnect_wait_interval *= 2;

    if (MAX_RECONNECT_WAIT_INTERVAL < pClient->current_reconnect_wait_interval) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT);
    }
    countdown_ms(&(pClient->reconnect_delay_timer), pClient->current_reconnect_wait_interval);

    IOT_FUNC_EXIT_RC(rc);
}

/**
 * @brief handle MQTT keep alive (hearbeat with server)
 *
 * @param pClient
 * @return
 */
static int _mqtt_keep_alive(Qcloud_IoT_Client *pClient)
{
#define MQTT_PING_RETRY_TIMES 2

    IOT_FUNC_ENTRY;

    int      rc;
    Timer    timer;
    uint32_t serialized_len = 0;

    if (0 == pClient->options.keep_alive_interval) {
        IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
    }

    if (!expired(&pClient->ping_timer)) {
        IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
    }

    if (pClient->is_ping_outstanding >= MQTT_PING_RETRY_TIMES) {
        // Reaching here means we haven't received any MQTT packet for a long time (keep_alive_interval)
        Log_e("Fail to recv MQTT msg. Something wrong with the connection.");
        rc = _handle_disconnect(pClient);
        IOT_FUNC_EXIT_RC(rc);
    }

    /* there is no ping outstanding - send one */
    HAL_MutexLock(pClient->lock_write_buf);
    rc = serialize_packet_with_zero_payload(pClient->write_buf, pClient->write_buf_size, PINGREQ, &serialized_len);
    if (QCLOUD_RET_SUCCESS != rc) {
        HAL_MutexUnlock(pClient->lock_write_buf);
        IOT_FUNC_EXIT_RC(rc);
    }

    /* send the ping packet */
    int i = 0;
    InitTimer(&timer);
    do {
        countdown_ms(&timer, pClient->command_timeout_ms);
        rc = send_mqtt_packet(pClient, serialized_len, &timer);
    } while (QCLOUD_RET_SUCCESS != rc && (i++ < 3));

    if (QCLOUD_RET_SUCCESS != rc) {
        HAL_MutexUnlock(pClient->lock_write_buf);
        // If sending a PING fails, propably the connection is not OK and we decide to disconnect and begin reconnection
        // attempts
        Log_e("Fail to send PING request. Something wrong with the connection.");
        rc = _handle_disconnect(pClient);
        IOT_FUNC_EXIT_RC(rc);
    }
    HAL_MutexUnlock(pClient->lock_write_buf);

    HAL_MutexLock(pClient->lock_generic);
    pClient->is_ping_outstanding++;
    /* start a timer to wait for PINGRESP from server */
    countdown(&pClient->ping_timer, Min(5, pClient->options.keep_alive_interval / 2));
    HAL_MutexUnlock(pClient->lock_generic);
    Log_d("PING request %u has been sent...", pClient->is_ping_outstanding);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 * @brief Check connection and keep alive state, read/handle MQTT message in synchronized way
 *
 * @param pClient    handle to MQTT client
 * @param timeout_ms timeout value (unit: ms) for this operation
 *
 * @return QCLOUD_RET_SUCCESS when success, QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT when try reconnecing, or err code for
 * failure
 */
int qcloud_iot_mqtt_yield(Qcloud_IoT_Client *pClient, uint32_t timeout_ms)
{
    IOT_FUNC_ENTRY;

    int     rc = QCLOUD_RET_SUCCESS;
    Timer   timer;
    uint8_t packet_type;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);
    NUMBERIC_SANITY_CHECK(timeout_ms, QCLOUD_ERR_INVAL);

    // 1. check if manually disconnect
    if (!get_client_conn_state(pClient) && pClient->was_manually_disconnected == 1) {
        IOT_FUNC_EXIT_RC(QCLOUD_RET_MQTT_MANUALLY_DISCONNECTED);
    }

    // 2. check connection state and if auto reconnect is enabled
    if (!get_client_conn_state(pClient) && pClient->options.auto_connect_enable == 0) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    InitTimer(&timer);
    countdown_ms(&timer, timeout_ms);

    // 3. main loop for packet reading/handling and keep alive maintainance
    while (!expired(&timer)) {
        if (!get_client_conn_state(pClient)) {
            if (pClient->current_reconnect_wait_interval > MAX_RECONNECT_WAIT_INTERVAL) {
                rc = QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT;
                break;
            }
            rc = _handle_reconnect(pClient);

            continue;
        }

        rc = cycle_for_read(pClient, &timer, &packet_type, QOS0);

        if (rc == QCLOUD_RET_SUCCESS) {
            /* check list of wait publish ACK to remove node that is ACKED or timeout */
            qcloud_iot_mqtt_pub_info_proc(pClient);

            /* check list of wait subscribe(or unsubscribe) ACK to remove node that is ACKED or timeout */
            qcloud_iot_mqtt_sub_info_proc(pClient);

            rc = _mqtt_keep_alive(pClient);
        } else if (rc == QCLOUD_ERR_SSL_READ_TIMEOUT || rc == QCLOUD_ERR_SSL_READ ||
                   rc == QCLOUD_ERR_TCP_PEER_SHUTDOWN || rc == QCLOUD_ERR_TCP_READ_FAIL) {
            Log_e("network read failed, rc: %d. MQTT Disconnect.", rc);
            rc = _handle_disconnect(pClient);
        }

        if (rc == QCLOUD_ERR_MQTT_NO_CONN) {
            pClient->counter_network_disconnected++;

            if (pClient->options.auto_connect_enable == 1) {
                pClient->current_reconnect_wait_interval = _get_random_interval();
                countdown_ms(&(pClient->reconnect_delay_timer), pClient->current_reconnect_wait_interval);

                // reconnect timeout
                rc = QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT;
            } else {
                break;
            }
        } else if (rc != QCLOUD_RET_SUCCESS) {
            break;
        }
    }

    IOT_FUNC_EXIT_RC(rc);
}

// workaround wrapper for qcloud_iot_mqtt_yield for multi-thread mode
int qcloud_iot_mqtt_yield_mt(Qcloud_IoT_Client *mqtt_client, uint32_t timeout_ms)
{
    POINTER_SANITY_CHECK(mqtt_client, QCLOUD_ERR_INVAL);
    NUMBERIC_SANITY_CHECK(timeout_ms, QCLOUD_ERR_INVAL);

#ifdef MULTITHREAD_ENABLED
    /* only one instance of yield is allowed in running state*/
    if (mqtt_client->thread_running) {
        HAL_SleepMs(timeout_ms);
        return QCLOUD_RET_SUCCESS;
    }
#endif

    return qcloud_iot_mqtt_yield(mqtt_client, timeout_ms);
}

/**
 * @brief puback waiting timeout process
 *
 * @param pClient reference to MQTTClient
 *
 */
int qcloud_iot_mqtt_pub_info_proc(Qcloud_IoT_Client *pClient)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    HAL_MutexLock(pClient->lock_list_pub);
    do {
        if (0 == pClient->list_pub_wait_ack->len) {
            break;
        }

        ListIterator *iter;
        ListNode *    node      = NULL;
        ListNode *    temp_node = NULL;

        if (NULL == (iter = list_iterator_new(pClient->list_pub_wait_ack, LIST_TAIL))) {
            Log_e("new list failed");
            break;
        }

        for (;;) {
            node = list_iterator_next(iter);

            if (NULL != temp_node) {
                list_remove(pClient->list_pub_wait_ack, temp_node);
                temp_node = NULL;
            }

            if (NULL == node) {
                break; /* end of list */
            }

            QcloudIotPubInfo *repubInfo = (QcloudIotPubInfo *)node->val;
            if (NULL == repubInfo) {
                Log_e("node's value is invalid!");
                temp_node = node;
                continue;
            }

            /* remove invalid node */
            if (MQTT_NODE_STATE_INVALID == repubInfo->node_state) {
                temp_node = node;
                continue;
            }

            if (!pClient->is_connected) {
                continue;
            }

            /* check the request if timeout or not */
            if (left_ms(&repubInfo->pub_start_time) > 0) {
                continue;
            }

            HAL_MutexUnlock(pClient->lock_list_pub);
            /* If wait ACK timeout, remove the node from list */
            /* It is up to user to do republishing or not */
            temp_node = node;

            countdown_ms(&repubInfo->pub_start_time, pClient->command_timeout_ms);
            HAL_MutexLock(pClient->lock_list_pub);

            /* notify timeout event */
            if (NULL != pClient->event_handle.h_fp) {
                MQTTEventMsg msg;
                msg.event_type = MQTT_EVENT_PUBLISH_TIMEOUT;
                msg.msg        = (void *)(uintptr_t)repubInfo->msg_id;
                pClient->event_handle.h_fp(pClient, pClient->event_handle.context, &msg);
            }
        }

        list_iterator_destroy(iter);

    } while (0);

    HAL_MutexUnlock(pClient->lock_list_pub);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

/**
 * @brief suback waiting timeout process
 *
 * @param pClient reference to MQTTClient
 *
 */
int qcloud_iot_mqtt_sub_info_proc(Qcloud_IoT_Client *pClient)
{
    IOT_FUNC_ENTRY;
    int rc = QCLOUD_RET_SUCCESS;

    if (!pClient) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    HAL_MutexLock(pClient->lock_list_sub);
    do {
        if (0 == pClient->list_sub_wait_ack->len) {
            break;
        }

        ListIterator *iter;
        ListNode *    node      = NULL;
        ListNode *    temp_node = NULL;
        uint16_t      packet_id = 0;
        MessageTypes  msg_type;

        if (NULL == (iter = list_iterator_new(pClient->list_sub_wait_ack, LIST_TAIL))) {
            Log_e("new list failed");
            HAL_MutexUnlock(pClient->lock_list_sub);
            IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
        }

        for (;;) {
            node = list_iterator_next(iter);

            if (NULL != temp_node) {
                list_remove(pClient->list_sub_wait_ack, temp_node);
                temp_node = NULL;
            }

            if (NULL == node) {
                break; /* end of list */
            }

            QcloudIotSubInfo *sub_info = (QcloudIotSubInfo *)node->val;
            if (NULL == sub_info) {
                Log_e("node's value is invalid!");
                temp_node = node;
                continue;
            }

            /* remove invalid node */
            if (MQTT_NODE_STATE_INVALID == sub_info->node_state) {
                temp_node = node;
                continue;
            }

            if (pClient->is_connected <= 0) {
                continue;
            }

            /* check the request if timeout or not */
            if (left_ms(&sub_info->sub_start_time) > 0) {
                continue;
            }

            /* When arrive here, it means timeout to wait ACK */
            packet_id = sub_info->msg_id;
            msg_type  = sub_info->type;

            /* Wait MQTT SUBSCRIBE ACK timeout */
            if (NULL != pClient->event_handle.h_fp) {
                MQTTEventMsg msg;

                if (SUBSCRIBE == msg_type) {
                    /* subscribe timeout */
                    msg.event_type = MQTT_EVENT_SUBCRIBE_TIMEOUT;
                    msg.msg        = (void *)(uintptr_t)packet_id;

                    /* notify this event to topic subscriber */
                    if (NULL != sub_info->handler.sub_event_handler)
                        sub_info->handler.sub_event_handler(pClient, MQTT_EVENT_SUBCRIBE_TIMEOUT,
                                                            sub_info->handler.handler_user_data);

                } else {
                    /* unsubscribe timeout */
                    msg.event_type = MQTT_EVENT_UNSUBCRIBE_TIMEOUT;
                    msg.msg        = (void *)(uintptr_t)packet_id;
                }

                pClient->event_handle.h_fp(pClient, pClient->event_handle.context, &msg);
            }

            if (NULL != sub_info->handler.topic_filter)
                HAL_Free((void *)(sub_info->handler.topic_filter));

            temp_node = node;
        }

        list_iterator_destroy(iter);

    } while (0);

    HAL_MutexUnlock(pClient->lock_list_sub);

    IOT_FUNC_EXIT_RC(rc);
}

#ifdef __cplusplus
}
#endif
