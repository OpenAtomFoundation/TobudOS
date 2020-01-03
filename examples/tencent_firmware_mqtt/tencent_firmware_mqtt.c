#include "tos_k.h"
#include "esp8266_tencent_firmware.h"
#include "tencent_firmware_module_wrapper.h"

void mqtt_demo_task(void)
{
	char payload[256];

    mqtt_state_t state;

    char *product_id = "03UKNYBUZG";
    char *device_name = "at_dev";
    char *key = "ttOARy0PjYgzd9OSs4Z3RA==";

    device_info_t dev_info;

    esp8266_tencent_firmware_sal_init(HAL_UART_PORT_3);
    esp8266_tencent_firmware_join_ap("SheldonDai", "srnr6x9xbhmb0");

    strncpy(dev_info.product_id, product_id, PRODUCT_ID_MAX_SIZE);
    strncpy(dev_info.device_name, device_name, DEVICE_NAME_MAX_SIZE);
    strncpy(dev_info.device_serc, key, DEVICE_SERC_MAX_SIZE);
    tos_tf_module_info_set(&dev_info, TLS_MODE_PSK);

	mqtt_param_t init_params = DEFAULT_MQTT_PARAMS;
	if (tos_tf_module_mqtt_conn(init_params) != 0) {
		printf("module mqtt conn fail\n");
	} else {
		printf("module mqtt conn success\n");
	}

    if (tos_tf_module_mqtt_state_get(&state) != -1) {
        printf("MQTT: %s\n", state == MQTT_STATE_CONNECTED ? "CONNECTED" : "DISCONNECTED");
    }

	static char topic_name[TOPIC_MAX_SIZE] = {0};
	int size = snprintf(topic_name, TOPIC_MAX_SIZE, "%s/%s/data", product_id, device_name);

	if (size < 0 || size > sizeof(topic_name) - 1) {
		printf("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_name));
	}
	if (tos_tf_module_mqtt_sub(topic_name, QOS0) != 0) {
		printf("module mqtt sub fail\n");
	} else {
		printf("module mqtt sub success\n");
	}

	while (1) {
		HAL_Delay(1000);
		memset(payload, 0, 256);
		//*注意转义的处理，不同模组在json格式数据需要的转义处理有些差别*//
#if 0
#ifdef TRANSFER_LABEL_NEED
		snprintf(payload, 256, "{\\\"action\\\": \\\"publish_test\\\"\\, \\\"count\\\": \\\"%d\\\"}", count++);
#else
		snprintf(payload, 256, "{\"action\": \"publish_test\"\, \"count\": \"%d\"}", count++);
#endif
#else
        strncpy(payload, "{\\\"type\\\":\\\"get\\\"\\, \\\"clientToken\\\":\\\"03UKNYBUZG-0\\\"}", sizeof(payload));
#endif
		printf("message publish: %s\n", payload);
		if (tos_tf_module_mqtt_pub(topic_name, QOS0, payload) != 0) {
			printf("module mqtt pub fail\n");
			break;
		} else {
			printf("module mqtt pub success\n");
		}

		if (tos_tf_module_mqtt_publ(topic_name, QOS0, payload) != 0) {
			printf("module mqtt publ fail\n");
			break;
		} else {
			printf("module mqtt publ success\n");
		}
	}
}

void application_entry(void *arg)
{
    mqtt_demo_task();
    while (1) {
        printf("This is a mqtt demo!\r\n");
        tos_task_delay(1000);
    }
}

