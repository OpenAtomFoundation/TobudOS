/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __ALCS_API_INTERNAL_H__
#define __ALCS_API_INTERNAL_H__
#include "CoAPExport.h"
#include "alcs_api.h"
#include "alcs_internal.h"

#define KEY_MAXCOUNT 10
#define RANDOMKEY_LEN 16
#define KEYSEQ_LEN 3
#define COAP_OPTION_SESSIONID 71

#ifdef ALCS_CLIENT_ENABLED
typedef struct {
    char             *accessKey;
    char             *accessToken;
    char             *deviceName;
    char             *productKey;
    struct list_head  lst;
} ctl_key_item;
#endif

#ifdef ALCS_SERVER_ENABLED

typedef struct {
    char              keyprefix[KEYPREFIX_LEN + 1];
    char             *secret;
    ServerKeyPriority priority;
} svr_key_info;

typedef struct {
    svr_key_info keyInfo;
    struct list_head  lst;
} svr_key_item;

typedef struct {
    char *id;
    char *revocation;
    svr_key_info keyInfo;
    struct list_head  lst;
} svr_group_item;
#endif

typedef struct {
    char *id;
    char *accessKey;
    char *accessToken;
    struct list_head  lst;
} ctl_group_item;

typedef struct {
    void                    *list_mutex;
#ifdef ALCS_CLIENT_ENABLED
    struct list_head         lst_ctl;
    unsigned char            ctl_count;
#endif
#ifdef ALCS_SERVER_ENABLED
    struct list_head         lst_svr;
    unsigned char            svr_count;
    char                    *revocation;
#endif
    struct list_head         lst_ctl_group;
    int                      ctl_group_count;
    struct list_head         lst_svr_group;
    int                      svr_group_count;
} auth_list;

#define PK_DN_CHECKSUM_LEN 6
typedef struct {
    char randomKey[RANDOMKEY_LEN + 1];
    int sessionId;
    char sessionKey[32];
    int authed_time;
    int heart_time;
    int interval;
    NetworkAddr addr;
    char pk_dn[PK_DN_CHECKSUM_LEN];
    struct list_head  lst;
} session_item;

#define ROLE_SERVER 2
#define ROLE_CLIENT 1

typedef struct {
    CoAPContext *context;
    int seq;
    auth_list lst_auth;
#ifdef ALCS_SERVER_ENABLED
    struct list_head lst_svr_sessions;
#endif
#ifdef ALCS_CLIENT_ENABLED
    struct list_head lst_ctl_sessions;
#endif
    char role;
    struct list_head lst;
} device_auth_list;

#ifdef SUPPORT_MULTI_DEVICES
    extern struct list_head device_list;

    device_auth_list *get_device(CoAPContext *context);

    auth_list *get_list(CoAPContext *context);

    #ifdef ALCS_CLIENT_ENABLED
        struct list_head *get_ctl_session_list(CoAPContext *context);
    #endif

    #ifdef ALCS_SERVER_ENABLED
        struct list_head *get_svr_session_list(CoAPContext *context);
    #endif

#else
    extern device_auth_list _device;
    #define get_device(v) (&_device)

    #ifdef ALCS_SERVER_ENABLED
        #define get_svr_session_list(v) (_device.role&ROLE_SERVER? &_device.lst_svr_sessions : NULL)
    #endif
    #ifdef ALCS_CLIENT_ENABLED
        #define get_ctl_session_list(v) (_device.role&ROLE_CLIENT? &_device.lst_ctl_sessions : NULL)
    #endif

    #define get_list(v) (&_device.lst_auth)
#endif

void remove_session(CoAPContext *ctx, session_item *session);

#ifdef ALCS_CLIENT_ENABLED
    session_item *get_ctl_session(CoAPContext *ctx, AlcsDeviceKey *key);
#endif

#ifdef ALCS_SERVER_ENABLED
session_item *get_svr_session(CoAPContext *ctx, AlcsDeviceKey *key);
session_item *get_session_by_checksum(struct list_head *sessions, NetworkAddr *addr, char ck[PK_DN_CHECKSUM_LEN]);

#define MAX_PATH_CHECKSUM_LEN (5)
typedef struct {
    char              path[MAX_PATH_CHECKSUM_LEN];
    char              pk_dn[PK_DN_CHECKSUM_LEN];
    char              *filter_path;
    path_type_t       path_type;
    CoAPRecvMsgHandler cb;
    struct list_head   lst;
} secure_resource_cb_item;

extern struct list_head secure_resource_cb_head;
#endif

int alcs_encrypt(const char *src, int len, const char *key, void *out);
int alcs_decrypt(const char *src, int len, const char *key, void *out);
int observe_data_encrypt(CoAPContext *ctx, const char *paths, NetworkAddr *addr,
                         CoAPMessage *message, CoAPLenString *src, CoAPLenString *dest);

bool is_networkadd_same(NetworkAddr *addr1, NetworkAddr *addr2);
void gen_random_key(unsigned char random[], int len);
bool req_payload_parser(const char *payload, int len, char **seq, int *seqlen, char **data, int *datalen);
int internal_secure_send(CoAPContext *ctx, session_item *session, NetworkAddr *addr,
                         CoAPMessage *message, char observe, CoAPSendMsgHandler handler);

int alcs_resource_register_secure(CoAPContext *context, const char *pk, const char *dn, const char *path,
                                  unsigned short permission,
                                  unsigned int ctype, unsigned int maxage, CoAPRecvMsgHandler callback);
void alcs_resource_cb_deinit(void);
void alcs_auth_list_deinit(void);
void alcs_utils_md5_hexstr(unsigned char input[16], unsigned char output[32]);

#endif
