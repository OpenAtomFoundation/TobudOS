#ifndef _W25QXX_H_
#define _W25QXX_H_
#include "main.h"

enum {
    ManufactDeviceID_CMD    = 0x90,
    READ_STATU_REGISTER_1   = 0x05,
    READ_STATU_REGISTER_2   = 0x35,
    WRITE_STATU_REGISTER    = 0x01,
    READ_DATA_CMD           = 0x03,
    FAST_READ_CMD           = 0x0B,
    FAST_READ_DUAL_O_CMD    = 0x3B,
    FAST_READ_DUAL_IO_CMD   = 0xBB,
    FAST_READ_QUAD_O_CMD    = 0x6B,
    FAST_READ_QUAD_IO_CMD   = 0xEB,
    WRITE_ENABLE_CMD        = 0x06,
    WRITE_DISABLE_CMD       = 0x04,
    SECTOR_ERASE_CMD        = 0x20,
    CHIP_ERASE_CMD          = 0xc7,
    PAGE_PROGRAM_CMD        = 0x02,
    QUAD_PAGE_PROGRAM_CMD   = 0x32,
    BLOCK_ERASE_64KB_CMD    = 0xD8,
    BLOCK_ERASE_32KB_CMD    = 0x52,
    ERASE_SUSPEND_CMD       = 0x75,
    ERASE_RESUME_CMD        = 0x7A,
    HIGH_PERFORM_MODE_CMD   = 0xA3,
    ENABLE_RESET_CMD        = 0x66,
    RESET_DEVICE_CMD        = 0x99,
};

int w25qxx_init(void);
int w25qxx_memory_mapped(void);
uint16_t w25qxx_read_deviceid(void);
int w25qxx_read(uint8_t* buffer, uint32_t start_addr, uint16_t nbytes);
int w25qxx_erase_sector(uint32_t sector_addr);
int w25qxx_erase_chip(void);
int w25qxx_page_program(uint8_t* dat, uint32_t write_addr, uint16_t nbytes);
int w25qxx_reset(void);

#endif /* _W25QXX_H_ */
