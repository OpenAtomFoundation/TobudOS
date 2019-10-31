/*
* Copyright (C) 2015-2018 Alibaba Group Holding Limited
*/

#include <stdio.h>

#include "coap_api.h"
#include "iotx_coap_internal.h"
#include "Cloud_CoAPPlatform.h"
#include "Cloud_CoAPPlatform.h"
#include "Cloud_CoAPMessage.h"
#include "Cloud_CoAPExport.h"

#define IOTX_SIGN_LENGTH         (40+1)
#define IOTX_SIGN_SOURCE_LEN     (256)
#define IOTX_AUTH_TOKEN_LEN      (192+1)
#define IOTX_COAP_INIT_TOKEN     (0x01020304)
#define IOTX_LIST_MAX_ITEM       (10)

#ifndef INFRA_LOG
    #undef HEXDUMP_DEBUG
    #undef HEXDUMP_INFO

    #define HEXDUMP_DEBUG(...)
    #define HEXDUMP_INFO(...)
#endif

#define IOTX_AUTH_STR      "auth"
#define IOTX_SIGN_SRC_STR  "clientId%sdeviceName%sproductKey%s"
#define IOTX_SIGN_SRC_STR_WITH_SEQ  "clientId%sdeviceName%sproductKey%sseq%d"

#define IOTX_AUTH_DEVICENAME_STR "{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"clientId\":\"%s\",\"sign\":\"%s\"}"
#define IOTX_AUTH_DEVICENAME_STR_WITH_SEQ "{\"productKey\":\"%s\",\"deviceName\":\"%s\",\"clientId\":\"%s\",\"sign\":\"%s\",\"seq\":\"%d\"}"

#define IOTX_COAP_ONLINE_DTLS_SERVER_URL "coaps://%s.coap.cn-shanghai.link.aliyuncs.com:5684"
#define IOTX_COAP_ONLINE_PSK_SERVER_URL "coap-psk://%s.coap.cn-shanghai.link.aliyuncs.com:5682"

iotx_coap_context_t *g_coap_context = NULL;

typedef struct {
    char                *p_auth_token;
    int                  auth_token_len;
    char                 is_authed;
    iotx_deviceinfo_t   *p_devinfo;
    Cloud_CoAPContext         *p_coap_ctx;
    unsigned int         coap_token;
    unsigned int         seq;
    unsigned char        key[32];
    iotx_event_handle_t  event_handle;
} iotx_coap_t;


int iotx_calc_sign(const char *p_device_secret, const char *p_client_id,
                   const char *p_device_name, const char *p_product_key, char sign[IOTX_SIGN_LENGTH])
{
    char *p_msg = NULL;

    p_msg = (char *)coap_malloc(IOTX_SIGN_SOURCE_LEN);
    if (NULL == p_msg) {
        return IOTX_ERR_NO_MEM;
    }
    memset(sign,  0x00, IOTX_SIGN_LENGTH);
    memset(p_msg, 0x00, IOTX_SIGN_SOURCE_LEN);

    HAL_Snprintf(p_msg, IOTX_SIGN_SOURCE_LEN,
                 IOTX_SIGN_SRC_STR,
                 p_client_id,
                 p_device_name,
                 p_product_key);
    utils_hmac_md5(p_msg, strlen(p_msg), sign, p_device_secret, strlen(p_device_secret));

    coap_free(p_msg);
    COAP_DEBUG("The device name sign: %s", sign);
    return IOTX_SUCCESS;
}

int iotx_calc_sign_with_seq(const char *p_device_secret, const char *p_client_id,
                            const char *p_device_name, const char *p_product_key, unsigned int seq, char sign[IOTX_SIGN_LENGTH])
{
    char *p_msg = NULL;

    p_msg = (char *)coap_malloc(IOTX_SIGN_SOURCE_LEN);
    if (NULL == p_msg) {
        return IOTX_ERR_NO_MEM;
    }
    memset(sign,  0x00, IOTX_SIGN_LENGTH);
    memset(p_msg, 0x00, IOTX_SIGN_SOURCE_LEN);

    HAL_Snprintf(p_msg, IOTX_SIGN_SOURCE_LEN,
                 IOTX_SIGN_SRC_STR_WITH_SEQ,
                 p_client_id,
                 p_device_name,
                 p_product_key, seq);
    COAP_DEBUG("The source string: %s", p_msg);
    utils_hmac_md5(p_msg, strlen(p_msg), sign, p_device_secret, strlen(p_device_secret));

    coap_free(p_msg);
    COAP_DEBUG("The device name sign with seq: %s", sign);
    return IOTX_SUCCESS;
}


static int iotx_get_token_from_json(char *p_str, char *p_token, int len)
{
    char *p_value = NULL;
    if (NULL == p_str || NULL == p_token) {
        COAP_ERR("Invalid paramter p_str %p, p_token %p", p_str, p_token);
        return IOTX_ERR_INVALID_PARAM;
    }

    p_value = LITE_json_value_of("token", p_str, 0x1234, "coap.cloud");
    if (NULL != p_value) {
        if (len - 1 < strlen(p_value)) {
            return IOTX_ERR_BUFF_TOO_SHORT;
        }
        memset(p_token, 0x00, len);
        strncpy(p_token, p_value, strlen(p_value));
#ifdef INFRA_MEM_STATS
        LITE_free(p_value);
#else
        HAL_Free((void *)p_value);
#endif
        return IOTX_SUCCESS;
    }

    return IOTX_ERR_AUTH_FAILED;
}

