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
 * @file n32g020xx_scd.c
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief 智能卡从端模块接口实现文件。
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */

#include "n32g020xx_scd.h"

#define printmsg(...)

#define FI(TA1) (((TA1)>>4) & 0x0F)
#define DI(TA1) (((TA1)>>0) & 0x0F)

#define FI_DI_DEFAULT   0x11

//#ifdef SCD_DRV_EN


/*用于选择传输模式*/
volatile uint8_t scd_trans_mode = SCD_NORMAL_MODE;
/*测ETU中断专用*/
volatile uint8_t etu_timer_int = 0;
//测试parity专用
volatile uint8_t parity_error_int = 0;

/*仅中断接收时使用*/
static volatile uint8_t scd_int_rx_buf[260];
volatile uint8_t scd_int_rx_r, scd_int_rx_w;

/*按7816协议的方式选择波特率orETU*/
static const uint16_t ___F_Integer[16][2] =
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

const uint8_t ___D_Integer[16] =
{
    //DI  0000 0001 0010 0011 0100 0101 0110 0111
    /*D*/    0,   1,   2,   4,   8,  16,  32,  64,
    //DI  1000 1001 1010 1011 1100 1101 1110 1111
    /*D*/   12,  20,   0,   0,   0,   0,   0,   0
};


/*SCD发送ATR的 示例，正向卡，波特率=0x96,gt =0*/
//uint8_t atr_buf[20] = {0x3b, 0x72, 0x11, 0x00, 0x00, 0x01, 0x02};
//uint8_t atr_bytelen = 7;




uint8_t atr_buf[20] = {0x3b, 0x68, 0x00, 0x00, 0x00, 0x73, 0xc8,0x40, 0x13, 0x00, 0x90, 0x00};
uint8_t atr_bytelen = 12;




//3B6800000073C84013009000
/*SCD发送ATR的 示例，正向卡，波特率=0x18,gt =0*/
//uint8_t atr_buf[20] = {0x3b, 0x56, 0x18, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96};
//uint8_t atr_bytelen = 7;

#define  TS atr_buf[0]
#define  T0 atr_buf[1]
#define  TA1 atr_buf[2]
#define  TB1 atr_buf[3]
#define  TC1 atr_buf[4]



/*默认DMA通道*/
#define DMA_CH	 3


#if 0
static void DmaTcHandle(void)
{
    printmsg( "dma int trans finish\r\n");
}

#endif




int32_t SCD_SetRstWay(uint8_t rstway)
{// GPIO22是CLK GPIO23是SIO GPIO5是RST
	//int32_t re = 1;
#if 1
    if(rstway == SCD_RISING_EDGE_RST)
    {
//        printmsg( "rising-edge rst\r\n");
		//IOMSetGPIOIntPositive(GPIO5, 1);//SCD_RSTPEdge
		
        
		IOM_SCDRST_EnableIntPosEdge();
        
		//__IOM_SetScdRstIntNEn();
        //ScuScdRstIntEnP();
    }
    else if(rstway == SCD_FALLING_EDGE_RST)
    {
////        printmsg( "falling-edge rst\r\n");
		//IOMSetGPIOIntNegative(GPIO5, 1);//SCD_RSTNEdge
		
        
		IOM_SCDRST_EnableIntNegEdge();
        
        
		//__IOM_SetScdRstIntPEn();
//        ScuScdRstIntEnN();
    }
    else
    {
////        printmsg( "global rst\r\n");
//        ScuScdRstGlbEn();
        //printmsg( "SCU->SR0 & SCU_SCDRST_FLAG : %x\r\n",SCU->SR0 & SCU_SCDRST_FLAG);
//        while(!(SCU->SR0 & SCU_SCDRST_FLAG));
        /*wait for a short time when sys clk >10M*/
//		for(uint32_t i = 0; i < 100000 ; i ++) {__NOP();}
//        SCU->SR0 |= SCU_SCDRST_FLAG;
//        while(!(SCU->SCDRSTCSR & SCU_SCDRSTSTAT_FLAG));
//        ScdSendAtr(atr_buf, atr_bytelen);
    }
#endif
	return SCD_SUCCESS;
}

