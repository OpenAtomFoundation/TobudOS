/**
 * @file    wm_internal_fls.c
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
#include "wm_dbg.h"
#include "wm_mem.h"
#include "list.h"
#include "wm_flash_map.h"
#include "wm_internal_flash.h"
#include "wm_flash.h"

static struct tls_inside_fls *inside_fls = NULL;
static u32 inner1flashsize = 0; /*first inner flash size*/
static u32 inner2flashsize = 0; /*second inner flash size*/

u32 flashtotalsize = 0;

unsigned char com_mem[4096];


static void writeEnable(void)
{
	M32(0x40002000) = 0x6;
	M32(0x40002004) = 0x10000000;
}

unsigned char readRID(void)
{
	M32(0x40002000) = 0x2c09F;
	M32(0x40002004) = 0x10000000;
	return M32(0x40002200)&0xFF;
}

void writeGDBpBit(char cmp, char bp4, char bp3, char bp2, char bp1, char bp0)
{
	int status = 0;
	int bpstatus = 0;

	M32(0x40002000) = 0x0C005;
	M32(0x40002004) = 0x10000000;
	status =  M32(0x40002200)&0xFF;

	M32(0x40002000) = 0x0C035;
	M32(0x40002004) = 0x10000000;
	status  |=  (M32(0x40002200)&0xFF)<<8;

	/*Write Enable*/
	M32(0x40002000) = 0x6;
	M32(0x40002004) = 0x10000000;

	bpstatus  = (bp4<<6)|(bp3<<5)|(bp2<<4)|(bp1<<3)|(bp0<<2);
	status      = (status&0xBF83)|bpstatus|(cmp<<14);

	M32(0x40002200)  = status;
	M32(0x40002000) = 0x1A001;
	M32(0x40002004) = 0x10000000;
}

void writeESMTBpBit(char cmp, char bp4, char bp3, char bp2, char bp1, char bp0)
{
	int status = 0;
	int bpstatus = 0;

	M32(0x40002000) = 0x0C005;
	M32(0x40002004) = 0x10000000;
	status =  M32(0x40002200)&0xFF;
	bpstatus  = (bp4<<6)|(bp3<<5)|(bp2<<4)|(bp1<<3)|(bp0<<2);
	status      = (status&0x83)|bpstatus;

	/*Write Enable*/
	M32(0x40002000) = 0x6;
	M32(0x40002004) = 0x10000000;

	bpstatus  = (bp4<<6)|(bp3<<5)|(bp2<<4)|(bp1<<3)|(bp0<<2);
	status      = (status&0x83)|bpstatus|(cmp<<14);

	M32(0x40002200)  = status;
	M32(0x40002000) = 0x0A001;
	M32(0x40002004) = 0x10000000;


	M32(0x40002000) = 0x0C085;
	M32(0x40002004) = 0x10000000;
	status  =  M32(0x40002200)&0xFF;

	/*Write Enable*/
	M32(0x40002000) = 0x6;
	M32(0x40002004) = 0x10000000;

	status		= (status&0xBF)|(cmp<<6);
	M32(0x40002200)  = status;
	M32(0x40002000) = 0x0A0C1;
	M32(0x40002004) = 0x10000000;
}

int flashunlock(void)
{
	switch(readRID())
	{
		case SPIFLASH_MID_GD:
		case SPIFLASH_MID_PUYA:
			writeGDBpBit(0,0,0,0,0,0);
		break;
		case SPIFLASH_MID_ESMT:
			writeESMTBpBit(0,0,0,0,0,0);
		break;
		default:
			return -1;	
	}
	return 0;
}

int flashlock(void)
{
	switch(readRID())
	{
		case SPIFLASH_MID_GD:
		case SPIFLASH_MID_PUYA:
			writeGDBpBit(0,1,1,0,1,0);
		break;
		case SPIFLASH_MID_ESMT:
			writeESMTBpBit(0,1,1,0,1,0);
		break;
		default:
			return -1;/*do not clear QIO Mode*/
	}
	return 0;
}

