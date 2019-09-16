/**
  ******************************************************************************
  * @file    tpm.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "tpm.h"
#include "common.h"
#include "gpio.h"

#if 0
/* global vars */
TPM_Type * const TPM_InstanceTable[] = TPM_BASES;
static TPM_CallBackType TPM_CallBackTable[ARRAY_SIZE(TPM_InstanceTable)] = {NULL};


static const uint32_t TPM_ChlMaxTable[] = {5,2,2}; /* reference to chip configuration->flextimer configuration */
static const struct reg_ops SIM_TPMClockGateTable[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_TPM0_MASK},
    {(void*)&(SIM->SCGC6), SIM_SCGC6_TPM1_MASK},
    {(void*)&(SIM->SCGC6), SIM_SCGC6_TPM2_MASK},
};
static const IRQn_Type TPM_IRQnTable[] = 
{
    TPM0_IRQn,
    TPM1_IRQn,
    TPM2_IRQn,
};

/* internal use, FTM mode select */
typedef enum
{
    kTPM_Mode_EdgeAligned,
    kTPM_Mode_CenterAligned,
    kFTM_Mode_InputCapture,
}TPM_Mode_Type;


static void _TPM_InitBasic(uint32_t instance, uint32_t modulo, TPM_ClockDiv_Type ps)
{
    /* enable clock gate */
    *(uint32_t*)SIM_TPMClockGateTable[instance].addr |= SIM_TPMClockGateTable[instance].mask;
    
    /* disable FTM, we must set CLKS(0) before config FTM! */
    TPM_InstanceTable[instance]->SC = 0;
     
    /* set CNT and CNTIN */
    TPM_InstanceTable[instance]->CNT = 0;
    
    /* set modulo */
    TPM_InstanceTable[instance]->MOD = modulo;
    
    /* set FTM clock to system clock */
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
    
    /* set ps, this must be done after set modulo */
    TPM_InstanceTable[instance]->SC &= ~TPM_SC_PS_MASK;
    TPM_InstanceTable[instance]->SC |= TPM_SC_PS(ps); 
    
}


void TPM_PWM_InvertPolarity(uint32_t instance, uint8_t chl, uint32_t config)
{
    switch(config)
    {
        case kTPM_PWM_HighTrue:
            TPM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~TPM_CnSC_ELSA_MASK;
            TPM_InstanceTable[instance]->CONTROLS[chl].CnSC |= TPM_CnSC_ELSB_MASK;
            break;
        case kTPM_PWM_LowTrue:
            TPM_InstanceTable[instance]->CONTROLS[chl].CnSC |= TPM_CnSC_ELSA_MASK;
            TPM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~TPM_CnSC_ELSB_MASK;
            break;
        default:
            break;
    }
}

static void TPM_SetMode(uint32_t instance, uint8_t chl, TPM_Mode_Type mode)
{
    TPM_InstanceTable[instance]->SC &= ~TPM_SC_CMOD_MASK;
    TPM_InstanceTable[instance]->SC |=  TPM_SC_CMOD(0);
    switch(mode)
    {
        /* FSL: CnSC must be written all bit at one time */
        case kTPM_Mode_EdgeAligned:
            TPM_InstanceTable[instance]->SC &= ~TPM_SC_CPWMS_MASK;
            TPM_InstanceTable[instance]->CONTROLS[chl].CnSC |= TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
            TPM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~TPM_CnSC_MSA_MASK;
            LIB_TRACE("chl:%d\r\n", chl);
            LIB_TRACE("Cnsc:0x%X\r\n", TPM_InstanceTable[instance]->CONTROLS[chl].CnSC);
            break;
        case kTPM_Mode_CenterAligned:
            TPM_InstanceTable[instance]->SC |= TPM_SC_CPWMS_MASK;
            TPM_InstanceTable[instance]->CONTROLS[chl].CnSC |= TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
            TPM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~TPM_CnSC_MSA_MASK;
        case kFTM_Mode_InputCapture:
            TPM_InstanceTable[instance]->SC &= ~TPM_SC_CPWMS_MASK;
            TPM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~TPM_CnSC_MSB_MASK;
            TPM_InstanceTable[instance]->CONTROLS[chl].CnSC &= ~TPM_CnSC_MSA_MASK;  
            break;
        default:
            break;
    }        
    TPM_InstanceTable[instance]->SC &= ~TPM_SC_CMOD_MASK;
    TPM_InstanceTable[instance]->SC |=  TPM_SC_CMOD(1);
}

