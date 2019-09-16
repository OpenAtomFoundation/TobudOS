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

int tos_lora_module_join(void)
{
    if (g_lora_module && g_lora_module->join) {
        return g_lora_module->join();
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

int tos_lora_module_recv_register(void* mcps_indication)
{
    if (g_lora_module && g_lora_module->recv_register) {
        return g_lora_module->recv_register(mcps_indication);
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
