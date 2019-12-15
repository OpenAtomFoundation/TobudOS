/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_ota_internal.h"

#if (OTA_SIGNAL_CHANNEL) == 1
    #include "ota_mqtt.c"
#elif (OTA_SIGNAL_CHANNEL) == 2
    #include "ota_coap.c"
#else
    #error "NOT support yet!"
#endif


typedef struct  {
    const char *product_key;    /* point to product key */
    const char *device_name;    /* point to device name */

    uint32_t id;                /* message id */
    IOT_OTA_State_t state;      /* OTA state */
    IOT_OTA_Type_t type;        /* OTA Type */
    uint32_t size_last_fetched; /* size of last downloaded */
    uint32_t size_fetched;      /* size of already downloaded */
    uint32_t size_file;         /* size of file */
    char *purl;                 /* point to URL */
    char *version;              /* point to string */
    char md5sum[33];            /* MD5 string */

    void *md5;                  /* MD5 handle */
    void *sha256;               /* Sha256 handle */
    void *ch_signal;            /* channel handle of signal exchanged with OTA server */
    void *ch_fetch;             /* channel handle of download */

    /* cota */
    char *configId;
    uint32_t configSize;
    char *sign;
    char *signMethod;
    char *cota_url;
    char *getType;

    int err;                    /* last error code */

    ota_fetch_cb_fpt  fetch_cb;  /* fetch_callback */
    void  *user_data;            /* fetch_callback's user_data */

    cota_fetch_cb_fpt fetch_cota_cb;
    void  *cota_user_data;

} OTA_Struct_t, *OTA_Struct_pt;


/* check whether the progress state is valid or not */
/* return: true, valid progress state; false, invalid progress state. */
static int ota_check_progress(IOT_OTA_Progress_t progress)
{
    return ((progress >= IOT_OTAP_BURN_FAILED)
            && (progress <= IOT_OTAP_FETCH_PERCENTAGE_MAX));
}


int iotx_ota_set_fetch_callback(void *pt, ota_fetch_cb_fpt fetch_cb, void *user_data)
{
    OTA_Struct_pt ota_pt = (OTA_Struct_pt)pt;

    if (NULL == ota_pt || NULL == fetch_cb) {
        return -1;
    }

    ota_pt->fetch_cb = fetch_cb;
    ota_pt->user_data = user_data;
    return 0;
}


int iotx_ota_set_cota_fetch_callback(void *pt, cota_fetch_cb_fpt fetch_cb, void *user_data)
{
    OTA_Struct_pt ota_pt = (OTA_Struct_pt)pt;

    if (NULL == ota_pt || NULL == fetch_cb) {
        return -1;
    }

    ota_pt->fetch_cota_cb = fetch_cb;
    ota_pt->cota_user_data = user_data;
    return 0;
}