static int iotx_parse_auth_from_json(char *p_str, iotx_coap_t *p_iotx_coap)
{
    int ret = -1;
    lite_cjson_t root;
    lite_cjson_t node;
    unsigned char key[32] = {0};
    unsigned char buff[128] = {0};
    unsigned char random[32 + 1]   = {0};

    if (NULL == p_str || NULL == p_iotx_coap) {
        return IOTX_ERR_INVALID_PARAM;
    }

    memset(&root, 0x00, sizeof(lite_cjson_t));
    memset(&node, 0x00, sizeof(lite_cjson_t));
    ret = lite_cjson_parse(p_str, strlen(p_str), &root);
    if (-1 == ret) {
        return IOTX_ERR_AUTH_FAILED;
    }

    ret = lite_cjson_object_item(&root, "token", strlen("token"), &node);
    if (-1 == ret) {
        return IOTX_ERR_AUTH_FAILED;
    }
    if (p_iotx_coap->auth_token_len - 1 < node.value_length) {
        return IOTX_ERR_BUFF_TOO_SHORT;
    }
    memset(p_iotx_coap->p_auth_token, 0x00, node.value_length);
    strncpy(p_iotx_coap->p_auth_token, node.value, node.value_length);

    memset(&node, 0x00, sizeof(lite_cjson_t));
    ret = lite_cjson_object_item(&root, "seqOffset", strlen("seqOffset"), &node);
    if (-1 == ret) {
        return IOTX_ERR_AUTH_FAILED;
    }
    p_iotx_coap->seq = node.value_int;

    memset(&node, 0x00, sizeof(lite_cjson_t));
    ret = lite_cjson_object_item(&root, "random", strlen("random"), &node);
    if (-1 == ret) {
        return IOTX_ERR_AUTH_FAILED;
    }
    if(node.value_length > 32) {
        return IOTX_ERR_BUFF_TOO_SHORT;
    }
    memcpy(random, node.value, node.value_length);
    HAL_Snprintf((char *)buff, sizeof(buff), "%s,%s",
                 p_iotx_coap->p_devinfo->device_secret,  random);
    COAP_DEBUG("The src:%s", buff);
    utils_sha256(buff,  strlen((char *)buff), key);
    memcpy(p_iotx_coap->key, key + 8, 16);
    COAP_DEBUG("The key is:");
    HEXDUMP_DEBUG(key, 32);
    COAP_DEBUG("The short key:");
    HEXDUMP_DEBUG(p_iotx_coap->key, 16);

    return IOTX_SUCCESS;
}

static void iotx_device_name_auth_callback(void *user, void *p_message)
{
    int ret_code = IOTX_SUCCESS;
    iotx_coap_t *p_iotx_coap = NULL;
    Cloud_CoAPMessage *message = (Cloud_CoAPMessage *)p_message;

    if (NULL == user) {
        COAP_ERR("Invalid paramter, p_arg %p", user);
        return ;
    }
    p_iotx_coap = (iotx_coap_t *)user;

    if (NULL == message) {
        COAP_ERR("Invalid paramter, message %p",  message);
        return;
    }
    COAP_DEBUG("Receive response message:");
    COAP_DEBUG("* Response Code : 0x%x", message->header.code);
    COAP_DEBUG("* Payload: %s", message->payload);

    switch (message->header.code) {
        case COAP_MSG_CODE_205_CONTENT: {
            if (COAP_ENDPOINT_PSK == p_iotx_coap->p_coap_ctx->network.ep_type) {
                ret_code = iotx_parse_auth_from_json((char *)message->payload, p_iotx_coap);
            } else {
                ret_code = iotx_get_token_from_json((char *)message->payload, p_iotx_coap->p_auth_token, p_iotx_coap->auth_token_len);
            }

            if (IOTX_SUCCESS == ret_code) {
                p_iotx_coap->is_authed = IOT_TRUE;
                COAP_INFO("CoAP authenticate success!!!");
            }
            break;
        }
        case COAP_MSG_CODE_500_INTERNAL_SERVER_ERROR: {
            COAP_INFO("CoAP internal server error, authenticate failed, will retry it");
            HAL_SleepMs(1000);
            IOT_CoAP_DeviceNameAuth((iotx_coap_context_t *)p_iotx_coap);
            break;
        }
        default:
            break;
    }

}

static unsigned int iotx_get_coap_token(iotx_coap_t       *p_iotx_coap, unsigned char *p_encoded_data)
{
    unsigned int value = p_iotx_coap->coap_token;
    p_encoded_data[0] = (unsigned char)((value & 0x00FF) >> 0);
    p_encoded_data[1] = (unsigned char)((value & 0xFF00) >> 8);
    p_encoded_data[2] = (unsigned char)((value & 0xFF0000) >> 16);
    p_encoded_data[3] = (unsigned char)((value & 0xFF000000) >> 24);
    p_iotx_coap->coap_token++;
    return sizeof(unsigned int);
}