/**
 * @brief  改变PWM占空比
 * @endcode         
 * @param  instance       : 模块号
 * @param  chl            : 通道
 * @param  pwmDuty        : 占空比
 * @retval None
 */
void TPM_PWM_ChangeDuty(uint32_t instance, uint8_t chl, uint32_t pwmDuty)
{
    uint32_t cv = ((TPM_InstanceTable[instance]->MOD) * pwmDuty) / 10000;
    TPM_InstanceTable[instance]->CONTROLS[chl].CnV = cv;
}

/**
 * @brief  初始化PWM波产生
 * @code
 *      
 *  //TPM0_CH0_PE24 使用FTM0模块通道0 引脚E24上产生1KHz PWM波
 *   FTM_PWM_QuickInit(TPM0_CH0_PE24, 1000); 
 * @endcode         
 * @param  MAP            : 快速初始化通道
 * @param  frequencyInHZ  : 频率
 * @retval None
 */
uint8_t TPM_PWM_QuickInit(uint32_t MAP, TPM_PWM_Mode_Type mode, uint32_t req)
{
    uint8_t i;
    uint32_t modulo;
    uint32_t clock;
    int32_t pres;
    uint8_t ps = 0;
    QuickInit_Type * pq = (QuickInit_Type*)&(MAP); 
    /* init pinmux */
    for(i = 0; i < pq->io_offset; i++)
    {
        PORT_PinMuxConfig(pq->io_instance, pq->io_base + i, (PORT_PinMux_Type) pq->mux); 
    }
    /* calc req and ps */
    uint32_t min_val = 0xFFFF;
    /* cal ps */
    CLOCK_GetClockFrequency(kMCGOutClock, &clock);
    if(MCG->C6 & MCG_C6_PLLS_MASK) /* PLL */
    {
        clock /= 2;
    }
    else /* FLL */
    {
        
    }
    LIB_TRACE("%s %d\r\n", __func__, clock);
    pres = (clock/req)/TPM_MOD_MOD_MASK;
    if((clock/req)/pres > TPM_MOD_MOD_MASK)
    {
        pres++;
    }
    for(i = 0; i < 7; i++)
    {
        if((ABS(pres - (1<<i))) < min_val)
        {
            ps = i;
            min_val = ABS(pres - (1<<i));
        }
    }
    if(pres > (1<<ps)) ps++;
    if(ps > 7) ps = 7;
    modulo = ((clock/(1<<ps))/req) - 1;
    LIB_TRACE("input frequency:%dHz\r\n", req);
    LIB_TRACE("input clk:%d\r\n", clock);
    LIB_TRACE("ps:%d\r\n", pres);
    LIB_TRACE("modulo:%d\r\n", modulo);
    LIB_TRACE("ip_instance:%d\r\n", pq->ip_instance);
    LIB_TRACE("io_instance:%d\r\n", pq->io_instance);
    LIB_TRACE("io_base:%d\r\n", pq->io_base);
    LIB_TRACE("io_offset:%d\r\n", pq->io_offset);
    LIB_TRACE("chl:%d\r\n", pq->channel);
    _TPM_InitBasic(pq->ip_instance, modulo, (TPM_ClockDiv_Type)ps);
    /* set FTM mode */
    switch(mode)
    {
        case kPWM_EdgeAligned:
            TPM_SetMode(pq->ip_instance, pq->channel, kTPM_Mode_EdgeAligned);
            break;
        default:
            LIB_TRACE("error in FTM_PWM_Init\r\n");
            break;
    }
    /* set duty to 50% */
    TPM_PWM_ChangeDuty(pq->ip_instance, pq->channel, 5000);
    return pq->ip_instance;
}



