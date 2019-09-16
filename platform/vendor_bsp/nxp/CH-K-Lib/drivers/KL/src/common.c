/**
  ******************************************************************************
  * @file    common.c
  * @author  YANDLD
  * @version V2.6
  * @date    2015.6.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include <string.h>
#include "common.h"

#ifndef DEFAULT_SYSTEM_CLOCK
#define DEFAULT_SYSTEM_CLOCK    (48000000)
#endif

typedef struct
{
    int (*putc)(uint8_t ch);
    int (*getc)(void);
}Console_t;

volatile static uint32_t fac_us = DEFAULT_SYSTEM_CLOCK/1000000;
volatile static uint32_t fac_ms = DEFAULT_SYSTEM_CLOCK/1000;
volatile static Console_t Console;



static const Reg_t CLKTbl[] =
{ 
    {(void*)&(SIM->CLKDIV1), SIM_CLKDIV1_OUTDIV1_MASK, SIM_CLKDIV1_OUTDIV1_SHIFT},
    {(void*)&(SIM->CLKDIV1), SIM_CLKDIV1_OUTDIV4_MASK, SIM_CLKDIV1_OUTDIV4_SHIFT}, 
    {(void*)&(SIM->CLKDIV1) , SIM_CLKDIV1_OUTDIV4_MASK, SIM_CLKDIV1_OUTDIV4_SHIFT}, 
    {0,0},
};

static const Reg_t PORTCLKTbl[] =
{
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTA_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTB_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTC_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTD_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTE_MASK, SIM_SCGC5_PORTE_SHIFT},
};

static PORT_Type * const PORT_IPTbl[] = PORT_BASES;


 /**
 * @brief  获得系统各个总线时钟的频率
 * @param  clockName:时钟名称
 *         @arg kCoreClock    :内核时钟
 *         @arg kBusClock     :总线时钟
 *         @arg kFlexBusClock :Flexbus总线时钟
 *         @arg kFlashClock   :Flash总线时钟
 * @retval the clock
 */
uint32_t GetClock(Clock_t clock)
{
    uint32_t val;
    
    /* calualte MCGOutClock system_MKxxx.c must not modified */
    val = SystemCoreClock * (REG_GET(CLKTbl, kCoreClock)+1);
    if(clock == kMCGOutClock)
    {
        return val;
    }
    val = val/(REG_GET(CLKTbl, clock)+1);
    return val;
}

void SetPinMux(uint32_t instance, uint32_t pin, uint32_t mux)
{
    CLK_EN(PORTCLKTbl, instance);
    PORT_IPTbl[instance]->PCR[pin] &= ~PORT_PCR_MUX_MASK;
    PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_MUX(mux);
}

/* 0 pull down, 1 pull up, other no floating */
void SetPinPull(uint32_t instance, uint32_t pin, uint32_t val)
{
    CLK_EN(PORTCLKTbl, instance);
    switch(val)
    {
        case 0:
            PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_PE_MASK;
            PORT_IPTbl[instance]->PCR[pin] &= ~PORT_PCR_PS_MASK;
            break;
        case 1:
            PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_PE_MASK;
            PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_PS_MASK;
            break;
        default:
            PORT_IPTbl[instance]->PCR[pin] &= ~PORT_PCR_PE_MASK;
            break;
    }
}

 /**
 * @brief  Enter ARM WAIT mode
 * @param  enSleepOnExit: if continue sleep when exit STOP mode
 * @retval None
 * @note  
 */
void EnterWaitMode(bool enSleepOnExit)
{
    /* Set the SLEEPDEEP bit to enable sleep mode (WAIT) */
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    if (enSleepOnExit)
    {
        SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    }
    else
    {
        SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
    }
    
    /* WFI instruction will start entry into STOP mode */
    __ASM("WFI");
}