void iotx_event_notifyer(unsigned int code, Cloud_CoAPMessage *message)
{
    if (NULL == message) {
        COAP_ERR("Invalid paramter, message %p", message);
        return ;
    }

    COAP_DEBUG("Error code: 0x%x, payload: %s", code, message->payload);
    switch (code) {
        case COAP_MSG_CODE_402_BAD_OPTION:
        case COAP_MSG_CODE_401_UNAUTHORIZED: {
            iotx_coap_t *p_context = NULL;
            if (NULL != message->user) {
                p_context = (iotx_coap_t *)message->user;
                p_context->is_authed = IOT_FALSE;
                IOT_CoAP_DeviceNameAuth(p_context);
                COAP_INFO("IoTx token expired, will reauthenticate");
            }
            /* TODO: call event handle to notify application */
            /* p_context->event_handle(); */
            break;
        }

        default:
            break;
    }
}

static void iotx_get_well_known_handler(void *arg, void *p_response)
{

    int            len       = 0;
    unsigned char *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;
    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
    COAP_INFO("[APPL]: Message response code: %d", resp_code);
    COAP_INFO("[APPL]: Len: %d, Payload: %s, ", len, p_payload);
}


int iotx_get_well_known(iotx_coap_context_t *p_context)
{
    int len = 0;
    Cloud_CoAPContext      *p_coap_ctx = NULL;
    iotx_coap_t      *p_iotx_coap = NULL;
    Cloud_CoAPMessage      message;
    unsigned char    token[8] = {0};

    p_iotx_coap = (iotx_coap_t *)p_context;
    p_coap_ctx = (Cloud_CoAPContext *)p_iotx_coap->p_coap_ctx;


    CoAPMessage_init(&message);
    CoAPMessageType_set(&message, COAP_MESSAGE_TYPE_CON);
    CoAPMessageCode_set(&message, COAP_MSG_CODE_GET);
    CoAPMessageId_set(&message, Cloud_CoAPMessageId_gen(p_coap_ctx));
    len = iotx_get_coap_token(p_iotx_coap, token);
    CoAPMessageToken_set(&message, token, len);
    Cloud_CoAPMessageHandler_set(&message, iotx_get_well_known_handler);
    CoAPStrOption_add(&message, COAP_OPTION_URI_PATH, (unsigned char *)".well-known", strlen(".well-known"));
    CoAPStrOption_add(&message, COAP_OPTION_URI_PATH, (unsigned char *)"core", strlen("core"));
    CoAPUintOption_add(&message, COAP_OPTION_ACCEPT, COAP_CT_APP_LINK_FORMAT);
    CoAPMessageUserData_set(&message, (void *)p_iotx_coap);
    Cloud_CoAPMessage_send(p_coap_ctx, &message);
    CoAPMessage_destory(&message);
    return IOTX_SUCCESS;
}

static void iotx_coap_report_rsphdl(void *arg, void *p_response)
{
    int                     p_payload_len = 0;
    unsigned char          *p_payload = NULL;
    iotx_coap_resp_code_t   resp_code;

    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    IOT_CoAP_GetMessagePayload(p_response, &p_payload, &p_payload_len);
    COAP_DEBUG("Report response: CoAP response code = %d", resp_code);
    COAP_DEBUG("Report response: CoAP msg_len = %d", p_payload_len);
    if (p_payload_len > 0) {
        COAP_DEBUG("Report response: CoAP msg = '%.*s'", p_payload_len, p_payload);
    } else {
        COAP_WRN("Report response: CoAP response payload_len = 0");
    }
}

static int coap_report_func(void *handle, const char *topic_name, int req_ack, void *data, int len)
{
    iotx_message_t          message;
    char coap_topic[100] = {0};
    (void)req_ack;

    memset(&message, 0, sizeof(iotx_message_t));
    message.p_payload = (unsigned char *)data;
    message.payload_len = len;
    message.resp_callback = iotx_coap_report_rsphdl;
    message.msg_type = IOTX_MESSAGE_NON;
    message.content_type = IOTX_CONTENT_TYPE_JSON;
    HAL_Snprintf(coap_topic, 100, "/topic%s", topic_name);
    return IOT_CoAP_SendMessage(handle, (char *)coap_topic, &message);
}

int iotx_aes_cbc_encrypt(const unsigned char *src, int len, const unsigned char *key, void *out)
{
    char *iv = "543yhjy97ae7fyfg";

    int len1 = len & 0xfffffff0;
    int len2 = len1 + 16;
    int pad = len2 - len;
    int ret = 0;

    p_HAL_Aes128_t aes_e_h = HAL_Aes128_Init((unsigned char *)key, (unsigned char *)iv, HAL_AES_ENCRYPTION);
    if (len1) {
        ret = HAL_Aes128_Cbc_Encrypt(aes_e_h, src, len1 >> 4, out);
    }
    if (!ret && pad) {
        char buf[16] = {0};
        memcpy(buf, src + len1, len - len1);
        memset(buf + len - len1, pad, pad);
        ret = HAL_Aes128_Cbc_Encrypt(aes_e_h, buf, 1, (unsigned char *)out + len1);

    }

    HAL_Aes128_Destroy(aes_e_h);

    COAP_DEBUG("to encrypt src: %s, len: %d", src, len2);
    return ret == 0 ? len2 : 0;
}

