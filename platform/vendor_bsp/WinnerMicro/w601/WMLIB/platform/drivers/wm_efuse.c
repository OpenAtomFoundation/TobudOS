/**************************************************************************
 * File Name                   : tls_efuse.c
 * Author                      :
 * Version                     :
 * Date                        :
 * Description                 : Use Flash Addr as virtual efuse
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd. 
 * All rights reserved.
 *
 ***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wm_debug.h"
#include "wm_regs.h"
#include "wm_efuse.h"
#include "wm_config.h"
#include "list.h"
#include "wm_internal_flash.h"
#include "wm_crypto_hard.h"
#include "wm_timer.h"
#include "wm_cpu.h"
#include "wm_irq.h"
#include "wm_flash_map.h"

extern void flashSRRW(unsigned long offset,unsigned char *buf,unsigned long sz, unsigned char *backbuf, unsigned int backlen, unsigned int rd);


#define FT_MAGICNUM_ADDR		(FLASH_BASE_ADDR)
#define MAGICNUM_LEN			(4)
#define FT_CHECKSUM_ADDR		(FT_MAGICNUM_ADDR + MAGICNUM_LEN)
#define CHECKSUM_LEN			(4)
#define FT_DATALEN_ADDR			(FT_CHECKSUM_ADDR + CHECKSUM_LEN)
#define FT_DATALEN_LEN			(4)
#define FT_MAC_ADDR        			(FT_DATALEN_ADDR  + FT_DATALEN_LEN)
#define MAC_ADDR_LEN			(8)
#define FT_TX_DC_OFFSET_ADDR	(FT_MAC_ADDR + MAC_ADDR_LEN)
#define TX_DC_OFFSET_LEN		(4)
#define FT_RX_DC_OFFSET_ADDR	(FT_TX_DC_OFFSET_ADDR + TX_DC_OFFSET_LEN)
#define RX_DC_OFFSET_LEN		(4)
#define FT_TX_IQ_GAIN_ADDR		(FT_RX_DC_OFFSET_ADDR + RX_DC_OFFSET_LEN)
#define TX_IQ_GAIN_LEN			(4)
#define FT_RX_IQ_GAIN_ADDR		(FT_TX_IQ_GAIN_ADDR + TX_IQ_GAIN_LEN)
#define RX_IQ_GAIN_LEN			(4)
#define FT_TX_IQ_PHASE_ADDR		(FT_RX_IQ_GAIN_ADDR + RX_IQ_GAIN_LEN)
#define TX_IQ_PHASE_LEN			(4)
#define FT_RX_IQ_PHASE_ADDR		(FT_TX_IQ_PHASE_ADDR + TX_IQ_PHASE_LEN)
#define RX_IQ_PHASE_LEN			(4)
#define FT_GAIN_ADDR        			(FT_RX_IQ_PHASE_ADDR  + RX_IQ_PHASE_LEN)
#define FT_GAIN_LEN				(84)
#define FT_FIX_DATA_LEN (MAC_ADDR_LEN + TX_DC_OFFSET_LEN + RX_DC_OFFSET_LEN + TX_IQ_GAIN_LEN + RX_IQ_GAIN_LEN \
	+ TX_IQ_PHASE_LEN \
	+ RX_IQ_PHASE_LEN)


/* key paramater area -- begin -- */
/** PHY parameter area **/
#define PHY_BASE_ADDR						(FLASH_BASE_ADDR)
#define PHY_AREA_LEN						(0x1000)

/** QFlash parameter area **/
#define QFLASH_BASE_ADDR					(PHY_BASE_ADDR + PHY_AREA_LEN)
#define QFLASH_AREA_LEN						(0x1000)
#define QFLASH_HDR_BASE_ADDR				(QFLASH_BASE_ADDR)
#define QFLASH_HDR_LEN						(12)
#define QFLASH_Sec_Level_BASE_ADDR			(QFLASH_HDR_BASE_ADDR + QFLASH_HDR_LEN)
#define QFLASH_Sec_Level_LEN				(4)
#define QFLASH_CHIP_ID_BASE_ADDR			(QFLASH_Sec_Level_BASE_ADDR + QFLASH_Sec_Level_LEN)
#define QFLASH_CHIP_ID_LEN					(16)

/* key parameter area -- end -- */

