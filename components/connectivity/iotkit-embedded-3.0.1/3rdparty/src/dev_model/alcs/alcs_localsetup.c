/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "alcs_internal.h"
#include "alcs_api_internal.h"
#include "CoAPExport.h"
#include "alcs_api.h"
#include "alcs_mqtt.h"
#include "alcs_adapter.h"
#include "CoAPPlatform.h"
#include "CoAPExport.h"
#include "iotx_alcs.h"

char *DEFAULT_AC = "Xtau@iot";
char *DEFAULT_AS = "Yx3DdsyetbSezlvc";
void *g_adapter_handle = NULL;
void *g_coap_handle = NULL;

typedef enum {
    ALCS_LOCALSETUP_SUCCESS,
    ALCS_LOCALSETUP_ERROR
} localsetup_status;

static localsetup_status __alcs_localsetup_kv_set(const char *key, const void *val, int len, int sync)
{
    if (HAL_Kv_Set(key, val, len, sync) != 0) {
        return ALCS_LOCALSETUP_ERROR;
    }

    COAP_INFO("ALCS KV Set, Key: %s, Val: %s, Len: %d", key, (char *)val, len);
    return ALCS_LOCALSETUP_SUCCESS;
}

static localsetup_status __alcs_localsetup_kv_get(const char *key, void *buffer, int *buffer_len)
{
    int     rc = -1;

    if ((rc = HAL_Kv_Get(key, buffer, buffer_len)) != 0) {
        COAP_WRN("HAL_Kv_Get('%s') = %d (!= 0), return %d", key, rc, ALCS_LOCALSETUP_ERROR);
        return ALCS_LOCALSETUP_ERROR;
    }

    COAP_INFO("ALCS KV Get, Key: %s", key);

    return ALCS_LOCALSETUP_SUCCESS;
}

static localsetup_status __alcs_localsetup_kv_del(const char *key)
{
    if (HAL_Kv_Del(key) != 0) {
        return ALCS_LOCALSETUP_ERROR;
    }

    COAP_INFO("ALCS KV Del, Key: %s", key);

    return ALCS_LOCALSETUP_SUCCESS;
}

static localsetup_status __fill_key(const char *pk, uint16_t pk_len,
                                    const char *dn, uint16_t dn_len, char key_md5_hexstr[33])
{
    uint8_t key_md5[16] = {0};
    char key_source[IOTX_PRODUCT_KEY_LEN + 1 + IOTX_DEVICE_NAME_LEN + 1 + 3];

    if (pk == NULL || pk_len >= IOTX_PRODUCT_KEY_LEN + 1 ||
        dn == NULL || dn_len >= IOTX_DEVICE_NAME_LEN + 1) {
        COAP_ERR("Invalid Parameter");
        return ALCS_LOCALSETUP_ERROR;
    }

    /* Calculate Key */
    HAL_Snprintf(key_source, sizeof(key_source), "%.*s%.*s.l", pk_len, pk, dn_len, dn);

    utils_md5((const unsigned char *)key_source, strlen(key_source), key_md5);
    alcs_utils_md5_hexstr(key_md5, (unsigned char *)key_md5_hexstr);
    return ALCS_LOCALSETUP_SUCCESS;
}

static localsetup_status __alcs_localsetup_ac_as_save(const char *pk, uint16_t pk_len,
        const char *dn, uint16_t dn_len,
        const char *prefix, uint16_t prefix_len,
        const char *secret, uint16_t secret_len)
{
    char key_md5_hexstr[33] = {0};
    char *value = NULL;
    int rt;
    if (prefix == NULL || secret == NULL) {
        COAP_ERR("Invalid Parameter");
        return ALCS_LOCALSETUP_ERROR;
    }

    rt = __fill_key(pk, pk_len, dn, dn_len, key_md5_hexstr);
    if (rt != ALCS_LOCALSETUP_SUCCESS) {
        return rt;
    }

    /* Calculate Value */
    value = ALCS_ADAPTER_malloc(prefix_len + secret_len + 3);
    if (value == NULL) {
        COAP_ERR("No Enough Memory");
        return ALCS_LOCALSETUP_ERROR;
    }
    memset(value, 0, prefix_len + secret_len + 3);

    value[0] = prefix_len;
    value[1] = secret_len;
    HAL_Snprintf(&value[2], prefix_len + secret_len + 1, "%.*s%.*s", prefix_len, prefix, secret_len, secret);

    if (ALCS_LOCALSETUP_SUCCESS != __alcs_localsetup_kv_set(key_md5_hexstr, value, prefix_len + secret_len + 3, 1)) {
        COAP_WRN("ALCS KV Set Prefix And Secret Fail");
        ALCS_free(value);
        return ALCS_LOCALSETUP_ERROR;
    }

    ALCS_free(value);
    return ALCS_LOCALSETUP_SUCCESS;
}