int iotx_aes_cbc_decrypt(const unsigned char *src, int len, const unsigned char *key, void *out)
{
    char *iv = "543yhjy97ae7fyfg";

    p_HAL_Aes128_t aes_d_h;
    int ret = 0;
    int n = len >> 4;

    aes_d_h  = HAL_Aes128_Init((uint8_t *)key, (uint8_t *)iv, HAL_AES_DECRYPTION);
    if (!aes_d_h) {
        COAP_INFO("fail to decrypt");
        return  0;
    }
    if (n > 1) {
        ret = HAL_Aes128_Cbc_Decrypt(aes_d_h, src, n - 1, out);
    }

    if (ret == 0) {
        char *out_c = (char *)out;
        int offset = n > 0 ? ((n - 1) << 4) : 0;
        out_c[offset] = 0;

        if (aes_d_h) {
            ret = HAL_Aes128_Cbc_Decrypt(aes_d_h, src + offset, 1, out_c + offset);
        } else {
            COAP_ERR("fail to decrypt remain data");
        }

        if (ret == 0) {
            char pad = out_c[len - 1];
            out_c[len - pad] = 0;
            /*
            COAP_DEBUG("decrypt data:%s, len:%d", out_c, len - pad);
            */
            HAL_Aes128_Destroy(aes_d_h);
            return len - pad;
        }
    }
    HAL_Aes128_Destroy(aes_d_h);

    return 0;
}


#if AES_CFB_NOPADDING
static int iotx_aes_cfb_encrypt(const unsigned char *src, int len, const unsigned char *key, void *out)
{
    int ret = -1;
    char *iv = "543yhjy97ae7fyfg";

    p_HAL_Aes128_t aes_e_h = HAL_Aes128_Init((unsigned char *)key, (unsigned char *)iv, HAL_AES_ENCRYPTION);
    ret = HAL_Aes128_Cfb_Encrypt(aes_e_h, src, len, out);
    HAL_Aes128_Destroy(aes_e_h);

    COAP_DEBUG("to encrypt src:%s, len:%d", src, len);
    return len;
}

int iotx_aes_cfb_decrypt(const unsigned char *src, int len, const unsigned char *key, void *out)
{
    int ret = -1;
    char *iv = "543yhjy97ae7fyfg";

    p_HAL_Aes128_t aes_d_h = HAL_Aes128_Init((unsigned char *)key, (unsigned char *)iv, HAL_AES_ENCRYPTION);
    ret = HAL_Aes128_Cfb_Decrypt(aes_d_h, src, len, out);
    HAL_Aes128_Destroy(aes_d_h);

    return ret;
}
#endif