//CARD_ATTRIBUTE *card;

int32_t SCD_SetGardTime(uint8_t gt)
{
	//int32_t re =1;
//    printmsg( "set gt %x\r\n",gt);
    SCD_GTRHSetData((gt >> 4) & 0x0f); 
    //ScdGTRHSetData(gt);    
    SCD_BPRHSetData((SCD_BPRHRead() & 0x0f) | ((gt << 4) & 0xf0));
	return SCD_SUCCESS;
}


int32_t SCD_SetProtocol(uint8_t protocol)
{
	//int32_t re =1;
    if(protocol)
    {
//        printmsg( "protocol T1\r\n");
        SCD_PORT1();
		
    }
    else
    {
//        printmsg( "protocol T0\r\n");
        SCD_PORT0();
    }
	return SCD_SUCCESS;
}


int32_t SCD_SetParity(uint8_t parity)
{
	//int32_t re = 1;
    switch(parity)
    {
    case SCD_PARI_DIS:
//		printmsg( "parity dis\r\n");
        SCD_PARDis();
        break;
    case SCD_PARI_EVEN:
//		printmsg( "parity even\r\n");
        SCD_PAREn();
        SCD_SetParEven();
        break;
    case SCD_PARI_ODD:
//		printmsg( "parity odd\r\n");
        SCD_PAREn();
        SCD_SetParOdd();//奇校验
        break;
    default:
        break;
    }
	return SCD_SUCCESS;
}

