#include "ota_image.h"
#include "ota_port.h"


#define TEMP_BUF_LEN    (128)
static uint8_t temp_buf[TEMP_BUF_LEN];

int image_header_read(partition_type_t type, image_hdr_t *header)
{
    partition_info_t info;
    ota_port_ctx_t * port = ota_get_port_ctx();
    
    if ((type != PARTITION_TYPE_APP) && (type != PARTITION_TYPE_OTA)) {
        return OTA_ERR_INVALID_PARAM;
    }
    
    if (LN_TRUE == ln_fetch_partition_info(type, &info)) {
        port->flash_drv.read(info.start_addr, (uint8_t*)header, sizeof(image_hdr_t));
        return OTA_ERR_NONE;
    }

    return OTA_ERR_IMG_HEADER_READ;
}

int image_header_fast_read(uint32_t start_addr, image_hdr_t *header)
{
    ota_port_ctx_t * port = ota_get_port_ctx();

    port->flash_drv.read(start_addr, (uint8_t*)header, sizeof(image_hdr_t));

    return OTA_ERR_NONE;
}

int image_header_verify(image_hdr_t *header)
{
    if (header->header_crc32 == ln_crc32_signle_cal((uint8_t *)header, sizeof(image_hdr_t) - sizeof(uint32_t))) {
        return OTA_ERR_NONE;
    }

    return OTA_ERR_IMG_HEADER_VERIFY;
}

int image_body_verify(uint32_t start_addr, image_hdr_t *header)
{
    uint32_t offset = 0;
    uint32_t size = 0;
    uint32_t i = 0;
	uint32_t crc32_result = 0;
	crc32_ctx_t crc_ctx = {0,};
    ota_port_ctx_t * port = ota_get_port_ctx();
    
    if (!header) {
        return OTA_ERR_INVALID_PARAM;
    }
    
    offset = start_addr + sizeof(image_hdr_t);

    switch (header->image_type)
    {
        case IMAGE_TYPE_ORIGINAL:
            size = header->img_size_orig;
            break;
        case IMAGE_TYPE_ORIGINAL_XZ:
            size = header->img_size_orig_xz;
            break;
        case IMAGE_TYPE_DIFF:
            size = header->img_size_diff;
            break;
        case IMAGE_TYPE_DIFF_XZ:
            size = header->img_size_diff_xz;
            break;
        default:
            return OTA_ERR_IMG_TYPE;
    }
    
    /* calculate CRC32 checksum of flash image segment by segment */
    ln_crc32_init(&crc_ctx);
    
    for (i = 0; i < size/TEMP_BUF_LEN; i++) {
        port->flash_drv.read(offset, temp_buf, TEMP_BUF_LEN);
        
        ln_crc32_update(&crc_ctx, temp_buf, TEMP_BUF_LEN);

        offset += TEMP_BUF_LEN;
    }
    
    port->flash_drv.read(offset, temp_buf, size % TEMP_BUF_LEN);
    ln_crc32_update(&crc_ctx, temp_buf, size % TEMP_BUF_LEN);
    crc32_result = ln_crc32_final(&crc_ctx);
    
    /* check the CRC32 checksum */
    switch (header->image_type)
    {
        case IMAGE_TYPE_ORIGINAL:
            return ((crc32_result == header->img_crc32_orig) ? OTA_ERR_NONE : OTA_ERR_IMG_ENTITY_VERIFY);

        case IMAGE_TYPE_ORIGINAL_XZ:
            return ((crc32_result == header->img_crc32_orig_xz) ? OTA_ERR_NONE : OTA_ERR_IMG_ENTITY_VERIFY);

        case IMAGE_TYPE_DIFF:
            return ((crc32_result == header->img_crc32_diff) ? OTA_ERR_NONE : OTA_ERR_IMG_ENTITY_VERIFY);

        case IMAGE_TYPE_DIFF_XZ:
            return ((crc32_result == header->img_crc32_diff_xz) ? OTA_ERR_NONE : OTA_ERR_IMG_ENTITY_VERIFY);

        default:
            return OTA_ERR_IMG_TYPE;
    }
}




