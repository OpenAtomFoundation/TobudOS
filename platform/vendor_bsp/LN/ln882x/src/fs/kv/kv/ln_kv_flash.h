#ifndef  __LN_KV_FLASH_H__
#define  __LN_KV_FLASH_H__

#include "ln_types.h"

typedef int (*kv_flash_write_t)(uint32_t addr, const void *buf, size_t len);
typedef int (*kv_flash_read_t)(uint32_t addr, void *buf, size_t len);
typedef int (*kv_flash_erase_t)(uint32_t addr, size_t len);

typedef struct kv_flash_drv_st {
    kv_flash_write_t    write;
    kv_flash_read_t     read;
    kv_flash_erase_t    erase;
} kv_flash_drv_t;

typedef struct kv_flash_property_st {
    uint8_t             sector_size_log2;
} kv_flash_prop_t;

#endif /* __LN_KV_FLASH_H__ */