void TPM_ITDMAConfig(uint32_t instance, TPM_ITDMAConfig_Type config, bool status)
{
    if(status)
    {
        NVIC_EnableIRQ(TPM_IRQnTable[instance]);
    }

    switch(config)
    {
        case kTPM_IT_TOF:
            (status)?
            (TPM_InstanceTable[instance]->SC |= TPM_SC_TOIE_MASK):
            (TPM_InstanceTable[instance]->SC &= ~TPM_SC_TOIE_MASK);
            break;
        case kTPM_IT_CH0:
        case kTPM_IT_CH1:
        case kTPM_IT_CH2:
        case kTPM_IT_CH3:
        case kTPM_IT_CH4:
        case kTPM_IT_CH5:
            (status)?
            (TPM_InstanceTable[instance]->CONTROLS[config].CnSC |= TPM_CnSC_CHIE_MASK):
            (TPM_InstanceTable[instance]->CONTROLS[config].CnSC &= ~TPM_CnSC_CHIE_MASK);
        case kTPM_DMA_CH0:
        case kTPM_DMA_CH1:     
        case kTPM_DMA_CH2:
        case kTPM_DMA_CH3:
        case kTPM_DMA_CH4:
        case kTPM_DMA_CH5:
            (status)?
            (TPM_InstanceTable[instance]->CONTROLS[config].CnSC |= TPM_CnSC_CHIE_MASK):
            (TPM_InstanceTable[instance]->CONTROLS[config].CnSC &= ~TPM_CnSC_CHIE_MASK);
            (status)?
            (TPM_InstanceTable[instance]->CONTROLS[config].CnSC |= TPM_CnSC_DMA_MASK):
            (TPM_InstanceTable[instance]->CONTROLS[config].CnSC &= ~TPM_CnSC_DMA_MASK);
        default:
            break;
    }
}

void TPM_CallbackInstall(uint32_t instance, TPM_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        TPM_CallBackTable[instance] = AppCBFun;
    }
}

/**
 * @brief  获得TPM 通道计数值
 * @param  instance     : 模块号
 * @param  chl          : 通道号
 * @retval 计数值
 */
uint32_t TPM_GetChlCounter(uint32_t instance, uint32_t chl)
{
    return TPM_InstanceTable[instance]->CONTROLS[chl].CnV;
}

/**
 * @brief  设置TPM主通道计数值
 * @param  instance     : 模块号
 * @param  val          : value
 * @retval None
 */
void TPM_SetMoudleCounter(uint32_t instance, uint32_t val)
{
    TPM_InstanceTable[instance]->CNT = val;
}


void TPM0_IRQHandler(void)
{
    uint32_t i;
    if(TPM_CallBackTable[0])
    {
        TPM_CallBackTable[0]();
    }
    /* clear pending register */
    if(TPM_InstanceTable[0]->SC & (TPM_SC_TOF_MASK | TPM_SC_TOIE_MASK))
    {
        TPM_InstanceTable[0]->SC |= TPM_SC_TOF_MASK;
    }
    for(i = 0; i < TPM_ChlMaxTable[0]; i++)
    {
        if(TPM_InstanceTable[0]->CONTROLS[i].CnSC & (TPM_CnSC_CHIE_MASK | TPM_CnSC_CHF_MASK))
        {
            TPM_InstanceTable[0]->CONTROLS[i].CnSC |= TPM_CnSC_CHF_MASK;
            break;
        }
    }
}

void TPM1_IRQHandler(void)
{
    uint32_t i;
    if(TPM_CallBackTable[1])
    {
        TPM_CallBackTable[1]();
    } 
    /* clear pending register */
    if(TPM_InstanceTable[1]->SC & (TPM_SC_TOF_MASK | TPM_SC_TOIE_MASK))
    {
        TPM_InstanceTable[1]->SC |= TPM_SC_TOF_MASK;
    }
    for(i = 0; i < TPM_ChlMaxTable[1]; i++)
    {
        if(TPM_InstanceTable[1]->CONTROLS[i].CnSC & (TPM_CnSC_CHIE_MASK | TPM_CnSC_CHF_MASK))
        {
            TPM_InstanceTable[1]->CONTROLS[i].CnSC |= TPM_CnSC_CHF_MASK;
            break;
        }
    }
}