int32_t SCD_Init(CARD_ATTRIBUTE *card)
{
    
    //printmsg("SCD_Init\r\n");
    SET_REG_BIT_MSK(CGU->MCGR3, BIT7, BIT7); //打开IOM时钟
	SET_REG_BIT_MSK(CGU->MCGR4, BIT6, BIT6); //打开AFEC时钟
    //SET_REG_BIT_MSK(CGU->MCGR6, BIT0, BIT0); //打开SCD时钟
    
	SET_REG_BIT_MSK(AFEC->OSC80MCR1, BIT0, BIT0);//临时打开AFEC中的80M时钟输出
	SET_REG_BIT_MSK(CGU->OSC80MOUTCR, BIT6, BIT6);//打开外部80M时钟
	SET_REG_BIT_MSK(CGU->CLKSRCFREQCR, BIT0, BIT0);//提升到80M时钟输出
    
    SET_REG_BIT(FLASH->CR, BIT0);//打开FLASH的预取
	SET_REG_BIT(FLASH->CR, BIT1);//	
	SET_REG_BIT_MSK(ICACHE->CSR, BIT0, BIT0);//打开icache
    
	//设置SCD各个管脚的GPIO
	//IOMSetScdEn(0);
    SET_REG_BIT_MSK(IOM->CR15, BIT3|BIT0, (BIT5|BIT4|BIT3|BIT2|BIT1|BIT0));//GPIO28和GPIO29开启使能
    SET_REG_BIT_MSK(IOM->GPIOIED, BIT4|BIT5, BIT4|BIT5);//GPIO28和GPIO29打开输入使能
    SET_REG_BIT_MSK(IOM->GPIOOED, BIT4|BIT5, BIT4|BIT5);//GPIO28和GPIO29打开输出使能	
    

   
	//IOMSetGPIO5Ctrl(IO5_SCD_RST);
    SET_REG_BIT_MSK(IOM->CR3, 0x10, BIT3|BIT4|BIT5);//gpio5 打开使能
	SET_REG_BIT_MSK(IOM->GPIOOEA, BIT5, BIT5);//打开GPIO5输出使能
    SET_REG_BIT_MSK(IOM->GPIOIEA, BIT5, BIT5);//打开GPIO5输入使能
    
    CGU_EnableModuleFunctions(CGUM_SCD_PCLK | CGUM_SCD_CLK);
    	
    //打印IOM 的数据
//    int i;
//    for (i = 0; i < 50; i++)    
//    {
//        printmsg("[addr:%x = %x], ", (IOM_BASE + i * 4), *(volatile unsigned int*)(IOM_BASE + i * 4));
//        if ((i % 4) == 0) {
//            printmsg("\r\n");
//        }
//    }
//    
//    //打印CGU数据
//     for (i = 0; i < 50; i++)    
//    {
//        printmsg("[addr:%x = %x], ", (CGU_BASE + i * 4), *(volatile unsigned int*)(CGU_BASE + i * 4));
//        if ((i % 4) == 0) {
//            printmsg("\r\n");
//        }
//    }
    
	/*模块复位*/
	SCD_SCDRst();//SCD rest

    printmsg("SCD_SCDRst\r\n");
    
    //NVIC_EnableIRQ(Scd_Exception_IrqNum); // 打开IO中断，捕获SCD复位信号进入中断后发送ATR
        
	/*传输模式*/
	// EnableIrq(ISR_SCD_NUM, 1);//1使能
	scd_trans_mode = card->mode;
	if(card->mode == SCD_NORMAL_MODE) //正常传输模式
	{
        printmsg("normal mode\r\n");
		SCD_CoreFNEDis(); //SCDIER1[7] = 0: 禁止接收FIFO非空中断
		SCD_TXEndDis();//SCDIER2[4] = 0:禁止发送完成中断
	}
	else if(card->mode == SCD_INT_MODE)//中断传输模式
	{
		printmsg("int mode\r\n");
	//	SCD_CardFHFUEn();//1:使能接收FIFO半满中断		
	//	SCD_CoreFNEEn();//1:使能接收FIFO非空中断
		// 发送完成中断没有必要
		// SCD_TXEndEn();//1:使能发送完成中断
		//irq register
		SCD_CoreFNEEn();	
	
		NVIC_DisableIRQ((Scd_Exception_IrqNum));
		NVIC_SetVector((Scd_Exception_IrqNum), (uint32_t)SCD_IrqService);	
		NVIC_ClearPendingIRQ(Scd_Exception_IrqNum);
	    NVIC_EnableIRQ((Scd_Exception_IrqNum));
	}
	else if(card->mode == SCD_DMA_MODE)//DMA传输模式
	{
	    printmsg("dma mode\r\n");
		SCD_CoreFNEDis();
		SCD_TXEndDis();
		//
		
		
	}
	/*卡方向*/
    
	if(card->dir == 0)
	{
        //printmsg(" SCD_DISDirect\r\n");
		SCD_DISDirect();//0:正向模式
	}
	else
	{
		SCD_DISInverse();
	}
	SCD_SetBpr(card->fidi);  //波特率	
	SCD_SetRstWay(card->rstway);  //设置复位形式
	/*奇偶校验*/
	SCD_SetParity(card->parity);
	/*协议，T=0 or T=1*/
	SCD_SetProtocol(card->protocol);
	/*保护时间*/
	SCD_SetGardTime(card->guardtime + 1);
	//	while(!__IOM_CheckScdRstIntNStatus() || !__IOM_CheckScdRstIntPStatus());
	//	__IOM_ClearScdRstIntNStatus();

#if 1
	//Memset(atr_buf, 0, 20);
	//此处应该根据实际参数配置ATR
	//初始字符TS
	if(card->dir == 0)//0为正向
	{
		atr_buf[0] = 0x3b;
	}
	else
	{
		atr_buf[0] = 0x3f;
	}

	//格式字符T0
	atr_buf[1] = 0xf0 | 0x8;//四个历史字节

	//接口字符TA1
	atr_buf[2] = card->fidi;

	//接口字符TB1:II/PI1
	atr_buf[3] = 0;

	//接口字符TC1:GT:N
	atr_buf[4] = card->guardtime + 1;


	//接口字符TD1:使用TA2 T
	atr_buf[4] = 0x10 | card->protocol;

	//接口字符TA2 :特定方式
	atr_buf[5] = 0x0 | card->protocol;


	//8个历史字节
	atr_buf[6] = 0x0;
	atr_buf[7] = 0x73;
	atr_buf[8] = 0xc8;
	atr_buf[9] = 0x40;
	atr_buf[10] = 0x13;
	atr_buf[11] = 0x0;
	atr_buf[12] = 0x90;
	atr_buf[13] = 0x00;


	//TCK
	if(card->protocol == 1)
	{	
		uint8_t tem = atr_buf[0];
		for(uint8_t k = 1; k<14; k++)
		{
			tem ^= atr_buf[k];
		}
		atr_buf[14] = tem;
		atr_bytelen = 15;
	}
	else
	{
		atr_bytelen = 14;
	}
   // printmsg("ATR_BUF:%x",*atr_buf);
#endif
	return SCD_SUCCESS;
}