static int programSR(unsigned int  cmd, unsigned long addr, unsigned char *buf,  unsigned int sz)
{
	unsigned long base_addr = 0;
	unsigned int size = 0;


	if (sz > INSIDE_FLS_PAGE_SIZE)
	{
		sz = INSIDE_FLS_PAGE_SIZE;
	}

	base_addr = 0x40002200;
	size = sz;
	while(size)
	{
		M32(base_addr) = *((unsigned long *)buf);
		base_addr += 4;
		buf += 4;
		size -= 4;				
	}

	writeEnable();
	M32(0x40002000) = cmd | ((sz-1) << 16);
	M32(0x40002004) = 0x10000000 | ((addr&0xFFFFF) << 8);

	return 0;
}


static int programPage (unsigned long adr, unsigned long sz, unsigned char *buf) 
{
	programSR(0x80009002, adr, buf, sz);
	return(0);
}

static int eraseSR(unsigned int cmd, unsigned long addr)
{
	/*Write Enable*/
	writeEnable();	
	M32(0x40002000) = cmd;
	M32(0x40002004) = 0x10000000|((addr&0xFFFFF)<<8);

	return 0;
}

static int eraseSector (unsigned long adr) 
{
	eraseSR(0x80000820, adr);
  
	return (0);                                  				// Finished without Errors
}

static unsigned int getFlashDensity(void)
{
	unsigned char density = 0;

	M32(0x40002000) = 0x2c09F;
	M32(0x40002004) = 0x10000000;
	
	density = ((M32(0x40002200)&0xFFFFFF)>>16)&0xFF;
	if ((density == 0x14) ||(density == 0x13))
	{
		return (1<<density);
	}

	return 0;
}
/*sr start*/
/************************************************************
addr:
For GD,		      address:0x000,0x100
For ESMT,		address:0xFF000,0xFF100
For WINBOND,	address:0x1000
*************************************************************/
int readSR(unsigned int cmd, unsigned long addr, unsigned char *buf, unsigned long sz)
{
	int i = 0;
	int word = sz/4;
	int byte = sz%4;
	unsigned long addr_read;

	M32(0x40002000) = cmd|((sz-1)<<16);
	M32(0x40002004) = 0x10000000|((addr&0xFFFFF)<<8);
	
	addr_read = 0x40002200;
	for(i = 0;i < word; i ++)
	{
		M32(buf) = M32(addr_read);
		buf += 4;
		addr_read += 4;
	}
	
	if(byte > 0)
	{
		M32(buf) = M32(addr_read);
		buf += 3;							//point last byte
		while(byte)
		{
			*buf = 0;
			buf --;
			byte --;
		}
	}	

	return 0;
}

