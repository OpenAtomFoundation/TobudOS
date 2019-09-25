/**
 * @file    wm_flash.h
 *
 * @brief   flash Driver module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_FLASH_H
#define WM_FLASH_H

#include "wm_type_def.h"
#include "wm_osal.h"

#define TLS_FLS_STATUS_OK      (0)
#define TLS_FLS_STATUS_EINVAL      (1)
#define TLS_FLS_STATUS_EBUSY      (2)
#define TLS_FLS_STATUS_EPERM      (3)
#define TLS_FLS_STATUS_ENOSUPPORT      (4)
#define TLS_FLS_STATUS_EEXIST      (5)
#define TLS_FLS_STATUS_ENOMEM      (6)
#define TLS_FLS_STATUS_EOVERFLOW      (7)
#define TLS_FLS_STATUS_ENODEV      (8)
#define TLS_FLS_STATUS_EDEV      (9)
#define TLS_FLS_STATUS_EIO      (10)
#define TLS_FLS_STATUS_ENODRV      (11)

#define TLS_FLS_PARAM_TYPE_ID      (0)
#define TLS_FLS_PARAM_TYPE_SIZE      (1)
#define TLS_FLS_PARAM_TYPE_PAGE_SIZE      (2)
#define TLS_FLS_PARAM_TYPE_PROG_SIZE      (3)
#define TLS_FLS_PARAM_TYPE_SECTOR_SIZE      (4)

#define TLS_FLS_FLAG_UNDER_PROTECT      (1<<0)
#define TLS_FLS_FLAG_FAST_READ      (1<<1)
#define TLS_FLS_FLAG_AAAI      (1<<2)

#define FLS_CMD_READ_DEV_ID     (0x9F)  // read device id //(0x9f)

/**
 * @struct fls_list     list
 */
struct fls_list
{
    struct fls_list *next;
    struct fls_list *prev;
};


/**
 * @struct tls_fls_drv     flash driver
 */
struct tls_fls_drv
{
    struct fls_list drv_list;
    u32 id;
    u32 total_size;
    u32 page_size;
    u32 program_size;
    u32 sector_size;
    u32 clock;
    u8 mode;
    u8 cs_active;
    u8 flags;
    int (*read) (u32, u8 *, u32);
    int (*fast_read) (u32, u8 *, u32);
    int (*page_write) (u32, u8 *);
    int (*erase) (u32);
    int (*chip_erase) (void);
    int (*probe)(u32 id);
    void (*remove) (void);
};

/**
 * @struct tls_fls     flash
 */
struct tls_fls
{
    struct fls_list fls_drvs;
    struct tls_fls_drv *current_drv;
    tls_os_sem_t *fls_lock;
};

/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup SPIFLASH_Driver_APIs SPIFLASH Driver APIs
 * @brief SPIFLASH driver APIs
 */

/**
 * @addtogroup SPIFLASH_Driver_APIs
 * @{
 */

/**
 * @brief          This function is used to initial flash module structer.
 *
 * @param[in]      None
 *
 * @retval         TLS_FLS_STATUS_OK	                if init sucsess
 * @retval         TLS_FLS_STATUS_EBUSY            already inited
 * @retval         TLS_FLS_STATUS_ENOMEM         memory error
 *
 * @note           None
 */
int tls_spifls_init(void);


/**
 * @brief          This function is used to read data from the flash.
 *
 * @param[in]      addr                  Specifies the starting address to read from
 * @param[in]      buf                   Pointer to a byte array that is to be written.
 * @param[in]      len                   length to read.
 *
 * @retval         TLS_FLS_STATUS_OK	    if read sucsess
 * @retval         TLS_FLS_STATUS_EIO	    if read fail
 *
 * @note           None
 */
int tls_spifls_read(u32 addr, u8 * buf, u32 len);


/**
 * @brief          This function is used to write data into the flash.
 *
 * @param[in]      addr      Specifies the starting address to write to.
 * @param[in]      buf       Pointer to a byte array that holds the data to be written.
 * @param[in]      len       length to write.
 *
 * @retval         TLS_FLS_STATUS_OK	           if write flash success
 * @retval         TLS_FLS_STATUS_EPERM	    if flash struct point is null
 * @retval         TLS_FLS_STATUS_ENODRV	    if flash driver is not installed
 * @retval         TLS_FLS_STATUS_EINVAL	    if argument is invalid
 * @retval         TLS_FLS_STATUS_EIO           if io error
 *
 * @note           None
 */
int tls_spifls_write(u32 addr, u8 * buf, u32 len);

/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_FLASH_H */