int32_t SCD_Close(void)
{
	//int32_t re =1;
	SCD_SCDRst();
	//CguScdDis();
    //CGU关闭时钟
    CGU_DisableModuleFunctions(CGUM_SCD_CLK|CGUM_SCD_PCLK);
    
	//IOMSetScdDis();
    SET_REG_BIT_MSK(IOM->GPIOIEC, 0X00, BIT7|BIT6);//GPIO22和GPIO23关闭输入使能
    SET_REG_BIT_MSK(IOM->GPIOOEC, 0X00, BIT7|BIT6);//GPIO22和GPIO23关闭输出使能	
    SET_REG_BIT_MSK(IOM->CR12, 0, (BIT3|BIT1));//GPIO22和GPIO23关闭使能

	SET_REG_BIT_MSK(IOM->GPIOOEA, 0x0, BIT5);//关闭GPIO5输出使能
    SET_REG_BIT_MSK(IOM->GPIOIEA, 0x0, BIT5);//关闭GPIO5输入使能
    SET_REG_BIT_MSK(IOM->CR3, 0x0, BIT4);//gpio5 关闭使能
  
 //   ScuVrCard1Dis();
 //   IomScdDis();
 //   ScuScdClkDis();
 //   ScuScdRstIntDis();
 //   ScuScdRstIntPFlagClr();
    //SCD_IER1 = 0;
	//SCD_IER2 = 0;
	SCD_IER1Dis();
	SCD_IER2Dis();
   
    if(scd_trans_mode == SCD_DMA_MODE)
    {
        //暂时不需要DMA
        //CguDmaDis();
    }
	return SCD_SUCCESS;
}

int32_t SCD_DelayEtu(uint32_t num)
{
	//int32_t re = 1;
	SCD_ETIMER_ENDClr();
    SCD_SetETUTimer(num);
    while(!SCD_SENDCRC_ENDRead());
    SCD_ETIMER_ENDClr();
	return SCD_SUCCESS;
}

int32_t SCD_SetETUTimer(uint32_t num)
{
	//int32_t re =1;
	SCD_EtuDis();
	SCD_EECRL_Set(num & 0xFF);
	SCD_EECRH_Set((num >> 8) & 0xFF);
	SCD_EtuEn();
	return SCD_SUCCESS;
}

int32_t SCD_EtuTimerStart(uint32_t num)
{
	//int32_t re =1;
	// 硬件自动计数ETU完成标志位
    SCD_ETIMER_ENDClr();
	// 使能硬件自动计数etu完成状态位中断
	SCD_ETimerEndEn();
    SCD_SetETUTimer(num & 0x3fff);
	return SCD_SUCCESS;
}

int32_t SCD_EtuTimerStop(void)
{
	//int32_t re =1;
	// 禁止硬件自动计数etu完成状态位中断
    SCD_ETimerEndDis();
    SCD_EtuDis();
	return SCD_SUCCESS;
}

int32_t SCD_SendAtr(uint8_t* atr, uint8_t bytelen)
{
	//int32_t re = 1;
    uint16_t i;

    for(i = 0; i < bytelen - 1; i++)
    {
        SCD_SendByte(atr[i]);
    }
	SCD_SendLastByte(*(atr + bytelen - 1));
	return SCD_SUCCESS;
//	SCD_SendLBClr();
}


