/**
  ******************************************************************************
  * @file    uart.c
  * @author  YANDLD
  * @version V2.6
  * @date    2015.6.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "uart.h"
#include "common.h"


#ifdef MKL26Z4_H_
#undef  UART_BASES
#define UART_BASES  {UART0, UART1, UART2}

static const Reg_t CLKTbl[] =
{
    {(void*)&(SIM->SCGC4), SIM_SCGC4_UART0_MASK, SIM_SCGC4_UART0_SHIFT},
    {(void*)&(SIM->SCGC4), SIM_SCGC4_UART1_MASK, SIM_SCGC4_UART1_SHIFT},
    {(void*)&(SIM->SCGC4), SIM_SCGC4_UART2_MASK, SIM_SCGC4_UART2_SHIFT},
};

static const IRQn_Type UART_IrqTbl[] = 
{
    (IRQn_Type)(UART0_IRQn+0),
    (IRQn_Type)(UART0_IRQn+1),
    (IRQn_Type)(UART0_IRQn+2),
};
#endif

#ifdef MKL27Z4_H_
static const struct Reg_t CLKTbl[] =
{
    {(void*)&(SIM->SCGC4), SIM_SCGC4_UART2_MASK, SIM_SCGC4_UART2_SHIFT},
};

static const IRQn_Type UART_IrqTbl[] = 
{
    UART0_FLEXIO_IRQn,
};
#endif

const void* UART_IPTbl[] = UART_BASES;
static uint8_t UART_DebugInstance;


static int _getc(void)
{
    uint8_t ch;
    while(UART_GetChar(UART_DebugInstance, &ch));
    return ch;
}

static int _putc(uint8_t ch)
{
    UART_PutChar(UART_DebugInstance, ch);
    return ch;
}

static uint32_t _SetClockSrc(uint32_t instance, void *param)
{
    uint32_t clk;
    if(instance == HW_UART0)
    {
        clk = GetClock(kMCGOutClock);
        #ifdef SIM_SOPT2_UART0SRC_MASK
        SIM->SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK; /* use PLL/2 or FLL */
        SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);  
        #endif
        
        #ifdef MCG_C6_PLLS_MASK
        if(MCG->C6 & MCG_C6_PLLS_MASK) /* PLL */
        {
            SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
            clk /= 2;
        }
        else /* FLL */
        {
            SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
        }
        #endif
    }
    else
    {
        clk = GetClock(kBusClock);
    }
    return clk;
}

void UART_SetBaudRate(uint32_t instance, uint32_t baud)
{
    uint32_t sbr, clk, osr, calculatedBaud;
    UART_Type * UARTx = (UART_Type*)UART_IPTbl[instance];
    UARTx->C2 &= ~((UART_C2_TE_MASK)|(UART_C2_RE_MASK));
    
    /* config baudrate */
    clk = _SetClockSrc(instance, NULL);
    
    for(osr=5; osr<32; osr++)
    {
        sbr = clk/(baud * osr);
        calculatedBaud = clk/(osr*sbr);
        if(ABS((int)calculatedBaud - (int)baud) < (baud*0.03))
        {
            break;
        }
    }
//    if ((osr > 3) && (osr < 8))
//    {
//        UART0->C5 |= UART0_C5_BOTHEDGE_MASK;
//    }
    UARTx->BDH &= ~(UART_BDH_SBR_MASK);
    UARTx->BDH |= (sbr>>8) & UART_BDH_SBR_MASK;
    UARTx->BDL = (sbr & UART_BDL_SBR_MASK);
    UART0->C4 = UART0_C4_OSR(osr-1);
    UARTx->C2 |= ((UART_C2_TE_MASK)|(UART_C2_RE_MASK));
}



static void _UART_Init(uint32_t instance, uint32_t srcClock, uint32_t baud)
{
    static bool is_fitst_init = true;
    
    CLK_EN(CLKTbl, instance);
    
    UART_SetBaudRate(instance, baud);
    
    if(is_fitst_init)
    {
        UART_DebugInstance = instance;
        SetConsole(_putc, _getc);
    }
    is_fitst_init = false;
}

