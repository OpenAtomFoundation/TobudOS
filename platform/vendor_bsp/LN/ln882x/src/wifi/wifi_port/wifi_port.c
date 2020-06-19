#include "wifi_port.h"
#include "ln_kv_err.h"
#include "ln_kv_api.h"
#include "itypes.h"
#include "osal/osal.h"

#define KV_PSK_INFO    ("kv_psk_info")

/* restore/store psk info */
int kv_set_psk_info_list(uint8_t *v_buf, uint32_t buf_len)
{
    if (KV_ERR_NONE != ln_kv_set((const char *)KV_PSK_INFO, v_buf, (size_t)buf_len)) {
        return BFALSE;
    }
    return BTRUE;
}

int kv_get_psk_info_list(uint8_t *v_buf, uint32_t buf_len)
{
    size_t len;
    if (KV_ERR_NONE != ln_kv_get((const char *)KV_PSK_INFO, v_buf, (size_t)buf_len, &len)) {
        return BFALSE;
    }
    return BTRUE;
}

int kv_has_psk_info_list(void)
{
    size_t len;
    if (LN_TRUE == ln_kv_has_key((const char *)KV_PSK_INFO)) {
        return BTRUE;
    }
    return BFALSE;
}

/* heap memory manager */
void *mac_malloc(uint32_t size)
{
    return OS_Malloc(size);
}

void mac_free(void *ptr)
{
    OS_Free(ptr);
}

















