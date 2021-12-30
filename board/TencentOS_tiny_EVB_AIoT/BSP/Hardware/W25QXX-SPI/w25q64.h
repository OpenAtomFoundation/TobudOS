#ifndef _W25Q64_H_
#define _W25Q64_H_

//#define USE_ST_HAL
#define USE_NXP_FSL

#if defined(USE_ST_HAL)
#include "spi.h"

#define SPI_Handle  hspi1
#define SPI_CS_PORT GPIOA
#define SPI_CS_PIN  GPIO_PIN_4

#elif defined(USE_NXP_FSL)
#include "fsl_gpio.h"
#include "fsl_lpspi.h"

#define SPI_Handle  LPSPI3
#define SPI_CS_PORT GPIO1
#define SPI_CS_PIN  3
/* Select USB1 PLL PFD0 (720 MHz) as lpspi clock source */
#define LPSPI_CLOCK_SOURCE_SELECT (1U)
/* Clock divider for master lpspi clock source */
#define LPSPI_CLOCK_SOURCE_DIVIDER (7U)
/* 90MHz */
#define LPSPI_MASTER_CLK_FREQ (CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk) / (LPSPI_CLOCK_SOURCE_DIVIDER + 1U))
/*! Transfer baudrate - 80M */
#define TRANSFER_BAUDRATE 80000000U
#endif /* USE_ST_HAL or USE_NXP_FSL */

enum {
    ManufactDeviceID_CMD    = 0x90,
    READ_STATU_REGISTER_1   = 0x05,
    READ_STATU_REGISTER_2   = 0x35,
    READ_DATA_CMD           = 0x03,
    WRITE_ENABLE_CMD        = 0x06,
    WRITE_DISABLE_CMD       = 0x04,
    SECTOR_ERASE_CMD        = 0x20,
    CHIP_ERASE_CMD          = 0xc7,
    PAGE_PROGRAM_CMD        = 0x02,
};

int w25qxx_init(void);
uint16_t w25qxx_read_deviceid(void);
int w25qxx_read(uint8_t* buffer, uint32_t start_addr, uint16_t nbytes);
int w25qxx_erase_sector(uint32_t sector_addr);
int w25qxx_page_program(uint8_t* dat, uint32_t write_addr, uint16_t nbytes);

#endif
