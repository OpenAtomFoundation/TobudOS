#include "crc8.h"
#include "ota_image.h"

uint8_t ota_img_hdr_crc(ota_img_hdr_t *img_hdr)
{
    uint8_t crc = 0;

    crc = crc8(crc, (uint8_t *)&img_hdr->magic, sizeof(uint16_t));
    crc = crc8(crc, (uint8_t *)&img_hdr->new_version, sizeof(ota_img_vs_t));
    crc = crc8(crc, (uint8_t *)&img_hdr->old_version, sizeof(ota_img_vs_t));

    crc = crc8(crc, (uint8_t *)&img_hdr->new_crc, sizeof(uint8_t));
    crc = crc8(crc, (uint8_t *)&img_hdr->new_size, sizeof(uint32_t));

    crc = crc8(crc, (uint8_t *)&img_hdr->old_crc, sizeof(uint8_t));
    crc = crc8(crc, (uint8_t *)&img_hdr->old_size, sizeof(uint32_t));

    crc = crc8(crc, (uint8_t *)&img_hdr->patch_size, sizeof(uint32_t));

    return crc;
}

