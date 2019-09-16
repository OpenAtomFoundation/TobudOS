#include "tencent_firmware_module_wrapper.h"

static tencent_firmware_module_t *g_tencent_firmware_module = NULL;

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

int tos_tf_module_mqtt_sub(char *topic, qos_t qos)
{
    if (g_tencent_firmware_module && g_tencent_firmware_module->mqtt_sub) {
        return g_tencent_firmware_module->mqtt_sub(topic, qos);
    }
    return -1;
}

int tos_tf_module_mqtt_unsub(const char *topic)
{
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

