#ifndef _DM_SERVER_H_
#define _DM_SERVER_H_

#ifdef ALCS_ENABLED
typedef struct {
    const char *uri_name;
    const char *uri_prefix;
    int auth_type;
    CoAPRecvMsgHandler callback;
} dm_server_uri_map_t;

#define DM_SERVER_ALCS_NO_AUTH (0)
#define DM_SERVER_ALCS_AUTH    (1)

void dm_server_alcs_event_handler(void *pcontext, void *phandle, iotx_alcs_event_msg_t *msg);

int dm_server_subscribe_all(char product_key[IOTX_PRODUCT_KEY_LEN + 1], char device_name[IOTX_DEVICE_NAME_LEN + 1]);
void dm_server_thing_service_property_set(CoAPContext *context, const char *paths, NetworkAddr *remote,
        CoAPMessage *message);
void dm_server_thing_service_property_get(CoAPContext *context, const char *paths, NetworkAddr *remote,
        CoAPMessage *message);
void dm_server_thing_service_property_post(CoAPContext *context, const char *paths, NetworkAddr *remote,
        CoAPMessage *message);
void dm_server_thing_dev_core_service_dev(CoAPContext *context, const char *paths, NetworkAddr *remote,
        CoAPMessage *message);
void dm_server_thing_service_request(CoAPContext *context, const char *paths, NetworkAddr *remote,
                                     CoAPMessage *message);
#endif
#endif