static int ota_callback(void *pcontext, const char *msg, uint32_t msg_len, iotx_ota_topic_types_t type)
{
    const char *pvalue;
    uint32_t val_len;

    OTA_Struct_pt h_ota = (OTA_Struct_pt) pcontext;

    if (h_ota->state == IOT_OTAS_FETCHING) {
        OTA_LOG_INFO("In downloading state");
        return -1;
    }

    switch (type) {
        case IOTX_OTA_TOPIC_TYPE_DEVICE_REQUEST:
        case IOTX_OTA_TOPIC_TYPE_DEVICE_UPGRATE: {
            pvalue = otalib_JsonValueOf(msg, msg_len, "message", &val_len);
            if (NULL == pvalue) {
                OTA_LOG_ERROR("invalid json doc of OTA ");
                return -1;
            }

            /* check whether is positive message */
            if (!((strlen("success") == val_len) && (0 == strncmp(pvalue, "success", val_len)))) {
                OTA_LOG_ERROR("fail state of json doc of OTA");
                return -1;
            }

            /* get value of 'data' key */
            pvalue = otalib_JsonValueOf(msg, msg_len, "data", &val_len);
            if (NULL == pvalue) {
                OTA_LOG_ERROR("Not 'data' key in json doc of OTA");
                return -1;
            }

            if (0 != otalib_GetParams(pvalue, val_len, &h_ota->purl, &h_ota->version, h_ota->md5sum, &h_ota->size_file)) {
                OTA_LOG_ERROR("Get config parameter failed");
                return -1;
            }

            if (NULL == (h_ota->ch_fetch = ofc_Init(h_ota->purl))) {
                OTA_LOG_ERROR("Initialize fetch module failed");
                return -1;
            }

            h_ota->type = IOT_OTAT_FOTA;
            h_ota->state = IOT_OTAS_FETCHING;

            if (h_ota->fetch_cb) {
                h_ota->fetch_cb(h_ota->user_data, 0, h_ota->size_file, h_ota->purl, h_ota->version);
            }
        }
        break;

        case IOTX_OTA_TOPIC_TYPE_CONFIG_GET: {
            pvalue = otalib_JsonValueOf(msg, msg_len, "code", &val_len);
            if (NULL == pvalue) {
                OTA_LOG_ERROR("invalid json doc of OTA ");
                return -1;
            }

            /* check whether is positive message */
            if (!((strlen("200") == val_len) && (0 == strncmp(pvalue, "200", val_len)))) {
                OTA_LOG_ERROR("fail state of json doc of OTA");
                return -1;
            }

            /* get value of 'data' key */
            pvalue = otalib_JsonValueOf(msg, msg_len, "data", &val_len);
            if (NULL == pvalue) {
                OTA_LOG_ERROR("Not 'data' key in json doc of OTA");
                return -1;
            }

            if (0 != otalib_GetConfigParams(pvalue, val_len, &h_ota->configId, &h_ota->configSize,
                                            &h_ota->sign, &h_ota->signMethod, &h_ota->cota_url, &h_ota->getType))  {
                OTA_LOG_ERROR("Get firmware parameter failed");
                return -1;
            }

            h_ota->size_file = h_ota->configSize;
            h_ota->size_fetched = 0;
            if (NULL != h_ota->md5) {
                otalib_MD5Deinit(h_ota->md5);
            }
            h_ota->md5 = otalib_MD5Init();

            if (NULL != h_ota->sha256) {
                otalib_Sha256Deinit(h_ota->sha256);
            }
            h_ota->sha256 = otalib_Sha256Init();

            if (NULL == (h_ota->ch_fetch = ofc_Init(h_ota->cota_url))) {
                OTA_LOG_ERROR("Initialize fetch module failed");
                return -1;
            }

            h_ota->type = IOT_OTAT_COTA;
            h_ota->state = IOT_OTAS_FETCHING;

            if (h_ota->fetch_cota_cb) {
                h_ota->fetch_cota_cb(h_ota->user_data, 0, h_ota->configId, h_ota->configSize, h_ota->sign, h_ota->signMethod,
                                     h_ota->cota_url, h_ota->getType);
            }
        }
        break;

        case IOTX_OTA_TOPIC_TYPE_CONFIG_PUSH: {
            /* get value of 'params' key */
            pvalue = otalib_JsonValueOf(msg, msg_len, "params", &val_len);
            if (NULL == pvalue) {
                OTA_LOG_ERROR("Not 'data' key in json doc of OTA");
                return -1;
            }

            if (0 != otalib_GetConfigParams(pvalue, val_len, &h_ota->configId, &h_ota->configSize,
                                            &h_ota->sign, &h_ota->signMethod, &h_ota->cota_url, &h_ota->getType)) {
                OTA_LOG_ERROR("Get firmware parameter failed");
                return -1;
            }

            h_ota->size_file = h_ota->configSize;
            h_ota->size_fetched = 0;
            if (NULL != h_ota->md5) {
                otalib_MD5Deinit(h_ota->md5);
            }
            h_ota->md5 = otalib_MD5Init();

            if (NULL != h_ota->sha256) {
                otalib_Sha256Deinit(h_ota->sha256);
            }
            h_ota->sha256 = otalib_Sha256Init();

            if (NULL == (h_ota->ch_fetch = ofc_Init(h_ota->cota_url))) {
                OTA_LOG_ERROR("Initialize fetch module failed");
                return -1;
            }

            h_ota->type = IOT_OTAT_COTA;
            h_ota->state = IOT_OTAS_FETCHING;

            if (h_ota->fetch_cota_cb) {
                h_ota->fetch_cota_cb(h_ota->user_data, 0, h_ota->configId, h_ota->configSize, h_ota->sign, h_ota->signMethod,
                                     h_ota->cota_url, h_ota->getType);
            }
        }
        break;

        default:
            return -1;
            break;
    }

    return 0;
}

