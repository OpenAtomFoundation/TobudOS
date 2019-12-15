#include "dev_sign_api.h"

#define EXAMPLE_PRODUCT_KEY     "a1X2bEnP82z"
#define EXAMPLE_PRODUCT_SECRET  "7jluWm1zql7bt8qK"
#define EXAMPLE_DEVICE_NAME     "example1"
#define EXAMPLE_DEVICE_SECRET   "ga7XA6KdlEeiPXQPpRbAjOZXwG8ydgSe"

/* Implenment this HAL or using "printf" of your own system if you want to print something in example*/
void HAL_Printf(const char *fmt, ...);

int main(int argc, char *argv[])
{
    iotx_mqtt_region_types_t region = IOTX_CLOUD_REGION_SHANGHAI;
    iotx_dev_meta_info_t meta;
    iotx_sign_mqtt_t sign_mqtt;

    memset(&meta, 0, sizeof(iotx_dev_meta_info_t));
    memcpy(meta.product_key, EXAMPLE_PRODUCT_KEY, strlen(EXAMPLE_PRODUCT_KEY));
    memcpy(meta.product_secret, EXAMPLE_PRODUCT_SECRET, strlen(EXAMPLE_PRODUCT_SECRET));
    memcpy(meta.device_name, EXAMPLE_DEVICE_NAME, strlen(EXAMPLE_DEVICE_NAME));
    memcpy(meta.device_secret, EXAMPLE_DEVICE_SECRET, strlen(EXAMPLE_DEVICE_SECRET));

    if (IOT_Sign_MQTT(region, &meta, &sign_mqtt) < 0) {
        return -1;
    }

#if 1   /* Uncomment this if you want to show more information */
    HAL_Printf("sign_mqtt.hostname: %s\n", sign_mqtt.hostname);
    HAL_Printf("sign_mqtt.port    : %d\n", sign_mqtt.port);
    HAL_Printf("sign_mqtt.username: %s\n", sign_mqtt.username);
    HAL_Printf("sign_mqtt.password: %s\n", sign_mqtt.password);
    HAL_Printf("sign_mqtt.clientid: %s\n", sign_mqtt.clientid);
#endif

    /*
     * Then you can pass this output parameter to MQTT connect APIs to establish connection
     *
     * e.g.
     *
     *    memset(&mqtt_params, 0x0, sizeof(mqtt_params));
     *
     *    mqtt_params.port = sign_mqtt.port;
     *    mqtt_params.host = sign_mqtt.hostname;
     *    mqtt_params.client_id = sign_mqtt.clientid;
     *    mqtt_params.username = sign_mqtt.username;
     *    mqtt_params.password = sign_mqtt.password;
     *
     *    mqtt_params.request_timeout_ms = 2000;
     *    mqtt_params.clean_session = 0;
     *    mqtt_params.keepalive_interval_ms = 60000;
     *    mqtt_params.read_buf_size = 1024;
     *    mqtt_params.write_buf_size = 1024;
     *
     *    mqtt_params.handle_event.h_fp = example_event_handle;
     *    mqtt_params.handle_event.pcontext = NULL;
     *
     *    pclient = IOT_MQTT_Construct(&mqtt_params);
     *
     */

    return 0;
}

