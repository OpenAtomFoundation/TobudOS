/**
 * @file    wm_fls.c
 *
 * @brief   flash Driver Module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "list.h"
#include "wm_hostspi.h"
#include "wm_flash.h"
#include "wm_dbg.h"
#include "wm_mem.h"
#include "wm_fls_gd25qxx.h"


static struct tls_fls *spi_fls = NULL;

int tls_spifls_read_id(u32 * id)
{
    u32 cmd;
    int err;

    cmd = FLS_CMD_READ_DEV_ID;
    *id = 0;

    err = tls_spi_read_with_cmd((const u8 *) &cmd, 4, (u8 *) id, 3);

    if (err != TLS_SPI_STATUS_OK)
    {
        TLS_DBGPRT_ERR("flash read ID fail(%d)!\n", err);
        return TLS_FLS_STATUS_EIO;
    }

    TLS_DBGPRT_FLASH_INFO("flash ID - 0x%x.\n", *id);

    return TLS_FLS_STATUS_OK;
}


/**
 * @brief          This function is used to read data from the flash.
 *
 * @param[in]      addr                 is byte offset addr for read from the flash.
 * @param[in]      buf                   is user for data buffer of flash read
 * @param[in]      len                   is byte length for read.
 *
 * @retval         TLS_FLS_STATUS_OK	    if read sucsess
 * @retval         TLS_FLS_STATUS_EIO	    if read fail
 *
 * @note           None
 */
int tls_spifls_read(u32 addr, u8 * buf, u32 len)
{
    int err;
    u32 read_bytes;
    struct tls_fls_drv *drv;

    if (spi_fls == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EPERM;
    }

    if (spi_fls->current_drv == NULL)
    {
        TLS_DBGPRT_ERR("the current spi flash driver not installed!\n");
        return TLS_FLS_STATUS_ENODRV;
    }

    if ((addr >= spi_fls->current_drv->total_size) || (len == 0)
        || (buf == NULL))
    {
        return TLS_FLS_STATUS_EINVAL;
    }
    tls_os_sem_acquire(spi_fls->fls_lock, 0);
    drv = spi_fls->current_drv;
    read_bytes =
        ((addr + len) > drv->total_size) ? (drv->total_size - addr) : len;

    err = TLS_FLS_STATUS_OK;

    err = drv->read(addr, buf, read_bytes);
    tls_os_sem_release(spi_fls->fls_lock);
    return err;
}

int tls_spifls_fast_read(u32 addr, u8 * buf, u32 len)
{
    int err;
    u32 read_bytes;
    struct tls_fls_drv *drv;

    if (spi_fls == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EPERM;
    }

    if (spi_fls->current_drv == NULL)
    {
        TLS_DBGPRT_ERR("the current spi flash driver not installed!\n");
        return TLS_FLS_STATUS_ENODRV;
    }

    if ((addr >= spi_fls->current_drv->total_size) || (len == 0)
        || (buf == NULL))
    {
        return TLS_FLS_STATUS_EINVAL;
    }
    if ((spi_fls->current_drv->flags & TLS_FLS_FLAG_FAST_READ) !=
        TLS_FLS_FLAG_FAST_READ)
    {
        return TLS_FLS_STATUS_ENOSUPPORT;
    }

    drv = spi_fls->current_drv;
    read_bytes =
        ((addr + len) > drv->total_size) ? (drv->total_size - addr) : len;

    err = TLS_FLS_STATUS_OK;

    err = drv->fast_read(addr, buf, read_bytes);

    return err;
}

