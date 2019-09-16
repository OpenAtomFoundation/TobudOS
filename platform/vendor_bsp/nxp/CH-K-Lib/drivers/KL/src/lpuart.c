#include <stdio.h>

#include "common.h"
#include "lpuart.h"
#include "gpio.h"


#if (!defined(LPUART_BASES))
#ifdef      LPUART0
#define     LPUART_BASES    {LPUART0}
#elif       LPUART1
#define     LPUART_BASES    {LPUART0, LPUART1}
#endif
#endif

LPUART_Type * const LPUART_IPTbl[] = LPUART_BASES;

static const Reg_t LPUARTCLKTbl[] =
{
#ifdef LPUART0
    {(void*)&(SIM->SCGC5), SIM_SCGC5_LPUART0_MASK, SIM_SCGC5_LPUART0_SHIFT},
#endif
#ifdef LPUART1
    {(void*)&(SIM->SCGC5), SIM_SCGC5_LPUART1_MASK, SIM_SCGC5_LPUART1_SHIFT}, 
#endif
};

static const IRQn_Type LPUART_IrqTbl[] = 
{
    (IRQn_Type)(LPUART0_IRQn+0),
    (IRQn_Type)(LPUART1_IRQn+1),
};


/* special use for printf */
static uint8_t UART_DebugInstance;

uint32_t LPUART_QuickInit(uint32_t MAP, uint32_t baudrate)
{
    uint8_t i;
    LPUART_InitTypeDef Init;
    map_t * map = (map_t*)&(MAP);
    Init.baudrate = baudrate;
    Init.instance = map->ip;
    Init.parityMode = kLPUART_ParityDisabled;
    Init.bitPerChar = kLPUART_8BitsPerChar;
    Init.srcClock = 48*1000*1000;
    
    /* clock config use IRC48M */
    switch(map->ip)
    {
        case HW_LPUART0:
            SIM->SOPT2 &= ~SIM_SOPT2_LPUART0SRC_MASK;
            SIM->SOPT2 |= SIM_SOPT2_LPUART0SRC(1);
            break;
        case HW_LPUART1:
            SIM->SOPT2 &= ~SIM_SOPT2_LPUART1SRC_MASK;
            SIM->SOPT2 |= SIM_SOPT2_LPUART1SRC(1);
            break; 
    }
    
    for(i = 0; i < map->pin_count; i++)
    {
        SetPinMux(map->io, map->pin_start + i, map->mux); 
    }
    
    LPUART_Init(&Init);
    
    return map->ip;
}

void LPUART_Init(LPUART_InitTypeDef* UART_InitStruct)
{
    uint16_t sbr, sbrTemp, i;
    uint32_t osr, tempDiff, calculatedBaud, baudDiff;
    static bool is_fitst_init = true;
    LPUART_Type *LPUARTx;
    
    LPUARTx = LPUART_IPTbl[UART_InitStruct->instance];
    
    /* enable clock gate */
    *((uint32_t*) LPUARTCLKTbl[UART_InitStruct->instance].addr) |= LPUARTCLKTbl[UART_InitStruct->instance].mask;

    /* disable Tx Rx first */
    LPUARTx->CTRL &= ~(LPUART_CTRL_RE_MASK | LPUART_CTRL_TE_MASK);
    
    osr = 4;
    sbr = (UART_InitStruct->srcClock/(UART_InitStruct->baudrate * osr));
    calculatedBaud = (UART_InitStruct->srcClock / (osr * sbr));
    if (calculatedBaud > UART_InitStruct->baudrate)
    {
        baudDiff = calculatedBaud - UART_InitStruct->baudrate;
    }
    else
    {
        baudDiff = UART_InitStruct->baudrate - calculatedBaud;
    }
    for (i = 5; i <= 32; i++)
    {
        /* calculate the temporary sbr value   */
        sbrTemp = (UART_InitStruct->srcClock/(UART_InitStruct->baudrate * i));
        /* calculate the baud rate based on the temporary osr and sbr values */
        calculatedBaud = (UART_InitStruct->srcClock / (i * sbrTemp));

        if (calculatedBaud > UART_InitStruct->baudrate)
        {
            tempDiff = calculatedBaud - UART_InitStruct->baudrate;
        }
        else
        {
            tempDiff = UART_InitStruct->baudrate - calculatedBaud;
        }

        if (tempDiff <= baudDiff)
        {
            baudDiff = tempDiff;
            osr = i;  /* update and store the best osr value calculated */
            sbr = sbrTemp;  /* update store the best sbr value calculated */
        }
    }

    LPUARTx->BAUD &= ~LPUART_BAUD_SBR_MASK;
    LPUARTx->BAUD &= ~LPUART_BAUD_OSR_MASK;
    LPUARTx->BAUD |= LPUART_BAUD_SBR(sbr) | LPUART_BAUD_OSR(osr-1);
    
    /* parity */
    switch(UART_InitStruct->parityMode)
    {
        case kLPUART_ParityDisabled:
            LPUARTx->CTRL &= ~LPUART_CTRL_PE_MASK;
            break;
        case kLPUART_ParityEven:
            LPUARTx->CTRL |= LPUART_CTRL_PE_MASK;
            LPUARTx->CTRL &= ~LPUART_CTRL_PT_MASK;
            break;
        case kLPUART_ParityOdd:
            LPUARTx->CTRL |= LPUART_CTRL_PE_MASK;
            LPUARTx->CTRL |= LPUART_CTRL_PT_MASK;
            break;
        default:
            break;
    }
   
    /* enable Tx Rx */
    LPUARTx->CTRL |= (LPUART_CTRL_RE_MASK | LPUART_CTRL_TE_MASK);

    /* link debug instance */
    /* if it's first initalized ,link getc and putc to it */
    if(is_fitst_init)
    {
        UART_DebugInstance = UART_InitStruct->instance;
    }
    is_fitst_init = false;
}

