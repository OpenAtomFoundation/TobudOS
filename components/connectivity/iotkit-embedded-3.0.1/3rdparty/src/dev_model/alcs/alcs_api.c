/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include <stdlib.h>
#include <time.h>
#include "alcs_internal.h"
#include "alcs_api.h"
#include "alcs_coap.h"
#include "alcs_api_internal.h"
#include "CoAPPlatform.h"
#include "CoAPObserve.h"

LIST_HEAD(secure_resource_cb_head);

static bool is_inited = 0;
#ifdef SUPPORT_MULTI_DEVICES
LIST_HEAD(device_list);

device_auth_list *get_device(CoAPContext *context)
{
    device_auth_list *node = NULL, *next = NULL;
    list_for_each_entry_safe(node, next, &device_list, lst, device_auth_list) {
        if (node->context == context) {
            return node;
        }
    }
    return NULL;
}

auth_list *get_list(CoAPContext *context)
{
    device_auth_list *dev_lst = get_device(context);
    return dev_lst ? &dev_lst->lst_auth : NULL;
}

#ifdef ALCS_CLIENT_ENABLED
struct list_head *get_ctl_session_list(CoAPContext *context)
{
    device_auth_list *dev_lst = get_device(context);
    if (!dev_lst || !(dev_lst->role & ROLE_CLIENT)) {
        return NULL;
    }
    return &dev_lst->lst_ctl_sessions;
}
#endif
#ifdef ALCS_SERVER_ENABLED
struct list_head *get_svr_session_list(CoAPContext *context)
{
    device_auth_list *dev_lst = get_device(context);
    return dev_lst && (dev_lst->role & ROLE_SERVER) ? &dev_lst->lst_svr_sessions : NULL;
}
#endif

#else
device_auth_list _device;
#endif

void remove_session(CoAPContext *ctx, session_item *session)
{
    COAP_INFO("remove_session");
    if (session) {
        CoapObsServerAll_delete(ctx, &session->addr);
        list_del(&session->lst);
        coap_free(session);
    }
}

session_item *get_session_by_checksum(struct list_head *sessions, NetworkAddr *addr, char ck[PK_DN_CHECKSUM_LEN])
{
    session_item *node = NULL, *next = NULL;
    if (!sessions || !ck) {
        return NULL;
    }
    list_for_each_entry_safe(node, next, sessions, lst, session_item) {
        if (is_networkadd_same(addr, &node->addr)
            && strncmp(node->pk_dn, ck, PK_DN_CHECKSUM_LEN) == 0) {
            COAP_DEBUG("find node, sessionid:%d", node->sessionId);
            return node;
        }
    }
    return NULL;
}

static session_item *get_session(struct list_head *sessions, AlcsDeviceKey *devKey)
{
    char ck[PK_DN_CHECKSUM_LEN] = {0};
    char path[100] = {0};
    if (!sessions || !devKey || !devKey->pk || !devKey->dn) {
        return NULL;
    }
    HAL_Snprintf(path, sizeof(path), "%s%s", devKey->pk, devKey->dn);
    CoAPPathMD5_sum(path, strlen(path), ck, PK_DN_CHECKSUM_LEN);

    return get_session_by_checksum(sessions, &devKey->addr, ck);
}

#ifdef ALCS_CLIENT_ENABLED
session_item *get_ctl_session(CoAPContext *ctx, AlcsDeviceKey *devKey)
{
    struct list_head *sessions = get_ctl_session_list(ctx);
    COAP_DEBUG("get_ctl_session");
    return get_session(sessions, devKey);
}

#endif

#ifdef ALCS_SERVER_ENABLED
session_item *get_svr_session(CoAPContext *ctx, AlcsDeviceKey *devKey)
{
    struct list_head *sessions = get_svr_session_list(ctx);
    return get_session(sessions, devKey);
}
#endif

static session_item *get_auth_session(CoAPContext *ctx, AlcsDeviceKey *devKey)
{
#ifdef ALCS_CLIENT_ENABLED
    session_item *node = get_ctl_session(ctx, devKey);
    if (node && node->sessionId) {
        return node;
    }
#endif
#ifdef ALCS_SERVER_ENABLED
    session_item *node1 = get_svr_session(ctx, devKey);
    if (node1 && node1->sessionId) {
        return node1;
    }
#endif

    return NULL;
}