int tls_spifls_page_write(u32 page, u8 * buf, u32 page_cnt)
{
    int err;
    u32 write_pages;
    u32 i;
    struct tls_fls_drv *drv;

    if (spi_fls == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EPERM;
    }

    if (spi_fls->current_drv == NULL)
    {
        TLS_DBGPRT_ERR("the current spi flash driver not installed!\n");
        return TLS_FLS_STATUS_ENODRV;
    }

    if ((page >= (spi_fls->current_drv->total_size / spi_fls->current_drv->page_size))
        || (page_cnt == 0) || (buf == NULL))
    {
        return TLS_FLS_STATUS_EINVAL;
    }

    drv = spi_fls->current_drv;
    write_pages = ((page + page_cnt) > (drv->total_size / drv->page_size)) ?
                    ((drv->total_size / drv->page_size) -page) : page_cnt;

    err = TLS_FLS_STATUS_OK;

    for (i = 0; i < write_pages; i++)
    {
        err = drv->page_write(page + i, buf + i * drv->page_size);
        if (err != TLS_FLS_STATUS_OK)
        {
            TLS_DBGPRT_ERR("flash page write fail(page %d)!\n", (page + i));
            break;
        }
    }

    return err;
}

/**
 * @brief          This function is used to write data to the flash.
 *
 * @param[in]      addr     is byte offset addr for write to the flash
 * @param[in]      buf       is the data buffer want to write to flash
 * @param[in]      len       is the byte length want to write
 *
 * @retval         TLS_FLS_STATUS_OK	           if write flash success
 * @retval         TLS_FLS_STATUS_EPERM	    if flash struct point is null
 * @retval         TLS_FLS_STATUS_ENODRV	    if flash driver is not installed
 * @retval         TLS_FLS_STATUS_EINVAL	    if argument is invalid
 * @retval         TLS_FLS_STATUS_EIO           if io error
 *
 * @note           None
 */
int tls_spifls_write(u32 addr, u8 * buf, u32 len)
{
    u8 *cache;
    int err;
    u32 sector_addr;
    u32 sector_num;
    u32 write_bytes;
    u32 i;
    struct tls_fls_drv *drv;

    if (spi_fls == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EPERM;
    }

    if (spi_fls->current_drv == NULL)
    {
        TLS_DBGPRT_ERR("the current spi flash driver not installed!\n");
        return TLS_FLS_STATUS_ENODRV;
    }

    if ((addr >= spi_fls->current_drv->total_size) || (len == 0)
        || (buf == NULL))
    {
        return TLS_FLS_STATUS_EINVAL;
    }
    tls_os_sem_acquire(spi_fls->fls_lock, 0);
    drv = spi_fls->current_drv;
    write_bytes =
        ((addr + len) > drv->total_size) ? (drv->total_size - addr) : len;
    sector_addr = addr / drv->sector_size;
    sector_num = (addr + write_bytes - 1) / drv->sector_size - sector_addr + 1;

    TLS_DBGPRT_FLASH_INFO
        ("write to flash: sector address - %d, sectors - %d.\n", sector_addr,
         sector_num);

    err = TLS_FLS_STATUS_OK;

    cache = tls_mem_alloc(drv->sector_size);
    if (cache == NULL)
    {
        tls_os_sem_release(spi_fls->fls_lock);
        TLS_DBGPRT_ERR("allocate sector cache memory(%dB) fail!\n",
                       drv->sector_size);
        return TLS_FLS_STATUS_ENOMEM;
    }

    for (i = 0; i < sector_num; i++)
    {
        TLS_DBGPRT_FLASH_INFO("firstly, read the sector - %d to cache.\n",
                              sector_addr + i);
        err = drv->read((sector_addr + i) * drv->sector_size, cache, drv->sector_size);
        if (err != TLS_FLS_STATUS_OK)
        {
            tls_os_sem_release(spi_fls->fls_lock);
            TLS_DBGPRT_ERR("flash read fail(sector %d)!\n", (sector_addr + i));
            break;
        }

		if (1 == sector_num){/*flash write only in one sector*/
			MEMCPY(cache + (addr%drv->sector_size), buf, write_bytes);	
			buf += write_bytes;
			write_bytes = 0;			
		}else{/*flash write through some sectors*/
			if (0 == i) {
				MEMCPY(cache+(addr%drv->sector_size), buf, drv->sector_size - (addr%drv->sector_size));
				buf += drv->sector_size - (addr%drv->sector_size);
				write_bytes -= drv->sector_size - (addr%drv->sector_size);
			} else if (i == (sector_num - 1)) {
				MEMCPY(cache, buf, write_bytes);
				buf += write_bytes;
				write_bytes = 0;
			} else {
				MEMCPY(cache, buf, drv->sector_size);
				buf += drv->sector_size;
				write_bytes -= drv->sector_size;
			}
		}

        TLS_DBGPRT_FLASH_INFO("second, erase the sector - %d.\n",
                              sector_addr + i);
        err = drv->erase(sector_addr + i);
        if (err != TLS_FLS_STATUS_OK)
        {
            tls_os_sem_release(spi_fls->fls_lock);
            TLS_DBGPRT_ERR("flash erase fail(sector %d)!\n", (sector_addr + i));
            break;
        }

        TLS_DBGPRT_FLASH_INFO
            ("finnaly, write the data in cache to the sector - %d.\n",
             sector_addr + i);
        err = tls_spifls_page_write((sector_addr +i) * (drv->sector_size / drv->page_size),
                            cache, drv->sector_size / drv->page_size);
        if (err != TLS_FLS_STATUS_OK)
        {
            tls_os_sem_release(spi_fls->fls_lock);
            TLS_DBGPRT_ERR("flash write fail(sector %d)!\n", (sector_addr + i));
            break;
        }
    }

    tls_mem_free(cache);
    tls_os_sem_release(spi_fls->fls_lock);
    return err;
}