int IOT_CoAP_DeviceNameAuth(iotx_coap_context_t *p_context)
{
    int len = 0;
    int ret = COAP_SUCCESS;
    Cloud_CoAPContext      *p_coap_ctx = NULL;
    iotx_coap_t      *p_iotx_coap = NULL;
    Cloud_CoAPMessage       message;
    unsigned char    *p_payload   = NULL;
    unsigned char     token[8] = {0};
    char sign[IOTX_SIGN_LENGTH]   = {0};

    p_iotx_coap = (iotx_coap_t *)p_context;
    if (NULL == p_iotx_coap ||
        (NULL != p_iotx_coap &&
         (NULL == p_iotx_coap->p_auth_token || NULL == p_iotx_coap->p_coap_ctx || 0 == p_iotx_coap->auth_token_len))) {
        COAP_DEBUG("Invalid paramter");
        return IOTX_ERR_INVALID_PARAM;
    }

    p_coap_ctx = (Cloud_CoAPContext *)p_iotx_coap->p_coap_ctx;

    CoAPMessage_init(&message);
    CoAPMessageType_set(&message, COAP_MESSAGE_TYPE_CON);
    CoAPMessageCode_set(&message, COAP_MSG_CODE_POST);
    CoAPMessageId_set(&message, Cloud_CoAPMessageId_gen(p_coap_ctx));
    len = iotx_get_coap_token(p_iotx_coap, token);
    CoAPMessageToken_set(&message, token, len);
    Cloud_CoAPMessageHandler_set(&message, iotx_device_name_auth_callback);

    CoAPStrOption_add(&message, COAP_OPTION_URI_PATH, (unsigned char *)IOTX_AUTH_STR, strlen(IOTX_AUTH_STR));
    CoAPUintOption_add(&message, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_JSON);
    CoAPUintOption_add(&message, COAP_OPTION_ACCEPT, COAP_CT_APP_JSON);

    CoAPMessageUserData_set(&message, (void *)p_iotx_coap);

    p_payload = coap_malloc(COAP_MSG_MAX_PDU_LEN);
    if (NULL == p_payload) {
        CoAPMessage_destory(&message);
        return IOTX_ERR_NO_MEM;
    }
    memset(p_payload, 0x00, COAP_MSG_MAX_PDU_LEN);

    if (COAP_ENDPOINT_PSK == p_iotx_coap->p_coap_ctx->network.ep_type) {
        iotx_calc_sign_with_seq(p_iotx_coap->p_devinfo->device_secret, p_iotx_coap->p_devinfo->device_id,
                                p_iotx_coap->p_devinfo->device_name, p_iotx_coap->p_devinfo->product_key, p_iotx_coap->seq, sign);
        HAL_Snprintf((char *)p_payload, COAP_MSG_MAX_PDU_LEN,
                     IOTX_AUTH_DEVICENAME_STR_WITH_SEQ,
                     p_iotx_coap->p_devinfo->product_key,
                     p_iotx_coap->p_devinfo->device_name,
                     p_iotx_coap->p_devinfo->device_id,
                     sign, p_iotx_coap->seq);

    } else {
        iotx_calc_sign(p_iotx_coap->p_devinfo->device_secret, p_iotx_coap->p_devinfo->device_id,
                       p_iotx_coap->p_devinfo->device_name, p_iotx_coap->p_devinfo->product_key, sign);
        HAL_Snprintf((char *)p_payload, COAP_MSG_MAX_PDU_LEN,
                     IOTX_AUTH_DEVICENAME_STR,
                     p_iotx_coap->p_devinfo->product_key,
                     p_iotx_coap->p_devinfo->device_name,
                     p_iotx_coap->p_devinfo->device_id,
                     sign);
    }
    CoAPMessagePayload_set(&message, p_payload, strlen((char *)p_payload));
    COAP_DEBUG("The payload is: %s", message.payload);
    COAP_DEBUG("Send authentication message to server");
    ret = Cloud_CoAPMessage_send(p_coap_ctx, &message);
    coap_free(p_payload);
    CoAPMessage_destory(&message);

    if (COAP_SUCCESS != ret) {
        COAP_DEBUG("Send authentication message to server failed, ret = %d", ret);
        return IOTX_ERR_SEND_MSG_FAILED;
    }

    ret = Cloud_CoAPMessage_recv(p_coap_ctx, CONFIG_COAP_AUTH_TIMEOUT, 2);
    if (0 < ret && !p_iotx_coap->is_authed) {
        COAP_INFO("CoAP authenticate failed");
        return IOTX_ERR_AUTH_FAILED;
    }


    iotx_set_report_func(coap_report_func);
    /* report module id */
    ret = iotx_report_mid(p_context);
    if (SUCCESS_RETURN != ret) {
        COAP_WRN("Send ModuleId message to server(CoAP) failed, ret = %d", ret);
    }
    /* report device information */
    ret = iotx_report_devinfo(p_context);
    if (SUCCESS_RETURN != ret) {
        COAP_WRN("Send devinfo message to server(CoAP) failed, ret = %d", ret);
    }

#if 0
    /* report firmware version */
    ret = iotx_report_firmware_version(p_context);
    if (SUCCESS_RETURN != ret) {
        COAP_DEBUG("Send firmware message to server(CoAP) failed, ret = %d", ret);
        return IOTX_ERR_SEND_MSG_FAILED;
    }
#endif

    return IOTX_SUCCESS;
}

static int iotx_split_path_2_option(char *uri, Cloud_CoAPMessage *message)
{
    char *ptr     = NULL;
    char *pstr    = NULL;
    char  path[COAP_MSG_MAX_PATH_LEN]  = {0};

    if (NULL == uri || NULL == message) {
        COAP_ERR("Invalid paramter p_path %p, p_message %p", uri, message);
        return IOTX_ERR_INVALID_PARAM;
    }
    if (IOTX_URI_MAX_LEN < strlen(uri)) {
        COAP_ERR("The uri length is too loog,len = %d", (int)strlen(uri));
        return IOTX_ERR_URI_TOO_LOOG;
    }
    COAP_DEBUG("The uri is %s", uri);
    ptr = pstr = uri;
    while ('\0' != *ptr) {
        if ('/' == *ptr) {
            if (ptr != pstr) {
                memset(path, 0x00, sizeof(path));
                strncpy(path, pstr, ptr - pstr);
                COAP_DEBUG("path: %s,len=%d", path, (int)(ptr - pstr));
                CoAPStrOption_add(message, COAP_OPTION_URI_PATH,
                                  (unsigned char *)path, (int)strlen(path));
            }
            pstr = ptr + 1;

        }
        if ('\0' == *(ptr + 1) && '\0' != *pstr) {
            memset(path, 0x00, sizeof(path));
            strncpy(path, pstr, sizeof(path) - 1);
            COAP_DEBUG("path: %s,len=%d", path, (int)strlen(path));
            CoAPStrOption_add(message, COAP_OPTION_URI_PATH,
                              (unsigned char *)path, (int)strlen(path));
        }
        ptr ++;
    }
    return IOTX_SUCCESS;
}

uint32_t IOT_CoAP_GetCurToken(iotx_coap_context_t *p_context)
{
    iotx_coap_t *p_iotx_coap = NULL;

    if (p_context == NULL) {
        return IOTX_ERR_INVALID_PARAM;
    }
    p_iotx_coap = (iotx_coap_t *)p_context;

    return p_iotx_coap->coap_token;
}