static session_item *get_auth_session_by_checksum(CoAPContext *ctx, NetworkAddr *addr, char ck[])
{
#ifdef ALCS_CLIENT_ENABLED
    struct list_head *sessions = get_ctl_session_list(ctx);
    session_item *node = get_session_by_checksum(sessions, addr, ck);
    if (node && node->sessionId) {
        return node;
    }
#endif
#ifdef ALCS_SERVER_ENABLED
    struct list_head *sessions1 = get_svr_session_list(ctx);
    session_item *node1 = get_session_by_checksum(sessions1, addr, ck);
    if (node1 && node1->sessionId) {
        return node1;
    }
#endif

    return NULL;
}

void gen_random_key(unsigned char random[], int len)
{
    int i = 0, flag = 0;

    memset(random, 0x00, len);
    srand((unsigned)time(NULL));

    for (i = 0; i < len - 1; i++) {
        flag = rand() % 3;
        switch (flag) {
            case 0:
                random[i] = 'A' + rand() % 26;
                break;
            case 1:
                random[i] = 'a' + rand() % 26;
                break;
            case 2:
                random[i] = '0' + rand() % 10;
                break;
            default:
                random[i] = 'x';
                break;
        }
    }
}

#ifdef ALCS_SERVER_ENABLED
    extern void alcs_rec_auth_select(CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *request);
    extern void alcs_rec_auth(CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *request);
    extern void alcs_rec_heart_beat(CoAPContext *context, const char *paths, NetworkAddr *remote, CoAPMessage *request);
#endif

int alcs_auth_init(CoAPContext *ctx, const char *productKey, const char *deviceName, char role)
{
    device_auth_list *dev;
#ifdef ALCS_SERVER_ENABLED
    char path[256];
#endif
    if (is_inited) {
        return 0;
    }
    is_inited = 1;

    /* auth_list* lst_auth; */

#ifdef SUPPORT_MULTI_DEVICES
    INIT_LIST_HEAD(&device_list);

    dev = coap_malloc(sizeof(device_auth_list));
    list_add_tail(&dev->lst, &device_list);
#else
    dev = &_device;
#endif
    dev->context = ctx;
    dev->seq = 1;
    dev->role = role;
    memset(&dev->lst_auth, 0, sizeof(auth_list));
    /* strcpy (dev->deviceName, deviceName); */
    /* strcpy (dev->productKey, productKey); */

    INIT_LIST_HEAD(&dev->lst);
    INIT_LIST_HEAD(&secure_resource_cb_head);

    if (role & ROLE_SERVER) {
#ifdef ALCS_SERVER_ENABLED
        INIT_LIST_HEAD(&dev->lst_svr_sessions);
        INIT_LIST_HEAD(&dev->lst_auth.lst_svr);

        HAL_Snprintf(path, sizeof(path), "/dev/%s/%s/core/service/auth", productKey, deviceName);
        alcs_resource_register(ctx, productKey, deviceName, path, COAP_PERM_GET, COAP_CT_APP_JSON, 60, 0, alcs_rec_auth);
        strcat(path, "/select");
        alcs_resource_register(ctx, productKey, deviceName, path, COAP_PERM_GET, COAP_CT_APP_JSON, 60, 0, alcs_rec_auth_select);
        alcs_resource_register(ctx, productKey, deviceName, "/dev/core/service/heartBeat", COAP_PERM_GET, COAP_CT_APP_JSON, 60,
                               0, alcs_rec_heart_beat);
#endif
    }

    if (role & ROLE_CLIENT) {
#ifdef ALCS_CLIENT_ENABLED
        INIT_LIST_HEAD(&dev->lst_ctl_sessions);
        INIT_LIST_HEAD(&dev->lst_auth.lst_ctl);
#endif
    }

    INIT_LIST_HEAD(&dev->lst_auth.lst_ctl_group);
    INIT_LIST_HEAD(&dev->lst_auth.lst_svr_group);
    dev->lst_auth.list_mutex = HAL_MutexCreate();

    return COAP_SUCCESS;
}

