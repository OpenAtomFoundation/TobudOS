#include "infra_types.h"
#include "infra_defs.h"
#include "infra_string.h"
#include "infra_list.h"
#include "infra_report.h"
#include "infra_sha256.h"
#include "infra_compat.h"

#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "mqtt_wrapper.h"

#ifdef PLATFORM_HAS_DYNMEM
    #ifdef INFRA_MEM_STATS
        #include "infra_mem_stats.h"
        #define mqtt_api_malloc(size)            LITE_malloc(size, MEM_MAGIC, "mqtt-api")
        #define mqtt_api_free(ptr)               LITE_free(ptr)
    #else
        #define mqtt_api_malloc(size)            HAL_Malloc(size)
        #define mqtt_api_free(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
    #endif

#else
    static iotx_mqtt_param_t g_iotx_mqtt_param;
#endif

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define mqtt_emerg(...)             log_emerg("MQTT", __VA_ARGS__)
    #define mqtt_crit(...)              log_crit("MQTT", __VA_ARGS__)
    #define mqtt_err(...)               log_err("MQTT", __VA_ARGS__)
    #define mqtt_warning(...)           log_warning("MQTT", __VA_ARGS__)
    #define mqtt_info(...)              log_info("MQTT", __VA_ARGS__)
    #define mqtt_debug(...)             log_debug("MQTT", __VA_ARGS__)
#else
    #define mqtt_emerg(...)             do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
    #define mqtt_crit(...)              do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
    #define mqtt_err(...)               do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
    #define mqtt_warning(...)           do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
    #define mqtt_info(...)              do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
    #define mqtt_debug(...)             do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#endif

static void        *g_mqtt_client = NULL;
iotx_sign_mqtt_t    g_default_sign;
static char         iotx_ca_crt_itls[IOTX_PRODUCT_KEY_LEN + IOTX_PRODUCT_SECRET_LEN + 2] = {0};

/* Handle structure of subscribed topic */
typedef struct  {
#ifdef PLATFORM_HAS_DYNMEM
    char *topic_filter;
#else
    char topic_filter[CONFIG_MQTT_TOPIC_MAXLEN];
#endif
    iotx_mqtt_event_handle_func_fpt handle;
    void *user_data;
    iotx_mqtt_qos_t qos;
#ifdef PLATFORM_HAS_DYNMEM
    struct list_head linked_list;
#else
    int used;
#endif
} iotx_mc_offline_subs_t;

typedef struct {
    int init;
    void *mutex;
#ifdef PLATFORM_HAS_DYNMEM
    struct list_head offline_sub_list;
#else
    iotx_mc_offline_subs_t offline_sub_list[CONFIG_MQTT_OFFLINE_TOPIC_MAXNUM];
#endif

} offline_sub_list_t;

static offline_sub_list_t g_mqtt_offline_subs_list = {0};

static int _offline_subs_list_init(void)
{
    if (g_mqtt_offline_subs_list.init) {
        return SUCCESS_RETURN;
    }

    memset(&g_mqtt_offline_subs_list, 0, sizeof(offline_sub_list_t));
    g_mqtt_offline_subs_list.init = 1;

#ifdef PLATFORM_HAS_DYNMEM
    INIT_LIST_HEAD(&g_mqtt_offline_subs_list.offline_sub_list);
#endif

    g_mqtt_offline_subs_list.mutex = HAL_MutexCreate();

    return SUCCESS_RETURN;
}

static int _offline_subs_list_deinit(void)
{
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_offline_subs_t *node = NULL, *next_node = NULL;
    list_for_each_entry_safe(node, next_node, &g_mqtt_offline_subs_list.offline_sub_list, linked_list,
                             iotx_mc_offline_subs_t) {
        list_del(&node->linked_list);
        mqtt_api_free(node->topic_filter);
        mqtt_api_free(node);
    }
#endif

    if (g_mqtt_offline_subs_list.mutex) {
        HAL_MutexDestroy(g_mqtt_offline_subs_list.mutex);
    }
    memset(&g_mqtt_offline_subs_list, 0, sizeof(offline_sub_list_t));

    return 0;
}

