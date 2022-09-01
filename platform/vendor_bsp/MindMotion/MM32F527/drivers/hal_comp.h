/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_COMP_H__
#define __HAL_COMP_H__

#include "hal_common.h"

/*!
 * @addtogroup COMP
 * @{
 */

/*!
 * @brief COMP driver version number.
 */
#define COMP_DRIVER_VERSION 1u /*!< COMP_1. */

/*!
 * @brief COMP number.
 */
#define COMP_CHANNEL_NUM 3u

/*!
 * @brief COMP output filter type.
 *
 * If the compare result keeps n APB Clocks unchanged, the output is valid.
 */
typedef enum
{
    COMP_OutFilter_1   = 0x00, /*!< 1   PCLK filter. */
    COMP_OutFilter_2   = 0x01, /*!< 2   PCLK filter. */
    COMP_OutFilter_4   = 0x02, /*!< 4   PCLK filter. */
    COMP_OutFilter_8   = 0x03, /*!< 8   PCLK filter. */
    COMP_OutFilter_16  = 0x04, /*!< 16  PCLK filter. */
    COMP_OutFilter_32  = 0x05, /*!< 32  PCLK filter. */
    COMP_OutFilter_64  = 0x06, /*!< 64  PCLK filter. */
    COMP_OutFilter_128 = 0x07, /*!< 128 PCLK filter. */
} COMP_OutFilter_Type;

/*!
 * @brief COMP hysteresis type.
 *
 * If V(InvInput) > V(PosInput), the compare result is high, if V(InvInput) < (V(PosInput) - hysteresis), the compare result is low.
 */
typedef enum
{
    COMP_Hysteresis_Alt0 = 0x00, /*!< Hysteresis alt 0. */
    COMP_Hysteresis_Alt1 = 0x01, /*!< Hysteresis alt 1. */
    COMP_Hysteresis_Alt2 = 0x02, /*!< Hysteresis alt 2. */
    COMP_Hysteresis_Alt3 = 0x03, /*!< Hysteresis alt 3. */
} COMP_Hysteresis_Type;

/*!
 * @brief COMP output mux type.
 */
typedef enum
{
    COMP_OutMux_Alt0  = 0x00,  /*!< Output mux alt 0. */
    COMP_OutMux_Alt1  = 0x01,  /*!< Output mux alt 1. */
    COMP_OutMux_Alt2  = 0x02,  /*!< Output mux alt 2. */
    COMP_OutMux_Alt3  = 0x03,  /*!< Output mux alt 3. */
    COMP_OutMux_Alt4  = 0x04,  /*!< Output mux alt 4. */
    COMP_OutMux_Alt5  = 0x05,  /*!< Output mux alt 5. */
    COMP_OutMux_Alt6  = 0x06,  /*!< Output mux alt 6. */
    COMP_OutMux_Alt7  = 0x07,  /*!< Output mux alt 7. */
    COMP_OutMux_Alt8  = 0x08,  /*!< Output mux alt 8. */
    COMP_OutMux_Alt9  = 0x09,  /*!< Output mux alt 0. */
    COMP_OutMux_Alt11 = 0x0A,  /*!< Output mux alt 1. */
    COMP_OutMux_Alt12 = 0x0B,  /*!< Output mux alt 2. */
    COMP_OutMux_Alt13 = 0x0C,  /*!< Output mux alt 3. */
    COMP_OutMux_Alt14 = 0x0D,  /*!< Output mux alt 4. */
    COMP_OutMux_Alt15 = 0x0E,  /*!< Output mux alt 5. */
}   COMP_OutMux_Type;

/*!
 * @brief COMP input Mux type.
 */
typedef enum
{
    COMP_InMux_Alt0 = 0x00, /*!< Input mux 0. */
    COMP_InMux_Alt1 = 0x01, /*!< Input mux 1. */
    COMP_InMux_Alt2 = 0x02, /*!< Input mux 2. */
    COMP_InMux_Alt3 = 0x03, /*!< Input mux 3. */
    COMP_InMux_Alt4 = 0x04, /*!< Input mux 4. */
    COMP_InMux_Alt5 = 0x05, /*!< Input mux 5. */
} COMP_InMux_Type;

/*!
 * @brief COMP speed type.
 */
typedef enum
{
    COMP_Speed_High    = 0x00, /*!< High speed. */
    COMP_Speed_Middle  = 0x01, /*!< Middle speed. */
    COMP_Speed_Low     = 0x02, /*!< Low speed. */
    COMP_Speed_DeepLow = 0x03, /*!< Deep low speed. */
} COMP_Speed_Type;