void alcs_auth_subdev_init(CoAPContext *ctx, const char *productKey, const char *deviceName)
{
    char path[256];
    HAL_Snprintf(path, sizeof(path), "/dev/%s/%s/core/service/auth", productKey, deviceName);
    alcs_resource_register(ctx, productKey, deviceName, path, COAP_PERM_GET, COAP_CT_APP_JSON, 60, 0, alcs_rec_auth);
    strcat(path, "/select");
    alcs_resource_register(ctx, productKey, deviceName, path, COAP_PERM_GET, COAP_CT_APP_JSON, 60, 0, alcs_rec_auth_select);
}

void alcs_auth_deinit(void)
{
#ifdef SUPPORT_MULTI_DEVICES
    device_auth_list *node = NULL, *next = NULL;
#endif
    if (is_inited == 0) {
        return;
    }
    is_inited = 0;

    alcs_resource_cb_deinit();
    alcs_auth_list_deinit();

#ifdef SUPPORT_MULTI_DEVICES
    list_for_each_entry_safe(node, next, &device_list, lst, device_auth_list) {
        if (node->lst_auth.list_mutex) {
            HAL_MutexDestroy(node->lst_auth.list_mutex);
            node->lst_auth.list_mutex = NULL;
        }
    }
#else
    if (_device.lst_auth.list_mutex) {
        HAL_MutexDestroy(_device.lst_auth.list_mutex);
        _device.lst_auth.list_mutex = NULL;

    }
#endif
}

bool is_networkadd_same(NetworkAddr *addr1, NetworkAddr *addr2)
{
    if (!addr1 || !addr2) {
        return 0;
    }
    COAP_DEBUG("compare addr1:%s,addr2:%s", addr1->addr, addr2->addr);
    return addr1->port == addr2->port && !strcmp((const char *)addr1->addr, (const char *)addr2->addr);
}

int alcs_encrypt(const char *src, int len, const char *key, void *out)
{
    char *iv = "a1b1c1d1e1f1g1h1";

    int len1 = len & 0xfffffff0;
    int len2 = len1 + 16;
    int pad = len2 - len;
    int ret = 0;

    if (len1) {
        p_HAL_Aes128_t aes_e_h = HAL_Aes128_Init((uint8_t *)key, (uint8_t *)iv, HAL_AES_ENCRYPTION);
        ret = HAL_Aes128_Cbc_Encrypt(aes_e_h, src, len1 >> 4, out);
        HAL_Aes128_Destroy(aes_e_h);
    }
    if (!ret && pad) {
        char buf[16];
        p_HAL_Aes128_t aes_e_h = NULL;
        memcpy(buf, src + len1, len - len1);
        memset(buf + len - len1, pad, pad);
        aes_e_h = HAL_Aes128_Init((uint8_t *)key, (uint8_t *)iv, HAL_AES_ENCRYPTION);
        ret = HAL_Aes128_Cbc_Encrypt(aes_e_h, buf, 1, (uint8_t *)out + len1);
        HAL_Aes128_Destroy(aes_e_h);
    }

    COAP_DEBUG("to encrypt src:%s, len:%d", src, len2);
    return ret == 0 ? len2 : 0;
}

int alcs_decrypt(const char *src, int len, const char *key, void *out)
{
    char *iv = "a1b1c1d1e1f1g1h1";
    p_HAL_Aes128_t aes_d_h;
    int n = len >> 4;
    char *out_c = NULL;
    int offset = 0;
    int ret = 0;
    char pad = 0;

    COAP_DEBUG("to decrypt len:%d", len);

    do {
        if (n > 1) {
            aes_d_h  = HAL_Aes128_Init((uint8_t *)key, (uint8_t *)iv, HAL_AES_DECRYPTION);
            if (!aes_d_h) {
                COAP_ERR("fail to decrypt init");
                break;
            }

            ret = HAL_Aes128_Cbc_Decrypt(aes_d_h, src, n - 1, out);
            HAL_Aes128_Destroy(aes_d_h);

            if (ret != 0) {
                COAP_ERR("fail to decrypt");
                break;
            }
        }

        out_c = (char *)out;
        offset = n > 0 ? ((n - 1) << 4) : 0;
        out_c[offset] = 0;

        aes_d_h  = HAL_Aes128_Init((uint8_t *)key, (uint8_t *)iv, HAL_AES_DECRYPTION);
        if (!aes_d_h) {
            COAP_ERR("fail to decrypt init");
            break;
        }

        ret = HAL_Aes128_Cbc_Decrypt(aes_d_h, src + offset, 1, out_c + offset);
        HAL_Aes128_Destroy(aes_d_h);

        if (ret != 0) {
            COAP_ERR("fail to decrypt remain data");
            break;
        }

        pad = out_c[len - 1];
        out_c[len - pad] = 0;
        COAP_DEBUG("decrypt data:%s, len:%d", out_c, len - pad);
        return len - pad;
    } while (0);

    return 0;
}