static int iotx_mqtt_offline_subscribe(const char *topic_filter, iotx_mqtt_qos_t qos,
                                       iotx_mqtt_event_handle_func_fpt topic_handle_func, void *pcontext)
{
    int ret;
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_offline_subs_t *sub_info = NULL;
#else
    int idx = 0;
#endif

    if (topic_filter == NULL || topic_handle_func == NULL) {
        return NULL_VALUE_ERROR;
    }

    _offline_subs_list_init();

#ifdef PLATFORM_HAS_DYNMEM
    HAL_MutexLock(g_mqtt_offline_subs_list.mutex);
    list_for_each_entry(sub_info, &g_mqtt_offline_subs_list.offline_sub_list, linked_list, iotx_mc_offline_subs_t) {
        if ((strlen(sub_info->topic_filter) == strlen(topic_filter)) &&
            memcmp(sub_info->topic_filter, topic_filter, strlen(topic_filter)) == 0) {
            sub_info->qos = qos;
            sub_info->handle = topic_handle_func;
            sub_info->user_data = pcontext;
            HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
            return SUCCESS_RETURN;
        }
    }

    sub_info = mqtt_api_malloc(sizeof(iotx_mc_offline_subs_t));
    if (sub_info == NULL) {
        HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
        return ERROR_MALLOC;
    }

    memset(sub_info, 0, sizeof(iotx_mc_offline_subs_t));
    sub_info->topic_filter = mqtt_api_malloc(strlen(topic_filter) + 1);
    if (sub_info->topic_filter == NULL) {
        mqtt_api_free(sub_info);
        HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
        return ERROR_MALLOC;
    }
    memset(sub_info->topic_filter, 0, strlen(topic_filter) + 1);
    strncpy(sub_info->topic_filter, topic_filter, strlen(topic_filter));
    sub_info->qos = qos;
    sub_info->handle = topic_handle_func;
    sub_info->user_data = pcontext;
    INIT_LIST_HEAD(&sub_info->linked_list);

    list_add_tail(&sub_info->linked_list, &g_mqtt_offline_subs_list.offline_sub_list);
    HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
    ret = SUCCESS_RETURN;
#else
    if (strlen(topic_filter) >= CONFIG_MQTT_TOPIC_MAXLEN) {
        return MQTT_TOPIC_LEN_TOO_SHORT;
    }

    HAL_MutexLock(g_mqtt_offline_subs_list.mutex);
    for (idx = 0; idx < CONFIG_MQTT_OFFLINE_TOPIC_MAXNUM; idx++) {
        if (g_mqtt_offline_subs_list.offline_sub_list[idx].used &&
            (strlen(g_mqtt_offline_subs_list.offline_sub_list[idx].topic_filter) == strlen(topic_filter)) &&
            memcmp(g_mqtt_offline_subs_list.offline_sub_list[idx].topic_filter, topic_filter, strlen(topic_filter)) == 0) {
            g_mqtt_offline_subs_list.offline_sub_list[idx].qos = qos;
            g_mqtt_offline_subs_list.offline_sub_list[idx].handle = topic_handle_func;
            g_mqtt_offline_subs_list.offline_sub_list[idx].user_data = pcontext;
            HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
            return SUCCESS_RETURN;
        }
    }
    for (idx = 0; idx < CONFIG_MQTT_OFFLINE_TOPIC_MAXNUM; idx++) {
        if (g_mqtt_offline_subs_list.offline_sub_list[idx].used == 0) {
            memset(&g_mqtt_offline_subs_list.offline_sub_list[idx], 0, sizeof(iotx_mc_offline_subs_t));
            memcpy(g_mqtt_offline_subs_list.offline_sub_list[idx].topic_filter, topic_filter, strlen(topic_filter));
            g_mqtt_offline_subs_list.offline_sub_list[idx].qos = qos;
            g_mqtt_offline_subs_list.offline_sub_list[idx].handle = topic_handle_func;
            g_mqtt_offline_subs_list.offline_sub_list[idx].user_data = pcontext;
            g_mqtt_offline_subs_list.offline_sub_list[idx].used = 1;
            HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
            return SUCCESS_RETURN;
        }
    }
    HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);
    ret = MQTT_OFFLINE_LIST_LEN_TOO_SHORT;
