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
 * @brief SCC智能卡主机控制模块接口实现文件。
 * @file n32g020xx_scc.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_scc.h"

//SCC SCCSR Rsg
#define  SCC_FHF_MSK      		(BIT6)
#define  SCC_FT2REND_MSK  		(BIT5)
#define  SCC_PE_MSK       		(BIT4)
#define  SCC_OVR_MSK	  		(BIT3)
#define  SCC_FER_MSK	  		(BIT2)
#define  SCC_RBF_MSK      		(BIT1)
#define  SCC_TBE_MSK      		(BIT1)
#define  SCC_FNE_MSK      		(BIT0)
//SCC SCCSR1 Rsg
#define  SCC_SYNCEND_MSK  		(BIT2)
#define  SCC_PRL_MSK      		(BIT1)
#define  SCC_ECNTOVER_MSK 		(BIT0)

#define SCC_RST_LOW_TIME                   42000

/**
 * @brief for scc interrupt read
 */
typedef struct
{
    uint8_t rp;
    uint8_t wp;
    uint8_t buf[0xFF];   
}STRUCT_7816_DATA;

#define FI(TA1) (((TA1)>>4) & 0x0F)
#define DI(TA1) (((TA1)>>0) & 0x0F)

#define FI_DI_DEFAULT   0x11

#define PROTOCOL_T0   0
#define PROTOCOL_T1   1
#define PROTOCOL_T15  15

#define PROTOCOL_DEFAULT    PROTOCOL_T0

//volatile uint32_t g_vulSccInt = 0;

//-----------------------------------------------------------------------------
//                        basefunc
//-----------------------------------------------------------------------------
//#define ISR_SCC_NUM				34//SCC


//SCC DR Rsg
#define __SCC_WriteByte(Data)  	SET_REG_VAL((SCC->DR),Data)
#define __SCC_GetDR()      		GET_REG_VAL(SCC->DR)

#define __SCC_SetUCR_RIU(val)   do{__SCC_SynEndClr();SET_REG_BIT_MSK((SCC->UCR), val, BIT1); while(!__SCC_SynEndRead());}while(0)
#define __SCC_SetTCR_TR(val)    do{__SCC_SynEndClr();SET_REG_BIT_MSK((SCC->TCR), val, BIT3); while(!__SCC_SynEndRead());}while(0)


#define __SCC_SetBPRL(Data)  	SET_REG_VAL((SCC->BPRL), Data)
#define __SCC_GetBPRL()      	GET_REG_VAL(SCC->BPRL)
#define __SCC_SetBPRH(Data)  	SET_REG_BIT_MSK(SCC->BPRH, Data, BIT0|BIT1|BIT2|BIT3)
#define __SCC_SetBPRH_Clr()  	SET_REG_BIT_MSK(SCC->BPRH, 0, BIT0|BIT1|BIT2|BIT3)
#define __SCC_GetBPRH()      	GET_REG_BIT(SCC->BPRH, BIT0|BIT1|BIT2|BIT3)

#define __SCC_SetIntEn()        SET_REG_BIT((SCC->IER), 0x7F)
#define __SCC_SetIntDis()       CLR_REG_BIT((SCC->IER), 0x7F)
#define __SCC_SetInt1En()       SET_REG_BIT((SCC->IER1), 0x07)
#define __SCC_SetInt1Dis()      CLR_REG_BIT((SCC->IER1), 0x07)


//SCC GTR Rsg
#define __SCC_SetGt(gt)         	SET_REG_VAL((SCC->GTR), (gt))
#define __SCC_GetGTR()        		GET_REG_VAL(SCC->GTR)

//SCC ECNTL&ECNTH Rsg
#define __SCC_ECNTLSet(etu)       	SET_REG_VAL((SCC->ECNTL), (etu))
#define __SCC_ECNTHSet(etu)       	SET_REG_VAL((SCC->ECNTH), (etu))
#define __SCC_GetECNTH()          	GET_REG_VAL(SCC->ECNTH)
#define __SCC_GetECNTL()          	GET_REG_VAL(SCC->ECNTL)

//SCC RETXCNT Rsg
#define __SCC_SetRETXCNT(x)		  	SET_REG_VAL((SCC->RETXCNT), (x))
#define __SCC_GetRETXCNT()		  	GET_REG_VALSCC->RETXCNT