uint32_t GetResetStatus(void)
{
    uint8_t SRS0, SRS1;

    SRS0 = RCM->SRS0;
    SRS1 = RCM->SRS1;
    
    if(SRS0 & RCM_SRS0_WAKEUP_MASK)
        return SYSTEM_RESET_WEAKUP;
    
    if(SRS0 & RCM_SRS0_LVD_MASK)
        return SYSTEM_RESET_LVD;
    
    if(SRS0 & RCM_SRS0_PIN_MASK)
        return SYSTEM_RESET_PIN;
    
    if(SRS0 & RCM_SRS0_POR_MASK)
        return SYSTEM_RESET_POR;
    
    if(SRS1 & RCM_SRS1_SW_MASK)
        return SYSTEM_RESET_SW;
    
    return 0;
}

uint32_t EncodeMAP(map_t * type)
{
    return *(uint32_t*)type;
}


void DecodeMAP(uint32_t map, map_t * type)
{
    map_t * pMap = (map_t*)&(map);
    memcpy(type, pMap, sizeof(map_t));  
}

/**
 * @brief  延时初始化函数
 * @code
 *      // 完成延时初始化配置，
 *      //使用内核的SYStic模块实现延时功能
 *        DelayInit();
 * @endcode
 * @param  None
 * @retval None
 */
#pragma weak DelayInit
void DelayInit(void)
{
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; 
    fac_us = GetClock(kCoreClock);
    fac_us /= 1000000;
    fac_ms = fac_us * 1000;
    SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
}

/**
 * @brief 毫秒级的延时设置函数
 * @code
 *      // 实现500ms的延时功能
 *        DelayMs(500);
 * @endcode
 * @param  ms :需要延时的时间，单位毫秒
 * @retval None
 */

#pragma weak DelayMs
void DelayMs(uint32_t ms)
{
    uint32_t temp;
    uint32_t i;
    SysTick->LOAD = fac_ms;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
    for(i = 0; i < ms; i++)
	{
		SysTick->VAL = 0;
		do
		{
			temp = SysTick->CTRL;
		}
        while((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
	}
}

/**
 * @brief 微秒级的延时设置函数
 * @code
 *      // 实现500us的延时功能
 *        DelayUs(500);
 * @endcode
 * @param  us :需要延时的时间，单位微秒
 * @retval None
 */
#pragma weak DelayUs
void DelayUs(uint32_t us)
{
    uint32_t temp;
    SysTick->LOAD = us * fac_us;
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
}

void SetConsole(int (*putc)(uint8_t ch), int (*getc)(void))
{
    Console.getc = getc;
    Console.putc = putc;
}

#include <stdio.h>
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
int fputc(int ch,FILE *f)
{
    if(Console.putc)
    {
        Console.putc((uint8_t)ch);
    }
	return ch;
}

int fgetc(FILE *f)
{
    if(Console.getc)
    {
        return (Console.getc() & 0xFF);
    }
    return 0;
}

#elif __ICCARM__ /* IAR support */
size_t __write(int handle, const unsigned char * buffer, size_t size)
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
        Console.putc(*buffer++);
        ++nChars;
    }
    return nChars;
}

size_t __read(int handle, unsigned char * buffer, size_t size)
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
    if ((handle != _LLIO_STDIN) && (handle != _LLIO_STDERR))
    {
        return _LLIO_ERROR;
    }
    /* read data.*/
    while (size--)
    {
        *buffer++ = Console.getc() & 0xFF;
        ++nChars;
    }
    return nChars;
}
#endif /* comiler support */

void SystemSoftReset(void)
{
    NVIC_SystemReset();
}

uint32_t GetUID(void) 
{
    return SIM->UIDL ^ SIM->UIDML ^ SIM->UIDMH;
}

/**
 * @brief  Systick set interrupt mode
 * @param  val: enable control
 * @retval None
 */
void SysTick_SetIntMode(bool val)
{
    (true == val)?(SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk):(SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk);
}
    
/**
 * @brief  Systick set time
 * @param  us
 * @retval None
 */
void SysTick_SetTime(uint32_t us)
{
    ((us*fac_us) > 0xFFFFFF)?(SysTick->LOAD = 0xFFFFFF):(SysTick->LOAD = us*fac_us);
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
}

void NMI_Handler(void)
{
    
}

/*
    for(i=0;i<ARRAY_SIZE(I2C_QuickInitTable);i++)
    {
        printf("(0X%08XU)\r\n", QuickInitEncode(&I2C_QuickInitTable[i]));
    }
*/