bool alcs_is_auth(CoAPContext *ctx, AlcsDeviceKey *devKey)
{
    return get_auth_session(ctx, devKey) != NULL;
}

/*---------------------------------------------------------*/
typedef struct {
    void *orig_user_data;
    char pk_dn[PK_DN_CHECKSUM_LEN];
    CoAPSendMsgHandler orig_handler;
} secure_send_item;

static int do_secure_send(CoAPContext *ctx, NetworkAddr *addr, CoAPMessage *message, const char *key, char *buf)
{
    int ret = COAP_SUCCESS;
    void *payload_old = message->payload;
    int len_old = message->payloadlen;

    COAP_DEBUG("do_secure_send");

    message->payload = (unsigned char *)buf;
    message->payloadlen = alcs_encrypt((const char *)payload_old, len_old, key, message->payload);
    ret = CoAPMessage_send(ctx, addr, message);

    message->payload = payload_old;
    message->payloadlen = len_old;

    return ret;
}

void secure_sendmsg_handler(CoAPContext *context, CoAPReqResult result, void *userdata, NetworkAddr *remote,
                            CoAPMessage *message);
int internal_secure_send(CoAPContext *ctx, session_item *session, NetworkAddr *addr, CoAPMessage *message, char observe,
                         CoAPSendMsgHandler handler)
{
    int encryptlen = 0;

    COAP_DEBUG("internal_secure_send");
    if (!ctx || !session || !addr || !message) {
        COAP_ERR("parameter is null");
        return COAP_ERROR_INVALID_PARAM;
    }

    if (handler) {
        secure_send_item *item = (secure_send_item *)coap_malloc(sizeof(secure_send_item));
        item->orig_user_data = message->user;
        item->orig_handler = handler;
        memcpy(item->pk_dn, session->pk_dn, PK_DN_CHECKSUM_LEN);

        message->handler = secure_sendmsg_handler;
        message->user = item;
    }

    if (observe == 0) {
        CoAPUintOption_add(message, COAP_OPTION_OBSERVE, observe);
    }
    CoAPUintOption_add(message, COAP_OPTION_CONTENT_FORMAT, COAP_CT_APP_OCTET_STREAM);
    CoAPUintOption_add(message, COAP_OPTION_SESSIONID, session->sessionId);
    COAP_DEBUG("secure_send sessionId:%d", session->sessionId);

    encryptlen = (message->payloadlen & 0xfffffff0) + 16;
    if (encryptlen > 64) {
        char *buf = (char *)coap_malloc(encryptlen);
        int rt = do_secure_send(ctx, addr, message, session->sessionKey, buf);
        coap_free(buf);
        return rt;
    } else {
        char buf[64];
        return do_secure_send(ctx, addr, message, session->sessionKey, buf);
    }
}

static void call_cb(CoAPContext *context, NetworkAddr *remote, CoAPMessage *message, const char *key, char *buf,
                    secure_send_item *send_item)
{
    if (send_item->orig_handler) {
        int len = alcs_decrypt((const char *)message->payload, message->payloadlen, key, buf);
        CoAPMessage tmpMsg;
        memcpy(&tmpMsg, message, sizeof(CoAPMessage));
        tmpMsg.payload = (unsigned char *)buf;
        tmpMsg.payloadlen = len;
        send_item->orig_handler(context, COAP_REQUEST_SUCCESS, send_item->orig_user_data, remote, &tmpMsg);
    }
}