//SCC RETXCNTRMI Rsg
#define __SCC_GetRETXCNTRMI()		GET_REG_VAL(SCC->RETXCNTRMI)


#define __SCC_EtuSetStart(etu)    do{                               \
                                    __SCC_ECNTLSet((etu) & 0xFF); 	\
                                    __asm volatile ("nop");			\
                                    __asm volatile ("nop");			\
                                    __asm volatile ("nop");			\
                                    __SCC_ECNTHSet(((etu) >> 8));  	\
                                }while(0)  

//SCC SR Rsg
#define __SCC_SetSR_Clr()  		SET_REG_VAL((SCC->SR), 0xFF)
#define __SCC_GetSR()     		GET_REG_VAL(SCC->SR)
#define __SCC_SetSR(Data)     	SET_REG_VAL((SCC->SR), (Data))

#define __SCC_FHFRead()			GET_REG_BIT((SCC->SR), BIT6)		
#define __SCC_FT2RENDRead()		GET_REG_BIT((SCC->SR), BIT5)		
#define __SCC_RBFRead()			GET_REG_BIT((SCC->SR), BIT1)		
#define __SCC_TBERead()			GET_REG_BIT((SCC->SR), BIT1)		
#define __SCC_FNERead()			GET_REG_BIT((SCC->SR), BIT0)		


//SCC SR1 Rsg
#define __SCC_GetSR1()      	GET_REG_VAL(SCC->SR1)
#define __SCC_SetSR1(Data)     	SET_REG_VAL((SCC->SR1), (Data))

#define __SCC_IDLERead()		GET_REG_BIT((SCC->SR1), BIT3)		
#define __SCC_SynEndClr()		SET_REG_BIT((SCC->SR1), BIT2)
#define __SCC_SynEndRead()		GET_REG_BIT((SCC->SR1), BIT2)
#define __SCC_ECNTOVERRead()	GET_REG_BIT((SCC->SR1), BIT0)		
#define __SCC_ECNTOVERClr()		SET_REG_BIT((SCC->SR1), BIT0)


//SCC IER Rsg
#define __SCC_GetIER()     		GET_REG_VAL(SCC->IER)
#define __SCC_SetIER(x)    		SET_REG_VAL((SCC->IER), (x))

#define __SCC_FHFEn()			SET_REG_BIT((SCC->IER), BIT6)		
#define __SCC_FHFDis()			SET_REG_BIT_MSK((SCC->IER), 0, BIT6)			
#define __SCC_FT2RENDEn()		SET_REG_BIT((SCC->IER), BIT5)		
#define __SCC_FT2RENDDis()		SET_REG_BIT_MSK((SCC->IER), 0, BIT5)			
#define __SCC_PEEn()			SET_REG_BIT((SCC->IER), BIT4)		
#define __SCC_PEDis()			SET_REG_BIT_MSK((SCC->IER), 0, BIT4)			
#define __SCC_OVEEn()			SET_REG_BIT((SCC->IER), BIT3)		
#define __SCC_OVEDis()			SET_REG_BIT_MSK((SCC->IER), 0, BIT3)			
#define __SCC_FEREn()			SET_REG_BIT((SCC->IER), BIT2)		
#define __SCC_RBFEn()			SET_REG_BIT((SCC->IER), BIT1)		
#define __SCC_RBFDis()			SET_REG_BIT_MSK((SCC->IER), 0, BIT1)			
#define __SCC_TBEEn()			SET_REG_BIT((SCC->IER), BIT1)		
#define __SCC_TBEDis()			SET_REG_BIT_MSK((SCC->IER), 0, BIT1)			
#define __SCC_FNEEn()			SET_REG_BIT((SCC->IER), BIT0)		
#define __SCC_FNEDis()			SET_REG_BIT_MSK((SCC->IER), 0, BIT0)			
#define __SCC_IER_FNERead()	    GET_REG_BIT((SCC->IER), BIT0)
#define __SCC_SYNCENDEn()		SET_REG_BIT((SCC->IER1), BIT2)		
#define __SCC_SYNCENDDis()		SET_REG_BIT_MSK((SCC->IER1), 0, BIT2)		


