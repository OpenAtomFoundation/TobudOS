#include "qcloud.h"

__QCLOUD_STATIC__ qcloud_err_t mqtt_client_network_host_construct(qcloud_network_t *network, qcloud_device_t *device)
{
    int server_len;
    char mqtt_server[QCLOUD_SERVER_DOMAIN_MAX];

    memset(network->host, 0, sizeof(network->host));
    server_len = osal_snprintf(mqtt_server, sizeof(mqtt_server), "%s.%s", device->product_id, qcloud_mqtt_server);
    if (server_len < 0 || server_len > QCLOUD_SERVER_DOMAIN_MAX - 1) {
		QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
	}
    memcpy(network->host, mqtt_server, sizeof(network->host));

    network->port = qcloud_mqtt_port;

    return QCLOUD_ERR_SUCCESS;
}

__QCLOUD_STATIC__ qcloud_err_t mqtt_client_network_init(qcloud_network_t *network, qcloud_device_t *device)
{
#if (QCLOUD_CFG_TLS_EN > 0u)
    QCLOUD_FUNC_EXIT_RC_IF_NOT(qcloud_tls_init(&network->tls_opt, device), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);
    QCLOUD_FUNC_EXIT_RC_IF_NOT(qcloud_network_tls_init(network), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);
#else
    QCLOUD_FUNC_EXIT_RC_IF_NOT(qcloud_network_tcp_init(network), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);
#endif

    QCLOUD_FUNC_EXIT_RC_IF_NOT(mqtt_client_network_host_construct(network, device), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

    return QCLOUD_ERR_SUCCESS;
}

__QCLOUD_STATIC__ uint16_t mqtt_client_random_packet_id_generate(void)
{
#define PACKET_ID_MAX   (65535)

    srand((unsigned)osal_timer_current_sec());
    return rand() % (PACKET_ID_MAX + 1) + 1;
}

__QCLOUD_STATIC__ qcloud_err_t mqtt_client_construct(qcloud_mqtt_client_t *client,
                                                                    mqtt_event_handler_fn_t handler,
                                                                    void *handler_context,
                                                                    qcloud_auto_connect_state_t auto_connect_state)
{
    client->event_handler.handler           = handler;
    client->event_handler.context           = handler_context;
    client->auto_connect_state              = auto_connect_state;

    client->command_timeout                 = QCLOUD_MQTT_COMMAND_TIMEOUT;
    client->connection_state                = QCLOUD_MQTT_CONNECTION_STATE_DISCONNECTED;

    // packet id 取随机数 1- 65536
    client->packet_id                       = mqtt_client_random_packet_id_generate();
    client->ping_outstanding                = 0;
    client->is_manually_disconnected        = QCLOUD_FALSE;
    client->network_disconnect_counter      = 0;

    if ((client->global_lock = osal_mutex_create()) == NULL) {
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    if ((client->tx_lock = osal_mutex_create()) == NULL) {
        QCLOUD_LOG_E("write buf lock failed.");
        goto errout;
    }

    if ((client->ack_pend_list_lock = osal_mutex_create()) == NULL) {
        QCLOUD_LOG_E("ack list lock failed.");
        goto errout;
    }

    if ((client->msg_handler_list_lock = osal_mutex_create()) == NULL) {
        QCLOUD_LOG_E("handler list lock failed.");
        goto errout;
    }

    qcloud_list_init(&client->ack_pend_list);

    qcloud_list_init(&client->msg_handler_list);

    // ping定时器以及重连延迟定时器相关初始化
    osal_timer_init(&client->ping_timer);
    osal_timer_init(&client->reconnect_timer);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);

errout:
    if (client->global_lock) {
        osal_mutex_destroy(client->global_lock);
        client->global_lock = NULL;
    }

    if (client->tx_lock) {
        osal_mutex_destroy(client->tx_lock);
        client->tx_lock = NULL;
    }

    if (client->ack_pend_list_lock) {
        osal_mutex_destroy(client->ack_pend_list_lock);
        client->ack_pend_list_lock = NULL;
    }

    if (client->msg_handler_list_lock) {
        osal_mutex_destroy(client->msg_handler_list_lock);
        client->msg_handler_list_lock = NULL;
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE)
}

#if (QCLOUD_CFG_TLS_EN == 0u) && (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_KEY)
__QCLOUD_STATIC__ qcloud_err_t mqtt_client_key_decode(uint8_t *decoded_buf, size_t decoded_buf_size, const char *key, size_t *decoded_key_len)
{
    QCLOUD_POINTER_SANITY_CHECK(decoded_buf, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(key, QCLOUD_ERR_INVAL);

    int key_len = 0;

    memset(decoded_buf, 0, decoded_buf_size);

    key_len = strlen(key);
    if (qcloud_utils_base64_decode(decoded_buf, decoded_buf_size, decoded_key_len,
                                            (unsigned char *)key, key_len) != 0) {
        QCLOUD_LOG_E("psk decode failed!");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ qcloud_err_t mqtt_client_password_encode(mqtt_connect_opt_t *connect_opt, uint8_t *key, size_t key_len)
{
    char digest[41];
    int password_len = 0;

    memset(digest, 0, sizeof(digest));
    utils_hmac_sha1(connect_opt->username, strlen(connect_opt->username), digest, (const char *)key, key_len);

    password_len = osal_snprintf(connect_opt->password, sizeof(connect_opt->password), "%s;hmacsha1", digest);
    if (password_len < 0 || password_len >= QCLOUD_DEVICE_PASSWORD_MAX) {
        QCLOUD_LOG_E("password encode failed");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_DEV_INFO);
    }

    connect_opt->password_len = password_len;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

#endif

__QCLOUD_STATIC__ qcloud_err_t mqtt_client_id_generate(mqtt_connect_opt_t *connect_opt, qcloud_device_t *device)
{
    int client_id_len = 0;

    memset(connect_opt->client_id, 0, sizeof(connect_opt->client_id));

    client_id_len = osal_snprintf(connect_opt->client_id, sizeof(connect_opt->client_id), "%s%s", device->product_id, device->device_name);
    if (client_id_len < 0 || client_id_len >= QCLOUD_MQTT_DEVICE_CLIENT_ID_MAX) {
        QCLOUD_LOG_E("client id generate failed");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_DEV_INFO);
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_STATIC__ qcloud_err_t mqtt_client_username_generate(mqtt_connect_opt_t *connect_opt)
{
    uint32_t now;
    int username_len = 0;
    char connection_id[QCLOUD_MQTT_CONNECT_ID_MAX + 1];

    now = osal_timer_current_sec() + QCLOUD_MQTT_ACCESS_EXPIRE_TIMEOUT_MAX / 1000;

    mqtt_glue_connect_id_generate(connection_id);

    memset(connect_opt->username, 0, sizeof(connect_opt->username));

    username_len = osal_snprintf(connect_opt->username, sizeof(connect_opt->username), "%s;%s;%s;%ld",
                                    connect_opt->client_id, QCLOUD_APPID, connection_id, now);
    if (username_len < 0 || username_len >= QCLOUD_DEVICE_USERNAME_MAX) {
        QCLOUD_LOG_E("username generate failed");
        QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_DEV_INFO);
    }

    connect_opt->username_len = username_len;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_INTERNAL__ void mqtt_client_connection_state_set(qcloud_mqtt_client_t *client, qcloud_mqtt_con_status_t state)
{
    osal_mutex_lock(client->global_lock);
    client->connection_state = state;
    osal_mutex_unlock(client->global_lock);
}

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_connect_opt_create(mqtt_connect_opt_t *connect_opt,
                                                                                    qcloud_device_t *device,
                                                                                    mqtt_version_t mqtt_version,
                                                                                    uint16_t keep_alive_interval,
                                                                                    mqtt_clean_session_state_t clean_session)
{
    QCLOUD_POINTER_SANITY_CHECK(connect_opt, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(device, QCLOUD_ERR_INVAL);

#if (QCLOUD_CFG_TLS_EN == 0u)
        uint8_t decoded_key[QCLOUD_PSK_MAX];
        size_t  decoded_key_len;
#endif

    memset(connect_opt, 0, sizeof(mqtt_connect_opt_t));

    // 1. client id
    QCLOUD_FUNC_EXIT_RC_IF_NOT(mqtt_client_id_generate(connect_opt, device), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

    // 2. username
    QCLOUD_FUNC_EXIT_RC_IF_NOT(mqtt_client_username_generate(connect_opt), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

#if (QCLOUD_CFG_TLS_EN == 0u) && (QCLOUD_CFG_AUTH_MODE == QCLOUD_AUTH_MODE_KEY)
    // 3. key(temporary)
    QCLOUD_FUNC_EXIT_RC_IF_NOT(mqtt_client_key_decode(decoded_key, sizeof(decoded_key), device->key, &decoded_key_len), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

    // 4. password
    QCLOUD_FUNC_EXIT_RC_IF_NOT(mqtt_client_password_encode(connect_opt, decoded_key, decoded_key_len), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);
#endif

    connect_opt->mqtt_version           = mqtt_version;
    // keep alive interval is in second, no long than 11.5 minutes(11.5 * 60 seconds)
    connect_opt->keep_alive_interval    = QCLOUD_MIN(keep_alive_interval, 11.5 * 60);
    connect_opt->clean_session          = clean_session;

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_create(qcloud_mqtt_client_t *client,
                                                                        qcloud_device_t *device,
                                                                        mqtt_event_handler_fn_t handler,
                                                                        void *handler_context,
                                                                        qcloud_auto_connect_state_t auto_connect_state)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(device, QCLOUD_ERR_INVAL);

    memset(client, 0, sizeof(qcloud_mqtt_client_t));

    QCLOUD_FUNC_EXIT_RC_IF_NOT(mqtt_client_network_init(&client->network, device), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

    QCLOUD_FUNC_EXIT_RC_IF_NOT(mqtt_client_construct(client, handler, handler_context, auto_connect_state), QCLOUD_ERR_SUCCESS, QCLOUD_ERR_FAILURE);

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_API__ void qcloud_mqtt_client_destroy(qcloud_mqtt_client_t *client)
{
#if (QCLOUD_CFG_DUPLICATED_MSG_REMOVE_EN > 0u)
    mqtt_glue_packet_id_cache_reset();
#endif

    if (qcloud_mqtt_client_is_connected(client)) {
        qcloud_mqtt_client_disconnect(client);
    }

    mqtt_glue_ack_list_destroy(client);

    osal_mutex_destroy(client->ack_pend_list_lock);
    osal_mutex_destroy(client->global_lock);
    osal_mutex_destroy(client->tx_lock);
}

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_publish(qcloud_mqtt_client_t *client, char *topic, mqtt_publish_opt_t *publish_opt)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(topic, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(publish_opt, QCLOUD_ERR_INVAL);

	QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_publish(client, topic, publish_opt));
}

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_subscribe(qcloud_mqtt_client_t *client, const char *topic_filter, mqtt_subscribe_opt_t *subscribe_opt)
{
	QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(topic_filter, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(subscribe_opt, QCLOUD_ERR_INVAL);

	QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_subscribe(client, topic_filter, subscribe_opt));
}

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_unsubscribe(qcloud_mqtt_client_t *client, const char *topic_filter)
{
    QCLOUD_FUNC_ENTRY;

    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(topic_filter, QCLOUD_ERR_INVAL);

	QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_unsubscribe(client, topic_filter));
}

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_yield(qcloud_mqtt_client_t *client, mqtt_connect_opt_t *connect_opt, uint32_t timeout_ms)
{
	QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_yield(client, connect_opt, timeout_ms));
}

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_disconnect(qcloud_mqtt_client_t *client)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    QCLOUD_FUNC_EXIT_RC(qcloud_mqtt_disconnect(client));
}

__QCLOUD_API__ qcloud_err_t qcloud_mqtt_client_connect(qcloud_mqtt_client_t *client, mqtt_connect_opt_t *connect_opt)
{
    QCLOUD_POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    QCLOUD_POINTER_SANITY_CHECK(connect_opt, QCLOUD_ERR_INVAL);

    qcloud_err_t rc;

    QCLOUD_FUNC_EXIT_RC_IF(client->connection_state, QCLOUD_MQTT_CONNECTION_STATE_CONNECTED, QCLOUD_ERR_MQTT_ALREADY_CONNECTED);

    rc = qcloud_mqtt_connect(client, connect_opt);
    if (rc != QCLOUD_ERR_SUCCESS) {
        QCLOUD_LOG_E("mqtt connect failed: %d", rc);
    } else {
        QCLOUD_LOG_I("mqtt connect success");
    }

    QCLOUD_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

__QCLOUD_API__ int qcloud_mqtt_client_is_connected(qcloud_mqtt_client_t *client)
{
	int is_connected = 0;

    if (!client) {
        return QCLOUD_FALSE;
    }

	osal_mutex_lock(client->global_lock);
	is_connected = client->connection_state == QCLOUD_MQTT_CONNECTION_STATE_CONNECTED;
	osal_mutex_unlock(client->global_lock);

    return is_connected ? QCLOUD_TRUE : QCLOUD_FALSE;
}

