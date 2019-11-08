#include "iotx_dm_internal.h"

#ifdef ALCS_ENABLED

#ifdef LOG_REPORT_TO_CLOUD
    #include "iotx_log_report.h"
#endif
static int _dm_server_malloc_context(_IN_ NetworkAddr *remote, _IN_ CoAPMessage *message,
                                     _OU_ dm_server_alcs_context_t **context)
{
    dm_server_alcs_context_t *alcs_context = NULL;

    alcs_context = DM_malloc(sizeof(dm_server_alcs_context_t));
    if (alcs_context == NULL) {
        return FAIL_RETURN;
    }
    memset(alcs_context, 0, sizeof(dm_server_alcs_context_t));

    alcs_context->ip = DM_malloc(strlen((char *)remote->addr) + 1);
    if (alcs_context->ip == NULL) {
        DM_free(alcs_context);
        return FAIL_RETURN;
    }
    memset(alcs_context->ip, 0, strlen((char *)remote->addr) + 1);
    memcpy(alcs_context->ip, (char *)remote->addr, strlen((char *)remote->addr) + 1);

    alcs_context->port = remote->port;
    dm_log_info("alcs_context->ip: %s", alcs_context->ip);
    dm_log_info("alcs_context->port: %d", alcs_context->port);

    alcs_context->token = DM_malloc(message->header.tokenlen);
    if (alcs_context->token == NULL) {
        DM_free(alcs_context->ip);
        DM_free(alcs_context);
        return FAIL_RETURN;
    }
    memset(alcs_context->token, 0, message->header.tokenlen);
    memcpy(alcs_context->token, message->token, message->header.tokenlen);

    alcs_context->token_len = message->header.tokenlen;

    *context = alcs_context;

    return SUCCESS_RETURN;
}

void dm_server_free_context(_IN_ void *ctx)
{
    dm_server_alcs_context_t *context = (dm_server_alcs_context_t *)ctx;
    DM_free(context->ip);
    DM_free(context->token);
    DM_free(context);
}

static dm_server_uri_map_t g_dm_server_uri_map[] = {
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
    {DM_URI_THING_SERVICE_PROPERTY_SET,       DM_URI_SYS_PREFIX,         IOTX_DM_LOCAL_AUTH,      dm_server_thing_service_property_set         },
    {DM_URI_THING_SERVICE_PROPERTY_GET,       DM_URI_SYS_PREFIX,         IOTX_DM_LOCAL_AUTH,      dm_server_thing_service_property_get         },
    {DM_URI_THING_EVENT_PROPERTY_POST,        DM_URI_SYS_PREFIX,         IOTX_DM_LOCAL_AUTH,      dm_server_thing_service_property_post        },
    {DM_URI_THING_SERVICE_REQUEST_WILDCARD2,  DM_URI_SYS_PREFIX,         IOTX_DM_LOCAL_AUTH,      dm_server_thing_service_request              },
#endif
    {DM_URI_DEV_CORE_SERVICE_DEV,            NULL,                      IOTX_DM_LOCAL_NO_AUTH,   dm_server_thing_dev_core_service_dev         },
};

int dm_server_subscribe_all(char product_key[IOTX_PRODUCT_KEY_LEN + 1], char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
    int res = 0, index = 0, auth = 0;
    int number = sizeof(g_dm_server_uri_map) / sizeof(dm_server_uri_map_t);
    char *uri = NULL;

    for (index = 0; index < number; index++) {
        res = dm_utils_service_name((char *)g_dm_server_uri_map[index].uri_prefix, (char *)g_dm_server_uri_map[index].uri_name,
                                    product_key, device_name, &uri);
        if (res < SUCCESS_RETURN) {
            index--;
            continue;
        }

        auth = (g_dm_server_uri_map[index].auth_type & IOTX_DM_SERVICE_LOCAL_AUTH) ? (IOTX_DM_MESSAGE_AUTH) :
               (IOTX_DM_MESSAGE_NO_AUTH);
        res = dm_server_subscribe(uri, (void *)g_dm_server_uri_map[index].callback, auth);
        if (res < SUCCESS_RETURN) {
            index--;
            DM_free(uri);
            continue;
        }
        DM_free(uri);
    }

    return SUCCESS_RETURN;
}

void dm_server_alcs_event_handler(void *pcontext, void *phandle, iotx_alcs_event_msg_t *msg)
{

}

#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
void dm_server_thing_service_property_set(CoAPContext *context, const char *paths, NetworkAddr *remote,
        CoAPMessage *message)
{
    int res = 0;
    dm_msg_source_t source;
    dm_msg_dest_t dest;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;
    dm_server_alcs_context_t *alcs_context = NULL;

    res = _dm_server_malloc_context(remote, message, &alcs_context);
    if (res != SUCCESS_RETURN) {
        return;
    }

    memset(&source, 0, sizeof(dm_msg_source_t));
    memset(&dest, 0, sizeof(dm_msg_dest_t));
    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    source.uri = paths;
    source.payload = (unsigned char *)message->payload;
    source.payload_len = message->payloadlen;
    source.context = alcs_context;

    dest.uri_name = DM_URI_THING_SERVICE_PROPERTY_SET;

    res = dm_msg_proc_thing_service_property_set(&source, &dest, &request, &response);
    if (res < SUCCESS_RETURN) {
        dm_server_free_context(alcs_context);
        return;
    }

#ifdef LOG_REPORT_TO_CLOUD
    {
        extern void send_permance_info(char *input, int input_len, char *comments, int report_format);
        if (SUCCESS_RETURN == check_target_msg(request.id.value, request.id.value_length)) {
            send_permance_info(request.id.value, request.id.value_length, "2", 1);
        }
    }
#endif

    dm_msg_response(DM_MSG_DEST_LOCAL, &request, &response, "{}", strlen("{}"), (void *)alcs_context);
    dm_server_free_context(alcs_context);
}