void LPUART_WriteByte(uint32_t instance, char ch)
{
    while(!(LPUART_IPTbl[instance]->STAT & LPUART_STAT_TDRE_MASK));
    LPUART_IPTbl[instance]->DATA = (ch & 0xFF);
}

uint8_t LPUART_ReadByte(uint32_t instance, uint8_t *ch)
{    
    if(LPUART_IPTbl[instance]->STAT & LPUART_STAT_RDRF_MASK)
    {
        *ch = (uint8_t)(LPUART_IPTbl[instance]->DATA);	
        return 0; 		  
    }
    return 1;
}


/**
 * @brief  配置UART模块的中断或DMA属性
 * @code
 *      //配置UART0模块开启接收中断功能
 *      UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
 * @endcode
 * @param  instance      :芯片串口端口
 *         @arg HW_UART0 :芯片的UART0端口
 *         @arg HW_UART1 :芯片的UART1端口
 *         @arg HW_UART2 :芯片的UART2端口
 *         @arg HW_UART3 :芯片的UART3端口
 *         @arg HW_UART4 :芯片的UART4端口
 *         @arg HW_UART5 :芯片的UART5端口
 * @param  status      :开关
 * @param  config: 工作模式选择
 *         @arg kUART_IT_Tx:
 *         @arg kUART_DMA_Tx:
 *         @arg kUART_IT_Rx:
 *         @arg kUART_DMA_Rx:
 * @retval None
 */
void LPUART_ITDMAConfig(uint32_t instance, LPUART_ITDMAConfig_Type config, bool status)
{
    LPUART_Type *LPUARTx;
    /* enable clock gate */
    *((uint32_t*) LPUARTCLKTbl[instance].addr) |= LPUARTCLKTbl[instance].mask;

    LPUARTx = LPUART_IPTbl[instance];
    
    switch(config)
    {
        case kUART_IT_Tx:
            (status)?
            (LPUARTx->CTRL |= LPUART_CTRL_TIE_MASK):
            (LPUARTx->CTRL &= ~LPUART_CTRL_TIE_MASK);
            NVIC_EnableIRQ(LPUART_IrqTbl[instance]);
            break; 
        case kUART_IT_Rx:
            (status)?
            (LPUARTx->CTRL |= LPUART_CTRL_RIE_MASK):
            (LPUARTx->CTRL &= ~LPUART_CTRL_RIE_MASK);
            NVIC_EnableIRQ(LPUART_IrqTbl[instance]);
            break;
        case kUART_DMA_Tx:
            (status)?
            (LPUARTx->BAUD |= LPUART_BAUD_TDMAE_MASK):
            (LPUARTx->BAUD &= ~LPUART_BAUD_TDMAE_MASK);
            break;
        case kUART_DMA_Rx:
            (status)?
            (LPUARTx->BAUD |= LPUART_BAUD_RDMAE_MASK):
            (LPUARTx->BAUD &= ~LPUART_BAUD_RDMAE_MASK);
            break;
        default:
            break;
    }
    
}