#endif

    return ret;
}

static int iotx_mqtt_deal_offline_subs(void *client)
{
#ifdef PLATFORM_HAS_DYNMEM
    iotx_mc_offline_subs_t *node = NULL, *next_node = NULL;
#else
    int idx;
#endif
    if (g_mqtt_offline_subs_list.init == 0) {
        return SUCCESS_RETURN;
    }

    HAL_MutexLock(g_mqtt_offline_subs_list.mutex);
#ifdef PLATFORM_HAS_DYNMEM
    list_for_each_entry_safe(node, next_node, &g_mqtt_offline_subs_list.offline_sub_list, linked_list,
                             iotx_mc_offline_subs_t) {
        list_del(&node->linked_list);
        wrapper_mqtt_subscribe(client, node->topic_filter, node->qos, node->handle, node->user_data);
        mqtt_api_free(node->topic_filter);
        mqtt_api_free(node);
    }
#else
    for (idx = 0; idx < CONFIG_MQTT_OFFLINE_TOPIC_MAXNUM; idx++) {
        if (g_mqtt_offline_subs_list.offline_sub_list[idx].used) {
            wrapper_mqtt_subscribe(client, g_mqtt_offline_subs_list.offline_sub_list[idx].topic_filter,
                                   g_mqtt_offline_subs_list.offline_sub_list[idx].qos,
                                   g_mqtt_offline_subs_list.offline_sub_list[idx].handle,
                                   g_mqtt_offline_subs_list.offline_sub_list[idx].user_data);
            g_mqtt_offline_subs_list.offline_sub_list[idx].used = 0;
        }
    }
#endif
    HAL_MutexUnlock(g_mqtt_offline_subs_list.mutex);

    _offline_subs_list_deinit();

    return SUCCESS_RETURN;
}

static void iotx_mqtt_report_funcs(void *pclient)
{
    int                 err;

    iotx_mqtt_deal_offline_subs(pclient);

#ifndef ATHOST_MQTT_REPORT_DISBALED
    iotx_set_report_func(IOT_MQTT_Publish_Simple);
    /* report module id */
    err = iotx_report_mid(pclient);
    if (SUCCESS_RETURN != err) {
#ifdef DEBUG_REPORT_MID_DEVINFO_FIRMWARE
        mqtt_err("failed to report mid");
#endif
    }

    /* report device info */
    err = iotx_report_devinfo(pclient);
    if (SUCCESS_RETURN != err) {
#ifdef DEBUG_REPORT_MID_DEVINFO_FIRMWARE
        mqtt_err("failed to report devinfo");
#endif
    }

    /* report firmware version */
#if !defined(BUILD_AOS) && !defined(MUTE_VERSION_REPORT)
    err = iotx_report_firmware_version(pclient);

    if (SUCCESS_RETURN != err) {
#ifdef DEBUG_REPORT_MID_DEVINFO_FIRMWARE
        mqtt_err("failed to report firmware version");
#endif
    }
#endif

#endif
}

#ifdef DYNAMIC_REGISTER
#include "dynreg_api.h"
int HAL_SetDeviceSecret(char *device_secret);
int HAL_GetProductSecret(char *product_secret);
int HAL_Kv_Set(const char *key, const void *val, int len, int sync);
int HAL_Kv_Get(const char *key, void *val, int *buffer_len);

#define DYNAMIC_REG_KV_PREFIX       "DYNAMIC_REG_"
#define DYNAMIC_REG_KV_PREFIX_LEN   12

