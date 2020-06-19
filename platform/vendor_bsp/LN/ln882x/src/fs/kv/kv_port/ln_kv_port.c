#include "ln_kv_port.h"
#include "ln_kv_flash.h"
#include "ln_kv.h"
#include "hal/flash.h"


static kv_flash_drv_t   flash_drv;
static kv_flash_prop_t  flash_prop;

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

kv_err_t ln_kv_port_init(uint32_t flash_start, uint32_t flash_end)
{
    flash_drv.read  = flash_read;
    flash_drv.write = flash_write;
    flash_drv.erase = flash_erase;

    flash_prop.sector_size_log2 = KV_CONFIG_FLASH_SECT_SIZE_LOG2;

    return ln_kv_init(flash_start, flash_end, &flash_drv, &flash_prop);
}


