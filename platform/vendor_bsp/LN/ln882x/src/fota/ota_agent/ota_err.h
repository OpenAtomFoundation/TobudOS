#ifndef __OTA_ERR_H__
#define __OTA_ERR_H__

typedef enum {
    OTA_ERR_NONE = 0u,
    OTA_ERR_INVALID_PARAM,
    
    OTA_ERR_KV_RW = 10u,
    OTA_ERR_UPG_STATE,
    OTA_ERR_PARTITION_TAB,
    OTA_ERR_DECOMPRESS,
    
    OTA_ERR_IMG_TYPE = 20u,
    OTA_ERR_IMG_HEADER_READ,
    OTA_ERR_IMG_HEADER_VERIFY,
    OTA_ERR_IMG_ENTITY_VERIFY,
    
    OTA_ERR_IMPOSSIBLE_VER = 30u,
    OTA_ERR_NOT_SUPPORT,
    
} ota_err_t;

#endif /* __OTA_ERR_H__ */

