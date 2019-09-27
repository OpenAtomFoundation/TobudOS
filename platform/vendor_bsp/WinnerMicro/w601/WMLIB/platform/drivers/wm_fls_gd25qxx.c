/**
 * @file    wm_fls_gd25qxx.c
 *
 * @brief   gd25qxx flash Driver Module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include "wm_type_def.h"
#include "wm_flash.h"
#include "wm_hostspi.h"
#include "wm_fls_gd25qxx.h"
#include "wm_debug.h"
#include "wm_gpio.h"
#include "VS10XX.h"

static int tls_spifls_drv_read(u32 addr, u8 * buf, u32 len);
static int tls_spifls_drv_fast_read(u32 addr, u8 * buf, u32 len);
static int tls_spifls_drv_page_write(u32 page, u8 * buf);
static int tls_spifls_drv_erase(u32 sector);
static int tls_spifls_drv_chip_erase(void);
static int tls_spifls_drv_probe(u32 id);
static void tls_spifls_drv_remove(void);

static struct tls_fls_drv exspi_fls = {
    .drv_list = {NULL, NULL},
    .clock = SPI_SCLK,
    .mode = TLS_SPI_MODE_0,
    .cs_active = TLS_SPI_CS_LOW,
    .flags = 0,
    .read = tls_spifls_drv_read,
    .fast_read = tls_spifls_drv_fast_read,
    .page_write = tls_spifls_drv_page_write,
    .erase = tls_spifls_drv_erase,
    .chip_erase = tls_spifls_drv_chip_erase,
    .probe = tls_spifls_drv_probe,
    .remove = tls_spifls_drv_remove
};

static struct tls_fls_drv *exspifls_drv = NULL;


static unsigned int swap32(unsigned int v)
{
    return ((v & 0xff) << 24) | ((v & 0xff00) << 8) |
        ((v & 0xff0000) >> 8) | (v >> 24);
}

static int tls_spifls_drv_write_enable(void)
{
    u8 cmd;
    int err;

    cmd = EXSPIFLASH_WRITE_ENABLE;

    err = tls_spi_write((const u8 *) &cmd, 1);
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }

    return TLS_FLS_STATUS_OK;
}

static int tls_spifls_drv_wait_write_enable(void)
{
    u8 cmd;
    u8 sr;
    int err;

    cmd = EXSPIFLASH_READ_SR1;
    sr = 0;
    do
    {
        err = tls_spi_read_with_cmd((const u8 *) &cmd, 1, &sr, 1);
        if (err != TLS_SPI_STATUS_OK)
        {
            return TLS_FLS_STATUS_EIO;
        }

        if (sr & FLASH_STATUS_WEL)
        {
            break;
        }
    }
    while (1);

    return TLS_FLS_STATUS_OK;
}

static int tls_spifls_drv_wait_flash_ready(void)
{
    u8 cmd;
    u8 sr;
    int err;

    cmd = EXSPIFLASH_READ_SR1;
    sr = 0;
    do
    {
        err = tls_spi_read_with_cmd((const u8 *) &cmd, 1, &sr, 1);
        if (err != TLS_SPI_STATUS_OK)
        {
            return TLS_FLS_STATUS_EIO;
        }

        if ((sr & FLASH_STATUS_BUSY) == 0x00)
        {
            break;
        }
    }
    while (1);

    return TLS_FLS_STATUS_OK;
}

static int tls_spifls_drv_read(u32 addr, u8 * buf, u32 len)
{
    u32 cmd;
    int err;

    cmd = 0;
    cmd |= EXSPIFLASH_DATA_READ;
    cmd |= swap32(addr) & 0xffffff00;
    err = tls_spi_read_with_cmd((const u8 *) &cmd, 4, buf, len);
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }

    return TLS_FLS_STATUS_OK;
}

static int tls_spifls_drv_fast_read(u32 addr, u8 * buf, u32 len)
{
    return TLS_FLS_STATUS_ENOSUPPORT;
}

static int tls_spifls_drv_page_write(u32 page, u8 * buf)
{
    u32 cmd;
    int err;

    err = tls_spifls_drv_write_enable();
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }

    err = tls_spifls_drv_wait_write_enable();
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }

    cmd = 0;
    cmd |= EXSPIFLASH_PAGE_PROGRAM;
    cmd |= swap32(page * exspifls_drv->page_size) & 0xffffff00;
    err = tls_spi_write_with_cmd((const u8 *) &cmd, 4, (const u8 *) buf,
                               exspifls_drv->page_size);
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }

    err = tls_spifls_drv_wait_flash_ready();
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }

    return TLS_FLS_STATUS_OK;
}

static int tls_spifls_drv_erase(u32 sector)
{
    u32 cmd;
    int err;

    err = tls_spifls_drv_write_enable();
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }
    err = tls_spifls_drv_wait_write_enable();
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }
    cmd = 0;
    cmd |= EXSPIFLASH_SECTOR_ERASE;
    cmd |= swap32(sector * exspifls_drv->sector_size) & 0xffffff00;
    err = tls_spi_write((const u8 *) &cmd, 4);
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }
    tls_os_time_delay(6);
    err = tls_spifls_drv_wait_flash_ready();
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }
    return TLS_FLS_STATUS_OK;
}

static int tls_spifls_drv_chip_erase(void)
{
    u8 cmd;
    int err;

    err = tls_spifls_drv_write_enable();
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }

    err = tls_spifls_drv_wait_write_enable();
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }

    cmd = EXSPIFLASH_CHIP_ERASE;
    err = tls_spi_write((const u8 *) &cmd, 1);
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }

    err = tls_spifls_drv_wait_flash_ready();
    if (err != TLS_SPI_STATUS_OK)
    {
        return TLS_FLS_STATUS_EIO;
    }

    return TLS_FLS_STATUS_OK;
}

static int tls_spifls_drv_probe(u32 id)
{
//	int i = 0;


    if (!id)
    {
        return TLS_FLS_STATUS_EINVAL;
    }

    exspi_fls.id = id;
    if ((id>>16)&0xFF)
    {
    	exspi_fls.total_size = 1 << (id>>16);
    }else{
    	exspi_fls.total_size = FLASH_TOTAL_SIZE;  /*1MByte*/
    }
    
    exspi_fls.page_size = PAGE_SIZE;
    exspi_fls.program_size = PROGRAM_SIZE;
    exspi_fls.sector_size = SECTOR_SIZE;
    
    
    exspifls_drv = &exspi_fls;
    return TLS_FLS_STATUS_OK;
}