typedef struct FT_PARAM
{
	unsigned int		magic_no;
	unsigned int 		checksum;
	unsigned int		data_len;
	unsigned char		mac_addr[MAC_ADDR_LEN];
	unsigned int		tx_dcoffset;
	unsigned int		rx_dcoffset;	
	unsigned int		tx_iq_gain;
	unsigned int		rx_iq_gain;	
	unsigned int		tx_iq_phase;
	unsigned int		rx_iq_phase;	
	unsigned char		tx_gain[FT_GAIN_LEN];
}FT_PARAM_ST;

static u8 default_mac[6] = {0x00,0x25,0x08,0x09,0x01,0x0F};

FT_PARAM_ST gftParam;
int tls_ft_param_init(void)
{
	int i = 0;
	int usedcnt[2] = {0, 0};
	u32 crcvalue = 0;
	psCrcContext_t ctx;	
	u8 *pmem = NULL;
	FT_PARAM_ST *pft = NULL;
	
	if (gftParam.magic_no == SIGNATURE_WORD)
	{
		return TRUE;
	}

	pmem = tls_mem_alloc(512);
	if (pmem == NULL)
	{
		return FALSE;
	}
	
	pft = tls_mem_alloc(sizeof(FT_PARAM_ST));
	if (pft == NULL)
	{
		return FALSE;
	}

	
	for (i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			tls_fls_read(FT_MAGICNUM_ADDR, (unsigned char *)pft, sizeof(FT_PARAM_ST));
		}else{
			flashSRRW(0, (unsigned char *)pft, sizeof(FT_PARAM_ST), pmem, 512, 1);
		}
		if (pft->magic_no == SIGNATURE_WORD)
		{
			tls_crypto_init();
			tls_crypto_crc_init(&ctx, 0xFFFFFFFF, CRYPTO_CRC_TYPE_32, INPUT_REFLECT | OUTPUT_REFLECT);
			tls_crypto_crc_update(&ctx, (unsigned char *)pft + 12, sizeof(FT_PARAM_ST) - 12);
			tls_crypto_crc_final(&ctx, &crcvalue);		
			if ((pft->checksum != crcvalue))
			{
				usedcnt[i] = -1;
				continue;
			}

			if (gftParam.magic_no != SIGNATURE_WORD)
			{
				memcpy(&gftParam, pft, sizeof(FT_PARAM_ST));
			}
		}else{
			usedcnt[i] = -1;
		}
	}

	if ((usedcnt[0] + usedcnt[1]) == -2)
	{
		/*Use default ft param*/
	}
	else if (usedcnt[0] < 0) /*flash param destroyed*/
	{
		tls_flash_unlock();
		tls_fls_write(FT_MAGICNUM_ADDR, (unsigned char *)&gftParam, sizeof(gftParam));
		tls_flash_lock();
	}
	else if (usedcnt[1] < 0) /*SR param destroyed*/
	{
		flashSRRW(0, (unsigned char *)&gftParam, sizeof(FT_PARAM_ST), pmem, 512, 0);		
	}
	else
	{
		if (gftParam.checksum != pft->checksum)
		{
			flashSRRW(0, (unsigned char *)&gftParam, sizeof(FT_PARAM_ST), pmem, 512, 0);
		}
	}

	tls_mem_free(pmem);
	tls_mem_free(pft);

	return TRUE;
}

int tls_ft_param_get(unsigned int opnum, void *data, unsigned int rdlen)
{
	//FT_PARAM_ST ft;

	//tls_fls_read(FT_MAGICNUM_ADDR, (unsigned char *)&ft, sizeof(ft));
	switch (opnum)
	{
		case CMD_MAC:	/*MAC*/
			if ((gftParam.mac_addr[0]&0x1)
				||(0 == (gftParam.mac_addr[0]|gftParam.mac_addr[1]|gftParam.mac_addr[2]|gftParam.mac_addr[3]|gftParam.mac_addr[4]|gftParam.mac_addr[5])))		
			{
				memcpy(data, default_mac, rdlen);
			}
			else
			{
				memcpy(data, gftParam.mac_addr, rdlen);
			}
		break;
		
		case CMD_TX_DC: /*tx_dcoffset*/
			*(unsigned int *)data = gftParam.tx_dcoffset;
		break;	
		
		case CMD_RX_DC: /*rx_dcoffset*/
			*(unsigned int *)data = gftParam.rx_dcoffset;
		break;	
		
		case CMD_TX_IQ_GAIN:
			*(unsigned int *)data = gftParam.tx_iq_gain;
		break;	
		
		case CMD_RX_IQ_GAIN:
			*(unsigned int *)data = gftParam.rx_iq_gain;
		break;
		
		case CMD_TX_IQ_PHASE:
			*(unsigned int *)data = gftParam.tx_iq_phase;
		break;	
		
		case CMD_RX_IQ_PHASE:
			*(unsigned int *)data = gftParam.rx_iq_phase;
		break;
		
		case CMD_TX_GAIN: /*gain*/
			if (rdlen < FT_GAIN_LEN)
			{
				memcpy(data, gftParam.tx_gain, rdlen);
			}
			else
			{
				memcpy(data, gftParam.tx_gain, FT_GAIN_LEN);
			}
		break;
		
		default:
		return -1;
	}
	return 0;
}