int IOT_CoAP_SendMessage(iotx_coap_context_t *p_context, char *p_path, iotx_message_t *p_message)
{

    int len = 0;
    int ret = IOTX_SUCCESS;
    Cloud_CoAPContext *p_coap_ctx = NULL;
    iotx_coap_t *p_iotx_coap = NULL;
    Cloud_CoAPMessage message;
    unsigned char token[8] = {0};
    unsigned char *payload = NULL;

    p_iotx_coap = (iotx_coap_t *)p_context;

    if (NULL == p_context || NULL == p_path || NULL == p_message ||
        (NULL != p_iotx_coap && NULL == p_iotx_coap->p_coap_ctx)) {
        COAP_ERR("Invalid paramter p_context %p, p_uri %p, p_message %p",
                 p_context, p_path, p_message);
        return IOTX_ERR_INVALID_PARAM;
    }

#ifdef INFRA_LOG_NETWORK_PAYLOAD
    COAP_INFO("Upstream Topic: '%s'", p_path);
    COAP_INFO("Upstream Payload:");
    iotx_facility_json_print((const char *)p_message->p_payload, LOG_INFO_LEVEL, '>');
#endif

    /* as this function only support POST request message, type ACK and RST shall be considered error parameters */
    if (p_message->msg_type != IOTX_MESSAGE_CON && p_message->msg_type != IOTX_MESSAGE_NON) {
        return IOTX_ERR_INVALID_PARAM;
    }

    if (p_message->payload_len >= COAP_MSG_MAX_PDU_LEN) {
        COAP_ERR("The payload length %d is too loog", p_message->payload_len);
        return IOTX_ERR_MSG_TOO_LOOG;
    }

    p_coap_ctx = (Cloud_CoAPContext *)p_iotx_coap->p_coap_ctx;
    if (p_iotx_coap->is_authed) {

        /* CoAPMessage_init(&message); */
        CoAPMessage_init(&message);
        CoAPMessageType_set(&message, p_message->msg_type);
        CoAPMessageCode_set(&message, COAP_MSG_CODE_POST);
        CoAPMessageId_set(&message, Cloud_CoAPMessageId_gen(p_coap_ctx));
        len = iotx_get_coap_token(p_iotx_coap, token);
        CoAPMessageToken_set(&message, token, len);
        CoAPMessageUserData_set(&message, (void *)p_message->user_data);
        Cloud_CoAPMessageHandler_set(&message, p_message->resp_callback);

        ret = iotx_split_path_2_option(p_path, &message);
        if (IOTX_SUCCESS != ret) {
            return ret;
        }

        if (IOTX_CONTENT_TYPE_CBOR == p_message->content_type) {
            CoAPUintOption_add(&message, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_CBOR);
            CoAPUintOption_add(&message, COAP_OPTION_ACCEPT, COAP_CT_APP_OCTET_STREAM);
        } else {
            CoAPUintOption_add(&message, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_JSON);
            CoAPUintOption_add(&message, COAP_OPTION_ACCEPT, COAP_CT_APP_OCTET_STREAM);
        }
        CoAPStrOption_add(&message,  COAP_OPTION_AUTH_TOKEN,
                          (unsigned char *)p_iotx_coap->p_auth_token, strlen(p_iotx_coap->p_auth_token));
        if (COAP_ENDPOINT_PSK == p_iotx_coap->p_coap_ctx->network.ep_type) {
            unsigned char buff[32] = {0};
            unsigned char seq[33] = {0};
            HAL_Snprintf((char *)buff, sizeof(buff) - 1, "%d", p_iotx_coap->seq++);
            len = iotx_aes_cbc_encrypt(buff, strlen((char *)buff), p_iotx_coap->key, seq);
            if (0 < len) {
                CoAPStrOption_add(&message,  COAP_OPTION_SEQ, (unsigned char *)seq, len);
            } else {
                COAP_INFO("Encrypt seq failed");
            }
            HEXDUMP_DEBUG(seq, len);

            payload = (unsigned char *)coap_malloc(COAP_MSG_MAX_PDU_LEN);
            if (NULL == payload) {
                return IOTX_ERR_NO_MEM;
            }
            memset(payload, 0x00, COAP_MSG_MAX_PDU_LEN);
            len = iotx_aes_cbc_encrypt(p_message->p_payload, p_message->payload_len, p_iotx_coap->key, payload);
            if (0 == len) {
                coap_free(payload);
                payload = NULL;
                return IOTX_ERR_INVALID_PARAM;
            }

            HEXDUMP_DEBUG(payload, len);
            CoAPMessagePayload_set(&message, payload, len);
        } else {
            CoAPMessagePayload_set(&message, p_message->p_payload, p_message->payload_len);
        }
        ret = Cloud_CoAPMessage_send(p_coap_ctx, &message);
        CoAPMessage_destory(&message);
        if (NULL != payload) {
            coap_free(payload);
            payload = NULL;
        }

        if (COAP_ERROR_DATA_SIZE == ret) {
            return IOTX_ERR_MSG_TOO_LOOG;
        }

        return IOTX_SUCCESS;
    } else {
        COAP_ERR("The client [%s/%s] still un-authorized yet, return %d",
                 p_iotx_coap->p_devinfo->product_key,
                 p_iotx_coap->p_devinfo->device_name,
                 IOTX_ERR_NOT_AUTHED
                );
        return IOTX_ERR_NOT_AUTHED;
    }
}


