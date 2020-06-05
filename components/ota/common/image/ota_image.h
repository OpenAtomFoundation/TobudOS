#ifndef _OTA_IMAGE_H_
#define _OTA_IMAGE_H_

#include "stdint.h"

#define OTA_IMAGE_MAGIC             0xBADE

typedef struct ota_image_version_st {
    uint8_t     major;  /* major version number */
    uint8_t     minor;  /* minor version number */
} ota_img_vs_t;

#pragma pack(push, 1)
typedef struct ota_image_header_st {
    uint16_t            magic;
    ota_img_vs_t        new_version;
    ota_img_vs_t        old_version;

    uint8_t             new_crc;
    uint8_t             old_crc;
    uint8_t             patch_crc;

    uint32_t            new_size;
    uint32_t            old_size;
    uint32_t            patch_size;
} ota_img_hdr_t;
#pragma pack(pop)

uint8_t ota_img_hdr_crc(ota_img_hdr_t *img_hdr);

#endif /* _OTA_IMAGE_H_ */