void secure_sendmsg_handler(CoAPContext *context, CoAPReqResult result, void *userdata, NetworkAddr *remote,
                            CoAPMessage *message)
{
    secure_send_item *send_item = (secure_send_item *)userdata;
    session_item *session = NULL;
    unsigned int obsVal;

    if (!context || !userdata || !remote) {
        return;
    }
    if (result == COAP_RECV_RESP_TIMEOUT) {
        if (send_item->orig_handler) {
            send_item->orig_handler(context, COAP_RECV_RESP_TIMEOUT, send_item->orig_user_data, remote, NULL);
        }
        COAP_INFO("secure_sendmsg_handler timeout");
    } else {
        unsigned int sessionId = 0;
        CoAPUintOption_get(message, COAP_OPTION_SESSIONID, &sessionId);
        COAP_DEBUG("secure_sendmsg_handler, sessionID:%d", (int)sessionId);

        session = get_auth_session_by_checksum(context, remote, send_item->pk_dn);

        if (!session || session->sessionId != sessionId) {
            COAP_ERR("secure_sendmsg_handler, need auth, from:%s", remote->addr);
            /* todo */
        } else {
            session->heart_time = HAL_UptimeMs();
            if (message->payloadlen < 128) {
                char buf[128];
                call_cb(context, remote, message, session->sessionKey, buf, send_item);
            } else {
                char *buf = (char *)coap_malloc(message->payloadlen);
                if (buf) {
                    call_cb(context, remote, message, session->sessionKey, buf, send_item);
                    coap_free(buf);
                }
            }
        }
    }

    if (CoAPUintOption_get(message, COAP_OPTION_OBSERVE, &obsVal) != COAP_SUCCESS) {
        coap_free(send_item);
    }
}

int alcs_sendmsg_secure(CoAPContext *ctx, AlcsDeviceKey *devKey, CoAPMessage *message, char observe,
                        CoAPSendMsgHandler handler)
{
    session_item *session = NULL;

    if (!ctx || !devKey || !message) {
        return COAP_ERROR_INVALID_PARAM;
    }

    session = get_auth_session(ctx, devKey);
    if (!session) {
        COAP_DEBUG("alcs_sendmsg_secure, session not found");
        return ALCS_ERR_AUTH_UNAUTH;
    }

    return internal_secure_send(ctx, session, &devKey->addr, message, observe, handler);
}

int alcs_sendrsp_secure(CoAPContext *ctx, AlcsDeviceKey *devKey, CoAPMessage *message, char observe,
                        unsigned short msgid, CoAPLenString *token)
{
    session_item *session = NULL;

    COAP_DEBUG("alcs_sendrsp_secure");
    if (!ctx || !devKey || !message) {
        return COAP_ERROR_INVALID_PARAM;
    }

    if (msgid == 0) {
        message->header.msgid = CoAPMessageId_gen(ctx);
    } else {
        message->header.msgid = msgid;
    }

    if (token) {
        message->header.tokenlen = token->len;
        memcpy(&message->token, token->data, token->len);
    }

    session = get_auth_session(ctx, devKey);
    if (!session) {
        COAP_DEBUG("alcs_sendrsp_secure, session not found");
        return ALCS_ERR_AUTH_UNAUTH;
    }

    return internal_secure_send(ctx, session, &devKey->addr, message, observe, NULL);
}

bool req_payload_parser(const char *payload, int len, char **seq, int *seqlen, char **data, int *datalen)
{
    if (!payload || !len || !seq || !seqlen || !datalen || !data) {
        return 0;
    }

    *seq = json_get_value_by_name((char *)payload, len, "id", seqlen, NULL);

    *data = json_get_value_by_name((char *)payload, len, "params", datalen, NULL);
    return *data && datalen;
}

void on_auth_timer(void *param)
{
    CoAPContext *ctx = NULL;
    if (!is_inited) {
        return;
    }

    ctx = (CoAPContext *) param;
#ifdef ALCS_CLIENT_ENABLED
    {
        extern void on_client_auth_timer(CoAPContext *);
        on_client_auth_timer(ctx);
    }
#endif
#ifdef ALCS_SERVER_ENABLED
    {
        extern void on_svr_auth_timer(CoAPContext *);
        on_svr_auth_timer(ctx);
    }
#endif
}