void dm_server_thing_service_request(CoAPContext *context, const char *paths, NetworkAddr *remote,
                                     CoAPMessage *message)
{
    int res = 0;
    dm_msg_source_t source;

    dm_server_alcs_context_t *alcs_context = NULL;

    res = _dm_server_malloc_context(remote, message, &alcs_context);
    if (res != SUCCESS_RETURN) {
        return;
    }

    memset(&source, 0, sizeof(dm_msg_source_t));

    source.uri = paths;
    source.payload = (unsigned char *)message->payload;
    source.payload_len = message->payloadlen;
    source.context = alcs_context;

    if (dm_msg_proc_thing_service_request(&source) < 0) {
        dm_server_free_context(alcs_context);
    }
}

void dm_server_thing_service_property_get(CoAPContext *context, const char *paths, NetworkAddr *remote,
        CoAPMessage *message)
{
    int res = 0;
    dm_server_alcs_context_t *alcs_context = NULL;
    dm_msg_source_t source;
    dm_msg_dest_t dest;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;
    unsigned char *data = NULL;
    int data_len = 0;

    res = _dm_server_malloc_context(remote, message, &alcs_context);
    if (res != SUCCESS_RETURN) {
        return;
    }

    memset(&source, 0, sizeof(dm_msg_source_t));
    memset(&dest, 0, sizeof(dm_msg_dest_t));
    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    source.uri = paths;
    source.payload = (unsigned char *)message->payload;
    source.payload_len = message->payloadlen;
    source.context = alcs_context;

    dest.uri_name = DM_URI_THING_SERVICE_PROPERTY_GET;

    dm_msg_proc_thing_service_property_get(&source, &dest, &request, &response, &data, &data_len);

#ifdef DEPRECATED_LINKKIT
    dm_msg_response(DM_MSG_DEST_LOCAL, &request, &response, (char *)data, data_len, alcs_context);
    DM_free(data);
    dm_server_free_context(alcs_context);
#endif
}

void dm_server_thing_service_property_post(CoAPContext *context, const char *paths, NetworkAddr *remote,
        CoAPMessage *message)
{
    int res = 0;
    dm_server_alcs_context_t *alcs_context = NULL;
    dm_msg_source_t source;
    dm_msg_dest_t dest;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;

    res = _dm_server_malloc_context(remote, message, &alcs_context);
    if (res != SUCCESS_RETURN) {
        return;
    }

    memset(&source, 0, sizeof(dm_msg_source_t));
    memset(&dest, 0, sizeof(dm_msg_dest_t));
    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    source.uri = paths;
    source.payload = (unsigned char *)message->payload;
    source.payload_len = message->payloadlen;
    source.context = alcs_context;

    dest.uri_name = DM_URI_THING_EVENT_PROPERTY_POST;

    dm_msg_proc_thing_service_property_post(&source, &dest, &request, &response);

    dm_msg_response(DM_MSG_DEST_LOCAL, &request, &response, "{}", strlen("{}"), alcs_context);
    dm_server_free_context(alcs_context);
}

#endif
void dm_server_thing_dev_core_service_dev(CoAPContext *context, const char *paths, NetworkAddr *remote,
        CoAPMessage *message)
{
    int res = 0;
    dm_server_alcs_context_t *alcs_context = NULL;
    dm_msg_source_t source;
    dm_msg_dest_t dest;
    dm_msg_request_payload_t request;
    dm_msg_response_t response;
    unsigned char *data = NULL;
    int data_len = 0;

    res = _dm_server_malloc_context(remote, message, &alcs_context);
    if (res != SUCCESS_RETURN) {
        return;
    }

    memset(&source, 0, sizeof(dm_msg_source_t));
    memset(&dest, 0, sizeof(dm_msg_dest_t));
    memset(&request, 0, sizeof(dm_msg_request_payload_t));
    memset(&response, 0, sizeof(dm_msg_response_t));

    source.uri = paths;
    source.payload = (unsigned char *)message->payload;
    source.payload_len = message->payloadlen;
    source.context = alcs_context;

    dest.uri_name = DM_URI_DEV_CORE_SERVICE_DEV;

    res = dm_msg_proc_thing_dev_core_service_dev(&source, &dest, &request, &response, &data, &data_len);
    if (res < SUCCESS_RETURN) {
        dm_server_free_context(alcs_context);
        return;
    }

    dm_msg_response(DM_MSG_DEST_LOCAL, &request, &response, (char *)data, data_len, alcs_context);

    if (response.code == IOTX_DM_ERR_CODE_SUCCESS) {
        DM_free(data);
    }
    dm_server_free_context(alcs_context);
}
#endif
