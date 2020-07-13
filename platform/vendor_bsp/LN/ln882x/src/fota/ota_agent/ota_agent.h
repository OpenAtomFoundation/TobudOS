#ifndef __OTA_API_H__
#define __OTA_API_H__

#include "ota_err.h"
#include "ota_types.h"
#include "ota_port.h"

typedef void (*jump_to_app_t)(uint32_t app_entity_offset);

int ota_boot_upgrade_agent(jump_to_app_t jump_to_app);



#endif /* __OTA_API_H__ */