static int g_ota_is_initialized = 0;

/* Initialize OTA module */
void *IOT_OTA_Init(const char *product_key, const char *device_name, void *ch_signal)
{
    OTA_Struct_pt h_ota = NULL;

    if (1 == g_ota_is_initialized) {
        OTA_LOG_ERROR("iot ota has been initialized");
        return NULL;
    }

    if ((NULL == product_key) || (NULL == device_name)) {
        OTA_LOG_ERROR("one or more parameters is invalid");
        return NULL;
    }

    if (NULL == (h_ota = OTA_MALLOC(sizeof(OTA_Struct_t)))) {
        OTA_LOG_ERROR("allocate failed");
        return NULL;
    }
    memset(h_ota, 0, sizeof(OTA_Struct_t));
    h_ota->type = IOT_OTAT_NONE;
    h_ota->state = IOT_OTAS_UNINITED;

    h_ota->ch_signal = osc_Init(product_key, device_name, ch_signal, ota_callback, h_ota);
    if (NULL == h_ota->ch_signal) {
        OTA_LOG_ERROR("initialize signal channel failed");
        goto do_exit;
    }

    h_ota->md5 = otalib_MD5Init();
    if (NULL == h_ota->md5) {
        OTA_LOG_ERROR("initialize md5 failed");
        goto do_exit;
    }
    h_ota->sha256 = otalib_Sha256Init();
    if (NULL == h_ota->sha256) {
        OTA_LOG_ERROR("initialize sha256 failed");
        goto do_exit;
    }

    h_ota->product_key = product_key;
    h_ota->device_name = device_name;
    h_ota->state = IOT_OTAS_INITED;
    g_ota_is_initialized = 1;
    return h_ota;

do_exit:

    if (NULL != h_ota->ch_signal) {
        osc_Deinit(h_ota->ch_signal);
    }

    if (NULL != h_ota->md5) {
        otalib_MD5Deinit(h_ota->md5);
    }

    if (NULL != h_ota->sha256) {
        otalib_Sha256Deinit(h_ota->sha256);
    }

    if (NULL != h_ota) {
        OTA_FREE(h_ota);
    }

    return NULL;

#undef AOM_INFO_MSG_LEN
}


/* deinitialize OTA module */
int IOT_OTA_Deinit(void *handle)
{
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if (NULL == h_ota) {
        OTA_LOG_ERROR("handle is NULL");
        return IOT_OTAE_INVALID_PARAM;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        OTA_LOG_ERROR("handle is uninitialized");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return -1;
    }

    g_ota_is_initialized = 0;

    if (NULL != h_ota->ch_signal) {
        osc_Deinit(h_ota->ch_signal);
    }

    if (NULL != h_ota->ch_fetch) {
        ofc_Deinit(h_ota->ch_fetch);
    }

    if (NULL != h_ota->md5) {
        otalib_MD5Deinit(h_ota->md5);
    }

    if (NULL != h_ota->sha256) {
        otalib_Sha256Deinit(h_ota->sha256);
    }

    if (NULL != h_ota->purl) {
        OTA_FREE(h_ota->purl);
    }

    if (NULL != h_ota->version) {
        OTA_FREE(h_ota->version);
    }

    if (NULL != h_ota->configId) {
        OTA_FREE(h_ota->configId);
    }

    if (NULL != h_ota->sign) {
        OTA_FREE(h_ota->sign);
    }

    if (NULL != h_ota->signMethod) {
        OTA_FREE(h_ota->signMethod);
    }

    if (NULL != h_ota->cota_url) {
        OTA_FREE(h_ota->cota_url);
    }

    if (NULL != h_ota->getType) {
        OTA_FREE(h_ota->getType);
    }

    OTA_FREE(h_ota);
    return 0;
}