int32_t SCD_SetBpr(uint8_t fidi)
{
    uint16_t etucycle;
	//int32_t re = 1;
    if((___F_Integer[FI(fidi)][0] != 0) && (___D_Integer[DI(fidi)] != 0))
    {
        etucycle = ___F_Integer[FI(fidi)][0] / ___D_Integer[DI(fidi)];
    }
    else
    {
        etucycle = ___F_Integer[FI(FI_DI_DEFAULT)][0] / ___D_Integer[DI(FI_DI_DEFAULT)];
    }

	// SCDBPRL = (uint8_t)(etucycle & 0xFF);
	SCD_BPRLSetData((uint8_t)(etucycle & 0xFF));
	//SCDBPRH &= (~SCD_BPRH_MASK);
	SCD_BPRHSetData(SCD_BPRHRead() & (~SCD_BPRH_MASK));
	//SCDBPRH |= (uint8_t)((etucycle >> 8) & SCD_BPRH_MASK);
	SCD_BPRHSetData(SCD_BPRHRead() | ((uint8_t)((etucycle >> 8) & SCD_BPRH_MASK)));
	return SCD_SUCCESS;
}


int32_t SCD_SetDefaultCard(CARD_ATTRIBUTE* card)
{
	// Memset(card, 0, sizeof(CARD_ATTRIBUTE));
	//int32_t re = 1;
	card->mode = SCD_NORMAL_MODE; //正常传输模式
	card->dma_ch = 0;
	card->dma_pri = 0;
	card->rstway = SCD_RISING_EDGE_RST;//0：复位信号上升沿触发ScdRst中断
	card->parity = SCD_PARI_EVEN; //scd偶校验
	card->protocol = 0; //T=0
	card->dir = 0;   //正向卡
	card->fidi = 0x11;  //波特率9600
	card->guardtime = 0; //保护时间
	return SCD_SUCCESS;
}


int32_t SCD_SendByte( uint8_t data)
{
	//int32_t re =1;
    /*fast transfer to receive ,disable*/
    SCD_SendLBClr();//0:禁止快发转接收
    /*set tx mode*/
    SCD_TRSTx();//1:选择发送模式
//    printmsg( "send byte ");

	SCD_TDRSetData(data);//SCDTDR = data;
    
    while(!SCD_SynEndRead());//(SCDISR2 & SCD_SYN_END)
    SCD_SynEndClr();//SCDISR2 = SCD_SYN_END;

    //while(!((SCD->ISR1)) & SCD_FIFO_TFEM));//1:发送FIFO为空
    while(!SCD_FIFO_TFEMRead());//1:发送FIFO为空((SCD->ISR1)) & SCD_FIFO_TFEM)

    SCD_TX_ENDClr();//SCDISR2 = SCD_TX_END;//发送已完成 w1c0

     printmsg( "in send byte ,(SCD->ISR1)) %02x\r\n", (SCD->ISR1));
//  printmsg( "done \r\n");

    while(!SCD_IDLERead());//当SCD_为发送模式时，此位为1表示处于发送空闲状态
	return SCD_SUCCESS;
}


int32_t SCD_SendLastByte(uint8_t data)
{
	//int32_t re =1;
    //printmsg( "send last byte ");
    /*set tx mode*/
    SCD_TRSTx();
    /*fast transfer into receive */
    SCD_SendLBSet();

    SCD_SynEndClr();//SCDISR2 = SCD_SYN_END;
    //SCDTDR = data;
	SCD_TDRSetData(data);
    while(!SCD_SynEndRead());//(SCDISR2 & SCD_SYN_END)
    SCD_SynEndClr();//SCDISR2 = SCD_SYN_END;

    while(!SCD_FT2R_ENDRead());//1:快发转接收已完成((SCD->ISR1)) & SCD_FT2R_END)
    SCD_FT2R_ENDClr();//(SCD->ISR1)) =  SCD_FT2R_END;

    while(!SCD_FIFO_TFEMRead());//1:发送FIFO为空((SCD->ISR1)) & SCD_FIFO_TFEM)

    /*等待scd idle*/
    while(!SCD_IDLERead());//(SCDISR2 & SCD_IDLE)


    //printmsg( "done \r\n");
	return SCD_SUCCESS;
}


