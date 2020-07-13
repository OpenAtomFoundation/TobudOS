#ifndef __OTA_PORT_H__
#define __OTA_PORT_H__

#include "ln_types.h"
#include "ln_kv_err.h"
#include "ota_err.h"
#include "utils/crc32.h"

#define LEN_4KB    (1024*4)

typedef int   (*ota_flash_write_t)(uint32_t addr, const void *buf, uint32_t len);
typedef int   (*ota_flash_read_t)(uint32_t addr, void *buf, uint32_t len);
typedef int   (*ota_flash_erase_t)(uint32_t addr, uint32_t len);
typedef int   (*ota_kv_set_t)(const char *key, const void *value, size_t v_len);
typedef int   (*ota_kv_get_t)(const char *key, void *value_buf, size_t value_buf_size, size_t *v_len);
typedef void  (*chip_reboot_t)(void);

typedef struct {
    ota_flash_write_t  write;
    ota_flash_read_t   read;
    ota_flash_erase_t  erase;
} ota_flash_drv_t;

typedef struct {
    ota_kv_set_t       kv_set;
    ota_kv_get_t       kv_get;
} ota_kv_ops_t;

typedef struct {
    ota_flash_drv_t    flash_drv;
    ota_kv_ops_t       kv_ops;
    chip_reboot_t      chip_reboot;
} ota_port_ctx_t;


/**
 * @brief ota port init.
 * initialize some hardware api that ota module depend on.
 *
 * @attention None.
 *
 * @return  errcode
 * @retval  #OTA_ERR_NONE   initialize ok.
 * @retval  #OTA_ERR_*      initialize failed.
 */
ota_err_t        ota_port_init(void);

/**
 * @brief get ota port context.
 * get ota port context.
 *
 * @attention None.
 *
 * @return  the pointer of ota port context.
 */
ota_port_ctx_t * ota_get_port_ctx(void);


#endif /* __OTA_PORT_H__ */