void flashSRRW(unsigned long offset,unsigned char *buf,unsigned long sz, unsigned char *backbuf, unsigned int backlen, unsigned int rd)
{
#define SR_TOTAL_SZ (512)
#define SR_PROGRAM_SZE (256)
 	unsigned int i;    
	unsigned int j;
	unsigned int baseaddr = 0; 
	unsigned int sectoroffset = 0;
	unsigned int sectorsize = 0;
	unsigned int sectornum = 0;
	unsigned int remainsz;
	unsigned int erasecmd = 0;
	unsigned int readcmd  = 0;
	unsigned int writecmd = 0;

	unsigned char flashid = 0;
	if (!buf ||((rd == 0)&&( !backbuf || (backlen < 512))))
	{
		return;
	}

	flashid = readRID();
	switch(flashid)
	{
		case SPIFLASH_MID_GD:
			baseaddr = 0x0;
			sectoroffset = 256;
			sectorsize = 256;
			sectornum = 2;
			erasecmd	 = 0x80000844;
			readcmd 	 = 0xBC00C048;
			writecmd	 = 0x80009042;
		break;

		case SPIFLASH_MID_ESMT:
		{
			baseaddr = 0xFF000;
			sectoroffset = 0;
			sectorsize = 512;
			sectornum = 1;
			erasecmd	 = 0x80000820;
			readcmd 	 = 0xBC00C00B;
			writecmd	 = 0x80009002;

			M32(0x40002000) = 0x3A;             /*enter OTP*/
			M32(0x40002004) = 0x10000000;
		}
		break;

		case SPIFLASH_MID_PUYA:
			baseaddr = 0x1000;
			sectoroffset = 0;
			sectorsize = 512;
			sectornum = 1;
			erasecmd	 = 0x80000844;
			readcmd 	 = 0xBC00C048;
			writecmd	 = 0x80009042;

		break;

		default:
		{
		}
		break;
	}

	for (i =0 ; i < sectornum; i++)
	{
		readSR(readcmd, baseaddr + sectoroffset*i, backbuf+i*sectorsize, sectorsize);
	}

	if (rd)
	{
		for(i=0;i<sz;i++)				//Copy
		{
			buf[i] = backbuf[i+offset];	  
		}
	}
	else
	{
		for (i = 0; i < sectornum ; i++)
		{
			eraseSR(erasecmd, baseaddr + sectoroffset*i);
		}

		remainsz = (sz < (SR_TOTAL_SZ - offset))? sz : (SR_TOTAL_SZ - offset);
		for(i= 0; i< remainsz; i++)
		{
			backbuf[i+offset]= buf[i];	  
		}

		for ( i = 0; i < sectornum; i++)
		{
			for (j = 0; j < (sectorsize/SR_PROGRAM_SZE); j++)
			{
				programSR(writecmd, baseaddr + sectoroffset*i + j*SR_PROGRAM_SZE,  backbuf, SR_PROGRAM_SZE);
				backbuf += SR_PROGRAM_SZE;
			}
		}
	}



	if (SPIFLASH_MID_ESMT == flashid)
	{
		/*Write Disable*/
		M32(0x40002000) = 0x4;
		M32(0x40002004) = 0x10000000;
	}
}

/*sr end*/
int readByCMD(unsigned long addr, unsigned char *buf, unsigned long sz, unsigned int mode)
{
	int i = 0;
	int word = sz/4;
	int byte = sz%4;
	unsigned long addr_read;

	if (mode&0x1) /*QIO*/
	{
		addr_read = 0xEC00C0EB;
	}
	else	/*SPI*/
	{
		addr_read = 0xBC00C00B;
	}
	M32(0x40002000) = addr_read|(((sz-1)&0x3FF)<<16);
	M32(0x40002004) = 0x10000000|((addr&0xFFFFF)<<8);

	addr_read = 0x40002200;
	for(i = 0;i < word; i ++)
	{
		M32(buf) = M32(addr_read);	
		buf += 4;
		addr_read += 4;
	}

	if(byte > 0)
	{
		M32(buf) = M32(addr_read);	
		buf += 3;							//point last byte
		byte = 4 - byte;
		while(byte)
		{
			*buf = 0;
			buf --;
			byte --;
		}
	}

	return 0;
}