//SCC IER1 Rsg
#define __SCC_GetIER1()      	GET_REG_VAL(SCC->IER1)
#define __SCC_SetIER1(x)     	SET_REG_VAL((SCC->IER1), (x))

#define __SCC_PRLEn()			SET_REG_BIT((SCC->IER1), BIT1)		
#define __SCC_ECNTOVEREn()		SET_REG_BIT((SCC->IER1), BIT0)		



//SCC TCR Rsg
#define __SCC_GetTCR()      	GET_REG_VAL(SCC->TCR)
#define __SCC_SetTCR(x)     	SET_REG_VAL((SCC->TCR), (x))

#define __SCC_DmaMdScc2Ram()	do{__SCC_SynEndClr();SET_REG_BIT_MSK((SCC->TCR), 0, BIT7); while(!__SCC_SynEndRead());}while(0)		
#define __SCC_DmaMdRam2Scc()	do{__SCC_SynEndClr();SET_REG_BIT((SCC->TCR), BIT7); while(!__SCC_SynEndRead());}while(0)	
#define __SCC_SetFIP(p)			do{__SCC_SynEndClr();SET_REG_BIT_MSK((SCC->TCR), (p), BIT6); while(!__SCC_SynEndRead());}while(0)
#define __SCC_SetFIPHigh()		do{__SCC_SynEndClr();SET_REG_BIT((SCC->TCR), BIT6); while(!__SCC_SynEndRead());}while(0)
#define __SCC_FIPRead()			GET_REG_BIT((SCC->TCR), BIT6)
#define __SCC_AUTOCONVEn()	 	do{__SCC_SynEndClr();SET_REG_BIT((SCC->TCR), BIT5); while(!__SCC_SynEndRead());}while(0)	
#define __SCC_AUTOCONVDDis()	do{__SCC_SynEndClr();SET_REG_BIT_MSK((SCC->TCR), 0, BIT5);while(!__SCC_SynEndRead());}while(0)		
#define __SCC_PORTRead()		GET_REG_BIT((SCC->TCR), BIT4)
#define __SCC_SetProtocol(p)	do{__SCC_SynEndClr();SET_REG_BIT_MSK((SCC->TCR), (p), BIT4); while(!__SCC_SynEndRead());}while(0)	
#define __SCC_TRSetRecMode()	do{__SCC_SynEndClr();SET_REG_BIT_MSK((SCC->TCR), 0, BIT3); while(!__SCC_SynEndRead());}while(0)		
#define __SCC_TRSetSendMode()	do{__SCC_SynEndClr();SET_REG_BIT((SCC->TCR), BIT3); while(!__SCC_SynEndRead());}while(0)	
#define __SCC_TRRead()			GET_REG_BIT((SCC->TCR), BIT3)
#define __SCC_LCTEn()	    	do{__SCC_SynEndClr();SET_REG_BIT((SCC->TCR), BIT2); while(!__SCC_SynEndRead());}while(0)	
#define __SCC_SSSet()	    	do{__SCC_SynEndClr();SET_REG_BIT((SCC->TCR), BIT1); while(!__SCC_SynEndRead());}while(0)	
#define __SCC_CONVForward()		do{__SCC_SynEndClr();SET_REG_BIT_MSK((SCC->TCR), 0, BIT0); while(!__SCC_SynEndRead());}while(0)		
#define __SCC_CONVReverse()		do{__SCC_SynEndClr();SET_REG_BIT((SCC->TCR), BIT0); while(!__SCC_SynEndRead());}while(0)	


//SCC UCR Rsg
#define __SCC_GetUCR()      	GET_REG_VAL(SCC->UCR)
#define __SCC_SetUCR(x)     	SET_REG_VAL((SCC->UCR), (x))

#define __SCC_RSTINSetL()	    {__SCC_SynEndClr();SET_REG_BIT_MSK((SCC->UCR), 0, BIT2); while(!__SCC_SynEndRead());}	
#define __SCC_RSTINSetH()	    do{__SCC_SynEndClr();SET_REG_BIT((SCC->UCR), BIT2); while(!__SCC_SynEndRead());}while(0)	
#define __SCC_RIUSetSynRst()	do{__SCC_SynEndClr();SET_REG_BIT((SCC->UCR), BIT1); while(!__SCC_SynEndRead());}while(0)	
#define __SCC_ClkStop()			do{__SCC_SynEndClr();SET_REG_BIT((SCC->UCR), BIT0); while(!__SCC_SynEndRead());}while(0)	
#define __SCC_ClkStart()		do{__SCC_SynEndClr();SET_REG_BIT_MSK((SCC->UCR), 0, BIT0); while(!__SCC_SynEndRead());}while(0)	


