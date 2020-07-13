#ifndef __OTA_IMAGE_H__
#define __OTA_IMAGE_H__

#include "flash_partition_mgr.h"
#include "ota_types.h"
#include "ota_err.h"


int  image_header_read(partition_type_t type, image_hdr_t * header);
int  image_header_fast_read(uint32_t start_addr, image_hdr_t *header);
int  image_header_verify(image_hdr_t * header);
int  image_body_verify(uint32_t start_addr, image_hdr_t *header);


#endif /* __OTA_IMAGE_H__ */


