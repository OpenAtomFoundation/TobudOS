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
 * @file n32g020xx_pmu.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @brief PMU模块接口声明文件
 * 
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * 
 * @defgroup PMU PMU电源管理单元模块
 * @{
 */
#ifndef __N32G020XX_PMU_H__
#define __N32G020XX_PMU_H__
//------------------------------------------------------------------
//                        Headers
//------------------------------------------------------------------
#include "n32g020xx_common.h"
#include "n32g020xx.h"

//------------------------------------------------------------------
//                        Definitions
//------------------------------------------------------------------
#define PMU_EnableFlashVrefCtrl() SET_REG_BIT(PMU->CTRL0, BIT7)
#define PMU_DisableFlashVrefCtrl() CLR_REG_BIT(PMU->CTRL0, BIT7)
#define PMU_EnableOsc80MCtrl() SET_REG_BIT(PMU->CTRL1, BIT0)
#define PMU_DisableOsc80MCtrl() CLR_REG_BIT(PMU->CTRL1, BIT0)
#define PMU_EnableVraCtrl() SET_REG_BIT(PMU->CTRL1, BIT1)
#define PMU_DisableVraCtrl() CLR_REG_BIT(PMU->CTRL1, BIT1)
#define PMU_EnableVrfCtrl() SET_REG_BIT(PMU->CTRL1, BIT2)
#define PMU_DisableVrfCtrl() CLR_REG_BIT(PMU->CTRL1, BIT2)
#define PMU_EnableMainRegulatorLPCtrl() SET_REG_BIT(PMU->CTRL1, BIT3)
#define PMU_DisableMainRegulatorLPCtrl() CLR_REG_BIT(PMU->CTRL1, BIT3)
#define PMU_EnableVccGlitchDetCtrl() SET_REG_BIT(PMU->CTRL1, BIT4)
#define PMU_DisableVccGlitchDetCtrl() CLR_REG_BIT(PMU->CTRL1, BIT4)
#define PMU_EnableVdddGlitchDetCtrl() SET_REG_BIT(PMU->CTRL1, BIT5)
#define PMU_DisableVdddGlitchDetCtrl() CLR_REG_BIT(PMU->CTRL1, BIT5)
#define PMU_EnableVcchCtrl() SET_REG_BIT(PMU->CTRL1, BIT6)
#define PMU_DisableVcchCtrl() CLR_REG_BIT(PMU->CTRL1, BIT6)
#define PMU_EnableVcclCtrl() SET_REG_BIT(PMU->CTRL1, BIT7)
#define PMU_DisableVcclCtrl() CLR_REG_BIT(PMU->CTRL1, BIT7)
#define PMU_EnableVccvlCtrl() SET_REG_BIT(PMU->CTRL2, BIT0)
#define PMU_DisableVccvlCtrl() CLR_REG_BIT(PMU->CTRL2, BIT0)
#define PMU_EnableVdddvlCtrl() SET_REG_BIT(PMU->CTRL2, BIT1)
#define PMU_DisableVdddvlCtrl() CLR_REG_BIT(PMU->CTRL2, BIT1)
#define PMU_EnableVdd33Ctrl() SET_REG_BIT(PMU->CTRL2, BIT2)
#define PMU_DisableVdd33Ctrl() CLR_REG_BIT(PMU->CTRL2, BIT2)
#define PMU_EnableVdddCtrl() SET_REG_BIT(PMU->CTRL2, BIT3)
#define PMU_DisableVdddCtrl() CLR_REG_BIT(PMU->CTRL2, BIT3)
#define PMU_EnableVddfCtrl() SET_REG_BIT(PMU->CTRL2, BIT4)
#define PMU_DisableVddfCtrl() CLR_REG_BIT(PMU->CTRL2, BIT4)
#define PMU_EnableGroundVddeCtrl() SET_REG_BIT(PMU->CTRL2, BIT5)
#define PMU_DisableGroundVddeCtrl() CLR_REG_BIT(PMU->CTRL2, BIT5)

#define PMU_GetUsbDetInt() GET_REG_BIT(PMU->WKEXTINEN, BIT0)
#define PMU_EnableUsbDetInt() SET_REG_BIT(PMU->WKEXTINEN_SET, BIT0)
#define PMU_DisableUsbDetInt() SET_REG_BIT(PMU->WKEXTINEN_CLR, BIT0)

#define PMU_GetWakeUp3Int() GET_REG_BIT(PMU->WKEXTINEN, BIT1)
#define PMU_EnableWakeUp3Int() SET_REG_BIT(PMU->WKEXTINEN_SET, BIT1)
#define PMU_DisableWakeUp3Int() SET_REG_BIT(PMU->WKEXTINEN_CLR, BIT1)

#define PMU_GetWakeUp1Int() GET_REG_BIT(PMU->WKEXTINEN, BIT2)
#define PMU_EnableWakeUp1Int() SET_REG_BIT(PMU->WKEXTINEN_SET, BIT2)
#define PMU_DisableWakeUp1Int() SET_REG_BIT(PMU->WKEXTINEN_CLR, BIT2)

#define PMU_GetWakeUp2Int() GET_REG_BIT(PMU->WKEXTINEN, BIT3)
#define PMU_EnableWakeUp2Int() SET_REG_BIT(PMU->WKEXTINEN_SET, BIT3)
#define PMU_DisableWakeUp2Int() SET_REG_BIT(PMU->WKEXTINEN_CLR, BIT3)

#define PMU_GetUsbDetIS() GET_REG_BIT(PMU->WKEXTINSTR, BIT0)
#define PMU_SetUsbDetIS() SET_REG_BIT(PMU->WKEXTINSTR_SET, BIT0)
#define PMU_ClearUsbDetIS() SET_REG_BIT(PMU->WKEXTINSTR_CLR, BIT0)

#define PMU_GetWakeUp3IS() GET_REG_BIT(PMU->WKEXTINSTR, BIT1)
#define PMU_SetWakeUp3IS() SET_REG_BIT(PMU->WKEXTINSTR_SET, BIT1)
#define PMU_ClearWakeUp3IS() SET_REG_BIT(PMU->WKEXTINSTR_CLR, BIT1)

#define PMU_GetWakeUp1IS() GET_REG_BIT(PMU->WKEXTINSTR, BIT2)
#define PMU_SetWakeUp1IS() SET_REG_BIT(PMU->WKEXTINSTR_SET, BIT2)
#define PMU_ClearWakeUp1IS() SET_REG_BIT(PMU->WKEXTINSTR_CLR, BIT2)

#define PMU_GetWakeUp2IS() GET_REG_BIT(PMU->WKEXTINSTR, BIT3)
#define PMU_SetWakeUp2IS() SET_REG_BIT(PMU->WKEXTINSTR_SET, BIT3)
#define PMU_ClearWakeUp2IS() SET_REG_BIT(PMU->WKEXTINSTR_CLR, BIT3)


//------------------------------------------------------------------
//                        TypeDefs
//------------------------------------------------------------------
typedef enum
{
    WAKEUP_WFI, 
    WAKEUP_WFE,
} PMU_WU_METHOD; // 用于IDLE或STANDBY

typedef enum
{
    Src_Usb             = 0,
    Src_Rcv             = 1,
    Src_RcvLevel        = 2, // 区分RCV高电平或RCV低电平
    Src_UsbDetect       = 3,
    Src_WakeUpIO3       = 5,
    Src_Rtc             = 6,
    Src_WakeCore        = 7, // 区分唤醒源为RXEV事件或INT0
    Src_Timer4          = 8,
    Src_WakeUpIO1       = 9,
    Src_WakeUpIO2       = 10,
    Src_ComparatorLevel = 11, // 区分Comparator高电平或RCV低电平
    Src_Comparator      = 12,
} PMU_WAKEUP_SRC;

typedef enum
{
    WKSTR_Gpio0 = 1,
    WKSTR_Gpio1,
    WKSTR_Gpio2,
    WKSTR_Gpio3,
    WKSTR_Gpio4,
    WKSTR_Gpio5,
    WKSTR_Gpio6,
    WKSTR_Gpio7,
    WKSTR_Gpio8,
    WKSTR_Gpio9,
    WKSTR_Gpio10,
    WKSTR_Gpio11,
    WKSTR_Gpio12,
    WKSTR_Gpio13,
    WKSTR_Gpio14,
    WKSTR_Gpio15,
    WKSTR_Uart0Rx,
    WKSTR_Uart2Rx,
    WKSTR_Spis0Csn,
    WKSTR_Spis1Csn,
    WKSTR_I2c0Sda,
    WKSTR_I2c1Sda,
    WKSTR_Timer4 = 24,
    WKSTR_UsbResume,
    WKSTR_UsbDetect,
    WKSTR_WakeUpIO3,
    WKSTR_Rcv,
    WKSTR_WakeUpIO1,
    WKSTR_WakeUpIO2,
    WKSTR_Rtc,
    WKSTR_Comparator,
    WKSTR_ScdSio,
    WKSTR_ScdRst,
    WKSTR_ScdPlug,
    WKSTR_ScdUnplug,
} PMU_WAKEUP_STATE_IO;

typedef enum
{
    NORMAL_MODE      = 0x0UL, // Normal模式
    PD_MODE_RTC_SRAM = 0x3UL, // PD模式（RTC和5KB备电SRAM）
    PD_MODE          = 0x5UL, // PD模式
} RETPMU_PMU_MODE;

//------------------------------------------------------------------
//                        Exported variables
//------------------------------------------------------------------

//------------------------------------------------------------------
//                        Exported functions
//------------------------------------------------------------------
/**
 * @brief PMU唤醒源配置函数
 * 
 * @param src 唤醒源
 * @param sel 二选一，0：唤醒禁止或RCV/比较器低电平唤醒或RXEV事件唤醒，1：唤醒使能或RCV/比较器高电平唤醒或INT10唤醒
 * 
 * 部分唤醒源需要叠加配置才能有效，具体详见产品用户手册
 */
void PMU_ConfigWakeUpSrc(PMU_WAKEUP_SRC src, uint32_t sel);

/**
 * @brief 清除PMU唤醒状态函数
 * 
 * @param io 详见枚举类型PMU_WAKEUP_STATE_IO或产品用户手册
 */
void PMU_ClearWakeUpState(PMU_WAKEUP_STATE_IO io);

/**
 * @brief 获取PMU唤醒状态函数
 * 
 * @param io 详见枚举类型PMU_WAKEUP_STATE_IO或产品用户手册
 * @param state io对应的唤醒状态
 */
void PMU_GetWakeUpState(PMU_WAKEUP_STATE_IO io, uint32_t* state);

/**
 * @brief 进入IDLE函数
 * 
 * @param method 二选一，WAKEUP_WFI：WFI方式，WAKEUP_WFE：WFE方式
 */
void PMU_EnterIdle(PMU_WU_METHOD method);

/**
 * @brief 进入STANDBY函数
 * 
 * @param method 二选一，WAKEUP_WFI：WFI方式，WAKEUP_WFE：WFE方式
 */
void PMU_EnterStandBy(PMU_WU_METHOD method);

/**
 * @brief 利用RETPMU进入PowerDown
 * 
 * @param mode 三选一，NORMAL_MODE，PD_MODE_RTC_SRAM，PD_MODE，详见产品用户手册
 */
void RETPMU_SetPmuMode(RETPMU_PMU_MODE mode);

#endif // __N32G020XX_PMU_H__

/** @} */
