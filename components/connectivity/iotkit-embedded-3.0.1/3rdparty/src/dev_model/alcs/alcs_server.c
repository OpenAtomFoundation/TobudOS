/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "alcs_internal.h"
#include "alcs_api_internal.h"
#include "CoAPPlatform.h"
#include "CoAPResource.h"
#ifdef LOG_REPORT_TO_CLOUD
    #include "iotx_log_report.h"
#endif
#define RES_FORMAT "{\"id\":\"%.*s\",\"code\":%d,\"data\":{%s}}"

#ifdef ALCS_SERVER_ENABLED

int sessionid_seed = 0xff;
static int default_heart_expire = 120000;

void utils_hmac_sha1_base64(const char *msg, int msg_len, const char *key, int key_len, char *digest, int *digest_len)
{
    char buf[20];
    uint32_t outlen;
    utils_hmac_sha1_hex(msg, msg_len, buf, key, key_len);

    utils_base64encode((unsigned char *)buf, 20, *digest_len, (unsigned char *)digest, &outlen);
    *digest_len = outlen;
}

void alcs_rec_auth_select(CoAPContext *ctx, const char *paths, NetworkAddr *from, CoAPMessage *resMsg)
{
    int seqlen, datalen;
    char *seq, *data;
    char *targetKey = "";
    int targetLen = 0;
    auth_list *lst = NULL;
    char *accesskeys;
    int keylen;
    char back;
    char *str_pos, *entry;
    int entry_len, type;
    CoAPMessage msg;
    char keybuf[32];
    char payloadbuf[512];
    CoAPLenString token = {resMsg->header.tokenlen, resMsg->token};
    CoAPLenString payload;

    /* int res_code = 200; */
    COAP_DEBUG("receive data:%.*s", resMsg->payloadlen, resMsg->payload);

    do {

        if (!req_payload_parser((const char *)resMsg->payload, resMsg->payloadlen, &seq, &seqlen, &data, &datalen)) {
            break;
        }
        lst = get_list(ctx);

        accesskeys = json_get_value_by_name(data, datalen, "accessKeys", &keylen, NULL);
        if (!accesskeys || !keylen) {
            break;
        }
        COAP_DEBUG("accessKeys:%.*s", keylen, accesskeys);

        backup_json_str_last_char(accesskeys, keylen, back);
        json_array_for_each_entry(accesskeys, keylen, str_pos, entry, entry_len, type) {
            svr_key_item *node = NULL, *next = NULL;
            svr_group_item *gnode = NULL, *gnext = NULL;
            COAP_DEBUG("entry:%.*s", entry_len, entry);
            list_for_each_entry_safe(node, next, &lst->lst_svr, lst, svr_key_item) {
                COAP_DEBUG("keyprefix:%s", node->keyInfo.keyprefix);
                if (strstr(entry, node->keyInfo.keyprefix) == entry) {
                    COAP_DEBUG("target keyprefix:%s", entry);
                    targetKey = entry;
                    targetLen = entry_len;
                    break;
                }
            }
            if (targetKey) {
                break;
            }

            list_for_each_entry_safe(gnode, gnext, &lst->lst_svr_group, lst, svr_group_item) {
                COAP_DEBUG("keyprefix:%s", gnode->keyInfo.keyprefix);
                if (strstr(entry, gnode->keyInfo.keyprefix) == entry) {
                    COAP_DEBUG("target keyprefix:%s", entry);
                    targetKey = entry;
                    targetLen = entry_len;
                    break;
                }
            }
            if (targetKey) {
                break;
            }
        }
        restore_json_str_last_char(accesskeys, keylen, back);

    } while (0);

    COAP_DEBUG("key:%s", targetKey);


    HAL_Snprintf(keybuf, sizeof(keybuf), "\"accessKey\":\"%.*s\"", targetLen, targetKey);

    HAL_Snprintf(payloadbuf, sizeof(payloadbuf), RES_FORMAT, seqlen, seq, targetKey ? 200 : COAP_MSG_CODE_401_UNAUTHORIZED,
                 keybuf);
    payload.data = (unsigned char *)payloadbuf;
    payload.len = strlen(payloadbuf);

    alcs_msg_init(ctx, &msg, COAP_MSG_CODE_205_CONTENT, COAP_MESSAGE_TYPE_ACK, 0, &payload, NULL);
    alcs_sendrsp(ctx, from, &msg, 1, resMsg->header.msgid, &token);
}

