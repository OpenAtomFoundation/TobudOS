/**
 * @file    wm_ram_config.h
 *
 * @brief   WM ram model configure
 *
 * @author  winnermicro
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef __WM_RAM_CONFIG_H__
#define __WM_RAM_CONFIG_H__

/*OS Task Stack's lower & upper range*/
#define TASK_STACK_USING_MEM_LOWER_RANGE (0x20000000UL)
#define TASK_STACK_USING_MEM_UPPER_RANGE (0x20028000UL)

/*0x20000000~MASTER_SPI_DMA_ADDR for stack,heap & data*/

/*Master SPI use buffer when spi use dma transfer mode*/
#define MASTER_SPI_DMA_ADDR        0x20038000UL

/*High speed SPI or SDIO buffer to exchange data*/
#define SLAVE_HSPI_SDIO_ADDR        0x2003A000UL

/*Wi-Fi use buffer to exchange data*/
#define WIFI_MEM_START_ADDR		0x2003C000UL

#endif /*__WM_RAM_CONFIG_H__*/