uint32_t UART_Init(uint32_t MAP, uint32_t baudrate)
{
    uint8_t i;
    uint32_t clk;
    
    map_t * pq = (map_t*)&(MAP);
    clk = _SetClockSrc(pq->ip, NULL);
    for(i = 0; i < pq->pin_count; i++)
    {
        SetPinMux(pq->io, pq->pin_start + i, pq->mux); 
    }
    _UART_Init(pq->ip, clk, baudrate);
    return pq->ip;
}

uint32_t UART_SetIntMode(uint32_t instance, UART_Int_t mode, bool val)
{
    CLK_EN(CLKTbl, instance);
    UART_Type * UARTx = (UART_Type*)UART_IPTbl[instance];
    NVIC_EnableIRQ(UART_IrqTbl[instance]);
    
    switch(mode)
    {
        case kUART_IntTx:
            (val)?(UARTx->C2 |= UART_C2_TIE_MASK):(UARTx->C2 &= ~UART_C2_TIE_MASK);
            break;
        case kUART_IntRx:
            (val)?(UARTx->C2 |= UART_C2_RIE_MASK):(UARTx->C2 &= ~UART_C2_RIE_MASK);
            break;
        case kUART_IntIdleLine:
            (val)?(UARTx->C2 |= UART_C2_ILIE_MASK):(UARTx->C2 &= ~UART_C2_ILIE_MASK);
        default:
            break;
    }
    return 0;
}

uint32_t UART_SetDMAMode(uint32_t instance, UART_DMA_t mode, bool val)
{
    CLK_EN(CLKTbl, instance);
 //   UART_Type * UARTx = (UART_Type*)UART_IPTbl[instance];
    switch(mode)
    {
        case kUART_DMATx:
            (val)?(UART0->C5 |= UART0_C5_TDMAE_MASK):(UART0->C5 &= ~UART0_C5_TDMAE_MASK);
            break;
        case kUART_DMARx:
            (val)?(UART0->C5 |= UART0_C5_RDMAE_MASK):(UART0->C5 &= ~UART0_C5_RDMAE_MASK);
            break;
    }
    return 0;
}

void UART_PutChar(uint32_t instance, uint8_t ch)
{
    UART_Type * UARTx = (UART_Type*)UART_IPTbl[instance];
    while(!(UARTx->S1 & UART_S1_TDRE_MASK));
    UARTx->D = (uint8_t)ch;
}

uint8_t UART_GetChar(uint32_t instance, uint8_t *ch)
{
    UART_Type * UARTx = (UART_Type*)UART_IPTbl[instance];
    
    if(UARTx->S1 & UART_S1_RDRF_MASK)
    {
        *ch = (uint8_t)(UARTx->D);	
        return 0; 		  
    }
    if(UARTx->S1 & UART_S1_OR_MASK)
    {
        UARTx->S1 |= UART_S1_OR_MASK;
    }
    return 1;
}


#if (CHLIB_DMA_SUPPORT == 1)
#include "dma.h"
static IPDMA_t UART_DMATbl[]=
{
    {(void*)&UART0->D, (void*)&UART0->D, UART0_TRAN_DMAREQ, 0, false},
    {(void*)&UART1->D, (void*)&UART1->D, UART1_TRAN_DMAREQ, 0, false},
    {(void*)&UART2->D, (void*)&UART2->D, UART2_TRAN_DMAREQ, 0, false},
};