svr_key_info *is_legal_key(CoAPContext *ctx, const char *keyprefix, int prefixlen, const char *keyseq, int seqlen,
                           int *res_code)
{
    auth_list *lst = get_list(ctx);
    COAP_INFO("islegal prefix:%.*s, seq:%.*s", prefixlen, keyprefix, seqlen, keyseq);

    if (lst) {
        COAP_DEBUG("find devices");
        HAL_MutexLock(lst->list_mutex);

        if (lst->revocation) {
            int len = strlen(lst->revocation);
            int i;
            for (i = 0; i < len; i += KEYSEQ_LEN) {
                if (strncmp(keyseq, lst->revocation + i, seqlen) == 0) {
                    HAL_MutexUnlock(lst->list_mutex);
                    *res_code = ALCS_AUTH_REVOCATE;
                    COAP_INFO("accesskey is revocated");
                    return NULL;
                }
            }
        }

        if (list_empty(&lst->lst_svr)) {
            COAP_INFO("ALCS_AUTH_AUTHLISTEMPTY:%d\r\n", ALCS_AUTH_AUTHLISTEMPTY);
            *res_code = ALCS_AUTH_AUTHLISTEMPTY;
        } else {
            svr_key_item *node = NULL, *next = NULL;
            svr_group_item *gnode = NULL, *gnext = NULL;
            list_for_each_entry_safe(node, next, &lst->lst_svr, lst, svr_key_item) {
                COAP_DEBUG("node prefix:%s", node->keyInfo.keyprefix);
                if (strlen(node->keyInfo.keyprefix) == prefixlen && strncmp(keyprefix, node->keyInfo.keyprefix, prefixlen) == 0) {
                    *res_code = ALCS_AUTH_OK;
                    HAL_MutexUnlock(lst->list_mutex);
                    return &node->keyInfo;
                }
            }
            list_for_each_entry_safe(gnode, gnext, &lst->lst_svr_group, lst, svr_group_item) {
                COAP_DEBUG("node prefix:%s", gnode->keyInfo.keyprefix);
                if (strlen(gnode->keyInfo.keyprefix) == prefixlen && strncmp(keyprefix, gnode->keyInfo.keyprefix, prefixlen) == 0) {
                    *res_code = ALCS_AUTH_OK;
                    HAL_MutexUnlock(lst->list_mutex);
                    return &gnode->keyInfo;
                }
            }

            COAP_INFO("ALCS_AUTH_UNMATCHPREFIX:%d\r\n", ALCS_AUTH_UNMATCHPREFIX);
            *res_code = ALCS_AUTH_UNMATCHPREFIX;
        }

        HAL_MutexUnlock(lst->list_mutex);
    }

    return NULL;
}

