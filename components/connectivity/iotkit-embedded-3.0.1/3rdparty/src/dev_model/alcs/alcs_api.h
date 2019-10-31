/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "alcs_coap.h"

#ifndef __ALCS_API_H__
#define __ALCS_API_H__

#define SESSIONID_LEN 8
#define SESSIONKEY_MAXLEN 30

#define ALCS_ERR_AUTH_BASE                   (COAP_ERROR_BASE | 100)
#define ALCS_ERR_AUTH_AUTHING                (ALCS_ERR_AUTH_BASE | 1)
#define ALCS_ERR_AUTH_NOCTLKEY               (ALCS_ERR_AUTH_BASE | 2)
#define ALCS_ERR_AUTH_UNAUTH                 (ALCS_ERR_AUTH_BASE | 3)
#define ALCS_ERR_ENCRYPT_FAILED              (ALCS_ERR_AUTH_BASE | 5)

typedef enum {
    ALCS_AUTH_OK       = 200,
    ALCS_AUTH_REVOCATE = 501,
    ALCS_AUTH_UNMATCHPREFIX,
    ALCS_AUTH_INVALIDPARAM,
    ALCS_AUTH_AUTHLISTEMPTY,
    ALCS_AUTH_VERNOTSUPPORT,
    ALCS_AUTH_ILLEGALSIGN,
    ALCS_HEART_FAILAUTH,
} Auth_Result_Code;

#include "iotx_alcs_config.h"

typedef struct {
    int code;
    char *msg;/* MUST call coap_free to free memory */
} ResponseMsg;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
typedef struct
{
    ResponseMsg msg;
    char sessionId [SESSIONID_LEN];
    char sessionKey[SESSIONKEY_MAXLEN];
    NetworkAddr addr;
} AuthResult;
*/

typedef void (*AuthHandler)(CoAPContext *context, NetworkAddr *addr, void *user_data, ResponseMsg *result);
typedef struct {
    char *productKey;
    char *deviceName;
    char *accessKey;
    char *accessToken;
    void *user_data;
    AuthHandler handler;
} AuthParam;

typedef struct {
    NetworkAddr addr;
    char *pk;
    char *dn;
} AlcsDeviceKey;

/*  初始化认证模块
 *  context：   为当前设备生成的CoAPContext对象指针
 *  productKey：当前设备的productKey，可以为空
 *  deviceName： 当前设备的deviceName，可以为空
 *  role: 1 --client
 *        2 --server
 *        3 --client&server
 */
int alcs_auth_init(CoAPContext *context, const char *productKey, const char *deviceName, char role);
void alcs_auth_subdev_init(CoAPContext *ctx, const char *productKey, const char *deviceName);
void alcs_auth_deinit(void);

bool alcs_is_auth(CoAPContext *ctx, AlcsDeviceKey *devKey);
int alcs_sendmsg_secure(CoAPContext *ctx, AlcsDeviceKey *devKey, CoAPMessage *message, char observe,
                        CoAPSendMsgHandler handler);
int alcs_sendrsp_secure(CoAPContext *ctx, AlcsDeviceKey *devKey, CoAPMessage *message, char observe,
                        unsigned short msgid, CoAPLenString *token);

#ifdef ALCS_CLIENT_ENABLED
/*  身份认证--  直接传入accesskey&accesstoken
 *  context：   当前设备生成的CoAPContext对象指针
 *  addr：      待连设备地址
 *  auth_param：包含待连设备的信息和回调接口
 */
void alcs_auth_has_key(CoAPContext *ctx, NetworkAddr *addr, AuthParam *auth_param);

/*  身份认证--通过productkey&devicename在缓存的accesskey列表中查找合适accesskey
 *  此函数需要和alcs_add_client_key 配合使用
 *  若不知道准确的accessKey，认证前client会和server协商合适的accessKey
 *
 *  context：   为当前设备生成的CoAPContext对象指针
 *  addr：      待连设备地址
 *  productKey：待连设备的productKey
 *  deviceName：待连设备的deviceName
 *  handler：   结果回调接口
 */
void alcs_auth_nego_key(CoAPContext *ctx, AlcsDeviceKey *devKey, AuthHandler handler);
/*
 *
 *
 */
int alcs_add_client_key(CoAPContext *context, const char *accesskey, const char *accesstoken, const char *productKey,
                        const char *deviceName);
int alcs_remove_client_key(CoAPContext *context, const char *key, char isfullkey);
/*
 *
 *
 */
bool alcs_device_online(CoAPContext *context, AlcsDeviceKey *devKey);

#endif

#ifdef ALCS_SERVER_ENABLED
typedef enum {
    LOCALDEFAULT,
    LOCALSETUP,
    FROMCLOUDSVR
} ServerKeyPriority;

int alcs_add_svr_key(CoAPContext *context, const char *keyprefix, const char *secret, ServerKeyPriority priority);
int alcs_remove_svr_key(CoAPContext *context, const char *keyprefix);
/*  设置吊销列表*
*  context：   为当前设备生成的CoAPContext对象指针
*  seqlist：   吊销列表字符串，每个被吊销设备占用三字节
*/
int alcs_set_revocation(CoAPContext *context, const char *seqlist);
#endif

int alcs_add_ctl_group(CoAPContext *context, const char *groupid, const char *accesskey, const char *accesstoken);
int alcs_remove_ctl_group(CoAPContext *context, const char *groupid);

int alcs_add_svr_group(CoAPContext *context, const char *groupid, const char *keyprefix, const char *secret);
int alcs_remove_svr_group(CoAPContext *context, const char *groupid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