void TPM2_IRQHandler(void)
{
    uint32_t i;
    if(TPM_CallBackTable[2])
    {
        TPM_CallBackTable[2]();
    }
    /* clear pending register */
    if(TPM_InstanceTable[2]->SC & (TPM_SC_TOF_MASK | TPM_SC_TOIE_MASK))
    {
        TPM_InstanceTable[2]->SC |= TPM_SC_TOF_MASK;
    }
    for(i = 0; i < TPM_ChlMaxTable[2]; i++)
    {
        if(TPM_InstanceTable[2]->CONTROLS[i].CnSC & (TPM_CnSC_CHIE_MASK | TPM_CnSC_CHF_MASK))
        {
            TPM_InstanceTable[2]->CONTROLS[i].CnSC |= TPM_CnSC_CHF_MASK;
            break;
        }
    }
}

/*

static const QuickInit_Type TPM_QuickInitTable[] =
{
    { 0, 4, 3, 24, 1, 0}, //TPM0_CH0_PE24
    { 0, 0, 3,  3, 1, 0}, //TPM0_CH0_PA03
    { 0, 2, 4,  1, 1, 0}, //TPM0_CH0_PC01
    { 0, 3, 4,  0, 1, 0}, //TPM0_CH0_PD00
    { 0, 4, 3, 25, 1, 1}, //TPM0_CH1_PE25
    { 0, 0, 3,  4, 1, 1}, //TPM0_CH1_PA04
    { 0, 2, 4,  2, 1, 1}, //TPM0_CH1_PC02
    { 0, 3, 4,  1, 1, 1}, //TPM0_CH1_PD01
    { 0, 3, 4,  2, 1, 2}, //TPM0_CH2_PD02
    { 0, 4, 3, 29, 1, 2}, //TPM0_CH2_PE29
    { 0, 0, 3,  5, 1, 2}, //TPM0_CH2_PA05
    { 0, 2, 4,  3, 1, 2}, //TPM0_CH2_PC03
    { 0, 3, 4,  3, 1, 3}, //TPM0_CH3_PD03
    { 0, 4, 3, 30, 1, 3}, //TPM0_CH3_PE30
    { 0, 0, 3,  6, 1, 3}, //TPM0_CH3_PA06
    { 0, 2, 4,  4, 1, 3}, //TPM0_CH3_PC04
    { 0, 2, 3,  8, 1, 4}, //TPM0_CH4_PC08
    { 0, 3, 4,  4, 1, 4}, //TPM0_CH4_PD04
    { 0, 4, 3, 31, 1, 4}, //TPM0_CH4_PE31
    { 0, 0, 3,  7, 1, 4}, //TPM0_CH4_PA07
    { 0, 4, 3, 26, 1, 5}, //TPM0_CH5_PE26
    { 0, 0, 3,  0, 1, 5}, //TPM0_CH5_PA00
    { 0, 2, 3,  9, 1, 5}, //TPM0_CH5_PC09
    { 0, 3, 4,  5, 1, 5}, //TPM0_CH5_PD05
    { 1, 0, 3, 12, 1, 0}, //TPM1_CH0_PA12
    { 1, 1, 3,  0, 1, 0}, //TPM1_CH0_PB00
    { 1, 4, 3, 20, 1, 0}, //TPM1_CH0_PE20
    { 1, 4, 3, 21, 1, 1}, //TPM1_CH1_PE21
    { 1, 0, 3, 13, 1, 1}, //TPM1_CH1_PA13
    { 1, 1, 3,  1, 1, 1}, //TPM1_CH1_PB01
    { 2, 0, 3,  1, 1, 0}, //TPM2_CH0_PA01
    { 2, 1, 3,  2, 1, 0}, //TPM2_CH0_PB02
    { 2, 1, 3, 18, 1, 0}, //TPM2_CH0_PB18
    { 2, 4, 3, 23, 1, 1}, //TPM2_CH1_PE23
    { 2, 0, 3,  2, 1, 1}, //TPM2_CH1_PA02
    { 2, 1, 3,  3, 1, 1}, //TPM2_CH1_PB03
    { 2, 1, 3, 19, 1, 1}, //TPM2_CH1_PB19
};
*/
#endif
