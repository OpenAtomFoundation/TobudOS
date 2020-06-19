#ifndef __FLASH_PARTITION_MGR_H__
#define __FLASH_PARTITION_MGR_H__
#include "ln_types.h"


#define PARTITION_TAB_BASE        (1024*36)/* < Warning: Do not change it! */
#define PARTITION_TAB_SIZE        (1024*4) /* < Warning: Do not change it! */

typedef enum {
    PARTITION_TYPE_APP         = 0u,     /*< application */
    PARTITION_TYPE_OTA         = 1u,     /*< ota temp*/
    PARTITION_TYPE_KV          = 2u,     /*< Key-Value */
	PARTITION_TYPE_NVDS        = 3u,     /*< nvds(NVRAM) */
	PARTITION_TYPE_SIMU_EEPROM = 4u,     /*< simulation eeprom*/
    PARTITION_TYPE_USER        = 5u,     /*< user defined */
    PARTITION_TYPE_PLACE = 0x12345678,   /*< make different IDEs compatible */
} partition_type_t;

typedef struct {
    partition_type_t  type;              /*< what's the type of partition */
    uint32_t          start_addr;        /*< start address */
    uint32_t          size;              /*< partition size */
    uint32_t          crc32;             /*< checksum of the partition info except itself */
} partition_info_t;


/**
 * @brief Verify partition table.
 * verify the main partition table of flash.
 *
 * @attention None
 *
 * @return  whether the verification is successful.
 * @retval  #LN_TRUE     successful.
 * @retval  #LN_FALSE    failed.
 */
int ln_verify_partition_table(void);

/**
 * @brief fetch partition info.
 * Fetch partition information for a specified type partition.
 *
 * @attention None
 *
 * @param[in]    type    types indicating the type of the partition entity, see PARTITION_TYPE_*.
 * @param[out]   info    pointer to a partition info struct.
 *
 * @return  Whether to fetch info successfully.
 * @retval  #LN_TRUE     successful.
 * @retval  #LN_FALSE    failed.
 */
int ln_fetch_partition_info(partition_type_t type, partition_info_t * info);



#endif /* __FLASH_PARTITION_MGR_H__ */