void alcs_rec_auth(CoAPContext *ctx, const char *paths, NetworkAddr *from, CoAPMessage *resMsg)
{
    int seqlen, datalen;
    char *seq, *data;
    int res_code = 200;
    char body[200] = {0};
    char *accesskey, *randomkey, *sign;
    int tmplen;
    char *keyprefix;
    char *keyseq;
    char accessToken[64];
    int tokenlen;
    int randomkeylen;
    char buf[40];
    int calc_sign_len;
    int pklen, dnlen;
    char *pk;
    char *dn;
    char tmp1;
    char tmp2;
    svr_key_info *item;
    AlcsDeviceKey devKey;
    session_item *session;
    CoAPMessage message;
    char payloadbuf[512];
    CoAPLenString payload;
    CoAPLenString token;
    COAP_INFO("receive data:%.*s, from:%s", resMsg->payloadlen, resMsg->payload, from->addr);

    do {
        if (!req_payload_parser((const char *)resMsg->payload, resMsg->payloadlen, &seq, &seqlen, &data, &datalen)) {
            break;
        }

        accesskey = json_get_value_by_name(data, datalen, "accessKey", &tmplen, NULL);
        COAP_INFO("accesskey:%.*s", tmplen, accesskey);

        if (!accesskey || tmplen != KEYPREFIX_LEN + 1 + 1 + KEYSEQ_LEN) {
            break;
        }

        keyprefix = accesskey;
        keyseq = accesskey + KEYPREFIX_LEN + 1 + 1;

        item = is_legal_key(ctx, keyprefix, KEYPREFIX_LEN, keyseq, KEYSEQ_LEN, &res_code);
        if (!item) {
            COAP_INFO("islegal return null");
            break;
        }

        tokenlen = sizeof(accessToken);
        utils_hmac_sha1_base64(accesskey, tmplen, item->secret, strlen(item->secret), accessToken, &tokenlen);

        COAP_INFO("accessToken:%.*s", tokenlen, accessToken);
        randomkey = json_get_value_by_name(data, datalen, "randomKey", &randomkeylen, NULL);
        if (!randomkey || !randomkeylen) {
            res_code = ALCS_AUTH_INVALIDPARAM;
            break;
        }

        /*calc sign, save in buf*/

        calc_sign_len = sizeof(buf);
        utils_hmac_sha1_base64(randomkey, randomkeylen, accessToken, tokenlen, buf, &calc_sign_len);

        COAP_INFO("calc randomKey:%.*s,token:%.*s,sign:%.*s", randomkeylen, randomkey, tokenlen,
                  accessToken, calc_sign_len, buf);

        sign = json_get_value_by_name(data, datalen, "sign", &tmplen, NULL);
        if (!sign || tmplen != calc_sign_len || strncmp(sign, buf, calc_sign_len)) {
            res_code = ALCS_AUTH_ILLEGALSIGN;
            break;
        }

        pk = json_get_value_by_name(data, datalen, "prodKey", &pklen, NULL);
        dn = json_get_value_by_name(data, datalen, "deviceName", &dnlen, NULL);

        if (!pk || !pklen || !dn || !dnlen) {
            res_code = ALCS_AUTH_INVALIDPARAM;
            break;
        }
        tmp1 = pk[pklen];
        tmp2 = dn[dnlen];
        pk[pklen] = 0;
        dn[dnlen] = 0;


        memset(&devKey, 0x00, sizeof(AlcsDeviceKey));
        memcpy(&devKey.addr, from, sizeof(NetworkAddr));
        devKey.pk = pk;
        devKey.dn = dn;
        session = get_svr_session(ctx, &devKey);

        if (!session) {
            char path[100] = {0};
            struct list_head *svr_head;
            session = (session_item *)coap_malloc(sizeof(session_item));
            gen_random_key((unsigned char *)session->randomKey, RANDOMKEY_LEN);
            session->sessionId = ++sessionid_seed;

            strncpy(path, pk, sizeof(path));
            strncat(path, dn, sizeof(path) - strlen(path) - 1);
            CoAPPathMD5_sum(path, strlen(path), session->pk_dn, PK_DN_CHECKSUM_LEN);

            memcpy(&session->addr, from, sizeof(NetworkAddr));
            COAP_INFO("new session, addr:%s, port:%d", session->addr.addr, session->addr.port);
            svr_head = get_svr_session_list(ctx);
            list_add_tail(&session->lst, svr_head);
        }

        pk[pklen] = tmp1;
        dn[dnlen] = tmp2;

        HAL_Snprintf(buf, sizeof(buf), "%.*s%s", randomkeylen, randomkey, session->randomKey);
        utils_hmac_sha1_hex(buf, strlen(buf), session->sessionKey, accessToken, tokenlen);

        /*calc sign, save in buf*/
        calc_sign_len = sizeof(buf);
        utils_hmac_sha1_base64(session->randomKey, RANDOMKEY_LEN, accessToken, tokenlen, buf, &calc_sign_len);
        HAL_Snprintf(body, sizeof(body), "\"sign\":\"%.*s\",\"randomKey\":\"%s\",\"sessionId\":%d,\"expire\":86400",
                     calc_sign_len, buf, session->randomKey, session->sessionId);

        session->authed_time = HAL_UptimeMs();
        session->heart_time = session->authed_time;
        /* ??? */
        /* result = 1; */

    } while (0);

    HAL_Snprintf(payloadbuf, sizeof(payloadbuf), RES_FORMAT, seqlen, seq, res_code, body);
    payload.len = strlen(payloadbuf);
    payload.data = (unsigned char *)payloadbuf;
    alcs_msg_init(ctx, &message, COAP_MSG_CODE_205_CONTENT, COAP_MESSAGE_TYPE_ACK, 0, &payload, NULL);
    token.len = resMsg->header.tokenlen;
    token.data = resMsg->token;
    alcs_sendrsp(ctx, from, &message, 1, resMsg->header.msgid, &token);
}