//SCC UCR1 Rsg
#define __SCC_GetUCR1()      		GET_REG_VAL(SCC->UCR1)

#define __SCC_EnLastBSetAuto()		SET_REG_BIT((SCC->UCR1), BIT5)		
#define __SCC_EnLastBSetNoAuto()	SET_REG_BIT_MSK((SCC->UCR1), 0, BIT5)		
#define __SCC_SetClkLevel(x)      	SET_REG_BIT_MSK((SCC->UCR1), (x), BIT4)
#define __SCC_CLKIOVSetL()	    	do{__SCC_SynEndClr();SET_REG_BIT_MSK((SCC->UCR1), 0, BIT4); while(!__SCC_SynEndRead());}while(0)
#define __SCC_CLKIOVSetH()	    	do{__SCC_SynEndClr();SET_REG_BIT((SCC->UCR1), BIT4); while(!__SCC_SynEndRead());}while(0)
#define __SCC_T1PAREn()				SET_REG_BIT((SCC->UCR1), BIT3)
#define __SCC_T1PARDis()			SET_REG_BIT_MSK((SCC->UCR1), 0, BIT3)
#define __SCC_STSPDis()				SET_REG_BIT_MSK((SCC->UCR1), 0, BIT2)
#define __SCC_PRRead()				GET_REG_BIT((SCC->UCR1), BIT0)
#define __SCC_PRSet(x)       		SET_REG_BIT((SCC->UCR1), (x), BIT0)

typedef struct
{
    uint8_t sr0;
    uint8_t sr1;
    uint8_t sr00;
    uint8_t sr11;

    uint8_t fhf;				
    uint8_t ft2rend;			
    uint8_t pe;				
    uint8_t ovr;				
    uint8_t fer;				
    uint8_t tbe;				
    uint8_t rbf;				
    uint8_t fne;				
	
    uint8_t syncend;			
    uint8_t prl;				
    uint8_t ecntover;		
	
}SCCSR_TypeDef;	

SCCSR_TypeDef g_stSccSR;	//SCC状态寄存器位状态结构体
STRUCT_7816_DATA g_stSccData;

static uint16_t ___F_Integer[16][2] =
{
    // FI   { F, f(max)}
    /*0000*/{ 372,  4},
    /*0001*/{ 372,  5},
    /*0010*/{ 558,  6},
    /*0011*/{ 744,  8},
    /*0100*/{1116, 12},
    /*0101*/{1488, 16},
    /*0110*/{1860, 20},
    /*0111*/{   0,  0},
    /*1000*/{   0,  0},
    /*1001*/{ 512,  5},
    /*1010*/{ 768,  7},  //7.5
    /*1011*/{1024, 10},
    /*1100*/{1536, 15},
    /*1101*/{2048, 20},
    /*1110*/{   0,  0},
    /*1111*/{   0,  0}
};
static uint8_t ___D_Integer[16] =
{
    //DI  0000 0001 0010 0011 0100 0101 0110 0111
    /*D*/    0,   1,   2,   4,   8,  16,  32,  64,
    //DI  1000 1001 1010 1011 1100 1101 1110 1111
    /*D*/   12,  20,   0,   0,   0,   0,   0,   0
};
#if 0
static uint32_t dma_irq;
static uint32_t dma_intr_flg = 0;
static DMA_INTR_HANDLE dma_intr_map[8] =
{
	{0, NULL, NULL, NULL, NULL},
	{0, NULL, NULL, NULL, NULL},
	{0, NULL, NULL, NULL, NULL},
	{0, NULL, NULL, NULL, NULL},
	{0, NULL, NULL, NULL, NULL},
	{0, NULL, NULL, NULL, NULL},
	{0, NULL, NULL, NULL, NULL},
	{0, NULL, NULL, NULL, NULL}
};
#endif