#define OTA_VERSION_STR_LEN_MIN     (1)
#define OTA_VERSION_STR_LEN_MAX     (32)

int IOT_OTA_ReportVersion(void *handle, const char *version)
{
#define MSG_INFORM_LEN  (128)

    int ret, len;
    char *msg_informed;
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if ((NULL == h_ota) || (NULL == version)) {
        OTA_LOG_ERROR("one or more invalid parameter");
        return IOT_OTAE_INVALID_PARAM;
    }

    len = strlen(version);
    if ((len < OTA_VERSION_STR_LEN_MIN) || (len > OTA_VERSION_STR_LEN_MAX)) {
        OTA_LOG_ERROR("version string is invalid: must be [1, 32] chars");
        h_ota->err = IOT_OTAE_INVALID_PARAM;
        return -1;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        OTA_LOG_ERROR("handle is uninitialized");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return -1;
    }

    if (NULL == (msg_informed = OTA_MALLOC(MSG_INFORM_LEN))) {
        OTA_LOG_ERROR("allocate for msg_informed failed");
        h_ota->err = IOT_OTAE_NOMEM;
        return -1;
    }

    ret = otalib_GenInfoMsg(msg_informed, MSG_INFORM_LEN, h_ota->id, version);
    if (ret != 0) {
        OTA_LOG_ERROR("generate inform message failed");
        h_ota->err = ret;
        ret = -1;
        goto do_exit;
    }

    ret = osc_ReportVersion(h_ota->ch_signal, msg_informed);
    if (0 != ret) {
        OTA_LOG_ERROR("Report version failed");
        h_ota->err = ret;
        ret = -1;
        goto do_exit;
    }
    ret = 0;

do_exit:
    if (NULL != msg_informed) {
        OTA_FREE(msg_informed);
    }
    return ret;

#undef MSG_INFORM_LEN
}

int iotx_req_image(void *handle, const char *version)
{
#define MSG_REQUEST_LEN  (128)

    int ret, len;
    char *msg_informed;
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if ((NULL == h_ota) || (NULL == version)) {
        OTA_LOG_ERROR("one or more invalid parameter");
        return IOT_OTAE_INVALID_PARAM;
    }

    len = strlen(version);
    if ((len < OTA_VERSION_STR_LEN_MIN) || (len > OTA_VERSION_STR_LEN_MAX)) {
        OTA_LOG_ERROR("version string is invalid: must be [1, 32] chars");
        h_ota->err = IOT_OTAE_INVALID_PARAM;
        return -1;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        OTA_LOG_ERROR("handle is uninitialized");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return -1;
    }

    if (IOT_OTAS_FETCHING == h_ota->state) {
        OTA_LOG_ERROR("ota is busying");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return -1;
    }

    if (NULL == (msg_informed = OTA_MALLOC(MSG_REQUEST_LEN))) {
        OTA_LOG_ERROR("allocate for msg_informed failed");
        h_ota->err = IOT_OTAE_NOMEM;
        return -1;
    }

    ret = otalib_GenInfoMsg(msg_informed, MSG_REQUEST_LEN, h_ota->id, version);
    if (ret != 0) {
        OTA_LOG_ERROR("generate request image message failed");
        h_ota->err = ret;
        ret = -1;
        goto do_exit;
    }

    ret = osc_RequestImage(h_ota->ch_signal, msg_informed);
    if (0 != ret) {
        OTA_LOG_ERROR("Request image failed");
        h_ota->err = ret;
        ret = -1;
        goto do_exit;
    }
    ret = 0;

do_exit:
    if (NULL != msg_informed) {
        OTA_FREE(msg_informed);
    }
    return ret;

#undef MSG_REQUEST_LEN
}