static int alcs_remove_low_priority_key(CoAPContext *ctx, ServerKeyPriority priority)
{
    auth_list *lst = get_list(ctx);
    svr_key_item *node = NULL, *next = NULL;

    HAL_MutexLock(lst->list_mutex);

    list_for_each_entry_safe(node, next, &lst->lst_svr, lst, svr_key_item) {
        if (node->keyInfo.priority < priority) {
            coap_free(node->keyInfo.secret);
            list_del(&node->lst);
            coap_free(node);
            --lst->svr_count;
        }
    }
    HAL_MutexUnlock(lst->list_mutex);

    return COAP_SUCCESS;
}

static int add_svr_key(CoAPContext *ctx, const char *keyprefix, const char *secret, bool isGroup,
                       ServerKeyPriority priority)
{
    auth_list *lst = get_list(ctx);
    svr_key_item *node = NULL, *next = NULL;
    svr_key_item *item;
    COAP_INFO("add_svr_key\n");
    if (!lst || lst->svr_count >= KEY_MAXCOUNT || strlen(keyprefix) != KEYPREFIX_LEN) {
        return COAP_ERROR_INVALID_LENGTH;
    }
    alcs_remove_low_priority_key(ctx, priority);

    HAL_MutexLock(lst->list_mutex);
    list_for_each_entry_safe(node, next, &lst->lst_svr, lst, svr_key_item) {
        if (node->keyInfo.priority > priority) {
            /* find high priority key */
            HAL_MutexUnlock(lst->list_mutex);
            return COAP_ERROR_UNSUPPORTED;
        }
    }

    item = (svr_key_item *) coap_malloc(sizeof(svr_key_item));
    if (!item) {
        HAL_MutexUnlock(lst->list_mutex);
        return COAP_ERROR_MALLOC;
    }
    memset(item, 0, sizeof(svr_key_item));
    item->keyInfo.secret = (char *) coap_malloc(strlen(secret) + 1);
    if (!item->keyInfo.secret) {
        HAL_MutexUnlock(lst->list_mutex);
        coap_free(item);
        return COAP_ERROR_MALLOC;
    }
    memset(item->keyInfo.secret, 0, strlen(secret) + 1);
    strcpy(item->keyInfo.secret, secret);
    memcpy(item->keyInfo.keyprefix, keyprefix, KEYPREFIX_LEN);
    item->keyInfo.priority = priority;

    list_add_tail(&item->lst, &lst->lst_svr);
    ++lst->svr_count;
    HAL_MutexUnlock(lst->list_mutex);

    return COAP_SUCCESS;
}

int alcs_add_svr_key(CoAPContext *ctx, const char *keyprefix, const char *secret, ServerKeyPriority priority)
{
    COAP_INFO("alcs_add_svr_key, priority=%d", priority);
    return add_svr_key(ctx, keyprefix, secret, 0, priority);
}