localsetup_status alcs_localsetup_ac_as_load(const char *pk, uint16_t pk_len,
        const char *dn, uint16_t dn_len,
        char *prefix, int prefix_len, char *secret, int secret_len)
{
    char key_md5_hexstr[33] = {0};
    char value[128] = {0};
    int value_len = sizeof(value);

    int rt;
    if (prefix == NULL || secret == NULL) {
        COAP_ERR("Invalid Parameter");
        return ALCS_LOCALSETUP_ERROR;
    }

    rt = __fill_key(pk, pk_len, dn, dn_len, key_md5_hexstr);
    if (rt != ALCS_LOCALSETUP_SUCCESS) {
        return rt;
    }

    /* Get Value */
    if (ALCS_LOCALSETUP_SUCCESS != __alcs_localsetup_kv_get(key_md5_hexstr, value, &value_len)) {
        COAP_WRN("ALCS KV Get local Prefix And Secret Fail");
        return ALCS_LOCALSETUP_ERROR;
    }

    if (value[0] >= prefix_len || value[1] >= secret_len) {
        COAP_ERR("insuffient buffer!");
        return ALCS_LOCALSETUP_ERROR;
    }

    memset(prefix, 0, prefix_len);
    memcpy(prefix, &value[2], value[0]);
    memset(secret, 0, secret_len);
    memcpy(secret, &value[2 + value[0]], value[1]);
    return ALCS_LOCALSETUP_SUCCESS;
}

static localsetup_status __alcs_localsetup_ac_as_del(const char *pk, uint16_t pk_len,
        const char *dn, uint16_t dn_len)
{
    char key_md5_hexstr[33] = {0};
    int rt;
    rt = __fill_key(pk, pk_len, dn, dn_len, key_md5_hexstr);
    if (rt != ALCS_LOCALSETUP_SUCCESS) {
        return rt;
    }

    if (ALCS_LOCALSETUP_SUCCESS != __alcs_localsetup_kv_del(key_md5_hexstr)) {
        COAP_ERR("ALCS KV Get local Prefix And Secret Fail");
        return ALCS_LOCALSETUP_ERROR;
    }

    return ALCS_LOCALSETUP_SUCCESS;
}

