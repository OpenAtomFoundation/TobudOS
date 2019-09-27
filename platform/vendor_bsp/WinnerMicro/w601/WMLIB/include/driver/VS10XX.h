/**
 * @file    VS10XX.h
 *
 * @brief   VS10XX driver
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef __VS10XX_H__
#define __VS10XX_H__
//#include "sys.h"
#include "wm_type_def.h"

/** VS10XX ID list*/
#define	 VS1001 	0
#define	 VS1011 	1
#define	 VS1002 	2
#define	 VS1003 	3
#define	 VS1053 	4
#define	 VS1033 	5
#define	 VS1103 	7
extern u8 VS10XX_ID;	  		  /** VS10XX id */
/**command */
#define VS_WRITE_COMMAND 	0x02  /** write command */
#define VS_READ_COMMAND 	0x03  /** read command */

/** VS10XX register definition */
#define SPI_MODE        	0x00  /** mode control */
#define SPI_STATUS      	0x01  /** VS10XX status register */
#define SPI_BASS        	0x02  /** bass control */
#define SPI_CLOCKF      	0x03  /** Clock frequency multiplier register */
#define SPI_DECODE_TIME 	0x04  /** Decoding time length */
#define SPI_AUDATA      	0x05  /** Audio data */
#define SPI_WRAM        	0x06  /** RAM write/read */
#define SPI_WRAMADDR    	0x07  /** RAM write/read start addr */
#define SPI_HDAT0       	0x08  /** The data stream header 0 */
#define SPI_HDAT1       	0x09  /** The data stream header 1 */

#define SPI_AIADDR      	0x0a  /** application start addr */
#define SPI_VOL         	0x0b  /** volumn control */
#define SPI_AICTRL0     	0x0c  /** Application control register 0 */
#define SPI_AICTRL1     	0x0d  /** Application control register 1 */
#define SPI_AICTRL2     	0x0e  /** Application control register 2 */
#define SPI_AICTRL3     	0x0f  /** Application control register 3 */
//SPI_MODE的各位功能,下表根据VS1053的手册标注
#define SM_DIFF         	0x01  /**差分 */
#define SM_LAYER12         	0x02  /**允许MPEG 1,2解码 FOR vs1053 */
#define SM_RESET        	0x04  /**软件复位 */
#define SM_CANCEL       	0x08  /**取消当前解码 */
#define SM_EARSPEAKER_LO  	0x10  /**EarSpeaker低设定 */
#define SM_TESTS        	0x20  /**允许SDI测试 */
#define SM_STREAM       	0x40  /**流模式 */
#define SM_EARSPEAKER_HI   	0x80  /**EarSpeaker高设定  */
#define SM_DACT         	0x100 /**DCLK的有效边沿  */
#define SM_SDIORD       	0x200 /**SDI位顺序  */
#define SM_SDISHARE     	0x400 /**共享SPI片选  */
#define SM_SDINEW       	0x800 /**VS1002 本地SPI模式  */
#define SM_ADPCM        	0x1000/**ADPCM录音激活   */
#define SM_LINE1         	0x4000/**咪/线路1 选择  */
#define SM_CLK_RANGE     	0x8000/**输入时钟范围 	*/

//VS10xx与外部接口定义(未包含MISO,MOSI,SCK)
//#define VS_DQ       PAin(12)  		//DREQ
//#define VS_RST      PAout(11) 		//RST
//#define VS_XCS      PAout(8)  		//XCS
//#define VS_XDCS     PAout(4)  		//XDCS

//#define VS_DQ	3//13
//#define VS_RST	11
//#define VS_XCS	12
//#define VS_XDCS	2//18

#define VS_DQ				WM_GPIO_A_PIN_12
#define VS_RST				WM_GPIO_A_PIN_13
#define VS_XCS				WM_GPIO_A_PIN_11
#define VS_XDCS				WM_GPIO_A_PIN_9

#define VS_SPI_CLK_LOW		250000
#define VS_SPI_CLK_HIGH		5000000
#define VS_REC_PKG_SIZE	512

/**
 * @brief          pull cs low
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void codec_data_cs_low(void);

/**
 * @brief          pull cs high
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void codec_data_cs_high(void);

/**
 * @brief          read register
 *
 * @param[in]      address
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
u16  VS_RD_Reg(u8 address);

/**
 * @brief          write command
 *
 * @param[in]      address
 * @param[in]      data
 *
 * @return         None
 *
 * @note           None
 */
void VS_WR_Cmd(u8 address,u16 data);

/**
 * @brief          write data
 *
 * @param[in]      data
 *
 * @return         None
 *
 * @note           None
 */
void VS_WR_Data(u8 data);

/**
 * @brief          read ram
 *
 * @param[in]      addr
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
u16  VS_RD_Wram(u16 addr);

/**
 * @brief          initial VS10XX
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void VS_Init(void);

/**
 * @brief          hardware reset
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
u8   VS_HD_Reset(void);

/**
 * @brief          software reset
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void VS_Soft_Reset(void);

/**
 * @brief          ram test
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
u16  VS_Ram_Test(void);

/**
 * @brief          sine test
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void VS_Sine_Test(void);

/**
 * @brief          get decode time
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
u16  VS_Get_DecodeTime(void);

/**
 * @brief          get bitrate
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
u16  VS_Get_HeadInfo(void);

/**
 * @brief          set decode time
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void VS_Rst_DecodeTime(void);

/**
 * @brief          set VS10XX
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void set10XX(void);

/**
 * @brief          load FLAC decode code
 *
 * @param[in]      *patch
 * @param[in]      len
 *
 * @return         None
 *
 * @note           None
 */
void VS_Load_Patch(u16 *patch,u16 len);

/**
 * @brief          set volumn
 *
 * @param[in]      vol
 *
 * @return         None
 *
 * @note           None
 */
void vs_set_vol(int vol);

/**
 * @brief          mute
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void vs_mute(void);

/**
 * @brief          set record mode
 *
 * @param[in]      agc
 *
 * @return         None
 *
 * @note           None
 */
void VS_Rec_Mode(u16 agc);

/**
 * @brief          get record data
 *
 * @param[in]      recbuf
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           None
 */
int VS_Rec_GetData(char* recbuf);
#endif