int IOT_OTA_ReportProgress(void *handle, IOT_OTA_Progress_t progress, const char *msg)
{
#define MSG_REPORT_LEN  (256)

    int ret = -1;
    char *msg_reported;
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if (NULL == handle) {
        OTA_LOG_ERROR("handle is NULL");
        return IOT_OTAE_INVALID_PARAM;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        OTA_LOG_ERROR("handle is uninitialized");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return -1;
    }
    if (!ota_check_progress(progress)) {
        OTA_LOG_ERROR("progress is a invalid parameter");
        h_ota->err = IOT_OTAE_INVALID_PARAM;
        return -1;
    }

    if (NULL == (msg_reported = OTA_MALLOC(MSG_REPORT_LEN))) {
        OTA_LOG_ERROR("allocate for msg_reported failed");
        h_ota->err = IOT_OTAE_NOMEM;
        return -1;
    }

    ret = otalib_GenReportMsg(msg_reported, MSG_REPORT_LEN, h_ota->id, progress, msg);
    if (0 != ret) {
        OTA_LOG_ERROR("generate reported message failed");
        h_ota->err = ret;
        goto do_exit;
    }

    ret = osc_ReportProgress(h_ota->ch_signal, msg_reported);
    if (0 != ret) {
        OTA_LOG_ERROR("Report progress failed");
        h_ota->err = ret;
        goto do_exit;
    }

    ret = 0;

do_exit:
    if (NULL != msg_reported) {
        OTA_FREE(msg_reported);
    }
    return ret;

#undef MSG_REPORT_LEN
}

int iotx_ota_get_config(void *handle, const char *configScope, const char *getType, const char *attributeKeys)
{
#define MSG_REPORT_LEN  (256)

    int ret = -1;
    char *msg_get;
    char topic[OTA_MQTT_TOPIC_LEN] = {0};
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;
    iotx_mqtt_topic_info_t topic_info;

    memset(&topic_info, 0, sizeof(iotx_mqtt_topic_info_t));

    if (NULL == handle) {
        OTA_LOG_ERROR("handle is NULL");
        return IOT_OTAE_INVALID_PARAM;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        OTA_LOG_ERROR("handle is uninitialized");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return -1;
    }

    if (IOT_OTAS_FETCHING == h_ota->state) {
        OTA_LOG_ERROR("ota is busying");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return -1;
    }

    if (NULL == (msg_get = OTA_MALLOC(MSG_REPORT_LEN))) {
        OTA_LOG_ERROR("allocate for msg_reported failed");
        h_ota->err = IOT_OTAE_NOMEM;
        return -1;
    }

    if (0 > HAL_Snprintf(topic,
                         OTA_MQTT_TOPIC_LEN,
                         "/sys/%s/%s/thing/config/get",
                         h_ota->product_key,
                         h_ota->device_name)) {
        goto do_exit;
    };

    if (0 > HAL_Snprintf(msg_get,
                         MSG_REPORT_LEN,
                         "{\"id\" : %d,\"version\":\"1.0\",\"params\":{\"configScope\":\"%s\",\"getType\":\"%s\",\"attributeKeys\":\"%s\"},\"method\":\"thing.config.get\"}",
                         h_ota->id,
                         configScope,
                         getType,
                         attributeKeys)) {
        goto do_exit;
    };
    OTA_LOG_INFO(msg_get);
    topic_info.qos = IOTX_MQTT_QOS0;
    topic_info.payload = (void *)msg_get;
    topic_info.payload_len = strlen(msg_get);

    ret = osc_RequestConfig(h_ota->ch_signal, topic, &topic_info);
    if (ret < 0) {
        OTA_LOG_ERROR("publish failed");
        return IOT_OTAE_OSC_FAILED;
    }

    ret = 0;

do_exit:
    if (NULL != msg_get) {
        OTA_FREE(msg_get);
    }
    return ret;

#undef MSG_REPORT_LEN
}


/* check whether is downloading */
int IOT_OTA_IsFetching(void *handle)
{
    OTA_Struct_pt h_ota = (OTA_Struct_pt)handle;

    if (NULL == handle) {
        OTA_LOG_ERROR("handle is NULL");
        return 0;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        OTA_LOG_ERROR("handle is uninitialized");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return 0;
    }

    return (IOT_OTAS_FETCHING == h_ota->state);
}


/* check whether fetch over */
int IOT_OTA_IsFetchFinish(void *handle)
{
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if (NULL == handle) {
        OTA_LOG_ERROR("handle is NULL");
        return 0;
    }

    if (IOT_OTAS_UNINITED == h_ota->state) {
        OTA_LOG_ERROR("handle is uninitialized");
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return 0;
    }

    return (IOT_OTAS_FETCHED == h_ota->state);
}


