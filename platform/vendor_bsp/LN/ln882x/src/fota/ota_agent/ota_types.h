#ifndef __OTA_TYPES_H__
#define __OTA_TYPES_H__

#include "ln_types.h"

typedef enum {
    IMAGE_ATTACHE           = 0u,        /*< it's an attache drvice image */
	IMAGE_TYPE_ORIGINAL     = 1u,        /*< it's an original image */
	IMAGE_TYPE_ORIGINAL_XZ  = 2u,        /*< it's an original image which is zipped */
	IMAGE_TYPE_DIFF         = 3u,        /*< it's a diff image */
	IMAGE_TYPE_DIFF_XZ      = 4u,        /*< it's a diff image which is zipped */
    IMAGE_TYPE_PLACE = 0x12345678,       /*< make different IDEs compatible */
} image_type_t;

typedef struct {
	uint8_t           ver_major;         /*< major version number */
	uint8_t           ver_minor;         /*< minor version number */
} image_ver_t;

typedef struct  {
	image_type_t      image_type;        /*< what's the image type */
	image_ver_t       ver;               /*< image version number */
	image_ver_t       ver_diff_depend;   /*< which version of the file that diff image depend on */
    uint32_t          img_size_orig;     /*< size of original image  */
    uint32_t          img_size_orig_xz;  /*< size of original image which is zipped */
    uint32_t          img_size_diff;     /*< size of diff image */
    uint32_t          img_size_diff_xz;  /*< size of diff image which is zippede */
    uint32_t          img_crc32_orig;    /*< checksum of original image  */
    uint32_t          img_crc32_orig_xz; /*< checksum of original image which is zipped */
    uint32_t          img_crc32_diff;    /*< checksum of diff image */
    uint32_t          img_crc32_diff_xz; /*< checksum of diff image which is zippede */
    uint8_t           res[212];          /*< reserved space */
    uint32_t          header_crc32;      /*< checksum of the image header except for itself */
} image_hdr_t;

typedef enum {
	UPG_STATE_DOWNLOAD_ING  = 0u,        /*< ota image downloading */
    UPG_STATE_DOWNLOAD_OK   = 1u,        /*< ota image download finish and check success */
	UPG_STATE_RESTORE_ING   = 2u,        /*< ota image restoring */
	UPG_STATE_RESTORE_OK    = 3u,        /*< ota image restore finish and check success */
    UPG_STATE_RESTORE_FILED = 4u,        /*< ota image restore filed */
    UPG_STATE_PLACE = 0x12345678,        /*< make different IDEs compatible */
} upg_state_t;

#define KV_OTA_UPG_STATE    "kv_ota_upg_state"


#endif /*< __OTA_TYPES_H__ */