int32_t SCC_SetBpr(uint8_t fidi)
{
    //int32_t re=1;
    uint16_t etu;
    etu = ((___F_Integer[FI(fidi)][0] != 0) && (___D_Integer[DI(fidi)] != 0)) ? \
          ___F_Integer[FI(fidi)][0] / ___D_Integer[DI(fidi)] : \
          ___F_Integer[FI(FI_DI_DEFAULT)][0] / ___D_Integer[DI(FI_DI_DEFAULT)];
    
    __SCC_SetBPRL(etu & 0xFF);
    __SCC_SetBPRH_Clr();
    __SCC_SetBPRH((etu >> 8) & 0x0f);	//(*(SCCTypeDef.BPRH))_MSK
    //SCC0->BPRH = SCC0->BPRH & (~SCC0_BPRH_MSK) | ((etu >> 8) & SCC0_BPRH_MSK);
    return SCC_SUCCESS;
}

int32_t SCC_SetParity(SccParityType parity)
{
    //int32_t re;
    switch(parity)
    {
        case SCC_PARITY_EVEN:{
            if(__SCC_PORTRead())
             {
                __SCC_T1PAREn();
             }
            __SCC_SetFIP(0 << 6); 
            break;
        }        
        case SCC_PARITY_ODD:{
            if(__SCC_PORTRead())
             {
                __SCC_T1PAREn();
             }
            __SCC_SetFIP(1 << 6); 
            break;
        }
        case SCC_PARITY_NONE:
        default:{
            __SCC_T1PARDis();

        }
    }
    return SCC_SUCCESS;
}

int32_t SCC_Deactivate(void)
{
    //int32_t re = 1;
    __SCC_RSTINSetL();	
//    __SCC_CLKIOVSetL();
	__SCC_SetClkLevel(1);
	__SCC_ClkStop();	
	
    //关闭IOM 
    SET_REG_BIT_MSK(IOM->GPIOIEC, 0X00, BIT7|BIT6);//GPIO22和GPIO23关闭输入使能
    SET_REG_BIT_MSK(IOM->GPIOOEC, 0X00, BIT6);//GPIO22和GPIO23关闭输出使能		
        
    SET_REG_BIT_MSK(IOM->GPIOPDC, 0X00, BIT7|BIT6);//GPIO22和GPIO23关闭下拉使能
    SET_REG_BIT_MSK(IOM->GPIOPUC, 0X00, BIT7|BIT6);//GPIO22和GPIO23关闭上拉使能

    IOM_Gpio22ConfigMode(0x00);
	IOM_Gpio23ConfigMode(0);
  
	SET_REG_BIT_MSK(IOM->GPIOOEA, 0, BIT5|BIT2|BIT1|BIT0);//关闭GPIO5输出使能
    SET_REG_BIT_MSK(IOM->GPIOIEA, 0, BIT5|BIT2|BIT1|BIT0);//关闭GPIO5输入使能
    
    SET_REG_BIT_MSK(IOM->GPIOPDA, 0x00, BIT5);////关闭GPIO5下拉使能
    SET_REG_BIT_MSK(IOM->GPIOPUA, 0x00, BIT5);////关闭GPIO5上拉使能    
	
    IOM_Gpio5ConfigMode(0);
    
        
    return SCC_SUCCESS;
}

int32_t SCC_Activate(void)
{
    uint16_t etu;
	//int32_t re;

    etu = (uint16_t)__SCC_GetBPRH() << 8;
    etu = etu | (uint16_t)__SCC_GetBPRL();
	
    etu = (SCC_RST_LOW_TIME / etu) + 1;
    //printmsg(">Rst Low:%detu\r\n", etu);
    
	__SCC_ClkStart();	
    __SCC_EtuSetStart(etu);    
    while(!__SCC_ECNTOVERRead());	       			
    __SCC_ECNTOVERClr();
    __SCC_RSTINSetH();	
    
    return SCC_SUCCESS;			
}


/**
 * @brief SCC interrupt service function
 * @param none
 * @return none
 */