uint32_t UART_DMASend(uint32_t instance, uint32_t dmaChl, uint8_t *buf, uint32_t len)
{
    uint32_t chl, remain;
    DMA_Init_t Init;
    UART_Type * UARTx = (UART_Type*)UART_IPTbl[instance];
    
    if(UART_DMATbl[instance].isActive == true)
    {
        chl = UART_DMATbl[instance].dmaChl;
        remain = DMA_GetTransCnt(chl);
        if(remain == 0)
        {
            if(instance == HW_UART0)
                UART0->C5 &= ~UART0_C5_TDMAE_MASK;
            else
                UARTx->C4 &= ~UART_C4_TDMAS_MASK;
            UART_DMATbl[instance].isActive = false; 
        }
        else
        {
            return 1;
        }
    }
    chl = dmaChl;
    if(instance == HW_UART0)
        UART0->C5 |= UART0_C5_TDMAE_MASK;
    else
        UARTx->C4 |= UART_C4_TDMAS_MASK;

    Init.chl = chl;
    Init.chlTrigSrc = UART_DMATbl[instance].trigSrc;
    Init.trigSrcMod = kDMA_TrigSrc_Normal;
    Init.transCnt = len;

    Init.sAddr = (uint32_t)buf;
    Init.sAddrIsInc = true;
    Init.sDataWidth = kDMA_DataWidthBit_8;
    Init.sMod = kDMA_ModuloDisable;

    Init.dAddr = (uint32_t)UART_DMATbl[instance].dAddr;
    Init.dAddrIsInc = false;
    Init.dDataWidth = kDMA_DataWidthBit_8;
    Init.dMod = kDMA_ModuloDisable;
    DMA_Init(&Init);

    DMA_EnableReq(chl);
    
    UART_DMATbl[instance].dmaChl = chl;
    UART_DMATbl[instance].isActive = true;
    
    return 0;
}

uint32_t UART_DMAGetRemain(uint32_t instance)
{
    uint32_t ret, chl;
    UART_Type * UARTx = (UART_Type*)UART_IPTbl[instance];
    
    if(UART_DMATbl[instance].isActive == false)
    {
        return 0;
    }
    chl = UART_DMATbl[instance].dmaChl;
    ret = DMA_GetTransCnt(chl);
    if(ret == 0)
    {
        if(instance == HW_UART0)
            UART0->C5 &= ~UART0_C5_TDMAE_MASK;
        else
            UARTx->C4 &= ~UART_C4_TDMAS_MASK;
        UART_DMATbl[instance].isActive = false;
    }
    return ret;
}

#endif



//void UART0_IRQHandler(void)
//{
//    uint16_t ch;
//    UART_Type * UARTx = (UART_Type*)UART_IPTbl[HW_UART0];
//    /* Tx */
//    if((UARTx->S1 & UART_S1_TDRE_MASK) && (UARTx->C2 & UART_C2_TIE_MASK))
//    {
//        if(UART_CallBackTxTable[HW_UART0])
//        {
//            UART_CallBackTxTable[HW_UART0](&ch);
//        }
//        UARTx->D = (uint8_t)ch;
//    }
//    /* Rx */
//    if((UARTx->S1 & UART_S1_RDRF_MASK) && (UARTx->C2 & UART_C2_RIE_MASK))
//    {
//        ch = (uint8_t)UARTx->D;
//        if(UART_CallBackRxTable[HW_UART0])
//        {
//            UART_CallBackRxTable[HW_UART0](ch);
//        }    
//    }
//}


/*
static const QuickInit_Type UART_QuickInitTable[] =
{
    { 1, 4, 3, 0, 2, 0}, //UART1_RX_PE01_TX_PE00
    { 2, 4, 3,16, 2, 0}, //UART2_RX_PE17_TX_PE16
    { 0, 4, 4,20, 2, 0}, //UART0_RX_PE21_TX_PE20
    { 2, 4, 4,22, 2, 0}, //UART2_RX_PE23_TX_PE22
    { 0, 0, 2, 1, 2, 0}, //UART0_RX_PA01_TX_PA02
    { 0, 0, 3,14, 2, 0}, //UART0_RX_PA15_TX_PA14
    { 1, 0, 3,18, 2, 0}, //UART1_RX_PA18_TX_PA19
    { 0, 1, 3,16, 2, 0}, //UART0_RX_PB16_TX_PB17
    { 2, 3, 3, 2, 2, 0}, //UART2_RX_PD02_TX_PD03
    { 2, 3, 3, 4, 2, 0}, //UART2_RX_PD04_TX_PD05
    { 0, 3, 3, 6, 2, 0}, //UART0_RX_PD06_TX_PD07
};
    for(i=0;i<ARRAY_SIZE(UART_QuickInitTable);i++)
    {
        printf("(0x%08XU)\r\n", QuickInitEncode(&UART_QuickInitTable[i]));
    }
*/