int alcs_remove_svr_key(CoAPContext *ctx, const char *keyprefix)
{
    auth_list *lst = get_list(ctx);
    svr_key_item *node = NULL, *next = NULL;

    HAL_MutexLock(lst->list_mutex);

    list_for_each_entry_safe(node, next, &lst->lst_svr, lst, svr_key_item) {
        if (strcmp(node->keyInfo.keyprefix, keyprefix) == 0) {
            coap_free(node->keyInfo.secret);
            list_del(&node->lst);
            coap_free(node);
            --lst->svr_count;
            break;
        }
    }
    HAL_MutexUnlock(lst->list_mutex);

    return COAP_SUCCESS;
}

int alcs_set_revocation(CoAPContext *ctx, const char *seqlist)
{
    auth_list *lst = get_list(ctx);
    int len;

    HAL_MutexLock(lst->list_mutex);

    len = seqlist ? strlen(seqlist) : 0;
    if (lst->revocation) {
        coap_free(lst->revocation);
        lst->revocation = NULL;
    }

    if (len > 0) {
        lst->revocation = (char *)coap_malloc(len + 1);
        strcpy(lst->revocation, seqlist);
    }
    HAL_MutexUnlock(lst->list_mutex);

    return COAP_SUCCESS;
}

/* ----------------------------------------- */

void send_err_rsp(CoAPContext *ctx, NetworkAddr *addr, int code, CoAPMessage *request)
{
    CoAPMessage sendMsg;
    CoAPLenString payload = {0};
    CoAPLenString token;
    alcs_msg_init(ctx, &sendMsg, code, COAP_MESSAGE_TYPE_ACK, 0, &payload, NULL);
    token.len = request->header.tokenlen;
    token.data = request->token;
    alcs_sendrsp(ctx, addr, &sendMsg, 1, request->header.msgid, &token);
}

void call_cb(CoAPContext *context, const char *path, NetworkAddr *remote, CoAPMessage *message, const char *key,
             char *buf, CoAPRecvMsgHandler cb)
{
    CoAPMessage tmpMsg;
    memcpy(&tmpMsg, message, sizeof(CoAPMessage));

    if (key && buf) {
        int len = alcs_decrypt((const char *)message->payload, message->payloadlen, key, buf);
        tmpMsg.payload = (unsigned char *)buf;
        tmpMsg.payloadlen = len;
#ifdef LOG_REPORT_TO_CLOUD
        get_msgid(buf, 0);
#endif
    } else {
        tmpMsg.payload = NULL;
        tmpMsg.payloadlen = 0;
    }

    cb(context, path, remote, &tmpMsg);
}

static secure_resource_cb_item *get_resource_by_path(const char *path)
{
    secure_resource_cb_item *node, *next;
    char path_calc[MAX_PATH_CHECKSUM_LEN] = {0};
    CoAPPathMD5_sum(path, strlen(path), path_calc, MAX_PATH_CHECKSUM_LEN);

    list_for_each_entry_safe(node, next, &secure_resource_cb_head, lst, secure_resource_cb_item) {
        if (node->path_type == PATH_NORMAL) {
            if (memcmp(node->path, path_calc, MAX_PATH_CHECKSUM_LEN) == 0) {
                return node;
            }
        } else if (strlen(node->filter_path) > 0) {
            if (CoAPResource_topicFilterMatch(node->filter_path, path) == 0) {
                return node;
            }
        }
    }

    COAP_ERR("receive unknown request, path:%s", path);
    return NULL;
}

void recv_msg_handler(CoAPContext *context, const char *path, NetworkAddr *remote, CoAPMessage *message)
{
    secure_resource_cb_item *node = get_resource_by_path(path);
    struct list_head *sessions;
    session_item *session;
    unsigned int obsVal;

    unsigned int sessionId = 0;
    CoAPUintOption_get(message, COAP_OPTION_SESSIONID, &sessionId);
    COAP_DEBUG("recv_msg_handler, sessionID:%d", (int)sessionId);
    if (!node) {
        return;
    }
    sessions = get_svr_session_list(context);
    session = get_session_by_checksum(sessions, remote, node->pk_dn);
    if (!session || session->sessionId != sessionId) {
        send_err_rsp(context, remote, COAP_MSG_CODE_401_UNAUTHORIZED, message);
        COAP_ERR("need auth, path:%s, from:%s", path, remote->addr);
        return;
    }

    if (CoAPUintOption_get(message, COAP_OPTION_OBSERVE, &obsVal) == COAP_SUCCESS) {
        if (obsVal == 0) {
            CoAPObsServer_add(context, path, remote, message);
        }
    }

    if (message->payloadlen < 256) {
        char buf[256];
        call_cb(context, path, remote, message, session->sessionKey, buf, node->cb);
    } else {
        char *buf = (char *)coap_malloc(message->payloadlen);
        if (buf) {
            call_cb(context, path, remote, message, session->sessionKey, buf, node->cb);
            coap_free(buf);
        }
    }
}

