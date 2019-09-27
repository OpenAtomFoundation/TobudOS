/**
 * @file    wm_flash_map.h
 *
 * @brief   flash zone map
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef __WM_FLASH_MAP_H__
#define __WM_FLASH_MAP_H__

/**FLASH MAP**/

/**Flash Base Address */
#define FLASH_BASE_ADDR						(0x08000000UL)

#define FLASH_1M_END_ADDR                   (0x08100000UL)

/**Run-time image header area*/
#define CODE_RUN_HEADER_ADDR				(0x08010000UL)
#define CODE_RUN_HEADER_AREA_LEN			(0x100)

/**Run-time image area*/
#define CODE_RUN_START_ADDR					(0x08010100UL)

/********************************************************
******************LAYOUT For 1M Flash**********************
    *Reserved     	0x8000000-0x8010000	    64Kbyte
    *Code   		0x8010100-0x808FFFF	    512Kbyte - 256 byte
    *Update		0x8090000-0x80EFFFF	    384Kbyte
    *User			0x80F0000-0x80FBFFF	    48Kbyte    
    *Parameter	0x80FC000-0x80FFFFF          16Kbyte
*********************************************************

**************LAYOUT For 2M Flash as 1M LAYOUT**************
    *Reserved     		0x8000000-0x8010000	    64Kbyte
    *Code   			0x8010100-0x80FFFFF	    896Kbyte -256byte
    *Old  User Area	0x80F0000-0x80FBFFF          48Kbyte
    *Parameter		0x80FC000-0x80FFFFF          16Kbyte
    *Update			0x8100000-0x81AFFFF	    704Kbyte
    *EXT User			0x81B0000-0x81FFFFF	    320Kbyte    
*********************************************************

******************LAYOUT For 2M Flash*********************
    *Reserved     	0x8000000-0x8010000	    64Kbyte
    *Code   		0x8010100-0x80FFFFF	    960Kbyte -256byte
    *Update		0x8100000-0x81BFFFF	    768Kbyte
    *User			0x81C0000-0x81FBFFF	    240Kbyte    
    *Parameter	0x81FC000-0x81FFFFF          16Kbyte
*********************************************************
*********************************************************/

/**Run-time image area size*/
extern unsigned int CODE_RUN_AREA_LEN;

/**Area can be used by User in 1M position*/
extern unsigned int USER_ADDR_START;
extern unsigned int TLS_FLASH_PARAM_DEFAULT;
extern unsigned int USER_AREA_LEN;
extern unsigned int USER_ADDR_END;


/**Upgrade image header area & System parameter area */
extern unsigned int CODE_UPD_HEADER_ADDR;
extern unsigned int TLS_FLASH_PARAM1_ADDR;
extern unsigned int TLS_FLASH_PARAM2_ADDR;
extern unsigned int TLS_FLASH_PARAM_RESTORE_ADDR;

/**Upgrade image area*/
extern unsigned int CODE_UPD_START_ADDR;
extern unsigned int CODE_UPD_AREA_LEN;

/**Area can be used by User in 2M position*/
extern unsigned int EX_USER_ADDR_START;
extern unsigned int EX_USER_AREA_LEN;
extern unsigned int EX_USER_ADDR_END;

extern unsigned int TLS_FLASH_END_ADDR;

#define SIGNATURE_WORD      				0xa0ffff9f

#endif /*__WM_CONFIG_H__*/

