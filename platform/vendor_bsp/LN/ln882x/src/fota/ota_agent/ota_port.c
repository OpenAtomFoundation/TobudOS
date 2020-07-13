#include "ota_port.h"
#include "ota_err.h"
#include "hal/flash.h"
#include "ln_kv_api.h"
#include "ln_kv_err.h"

static ota_port_ctx_t ota_port;

static int flash_read(uint32_t offset, void *buf, uint32_t len)
{
    return FLASH_Read(offset, len, buf);
}

static int flash_write(uint32_t offset, const void *buf, uint32_t len)
{
    return FLASH_Program(offset, len, (uint8_t*)buf);
}

static int flash_erase(uint32_t offset, uint32_t len)
{
    FLASH_Erase(offset, len);
    return 0;
} 

static int ota_kv_set(const char *key, const void *value, size_t v_len)
{
    if (KV_ERR_NONE == ln_kv_set(key,value,v_len)) {
        return LN_TRUE;
    }
    return LN_FALSE;
}

static int ota_kv_get(const char *key, void *value_buf, size_t value_buf_size, size_t *v_len)
{
    if (KV_ERR_NONE == ln_kv_get(key,value_buf,value_buf_size,v_len)) {
        return LN_TRUE;
    }
    return LN_FALSE;
}

static void chip_reboot(void)
{

}

ota_err_t ota_port_init(void)
{
    ota_port.flash_drv.read  = flash_read;
    ota_port.flash_drv.write = flash_write;
    ota_port.flash_drv.erase = flash_erase;
    
    ota_port.kv_ops.kv_set   = ota_kv_set;
    ota_port.kv_ops.kv_get   = ota_kv_get;
    
    ota_port.chip_reboot     = chip_reboot;
    
    return OTA_ERR_NONE;
}

ota_port_ctx_t * ota_get_port_ctx(void)
{
    return &ota_port;
}