static void alcs_service_cb_setup(CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *message)
{
    char payload[128];
    char *id = NULL, *p;
    int idlen = 0, len, aclen, aslen, pklen, dnlen;
    char *ac = NULL, *as = NULL, *pk = NULL, *dn = dn;
    bool success = 0;
    char *err_msg = NULL;
    char configValueBack, acBack, asBack;
    char *str_pos, *entry;
    int entry_len, type;
    iotx_alcs_msg_t rsp_msg;

    COAP_DEBUG("alcs_service_cb_setup, path:%s", paths);
    do {
        if (!remote || !message) {
            COAP_DEBUG("alcs_service_cb_setup, param is NULL!");
            err_msg = "invalid package";
            break;
        }

        id = json_get_value_by_name((char *)message->payload, message->payloadlen, "id", &idlen, (int *)NULL);
        p = json_get_value_by_name((char *)message->payload, message->payloadlen, "params", &len, (int *)NULL);
        if (!p || !len) {
            err_msg = "params is not found";
            break;
        }

        p = json_get_value_by_name(p, len, "configValue", &len, (int *)NULL);
        if (!p || !len) {
            err_msg = "configValue is not found";
            break;
        }

        backup_json_str_last_char(p, len, configValueBack);

        json_array_for_each_entry(p, len, str_pos, entry, entry_len, type) {
            COAP_DEBUG("entry:%.*s", entry_len, entry);
            ac = json_get_value_by_name(entry, entry_len, "authCode", &aclen, (int *)NULL);
            as = json_get_value_by_name(entry, entry_len, "authSecret", &aslen, (int *)NULL);
            pk = json_get_value_by_name(entry, entry_len, "productKey", &pklen, (int *)NULL);
            dn = json_get_value_by_name(entry, entry_len, "deviceName", &dnlen, (int *)NULL);
            break;
        } /* end json_array_for_each_entry */
        restore_json_str_last_char(p, len, configValueBack);

        if (!ac || !aclen || !as || !aslen || !pk || !pklen || !dn || !dnlen) {
            err_msg = "authinfo is not found";
            break;
        }

        /* save */
        backup_json_str_last_char(ac, aclen, acBack);
        backup_json_str_last_char(as, aslen, asBack);
        __alcs_localsetup_ac_as_del(pk, pklen, dn, dnlen);
        __alcs_localsetup_ac_as_save(pk, pklen, dn, dnlen, ac, aclen, as, aslen);

        alcs_add_svr_key(g_coap_handle, ac, as, LOCALSETUP);

        restore_json_str_last_char(ac, aclen, acBack);
        restore_json_str_last_char(as, aslen, asBack)
        success = 1;

    } while (0);

    if (success) {
        HAL_Snprintf(payload, sizeof(payload), "{\"id\":\"%.*s\",\"code\":200}", idlen, id ? id : "");
    } else {
        HAL_Snprintf(payload, sizeof(payload), "{\"id\":\"%.*s\",\"code\":400,\"msg\":\"%s\"}", idlen, id ? id : "", err_msg);
        COAP_ERR("alcs_service_cb_setup, %s", err_msg);
    }

    memset(&rsp_msg, 0, sizeof(iotx_alcs_msg_t));

    rsp_msg.msg_code = ITOX_ALCS_COAP_MSG_CODE_205_CONTENT;
    rsp_msg.msg_type = IOTX_ALCS_MESSAGE_TYPE_CON;
    rsp_msg.payload = (unsigned char *)payload;
    rsp_msg.payload_len = strlen(payload);
    rsp_msg.ip = (char *)(remote ? remote->addr : NULL);
    rsp_msg.port = remote ? remote->port : 5683;
    rsp_msg.uri = (char *)paths;

    if (message) {
        iotx_alcs_send_Response(g_adapter_handle, &rsp_msg, message->header.tokenlen, message->token);
    }
}

static void alcs_localsetup_register_resource(void *adapter_handle, char *pk, char *dn)
{
    iotx_alcs_res_t alcs_res;
    char uri [IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 24];

    if (adapter_handle == NULL || pk == NULL || strlen(pk) > IOTX_PRODUCT_KEY_LEN ||
        dn == NULL || strlen(dn) > IOTX_DEVICE_NAME_LEN) {
        return;
    }

    HAL_Snprintf(uri, sizeof(uri), "/dev/%s/%s/core/service/setup", pk, dn);

    memset(&alcs_res, 0, sizeof(iotx_alcs_res_t));
    alcs_res.uri = uri;
    alcs_res.msg_ct = IOTX_ALCS_MESSAGE_CT_APP_JSON;
    alcs_res.msg_perm = IOTX_ALCS_MESSAGE_PERM_GET | IOTX_ALCS_MESSAGE_PERM_PUT;
    alcs_res.maxage = 60;
    alcs_res.need_auth = 1;
    alcs_res.callback = alcs_service_cb_setup;

    iotx_alcs_register_resource(adapter_handle, &alcs_res);
}

void alcs_localsetup_init(void *adapter_handle, void *coap_handler, char *pk, char *dn)
{
    char prefix [10];
    char secret [64];
    g_adapter_handle = adapter_handle;
    g_coap_handle = coap_handler;
    alcs_localsetup_register_resource(adapter_handle, pk, dn);

    if (alcs_localsetup_ac_as_load(pk, strlen(pk), dn, strlen(dn), prefix, sizeof(prefix), secret,
                                   sizeof(secret)) != ALCS_LOCALSETUP_SUCCESS) {
        alcs_add_svr_key(g_coap_handle, DEFAULT_AC, DEFAULT_AS, LOCALDEFAULT);
    } else {
        alcs_add_svr_key(g_coap_handle, prefix, secret, LOCALSETUP);
    }
}

void alcs_localsetup_add_sub_device(void *adapter_handle, char *pk, char *dn)
{
    alcs_localsetup_register_resource(adapter_handle, pk, dn);
}