int tls_ft_param_set(unsigned int opnum, void *data, unsigned int len)
{
	psCrcContext_t ctx;
	unsigned int writelen = 0;
	u8 *pmem = NULL;



	if (!data || !len)
	{
		return -1;
	}
	//tls_fls_read(FT_MAGICNUM_ADDR, (unsigned char *)&gftParam, sizeof(gftParam));
	switch (opnum)
	{
		case CMD_MAC:	/*MAC*/
			memcpy(gftParam.mac_addr, (unsigned char *)data, len);
		break;

		case CMD_TX_DC:	/*tx_dcoffset*/
			gftParam.tx_dcoffset = *(unsigned int *)data;
		break;	
		
		case CMD_RX_DC:	/*rx_dcoffset*/
			gftParam.rx_dcoffset = *(unsigned int *)data;
		break;	
		
		case CMD_TX_IQ_GAIN:
			gftParam.tx_iq_gain = *(unsigned int *)data;
		break;	
		
		case CMD_RX_IQ_GAIN:
			gftParam.rx_iq_gain = *(unsigned int *) data;			
		break;	
		
		case CMD_TX_IQ_PHASE:
			gftParam.tx_iq_phase = *(unsigned int *)data;
		break;	
		
		case CMD_RX_IQ_PHASE:
			gftParam.rx_iq_phase = *(unsigned int *) data;			
		break;	
		
		case CMD_TX_GAIN: /*gain*/
			if (len >= FT_GAIN_LEN)
			{
				writelen = FT_GAIN_LEN;
			}
			else
			{
				writelen = len;
			}
			memcpy(gftParam.tx_gain, data, writelen);
		break;
		
		default:
		return -1;
	}

	tls_crypto_init();
	tls_crypto_crc_init(&ctx, 0xFFFFFFFF, CRYPTO_CRC_TYPE_32, INPUT_REFLECT | OUTPUT_REFLECT);
	gftParam.magic_no = SIGNATURE_WORD;
	tls_crypto_crc_update(&ctx, (unsigned char *)&gftParam + 12, sizeof(gftParam) -12);
	tls_crypto_crc_final(&ctx, &gftParam.checksum);
	tls_flash_unlock();
	tls_fls_write(FT_MAGICNUM_ADDR, (unsigned char *)&gftParam, sizeof(gftParam));
	tls_flash_lock();
	pmem = tls_mem_alloc(512);
	if (pmem)
	{
		flashSRRW(0, (unsigned char *)&gftParam, sizeof(gftParam), pmem, 512, 0);
		tls_mem_free(pmem);
	}
	return 0;
}