int32_t SCD_Receive(uint8_t* read, uint8_t len)
{
   // int32_t result = -1;
	if(!len)
    {
        return SCD_PARAM_INCORRECT;
    }
			
    uint32_t status = 0;
    status = status;
    SCD_TRSRx();
    if(scd_trans_mode == SCD_INT_MODE)
    {
       //printmsg( "scd int receive ");
        while(len-- != 0)
        {
            while(scd_int_rx_r == scd_int_rx_w);
           // while(scd_int_rx_w - scd_int_rx_r <= 1);
            *read++ = scd_int_rx_buf[scd_int_rx_r++];


        }
		if (scd_int_rx_r > 240)
		{
			NVIC_DisableIRQ(Scd_Exception_IrqNum);//1使能，0禁止
			printmsg( "%s:%d-----------reset fifo-------------------", __FUNCTION__, __LINE__);
			uint8_t i = 0;
			for(; i<scd_int_rx_w - scd_int_rx_r; i++)
			{
				scd_int_rx_buf[i] = scd_int_rx_buf[scd_int_rx_r + i];
			}

			//SCD_CoreFNEDis();
			//SCD_CardFHFUDis();


			scd_int_rx_w = i;
			scd_int_rx_r = 0;
			NVIC_EnableIRQ(Scd_Exception_IrqNum);//1使能，0禁止
			//SCD_CardFHFUEn();//1:使能接收FIFO半满中断
			//SCD_CoreFNEEn();//1:使能接收FIFO非空中断
		}
		
//        printmsg( "done\r\n");
    }
    else if(scd_trans_mode == SCD_DMA_MODE)
    {
        printmsg( "scd dma read ");
        while(!SCD_FIFO_NERead());//1: FIFO非空((SCD->ISR1)) & SCD_FIFO_NE)
        SCD_DMAScd2Ram();
        SCD_TRSRx();
      #if 0  // 以下为DMA传输
		DmaInit();
		//DmaSetIntEN();
		// 配置DMA传输参数
		chCfg =   DMA_CHCTRL_BIT_08
				| (len << DMA_CHCTRL_SIZE_BITPOS)
				| DMA_CHCTRL_DSTINC_MASK
				| DMA_CHCTRL_FLOW_P2M
				| DMA_CHCTRL_SRCPER_SCD;
		DmaTransfer(DMA_CH, (uint32_t)(SCD->RDR), (uint32_t)read, chCfg);
		while(GetStatus(DMA_CH)==DMA_BUSY);
		CguDmaDis();
     #endif
    }
    else
    {       // printmsg( "scd normal read ");
        while(len != 0)
        {
            /*in case fifo full avoid fifo overflow*/
            if(len >= 8)
            {
                if(SCD_FIFO_FULLRead())//1:FIFO全满 (SCD->ISR1)) & SCD_FIFO_FU
                {
                    *read++ = SCD_RDRRead();//SCDRDR
                    *read++ = SCD_RDRRead();
                    *read++ = SCD_RDRRead();
                    *read++ = SCD_RDRRead();

                    *read++ = SCD_RDRRead();
                    *read++ = SCD_RDRRead();
                    *read++ = SCD_RDRRead();
                    *read++ = SCD_RDRRead();
                    len -= 8;
                }
            }
            /*in case fifo half full avoid fifo overflow*/
            if(len >= 4)
            {
                if(SCD_FIFO_HFURead())//SCD_ISR1 & SCD_FIFO_HFU
                {
                    *read++ = SCD_RDRRead();
                    *read++ = SCD_RDRRead();


                    *read++ = SCD_RDRRead();
                    *read++ = SCD_RDRRead();
                    SCD_FIFO_HFUClr();//SCD_ISR1 = SCD_FIFO_HFU;
                    len -= 4;
                }
            }
            if(len > 0)
            {
                /*receive one byte per time*/
                while(!SCD_FIFO_NERead());//FIFO非空
                *read++ = SCD_RDRRead();
                len--;
            }
        }
    }
//    printmsg( " read SR1 %x,SR2 %x \r\n",SCD_ISR1,SCDISR2);
//	/*等待SCD idle*/
    while(!SCD_IDLERead());//(SCDISR2 & SCD_IDLE)
    //result = 1;
    return SCD_SUCCESS;
    
}