static void SccIrqService(void)
{
    uint8_t sr0 = 0, sr1 = 0;
    sr0 = (__SCC_GetSR() & __SCC_GetIER());		
    __SCC_SetSR(sr0);							
	sr1 = (__SCC_GetIER1() & __SCC_GetSR1());	
    __SCC_SetSR1(sr1);							
	
	
#ifdef _TEST_SCC_
    g_stSccSR.sr00 = __SCC_GetSR();				
    g_stSccSR.sr11 = __SCC_GetSR1();			
    g_stSccSR.sr0 = sr0;
    g_stSccSR.sr1 = sr1;
#endif
	
	/*1.处理：SCC中断接收数据*/
	if(SCC_RBF_MSK & sr0)
    {
        uint8_t val = __SCC_TRRead();
        if(val != 0x01)		
        {
            g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
            g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
            g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
            g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
            g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
            g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
            g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
            g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
            #ifdef _TEST_SCC_
            g_stSccSR.rbf++;
            #endif
        }
        else				
        {
			//printmsg("Scc Send FIFO empty!!!\r\n");
            __SCC_TBEDis();		
            #ifdef _TEST_SCC_
            g_stSccSR.tbe++;
            #endif
        }
    }
    else if(SCC_FHF_MSK & sr0)	
    {
        g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
        g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
        g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
        g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
        #ifdef _TEST_SCC_
        g_stSccSR.fhf++;
        #endif
    }
    else if(SCC_FNE_MSK & sr0)	
    {
        do
        {
            g_stSccData.buf[g_stSccData.wp++] = __SCC_GetDR();
        }
        while(__SCC_FNERead());	
        #ifdef _TEST_SCC_
        g_stSccSR.fne++;
        #endif
    }
	
	
	/*2.处理：SCC快传转接收完成中断*/
    if(SCC_FT2REND_MSK & sr0)
    {
		//printmsg("Scc Fast_send To Receive Completed!!!\r\n");
        #ifdef _TEST_SCC_
        g_stSccSR.ft2rend++;
        #endif
    }
	
	/*3.处理：SCC帧错误中断*/
    if(SCC_FER_MSK & sr0)
    {
		//printmsg("Scc Frame Error!!!\r\n");
        #ifdef _TEST_SCC_
        g_stSccSR.fer++;
        #endif
    }
	
	/*4.处理：SCC接收FIFO溢出错误中断*/
    if(SCC_OVR_MSK & sr0)
    {
		//printmsg("Scc Receive_Fifo Overflow Error!!!\r\n");
#ifdef _TEST_SCC_
        g_stSccSR.ovr++;
#endif
    }
	
	/*5.处理：SCC奇偶校验错误中断*/
    if(SCC_PE_MSK & sr0)
    {
		//printmsg("Scc Parity Check Error!!!\r\n");
#ifdef _TEST_SCC_
        g_stSccSR.pe++;
#endif
    }
	
	/*6.处理：SCC插拔卡事件中断*/
    if(SCC_PRL_MSK & sr1)
    {
        if(__SCC_PRRead())	//1.插卡/拔卡
        {
#ifdef _TEST_SCC_
            g_stSccSR.prl = (g_stSccSR.prl + 1) & 0x0F;			//g_stSccSR.prl字节低4bit为1表示：插卡/拔卡
#endif
        }
        else				//2.拔卡/插卡
        {
#ifdef _TEST_SCC_
            g_stSccSR.prl = (g_stSccSR.prl + 0x10) & 0xF0;		//g_stSccSR.prl字节高4bit为1表示：拔卡/插卡
#endif
        }
    }
	
	/*7.处理：SCC模块ETU计数器溢出中断*/
    if(SCC_ECNTOVER_MSK & sr1)
    {
		//printmsg("Scc ETU Counter Overflow Error!!!\r\n");
#ifdef _TEST_SCC_
        g_stSccSR.ecntover++;
#endif
    }
	
	/*8.处理：SCC模块TCR/UCR/RIU写同步完成中断*/
    if(SCC_SYNCEND_MSK & sr1)
    {
		//printmsg("Scc Write TCR_UCR_RIU Completed!!!\r\n");
#ifdef _TEST_SCC_
        g_stSccSR.syncend++;
#endif
    }
}




// scc中断注册和使能
int32_t SCC_EnableIrq(void)
{
    //int32_t re = 1;
    
    NVIC_DisableIRQ(Scc_Exception_IrqNum);
    
    NVIC_SetVector(Scc_Exception_IrqNum, (uint32_t)SccIrqService);
	NVIC_ClearPendingIRQ(Scc_Exception_IrqNum);
    NVIC_EnableIRQ(Scc_Exception_IrqNum);
    
    return SCC_SUCCESS;
}



