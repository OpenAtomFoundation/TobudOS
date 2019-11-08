#include <stdio.h>
#include <string.h>
#include "mqtt_api.h"
#include "dev_reset_api.h"

void HAL_Printf(const char *fmt, ...);
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN]);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN]);

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

static int reset_mqtt_packet_id = 0;
static int reset_reply_received = 0;

void example_event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    EXAMPLE_TRACE("msg->event_type : %d", msg->event_type);
}

void example_devrst_evt_handle(iotx_devrst_evt_type_t evt, void *msg)
{
    switch (evt)
    {
        case IOTX_DEVRST_EVT_RECEIVED: {
            iotx_devrst_evt_recv_msg_t *recv_msg = (iotx_devrst_evt_recv_msg_t *)msg;
            if (recv_msg->msgid != reset_mqtt_packet_id) {
                return;
            }
            EXAMPLE_TRACE("Receive Reset Responst");
            EXAMPLE_TRACE("Msg ID: %d", recv_msg->msgid);
            EXAMPLE_TRACE("Payload: %.*s", recv_msg->payload_len, recv_msg->payload);
            reset_reply_received = 1;
        }
        break;

        default:
            break;
    }
}

int main(int argc, char *argv[])
{
    int res = 0;
    void                   *pclient = NULL;
    iotx_dev_meta_info_t    meta_info;
    iotx_mqtt_param_t       mqtt_params;

    memset(&mqtt_params, 0, sizeof(iotx_mqtt_param_t));
    memset(&meta_info, 0, sizeof(iotx_dev_meta_info_t));

    HAL_GetProductKey(meta_info.product_key);
    HAL_GetDeviceName(meta_info.device_name);

    mqtt_params.handle_event.h_fp = example_event_handle;

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        return -1;
    }

    res = IOT_DevReset_Report(&meta_info, example_devrst_evt_handle, NULL);
    if (res < 0) {
        return -1;
    }
    reset_mqtt_packet_id = res;

    while (!reset_reply_received) {

        IOT_MQTT_Yield(pclient, 200);
    }

    EXAMPLE_TRACE("Example Execute Success, Now Exit...");

    return 0;
}