int32_t SCD_Send(uint8_t* write, uint32_t len)
{
//	uint32_t chCfg;
	//int32_t re =1;
    /*for DMA mode*/
//    uint32_t status = 0;
    /*关快发转接收*/
    SCD_SendLBClr();

    if((scd_trans_mode == SCD_NORMAL_MODE) || (scd_trans_mode == SCD_INT_MODE) )
    {
        while(len-- != 0)
        {
            SCD_SendByte(*write ++);
        }
    }
    else
    {
        #if 0// 以下为DMA传输
        //printmsg( "scd dma write\r\n");
        /*set dma direction*/
        SCD_DMARam2Scd();//1:DMA搬移方向为SRAM到SCD
        SCD_TRSTx();//SCD发送/接收模式选择位
		
		DmaInit();
		// DmaSetIntEN();
		// 配置DMA传输参数
		chCfg =   DMA_CHCTRL_BIT_08
				| (len << DMA_CHCTRL_SIZE_BITPOS)
				| DMA_CHCTRL_SRCINC_MASK
				| DMA_CHCTRL_FLOW_M2P
				| DMA_CHCTRL_DSTPER_SCD;
		DmaTransfer(DMA_CH, (uint32_t)write, (uint32_t)(SCD->TDR), chCfg);
		while(GetStatus(DMA_CH)==DMA_BUSY);
		CguDmaDis();
        
        while(!ScdFIFO_TFEMRead());//1:发送FIFO为空(SCD_ISR1 & SCD_FIFO_TFEM)
        #endif
    }

    /*等待SCD idle*/
    //while(!(SCDISR2 & SCD_IDLE));
    while(!SCD_IDLERead());//当Scd为发送模式时，此位为1表示处于发送空闲状态
	return SCD_SUCCESS;
}





