/**
  ******************************************************************************
  * @file    tpm.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_KL_TPM_H__
#define __CH_LIB_KL_TPM_H__

#include <stdint.h>
#include <stdbool.h>

/* TPM模块号 */
#define HW_TPM0    (0x00)
#define HW_TPM1    (0x01) 
#define HW_TPM2    (0x02) 


/* TPM 通道号 */
#define HW_TPM_CH0 (0x00)
#define HW_TPM_CH1 (0x01)
#define HW_TPM_CH2 (0x02)
#define HW_TPM_CH3 (0x03)
#define HW_TPM_CH4 (0x04)
#define HW_TPM_CH5 (0x05)

/* PWM 输出模式 */
typedef enum
{
    kPWM_EdgeAligned,           
}TPM_PWM_Mode_Type;
/* PWM 极性 */
#define kTPM_PWM_HighTrue       (0x00)
#define kTPM_PWM_LowTrue        (0x01)


//!< Callback
typedef void (*TPM_CallBackType)(void);


//!< TPM 中断及DMA配置 
typedef enum
{
    kTPM_IT_TOF = 50,
    kTPM_IT_CH0 = 0,
    kTPM_IT_CH1 = 1,
    kTPM_IT_CH2 = 2,
    kTPM_IT_CH3 = 3,
    kTPM_IT_CH4 = 4,
    kTPM_IT_CH5 = 5,
    kTPM_DMA_CH0 = 10,
    kTPM_DMA_CH1 = 11,
    kTPM_DMA_CH2 = 12,
    kTPM_DMA_CH3 = 13,
    kTPM_DMA_CH4 = 14,
    kTPM_DMA_CH5 = 15,
}TPM_ITDMAConfig_Type;

/* 快速初始化结构 */
#define TPM0_CH0_PE24   (0x000070E0U)
#define TPM0_CH0_PA03   (0x000046C0U)
#define TPM0_CH0_PC01   (0x00004310U)
#define TPM0_CH0_PD00   (0x00004118U)
#define TPM0_CH1_PE25   (0x000872E0U)
#define TPM0_CH1_PA04   (0x000848C0U)
#define TPM0_CH1_PC02   (0x00084510U)
#define TPM0_CH1_PD01   (0x00084318U)
#define TPM0_CH2_PD02   (0x00104518U)
#define TPM0_CH2_PE29   (0x00107AE0U)
#define TPM0_CH2_PA05   (0x00104AC0U)
#define TPM0_CH2_PC03   (0x00104710U)
#define TPM0_CH3_PD03   (0x00184718U)
#define TPM0_CH3_PE30   (0x00187CE0U)
#define TPM0_CH3_PA06   (0x00184CC0U)
#define TPM0_CH3_PC04   (0x00184910U)
#define TPM0_CH4_PC08   (0x002050D0U)
#define TPM0_CH4_PD04   (0x00204918U)
#define TPM0_CH4_PE31   (0x00207EE0U)
#define TPM0_CH4_PA07   (0x00204EC0U)
#define TPM0_CH5_PE26   (0x002874E0U)
#define TPM0_CH5_PA00   (0x002840C0U)
#define TPM0_CH5_PC09   (0x002852D0U)
#define TPM0_CH5_PD05   (0x00284B18U)
#define TPM1_CH0_PA12   (0x000058C1U)
#define TPM1_CH0_PB00   (0x000040C9U)
#define TPM1_CH0_PE20   (0x000068E1U)
#define TPM1_CH1_PE21   (0x00086AE1U)
#define TPM1_CH1_PA13   (0x00085AC1U)
#define TPM1_CH1_PB01   (0x000842C9U)
#define TPM2_CH0_PA01   (0x000042C2U)
#define TPM2_CH0_PB02   (0x000044CAU)
#define TPM2_CH0_PB18   (0x000064CAU)
#define TPM2_CH1_PE23   (0x00086EE2U)
#define TPM2_CH1_PA02   (0x000844C2U)
#define TPM2_CH1_PB03   (0x000846CAU)
#define TPM2_CH1_PB19   (0x000866CAU)

typedef enum
{
    kTPM_ClockDiv1,
    kTPM_ClockDiv2,
    kTPM_ClockDiv4,
    kTPM_ClockDiv8,
    kTPM_ClockDiv16,
    kTPM_ClockDiv32,
    kTPM_ClockDiv64,
    kTPM_ClockDiv128,
}TPM_ClockDiv_Type;

//!< API functions

/* PWM functions */
uint8_t TPM_PWM_QuickInit(uint32_t MAP, TPM_PWM_Mode_Type mode, uint32_t req);
void TPM_PWM_ChangeDuty(uint32_t instance, uint8_t chl, uint32_t pwmDuty);
void TPM_PWM_InvertPolarity(uint32_t instance, uint8_t chl, uint32_t config);
/* IC functions */
//void FTM_IC_QuickInit(uint32_t MAP, FTM_ClockDiv_Type ps);
//void FTM_IC_SetTriggerMode(uint32_t instance, uint32_t chl, FTM_IC_Mode_Type mode);
/* IT & DMA config */
void TPM_ITDMAConfig(uint32_t instance, TPM_ITDMAConfig_Type config, bool status);
void TPM_CallbackInstall(uint32_t instance, TPM_CallBackType AppCBFun);
/* control function */
uint32_t TPM_GetChlCounter(uint32_t instance, uint32_t chl);
void TPM_SetMoudleCounter(uint32_t instance, uint32_t val);


#endif

