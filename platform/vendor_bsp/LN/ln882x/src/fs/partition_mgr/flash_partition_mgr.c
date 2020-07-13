#include "flash_partition_mgr.h"

#include "hal/flash.h"
#include "utils/crc32.h"


int ln_verify_partition_table(void)
{
    partition_info_t rd_info;
    
    for (uint32_t i = 0; i < PARTITION_TAB_SIZE; i += sizeof(partition_info_t))
    {
        FLASH_Read((PARTITION_TAB_BASE + i), sizeof(partition_info_t), (uint8_t *)&rd_info);

        if ((rd_info.type       == 0) && \
            (rd_info.start_addr == 0) && \
            (rd_info.size       == 0) && \
            (rd_info.crc32      == 0))
        {
            return LN_TRUE;
        }
        else
        {
            if (rd_info.crc32 != ln_crc32_signle_cal((uint8_t *)&rd_info, sizeof(partition_info_t) - sizeof(uint32_t))) {
                return LN_FALSE;
            }
        }
    }
    return LN_FALSE;
}

int ln_fetch_partition_info(partition_type_t type, partition_info_t *info)
{
    if (!info) {
        return LN_FALSE;
    }

    for (uint32_t i = 0; i < PARTITION_TAB_SIZE; i += sizeof(partition_info_t))
    {
        FLASH_Read((PARTITION_TAB_BASE + i), sizeof(partition_info_t), (uint8_t *)info);

        if (info->type == type)
        {
            if (info->crc32 == ln_crc32_signle_cal((uint8_t *)info, sizeof(partition_info_t) - sizeof(uint32_t))) {
                return LN_TRUE;
            } else {
                return LN_FALSE;
            }
        } 
        else if ((info->type       == 0) && \
                 (info->start_addr == 0) && \
                 (info->size       == 0) && \
                 (info->crc32      == 0))
        {
            return LN_FALSE;
        }
    }
    return LN_FALSE;
}