//#define SCD_IRQ_DEBUG
int32_t SCD_IrqService(void)
{
	//int32_t re =1;
	//printmsg("%s\r\n",__func__);
	//printmsg("ISR1 = 0x%x  ISR2 = 0x%x\r\n", (*(volatile unsigned int *)(SCD_BASE)), (*(volatile unsigned int *)(SCD_BASE + 0x008)));
	//printmsg("ISE1 = 0x%x  ISE2 = 0x%x\r\n", (*(volatile unsigned int *)(SCD_BASE + 4)), (*(volatile unsigned int *)(SCD_BASE + 0x00c)));	
	//EnableIrq(ISR_SCD_NUM, 0);//1使能，0禁止:核中断关闭时产生的中断放在pending寄存器中？

	if(scd_trans_mode == SCD_INT_MODE)
	{

		//while(SCD_FIFO_NERead());
	
		// printmsg("scd int irq\r\n");
		//if(SCD_FIFO_NERead())//FIFO非空 SCD_ISR1 & SCD_FIFO_NE
		{
			#if 1
			if(SCD_FIFO_FOVRead())//:FIFO溢出 SCD_ISR1 & SCD_FIFO_OV
			{
				SCD_FIFO_FOVClr();//SCD_ISR1 = SCD_FIFO_OV;
				printmsg("RCV FIFO OverFlow\r\n");
			}
			#endif
			
			if(SCD_FIFO_FULLRead())//FIFO全满
			{
				//SCD_FIFO_HFUClr();//SCD_ISR1 = SCD_FIFO_HFU;
				printmsg("RCV FIFO FU INT \r\n");
				for(uint8_t i = 0; i<8; i++)
				{
					while(!SCD_FIFO_NERead());
					scd_int_rx_buf[scd_int_rx_w++] = SCD_RDRRead();
				}
			}
			else if(SCD_FIFO_HFURead())//FIFO半满
			{
				SCD_FIFO_HFUClr();//SCD_ISR1 = SCD_FIFO_HFU;
				printmsg("RCV FIFO HFU INT \r\n");
				for(uint8_t i = 0; i<4; i++)
				{
					while(!SCD_FIFO_NERead());
					scd_int_rx_buf[scd_int_rx_w++] = SCD_RDRRead();
				}
			}
			else if(SCD_FIFO_NERead())//FIFO非空 SCD_ISR1 & SCD_FIFO_NE
			{
				scd_int_rx_buf[scd_int_rx_w++] = SCD_RDRRead();				
			}
		}

		#if 1
		if(SCD_TXEndCheck() && SCD_TX_ENDRead())//发送已完成 SCDISR2 & SCD_TX_END
		{
			SCD_TX_ENDClr();//SCDISR2 = SCD_TX_END;
			printmsg("TX Byte End INT \r\n");
		}
		#endif
		
	}

	#if 1
	if(SCD_TFEMCheck() && SCD_FIFO_TFEMRead())//发送FIFO为空 SCD_ISR1 & SCD_FIFO_TFEM
	{
		SCD_TFEMDis();
		printmsg("TX FIFO EMPTY INT\r\n");
		 
	}
	#endif
	#if 1
	if(SCD_ETimerEndCheck() && SCD_ETIMER_ENDRead())//硬件自动计数ETU完成标志位 已经完成 SCDISR2 & SCD_ETIMER_END
	{
		SCD_ETIMER_ENDClr();
		printmsg("ETU TIMER END INT\r\n");
		//SCD_SendByte(0x60);
		//SCD_EtuTimerStart(950 * 10 * 1);
		//SCDISR2 = SCD_ETIMER_END;
		etu_timer_int = 1;
	}
	#endif
	#if 1
	if(SCD_TRECheck() && SCD_TR_ERRORRead())//有奇偶校验错误 SCD_ISR1 & SCD_TR_PE
	{
		SCD_TR_ERRORClr();
		parity_error_int = 1;
		printmsg("Par Err\r\n");	
	}
	#endif
	#if 1
	if(SCD_FT2REndCheck() && SCD_FT2R_ENDRead())//快发转接收已完成 SCD_ISR1 & SCD_FT2R_END
	{
		//SCD_FT2R_ENDClr();
		SCD_FT2REndDis();//不清中断，关闭中断使能。代码中有别的地方在等标志
		printmsg("FT2R END \r\n");
		//SCD_ISR1 = SCD_FT2R_END;
	}
	#endif
	#if 1
	if(SCD_SendCRCEndCheck() && SCD_SENDCRC_ENDRead())//硬件发送CRC码完成 SCDISR2 & SCD_CRCSEND_END
	{
		SCD_SendCRCEndDis();
		printmsg("CRC SEND END INT\r\n");
	}
	#endif
	#if 1
	if(SCD_CRCByteEndCheck() && SCD_CRCBYTE_ENDRead())//字节CRC码计算完成 SCDISR2 & SCD_CRCBYTE_END
	{
		SCD_CRCBYTE_ENDClr();//SCDISR2 = SCD_CRCBYTE_END;
		printmsg("CRC BYTE END\r\n");
	}
	#endif
	#if 1
	if(SCD_SYNEndCheck() && SCD_SynEndRead())//则信号同步过程已经完成 & SCD_SYN_END
	{
		//SCD_SynEndClr(); 不在这里清，代码里自有地方等待此标志
		SCD_SYNEndDis();
		printmsg("SYN END INT\r\n");
	}
	#endif
	
	//EnableIrq(ISR_SCD_NUM, 1);//1使能，0禁止
	// printmsg("SCD IRQ END\r\n");	
	return SCD_SUCCESS;
}


//#endif /*SCD_DRV_EN*/

/** @} */