int IOT_OTA_FetchYield(void *handle, char *buf, uint32_t buf_len, uint32_t timeout_s)
{
    int ret;
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if ((NULL == handle) || (NULL == buf) || (0 == buf_len)) {
        OTA_LOG_ERROR("invalid parameter");
        return IOT_OTAE_INVALID_PARAM;
    }

    if (IOT_OTAS_FETCHING != h_ota->state) {
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return IOT_OTAE_INVALID_STATE;
    }

    ret = ofc_Fetch(h_ota->ch_fetch, buf, buf_len, timeout_s);
    if (ret < 0) {
        OTA_LOG_ERROR("Fetch firmware failed");
        h_ota->state = IOT_OTAS_FETCHED;
        h_ota->type = IOT_OTAT_NONE;
        h_ota->err = IOT_OTAE_FETCH_FAILED;

        if (h_ota->fetch_cb && h_ota->purl) {
            h_ota->fetch_cb(h_ota->user_data, 1, h_ota->size_file, h_ota->purl, h_ota->version);
            /* remove */
            h_ota->purl = NULL;
        } else if (h_ota->fetch_cota_cb && h_ota->cota_url) {
            h_ota->fetch_cota_cb(h_ota->user_data, 1, h_ota->configId, h_ota->configSize, h_ota->sign, h_ota->signMethod,
                                 h_ota->cota_url, h_ota->getType);
            /* remove */
            h_ota->cota_url = NULL;
        }
        h_ota->size_fetched = 0;
        return -1;
    } else if (0 == h_ota->size_fetched) {
        /* force report status in the first */
        IOT_OTA_ReportProgress(h_ota, IOT_OTAP_FETCH_PERCENTAGE_MIN, "Enter in downloading state");
    }

    otalib_MD5Update(h_ota->md5, buf, ret);
    otalib_Sha256Update(h_ota->sha256, buf, ret);
    h_ota->size_last_fetched = ret;
    h_ota->size_fetched += ret;

    if (h_ota->size_fetched >= h_ota->size_file) {
        h_ota->type = IOT_OTAT_NONE;
        h_ota->state = IOT_OTAS_FETCHED;
        if (h_ota->fetch_cb && h_ota->purl) {
            h_ota->fetch_cb(h_ota->user_data, 1, h_ota->size_file, h_ota->purl, h_ota->version);
            /* remove */
            h_ota->purl = NULL;
        } else if (h_ota->fetch_cota_cb && h_ota->cota_url) {
            h_ota->fetch_cota_cb(h_ota->user_data, 1, h_ota->configId, h_ota->configSize, h_ota->sign, h_ota->signMethod,
                                 h_ota->cota_url, h_ota->getType);
            /* remove */
            h_ota->cota_url = NULL;
        }
    }

    return ret;
}