/*!
 * @brief COMP external reference voltage source type.
 */
typedef enum
{
    COMP_ExtVrefSource_VREFINT = 0x00,  /*!< Internal reference voltage. */
    COMP_ExtVrefSource_VDDA    = 0x01,  /*!< VDDA voltage. */
} COMP_ExtVrefSource_Type;

/*!
 * @brief COMP external reference voltage type.
 */
typedef enum
{
    COMP_ExtVrefVolt_Alt0  = 0x00, /*!< External reference voltage alt 0. */
    COMP_ExtVrefVolt_Alt1  = 0x01, /*!< External reference voltage alt 1. */
    COMP_ExtVrefVolt_Alt2  = 0x02, /*!< External reference voltage alt 2. */
    COMP_ExtVrefVolt_Alt3  = 0x03, /*!< External reference voltage alt 3. */
    COMP_ExtVrefVolt_Alt4  = 0x04, /*!< External reference voltage alt 4. */
    COMP_ExtVrefVolt_Alt5  = 0x05, /*!< External reference voltage alt 5. */
    COMP_ExtVrefVolt_Alt6  = 0x06, /*!< External reference voltage alt 6. */
    COMP_ExtVrefVolt_Alt7  = 0x07, /*!< External reference voltage alt 7. */
    COMP_ExtVrefVolt_Alt8  = 0x08, /*!< External reference voltage alt 8. */
    COMP_ExtVrefVolt_Alt9  = 0x09, /*!< External reference voltage alt 9. */
    COMP_ExtVrefVolt_Alt10 = 0x0A, /*!< External reference voltage alt 10. */
    COMP_ExtVrefVolt_Alt11 = 0x0B, /*!< External reference voltage alt 11. */
    COMP_ExtVrefVolt_Alt12 = 0x0C, /*!< External reference voltage alt 12. */
    COMP_ExtVrefVolt_Alt13 = 0x0D, /*!< External reference voltage alt 13. */
    COMP_ExtVrefVolt_Alt14 = 0x0E, /*!< External reference voltage alt 14. */
    COMP_ExtVrefVolt_Alt15 = 0x0F, /*!< External reference voltage alt 15. */
} COMP_ExtVrefVolt_Type;

/*!
 * @brief COMP round robin period type.
 *
 * wait n APB clock to compare next channel.
 */
typedef enum
{
    COMP_RoundRobinPeriod_1   = 0x00, /*!< Wait 1   PCLK to compare next channel. */
    COMP_RoundRobinPeriod_2   = 0x01, /*!< Wait 2   PCLK to compare next channel. */
    COMP_RoundRobinPeriod_4   = 0x02, /*!< Wait 4   PCLK to compare next channel. */
    COMP_RoundRobinPeriod_8   = 0x03, /*!< Wait 8   PCLK to compare next channel. */
    COMP_RoundRobinPeriod_16  = 0x04, /*!< Wait 16  PCLK to compare next channel. */
    COMP_RoundRobinPeriod_32  = 0x05, /*!< Wait 32  PCLK to compare next channel. */
    COMP_RoundRobinPeriod_64  = 0x06, /*!< Wait 64  PCLK to compare next channel. */
    COMP_RoundRobinPeriod_128 = 0x07, /*!< Wait 128 PCLK to compare next channel. */
} COMP_RoundRobinPeriod_Type;

/*!
 * @brief COMP round robin channel Type.
 */
typedef enum
{
    COMP_RoundRobinChnGroup_Alt0 = 0x00, /*!< Compare the positive input 1 & 2.    */
    COMP_RoundRobinChnGroup_Alt1 = 0x01, /*!< Compare the positive input 1, 2 & 3. */
} COMP_RoundRobinChnGroup_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref COMP_Init() to initialize the COMP module.
 */
typedef struct
{
    COMP_Hysteresis_Type Hysteresis; /*!< Specify the hysteresis. */
    COMP_OutMux_Type     OutMux;     /*!< Specify the output. */
    COMP_InMux_Type      PosInMux;   /*!< Specify the positive input. */
    COMP_InMux_Type      InvInMux;   /*!< Specify the nagetive input. */
    COMP_Speed_Type      Speed;      /*!< Specify the compare speed. */
} COMP_Init_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref COMP_EnableExtVrefConf() to enable the COMP ext vref.
 */