static int _iotx_dynamic_register(iotx_http_region_types_t region, iotx_dev_meta_info_t *meta_info)
{
    char device_secret_kv[IOTX_DEVICE_SECRET_LEN + 1] = {0};
    int device_secret_len = IOTX_DEVICE_SECRET_LEN;
    char kv_key[IOTX_DEVICE_NAME_LEN + DYNAMIC_REG_KV_PREFIX_LEN] = DYNAMIC_REG_KV_PREFIX;
    int res = FAIL_RETURN;

    memcpy(kv_key + strlen(kv_key), meta_info->device_name, strlen(meta_info->device_name));

    /* Check if Device Secret exist in KV */
    if (HAL_Kv_Get(kv_key, device_secret_kv, &device_secret_len) == 0) {
        mqtt_info("Get DeviceSecret from KV succeed");

        *(device_secret_kv + device_secret_len) = 0;
        HAL_SetDeviceSecret(device_secret_kv);
        memset(meta_info->device_secret, 0, IOTX_DEVICE_SECRET_LEN + 1);
        memcpy(meta_info->device_secret, device_secret_kv, strlen(device_secret_kv));
    } else {
        char product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = {0};

        /* KV not exit, goto dynamic register */
        mqtt_info("DeviceSecret KV not exist, Now We Need Dynamic Register...");

        res = IOT_Dynamic_Register(region, meta_info);
        if (res != SUCCESS_RETURN) {
            mqtt_err("Dynamic Register Failed");
            return FAIL_RETURN;
        }

        device_secret_len = strlen(meta_info->device_secret);
        if (HAL_Kv_Set(kv_key, meta_info->device_secret, device_secret_len, 1) != 0) {
            mqtt_err("Save Device Secret to KV Failed");
            return FAIL_RETURN;
        }

        HAL_SetDeviceSecret(meta_info->device_secret);
    }

    return SUCCESS_RETURN;
}
#endif /* #ifdef DYNAMIC_REGISTER */

#ifdef MQTT_PRE_AUTH
#include "infra_preauth.h"
extern int _iotx_generate_sign_string(const char *device_id, const char *device_name, const char *product_key,
                                      const char *device_secret, char *sign_string);

static int _iotx_preauth(iotx_mqtt_region_types_t region, iotx_dev_meta_info_t *meta,
                         iotx_sign_mqtt_t *preauth_out)
{
    uint16_t length = 0;
    char device_id[IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 1] = {0};
    char sign_string[65] = {0};
    int res;

    memset(preauth_out, 0, sizeof(iotx_sign_mqtt_t));

    /* setup device_id */
    memcpy(device_id, meta->product_key, strlen(meta->product_key));
    memcpy(device_id + strlen(device_id), ".", strlen("."));
    memcpy(device_id + strlen(device_id), meta->device_name, strlen(meta->device_name));

    /* setup sign_string */
    res = _iotx_generate_sign_string(device_id, meta->device_name, meta->product_key, meta->device_secret, sign_string);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    return preauth_get_connection_info(region, meta, sign_string, device_id, preauth_out);
}
#endif /* #ifdef MQTT_PRE_AUTH */

extern int _sign_get_clientid(char *clientid_string, const char *device_id, const char *custom_kv, uint8_t enable_itls);

