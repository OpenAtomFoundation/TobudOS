#include "lora_module_wrapper.h"

static lora_module_t *g_lora_module = NULL;

int tos_lora_module_register(lora_module_t *module)
{
    if (!g_lora_module) {
        g_lora_module = module;
        return 0;
    }

    return -1;
}

int tos_lora_module_init(void)
{
    if (g_lora_module && g_lora_module->init) {
        return g_lora_module->init();
    }
    return -1;
}

int tos_lora_module_join_otaa(const char *deveui, const char *appkey)
{
    if (g_lora_module && g_lora_module->join_otaa) {
        return g_lora_module->join_otaa(deveui, appkey);
    }
    return -1;
}

int tos_lora_module_join_abp(const char *deveui, const char *devaddr, const char *nwkskey, const char *appskey)
{
    if (g_lora_module && g_lora_module->join_abp) {
        return g_lora_module->join_abp(deveui, devaddr, nwkskey, appskey);
    }
    return -1;
}

int tos_lora_module_send(const void *buf, size_t len)
{
    if (g_lora_module && g_lora_module->send) {
        return g_lora_module->send(buf, len);
    }
    return -1;
}

int tos_lora_module_recvcb_register(lora_recv_callback_t recv_callback)
{
    if (g_lora_module) {
        g_lora_module->recv_callback = recv_callback;
        return 0;
    }
    return -1;
}

int tos_lora_module_close(void)
{
    if (g_lora_module && g_lora_module->close) {
        return g_lora_module->close();
    }
    return -1;
}