typedef struct
{
    COMP_ExtVrefSource_Type VrefSource; /*!< Specify the Vref source.  */
    COMP_ExtVrefVolt_Type   Volt;       /*!< Specify the Vref voltage. */
} COMP_ExtVrefConf_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref COMP_EnableRoundRobinConf() to enable the COMP round robin.
 */
typedef struct
{
    COMP_RoundRobinPeriod_Type   Period;         /*!< Specify the round robin period.  */
    bool                         EnableInvInFix; /*!< Specify the nagetive input fix.  */
    COMP_RoundRobinChnGroup_Type ChnGroup;       /*!< Specify the round robin channel. */
} COMP_RoundRobinConf_Type;

/*!
 * @brief Initialize the COMP module.
 *
 * @param COMPx         COMP instance.
 * @param channel       COMP channel.
 * @param init          Pointer to the initialization structure. See to @ref COMP_Init_Type.
 * @return              None.
 */
void COMP_Init(COMP_Type * COMPx, uint32_t channel, COMP_Init_Type * init);

/*!
 * @brief Enable the COMP module.
 *
 * @param COMPx         COMP instance.
 * @param channel       COMP channel.
 * @param enable        'true' to enable the module, 'false' to disable the module.
 * @return              None.
 */
void COMP_Enable(COMP_Type * COMPx, uint32_t channel, bool enable);

/*!
 * @brief Keep the COMP settings not changed.
 *
 * @param COMPx         COMP instance.
 * @param channel       COMP channel.
 * @return              None.
 */
void COMP_Lock(COMP_Type * COMPx, uint32_t channel);

/*!
 * @brief Get comp output status.
 *
 * @param COMPx         COMP instance.
 * @param channel       COMP channel.
 * @return              output status.
 */
bool COMP_GetOutputStatus(COMP_Type * COMPx, uint32_t channel);

/*!
 * @brief Enable the COMP ext Vref.
 *
 * @param COMPx         COMP instance.
 * @param init          Pointer to the initialization structure. See to @ref COMP_ExtVrefConf_Type, if null, disable ext Vref.
 * @return              None.
 */
void COMP_EnableExtVrefConf(COMP_Type * COMPx, COMP_ExtVrefConf_Type * conf);

/*!
 * @brief Enable polling function.
 *
 * Enable round robin function, the comp channel can get positive input 1, 2 & 3 status.
 *
 * @param COMPx COMP instance.
 * @param channel COMP channel.
 * @param init Pointer to the initialization structure. See to @ref COMP_RoundRobinConf_Type, if null, disable round robin.
 * @return None.
 */
void COMP_EnableRoundRobinConf(COMP_Type * COMPx, uint32_t channel, COMP_RoundRobinConf_Type * conf);

/*!
 * @brief Get round robin output status.
 *
 * @param COMPx COMP instance.
 * @param channel COMP channel.
 * @param pos_in Positive input Mux.
 * @return Status of comp channel output level.
 */
bool COMP_GetRoundRobinOutStatus(COMP_Type * COMPx, uint32_t channel, COMP_InMux_Type pos_in);

/*!
 * @brief Enable COMP window control.
 *
 * @param COMPx COMP instance.
 * @param channel COMP channel.
 * @param enable 'true' to enable window control, 'false' to disable window control.
 * @return None.
 */
void COMP_EnableWindow(COMP_Type * COMPx, uint32_t channel, bool enable);

/*!
 * @brief Set the output filter.
 *
 * @param COMPx COMP instance.
 * @param channel COMP channel.
 * @param filter Output filter, See to @ref COMP_OutFilter_Type.
 * @return None.
 */
void COMP_SetOutFilter(COMP_Type * COMPx, uint32_t channel, COMP_OutFilter_Type filter);

/*!
 * @brief Enable the out invert.
 *
 * @param COMPx COMP instance.
 * @param channel COMP channel.
 * @param enable 'true' to enable the out invert, 'false' to disable the out invert.
 * @return None.
 */
void COMP_EnableOutInvert(COMP_Type * COMPx, uint32_t channel, bool enable);

/*!
 * @brief Enable the bypass output.
 *
 * @param COMPx COMP instance.
 * @param channel COMP channel.
 * @param enable 'true' to enable the bypass output, 'false' to disable the bypass output.
 * @return None.
 */
void COMP_EnableBypassOutput(COMP_Type * COMPx, uint32_t channel, bool enable);

/*!
 *@}
 */

#endif /* __HAL_COMP_H__ */