int alcs_add_ctl_group(CoAPContext *context, const char *groupid, const char *accesskey, const char *accesstoken)
{
    ctl_group_item *item = NULL;
    auth_list *lst = get_list(context);
    if (!lst || lst->ctl_group_count >= KEY_MAXCOUNT) {
        return COAP_ERROR_INVALID_LENGTH;
    }

    item = (ctl_group_item *) coap_malloc(sizeof(ctl_group_item));
    if (!item) {
        return COAP_ERROR_MALLOC;
    }
    memset(item, 0, sizeof(ctl_group_item));

    do {
        item->id = (char *) coap_malloc(strlen(groupid) + 1);
        if (!item->id) {
            break;
        }

        item->accessKey = (char *) coap_malloc(strlen(accesskey) + 1);
        if (!item->accessKey) {
            break;
        }

        item->accessToken = (char *) coap_malloc(strlen(accesstoken) + 1);
        if (!item->accessToken) {
            break;
        }

        strcpy(item->accessKey, accesskey);
        strcpy(item->accessToken, accesstoken);
        strcpy(item->id, groupid);

        HAL_MutexLock(lst->list_mutex);
        list_add_tail(&item->lst, &lst->lst_ctl_group);
        ++lst->ctl_group_count;
        HAL_MutexUnlock(lst->list_mutex);

        return 0;

    } while (0);

    if (item->id) {
        coap_free(item->id);
    }
    if (item->accessKey) {
        coap_free(item->accessKey);
    }
    if (item->accessToken) {
        coap_free(item->accessToken);
    }
    coap_free(item);

    return COAP_ERROR_MALLOC;
}

int alcs_remove_ctl_group(CoAPContext *context, const char *groupid)
{
    return 0;
}

int alcs_add_svr_group(CoAPContext *context, const char *groupid, const char *keyprefix, const char *secret)
{
    svr_group_item *item = NULL;
    auth_list *lst = get_list(context);
    if (!lst || lst->svr_group_count >= KEY_MAXCOUNT) {
        return COAP_ERROR_INVALID_LENGTH;
    }

    item = (svr_group_item *) coap_malloc(sizeof(svr_group_item));
    if (!item) {
        return COAP_ERROR_MALLOC;
    }
    memset(item, 0, sizeof(svr_group_item));

    do {
        item->id = (char *) coap_malloc(strlen(groupid) + 1);
        if (!item->id) {
            break;
        }

        item->keyInfo.secret = (char *) coap_malloc(strlen(secret) + 1);
        if (!item->keyInfo.secret) {
            break;
        }

        strncpy(item->keyInfo.keyprefix, keyprefix, sizeof(item->keyInfo.keyprefix) - 1);
        strcpy(item->keyInfo.secret, secret);
        strcpy(item->id, groupid);

        HAL_MutexLock(lst->list_mutex);
        list_add_tail(&item->lst, &lst->lst_svr_group);
        ++lst->svr_group_count;
        HAL_MutexUnlock(lst->list_mutex);

        return 0;

    } while (0);

    if (item->id) {
        coap_free(item->id);
    }
    if (item->keyInfo.secret) {
        coap_free(item->keyInfo.secret);
    }
    coap_free(item);

    return COAP_ERROR_MALLOC;
}

int alcs_remove_svr_group(CoAPContext *context, const char *groupid)
{
    return 0;
}

void alcs_utils_md5_hexstr(unsigned char input[16], unsigned char output[32])
{
    unsigned char idx = 0;
    unsigned char output_char = 0;

    for (idx = 0; idx < 16; idx++) {
        if (((input[idx] >> 4) & 0x0F) <= 0x09) {
            output_char = ((input[idx] >> 4) & 0x0F) + '0';
        } else if (((input[idx] >> 4) & 0x0F) >= 0x0A) {
            output_char = ((input[idx] >> 4) & 0x0F) + 'a' - 0x0A;
        }
        output[2 * idx] = output_char;

        if (((input[idx]) & 0x0F) <= 0x09) {
            output_char = ((input[idx]) & 0x0F) + '0';
        } else if (((input[idx]) & 0x0F) >= 0x0A) {
            output_char = ((input[idx]) & 0x0F) + 'a' - 0x0A;
        }
        output[2 * idx + 1] = output_char;
    }
}