int IOT_CoAP_GetMessagePayload(void *p_message, unsigned char **pp_payload, int *p_len)
{
    Cloud_CoAPMessage *message = NULL;
    iotx_coap_t *p_iotx_coap = NULL;

    if (NULL == p_message || NULL == pp_payload || NULL == p_len || NULL == g_coap_context) {
        COAP_ERR("Invalid parameter: p_message=%p, pp_payload=%p, p_len=%p",
                 p_message, pp_payload, p_len);
        return IOTX_ERR_INVALID_PARAM;
    }

    p_iotx_coap = (iotx_coap_t *)g_coap_context;
    message = (Cloud_CoAPMessage *)p_message;

    COAP_DEBUG("message->payload: %p", message->payload);
    COAP_DEBUG("message->payloadlen: %d", message->payloadlen);

    if (message->payloadlen >= COAP_MSG_MAX_PDU_LEN) {
        COAP_ERR("Invalid parameter: message->payloadlen(%d) out of [0, %d]",
                 message->payloadlen, COAP_MSG_MAX_PDU_LEN);
        return IOTX_ERR_INVALID_PARAM;
    }

    if (COAP_ENDPOINT_PSK == p_iotx_coap->p_coap_ctx->network.ep_type) {
        int len = 0;
        unsigned char *payload = NULL;
        payload = coap_malloc(COAP_MSG_MAX_PDU_LEN);
        if (NULL == payload) {
            return IOTX_ERR_NO_MEM;
        }
        memset(payload, 0x00, COAP_MSG_MAX_PDU_LEN);

        HEXDUMP_DEBUG(message->payload, message->payloadlen);

        len = iotx_aes_cbc_decrypt(message->payload, message->payloadlen, p_iotx_coap->key, payload);
        if (len > 0) {
            COAP_DEBUG("payload: %.*s, len %d", len, payload, len);
        }
        if (len != 0) {
            memcpy(message->payload, payload, len);
            message->payloadlen = len;
            HEXDUMP_DEBUG(payload, len);
        }

        coap_free(payload);
    }

    *pp_payload    =  message->payload;
    *p_len         =  message->payloadlen;

    return IOTX_SUCCESS;
}

int  IOT_CoAP_GetMessageToken(void *p_message, unsigned int *token)
{

    Cloud_CoAPMessage *message = NULL;

    if (NULL == p_message || NULL == token) {
        COAP_ERR("Invalid paramter p_message %p, token= %p", p_message, token);
        return -1;
    }
    message = (Cloud_CoAPMessage *)p_message;

    *token = ((unsigned int)(message->token[3]) & 0xff) << 24;
    *token += ((unsigned int)(message->token[2]) & 0xff) << 16;
    *token += ((unsigned int)(message->token[1]) & 0xff) << 8;
    *token += ((unsigned int)(message->token[0]) & 0xff);
    return 0;
}

int  IOT_CoAP_GetMessageCode(void *p_message, iotx_coap_resp_code_t *p_resp_code)
{
    Cloud_CoAPMessage *message = NULL;

    if (NULL == p_message || NULL == p_resp_code) {
        COAP_ERR("Invalid paramter p_message %p, p_resp_code %p",
                 p_message, p_resp_code);
        return IOTX_ERR_INVALID_PARAM;
    }
    message = (Cloud_CoAPMessage *)p_message;
    *p_resp_code   = (iotx_coap_resp_code_t) message->header.code;

    return IOTX_SUCCESS;
}

static unsigned int iotx_get_seq(void)
{
    HAL_Srandom((unsigned int)HAL_UptimeMs());
    return HAL_Random(0xffffffff) % 10000;
}