/************************  Public Interface ************************/
void *IOT_MQTT_Construct(iotx_mqtt_param_t *pInitParams)
{
    void *pclient;
    iotx_dev_meta_info_t meta_info;
    iotx_mqtt_param_t mqtt_params;
    char device_id[IOTX_PRODUCT_KEY_LEN + IOTX_DEVICE_NAME_LEN + 1] = {0};
    int region = 0;
    int dynamic = 0;
    uint8_t enalbe_itls = 0;
    int ret;
    void *callback;

    if (g_mqtt_client != NULL) {
        mqtt_err("Already exist default MQTT connection, won't proceed another one");
        return g_mqtt_client;
    }

    /* get region */
    IOT_Ioctl(IOTX_IOCTL_GET_REGION, (void *)&region);

    /* get dynamic option */
    IOT_Ioctl(IOTX_IOCTL_GET_DYNAMIC_REGISTER, (void *)&dynamic);

    /* get meta_info from hal */
    memset(&meta_info, 0, sizeof(iotx_dev_meta_info_t));
    HAL_GetProductKey(meta_info.product_key);
    HAL_GetDeviceName(meta_info.device_name);

    if (meta_info.product_key[0] == '\0' || meta_info.product_key[IOTX_PRODUCT_KEY_LEN] != '\0') {
        mqtt_err("Invalid product key, abort!");
        return NULL;
    }
    if (meta_info.device_name[0] == '\0' || meta_info.device_name[IOTX_DEVICE_NAME_LEN] != '\0') {
        mqtt_err("Invalid device name, abort!");
        return NULL;
    }

#ifdef DYNAMIC_REGISTER /* get device secret through https dynamic register */
    if (dynamic) {
        HAL_GetProductSecret(meta_info.product_secret);
        if (meta_info.product_secret[0] == '\0' || meta_info.product_secret[IOTX_PRODUCT_SECRET_LEN] != '\0') {
            mqtt_err("Product Secret doesn't exist");
            return NULL;
        }

        ret = _iotx_dynamic_register(region, &meta_info);
        if (ret < SUCCESS_RETURN) {
            mqtt_err("ret = _iotx_dynamic_register() = %d, abort", ret);
            return NULL;
        }
    } else {
        HAL_GetDeviceSecret(meta_info.device_secret);
        if (meta_info.device_secret[0] == '\0' || meta_info.device_secret[IOTX_DEVICE_SECRET_LEN] != '\0') {
            mqtt_err("Invalid device secret, abort!");
            return NULL;
        }
    }
#else /* get device secret from hal */
    HAL_GetDeviceSecret(meta_info.device_secret);
    if (meta_info.device_secret[0] == '\0' || meta_info.device_secret[IOTX_DEVICE_SECRET_LEN] != '\0') {
        mqtt_err("Invalid device secret, abort!");
        return NULL;
    }
#endif /* #ifdef DYNAMIC_REGISTER */

#ifdef MQTT_PRE_AUTH /* preauth mode through https */
    ret = _iotx_preauth(region, &meta_info, (iotx_sign_mqtt_t *)&g_default_sign); /* type convert */
    if (ret < SUCCESS_RETURN) {
        mqtt_err("ret = _iotx_preauth() = %d, abort", ret);
        return NULL;
    }
#else /* direct mode */
    ret = IOT_Sign_MQTT(region, &meta_info, &g_default_sign);
    if (ret < SUCCESS_RETURN) {
        mqtt_err("ret = IOT_Sign_MQTT() = %d, abort", ret);
        return NULL;
    }
#endif /* #ifdef MQTT_PRE_AUTH */

    /* setup device_id */
    memcpy(device_id, meta_info.product_key, strlen(meta_info.product_key));
    memcpy(device_id + strlen(device_id), ".", strlen("."));
    memcpy(device_id + strlen(device_id), meta_info.device_name, strlen(meta_info.device_name));

    /* reconfig clientid, append custome clientKV and itls switch flag */
    if (pInitParams != NULL && pInitParams->customize_info != NULL) {
        if (strstr(pInitParams->customize_info, "authtype=id2") != NULL) {
            enalbe_itls = 1;
        }
        else {
            enalbe_itls = 0;
        }
    }

    if (_sign_get_clientid(g_default_sign.clientid, device_id,
                           (pInitParams != NULL) ? pInitParams->customize_info : NULL, enalbe_itls) != SUCCESS_RETURN) {
        return NULL;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(iotx_mqtt_param_t));

#ifdef SUPPORT_TLS
    {
        extern const char *iotx_ca_crt;
        if (enalbe_itls == 0) {
            mqtt_params.pub_key = iotx_ca_crt;
        }
        else {
            memset(iotx_ca_crt_itls, 0, sizeof(iotx_ca_crt_itls));
            HAL_GetProductKey(iotx_ca_crt_itls);
            iotx_ca_crt_itls[strlen(iotx_ca_crt_itls)] = '.';
            HAL_GetProductSecret(iotx_ca_crt_itls + strlen(iotx_ca_crt_itls));
            mqtt_params.pub_key = iotx_ca_crt_itls;
        }
    }
#endif
    mqtt_params.request_timeout_ms    = CONFIG_MQTT_REQUEST_TIMEOUT;
    mqtt_params.clean_session         = 0;
    mqtt_params.keepalive_interval_ms = CONFIG_MQTT_KEEPALIVE_INTERVAL * 1000;
    mqtt_params.read_buf_size         = CONFIG_MQTT_MESSAGE_MAXLEN;
    mqtt_params.write_buf_size        = CONFIG_MQTT_MESSAGE_MAXLEN;
    mqtt_params.handle_event.h_fp     = NULL;
    mqtt_params.handle_event.pcontext = NULL;

    /* optional configuration */
    if (pInitParams != NULL) {
        if (pInitParams->host && strlen(pInitParams->host)) {
            mqtt_params.host = pInitParams->host;
        } else {
            mqtt_warning("Using default hostname: '%s'", g_default_sign.hostname);
            mqtt_params.host = g_default_sign.hostname;
        }

        if (pInitParams->port) {
            mqtt_params.port = pInitParams->port;
        } else {
            mqtt_warning("Using default port: [%d]", g_default_sign.port);
            mqtt_params.port = g_default_sign.port;
        }

        if (pInitParams->client_id && strlen(pInitParams->client_id)) {
            mqtt_params.client_id = pInitParams->client_id;
        } else {
            mqtt_warning("Using default client_id: %s", g_default_sign.clientid);
            mqtt_params.client_id = g_default_sign.clientid;
        }

        if (pInitParams->username && strlen(pInitParams->username)) {
            mqtt_params.username = pInitParams->username;
        } else {
            mqtt_warning("Using default username: %s", g_default_sign.username);
            mqtt_params.username = g_default_sign.username;
        }

        if (pInitParams->password && strlen(pInitParams->password)) {
            mqtt_params.password = pInitParams->password;
        } else {
#if 1
            mqtt_warning("Using default password: %s", "******");
#else
            mqtt_warning("Using default password: %s", g_default_sign.password);
#endif
            mqtt_params.password = g_default_sign.password;
        }

        if (pInitParams->request_timeout_ms < CONFIG_MQTT_REQ_TIMEOUT_MIN ||
            pInitParams->request_timeout_ms > CONFIG_MQTT_REQ_TIMEOUT_MAX) {
            mqtt_warning("Using default request_timeout_ms: %d, configured value(%d) out of [%d, %d]",
                         mqtt_params.request_timeout_ms,
                         pInitParams->request_timeout_ms,
                         CONFIG_MQTT_REQ_TIMEOUT_MIN,
                         CONFIG_MQTT_REQ_TIMEOUT_MAX);
        } else {
            mqtt_params.request_timeout_ms = pInitParams->request_timeout_ms;
        }

        if (pInitParams->clean_session == 0 || pInitParams->clean_session == 1) {
            mqtt_params.clean_session = pInitParams->clean_session;
        }

        if (pInitParams->keepalive_interval_ms < CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN * 1000 ||
            pInitParams->keepalive_interval_ms > CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX * 1000) {
            mqtt_warning("Using default keepalive_interval_ms: %d, configured value(%d) out of [%d, %d]",
                         mqtt_params.keepalive_interval_ms,
                         pInitParams->keepalive_interval_ms,
                         CONFIG_MQTT_KEEPALIVE_INTERVAL_MIN * 1000,
                         CONFIG_MQTT_KEEPALIVE_INTERVAL_MAX * 1000);
        } else {
            mqtt_params.keepalive_interval_ms = pInitParams->keepalive_interval_ms;
        }

        if (!pInitParams->read_buf_size) {
            mqtt_warning("Using default read_buf_size: %d", mqtt_params.read_buf_size);
        } else {
            mqtt_params.read_buf_size = pInitParams->read_buf_size;
        }

        if (!pInitParams->write_buf_size) {
            mqtt_warning("Using default write_buf_size: %d", mqtt_params.write_buf_size);
        } else {
            mqtt_params.write_buf_size = pInitParams->write_buf_size;
        }

        if (pInitParams->handle_event.h_fp != NULL) {
            mqtt_params.handle_event.h_fp = pInitParams->handle_event.h_fp;
        }

        if (pInitParams->handle_event.pcontext != NULL) {
            mqtt_params.handle_event.pcontext = pInitParams->handle_event.pcontext;
        }
    } else {
        mqtt_warning("Using default port: [%d]", g_default_sign.port);
        mqtt_params.port = g_default_sign.port;

        mqtt_warning("Using default hostname: '%s'", g_default_sign.hostname);
        mqtt_params.host = g_default_sign.hostname;

        mqtt_warning("Using default client_id: %s", g_default_sign.clientid);
        mqtt_params.client_id = g_default_sign.clientid;

        mqtt_warning("Using default username: %s", g_default_sign.username);
        mqtt_params.username = g_default_sign.username;

#if 1
        mqtt_warning("Using default password: %s", "******");
#else
        mqtt_warning("Using default password: %s", g_default_sign.password);
#endif
        mqtt_params.password = g_default_sign.password;
    }

    pclient = wrapper_mqtt_init(&mqtt_params);
    if (pclient == NULL) {
        mqtt_err("wrapper_mqtt_init error");
        return NULL;
    }

    ret = wrapper_mqtt_connect(pclient);
    if (SUCCESS_RETURN != ret) {
        if (MQTT_CONNECT_BLOCK != ret) {
            mqtt_err("wrapper_mqtt_connect failed");
            wrapper_mqtt_release(&pclient);
            return NULL;
        }
    }

#ifndef ASYNC_PROTOCOL_STACK
    iotx_mqtt_report_funcs(pclient);
#endif

    g_mqtt_client = pclient;

    /* Mqtt Connect Callback */
    callback = iotx_event_callback(ITE_MQTT_CONNECT_SUCC);
    if (callback) {
        ((int (*)(void))callback)();
    }

    return pclient;
}