int flashRead(unsigned long addr, unsigned char *buf, unsigned long sz)
{
#define FLASH_READ_BUF_SIZE   (256)

	unsigned int flash_addr;
	unsigned int sz_1k = 0;
	unsigned int sz_remain = 0;
	int i = 0;
	int page_offset = addr&(FLASH_READ_BUF_SIZE - 1);
	
	char *cache = NULL;

	cache = tls_mem_alloc(INSIDE_FLS_SECTOR_SIZE);
	if (cache == NULL)
    {
        TLS_DBGPRT_ERR("allocate sector cache memory fail!\n");
        return TLS_FLS_STATUS_ENOMEM;
    }
	flash_addr = addr&~(FLASH_READ_BUF_SIZE - 1);
	readByCMD(flash_addr, (unsigned char*)cache, FLASH_READ_BUF_SIZE, 0);
	if (sz > FLASH_READ_BUF_SIZE - page_offset){
		MEMCPY(buf, cache+page_offset, FLASH_READ_BUF_SIZE - page_offset);
		buf += FLASH_READ_BUF_SIZE - page_offset;
		flash_addr 	+= FLASH_READ_BUF_SIZE;
		
		sz_1k = (sz -(FLASH_READ_BUF_SIZE - page_offset)) /FLASH_READ_BUF_SIZE;
		sz_remain = (sz -(FLASH_READ_BUF_SIZE - page_offset))%FLASH_READ_BUF_SIZE;
		for (i = 0; i < sz_1k; i++)
		{
			
			readByCMD(flash_addr, (unsigned char*)cache, FLASH_READ_BUF_SIZE, 0);
			MEMCPY(buf, cache, FLASH_READ_BUF_SIZE);
			buf 			+= FLASH_READ_BUF_SIZE;
			flash_addr 	+= FLASH_READ_BUF_SIZE;
		}
		
		if (sz_remain)
		{
			readByCMD(flash_addr, (unsigned char*)cache, sz_remain, 0);
			MEMCPY(buf, cache, sz_remain);
		}
	}else{
		MEMCPY(buf, cache+page_offset, sz);
	}
	tls_mem_free(cache);

	return 0;
}

/**
 * @brief           This function is used to unlock flash protect area [0x0~0x2000].
 *
 * @param	   None	 
 *
 * @return         None
 *
 * @note           None
 */
int tls_flash_unlock(void)
{
	return flashunlock();
}

/**
 * @brief           This function is used to lock flash protect area [0x0~0x2000].
 *
 * @param	   None	 
 *
 * @return         None
 *
 * @note           None
 */
int tls_flash_lock(void)
{
	return flashlock();
}


/**
 * @brief           This function is used to semaphore protect.
 *
 * @param	   None	 
 *
 * @return         None
 *
 * @note           None
 */
void tls_fls_sem_lock(void)
{
    if (inside_fls == NULL)
    {
        return;
    }
	tls_os_sem_acquire(inside_fls->fls_lock, 0);
}


/**
 * @brief           This function is used to semaphore protect cancel.
 *
 * @param	   None	 
 *
 * @return         None
 *
 * @note           None
 */
