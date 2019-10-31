#include "iotx_dm_internal.h"

static dm_client_ctx_t g_dm_client_ctx = {0};

static dm_client_ctx_t *dm_client_get_ctx(void)
{
    return &g_dm_client_ctx;
}

int dm_client_open(void)
{
    int res = 0;
    dm_client_ctx_t *ctx = dm_client_get_ctx();
    iotx_cm_init_param_t cm_param;

    memset(ctx, 0, sizeof(dm_client_ctx_t));
    memset(&cm_param, 0, sizeof(iotx_cm_init_param_t));

    cm_param.request_timeout_ms = IOTX_DM_CLIENT_REQUEST_TIMEOUT_MS;
    cm_param.keepalive_interval_ms = IOTX_DM_CLIENT_KEEPALIVE_INTERVAL_MS;
    cm_param.write_buf_size = CONFIG_MQTT_TX_MAXLEN;
    cm_param.read_buf_size = CONFIG_MQTT_RX_MAXLEN;
#if defined(COAP_COMM_ENABLED) && !defined(MQTT_COMM_ENABLED)
    cm_param.protocol_type = IOTX_CM_PROTOCOL_TYPE_COAP;
#else
    cm_param.protocol_type = IOTX_CM_PROTOCOL_TYPE_MQTT;
#endif
    cm_param.handle_event = dm_client_event_handle;

    res = iotx_cm_open(&cm_param);

    if (res < SUCCESS_RETURN) {
        return res;
    }
    ctx->fd = res;

    dm_log_info("CM Fd: %d", ctx->fd);

    return SUCCESS_RETURN;
}

int dm_client_connect(int timeout_ms)
{
    int res = 0;
    dm_client_ctx_t *ctx = dm_client_get_ctx();

    res = iotx_cm_connect(ctx->fd, timeout_ms);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    return SUCCESS_RETURN;
}

int dm_client_close(void)
{
    dm_client_ctx_t *ctx = dm_client_get_ctx();

    return iotx_cm_close(ctx->fd);
}

int dm_client_subscribe(char *uri, iotx_cm_data_handle_cb callback, void *context)
{
    int res = 0;
    uint8_t local_sub = 0;
    dm_client_ctx_t *ctx = dm_client_get_ctx();
    iotx_cm_ext_params_t sub_params;

    memset(&sub_params, 0, sizeof(iotx_cm_ext_params_t));
    if (context != NULL) {
        local_sub = *((uint8_t *)context);
    }

    if (local_sub == 1) {
        sub_params.ack_type = IOTX_CM_MESSAGE_SUB_LOCAL;
        sub_params.sync_mode = IOTX_CM_ASYNC;
    } else {
        sub_params.ack_type = IOTX_CM_MESSAGE_NO_ACK;
        sub_params.sync_mode = IOTX_CM_SYNC;
    }

    sub_params.sync_timeout = IOTX_DM_CLIENT_SUB_TIMEOUT_MS;
    sub_params.ack_cb = NULL;

    res = iotx_cm_sub(ctx->fd, &sub_params, (const char *)uri, callback, NULL);
    dm_log_info("Subscribe Result: %d", res);

    if (res < SUCCESS_RETURN) {
        return res;
    }

    return SUCCESS_RETURN;
}

int dm_client_unsubscribe(char *uri)
{
    int res = 0;
    dm_client_ctx_t *ctx = dm_client_get_ctx();

    res = iotx_cm_unsub(ctx->fd, uri);

    dm_log_info("Unsubscribe Result: %d", res);

    return res;
}

int dm_client_publish(char *uri, unsigned char *payload, int payload_len, iotx_cm_data_handle_cb callback)
{
    int res = 0;
    char *pub_uri = NULL;
    dm_client_ctx_t *ctx = dm_client_get_ctx();
    iotx_cm_ext_params_t pub_param;

    memset(&pub_param, 0, sizeof(iotx_cm_ext_params_t));
    pub_param.ack_type = IOTX_CM_MESSAGE_NO_ACK;
    pub_param.sync_mode = IOTX_CM_ASYNC;
    pub_param.sync_timeout = 0;
    pub_param.ack_cb = NULL;

#if defined(COAP_COMM_ENABLED) && !defined(MQTT_COMM_ENABLED)
    pub_param.ack_cb = callback;
    res = dm_utils_uri_add_prefix("/topic", uri, &pub_uri);
    if (res < SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
#else
    pub_uri = uri;
#endif

    res = iotx_cm_pub(ctx->fd, &pub_param, (const char *)pub_uri, (const char *)payload, (unsigned int)payload_len);
    dm_log_info("Publish Result: %d", res);

#if defined(COAP_COMM_ENABLED) && !defined(MQTT_COMM_ENABLED)
    DM_free(pub_uri);
#endif

    return res;
}

int dm_client_yield(unsigned int timeout)
{
    dm_client_ctx_t *ctx = dm_client_get_ctx();

    return iotx_cm_yield(ctx->fd, timeout);
}
