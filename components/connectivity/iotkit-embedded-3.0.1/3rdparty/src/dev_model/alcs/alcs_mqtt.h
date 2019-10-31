/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _ALCS_MQTT_H_
#define _ALCS_MQTT_H_

#include "alcs_internal.h"

typedef enum {
    ALCS_MQTT_STATUS_SUCCESS,
    ALCS_MQTT_STATUS_ERROR
} alcs_mqtt_status_e;

typedef struct {
    CoAPContext *coap_ctx;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1];
    char device_name[IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 2];
    uint32_t send_id;
} alcs_mqtt_ctx_t;

#define ALCS_MQTT_PREFIX                                       "/sys/%s/%s"

#define ALCS_MQTT_THING_LAN_PREFIX_GET_REPLY_FMT               "/thing/lan/prefix/get_reply"
#define ALCS_MQTT_THING_LAN_PREFIX_GET_FMT                     "/thing/lan/prefix/get"
#define ALCS_MQTT_THING_LAN_PREFIX_UPDATE_FMT                  "/thing/lan/prefix/update"
#define ALCS_MQTT_THING_LAN_PREFIX_UPDATE_REPLY_FMT            "/thing/lan/prefix/update_reply"
#define ALCS_MQTT_THING_LAN_PREFIX_BLACKLIST_UPDATE_FMT        "/thing/lan/blacklist/update"
#define ALCS_MQTT_THING_LAN_PREFIX_BLACKLIST_UPDATE_REPLY_FMT  "/thing/lan/blacklist/update_reply"

#define ALCS_MQTT_THING_ALCS_REQUEST              "{\"id\":\"%d\",\"version\":\"1.0\",\"params\":\"{}\",\"method\":\"thing.lan.prefix.get\"}"
#define ALCS_MQTT_THING_LAN_PREFIX_RESPONSE_FMT   "{\"id\": \"%d\", \"code\": %d, \"data\": %s}"
#define ALCS_MQTT_THING_ALCS_SUBDEV_REQUEST       "{\"id\":\"%d\",\"version\":\"1.0\",\"params\":{\"productKey\":\"%.*s\",\"deviceName\":\"%.*s\"},\"method\":\"thing.lan.prefix.get\"}"

#define ALCS_MQTT_TOPIC_MAX_LEN (128)

#define ALCS_MQTT_JSON_KEY_PRODUCT_KEY "productKey"
#define ALCS_MQTT_JSON_KEY_DEVICE_NAME "deviceName"
#define ALCS_MQTT_JSON_KEY_PREFIX   "prefix"
#define ALCS_MQTT_JSON_KEY_SECRET   "deviceSecret"
#define ALCS_MQTT_JSON_KEY_BLACK    "blacklist"

#define ALCS_MQTT_PREFIX_MAX_LEN (40)
#define ALCS_MQTT_SECRET_MAX_LEN (40)
#define ALCS_MQTT_BLACK_MAX_LEN  (100)

alcs_mqtt_status_e alcs_mqtt_init(void *handle, char *product_key, char *device_name);
alcs_mqtt_status_e alcs_mqtt_deinit(void *handle, char *product_key, char *device_name);
alcs_mqtt_status_e alcs_mqtt_blacklist_update(void *ctx);
alcs_mqtt_status_e alcs_mqtt_prefixkey_update(void *ctx);
void alcs_mqtt_add_srv_key(const char *prefix, const char *secret);
alcs_mqtt_status_e alcs_mqtt_prefix_secret_load(const char *pk, uint16_t pk_len,
        const char *dn, uint16_t dn_len,
        char *prefix, char *secret);
alcs_mqtt_status_e alcs_mqtt_prefix_secret_del(const char *pk, uint16_t pk_len,
        const char *dn, uint16_t dn_len);
alcs_mqtt_status_e alcs_mqtt_subdev_prefix_get(const char *product_key, const char *device_name);
alcs_mqtt_status_e alcs_prefixkey_get(const char *product_key, const char *device_name);

#endif