int IOT_MQTT_Destroy(void **phandler)
{
    void *client;
    if (phandler != NULL) {
        client = *phandler;
        *phandler = NULL;
    } else {
        client = g_mqtt_client;
    }

    if (client == NULL) {
        mqtt_err("handler is null");
        return NULL_VALUE_ERROR;
    }

    wrapper_mqtt_release(&client);
    g_mqtt_client = NULL;

    return SUCCESS_RETURN;
}

int IOT_MQTT_Yield(void *handle, int timeout_ms)
{
    void *pClient = (handle ? handle : g_mqtt_client);
    return wrapper_mqtt_yield(pClient, timeout_ms);
}

/* check whether MQTT connection is established or not */
int IOT_MQTT_CheckStateNormal(void *handle)
{
    void *pClient = (handle ? handle : g_mqtt_client);
    if (pClient == NULL) {
        mqtt_err("handler is null");
        return NULL_VALUE_ERROR;
    }

    return wrapper_mqtt_check_state(pClient);
}

int IOT_MQTT_Subscribe(void *handle,
                       const char *topic_filter,
                       iotx_mqtt_qos_t qos,
                       iotx_mqtt_event_handle_func_fpt topic_handle_func,
                       void *pcontext)
{
    void *client = handle ? handle : g_mqtt_client;

    if (client == NULL) { /* do offline subscribe */
        return iotx_mqtt_offline_subscribe(topic_filter, qos, topic_handle_func, pcontext);
    }

    if (topic_filter == NULL || strlen(topic_filter) == 0 || topic_handle_func == NULL) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

#ifdef SUB_PERSISTENCE_ENABLED
    if (qos > IOTX_MQTT_QOS3_SUB_LOCAL) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS3_SUB_LOCAL, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }
