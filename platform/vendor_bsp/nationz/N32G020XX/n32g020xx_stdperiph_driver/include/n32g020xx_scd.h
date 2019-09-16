/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file n32g020xx_scd.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief 智能卡从端模块接口声明文件。
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup SCD SCD智能卡从端模块
 * @{
 */

#ifndef __N32G020XX_SCD_H__
#define __N32G020XX_SCD_H__

//------------------------------------------------------------------
//                        Headers
//------------------------------------------------------------------

#include "n32g020xx.h"
#include "n32g020xx_common.h"
#include "n32g020xx_cgu.h"
#include "n32g020xx_iom.h"


//------------------------------------------------------------------
//                        MacroDefs
//------------------------------------------------------------------
#define IN
//#define OUT
enum
{
    SCD_SUCCESS = 0,     ///< 操作成功
    SCD_PARAM_INCORRECT, ///< 参数错误
};

#define SCD_GTRHSetData(x)       SET_REG_VAL(SCD->GTRH, x)
#define SCD_BPRHSetData(x)		SET_REG_VAL((SCD->BPRH), (x))
#define SCD_BPRHRead()			GET_REG_VAL((SCD->BPRH))

#define SCD_BPRLSetData(x)		SET_REG_VAL((SCD->BPRL), (x))

#define SCD_IER1Dis()			SET_REG_VAL((SCD->IER1), 0)
#define SCD_IER2Dis()			SET_REG_VAL((SCD->IER2), 0)
#define SCD_TDRSetData(x)		SET_REG_VAL((SCD->TDR), (x))



#define SCD_IDLERead()			GET_REG_BIT((SCD->ISR2), BIT5) ///< 为1表示发送空闲状态 或者接收空闲状态

#define SCD_TX_ENDClr()			SET_REG_VAL((SCD->ISR2), BIT4)///< 发送完成标志位
#define SCD_TX_ENDRead()			GET_REG_BIT((SCD->ISR2), BIT4)

#define SCD_SynEndClr()			SET_REG_VAL((SCD->ISR2), BIT3)///< 核时钟域信号同步到卡时钟域，同步完成标志位
#define SCD_SynEndRead()			GET_REG_BIT((SCD->ISR2), BIT3)

#define SCD_ETIMER_ENDClr()		SET_REG_VAL((SCD->ISR2), BIT2)///< 硬件自动计数ETU完成标志位
#define SCD_ETIMER_ENDRead()		GET_REG_BIT((SCD->ISR2), BIT2)

#define SCD_SENDCRC_ENDClr()		SET_REG_VAL((SCD->ISR2), BIT1)///< 硬件发送CRC码完成标志位
#define SCD_SENDCRC_ENDRead()	GET_REG_BIT((SCD->ISR2), BIT1)

#define SCD_CRCBYTE_ENDClr()		SET_REG_VAL((SCD->ISR2), BIT0)///< 字节CRC码计算完成标志位
#define SCD_CRCBYTE_ENDRead()	GET_REG_BIT((SCD->ISR2), BIT0)
#define SCD_ISR2_ClrAll()		SET_REG_VAL((SCD->ISR2), 0x1f)
#define SCD_ISR2Read()			GET_REG_VAL((SCD->ISR2))


//#define SCD_FIFO_NERead()		NAZ_HAL_ReadRegBitCheck(SCD_ISR1, 7)
#define SCD_FIFO_NERead()		GET_REG_BIT((SCD->ISR1), BIT7)///< 接收FIFO非空标志


//#define SCD_FIFO_FULLRead()		NAZ_HAL_ReadRegBitCheck(SCD_ISR1, 6)
#define SCD_FIFO_FULLRead()		GET_REG_BIT((SCD->ISR1), BIT6)///< 接收FIFO全满标志


//#define SCD_FIFO_FOVRead()		NAZ_HAL_ReadRegBitCheck((SCD->ISR1)), 5)
#define SCD_FIFO_FOVRead()		GET_REG_BIT((SCD->ISR1), BIT5)///< 接收FIFO溢出标志


//#define SCD_FIFO_FOVClr()		SET_REG_BIT_MSK((SCD->ISR1)), BIT5, BIT5)
#define SCD_FIFO_FOVClr()		SET_REG_VAL((SCD->ISR1), BIT5)///< 清FIFO溢出标志


//#define SCD_FIFO_HFURead()		NAZ_HAL_ReadRegBitCheck((SCD->ISR1)), 4)
#define SCD_FIFO_HFURead()		GET_REG_BIT((SCD->ISR1), BIT4)///< 检查接收 FIFO半满标志


//#define SCD_FIFO_HFUClr()		SET_REG_BIT_MSK((SCD->ISR1)), BIT4, BIT4)
#define SCD_FIFO_HFUClr()		SET_REG_VAL((SCD->ISR1), BIT4)///< FIFO半满 写1清零



//#define SCD_FT2R_ENDRead()		NAZ_HAL_ReadRegBitCheck((SCD->ISR1)), 2)
#define SCD_FT2R_ENDRead()		GET_REG_BIT((SCD->ISR1), BIT2)///< 检查快发转接收已完成



//#define SCD_FT2R_ENDClr()		SET_REG_BIT_MSK((SCD->ISR1)), BIT2, BIT2)
#define SCD_FT2R_ENDClr()		SET_REG_VAL((SCD->ISR1), BIT2)///< 快发转接收已完成



//#define SCD_TR_ERRORRead()		NAZ_HAL_ReadRegBitCheck((SCD->ISR1)), 1)
#define SCD_TR_ERRORRead()		GET_REG_BIT((SCD->ISR1), BIT1)///< 有奇偶校验错误 写1清零



//#define SCD_TR_ERRORClr()		SET_REG_BIT_MSK((SCD->ISR1)), BIT1, BIT1)
#define SCD_TR_ERRORClr()		SET_REG_VAL((SCD->ISR1), BIT1)///< 有奇偶校验错误 写1清零



//#define SCD_FIFO_TFEMRead()		NAZ_HAL_ReadRegBitCheck((SCD->ISR1)), 0)
#define SCD_FIFO_TFEMRead()		GET_REG_BIT((SCD->ISR1), BIT0)


//#define SCD_ISR1_ClrAll()		NAZ_HAL_WriteRegBitVal((SCD->ISR1)),  1,  5,  0x1f)
#define SCD_ISR1_ClrAll()		SET_REG_VAL((SCD->ISR1),0x3e)


#define SCD_RDRRead()			GET_REG_VAL((SCD->RDR))

#define SCD_CRCINITSETSetData(x)	SET_REG_VAL((SCD->CRCINITSET), (x))
#define SCD_CRCDATASetData(x)	SET_REG_VAL((SCD->CRCDATA), (x))

#define SCD_CRCHRead()			GET_REG_VAL((SCD->CRCH))
#define SCD_CRCLRead()			GET_REG_VAL((SCD->CRCL))


#define SCD_EECRL_Set(x)			SET_REG_VAL((SCD->EECRL), (x))///< SCD ETU Timer低位寄存器

#define SCD_EECRH_Set(x)			SET_REG_VAL((SCD->EECRH), (x))///< SCD ETU Timer高位寄存器
#define SCD_EECRL_Clr()			SET_REG_VAL((SCD->EECRL), 0)
#define SCD_EECRH_Clr()			SET_REG_VAL((SCD->EECRH), 0)



#define SCD_EtuEn()				SET_REG_BIT_MSK((SCD->ETUEN),BIT0, BIT0)///< 外部Etu计数器使能打开

#define SCD_EtuDis()				SET_REG_BIT_MSK((SCD->ETUEN), 0, BIT0)///< 外部Etu计数器使能关闭，SCDEECR的值固定为0
#define SCD_EtuCheck()  			GET_REG_BIT((SCD->ETUEN), BIT0);


#define SCD_T1TxParbEn()			SET_REG_BIT_MSK((SCD->ETUEN),BIT6, BIT6)///< 协议0/1发送奇偶校验位，同时接收反馈位

#define SCD_T1TxParbDis()		SET_REG_BIT_MSK((SCD->ETUEN), 0, BIT6)///< 协议0/1不发送奇偶校验位，也不接收反馈位

#define SCD_T1TxParbCheck()		GET_REG_BIT((SCD->ETUEN), BIT6)

#define SCD_ParEAckEn()			SET_REG_BIT_MSK((SCD->ETUEN), BIT5, BIT5)///< 协议1/0接收数据奇偶校验错误时，产生奇偶校验反馈

#define SCD_ParEAckDis()			SET_REG_BIT_MSK((SCD->ETUEN), 0, BIT5)///< 协议1/0接收数据奇偶校验错误时，不产生奇偶校验反馈
#define SCD_ParEAckCheck()			GET_REG_BIT((SCD->ETUEN), BIT5)
//#define SCD_ReTimes(t) 	   {SCDETUEN &= ~(SCD_RE_TIMES_MASK << 1);SCDETUEN |= ((SCD_RE_TIMES_MASK & t) << 1);}
#define SCD_ReTimes(t)			SET_REG_BIT_MSK((SCD->ETUEN), t, BIT1|BIT2|BIT3|BIT4 ) //need clr0?
#define SCD_ETUENRead()			GET_REG_VAL((SCD->ETUEN))
//MACRO DEFINE FUNCTIONS
///<SCD_IER1

#define SCD_CoreFNEEn()			SET_REG_BIT_MSK((SCD->IER1), BIT7, BIT7)///< 使能接收FIFO非空中断

#define SCD_CoreFNEDis()			SET_REG_BIT_MSK((SCD->IER1), 0, BIT7)///< 禁止接收FIFO非空中断

#define SCD_CoreFNECheck()			GET_REG_BIT((SCD->IER1), BIT7)

#define SCD_CardFFUEn()			SET_REG_BIT_MSK((SCD->IER1), BIT6, BIT6)///< 使能接收FIFO全满中断

#define SCD_CardFFUDis()			SET_REG_BIT_MSK((SCD->IER1), 0, BIT6)///< 禁止接收FIFO全满中断

#define SCD_CardFFUCheck()			GET_REG_BIT((SCD->IER1), BIT6)

#define SCD_CardFOVEn()			SET_REG_BIT_MSK((SCD->IER1), BIT5, BIT5)///< 使能接收FIFO溢出中断

#define SCD_CardFOVDis()			SET_REG_BIT_MSK((SCD->IER1), 0, BIT5)///< 禁止接收FIFO溢出中断
#define SCD_CardFOVCheck()  		GET_REG_BIT((SCD->IER1), BIT5)


#define SCD_CardFHFUEn()			SET_REG_BIT_MSK((SCD->IER1), BIT4, BIT4)///< 使能接收FIFO半满中断

#define SCD_CardFHFUDis()		SET_REG_BIT_MSK((SCD->IER1), 0, BIT4)///< 禁止接收FIFO半满中断

#define SCD_CardFHFUCheck()		GET_REG_BIT((SCD->IER1), BIT4)


#define SCD_FT2REndEn()			SET_REG_BIT_MSK((SCD->IER1), BIT2, BIT2)///< 使能快速发送转接收模式完成中断

#define SCD_FT2REndDis()			SET_REG_BIT_MSK((SCD->IER1), 0, BIT2)///< 禁止快速发送转接收模式完成中断

#define SCD_FT2REndCheck()		GET_REG_BIT((SCD->IER1), BIT2)


#define SCD_TREEn()				SET_REG_BIT_MSK((SCD->IER1), BIT1, BIT1)///< 使能SCD发送/接收奇偶校验错误中断

#define SCD_TREDis()				SET_REG_BIT_MSK((SCD->IER1), 0, BIT1)///< 禁止SCD发送/接收奇偶校验错误中断

//#define SCD_TRECheck()				NAZ_HAL_ReadRegBitCheck(SCD_IER1, 1)
#define SCD_TRECheck()				GET_REG_BIT((SCD->IER1), BIT1)


#define SCD_TFEMEn()				SET_REG_BIT_MSK((SCD->IER1), BIT0, BIT0)///< 使能发送FIFO的空中断

#define SCD_TFEMDis()			SET_REG_BIT_MSK((SCD->IER1), 0, BIT0)///< 禁止发送FIFO的空中断

#define SCD_TFEMCheck()			GET_REG_BIT((SCD->IER1), BIT0)

//SCDIER2

#define SCD_TXEndEn()			SET_REG_BIT_MSK((SCD->IER2), BIT4, BIT4)///< 使能发送完成中断

#define SCD_TXEndDis()			SET_REG_BIT_MSK((SCD->IER2), 0, BIT4)///< 禁止发送完成中断
#define SCD_TXEndCheck()			GET_REG_BIT((SCD->IER2), BIT4)

#define SCD_SYNEndEn()			SET_REG_BIT_MSK((SCD->IER2), BIT3, BIT3)///< 使能卡时钟域到核时钟域信号同步完成中断

#define SCD_SYNEndDis()			SET_REG_BIT_MSK((SCD->IER2), 0, BIT3)///< 禁止卡时钟域到核时钟域信号同步完成中断

#define SCD_SYNEndCheck()			GET_REG_BIT((SCD->IER2), BIT3)

#define SCD_ETimerEndEn()		SET_REG_BIT_MSK((SCD->IER2), BIT2, BIT2)///< 使能硬件自动计数etu完成状态位中断

#define SCD_ETimerEndDis()		do{SET_REG_BIT_MSK((SCD->IER2), 0, BIT2);SCD_EECRL_Clr();SCD_EECRH_Clr();}while(0)///< 禁止硬件自动计数etu完成状态位中断

#define SCD_ETimerEndCheck()		GET_REG_BIT((SCD->IER2), BIT2)




#define SCD_SendCRCEndEn()		SET_REG_BIT_MSK((SCD->IER2), BIT1, BIT1)///< 使能发送2ByteCRC校验码完成状态位中断

#define SCD_SendCRCEndDis()		SET_REG_BIT_MSK((SCD->IER2), 0, BIT1)///< 禁止发送2ByteCRC校验码完成状态位中断

#define SCD_SendCRCEndCheck()		GET_REG_BIT((SCD->IER2), BIT1)




#define SCD_CRCByteEndEn()		SET_REG_BIT_MSK((SCD->IER2), BIT0, BIT0)///< 使能1byte数据CRC校验码计算完成中断

#define SCD_CRCByteEndDis()		SET_REG_BIT_MSK((SCD->IER2), 0, BIT0)///< 禁止1byte数据CRC校验码计算完成中断

#define SCD_CRCByteEndCheck()	GET_REG_BIT((SCD->IER2), BIT0)


//SCDCR1
#define SCD_DmaMdSet1()			SET_REG_BIT_MSK((SCD->CR1), BIT7, BIT7)
#define SCD_DmaMdSet0()			SET_REG_BIT_MSK((SCD->CR1), 0, BIT7)

#define SCD_DmaMdCheck()		GET_REG_BIT((SCD->CR1), BIT7)



#define SCD_DMARam2Scd()		do{SCD_SynEndClr();SCD_DmaMdSet1(); while(!SCD_SynEndRead());}while(0)///< DMA搬移方向为SRAM到SCD

#define SCD_DMAScd2Ram()		do{SCD_SynEndClr();SCD_DmaMdSet0(); while(!SCD_SynEndRead());}while(0)///< DMA搬移方向为SCD到SRAM

#define SCD_PAREnSet1()			SET_REG_BIT_MSK((SCD->CR1), BIT6, BIT6)
#define SCD_PAREnSet0()			SET_REG_BIT_MSK((SCD->CR1), 0, BIT6)




#define SCD_PAREn()				do{SCD_SynEndClr();SCD_PAREnSet1(); while(!SCD_SynEndRead());}while(0)///< 使能奇偶校验

#define SCD_PARDis()			do{SCD_SynEndClr();SCD_PAREnSet0(); while(!SCD_SynEndRead());}while(0)///< 禁止奇偶校验





#define SCD_PORTSet1()			SET_REG_BIT_MSK((SCD->CR1), BIT5, BIT5)
#define SCD_PORTSet0()			SET_REG_BIT_MSK((SCD->CR1), 0, BIT5)

#define SCD_PORT1()				{SCD_SynEndClr();SCD_PORTSet1();while(!SCD_SynEndRead());}///< SCD工作在T=1传输协议

#define SCD_PORT0()				do{SCD_SynEndClr();SCD_PORTSet0(); while(!SCD_SynEndRead());}while(0)///< SCD工作在T=0传输协议


#define SCD_SCDRst()			do{SCD_SynEndClr();SET_REG_BIT((SCD->CR1), BIT4); while(!SCD_SynEndRead());}while(0)///< 复位SCD

#define SCD_TRSTx()				do{SCD_SynEndClr();SET_REG_BIT((SCD->CR1), BIT3); while(!SCD_SynEndRead());}while(0)///< 选择发送模式

#define SCD_TRSRx()				do{SCD_SynEndClr();SET_REG_BIT_MSK((SCD->CR1), 0, BIT3); while(!SCD_SynEndRead());}while(0)///< 选择接收模式

#define SCD_RE3En()				do{SCD_SynEndClr();SET_REG_BIT((SCD->CR1), BIT2); while(!SCD_SynEndRead());}while(0)///< 使能硬件自动3次重传

#define SCD_RE3Dis()				do{SCD_SynEndClr();SET_REG_BIT_MSK((SCD->CR1), 0, BIT2); while(!SCD_SynEndRead());}while(0)///< 禁止硬件自动3次重传

#define SCD_SetParOdd()			do{SCD_SynEndClr();SET_REG_BIT((SCD->CR1), BIT1); while(!SCD_SynEndRead());}while(0)///< 偶校验

#define SCD_SetParEven()			do{SCD_SynEndClr();SET_REG_BIT_MSK((SCD->CR1), 0, BIT1); while(!SCD_SynEndRead());}while(0)///< 奇校验

#define SCD_DISInverse()			do{SCD_SynEndClr();SET_REG_BIT((SCD->CR1), BIT0); while(!SCD_SynEndRead());}while(0)///< 反向模式

#define SCD_DISDirect()			do{SCD_SynEndClr();SET_REG_BIT_MSK((SCD->CR1), 0, BIT0); while(!SCD_SynEndRead());}while(0)///< 正向模式
#define SCD_CR1Read()			GET_REG_VAL((SCD->CR1))

//SCDCR2

#define SCD_CRCDataInverse()		SET_REG_BIT_MSK(SCD->CR2, BIT7, BIT7)///< 从IO移入CRC模块的数据反向

#define SCD_CRCDataNormal()		SET_REG_BIT_MSK(SCD->CR2, 0, BIT7)///< 从IO移入CRC模块的数据不反向


#define SCD_SendCrc()			SET_REG_BIT_MSK(SCD->CR2, BIT6, BIT6)///< 硬件发送CRC码
#define SCD_SendCrcClr()			SET_REG_BIT_MSK(SCD->CR2, 0, BIT6)

#define SCD_SendCrcCheck()		GET_REG_BIT(SCD->CR2, BIT6)



#define SCD_CRCMSBHigh()			SET_REG_BIT_MSK(SCD->CR2, BIT5, BIT5)///< 从IO移入CRC模块的数据高位先移入

#define SCD_CRCMSBLow()			SET_REG_BIT_MSK(SCD->CR2, 0, BIT5)///< 从IO移入CRC模块的数据低位先移入


#define SCD_CRCModeCRCDATA()		SET_REG_BIT_MSK(SCD->CR2, BIT4, BIT4)///< CRC处理CRCDATA寄存器中的数据

#define SCD_CRCModeSCDIO()		SET_REG_BIT_MSK(SCD->CR2, 0, BIT4)///< CRC处理SCD I/O线上数据


#define SCD_CRCInitValFF()		SET_REG_BIT_MSK(SCD->CR2, BIT3, BIT3)///< CRC校验码寄存器初值为0xFF

#define SCD_CRCInitVal00()		SET_REG_BIT_MSK(SCD->CR2, 0, BIT3)///< CRC校验码寄存器初值为0x00


#define SCD_SendLBSet()			SET_REG_BIT_MSK(SCD->CR2, BIT0, BIT0)///< 使能快发转接收

#define SCD_SendLBClr()			SET_REG_BIT_MSK(SCD->CR2, 0, BIT0)///< 禁止快发转接收
#define SCD_CR2Read()			GET_REG_VAL(SCD->CR2)


typedef struct
{
    IN uint8_t mode;///< 三种传输模式(正常 中断 DMA)
    IN uint8_t dma_ch;
    IN uint8_t dma_pri;
    IN uint8_t rstway;///< 复位形式
    IN uint8_t parity;///< 奇偶校验
    IN uint8_t protocol;///< 传输协议 T = 0 还是T = 1
    IN uint8_t dir;///< 正向卡还是反向卡
    IN uint8_t fidi;///< 波特率
    IN uint8_t guardtime;///< 保护时间
} CARD_ATTRIBUTE;

typedef enum
{
    SCD_NORMAL_MODE = 0, ///< /*正常传输模式*/
    SCD_INT_MODE,       ///< /*中断传输模式*/
    SCD_DMA_MODE,      ///< /*DMA传输模式*/
} SCD_TRANS_MODE;


typedef enum
{
    SCD_RISING_EDGE_RST = 0,  ///<  /*复位信号上升沿触发scu中断*/
    SCD_FALLING_EDGE_RST,     ///<  /*复位信号下降沿触发scu中断*/
    SCD_GLOBAL_RST,           ///< /*复位信号触发全局复位*/
} SCD_RST_MODE;

typedef enum
{
    SCD_PARI_DIS = 0,      ///< /*scd关闭奇偶校验*/
    SCD_PARI_EVEN,        ///<  /*scd偶校验*/
    SCD_PARI_ODD,          ///< /*scd奇校验*/
} SCD_PARI_MODE;


//------------------------------------------------------------------
//                        Definitions
//------------------------------------------------------------------
//ISR1
//#define SCD_ISR1					(SCD->ISR1)
#define SCD_FIFO_NE				(BIT7)
#define SCD_FIFO_FU				(BIT6)
#define SCD_FIFO_OV				(BIT5)
#define SCD_FIFO_HFU			(BIT4)
#define SCD_FT2R_END	    	(BIT2)
#define SCD_TR_PE		    	(BIT1)
#define SCD_FIFO_TFEM			(BIT0)
//IER1
//#define SCD_IER1					(SCD->IER1)
#define SCD_CORE_FNE_EN			(BIT7)
#define SCD_CARD_FFU_EN			(BIT6)
#define SCD_CARD_FOV_EN			(BIT5)
#define SCD_CARD_FHFU_EN		(BIT4)
#define SCD_FT2REND_EN			(BIT2)
#define SCD_TRE_EN				(BIT1)
#define SCD_TFEM_EN				(BIT0)
//ISR2
//#define SCDISR2					(SCD->ISR2)
#define SCD_IDLE				(BIT5)
#define SCD_TX_END				(BIT4)
#define SCD_SYN_END				(BIT3)
#define SCD_ETIMER_END			(BIT2)
#define SCD_CRCSEND_END			(BIT1)
#define SCD_CRCBYTE_END			(BIT0)
//IER2
//#define SCDIER2					(SCD->IER2)
#define SCD_TX_END_EN			(BIT4)
#define SCD_SYN_END_EN			(BIT3)
#define SCD_ETIMER_END_EN		(BIT2)
#define SCD_SEND_CRC_END_EN		(BIT1)
#define SCD_CRCBYTE_END_EN		(BIT0)
//CR1
//#define SCDCR1					(SCD->CR1)
#define SCD_DMA_MODE_RAM2SCD	(BIT7)
#define SCD_PAR_EN				(BIT6)
#define SCD_PROT_T1				(BIT5)
#define SCD_SCDRST				(BIT4)
#define SCD_TRS_TX				(BIT3)
#define SCD_RE3_EN				(BIT2)
#define SCD_ODD_EVEN			(BIT1)
#define SCD_DIS					(BIT0)
//CR2
//#define SCDCR2					(SCD->CR2)
#define SCD_CRCDATA_INV			(BIT7)
#define SCD_SEND_CRC			(BIT6)
#define SCD_CRC_MSB				(BIT5)
#define SCD_CRC_MODE			(BIT4)
#define SCD_CRCINIT_VAL			(BIT3)
#define SCD_SEND_LB				(BIT0)

#define SCD_GTRL_MASK			(0xf0)
#define SCD_BPRH_MASK			(0x0f)

#define SCD_EECRH_MASK			(0x3f)


#define SCD_TXPARB_EN			(BIT6)
#define SCD_PAREACKEN			(BIT5)
#define SCD_RE_TIMES_MASK		(0xf)
#define SCD_ETU_EN				(BIT0)


//------------------------------------------------------------------
//                        TypeDefs
//------------------------------------------------------------------

//------------------------------------------------------------------
//                        Exported variables
//------------------------------------------------------------------


//------------------------------------------------------------------
//                        Exported functions
//------------------------------------------------------------------


/**
 * @brief  scd ETU计数器 延迟
 * @param  num -
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_DelayEtu(uint32_t num);

/**
 * @brief  scd设置ETU计数器
 * @param  num -
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_SetETUTimer(uint32_t num);


/**
 * @brief  scd设置ETU开启计数器
 * @param  num -
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_EtuTimerStart(uint32_t num);

/**
 * @brief  scd设置ETU停止计数器
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_EtuTimerStop(void);                          

/**
 * @brief  scd初始化函数,依据CARD_ATTRIBUTE结构体设置相关属性
 * @param  card 初始化设置结构体
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_Init(CARD_ATTRIBUTE* card);                                 

/**
 * @brief   scd关闭函数，关闭所有时钟、中断等;
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_Close(void);
                                                       
/**
 * @brief  SCD发送ATR函数，用于SCD接收到复位信号后
 * @param  atr   atr数组
 * @param  bytelen  atr字节长度
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_SendAtr(uint8_t* atr,uint8_t bytelen);      

/**
 * @brief  用于设置协议规定的BPR
 * FI/DI
 * @param  fidi
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_SetBpr(uint8_t fidi);

/**
 * @brief  默认卡设置，用于scdinit之前。该默认配置为:正常传输、偶校验、T0协议、ETU=372、GT=0、复位信号上升沿触发scu中断;
 * @param  card
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_SetDefaultCard(CARD_ATTRIBUTE* card);

/**
 * @brief  scd字节发送函数
 * @param  data
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_SendByte(uint8_t data);
                                                     
/**
 * @brief  scd发送字节函数，用于快发转接收的场合
 * @param  data
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_SendLastByte(uint8_t data);               

/**
 * @brief  scd发送函数
 * @param  buf  发送数据数组
 * @param  len    发送数据长度
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_Send(uint8_t * buf, uint32_t len);        

/**
 * @brief  scd中断处理函数
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_IrqService(void);                          

/**
 * @brief  scd接收数据函数
 * @param  buf   接收数据数组
 * @param  len    接收数据长度
 * @return  `SCD_SUCCESS`表示成功，`SCD_PARAM_INCORRECT`表示参数错误
 */
int32_t SCD_Receive(uint8_t * buf, uint8_t len);          

#endif /*__SCD_DRV_H__*/                                      
/** @} */