int tls_spifls_erase(u32 sector)
{
    int err;
    struct tls_fls_drv *drv;

    if (spi_fls == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EPERM;
    }

    if (spi_fls->current_drv == NULL)
    {
        TLS_DBGPRT_ERR("the current spi flash driver not installed!\n");
        return TLS_FLS_STATUS_ENODRV;
    }

    if (sector >= (spi_fls->current_drv->total_size / spi_fls->current_drv->sector_size))
    {
        TLS_DBGPRT_ERR("the sector to be erase overflow!\n");
        return TLS_FLS_STATUS_EINVAL;
    }
    tls_os_sem_acquire(spi_fls->fls_lock, 0);
    drv = spi_fls->current_drv;

    err = TLS_FLS_STATUS_OK;

    err = drv->erase(sector);
    tls_os_sem_release(spi_fls->fls_lock);
    return err;
}

int tls_spifls_chip_erase(void)
{
    int err;
    struct tls_fls_drv *drv;

    if (spi_fls == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EPERM;
    }

    if (spi_fls->current_drv == NULL)
    {
        TLS_DBGPRT_ERR("the current spi flash driver not installed!\n");
        return TLS_FLS_STATUS_ENODRV;
    }

    drv = spi_fls->current_drv;

    err = TLS_FLS_STATUS_OK;

    err = drv->chip_erase();

    return err;
}

int tls_spifls_get_param(u8 type, void *param)
{
    int err;
    struct tls_fls_drv *drv;

    if (spi_fls == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EPERM;
    }

    if (spi_fls->current_drv == NULL)
    {
        TLS_DBGPRT_ERR("the current spi flash driver not installed!\n");
        return TLS_FLS_STATUS_ENODRV;
    }

    if (param == NULL)
    {
        return TLS_FLS_STATUS_EINVAL;
    }
    tls_os_sem_acquire(spi_fls->fls_lock, 0);
    drv = spi_fls->current_drv;
    err = TLS_FLS_STATUS_OK;
    switch (type)
    {
        case TLS_FLS_PARAM_TYPE_ID:
            *((u32 *) param) = drv->id;
            break;

        case TLS_FLS_PARAM_TYPE_SIZE:
            *((u32 *) param) = drv->total_size;
            break;

        case TLS_FLS_PARAM_TYPE_PAGE_SIZE:
            *((u32 *) param) = drv->page_size;
            break;

        case TLS_FLS_PARAM_TYPE_PROG_SIZE:
            *((u32 *) param) = drv->program_size;
            break;

        case TLS_FLS_PARAM_TYPE_SECTOR_SIZE:
            *((u32 *) param) = drv->sector_size;
            break;

        default:
            TLS_DBGPRT_WARNING("invalid parameter ID!\n");
            err = TLS_FLS_STATUS_EINVAL;
            break;
    }
    tls_os_sem_release(spi_fls->fls_lock);
    return err;
}