// 关闭scc中断模式
int32_t SCC_DisableIrq(void)
{
    //int32_t re = 1;
    
	NVIC_DisableIRQ(Scc_Exception_IrqNum);
    
    return SCC_SUCCESS;
}


 
int32_t SCC_Init(const SCC_Config* config)
{
    //SCC_SetConfig(_config);

    //int32_t re = 1;
	// 1/2.打开使能 选择时钟来源 设置分频系数 等待实现
    //SET_REG_BIT(CGU->MCGR3, BIT7); // enable IOM clock
	CGU_ConfigSccClock(config->div);
    CGU_EnableModuleFunctions(CGUM_IOM_PCLK | CGUM_SCC_CLK | CGUM_SCC_PCLK);

    //针对CGU添加的
    SET_REG_BIT_MSK(CGU->MCGR4, 0X10, BIT4);//RETPMU pclk使能
    SET_REG_BIT(CGU->SCCCLKCR, 0X53);
    SET_REG_BIT(CGU->OSC80MOUTCR, 0X03);
    SET_REG_BIT(CGU->CLKSRCFREQCR, 0X01);
    
    
	// 3.在SCC接收前清空接收FIFO，SCCUCR.RIU=1
	__SCC_SetUCR_RIU(BIT1);
	// 4.清中断状态寄存器SCCISR和SCCISR1
	SCC->SR = 0x00;
	SCC->SR1 = SCC->SR1;
	// 5.配置波特率寄存器SCCBPRL和SCCBPRH
	SCC_SetBpr(FI_DI_DEFAULT);
    
	// 6.将SCC配置成接收模式SCCTCR.TR=0
	__SCC_SetTCR_TR(0);
//	// 7.配置中断使能寄存器SCCIER和SCCIER1，使能相应中断
//	__SCC_SetIntEn();
//	__SCC_SetInt1En();
	// 8.配置其他参数
	if(config->protocol == 0)
	{
		__SCC_SetProtocol(PROTOCOL_T0 << 4);									
	}
	else
	{
		__SCC_SetProtocol(PROTOCOL_T1 << 4);									
	}
											
	SCC_SetCONV(config->conv);							
	SCC_SetParity(config->parity);	
	__SCC_SetGt(config->gt);													
	
	//printmsg(">%s\r\n", __SCC_PORTRead() ? "T1" : "T0");
    //printmsg(">gt:%d\r\n", __SCC_GetGTR());
    //printmsg(">bpr:%02X%02X\r\n", __SCC_GetBPRH(), __SCC_GetBPRL());
    
	__SCC_SetClkLevel(config->clklevel);							
//    __SCC_ClkStop();															
    __SCC_RIUSetSynRst();								
    SCC->IER = 0x00;								
	__SCC_OVEEn();										
    __SCC_FEREn();										
    __SCC_PEEn();										
    __SCC_PRLEn();										
	
    if(config->rbfIrq)	
    {
        __SCC_FNEEn();			
        __SCC_FHFEn();			
        g_stSccData.rp = 0;
        g_stSccData.wp = 0;
		SCC_EnableIrq();
        //printmsg("Scc Irq Receive!\r\n");
    }
	
    if(config->tbeIrq)	
    {
        __SCC_TBEEn();				
		SCC_EnableIrq();
        //printmsg("Scc Irq Send!\r\n");
    }
	
	__SCC_ClkStop();		//Scc停止输出给Scd时钟使能
	__SCC_RSTINSetL();		//RST_IO输出低电平

    
    //IOM使能开启	    
    SET_REG_BIT_MSK(IOM->GPIOIEC, 0XC0, BIT7|BIT6);//GPIO22和GPIO23打开输入使能
    SET_REG_BIT_MSK(IOM->GPIOOEC, 0XC0, BIT7|BIT6);//GPIO22和GPIO23打开输出使能		
        
    SET_REG_BIT_MSK(IOM->GPIOPDC, 0X00, BIT7|BIT6);//GPIO22和GPIO23关闭下拉使能
    SET_REG_BIT_MSK(IOM->GPIOPUC, 0XC0, BIT7|BIT6);//GPIO22和GPIO23开启上拉使能

    
    SET_REG_BIT_MSK(IOM->CR12, 0x13, (BIT5|BIT4|BIT3|BIT2|BIT1|BIT0));//GPIO22和GPIO23开启使能
  
	SET_REG_BIT_MSK(IOM->GPIOOEA, 0xFF, BIT5|BIT2|BIT1|BIT0);//打开GPIO5输出使能
    SET_REG_BIT_MSK(IOM->GPIOIEA, 0xFF, BIT5|BIT2|BIT1|BIT0);//打开GPIO5输入使能
    
    SET_REG_BIT_MSK(IOM->GPIOPDA, 0x00, BIT5);////关闭GPIO5下拉使能
    SET_REG_BIT_MSK(IOM->GPIOPUA, 0x20, BIT5);////开启GPIO5上拉使能    
	
    //IOM_Gpio5ConfigMode(BIT3|BIT4);
    SET_REG_BIT_MSK(IOM->CR3,0x18, (BIT5 | BIT4 | BIT3));//GPIO5开启使能
    
    return SCC_SUCCESS;
}