#else
    if (qos > IOTX_MQTT_QOS2) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS2, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }
#endif

    return wrapper_mqtt_subscribe(client, topic_filter, qos, topic_handle_func, pcontext);
}

#define SUBSCRIBE_SYNC_TIMEOUT_MAX 10000
int IOT_MQTT_Subscribe_Sync(void *handle,
                            const char *topic_filter,
                            iotx_mqtt_qos_t qos,
                            iotx_mqtt_event_handle_func_fpt topic_handle_func,
                            void *pcontext,
                            int timeout_ms)
{
    void *client = handle ? handle : g_mqtt_client;

    if (client == NULL) { /* do offline subscribe */
        return iotx_mqtt_offline_subscribe(topic_filter, qos, topic_handle_func, pcontext);
    }
    if (timeout_ms > SUBSCRIBE_SYNC_TIMEOUT_MAX) {
        timeout_ms = SUBSCRIBE_SYNC_TIMEOUT_MAX;
    }

    if (topic_filter == NULL || strlen(topic_filter) == 0 || topic_handle_func == NULL) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

#ifdef SUB_PERSISTENCE_ENABLED
    if (qos > IOTX_MQTT_QOS3_SUB_LOCAL) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS3_SUB_LOCAL, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }
#else
    if (qos > IOTX_MQTT_QOS2) {
        mqtt_warning("Invalid qos(%d) out of [%d, %d], using %d",
                     qos,
                     IOTX_MQTT_QOS0, IOTX_MQTT_QOS2, IOTX_MQTT_QOS0);
        qos = IOTX_MQTT_QOS0;
    }