int alcs_resource_register_secure(CoAPContext *context, const char *pk, const char *dn, const char *path,
                                  unsigned short permission,
                                  unsigned int ctype, unsigned int maxage, CoAPRecvMsgHandler callback)
{
    secure_resource_cb_item *node = NULL, *next_node = NULL;
    char pk_dn[100] = {0};
    int dup = 0;
    secure_resource_cb_item *item;

    COAP_INFO("alcs_resource_register_secure");
    item = (secure_resource_cb_item *)coap_malloc(sizeof(secure_resource_cb_item));
    if (item == NULL) {
        return -1;
    }
    memset(item, 0, sizeof(secure_resource_cb_item));
    item->cb = callback;
    item->path_type = PATH_NORMAL;
    if (strstr(path, "/#") != NULL) {
        item->path_type = PATH_FILTER;
    } else {
        CoAPPathMD5_sum(path, strlen(path), item->path, MAX_PATH_CHECKSUM_LEN);
    }
    list_for_each_entry_safe(node, next_node, &secure_resource_cb_head, lst, secure_resource_cb_item) {
        if (item->path_type == PATH_NORMAL && node->path_type == PATH_NORMAL) {
            if (memcmp(node->path, item->path, MAX_PATH_CHECKSUM_LEN) == 0) {
                dup = 1;
            }
        } else if (item->path_type == PATH_FILTER && node->path_type == PATH_FILTER) {
            if (strncmp(node->filter_path, path, strlen(path)) == 0) {
                dup = 1;
            }
        }
    }
    if (dup == 0) {
        if (item->path_type == PATH_FILTER) {
            item->filter_path = coap_malloc(strlen(path) + 1);
            if (item->filter_path == NULL) {
                coap_free(item);
                return -1;
            }
            memset(item->filter_path, 0, strlen(path) + 1);
            strncpy(item->filter_path, path, strlen(path));
        }

        strncpy(pk_dn, pk, sizeof(pk_dn) - 1);
        strncat(pk_dn, dn, sizeof(pk_dn) - strlen(pk_dn) - 1);

        CoAPPathMD5_sum(pk_dn, strlen(pk_dn), item->pk_dn, PK_DN_CHECKSUM_LEN);

        list_add_tail(&item->lst, &secure_resource_cb_head);
    } else {
        coap_free(item);
    }

    return CoAPResource_register(context, path, permission, ctype, maxage, &recv_msg_handler);
}

void alcs_resource_cb_deinit(void)
{
    secure_resource_cb_item *del_item = NULL;

    list_for_each_entry(del_item, &secure_resource_cb_head, lst, secure_resource_cb_item) {
        list_del(&del_item->lst);
        if (del_item->path_type == PATH_FILTER) {
            coap_free(del_item->filter_path);
        }
        coap_free(del_item);
        del_item = list_entry(&secure_resource_cb_head, secure_resource_cb_item, lst);
    }
}

void alcs_auth_list_deinit(void)
{
    auth_list *auth_list_ctx = get_list(ctx);
    svr_key_item *del_item = NULL, *next_item = NULL;

    list_for_each_entry_safe(del_item, next_item, &auth_list_ctx->lst_svr, lst, svr_key_item) {
        list_del(&del_item->lst);
        if (del_item->keyInfo.secret) {
            coap_free(del_item->keyInfo.secret);
        }
        coap_free(del_item);
    }
}

