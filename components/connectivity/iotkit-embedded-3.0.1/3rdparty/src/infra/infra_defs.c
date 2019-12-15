
#include "infra_config.h"
#include "infra_types.h"
#include "infra_defs.h"

const char * g_infra_mqtt_domain[IOTX_MQTT_DOMAIN_NUMBER] = {
    "iot-as-mqtt.cn-shanghai.aliyuncs.com",       /* Shanghai */
    "iot-as-mqtt.ap-southeast-1.aliyuncs.com",    /* Singapore */
    "iot-as-mqtt.ap-northeast-1.aliyuncs.com",    /* Japan */
    "iot-as-mqtt.us-west-1.aliyuncs.com",         /* America */
    "iot-as-mqtt.eu-central-1.aliyuncs.com",      /* Germany */
    NULL,                                           /* Custom */
};

const char *g_infra_http_domain[IOTX_HTTP_DOMAIN_NUMBER] = {
    "iot-auth.cn-shanghai.aliyuncs.com",         /* Shanghai */
    "iot-auth.ap-southeast-1.aliyuncs.com",      /* Singapore */
    "iot-auth.ap-northeast-1.aliyuncs.com",      /* Japan */
    "iot-auth.us-west-1.aliyuncs.com",           /* America */
    "iot-auth.eu-central-1.aliyuncs.com",        /* Germany */
    NULL,                                        /* Custom */
};

