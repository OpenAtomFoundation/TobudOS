/**
 * @file    wm_internal_flash.h
 *
 * @brief  inter flash driver
 *
 * @author  dave
 *
 * @copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */

#ifndef WM_INTERNAL_FLASH_H
#define WM_INTERNAL_FLASH_H

#include "wm_type_def.h"
#include "wm_osal.h"

typedef volatile unsigned char vu8;
typedef volatile unsigned short vu16;
typedef volatile unsigned long vu32;

#define M8(adr)		(*((vu8 *) (adr)))
#define M16(adr)	(*((vu16*) (adr)))
#define M32(adr)	(*((vu32*) (adr)))

#define INSIDE_FLS_BLOCK_SIZE	(0x10000UL)
#define INSIDE_FLS_SECTOR_SIZE	(0x1000UL)
#define INSIDE_FLS_PAGE_SIZE	256


#define INSIDE_FLS_BASE_ADDR		0x8000000UL
#define INSIDE_FLS_SECBOOT_ADDR 	(INSIDE_FLS_BASE_ADDR  + 0x02000)



enum TYPE_FLASH_ID{
	SPIFLASH_MID_GD = 0xC8,
	SPIFLASH_MID_ESMT = 0x1C,
	SPIFLASH_MID_PUYA = 0x85,	
	SPIFLASH_MID_WINBOND = 0xEF,	
};


/**
 * @typedef struct    Flash Registers
 */
typedef struct
{
    vu32 ACR;                   /**< offset 0x000 */
    vu32 KEYR;                 /**< offset 0x004 */
    vu32 SR;                     /**< offset 0x008 */
    vu32 CR;                     /**< offset 0x00C */
    vu32 AR;                     /**< offset 0x010 */
} FLASH_TypeDef;

#define FLASH_HS				0x00000001

 /** Flash Keys */
#define RDPRT_KEY       0x5AA5
#define FLASH_KEY1      0x57696E6E
#define FLASH_KEY2      0x65724D69
#define FLASH_KEY3			0x63726F21

 /** Flash Control Register definitions */
#define FLASH_PG        0x00000001
#define FLASH_PER       0x00000002
#define FLASH_MER       0x00000004
#define FLASH_STRT			0x00000008
#define FLASH_LOCK   	  0x00000020
#define FLASH_ERRIE     0x00000040
#define FLASH_EOPIE     0x00000080

 /** Flash Status Register definitions */
#define FLASH_BSY       0x00000001
#define FLASH_PGERR     0x00000002
#define FLASH_EOP       0x00000004


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

/**
 * @struct tls_inside_fls
 */
struct tls_inside_fls
{
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
 * @defgroup INTERNAL_FLASH_Driver_APIs INTERNAL_FLASH Driver APIs
 * @brief INTERNAL_FLASH driver APIs
 */

/**
 * @addtogroup INTERNAL_FLASH_Driver_APIs
 * @{
 */


/**
 * @brief          This function is used to unlock flash protect area [0x0~0x2000].
 *
 * @param	       None	 
 *
 * @return         None
 *
 * @note           None
 */
int tls_flash_unlock(void);

/**
 * @brief          This function is used to lock flash protect area [0x0~0x2000].
 *
 * @param	       None	 
 *
 * @return         None
 *
 * @note           None
 */
int tls_flash_lock(void);


/**
 * @brief          This function is used to get the flash semaphore.
 *
 * @param	       None	 
 *
 * @return         None
 *
 * @note           None
 */
void tls_fls_sem_lock(void);

/**
 * @brief          This function is used to release the flash semaphore.
 *
 * @param	       None	 
 *
 * @return         None
 *
 * @note           None
 */
void tls_fls_sem_unlock(void);


/**
 * @brief          This function is used to initial flash module structer.
 *
 * @param[in]      None
 *
 * @retval         TLS_FLS_STATUS_OK	         if init sucsess
 * @retval         TLS_FLS_STATUS_EBUSY          already inited
 * @retval         TLS_FLS_STATUS_ENOMEM         memory error
 *
 * @note           None
 */
int tls_fls_init(void);

/**
 * @brief          This function is used to read data from the flash.
 *
 * @param[in]      addr                 Specifies the starting address to read from.
 * @param[in]      buf                  Specified the address to save the readback data.
 * @param[in]      len                  Specifies the length of the data to read.
 *
 * @retval         TLS_FLS_STATUS_OK	    if read sucsess
 * @retval         TLS_FLS_STATUS_EIO	    if read fail
 *
 * @note           None
 */
int tls_fls_read(u32 addr, u8 * buf, u32 len);


/**
 * @brief          This function is used to write data into the flash.
 *
 * @param[in]      addr     Specifies the starting address to write to
 * @param[in]      buf      Pointer to a byte array that is to be written
 * @param[in]      len      Specifies the length of the data to be written
 *
 * @retval         TLS_FLS_STATUS_OK	        if write flash success
 * @retval         TLS_FLS_STATUS_EPERM	        if flash struct point is null
 * @retval         TLS_FLS_STATUS_ENODRV	    if flash driver is not installed
 * @retval         TLS_FLS_STATUS_EINVAL	    if argument is invalid
 * @retval         TLS_FLS_STATUS_EIO           if io error
 *
 * @note           None
 */
int tls_fls_write(u32 addr, u8 * buf, u32 len);


/**
 * @brief          	This function is used to erase the appointed sector
 *
 * @param[in]      	sector 	sector num of the flash, 4K bytes every sector	
 *
 * @retval         	TLS_FLS_STATUS_OK	    	if read sucsess
 * @retval         	other	    				if read fail
 *
 * @note           	None
 */
int tls_fls_erase(u32 sector);

#endif /* WM_INTERNAL_FLASH_H */
