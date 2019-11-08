#include "iotx_dm_internal.h"

#ifdef ALCS_ENABLED

static dm_server_ctx_t g_dm_server_ctx = {0};

static dm_server_ctx_t *dm_server_get_ctx(void)
{
    return &g_dm_server_ctx;
}

int dm_server_open(void)
{
    dm_server_ctx_t *ctx = dm_server_get_ctx();
    iotx_alcs_param_t alcs_param;
    iotx_alcs_event_handle_t event_handle;

    memset(&alcs_param, 0x0, sizeof(iotx_alcs_param_t));
    memset(&event_handle, 0x0, sizeof(iotx_alcs_event_handle_t));

    alcs_param.group = (char *)DM_SERVER_ALCS_ADDR;
    alcs_param.port = DM_SERVER_ALCS_PORT;
    alcs_param.send_maxcount = DM_SERVER_ALCS_SEND_MAXCOUNT;
    alcs_param.waittime = DM_SERVER_ALCS_WAITTIME;
    alcs_param.obs_maxcount = DM_SERVER_ALCS_OBS_MAXCOUNT;
    alcs_param.res_maxcount = DM_SERVER_ALCS_RES_MAXCOUNT;
    alcs_param.role = IOTX_ALCS_ROLE_CLIENT | IOTX_ALCS_ROLE_SERVER;
    event_handle.h_fp = dm_server_alcs_event_handler;
    event_handle.pcontext = NULL;

    alcs_param.handle_event = &event_handle;

    ctx->conn_handle  = iotx_alcs_construct(&alcs_param);
    if (ctx->conn_handle == NULL) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

int dm_server_connect(void)
{

    dm_server_ctx_t *ctx = dm_server_get_ctx();

    return iotx_alcs_cloud_init(ctx->conn_handle);
}

int dm_server_close(void)
{
    dm_server_ctx_t *ctx = dm_server_get_ctx();

    return iotx_alcs_destroy(&ctx->conn_handle);
}

int dm_server_send(char *uri, unsigned char *payload, int payload_len, void *context)
{
    int res = 0;
    dm_server_ctx_t *ctx = dm_server_get_ctx();
    iotx_alcs_msg_t alcs_msg;
    dm_server_alcs_context_t *alcs_context = (dm_server_alcs_context_t *)context;

    memset(&alcs_msg, 0, sizeof(iotx_alcs_msg_t));

    alcs_msg.group_id = 0;
    alcs_msg.ip = alcs_context ? alcs_context->ip : NULL;
    alcs_msg.port = alcs_context ? alcs_context->port : 0;
    alcs_msg.msg_code = (alcs_context && alcs_context->token_len
                         && alcs_context->token) ? ITOX_ALCS_COAP_MSG_CODE_205_CONTENT : ITOX_ALCS_COAP_MSG_CODE_GET;
    alcs_msg.msg_type = IOTX_ALCS_MESSAGE_TYPE_CON;
    alcs_msg.uri = uri;
    alcs_msg.payload = payload;
    alcs_msg.payload_len = payload_len;

    if (alcs_context == NULL) {
        res = iotx_alcs_observe_notify(ctx->conn_handle, alcs_msg.uri, alcs_msg.payload_len, alcs_msg.payload);
        dm_log_info("Send Observe Notify Result %d", res);
    } else if (alcs_context->ip && alcs_context->port && NULL == alcs_context->token) {
        res = iotx_alcs_send(ctx->conn_handle, &alcs_msg);
        dm_log_info("Send Result %d", res);
    } else if (alcs_context->ip && alcs_context->port && alcs_context->token_len && alcs_context->token) {
        res = iotx_alcs_send_Response(ctx->conn_handle, &alcs_msg, (uint8_t)alcs_context->token_len,
                                      (uint8_t *)alcs_context->token);
        dm_log_info("Send Response Result %d", res);
    }

    return res;
}

int dm_server_subscribe(char *uri, CoAPRecvMsgHandler callback, int auth_type)
{
    int res = 0;
    dm_server_ctx_t *ctx = dm_server_get_ctx();
    iotx_alcs_res_t alcs_res;

    memset(&alcs_res, 0, sizeof(iotx_alcs_res_t));

    alcs_res.uri = uri;
    alcs_res.msg_ct = IOTX_ALCS_MESSAGE_CT_APP_JSON;
    alcs_res.msg_perm = IOTX_ALCS_MESSAGE_PERM_GET;
    alcs_res.maxage = 60;
    alcs_res.need_auth = auth_type;
    alcs_res.callback = callback;

    res = iotx_alcs_register_resource(ctx->conn_handle, &alcs_res);

    dm_log_info("Register Resource Result: %d", res);

    return res;
}

int dm_server_add_device(char product_key[IOTX_PRODUCT_KEY_LEN + 1], char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int res = 0;
    dm_server_ctx_t *ctx = dm_server_get_ctx();

    res = iotx_alcs_add_sub_device(ctx->conn_handle, (const char *)product_key, (const char *)device_name);
    dm_log_info("Add Device Result: %d, Product Key: %s, Device Name: %s", res, product_key, device_name);

    return res;
}

int dm_server_del_device(char product_key[IOTX_PRODUCT_KEY_LEN + 1], char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int res = 0;
    dm_server_ctx_t *ctx = dm_server_get_ctx();

    res = iotx_alcs_remove_sub_device(ctx->conn_handle, (const char *)product_key, (const char *)device_name);
    dm_log_info("Del Device Result: %d, Product Key: %s, Device Name: %s", res, product_key, device_name);

    return res;
}

int dm_server_yield(void)
{
    dm_server_ctx_t *ctx = dm_server_get_ctx();

    return iotx_alcs_yield(ctx->conn_handle);
}
#endif