int IOT_OTA_Ioctl(void *handle, IOT_OTA_CmdType_t type, void *buf, int buf_len)
{
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if ((NULL == handle) || (NULL == buf) || (0 == buf_len)) {
        OTA_LOG_ERROR("invalid parameter");
        return IOT_OTAE_INVALID_PARAM;
    }

    if (h_ota->state < IOT_OTAS_FETCHING) {
        h_ota->err = IOT_OTAE_INVALID_STATE;
        return IOT_OTAE_INVALID_STATE;
    }

    switch (type) {
        case IOT_OTAG_COTA_CONFIG_ID: {
            char **value = (char **)buf;
            if (value == NULL || *value != NULL || h_ota->configId == NULL) {
                OTA_LOG_ERROR("Invalid parameter");
                h_ota->err = IOT_OTAE_INVALID_PARAM;
                return -1;
            } else {
                *value = OTA_API_MALLOC(strlen(h_ota->configId) + 1);
                if (*value == NULL) {
                    h_ota->err = IOT_OTAE_INVALID_PARAM;
                    return -1;
                }
                memset(*value, 0, strlen(h_ota->configId) + 1);
                memcpy(*value, h_ota->configId, strlen(h_ota->configId));
                return 0;
            }
        }
        break;
        case IOT_OTAG_COTA_CONFIG_SIZE: {
            if ((4 != buf_len) || (0 != ((unsigned long)buf & 0x3))) {
                OTA_LOG_ERROR("Invalid parameter");
                h_ota->err = IOT_OTAE_INVALID_PARAM;
                return -1;
            } else {
                *((uint32_t *)buf) = h_ota->configSize;
                return 0;
            }
        }
        break;
        case IOT_OTAG_COTA_SIGN: {
            char **value = (char **)buf;
            if (value == NULL || *value != NULL || h_ota->sign == NULL) {
                OTA_LOG_ERROR("Invalid parameter");
                h_ota->err = IOT_OTAE_INVALID_PARAM;
                return -1;
            } else {
                *value = OTA_API_MALLOC(strlen(h_ota->sign) + 1);
                if (*value == NULL) {
                    h_ota->err = IOT_OTAE_INVALID_PARAM;
                    return -1;
                }
                memset(*value, 0, strlen(h_ota->sign) + 1);
                memcpy(*value, h_ota->sign, strlen(h_ota->sign));
                return 0;
            }
        }
        break;
        case IOT_OTAG_COTA_SIGN_METHOD: {
            char **value = (char **)buf;
            if (value == NULL || *value != NULL || h_ota->signMethod == NULL) {
                OTA_LOG_ERROR("Invalid parameter");
                h_ota->err = IOT_OTAE_INVALID_PARAM;
                return -1;
            } else {
                *value = OTA_API_MALLOC(strlen(h_ota->signMethod) + 1);
                if (*value == NULL) {
                    h_ota->err = IOT_OTAE_INVALID_PARAM;
                    return -1;
                }
                memset(*value, 0, strlen(h_ota->signMethod) + 1);
                memcpy(*value, h_ota->signMethod, strlen(h_ota->signMethod));
                return 0;
            }
        }
        break;
        case IOT_OTAG_COTA_URL: {
            char **value = (char **)buf;
            if (value == NULL || *value != NULL || h_ota->cota_url == NULL) {
                OTA_LOG_ERROR("Invalid parameter");
                h_ota->err = IOT_OTAE_INVALID_PARAM;
                return -1;
            } else {
                *value = OTA_API_MALLOC(strlen(h_ota->cota_url) + 1);
                if (*value == NULL) {
                    h_ota->err = IOT_OTAE_INVALID_PARAM;
                    return -1;
                }
                memset(*value, 0, strlen(h_ota->cota_url) + 1);
                memcpy(*value, h_ota->cota_url, strlen(h_ota->cota_url));
                return 0;
            }
        }
        break;
        case IOT_OTAG_COTA_GETTYPE: {
            char **value = (char **)buf;
            if (value == NULL || *value != NULL || h_ota->getType == NULL) {
                OTA_LOG_ERROR("Invalid parameter");
                h_ota->err = IOT_OTAE_INVALID_PARAM;
                return -1;
            } else {
                *value = OTA_API_MALLOC(strlen(h_ota->getType) + 1);
                if (*value == NULL) {
                    h_ota->err = IOT_OTAE_INVALID_PARAM;
                    return -1;
                }
                memset(*value, 0, strlen(h_ota->getType) + 1);
                memcpy(*value, h_ota->getType, strlen(h_ota->getType));
                return 0;
            }
        }
        break;
        case IOT_OTAG_OTA_TYPE: {
            if ((4 != buf_len) || (0 != ((unsigned long)buf & 0x3))) {
                OTA_LOG_ERROR("Invalid parameter");
                h_ota->err = IOT_OTAE_INVALID_PARAM;
                return -1;
            } else {
                *((uint32_t *)buf) = h_ota->type;
                return 0;
            }
        }
        break;
        case IOT_OTAG_FETCHED_SIZE:
            if ((4 != buf_len) || (0 != ((unsigned long)buf & 0x3))) {
                OTA_LOG_ERROR("Invalid parameter");
                h_ota->err = IOT_OTAE_INVALID_PARAM;
                return -1;
            } else {
                *((uint32_t *)buf) = h_ota->size_fetched;
                return 0;
            }

        case IOT_OTAG_FILE_SIZE:
            if ((4 != buf_len) || (0 != ((unsigned long)buf & 0x3))) {
                OTA_LOG_ERROR("Invalid parameter");
                h_ota->err = IOT_OTAE_INVALID_PARAM;
                return -1;
            } else {
                *((uint32_t *)buf) = h_ota->size_file;
                return 0;
            };

        case IOT_OTAG_VERSION: {
            strncpy(buf, h_ota->version, buf_len);
            ((char *)buf)[buf_len - 1] = '\0';
        }
        break;

        case IOT_OTAG_MD5SUM:
            strncpy(buf, h_ota->md5sum, buf_len);
            ((char *)buf)[buf_len - 1] = '\0';
            break;

        case IOT_OTAG_CHECK_FIRMWARE:
            if ((4 != buf_len) || (0 != ((unsigned long)buf & 0x3))) {
                OTA_LOG_ERROR("Invalid parameter");
                h_ota->err = IOT_OTAE_INVALID_PARAM;
                return -1;
            } else if (h_ota->state != IOT_OTAS_FETCHED) {
                h_ota->err = IOT_OTAE_INVALID_STATE;
                OTA_LOG_ERROR("Firmware can be checked in IOT_OTAS_FETCHED state only");
                return -1;
            } else {
                char md5_str[33];
                otalib_MD5Finalize(h_ota->md5, md5_str);
                OTA_LOG_DEBUG("origin=%s, now=%s", h_ota->md5sum, md5_str);
                if (0 == strcmp(h_ota->md5sum, md5_str)) {
                    *((uint32_t *)buf) = 1;
                } else {
                    *((uint32_t *)buf) = 0;
                    IOT_OTA_ReportProgress(h_ota, IOT_OTAP_CHECK_FALIED, NULL);
                    OTA_LOG_ERROR("image checksum compare failed");
                }
                return 0;
            }
        case IOT_OTAG_CHECK_CONFIG:
            if ((4 != buf_len) || (0 != ((unsigned long)buf & 0x3))) {
                OTA_LOG_ERROR("Invalid parameter");
                h_ota->err = IOT_OTAE_INVALID_PARAM;
                return -1;
            } else if (h_ota->state != IOT_OTAS_FETCHED) {
                h_ota->err = IOT_OTAE_INVALID_STATE;
                OTA_LOG_ERROR("Config can be checked in IOT_OTAS_FETCHED state only");
                return -1;
            } else {
                if (0 == strncmp(h_ota->signMethod, "Md5", strlen(h_ota->signMethod))) {
                    char md5_str[33];
                    otalib_MD5Finalize(h_ota->md5, md5_str);
                    OTA_LOG_DEBUG("origin=%s, now=%s", h_ota->sign, md5_str);
                    if (0 == strcmp(h_ota->sign, md5_str)) {
                        *((uint32_t *)buf) = 1;
                    } else {
                        *((uint32_t *)buf) = 0;
                    }
                }
                if (0 == strncmp(h_ota->signMethod, "Sha256", strlen(h_ota->signMethod))) {
                    char sha256_str[65];
                    otalib_Sha256Finalize(h_ota->sha256, sha256_str);
                    OTA_LOG_DEBUG("origin=%s, now=%s", h_ota->sign, sha256_str);
                    if (0 == strcmp(h_ota->sign, sha256_str)) {
                        *((uint32_t *)buf) = 1;
                    } else {
                        *((uint32_t *)buf) = 0;
                    }
                }
                return 0;
            }
        case IOT_OTAG_RESET_FETCHED_SIZE: {
            h_ota->size_fetched = 0;
            return 0;
        }
        default:
            OTA_LOG_ERROR("invalid cmd type");
            h_ota->err = IOT_OTAE_INVALID_PARAM;
            return -1;
    }

    return 0;
}


/* Get last error code */
int IOT_OTA_GetLastError(void *handle)
{
    OTA_Struct_pt h_ota = (OTA_Struct_pt) handle;

    if (NULL == handle) {
        OTA_LOG_ERROR("handle is NULL");
        return  IOT_OTAE_INVALID_PARAM;
    }

    return h_ota->err;
}



