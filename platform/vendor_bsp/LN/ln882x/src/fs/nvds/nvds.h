#ifndef __NVDS_H__
#define __NVDS_H__

#include "ln88xx.h"
#include "flash_partition_table.h"

#define USR_NVDS_SECT1_OFFSET    (NVDS_SPACE_OFFSET)
#define USR_NVDS_SECT2_OFFSET    (USR_NVDS_SECT1_OFFSET + SIZE_4KB)
#define USR_NVDS_FLAG_OFFSET     (USR_NVDS_SECT2_OFFSET + SIZE_4KB)

#define NVDS_SECT_MAGIC_CODE          (0x4E564453)
#define NVDS_NONE_TAG                 (0xFF)

typedef enum
{
    NVDS_OK   = 0,
    NVDS_FAIL = -1, 
} nvds_status_t;

typedef enum
{
    NVDS_SECT1_VALID = 0xA5A5,
    NVDS_SECT2_VALID = 0x5A5A,
}nvds_valid_sect_t;


//User Non-Volatile Data Mapping
#define SECT_MAGIC_CODE_OFFSET     (0)
#define SECT_MAGIC_CODE_LEN        (4)

#define BT_MAC_OFFSET              (SECT_MAGIC_CODE_OFFSET + SECT_MAGIC_CODE_LEN)
#define BT_MAC_LEN                 (6)

#define BT_ID_OFFSET               (BT_MAC_OFFSET + BT_MAC_LEN)
#define BT_ID_LEN                  (24)

#define CHIP_SN_OFFSET             (BT_ID_OFFSET + BT_ID_LEN)
#define CHIP_SN_LEN                (20)

#define XTAL_CAP_COMP_OFFSET       (CHIP_SN_OFFSET + CHIP_SN_LEN)
#define XTAL_CAP_COMP_LEN          (1)

#define TX_POWER_COMP_OFFSET       (XTAL_CAP_COMP_OFFSET + XTAL_CAP_COMP_LEN)
#define TX_POWER_COMP_LEN          (1)


//if you add new data into the nvds,update it!
#if ((TX_POWER_COMP_OFFSET + TX_POWER_COMP_LEN) > SIZE_4KB) 
  #error "nvds data overrun!!!"
#endif


//nvds public api
int nvds_write(uint32_t offset, uint8_t* data, uint16_t len);
int nvds_read(uint32_t offset, uint8_t* data, uint16_t len);



#endif /* __NVDS_H__ */