int32_t SCC_ReceiveData(uint8_t *data, uint32_t len)
{
	//uint32_t chCfg;
    //int32_t ret = true;
    //uint8_t back;
    __SCC_TRSetRecMode();	

    {
        while(len)
        {
            //printmsg("Inquiry Mode Read!\r\n");
            while((!__SCC_FNERead()) && (!__SCC_ECNTOVERRead()));
            if(__SCC_ECNTOVERRead())
            {
                //printmsg("Rx overtime!\r\n");
                __SCC_ECNTOVERClr();	
                //ret = false;
                return SCC_PARAM_INCORRECT;
            }
            else if(__SCC_FNERead())
            {
                *data++ = __SCC_GetDR();
                len--;
            }
            else if(__SCC_FHFRead())
            {
                *data++ = __SCC_GetDR();
                *data++ = __SCC_GetDR();
                *data++ = __SCC_GetDR();
                *data++ = __SCC_GetDR();
                len -= 4;
            }
            else if(__SCC_RBFRead())		
            {
                *data++ = __SCC_GetDR();	
                *data++ = __SCC_GetDR();	
                *data++ = __SCC_GetDR();
                *data++ = __SCC_GetDR();
                *data++ = __SCC_GetDR();
                *data++ = __SCC_GetDR();
                *data++ = __SCC_GetDR();
                *data++ = __SCC_GetDR();
                len -= 8;
            }                
        }
    }
    return SCC_SUCCESS;
}

int32_t SCC_SendData(const uint8_t *data, uint32_t len)
{
    //int32_t re = 1;
	//uint32_t chCfg;
    //uint8_t back;
    if(len)
    {
        __SCC_TRSetSendMode();
        while(1 != len--)
        {
            __SCC_WriteByte(*data++);
            while(!__SCC_TBERead());		
        }
        __SCC_LCTEn();						
        __SCC_WriteByte(*data++);
        while(!__SCC_FT2RENDRead());		
        //printmsg("Inquery Mode Send!\r\n");	
        
        __SCC_TRSetRecMode();					
    }
    return SCC_SUCCESS;
}





int32_t SCC_SetCONV(uint8_t fmt)
{
    //int32_t re=1;
    switch(fmt)
    {
    case 2:
        __SCC_AUTOCONVEn();		
        __SCC_CONVReverse();	
        //printmsg(">3F\r\n");
        break;
    case 1:
        __SCC_AUTOCONVEn();		
        __SCC_CONVForward();	
        //printmsg(">3B\r\n");
        break;
    case 0:
    default:
        __SCC_AUTOCONVDDis();	
        __SCC_SSSet();			
        //printmsg(">AUTO\r\n");
        break;
    }
    return SCC_SUCCESS;
}



int32_t SCC_SetRstIOAndResetCard(bool isActivate)
{
    if(isActivate)
    {
        SCC_Activate();
    }else{
        SCC_Deactivate();
    }
    return SCC_SUCCESS;
}

int32_t SCC_SetIrq(bool isEnable)
{
    if(isEnable)
    {
        SCC_EnableIrq();
    }else{
        SCC_DisableIrq();
    }
    
    return SCC_SUCCESS;
}