iotx_coap_context_t *IOT_CoAP_Init(iotx_coap_config_t *p_config)
{
    Cloud_CoAPInitParam param;
    char url[128] = {0};
    iotx_coap_t *p_iotx_coap = NULL;

    if (NULL == p_config) {
        COAP_ERR("Invalid paramter p_config %p", p_config);
        return NULL;
    }
    if (NULL == p_config->p_devinfo) {
        COAP_ERR("Invalid paramter p_devinfo %p", p_config->p_devinfo);
        return NULL;
    }

    p_iotx_coap = coap_malloc(sizeof(iotx_coap_t));
    if (NULL == p_iotx_coap) {
        COAP_ERR(" Allocate memory for iotx_coap_context_t failed");
        return NULL;
    }
    memset(p_iotx_coap, 0x00, sizeof(iotx_coap_t));

    p_iotx_coap->p_auth_token = coap_malloc(IOTX_AUTH_TOKEN_LEN);
    if (NULL == p_iotx_coap->p_auth_token) {
        COAP_ERR(" Allocate memory for auth token failed");
        goto err;
    }
    memset(p_iotx_coap->p_auth_token, 0x00, IOTX_AUTH_TOKEN_LEN);

    /*Set the client isn't authed*/
    p_iotx_coap->is_authed = IOT_FALSE;
    p_iotx_coap->auth_token_len = IOTX_AUTH_TOKEN_LEN;

    /*Get deivce information*/
    p_iotx_coap->p_devinfo = coap_malloc(sizeof(iotx_deviceinfo_t));
    if (NULL == p_iotx_coap->p_devinfo) {
        COAP_ERR(" Allocate memory for iotx_deviceinfo_t failed");
        goto err;
    }
    memset(p_iotx_coap->p_devinfo, 0x00, sizeof(iotx_deviceinfo_t));

    /*It should be implement by the user*/
    if (NULL != p_config->p_devinfo) {
        memset(p_iotx_coap->p_devinfo, 0x00, sizeof(iotx_deviceinfo_t));
        strncpy(p_iotx_coap->p_devinfo->device_id,    p_config->p_devinfo->device_id,   strlen(p_config->p_devinfo->device_id));
        strncpy(p_iotx_coap->p_devinfo->product_key,  p_config->p_devinfo->product_key,
                strlen(p_config->p_devinfo->product_key));
        strncpy(p_iotx_coap->p_devinfo->device_secret, p_config->p_devinfo->device_secret,
                strlen(p_config->p_devinfo->device_secret));
        strncpy(p_iotx_coap->p_devinfo->device_name,  p_config->p_devinfo->device_name,
                strlen(p_config->p_devinfo->device_name));
    }

    /*Init coap token*/
    p_iotx_coap->coap_token = IOTX_COAP_INIT_TOKEN;
    p_iotx_coap->seq        = iotx_get_seq();
    memset(p_iotx_coap->key, 0x00, sizeof(p_iotx_coap->key));

    /*Create coap context*/
    memset(&param, 0x00, sizeof(Cloud_CoAPInitParam));

    if (NULL !=  p_config->p_url) {
        param.url = p_config->p_url;
    } else {
        HAL_Snprintf(url, sizeof(url), IOTX_COAP_ONLINE_PSK_SERVER_URL, p_iotx_coap->p_devinfo->product_key);
        param.url = url;
        COAP_INFO("Using default CoAP server: %s", url);
    }
    param.maxcount = IOTX_LIST_MAX_ITEM;
    param.notifier = (Cloud_CoAPEventNotifier)iotx_event_notifyer;
    param.waittime = p_config->wait_time_ms;
    p_iotx_coap->p_coap_ctx = Cloud_CoAPContext_create(&param);
    if (NULL == p_iotx_coap->p_coap_ctx) {
        COAP_ERR(" Create coap context failed");
        goto err;
    }

    /*Register the event handle to notify the application */
    p_iotx_coap->event_handle = p_config->event_handle;

    g_coap_context = (iotx_coap_context_t *)p_iotx_coap;
    return (iotx_coap_context_t *)p_iotx_coap;
err:
    /* Error, release the memory */
    if (NULL != p_iotx_coap) {
        if (NULL != p_iotx_coap->p_devinfo) {
            coap_free(p_iotx_coap->p_devinfo);
        }
        if (NULL != p_iotx_coap->p_auth_token) {
            coap_free(p_iotx_coap->p_auth_token);
        }
        if (NULL != p_iotx_coap->p_coap_ctx) {
            Cloud_CoAPContext_free(p_iotx_coap->p_coap_ctx);
        }

        p_iotx_coap->auth_token_len = 0;
        p_iotx_coap->is_authed = IOT_FALSE;
        coap_free(p_iotx_coap);
    }
    return NULL;
}

void IOT_CoAP_Deinit(iotx_coap_context_t **pp_context)
{
    iotx_coap_t *p_iotx_coap = NULL;

    if (NULL != pp_context && NULL != *pp_context) {
        p_iotx_coap = (iotx_coap_t *)*pp_context;
        p_iotx_coap->is_authed = IOT_FALSE;
        p_iotx_coap->auth_token_len = 0;
        p_iotx_coap->coap_token = IOTX_COAP_INIT_TOKEN;

        if (NULL != p_iotx_coap->p_auth_token) {
            coap_free(p_iotx_coap->p_auth_token);
            p_iotx_coap->p_auth_token = NULL;
        }

        if (NULL != p_iotx_coap->p_devinfo) {
            coap_free(p_iotx_coap->p_devinfo);
            p_iotx_coap->p_devinfo = NULL;
        }

        if (NULL != p_iotx_coap->p_coap_ctx) {
            Cloud_CoAPContext_free(p_iotx_coap->p_coap_ctx);
            p_iotx_coap->p_coap_ctx = NULL;
        }
        coap_free(p_iotx_coap);
        *pp_context = NULL;
        g_coap_context = NULL;
    }
}

int IOT_CoAP_Yield(iotx_coap_context_t *p_context)
{
    iotx_coap_t *p_iotx_coap = NULL;
    p_iotx_coap = (iotx_coap_t *)p_context;
    if (NULL == p_iotx_coap || (NULL != p_iotx_coap && NULL == p_iotx_coap->p_coap_ctx)) {
        COAP_ERR("Invalid paramter");
        return IOTX_ERR_INVALID_PARAM;
    }

    return Cloud_CoAPMessage_cycle(p_iotx_coap->p_coap_ctx);
}