void alcs_rec_heart_beat(CoAPContext *ctx, const char *path, NetworkAddr *remote, CoAPMessage *request)
{
    struct list_head *ctl_head = get_svr_session_list(ctx);
    session_item *session = NULL;
    session_item *node = NULL, *next = NULL;
    int seqlen, datalen;
    char *seq, *data;
    CoAPMessage msg;
    char databuf[32];
    char payloadbuf[128];
    CoAPLenString payload;

    COAP_DEBUG("alcs_rec_heart_beat");
    if (!ctl_head || list_empty(ctl_head)) {
        return;
    }

    list_for_each_entry_safe(node, next, ctl_head, lst, session_item) {
        if (node->sessionId && is_networkadd_same(&node->addr, remote)) {
            node->heart_time = HAL_UptimeMs();
            session = node;
        }
    }

    if (!session) {
        COAP_INFO("receive stale heart beat");
    }


    if (!req_payload_parser((const char *)request->payload, request->payloadlen, &seq, &seqlen, &data, &datalen)) {
        /* do nothing */
    }

    if (session) {
        HAL_Snprintf(databuf, sizeof(databuf), "\"delayTime\":%d", default_heart_expire / 1000);
        HAL_Snprintf(payloadbuf, sizeof(payloadbuf), RES_FORMAT, seqlen, seq, 200, databuf);
    } else {
        HAL_Snprintf(payloadbuf, sizeof(payloadbuf), RES_FORMAT, seqlen, seq, ALCS_HEART_FAILAUTH, "");
    }

    payload.data = (unsigned char *)payloadbuf;
    payload.len = strlen(payloadbuf);
    alcs_msg_init(ctx, &msg, COAP_MSG_CODE_205_CONTENT, COAP_MESSAGE_TYPE_CON, 0, &payload, NULL);
    if (session) {
        msg.header.msgid = request->header.msgid;
        msg.header.tokenlen = request->header.tokenlen;
        memcpy(&msg.token, request->token, request->header.tokenlen);
        internal_secure_send(ctx, session, remote, &msg, 1, NULL);
    } else {
        CoAPLenString token = {request->header.tokenlen, request->token};
        alcs_sendrsp(ctx, remote, &msg, 1, request->header.msgid, &token);
    }
    alcs_msg_deinit(&msg);
}

int observe_data_encrypt(CoAPContext *ctx, const char *path, NetworkAddr *from, CoAPMessage *message,
                         CoAPLenString *src, CoAPLenString *dest)
{
    secure_resource_cb_item *node = get_resource_by_path(path);
    struct list_head *sessions;
    session_item *session;
    COAP_DEBUG("observe_data_encrypt, src:%.*s", src->len, src->data);
    if (!node) {
        return COAP_ERROR_NOT_FOUND;
    }

    sessions = get_svr_session_list(ctx);
    session = get_session_by_checksum(sessions, from, node->pk_dn);

    if (session) {
        dest->len = (src->len & 0xfffffff0) + 16;
        dest->data  = (unsigned char *)coap_malloc(dest->len);
        alcs_encrypt((const char *)src->data, src->len, session->sessionKey, dest->data);
        CoAPUintOption_add(message, COAP_OPTION_SESSIONID, session->sessionId);
        return COAP_SUCCESS;
    }

    return COAP_ERROR_NOT_FOUND;
}

void on_svr_auth_timer(CoAPContext *ctx)
{
    struct list_head *head = get_svr_session_list(ctx);
    int tick;
    session_item *node = NULL, *next = NULL;

    if (!head || list_empty(head)) {
        return;
    }
    /* COAP_INFO ("on_svr_auth_timer:%d", (int)HAL_UptimeMs()); */

    /* device_auth_list* dev = get_device (ctx); */
    tick = HAL_UptimeMs();

    list_for_each_entry_safe(node, next, head, lst, session_item) {
        if (node->sessionId && node->heart_time + default_heart_expire < tick) {
            COAP_ERR("heart beat timeout");
            remove_session(ctx, node);
        }
    }
}
#endif