int tls_spifls_drv_register(struct tls_fls_drv *fls_drv)
{
    u32 cpu_sr;
    struct tls_fls_drv *drv;

    if (fls_drv == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EINVAL;
    }

    dl_list_for_each(drv, &spi_fls->fls_drvs, struct tls_fls_drv, drv_list)
    {
        if (drv->id == fls_drv->id)
        {
            TLS_DBGPRT_WARNING
                ("corresponding spi flash driver has registered!\n");
            return TLS_FLS_STATUS_EEXIST;
        }
    }

    cpu_sr = tls_os_set_critical();
    dl_list_add_tail((struct dl_list *) &spi_fls->fls_drvs,
                     (struct dl_list *) &fls_drv->drv_list);
    tls_os_release_critical(cpu_sr);

    TLS_DBGPRT_FLASH_INFO("the spi flash driver is registered successfully!\n");

    return TLS_FLS_STATUS_OK;
}

int tls_spifls_drv_unregister(struct tls_fls_drv *fls_drv)
{
    TLS_DBGPRT_WARNING
        ("unregister spi flash driver operation is not supported!\n");
    return TLS_FLS_STATUS_EPERM;
}

int tls_spifls_probe(void)
{
    int err;
    u32 id;
    struct tls_fls_drv *drv;

    if (spi_fls == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EPERM;
    }
    if (spi_fls->current_drv != NULL)
    {
        TLS_DBGPRT_ERR("the current spi flash has fount the matched driver!\n");
        return TLS_FLS_STATUS_EBUSY;
    }

    TLS_DBGPRT_FLASH_INFO("try to read the current spi flash ID.\n");
    err = tls_spifls_read_id(&id);
    if (err != TLS_FLS_STATUS_OK)
    {
        return err;
    }

    TLS_DBGPRT_FLASH_INFO("current spi flash ID - 0x%x.\n", id);

    dl_list_for_each(drv, &spi_fls->fls_drvs, struct tls_fls_drv, drv_list)
    {
        err = drv->probe(id);
        if (err != TLS_FLS_STATUS_OK)
        {
            return err;
        }

        tls_spi_setup(drv->mode, drv->cs_active, drv->clock);

        TLS_DBGPRT_FLASH_INFO("matched the spi flash driver.\n");
        spi_fls->current_drv = drv;
        break;
    }

    if (spi_fls->current_drv == NULL)
    {
        TLS_DBGPRT_WARNING("not found the matched spi flash driver!\n");
        return TLS_FLS_STATUS_ENODRV;
    }

    return TLS_FLS_STATUS_OK;
}

int tls_spifls_init(void)
{
    struct tls_fls *fls;
    int err;

    if (spi_fls != NULL)
    {
        TLS_DBGPRT_ERR("flash driver module has been installed!\n");
        return TLS_FLS_STATUS_EBUSY;
    }

    fls = (struct tls_fls *) tls_mem_alloc(sizeof(struct tls_fls));
    if (fls == NULL)
    {
        TLS_DBGPRT_ERR("allocate @spi_fls fail!\n");
        return TLS_FLS_STATUS_ENOMEM;
    }

    memset(fls, 0, sizeof(*fls));
    dl_list_init((struct dl_list *) &fls->fls_drvs);
    err = tls_os_sem_create(&fls->fls_lock, 1);
    if (err != TLS_OS_SUCCESS)
    {
        tls_mem_free(fls);
        TLS_DBGPRT_ERR("create semaphore @fls_lock fail!\n");
        return TLS_FLS_STATUS_ENOMEM;
    }
    spi_fls = fls;

    tls_spifls_drv_install();

    return TLS_FLS_STATUS_OK;
}

int tls_spifls_exit(void)
{
    TLS_DBGPRT_FLASH_INFO("Not support flash driver module uninstalled!\n");
    return TLS_FLS_STATUS_EPERM;
}