void tls_fls_sem_unlock(void)
{
    if (inside_fls == NULL)
    {
        return;
    }	
	tls_os_sem_release(inside_fls->fls_lock);
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
int tls_fls_read(u32 addr, u8 * buf, u32 len)
{
	int err = TLS_FLS_STATUS_EINVAL;
	u32 addrfor1M = 0;
	u32 lenfor1M = 0;
	u32 addrfor2M = 0;
	u32 lenfor2M = 0;

	addrfor1M = addr < FLASH_1M_END_ADDR ? addr:0xFFFFFFFF;
	if (addrfor1M != 0xFFFFFFFF)
	{
	    lenfor1M  = (addr + len ) <= FLASH_1M_END_ADDR ? len : (FLASH_1M_END_ADDR - addr);
	    if (inside_fls == NULL)
	    {
	        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
	        return TLS_FLS_STATUS_EPERM;
	    }

	    if (((addrfor1M&(INSIDE_FLS_BASE_ADDR-1)) >=  inner1flashsize) || (lenfor1M == 0) || (buf == NULL))
	    {
	        return TLS_FLS_STATUS_EINVAL;
	    }
		
	    tls_os_sem_acquire(inside_fls->fls_lock, 0);

		flashRead(addrfor1M, buf, lenfor1M);

	    err = TLS_FLS_STATUS_OK;
	    tls_os_sem_release(inside_fls->fls_lock);	
	}

	if (inner2flashsize)
	{
		addrfor2M = addr >= FLASH_1M_END_ADDR ? addr : ((addr + len ) >= FLASH_1M_END_ADDR ? FLASH_1M_END_ADDR : 0xFFFFFFFF);
		if (addrfor2M != 0xFFFFFFFF)
		{
			lenfor2M  = len - lenfor1M;
			lenfor2M = (addrfor2M + lenfor2M) <= (FLASH_BASE_ADDR | flashtotalsize) ? lenfor2M : ((FLASH_BASE_ADDR | flashtotalsize) - addrfor2M);
		    return tls_spifls_read(addrfor2M&0xFFFFF, buf+lenfor1M, lenfor2M);
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
int tls_fls_write(u32 addr, u8 * buf, u32 len)
{
    u8 *cache;
	unsigned int secpos;	   					
	unsigned int secoff;	   					
	unsigned int secremain;					
 	unsigned int i;    
	unsigned int offaddr; 

	u32 addrfor1M = 0;
	u32 lenfor1M = 0;
	u32 addrfor2M = 0;
	u32 lenfor2M = 0;

	addrfor1M = addr < FLASH_1M_END_ADDR ? addr:0xFFFFFFFF;
	if (addrfor1M != 0xFFFFFFFF)
	{
	    lenfor1M  = (addr + len ) <= FLASH_1M_END_ADDR ? len : (FLASH_1M_END_ADDR - addr);

	    if (inside_fls == NULL)
	    {
	        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
	        return TLS_FLS_STATUS_EPERM;
	    }

	    if (((addrfor1M&(INSIDE_FLS_BASE_ADDR-1)) >=  inner1flashsize) || (lenfor1M == 0) || (buf == NULL))
	    {
	        return TLS_FLS_STATUS_EINVAL;
	    }
		
	    tls_os_sem_acquire(inside_fls->fls_lock, 0);

	    cache = tls_mem_alloc(INSIDE_FLS_SECTOR_SIZE);
	    if (cache == NULL)
	    {
	        tls_os_sem_release(inside_fls->fls_lock);
	        TLS_DBGPRT_ERR("allocate sector cache memory fail!\n");
	        return TLS_FLS_STATUS_ENOMEM;
	    }

	    offaddr = addrfor1M&(INSIDE_FLS_BASE_ADDR -1);			//Offset of 0X08000000
	    secpos = offaddr/INSIDE_FLS_SECTOR_SIZE;				//Section addr
	    secoff = (offaddr%INSIDE_FLS_SECTOR_SIZE);			//Offset in section
	    secremain = INSIDE_FLS_SECTOR_SIZE - secoff;    // 扇区剩余空间大小

	    if(lenfor1M<=secremain)
	    {
	    	secremain=lenfor1M;								//Not bigger with remain size in section
	    }
	    while (1)
	    {
			flashRead(secpos*INSIDE_FLS_SECTOR_SIZE, cache, INSIDE_FLS_SECTOR_SIZE);

			eraseSector(secpos*INSIDE_FLS_SECTOR_SIZE);
	        for (i = 0; i < secremain; i++) // 复制
	        {
	            cache[i + secoff] = buf[i];
	        }
	        for (i = 0; i < (INSIDE_FLS_SECTOR_SIZE / INSIDE_FLS_PAGE_SIZE); i++)
	        {
				programPage(secpos*INSIDE_FLS_SECTOR_SIZE + i*INSIDE_FLS_PAGE_SIZE, INSIDE_FLS_PAGE_SIZE, &cache[i*INSIDE_FLS_PAGE_SIZE]);	//Write
	        }
	        if(lenfor1M == secremain)
	        {
	            break;              // 写入结束了
	        }
	        else                    // 写入未结束
	        {
	            secpos++;           // 扇区地址增1
	            secoff = 0;         // 偏移位置为0
	            buf += secremain;   // 指针偏移
		   lenfor1M -= secremain;					
		   if(lenfor1M > (INSIDE_FLS_SECTOR_SIZE))
		       secremain = INSIDE_FLS_SECTOR_SIZE; // 下一个扇区还是写不完
		   else
		      secremain = lenfor1M;					//Next section will finish
	        }
	    }

	    tls_mem_free(cache);
	    tls_os_sem_release(inside_fls->fls_lock);
	}

	if (inner2flashsize)
	{
		addrfor2M = addr >= FLASH_1M_END_ADDR ? addr : ((addr + len ) >= FLASH_1M_END_ADDR ? FLASH_1M_END_ADDR : 0xFFFFFFFF);
		if (addrfor2M != 0xFFFFFFFF)
		{
			lenfor2M  = len - lenfor1M;
			lenfor2M = (addrfor2M + lenfor2M) <= (FLASH_BASE_ADDR | flashtotalsize)? lenfor2M : ((FLASH_BASE_ADDR | flashtotalsize) - addrfor2M);
		    return tls_spifls_write((addrfor2M&0xFFFFF), buf + lenfor1M, lenfor2M);
		}
	}
    return TLS_FLS_STATUS_OK;
}

/**
 * @brief          	This function is used to erase the appoint sector
 *
 * @param[in]      	sector 	sector num of the flash, 4K byte a sector	
 *
 * @retval         	TLS_FLS_STATUS_OK	    	if read sucsess
 * @retval         	other	    				if read fail
 *
 * @note           	None
 */
int tls_fls_erase(u32 sector)
{
	u32 addr;
	if (sector < (inner1flashsize/INSIDE_FLS_SECTOR_SIZE + INSIDE_FLS_BASE_ADDR/INSIDE_FLS_SECTOR_SIZE))
	{
	    if (inside_fls == NULL)
	    {
	        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
	        return TLS_FLS_STATUS_EPERM;
	    }

	    tls_os_sem_acquire(inside_fls->fls_lock, 0);

	    addr = sector*INSIDE_FLS_SECTOR_SIZE;

	    eraseSector(addr);

	    tls_os_sem_release(inside_fls->fls_lock);
	}
	else if (sector < (flashtotalsize/INSIDE_FLS_SECTOR_SIZE + INSIDE_FLS_BASE_ADDR/INSIDE_FLS_SECTOR_SIZE))
	{
	    return tls_spifls_erase(sector&0xFF);
	}

    return TLS_FLS_STATUS_OK;
}


static u8 *gsflscache = NULL;
//static u32 gsSecOffset = 0;
static u32 gsSector = 0;


/**
 * @brief          	This function is used to flush the appoint sector
 *
 * @param      	None	
 *
 * @return         	None	    	
 *
 * @note           	The caller should use fls_lock semphore to protect flash operation!
 */
static void tls_fls_flush_sector(void)
{
    int i;
    u32 addr;	
    if (gsSector < (inner1flashsize/INSIDE_FLS_SECTOR_SIZE + INSIDE_FLS_BASE_ADDR/INSIDE_FLS_SECTOR_SIZE))
    {
	    addr = gsSector*INSIDE_FLS_SECTOR_SIZE;

	    eraseSector(addr);
	    for (i = 0; i < INSIDE_FLS_SECTOR_SIZE / INSIDE_FLS_PAGE_SIZE; i++)
	    {
	        programPage(gsSector * INSIDE_FLS_SECTOR_SIZE +
	                    i * INSIDE_FLS_PAGE_SIZE, INSIDE_FLS_PAGE_SIZE,
	                    &gsflscache[i * INSIDE_FLS_PAGE_SIZE]);
	    }
    }
    else if (gsSector < (flashtotalsize/INSIDE_FLS_SECTOR_SIZE + INSIDE_FLS_BASE_ADDR/INSIDE_FLS_SECTOR_SIZE))
    {
        addr = gsSector*INSIDE_FLS_SECTOR_SIZE;
        tls_spifls_write(addr&0xFFFFF, gsflscache, INSIDE_FLS_SECTOR_SIZE);
    }
    //gsSecOffset = 0;

}


/**
 * @brief          	This function is used to fast write flash initialize
 *
 * @param      	None	
 *
 * @retval         	TLS_FLS_STATUS_OK	    	sucsess
 * @retval         	other	    				fail    	
 *
 * @note           	None
 */
int tls_fls_fast_write_init(void)
{

    if (inside_fls == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EPERM;
    }
    if (NULL != gsflscache)
    {
        TLS_DBGPRT_ERR("tls_fls_fast_write_init installed!\n");
        return -1;
    }
    gsflscache = tls_mem_alloc(INSIDE_FLS_SECTOR_SIZE);
    if (NULL == gsflscache)
    {
        TLS_DBGPRT_ERR("tls_fls_fast_write_init malloc err!\n");
        return -1;
    }
    return TLS_FLS_STATUS_OK;
}

/**
 * @brief          	This function is used to destroy fast write flash
 *
 * @param      	None	
 *
 * @return         	None	    	
 *
 * @note           	None
 */
void tls_fls_fast_write_destroy(void)
{
    if (NULL != gsflscache)
    {
        if (inside_fls == NULL)
        {
            TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
            return;
        }else{
            tls_os_sem_acquire(inside_fls->fls_lock, 0);	
            tls_fls_flush_sector();
            tls_os_sem_release(inside_fls->fls_lock);	
        }

        tls_mem_free(gsflscache);
        gsflscache = NULL;
    }
}

/**
 * @brief          	This function is used to fast write data to the flash.
 *
 * @param[in]      	addr     	is byte offset addr for write to the flash
 * @param[in]      	buf       	is the data buffer want to write to flash
 * @param[in]      	length  	is the byte length want to write
 *
 * @retval         	TLS_FLS_STATUS_OK	success
 * @retval        	other				fail	
 *
 * @note           	None
 */
int tls_fls_fast_write(u32 addr, u8 * buf, u32 length)
{

    u32 sector, offset, maxlen, len;

    if (inside_fls == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EPERM;
    }

    if(((addr&(INSIDE_FLS_BASE_ADDR-1)) >=  flashtotalsize) || (length == 0) || (buf == NULL))
    {
        return TLS_FLS_STATUS_EINVAL;
    }
    tls_os_sem_acquire(inside_fls->fls_lock, 0);

    sector = addr / INSIDE_FLS_SECTOR_SIZE;
    offset = addr % INSIDE_FLS_SECTOR_SIZE;
    maxlen = INSIDE_FLS_SECTOR_SIZE;

    if ((sector != gsSector) && (gsSector != 0))
    {
        tls_fls_flush_sector();
    }
    gsSector = sector;
    if (offset > 0)
    {
        maxlen -= offset;
    }
    while (length > 0)
    {
        len = (length > maxlen) ? maxlen : length;
        MEMCPY(gsflscache + offset, buf, len);
        if (offset + len >= INSIDE_FLS_SECTOR_SIZE)
        {
            tls_fls_flush_sector();
            gsSector++;
        }
        offset = 0;
        maxlen = INSIDE_FLS_SECTOR_SIZE;
        sector++;
        buf += len;
        length -= len;
    }

    tls_os_sem_release(inside_fls->fls_lock);	

    return TLS_FLS_STATUS_OK;
}


/**
 * @brief          	This function is used to erase flash all chip
 *
 * @param      	None	
 *
 * @retval         	TLS_FLS_STATUS_OK	    	sucsess
 * @retval         	other	    				fail    	
 *
 * @note           	None
 */
int tls_fls_chip_erase(void)
{
	int i,j;
	u8 *cache;

    if (inside_fls == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EPERM;
    }

	tls_os_sem_acquire(inside_fls->fls_lock, 0);

    cache = tls_mem_alloc(INSIDE_FLS_SECTOR_SIZE);
    if (cache == NULL)
    {
        tls_os_sem_release(inside_fls->fls_lock);
        TLS_DBGPRT_ERR("allocate sector cache memory fail!\n");
        return TLS_FLS_STATUS_ENOMEM;
    }


	for( i = 0; i < ( inner1flashsize - (INSIDE_FLS_SECBOOT_ADDR&0xFFFFF))/INSIDE_FLS_SECTOR_SIZE; i ++)
    {
		flashRead(INSIDE_FLS_SECBOOT_ADDR + i*INSIDE_FLS_SECTOR_SIZE, cache, INSIDE_FLS_SECTOR_SIZE);	
		for (j = 0;j < INSIDE_FLS_SECTOR_SIZE; j++)
		{
			if (cache[j] != 0xFF)
    		{
				eraseSector(INSIDE_FLS_SECBOOT_ADDR + i*INSIDE_FLS_SECTOR_SIZE);
				break;
			}
		}
    }

	if (inner2flashsize)
	{
		tls_spifls_chip_erase();
	}

	tls_mem_free(cache);

	tls_os_sem_release(inside_fls->fls_lock);

    return TLS_FLS_STATUS_OK;
}


/**
 * @brief          	This function is used to get flash param
 *
 * @param[in]      	type		the type of the param need to get
 * @param[out]     	param	point to addr of out param
 *
 * @retval         	TLS_FLS_STATUS_OK	    	sucsess
 * @retval         	other	    				fail    	
 *
 * @note           	None
 */
int tls_fls_get_param(u8 type, void *param)
{
    int err;


    if (inside_fls == NULL)
    {
        TLS_DBGPRT_ERR("flash driver module not beed installed!\n");
        return TLS_FLS_STATUS_EPERM;
    }

    if (param == NULL)
    {
        return TLS_FLS_STATUS_EINVAL;
    }
    tls_os_sem_acquire(inside_fls->fls_lock, 0);
    err = TLS_FLS_STATUS_OK;
    switch (type)
    {
        case TLS_FLS_PARAM_TYPE_ID:
            *((u32 *) param) = 0x2013;
            break;

        case TLS_FLS_PARAM_TYPE_SIZE:
            *((u32 *) param) = flashtotalsize;
            break;

        case TLS_FLS_PARAM_TYPE_PAGE_SIZE:
            *((u32 *) param) = INSIDE_FLS_PAGE_SIZE;
            break;

        case TLS_FLS_PARAM_TYPE_PROG_SIZE:
            *((u32 *) param) = INSIDE_FLS_PAGE_SIZE;
            break;

        case TLS_FLS_PARAM_TYPE_SECTOR_SIZE:
            *((u32 *) param) = INSIDE_FLS_SECTOR_SIZE;
            break;

        default:
            TLS_DBGPRT_WARNING("invalid parameter ID!\n");
            err = TLS_FLS_STATUS_EINVAL;
            break;
    }
    tls_os_sem_release(inside_fls->fls_lock);
    return err;
}

/**
 * @brief          	This function is used to initialize the flash module
 *
 * @param      	None
 *
 * @retval         	TLS_FLS_STATUS_OK	    	sucsess
 * @retval         	other	    				fail    	
 *
 * @note           	None
 */
int tls_fls_init(void)
{
    struct tls_inside_fls *fls;
    int err;
	u32 id = 0;

    if (inside_fls != NULL)
    {
        TLS_DBGPRT_ERR("flash driver module has been installed!\n");
        return TLS_FLS_STATUS_EBUSY;
    }

    fls = (struct tls_inside_fls *) tls_mem_alloc(sizeof(struct tls_inside_fls));
    if (fls == NULL)
    {
        TLS_DBGPRT_ERR("allocate @inside_fls fail!\n");
        return TLS_FLS_STATUS_ENOMEM;
    }

    memset(fls, 0, sizeof(*fls));
    err = tls_os_sem_create(&fls->fls_lock, 1);
    if (err != TLS_OS_SUCCESS)
    {
        tls_mem_free(fls);
        TLS_DBGPRT_ERR("create semaphore @fls_lock fail!\n");
        return TLS_FLS_STATUS_ENOMEM;
    }
    inside_fls = fls;

	inner1flashsize = getFlashDensity();
	if (TLS_FLS_STATUS_OK == tls_spifls_read_id(&id))
	{
		id = (id>>16)&0xFF;
		inner2flashsize = (id ?(1<<id):0);
	}
	flashtotalsize = inner1flashsize+inner2flashsize;

    return TLS_FLS_STATUS_OK;
}

int tls_fls_exit(void)
{
    TLS_DBGPRT_FLASH_INFO("Not support flash driver module uninstalled!\n");
    return TLS_FLS_STATUS_EPERM;
}