#ifdef __CC_ARM // MDK Support
struct __FILE 
{ 
	int handle;
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;
FILE __stdin;
__weak int fputc(int ch,FILE *f)
{
	LPUART_WriteByte(UART_DebugInstance, ch);
	return ch;
}

__weak int fgetc(FILE *f)
{
    uint8_t ch;
    while(LPUART_ReadByte(UART_DebugInstance, &ch));
    return (ch & 0xFF);
}

#ifdef __cplusplus
}
#endif


#elif __ICCARM__ /* IAR support */
#include <yfuns.h>
__weak size_t __write(int handle, const unsigned char * buffer, size_t size)
{
    size_t nChars = 0;
    if (buffer == 0)
    {
        /* This means that we should flush internal buffers.  Since we*/
        /* don't we just return.  (Remember, "handle" == -1 means that all*/
        /* handles should be flushed.)*/
        return 0;
    }
    /* This function only writes to "standard out" and "standard err",*/
    /* for all other file handles it returns failure.*/
    if ((handle != _LLIO_STDOUT) && (handle != _LLIO_STDERR))
    {
        return _LLIO_ERROR;
    }
    /* Send data.*/
    while (size--)
    {
        LPUART_WriteByte(UART_DebugInstance, *buffer++);
        ++nChars;
    }
    return nChars;
}

__weak size_t __read(int handle, unsigned char * buffer, size_t size)
{
    size_t nChars = 0;
    uint8_t ch = 0;
    if (buffer == 0)
    {
        /* This means that we should flush internal buffers.  Since we*/
        /* don't we just return.  (Remember, "handle" == -1 means that all*/
        /* handles should be flushed.)*/
        return 0;
    }
    /* This function only writes to "standard out" and "standard err",*/
    /* for all other file handles it returns failure.*/
    if ((handle != _LLIO_STDIN) && (handle != _LLIO_STDERR))
    {
        return _LLIO_ERROR;
    }
    /* read data.*/
    while (size--)
    {
        while(LPUART_ReadByte(UART_DebugInstance, &ch));
        *buffer++ = (char)ch & 0xFF;
        ++nChars;
    }
    return nChars;
}
#endif

static void UART_putstr(uint32_t instance, const char *str)
{
    while(*str != '\0')
    {
        LPUART_WriteByte(instance, *str++);
    }
}

static void printn(unsigned int n, unsigned int b)
{
    static char *ntab = "0123456789ABCDEF";
    unsigned int a, m;
    if (n / b)
    {
        a = n / b;
        printn(a, b);  
    }
    m = n % b;
    LPUART_WriteByte(UART_DebugInstance, ntab[m]);
}

int UART_printf(const char *fmt, ...)
{
    char c;
    unsigned int *adx = (unsigned int*)(void*)&fmt + 1;
_loop:
    while((c = *fmt++) != '%')
    {
        if (c == '\0') return 0;
        LPUART_WriteByte(UART_DebugInstance, c);
    }
    c = *fmt++;
    if (c == 'd' || c == 'l')
    {
        printn(*adx, 10);
    }
    if (c == 'o' || c == 'x')
    {
        printn(*adx, c=='o'? 8:16 );
    }
    if (c == 's')
    {
        UART_putstr(UART_DebugInstance, (char*)*adx);
    }
    adx++;
    goto _loop;
}

/*
static  QuickInit_Type UART_QuickInitTable[] =
{
    { 0, 4, 4, 20, 2, 0, 0}, //LPUART0_RX_E21_TX_E20  4
    { 0, 1, 3, 16, 2, 0, 0}, //LPUART0_RX_B16_TX_B17  3
    { 0, 0, 2,  1, 2, 0, 0}, //LPUART0_RX_A01_TX_A02  2
    { 1, 0, 3, 18, 2, 0, 0}, //LPUART1_RX_A18_TX_A19  3
    { 1, 2, 3,  3, 2, 0, 0}, //LPUART1_RX_C03_TX_C04  3
    { 0, 3, 3,  6, 2, 0, 0}, //LPUART0_RX_D06_TX_D07  3
};
*/