/**********************************************************************************************************
* Description: 	This function is used to get mac addr.
*
* Arguments  : 	mac		mac addr,6 byte
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_get_mac_addr(u8 *mac)
{
	return tls_ft_param_get(CMD_MAC, mac, 6);
}

/**********************************************************************************************************
* Description: 	This function is used to set mac addr.
*
* Arguments  : 	mac		mac addr,6 byte
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_set_mac_addr(u8 *mac)
{
	return tls_ft_param_set(CMD_MAC, mac, 6);
}


/**********************************************************************************************************
* Description: 	This function is used to get tx lod.
*
* Arguments  : 	*txlo
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_get_tx_lo(u8 *txlo)
{
	return tls_ft_param_get(CMD_TX_DC, txlo, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to set tx lo.
*
* Arguments  : 	txlo
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_tx_lo(u8 *txlo)
{
	return tls_ft_param_set(CMD_TX_DC, txlo, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to get tx iq gain.
*
* Arguments  : 	txGain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_get_tx_iq_gain(u8 *txGain)
{
	return tls_ft_param_get(CMD_TX_IQ_GAIN, txGain, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to set tx iq gain.
*
* Arguments  : 	txGain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_tx_iq_gain(u8 *txGain)
{
	return tls_ft_param_set(CMD_TX_IQ_GAIN, txGain, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to get rx iq gain.
*
* Arguments  : 	rxGain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_get_rx_iq_gain(u8 *rxGain)
{
	return tls_ft_param_get(CMD_RX_IQ_GAIN, rxGain, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to set rx iq gain.
*
* Arguments  : 	rxGain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_rx_iq_gain(u8 *rxGain)
{
	return tls_ft_param_set(CMD_RX_IQ_GAIN, rxGain, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to get tx iq phase.
*
* Arguments  : 	txPhase
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_get_tx_iq_phase(u8 *txPhase)
{
	return tls_ft_param_get(CMD_TX_IQ_PHASE, txPhase, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to set tx iq phase.
*
* Arguments  : 	txPhase
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_tx_iq_phase(u8 *txPhase)
{
	return tls_ft_param_set(CMD_TX_IQ_PHASE, txPhase, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to get rx iq phase.
*
* Arguments  : 	rxPhase
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_get_rx_iq_phase(u8 *rxPhase)
{
	return tls_ft_param_get(CMD_RX_IQ_PHASE, rxPhase, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to set tx iq phase.
*
* Arguments  : 	rxPhase
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_rx_iq_phase(u8 *rxPhase)
{
	return tls_ft_param_set(CMD_RX_IQ_PHASE, rxPhase, 4);
}


int tls_freq_err_op(u8 *freqerr, u8 flag)
{
	tls_flash_unlock();
	if (flag){
		tls_fls_write(FT_MAGICNUM_ADDR + sizeof(FT_PARAM_ST), freqerr, 4);
	}
	else
	{
		tls_fls_read(FT_MAGICNUM_ADDR + sizeof(FT_PARAM_ST), freqerr, 4);
	}
	tls_flash_lock();
	return TLS_EFUSE_STATUS_OK;
}

int tls_rf_vcg_ctrl_op(u8 *vcg, u8 flag)
{
	tls_flash_unlock();
	if (flag){
		tls_fls_write(VCG_ADDR, vcg, VCG_LEN);
	}
	else
	{
		tls_fls_read(VCG_ADDR, vcg, VCG_LEN);
	}
	tls_flash_lock();	
	return TLS_EFUSE_STATUS_OK;
}

/**********************************************************************************************************
* Description: 	This function is used to get tx gain.
*
* Arguments  : 	txgain		tx gain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_get_tx_gain(u8 *txgain)
{

	tls_ft_param_get(CMD_TX_GAIN, txgain, TX_GAIN_LEN);
		
	return TLS_EFUSE_STATUS_OK;	
}

/**********************************************************************************************************
* Description: 	This function is used to set tx gain.
*
* Arguments  : 	txgain		tx gain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_tx_gain(u8 *txgain)
{

	tls_ft_param_set(CMD_TX_GAIN, txgain, TX_GAIN_LEN);
	
	return TLS_EFUSE_STATUS_OK;	
}

#define FF_n16	0xff, 0xff, 0xff, 0xff, \
		0xff, 0xff, 0xff, 0xff, \
		0xff, 0xff, 0xff, 0xff, \
		0xff, 0xff, 0xff, 0xff
		
int tls_get_chipid(u8 chip_id[QFLASH_CHIP_ID_LEN])
{
	int ret = 0;
	static u8 _chip_id[QFLASH_CHIP_ID_LEN] = { FF_n16 };
	u8 zero[QFLASH_CHIP_ID_LEN] = { FF_n16 };

	if (!memcmp(_chip_id, zero, QFLASH_CHIP_ID_LEN))
	{
		ret = tls_fls_read(QFLASH_CHIP_ID_BASE_ADDR, _chip_id, QFLASH_CHIP_ID_LEN);
	}
	if (0 == ret)
	{
		memcpy(chip_id, _chip_id, QFLASH_CHIP_ID_LEN);
	}
	return ret;
}

#undef FF_n16

unsigned int tls_sleep(unsigned int seconds)
{
	int ret = 0;
	ret = tls_msleep(seconds * 1000);
	return ret;
}


int tls_msleep(unsigned int msec)
{
	int ret = 0;
	ret = tls_delay_via_timer(msec, TIMER_MS_UNIT_FLAG);
	return ret;
}

int tls_usleep(unsigned int /*useconds_t*/ usec)
{
	int ret = 0;
	ret = tls_delay_via_timer(usec, TIMER_US_UNIT_FLAG);
	return ret;
}
