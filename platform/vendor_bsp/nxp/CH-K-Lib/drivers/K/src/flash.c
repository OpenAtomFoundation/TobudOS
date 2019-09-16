/**
  ******************************************************************************
  * @file    flash.c
  * @author  YANDLD
  * @version V3.0.0
  * @date    2015.8.28
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "flash.h"
#include "common.h"

/* flash commands */
#define RD1BLK    0x00  /* read 1 block */
#define RD1SEC    0x01  /* read 1 section */
#define PGMCHK    0x02  /* program check */
#define RDRSRC    0x03  /* read resource */
#define PGM4      0x06  /* program phase program 4 byte */
#define PGM8      0x07  /* program phase program 8 byte */
#define ERSBLK    0x08  /* erase flash block */
#define ERSSCR    0x09  /* erase flash sector */
#define PGMSEC    0x0B  /* program section */
#define RD1ALL    0x40  /* read 1s all block */
#define RDONCE    0x41  /* read once */
#define PGMONCE   0x43  /* program once */
#define ERSALL    0x44  /* erase all blocks */
#define VFYKEY    0x45  /* verift backdoor key */
#define PGMPART   0x80  /* program paritition */
#define SETRAM    0x81  /* set flexram function */
#define NORMAL_LEVEL 0x0


/* disable interrupt before lunch command */
#define CCIF    (1<<7)
#define ACCERR  (1<<5)
#define FPVIOL  (1<<4)
#define MGSTAT0 (1<<0)


#if defined(FTFL)
#define FTF    FTFL
#define SECTOR_SIZE     (2048)
#define PROGRAM_CMD      PGM4
#elif defined(FTFE)
#define FTF    FTFE
#define SECTOR_SIZE     (4096)
#define PROGRAM_CMD      PGM8
#elif defined(FTFA)
    #if (__CORTEX_M == 0)
        #if defined(MKL28Z7)
        #define SECTOR_SIZE     (2048)
        #else
        #define SECTOR_SIZE     (1024)
        #endif
    #else
    #define SECTOR_SIZE     (2048)
    #endif
#define PROGRAM_CMD      PGM4
#define FTF    FTFA
#endif


volatile uint8_t s_flash_command_run[] = {0x00, 0xB5, 0x80, 0x21, 0x01, 0x70, 0x01, 0x78, 0x09, 0x06, 0xFC, 0xD5,0x00, 0xBD};
typedef void (*flash_run_entry_t)(volatile uint8_t *reg);
flash_run_entry_t s_flash_run_entry;
    

    
static inline uint8_t FlashCmdStart(void)
{
    /* clear command result flags */
    FTF->FSTAT = ACCERR | FPVIOL;
    s_flash_run_entry = (flash_run_entry_t)((uint32_t)s_flash_command_run + 1);
    s_flash_run_entry(&FTF->FSTAT);
    
    if(FTF->FSTAT & (ACCERR | FPVIOL | MGSTAT0)) return CH_ERR;
    return CH_OK;
}

 /**
 * @brief  get flash sector size
 * @retval flash sector size
 */
uint32_t FLASH_GetSectorSize(void)
{
    return SECTOR_SIZE;
}

void FLASH_Init(void)
{
    /* clear status */
    FTF->FSTAT = ACCERR | FPVIOL;
}

 /**
 * @brief  flash erase sector
 * @note   this function will erase a flash sector
 * @param  addr: start addr
 * @retval Flash return code
 */
uint8_t FLASH_EraseSector(uint32_t addr)
{
    int ret;
	union
	{
		uint32_t  word;
		uint8_t   byte[4];
	} dest;
	dest.word = (uint32_t)addr;

    /* set cmd */
	FTF->FCCOB0 = ERSSCR; 
	FTF->FCCOB1 = dest.byte[2];
	FTF->FCCOB2 = dest.byte[1];
	FTF->FCCOB3 = dest.byte[0];
    __disable_irq();
    ret = FlashCmdStart();
    __enable_irq();
    
    return ret;
}

 /**
 * @brief  flash write sector
 * @note   len must = sector size
 * @param  addr: start addr
 * @param  buf : buffer pointer
 * @param  len : len
 * @retval Flash return code
 */
uint8_t FLASH_WriteSector(uint32_t addr, const uint8_t *buf, uint32_t len)
{
    uint16_t step, ret, i;
	union
	{
		uint32_t  word;
		uint8_t   byte[4];
	} dest;
	dest.word = (uint32_t)addr;

	FTF->FCCOB0 = PROGRAM_CMD;
    
    switch(PROGRAM_CMD)
    {
        case PGM4:
            step = 4;
            break;
        case PGM8:
            step = 8;
            break;
        default:
            LIB_TRACE("FLASH: no program cmd found!\r\n");
            step = 4;
            break;
    }
    
	for(i=0; i<len; i+=step)
	{
        /* set address */
		FTF->FCCOB1 = dest.byte[2];
		FTF->FCCOB2 = dest.byte[1];
		FTF->FCCOB3 = dest.byte[0];

		FTF->FCCOB4 = buf[3];
		FTF->FCCOB5 = buf[2];
		FTF->FCCOB6 = buf[1];
		FTF->FCCOB7 = buf[0];
        
        if(step == 8)
        {
            FTF->FCCOB8 = buf[7];
            FTF->FCCOB9 = buf[6];
            FTF->FCCOBA = buf[5];
            FTF->FCCOBB = buf[4];
        }

		dest.word += step; buf += step;

        __disable_irq();
        ret = FlashCmdStart();
        __enable_irq();
        
		if(CH_OK != ret) 
        {
            return CH_ERR;
        }
    }
    return CH_OK;
}


uint32_t FLASH_SetcorTest(uint32_t addr)
{
    uint32_t ret, i,j;
    uint8_t *p;
    ALIGN(8) uint8_t buf[32];
    
    LIB_TRACE("program addr:0x%X(%dKB) ...", addr, addr/1024);
    ret = FLASH_EraseSector(addr);
    
    for(i=0; i<sizeof(buf); i++)
    {
        buf[i] = i % 0xFF;
    }
    
    for(i=0; i<(SECTOR_SIZE/sizeof(buf)); i++)
    {
        ret += FLASH_WriteSector(addr + sizeof(buf)*i, buf, sizeof(buf));  
        if(ret)
        {
            LIB_TRACE("issue command failed\r\n");
            return CH_ERR;
        }
    }
    
    LIB_TRACE("varify addr:0x%X ...", addr);
    for(i=0; i<(SECTOR_SIZE/sizeof(buf)); i++)
    {
        p = (uint8_t*)(addr + sizeof(buf)*i);
        for(j=0; j<sizeof(buf); j++)
        {
            if(p[j] != (j%0xFF))
            {
                ret++;
                LIB_TRACE("ERR:[%d]:0x%02X ", i, *p); 
            }
        }
    }
    return ret;
}

 /**
 * @brief  flash self test
 * @note   make sure to have enough stack size
 * @retval FLASH_OK: succ, other flash test fail
 */
uint32_t FLASH_Test(uint32_t addr, uint32_t len)
{
    int i, ret;
    FLASH_Init();
    
    for(i=0; i<(len/SECTOR_SIZE); i++)
    {
        ret = FLASH_SetcorTest(addr + i*SECTOR_SIZE);
        if(ret == CH_OK)
        {
            LIB_TRACE("OK\r\n");
        }
        else
        {
            LIB_TRACE("ERR\r\n");
            return ret;
        }
    }
    return ret;
}
