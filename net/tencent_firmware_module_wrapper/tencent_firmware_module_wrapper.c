#include "tencent_firmware_module_wrapper.h"

static tencent_firmware_module_t *g_tencent_firmware_module = NULL;

static k_task_t  mqtt_message_handle_task;
static k_stack_t mqtt_message_handle_task_stack[MQTT_MESSAGE_HANDLE_TASK_STACK_SIZE];

static mqtt_message_t mqtt_message;
static uint8_t mqtt_message_pool[MQTT_MESSAGE_POOL_SIZE];

static k_list_t mqtt_sub_list;
static k_mmblk_pool_t sub_list_node_mbp;
static mqtt_message_handlers_t sub_list_node_pool[MQTT_SUB_TOPIC_HANDLES_POOL_SIZE];

k_mail_q_t mqtt_message_mail;

void mqtt_message_handle_task_entry(void *arg)
{
    /*
        topic:$thing/down/property/xxx/xxx
        payload:
        "{
            "method":"xxx",
            "clientToken":"xxx",
            "code":0,
            "status":"success"
        }"
    */
    size_t message_size;
    k_list_t *cur;
    mqtt_message_handlers_t *mqtt_message_handler;
    
    while (K_TRUE) {
        tos_mail_q_pend(&mqtt_message_mail, &mqtt_message, &message_size, TOS_TIME_FOREVER);

        TOS_LIST_FOR_EACH(cur, &mqtt_sub_list) {
            mqtt_message_handler = TOS_LIST_ENTRY(cur, mqtt_message_handlers_t, list);

            if (strcmp(mqtt_message_handler->topic_filter, mqtt_message.topic) == 0) {
                mqtt_message_handler->handler(&mqtt_message);
            }
        }
    }
}


int tos_tf_module_register(tencent_firmware_module_t *module)
{
    if (!g_tencent_firmware_module) {
        g_tencent_firmware_module = module;
        return 0;
    }
    return -1;
}

int tos_tf_module_init(void)
{        
    if (tos_mail_q_create(&mqtt_message_mail, mqtt_message_pool, MQTT_MESSAGE_NUM_MAX, sizeof(mqtt_message_t)) != K_ERR_NONE) {
        return -2;
    }
    
    tos_list_init(&mqtt_sub_list);

    if (tos_mmblk_pool_create(&sub_list_node_mbp, sub_list_node_pool, MQTT_SUB_TOPIC_MAX, sizeof(mqtt_message_handlers_t)) != K_ERR_NONE) {
        return -3;
    }
    
    if (tos_task_create(&mqtt_message_handle_task, "mqtt_message_handle", 
                        mqtt_message_handle_task_entry, NULL, MQTT_MESSAGE_HANDLE_TASK_PRIO,
                        mqtt_message_handle_task_stack, MQTT_MESSAGE_HANDLE_TASK_STACK_SIZE, 10) != K_ERR_NONE) {
        return -4;
    }
    
    if (g_tencent_firmware_module && g_tencent_firmware_module->init) {
        return g_tencent_firmware_module->init();
    }

    return -1;
}

int tos_tf_module_info_set(device_info_t *info, tls_mode_t mode)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->info_set) {
        return g_tencent_firmware_module->info_set(info, mode);
    }
    return -1;
}

int tos_tf_module_mqtt_conn(mqtt_param_t init_params)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->mqtt_conn) {
        return g_tencent_firmware_module->mqtt_conn(init_params);
    }
    return -1;
}

int tos_tf_module_mqtt_discon(void)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->mqtt_discon) {
        return g_tencent_firmware_module->mqtt_discon();
    }
    return -1;
}

int tos_tf_module_mqtt_pub(const char *topic, qos_t qos, char *payload)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->mqtt_pub) {
        return g_tencent_firmware_module->mqtt_pub(topic, qos, payload);
    }
    return -1;
}

int tos_tf_module_mqtt_publ(const char *topic, qos_t qos, char *payload)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->mqtt_publ) {
        return g_tencent_firmware_module->mqtt_publ(topic, qos, payload);
    }
    return -1;
}

int tos_tf_module_mqtt_sub(char *topic, qos_t qos, message_handler_t handle)
{
    mqtt_message_handlers_t *mqtt_message_handler;
    
    if (tos_mmblk_alloc(&sub_list_node_mbp, (void*)&mqtt_message_handler) != K_ERR_NONE) {
        return -1;
    }
    
    mqtt_message_handler->topic_filter = topic;
    mqtt_message_handler->qos = qos;
    mqtt_message_handler->handler = handle;
    
    tos_list_add_tail(&mqtt_message_handler->list, &mqtt_sub_list);
    
    if (g_tencent_firmware_module && g_tencent_firmware_module->mqtt_sub) {
        return g_tencent_firmware_module->mqtt_sub(topic, qos);
    }
    return -1;
}

int tos_tf_module_mqtt_unsub(const char *topic)
{
    k_list_t *cur;
    mqtt_message_handlers_t *mqtt_message_handler;
        
    TOS_LIST_FOR_EACH(cur, &mqtt_sub_list) {
        mqtt_message_handler = TOS_LIST_ENTRY(cur, mqtt_message_handlers_t, list);

        if (strcmp(mqtt_message_handler->topic_filter, topic) == 0) { 
            tos_list_del(&mqtt_message_handler->list);
            if (tos_mmblk_free(&sub_list_node_mbp, (void*)&mqtt_message_handler) != K_ERR_NONE) {
                return -1;
            }
        }
    }
    
    if (g_tencent_firmware_module && g_tencent_firmware_module->mqtt_unsub) {
        return g_tencent_firmware_module->mqtt_unsub(topic);
    }
    return -1;
}

int tos_tf_module_mqtt_state_get(mqtt_state_t *state)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->mqtt_state_get) {
        return g_tencent_firmware_module->mqtt_state_get(state);
    }
    return -1;
}

int tos_tf_module_debug_level_set(int log_level)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->debug_level_set) {
        return g_tencent_firmware_module->debug_level_set(log_level);
    }
    return -1;
}

int tos_tf_module_ota_set(ota_mode_t mode, char *version)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->ota_set) {
        return g_tencent_firmware_module->ota_set(mode, version);
    }
    return -1;
}

int tos_tf_module_ota_read_fwinfo(ota_fw_info_t *ota_fw_info)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->ota_read_fwinfo) {
        return g_tencent_firmware_module->ota_read_fwinfo(ota_fw_info);
    }
    return -1;
}

int tos_tf_module_ota_read_fwdata(uint8_t *ota_fw_data_buffer,uint16_t buf_size)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->ota_read_fwdata) {
        return g_tencent_firmware_module->ota_read_fwdata(ota_fw_data_buffer, buf_size);
    }
    return -1;
}

int tos_tf_module_smartconfig_start(void)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->start_smartconfig) {
        return g_tencent_firmware_module->start_smartconfig();
    }
    return -1;
}

int tos_tf_module_smartconfig_stop(void)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->stop_smartconfig) {
        return g_tencent_firmware_module->stop_smartconfig();
    }
    return -1;
}