static void tls_spifls_drv_remove(void)
{
    exspifls_drv = NULL;
}


/**
 * @brief          This function is used to install gd25qxx driver.
 *
 * @param[in]      None
 *
 * @retval         TLS_FLS_STATUS_OK	         if write flash success
 * @retval         TLS_FLS_STATUS_EPERM     if flash struct point is null
 * @retval         TLS_FLS_STATUS_ENODRV	  if flash driver is not installed
 * @retval         TLS_FLS_STATUS_EINVAL	  if argument is invalid
 * @retval         TLS_FLS_STATUS_EIO	         if io error
 * @retval         TLS_FLS_STATUS_EEXIST	  if driver is already existed
 *
 * @note           None
 */
int tls_spifls_drv_install(void)
{
    int err;
    extern int tls_spifls_probe(void);
    extern int tls_spifls_drv_register(struct tls_fls_drv *fls_drv);

    err = tls_spifls_drv_register((struct tls_fls_drv *) &exspi_fls);
    if (err == TLS_FLS_STATUS_EEXIST)
    {		
    	return err;
    }
    TLS_DBGPRT_INFO("register the spi flash driver - %d.\n", err);

    err = tls_spifls_probe();
    TLS_DBGPRT_INFO("probe spi flash - %d.\n", err);

    return err;
}