#endif

    return wrapper_mqtt_subscribe_sync(client, topic_filter, qos, topic_handle_func, pcontext, timeout_ms);
}

int IOT_MQTT_Unsubscribe(void *handle, const char *topic_filter)
{
    void *client = handle ? handle : g_mqtt_client;


    if (client == NULL || topic_filter == NULL || strlen(topic_filter) == 0) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    return wrapper_mqtt_unsubscribe(client, topic_filter);
}

int IOT_MQTT_Publish(void *handle, const char *topic_name, iotx_mqtt_topic_info_pt topic_msg)
{
    void *client = handle ? handle : g_mqtt_client;
    int                 rc = -1;

    if (client == NULL || topic_name == NULL || strlen(topic_name) == 0) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    rc = wrapper_mqtt_publish(client, topic_name, topic_msg);
    return rc;
}

int IOT_MQTT_Publish_Simple(void *handle, const char *topic_name, int qos, void *data, int len)
{
    iotx_mqtt_topic_info_t mqtt_msg;
    void *client = handle ? handle : g_mqtt_client;
    int rc = -1;

    if (client == NULL || topic_name == NULL || strlen(topic_name) == 0) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    memset(&mqtt_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));

    mqtt_msg.qos         = qos;
    mqtt_msg.retain      = 0;
    mqtt_msg.dup         = 0;
    mqtt_msg.payload     = (void *)data;
    mqtt_msg.payload_len = len;

    rc = wrapper_mqtt_publish(client, topic_name, &mqtt_msg);

    if (rc < 0) {
        mqtt_err("IOT_MQTT_Publish failed\n");
        return -1;
    }

    return rc;
}

int IOT_MQTT_Nwk_Event_Handler(void *handle, iotx_mqtt_nwk_event_t event, iotx_mqtt_nwk_param_t *param)
{
#ifdef ASYNC_PROTOCOL_STACK
    void *client = handle ? handle : g_mqtt_client;
    int rc = -1;

    if (client == NULL || event >= IOTX_MQTT_SOC_MAX || param == NULL) {
        mqtt_err("params err");
        return NULL_VALUE_ERROR;
    }

    rc = wrapper_mqtt_nwk_event_handler(client, event, param);

    if (rc < 0) {
        mqtt_err("IOT_MQTT_Nwk_Event_Handler failed\n");
        return -1;
    }

    switch (event) {
        case IOTX_MQTT_SOC_CONNECTED: {
            iotx_mqtt_report_funcs(client);
        }
        break;
        default: {
        }
        break;
    }

    return rc;
#else
    return -1;
#endif
}
