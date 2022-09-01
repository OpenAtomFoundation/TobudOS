/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_FLEXCAN_H__
#define __HAL_FLEXCAN_H__

#include "hal_common.h"

/*!
 * @addtogroup FLEXCAN
 * @{
 */

/*!
 * @brief FLEXCAN driver version number.
 */
#define FLEXCAN_DRIVER_VERSION 0u /*!< FLEXCAN_0. */

/*!
 * @addtogroup FLEXCAN_GLOBAL_INT
 * @{
 */
#define FLEXCAN_INT_RWRN       FLEXCAN_CTRL1_RWRNMSK_MASK /*!< Tx warning interrupt switchers. */
#define FLEXCAN_INT_TWRN       FLEXCAN_CTRL1_TWRNMSK_MASK /*!< Rx warning interrupt switchers. */
#define FLEXCAN_INT_ERR        FLEXCAN_CTRL1_ERRMSK_MASK  /*!< Error interrupt switchers. */
#define FLEXCAN_INT_BOFF       FLEXCAN_CTRL1_BOFFMSK_MASK /*!< Bus off interrupt switchers. */
/*!
 * @}
 */

/*!
 * @addtogroup FLEXCAN_STATUS
 * @{
 */
#define FLEXCAN_STATUS_WAK                 (1 << 0u)  /*!< Wake up interrupt flag. */
#define FLEXCAN_STATUS_ERR                 (1 << 1u)  /*!< Error interrupt flag. */
#define FLEXCAN_STATUS_BOFF                (1 << 2u)  /*!< Bus off interrupt flag. */
#define FLEXCAN_STATUS_RX                  (1 << 3u)  /*!< Reception state flag. */
#define FLEXCAN_STATUS_FLTCONF             (0 << 4u)  /*!< Fault confinement state flag. */
#define FLEXCAN_STATUS_TX                  (1 << 6u)  /*!< Transmission state flag. */
#define FLEXCAN_STATUS_IDLE                (1 << 7u)  /*!< Idele state flag. */
#define FLEXCAN_STATUS_RXWRN               (1 << 8u)  /*!< Rx error warning flag. */
#define FLEXCAN_STATUS_TXWRN               (1 << 9u)  /*!< Tx error warning flag. */
#define FLEXCAN_STATUS_STFERR              (1 << 10u) /*!< Stuffing error flag. */
#define FLEXCAN_STATUS_FMRERR              (1 << 11u) /*!< Form error flag. */
#define FLEXCAN_STATUS_CRCERR              (1 << 12u) /*!< CRC error flag. */
#define FLEXCAN_STATUS_ACKERR              (1 << 13u) /*!< ACK error flag. */
#define FLEXCAN_STATUS_BIT0ERR             (1 << 14u) /*!< Bit 0 error flag. */
#define FLEXCAN_STATUS_BIT1ERR             (1 << 15u) /*!< Bit 1 error flag. */
#define FLEXCAN_STATUS_RWRN                (1 << 16u) /*!< Rx warning interrupt flag. */
#define FLEXCAN_STATUS_TWRN                (1 << 17u) /*!< Tx warning interrupt flag. */
#define FLEXCAN_STATUS_SYNCH               (1 << 18u) /*!< Synchronization status flag. */
#define FLEXCAN_STATUS_BOFFDONE            (1 << 19u) /*!< Bus off done interrupt flag. */
#define FLEXCAN_STATUS_ERROVR              (1 << 21u) /*!< Error Overrun interrupt flag. */
/*!
 * @}
 */

/*!
 * @addtogroup FLEXCAN_CHANNEL_SELECTION
 * @{
 */
#define FLEXCAN_CHN_NUM    (16u) /*!< The total number of FLEXCAN channel. */
#define FLEXCAN_CHN_0      (0u)  /*!< The selection of FLEXCAN MB Channel 0. */
#define FLEXCAN_CHN_1      (1u)  /*!< The selection of FLEXCAN MB Channel 1. */
#define FLEXCAN_CHN_2      (2u)  /*!< The selection of FLEXCAN MB Channel 2. */
#define FLEXCAN_CHN_3      (3u)  /*!< The selection of FLEXCAN MB Channel 3. */
#define FLEXCAN_CHN_4      (4u)  /*!< The selection of FLEXCAN MB Channel 4. */
#define FLEXCAN_CHN_5      (5u)  /*!< The selection of FLEXCAN MB Channel 5. */
#define FLEXCAN_CHN_6      (6u)  /*!< The selection of FLEXCAN MB Channel 6. */
#define FLEXCAN_CHN_7      (7u)  /*!< The selection of FLEXCAN MB Channel 7. */
#define FLEXCAN_CHN_8      (8u)  /*!< The selection of FLEXCAN MB Channel 8. */
#define FLEXCAN_CHN_9      (9u)  /*!< The selection of FLEXCAN MB Channel 9. */
#define FLEXCAN_CHN_10     (10u) /*!< The selection of FLEXCAN MB Channel 10. */
#define FLEXCAN_CHN_11     (11u) /*!< The selection of FLEXCAN MB Channel 11. */
#define FLEXCAN_CHN_12     (12u) /*!< The selection of FLEXCAN MB Channel 12. */
#define FLEXCAN_CHN_13     (13u) /*!< The selection of FLEXCAN MB Channel 13. */
#define FLEXCAN_CHN_14     (14u) /*!< The selection of FLEXCAN MB Channel 14. */
#define FLEXCAN_CHN_15     (15u) /*!< The selection of FLEXCAN MB Channel 15. */
/*!
 *@}
 */

/*!
 * @addtogroup FLEXCAN_MB_INT
 * @{
 */
#define FLEXCAN_INT_MB_0        (1u << 0u)   /*!< Individual Mb 0 interrupt and clear fifo switcher. */
#define FLEXCAN_INT_MB_1        (1u << 1u)   /*!< Individual Mb 1 interrupt switcher. */
#define FLEXCAN_INT_MB_2        (1u << 2u)   /*!< Individual Mb 2 interrupt switcher. */
#define FLEXCAN_INT_MB_3        (1u << 3u)   /*!< Individual Mb 3 interrupt switcher. */
#define FLEXCAN_INT_MB_4        (1u << 4u)   /*!< Individual Mb 4 interrupt switcher. */
#define FLEXCAN_INT_MB_5        (1u << 5u)   /*!< Individual Mb 5 interrupt and Rx fifo frame available switcher. */
#define FLEXCAN_INT_MB_6        (1u << 6u)   /*!< Individual Mb 6 interrupt and Rx fifo warning interrupt switcher. */
#define FLEXCAN_INT_MB_7        (1u << 7u)   /*!< Individual Mb 7 interrupt and Rx fifo overflow interrupt switcher. */
#define FLEXCAN_INT_MB_8        (1u << 8u)   /*!< Individual Mb 8 interrupt switcher. */
#define FLEXCAN_INT_MB_9        (1u << 9u)   /*!< Individual Mb 9 interrupt switcher. */
#define FLEXCAN_INT_MB_10       (1u << 10u)  /*!< Individual Mb 10 interrupt switcher. */
#define FLEXCAN_INT_MB_11       (1u << 11u)  /*!< Individual Mb 11 interrupt switcher. */
#define FLEXCAN_INT_MB_12       (1u << 12u)  /*!< Individual Mb 12 interrupt switcher. */
#define FLEXCAN_INT_MB_13       (1u << 13u)  /*!< Individual Mb 13 interrupt switcher. */
#define FLEXCAN_INT_MB_14       (1u << 14u)  /*!< Individual Mb 14 interrupt switcher. */
#define FLEXCAN_INT_MB_15       (1u << 15u)  /*!< Individual Mb 15 interrupt switcher. */
/*!
 * @}
 */

/*!
 * @addtogroup FLEXCAN_MB_STATUS
 * @{
 */
#define FLEXCAN_STATUS_MB_0        (1u << 0u)   /*!< Individual Mb 0 interrupt and clear fifo switcher. */
#define FLEXCAN_STATUS_MB_1        (1u << 1u)   /*!< Individual Mb 1 interrupt switcher. */
#define FLEXCAN_STATUS_MB_2        (1u << 2u)   /*!< Individual Mb 2 interrupt switcher. */
#define FLEXCAN_STATUS_MB_3        (1u << 3u)   /*!< Individual Mb 3 interrupt switcher. */
#define FLEXCAN_STATUS_MB_4        (1u << 4u)   /*!< Individual Mb 4 interrupt switcher. */
#define FLEXCAN_STATUS_MB_5        (1u << 5u)   /*!< Individual Mb 5 interrupt and Rx fifo frame available switcher. */
#define FLEXCAN_STATUS_MB_6        (1u << 6u)   /*!< Individual Mb 6 interrupt and Rx fifo warning interrupt switcher. */
#define FLEXCAN_STATUS_MB_7        (1u << 7u)   /*!< Individual Mb 7 interrupt and Rx fifo overflow interrupt switcher. */
#define FLEXCAN_STATUS_MB_8        (1u << 8u)   /*!< Individual Mb 8 interrupt switcher. */
#define FLEXCAN_STATUS_MB_9        (1u << 9u)   /*!< Individual Mb 9 interrupt switcher. */
#define FLEXCAN_STATUS_MB_10       (1u << 10u)  /*!< Individual Mb 10 interrupt switcher. */
#define FLEXCAN_STATUS_MB_11       (1u << 11u)  /*!< Individual Mb 11 interrupt switcher. */
#define FLEXCAN_STATUS_MB_12       (1u << 12u)  /*!< Individual Mb 12 interrupt switcher. */
#define FLEXCAN_STATUS_MB_13       (1u << 13u)  /*!< Individual Mb 13 interrupt switcher. */
#define FLEXCAN_STATUS_MB_14       (1u << 14u)  /*!< Individual Mb 14 interrupt switcher. */
#define FLEXCAN_STATUS_MB_15       (1u << 15u)  /*!< Individual Mb 15 interrupt switcher. */
/*!
 * @}
 */

/*!
 * @brief FLEXCAN clock source type.
 */
typedef enum
{
    FLEXCAN_ClockSource_Osc = 0u, /*!< CAN Protocol Engine will use the Oscillator clock. */
    FLEXCAN_ClockSource_Periph = 1u, /*!< CAN Protocol Engine will use the Peripheral clock.*/
} FLEXCAN_ClockSource_Type;

/*!
 * @brief FLEXCAN self wake up from bus off source.
 */
typedef enum
{
    FLEXCAN_SelfWakeUp_Disabled     = 0u, /*!< Disable self wake up, FLEXCAN can only back from Stop mode when its request is cleared. */
    FLEXCAN_SelfWakeUp_BypassFilter = 1u, /*!< FLEXCAN can use the unfiltered Rx input to detect events on CAN bus then back from Stop mode. */
    FLEXCAN_SelfWakeUp_WithFilter   = 2u, /*!< FLEXCAN can use the filtered Rx input to detect events on CAN bus then back from Stop mode. */
} FLEXCAN_SelfWakeUp_Type;

/*!
 * @brief FLEXCAN function mode.
 */
typedef enum
{
    FLEXCAN_WorkMode_Normal       = 0u, /*!< Normal work mode. */
    FLEXCAN_WorkMode_LoopBack     = 1u, /*!< Loop-back working mode, transfer and receive its own frame. */
    FLEXCAN_WorkMode_ListenOnly   = 2u, /*!< Listen-only mode, no transfer. */
} FLEXCAN_WorkMode_Type;

/*!
 * @brief FLEXCAN Rx message buffer filter type.
 */
typedef enum
{
    FLEXCAN_Mask_Global     = 0u, /*!< Use global mask to filter all the received frame. */
    FLEXCAN_Mask_Individual = 1u, /*!< Use individual mask to filter the received frame. */
} FLEXCAN_Mask_Type;

/*!
 * @brief FLEXCAN Xfer Mb frame type switcher.
 */
typedef enum
{
    FLEXCAN_MbType_Data   = 0u, /*!< Data frames can be accepted and remote frames are rejected. */
    FLEXCAN_MbType_Remote = 1u, /*!< Data frames can be rejected and remote frames are accepted. */
} FLEXCAN_MbType_Type;

/*!
 * @brief FLEXCAN Xfer Mb frame format switcher.
 */
typedef enum
{
    FLEXCAN_MbFormat_Standard = 0u, /*!< Only Standard format frames can be accepted. */
    FLEXCAN_MbFormat_Extended = 1u, /*!< Only Extended format frames can be accepted. */
} FLEXCAN_MbFormat_Type;

/*!
 * @brief FLEXCAN Rx fifo ID filter type.
 */
typedef enum
{
    FLEXCAN_FifoIdFilterFormat_A = 0u, /*!< Format A: One full ID (standard and extended) per ID Filter Table element. */
    FLEXCAN_FifoIdFilterFormat_B = 1u, /*!< Format B: Two full standard IDs or two partial 14-bit (standard and extended) IDs per ID Filter Table element.*/
    FLEXCAN_FifoIdFilterFormat_C = 2u, /*!< Format C: Four partial 8-bit Standard IDs per ID Filter Table element. */
    FLEXCAN_FifoIdFilterFormat_D = 3u, /*!< Format D: All frames rejected. */
} FLEXCAN_FifoIdFilterFormat_Type;

/*!
 * @brief FLEXCAN fifo priority type.
 */
typedef enum
{
    FLEXCAN_FifoPriority_MbFirst   = 0u, /*!< Mactching process start with message buffer. */
    FLEXCAN_FifoPriority_FifoFirst = 1u, /*!< Mactching process start with fifo. */
}FLEXCAN_FifoPriority_Type;

/*!
 * @brief FLEXCAN Xfer MB frame code switcher.
 */
typedef enum
{
    FLEXCAN_MbCode_RxInactive     = 0u,  /*!< Code for MB being not active. */
    FLEXCAN_MbCode_RxFull         = 2u,  /*!< Code for MB being full. */
    FLEXCAN_MbCode_RxEmpty        = 4u,  /*!< Code for MB being active and empty. */
    FLEXCAN_MbCode_RxOverrun      = 6u,  /*!< Code for MB being over written without accessing the received frame. */
    FLEXCAN_MbCode_TxInactive     = 8u,  /*!< Code for data response for Tx inactive. */
    FLEXCAN_MbCode_TxAbort        = 9u,  /*!< Code for Tx abort after transmission. */
    FLEXCAN_MbCode_RxRanswer      = 10u, /*!< Code for Rx waiting for remote frame. */
    FLEXCAN_MbCode_TxDataOrRemote = 12u, /*!< Code for data frame or remote frame transmission. */
    FLEXCAN_MbCode_TxTanswer      = 14u, /*!< Code for data response for remote frame. */
    FLEXCAN_MbCode_RxBusy         = 15u, /*!< Code for Rx updating MB. */
} FLEXCAN_MbCode_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref FLEXCAN_SetTimingConf() to initialize the time configuration.
 */
typedef struct
{
    bool EnableExtendedTime;  /*!< If enable, the setting time range can be broader. */
    uint32_t PhaSegLen1;      /*!< Phase segment 1. Compensate for edge phase errors and may be lengthened during resynchronization.*/
    uint32_t PhaSegLen2;      /*!< Phase segment 2. Compensate for edge phase errors and may be lengthened during resynchronization.*/
    uint32_t PropSegLen;      /*!< Propagation segment. Compensate for signal delays across the network.*/
    uint32_t JumpWidth;       /*!< Resynchronize jump width. */
    uint32_t PreDiv;          /*!< The divider for FLEXCAN clock source. */
} FLEXCAN_TimConf_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref FLEXCAN_Init() to initialize the general setting of FLEXCAN.
 */
typedef struct
{
    uint8_t MaxXferNum;                   /*!< Max number of message buffer to be used. */
    uint32_t BaudRate;                    /*!< Data bit per second when using FLEXCAN for transmision and reception. */
    uint32_t ClockFreqHz;                 /*!< Clock source frequency. */
    FLEXCAN_ClockSource_Type ClockSource; /*!< Clock source selection. */
    FLEXCAN_SelfWakeUp_Type  SelfWakeUp;  /*!< Stop mode self wake up source. */
    FLEXCAN_WorkMode_Type WorkMode;       /*!< FLEXCAN function mode. */
    FLEXCAN_Mask_Type Mask;               /*!< Filter work range for filtering the received frames. */
    FLEXCAN_TimConf_Type * TimConf;       /*!< FLEXCAN timer and time synchronization setup. */
    bool EnableSelfReception;             /*!< Whether to receive frames sent by FLEXCAN itself. */
    bool EnableTimerSync;                 /*!< Refresh the timer every frame reception. */
} FLEXCAN_Init_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref FLEXCAN_SetGlobalMbMaskConf() to set the mask for buffer.
 */
typedef struct
{
    FLEXCAN_MbType_Type MbType; /*!< Data frame or Remote frame switcher. */
    FLEXCAN_MbFormat_Type MbFormat; /*!< Extended or standard id switcher. */
    uint32_t IdMask; /*!< Id mask. */
} FLEXCAN_RxMbMaskConf_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref FLEXCAN_SetRxFifoGlobalMaskConf() to set the conf for fifo mask filter.
 */
typedef struct
{
    FLEXCAN_MbType_Type MbType; /*!< Data frame or Remote frame switcher. */
    FLEXCAN_MbFormat_Type MbFormat; /*!< Extended or standard id switcher. */
    FLEXCAN_FifoIdFilterFormat_Type FilterFormat; /*!< fifo filter format. */
    union
    {
        uint32_t RxIdA;    /*!< The fifo Id setting for filter format A. */
        uint16_t RxIdB[2]; /*!< The fifo Id setting for filter format B. */
        uint8_t  RxIdC[4]; /*!< The fifo Id setting for filter format C. */
    };
} FLEXCAN_RxFifoMaskConf_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref FLEXCAN_SetRxMb() to set the mask for buffer.
 */
typedef struct
{
    FLEXCAN_MbType_Type MbType; /*!< Data frame or Remote frame switcher. */
    FLEXCAN_MbFormat_Type MbFormat; /*!< Extended or standard id switcher. */
    uint32_t Id; /*!< Id value. */
} FLEXCAN_RxMbConf_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref FLEXCAN_EnableRxFifo() to initialize the fifo setting.
 */
typedef struct
{
    FLEXCAN_FifoIdFilterFormat_Type FilterFormat; /*!< fifo filter format which will decide how to filter the fifo reception. */
    uint32_t IdFilterNum; /*!< The fifo filter element num. */
    uint32_t * IdFilterTable; /*!< Filter array to be set for Rx fifo. */
    FLEXCAN_FifoPriority_Type priority; /*!< Enable matching process start with fifo. */
} FLEXCAN_RxFifoConf_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref FLEXCAN_WriteTxMb() to set the mask for buffer.
 */
typedef struct
{
    struct
    {
        uint32_t TIMESTAMP  : 16;   /*!< Free-running counter time stamp. */
        uint32_t LENGTH     : 4;    /*!< Length of Data in Bytes. */
        uint32_t TYPE       : 1;    /*!< Frame data type or remote type. */
        uint32_t FORMAT     : 1;    /*!< Frame extended format or standard format. */
        uint32_t RESERVED_0 : 1;    /*!< Reservation. */
        uint32_t IDHIT      : 9;    /*!< Id filter number hit by fifo. */
    };
    struct
    {
        uint32_t ID :29;            /*!< Frame Identifier. */
        uint32_t PRIORITY: 3;       /*!< Local priority. */
    };
    union
    {
        struct
        {
            uint32_t WORD0; /*!< CAN Frame payload word0. */
            uint32_t WORD1; /*!< CAN Frame payload word1. */
        };
        struct
        {
            /* The sequence refers to the little-endian-storage and big-endian transfer. */
            uint8_t BYTE3; /*!< CAN Frame payload byte3. */
            uint8_t BYTE2; /*!< CAN Frame payload byte2. */
            uint8_t BYTE1; /*!< CAN Frame payload byte1. */
            uint8_t BYTE0; /*!< CAN Frame payload byte0. */
            uint8_t BYTE7; /*!< CAN Frame payload byte7. */
            uint8_t BYTE6; /*!< CAN Frame payload byte6. */
            uint8_t BYTE5; /*!< CAN Frame payload byte5. */
            uint8_t BYTE4; /*!< CAN Frame payload byte4. */
        };
    };
} FLEXCAN_Mb_Type;

/*!
 * @brief Enable the FLEXCAN module.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param enable 'true' to enable the module, 'false' to disable the module.
 * @return None.
 */
void FLEXCAN_Enable(FLEXCAN_Type * FLEXCANx, bool enable);

/*!
 * @brief Enable soft reset of FLEXCAN registers.
 *
 * This will reset most part of the FLEXCAN registers.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @return None.
 */
void FLEXCAN_DoSoftReset(FLEXCAN_Type * FLEXCANx);

/*!
 * @brief Set up the FLEXCAN module with the indicated setting.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param init Pointer to the initialization structure. See to @ref FLEXCAN_Init_Type.
 * @return 'true' to setup the FLEXCAN module successfully, false to fail to set the baudrate wrongly.
 */
bool FLEXCAN_Init(FLEXCAN_Type * FLEXCANx, FLEXCAN_Init_Type * init);

/*!
 * @brief Set up the FLEXCAN timing module with the indicated setting.
 *
 * This would work when people want to change the boadrate after setting up the module.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param conf Pointer to the initialization structure. See to @ref FLEXCAN_TimConf_Type.
 * @return None.
 */
void FLEXCAN_SetTimingConf(FLEXCAN_Type * FLEXCANx, FLEXCAN_TimConf_Type * conf);

/*!
 * @brief Enter freeze mode for configuration.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param enable 'true' to enable the module, 'false' to disable the module.
 * @return None.
 */
void FLEXCAN_EnableFreezeMode(FLEXCAN_Type * FLEXCANx, bool enable);

/*!
 * @brief Set the Rx message buffer mask globally.
 *
 * Setting should be done after entering Freeze Mode.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param conf Indicated mask and Mb frame type and format to be set for 16 message buffer globally. See to @ref FLEXCAN_RxMbMaskConf_Type.
 * @return None.
 */
void FLEXCAN_SetGlobalMbMaskConf(FLEXCAN_Type * FLEXCANx, FLEXCAN_RxMbMaskConf_Type * conf);

/*!
 * @brief Set the Rx fifo mask globally.
 *
 * Setting should be done after entering Freeze Mode.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param mask Indicated mask to be set for 16 message buffer globally. See to @ref FLEXCAN_RxFifoMaskConf_Type.
 * @return None.
 */
void FLEXCAN_SetRxFifoGlobalMaskConf(FLEXCAN_Type * FLEXCANx, FLEXCAN_RxFifoMaskConf_Type * mask);

/*!
 * @brief Set Rx message buffer mask individually.
 *
 * Setting should be done after entering Freeze Mode.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param channel The Mb channel to be masked indicatedly. See to @ref FLEXCAN_CHANNEL_SELECTION.
 * @param mask Indicated mask to be set for 16 message buffer globally. See to @ref FLEXCAN_RxMbMaskConf_Type.
 * @return None.
 */
void FLEXCAN_SetRxMbIndividualMaskConf(FLEXCAN_Type * FLEXCANx, uint32_t channel, FLEXCAN_RxMbMaskConf_Type * mask);

/*!
 * @brief Clear the indicated MB.
 *
 * Clear the previous information in the buffer.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param channel The Mb channel to be masked indicatedly. See to @ref FLEXCAN_CHANNEL_SELECTION.
 * @return None.
 */
void FLEXCAN_ResetMb(FLEXCAN_Type * FLEXCANx, uint32_t channel);

/*!
 * @brief Clear message buffer with value 0.
 *
 * Clear the previous information in the buffer.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param channel The Mb channel to be masked indicatedly. See to @ref FLEXCAN_CHANNEL_SELECTION.
 * @param code The code to be set in Mb channel. See to @ref FLEXCAN_MbCode_Type.
 * @return None.
 */
void FLEXCAN_SetMbCode(FLEXCAN_Type * FLEXCANx, uint32_t channel, FLEXCAN_MbCode_Type code);

/*!
 * @brief Set Rx message buffer before reception.
 *
 * Prepare the type, format and id for Rx receiving.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param channel The Mb channel to be masked indicatedly. See to @ref FLEXCAN_CHANNEL_SELECTION.
 * @param conf Configuration for Rx Mb receiving. See to @ref FLEXCAN_RxMbConf_Type.
 * @return None.
 */
void FLEXCAN_SetRxMb(FLEXCAN_Type * FLEXCANx, uint32_t channel, FLEXCAN_RxMbConf_Type * conf);

/*!
 * @brief Setup the Rx fifo setting.
 *
 * Need to be done under freeze mode.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param conf Pointer to the configuration structure. See to @ref FLEXCAN_RxFifoConf_Type.
 * @return 'true' to enbale the Rx fifo successfully, 'false' to fail for setting the filter number wrongly.
 */
bool FLEXCAN_EnableRxFifo(FLEXCAN_Type * FLEXCANx, FLEXCAN_RxFifoConf_Type * conf);

/*!
 * @brief Enable global FLEXCAN interrupts.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param interrupts Global interrupts to be set simultaneously. See to @ref FLEXCAN_GLOBAL_INT.
 * @param enable 'true' to enable the interrupts, 'false' to disable the interrupts.
 * @return None.
 */
void FLEXCAN_EnableInterrupts(FLEXCAN_Type * FLEXCANx, uint32_t interrupts, bool enable);

/*!
 * @brief Get the error status and global status of FLEXCAN.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @return FLEXCAN status. See to @ref FLEXCAN_STATUS.
 */
uint32_t FLEXCAN_GetStatus(FLEXCAN_Type * FLEXCANx);

/*!
 * @brief Get the error status and global status of FLEXCAN.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param flags Flags to be cleared. See to @ref FLEXCAN_STATUS.
 * @return None.
 */
void FLEXCAN_ClearStatus(FLEXCAN_Type * FLEXCANx, uint32_t flags);

/*!
 * @brief Enable FLEXCAN individual message buffer interrupts.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param interrupts Global interrupts to be set simultaneously. See to @ref FLEXCAN_MB_INT.
 * @param enable 'true' to enable the interrupts, 'false' to disable the interrupts.
 * @return None.
 */
void FLEXCAN_EnableMbInterrupts(FLEXCAN_Type * FLEXCANx, uint32_t interrupts, bool enable);

/*!
 * @brief Get the buffer status or fifo status of FLEXCAN.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @return FLEXCAN buffer status. See to @ref FLEXCAN_MB_STATUS.
 */
uint32_t FLEXCAN_GetMbStatus(FLEXCAN_Type * FLEXCANx);

/*!
 * @brief Get the buffer status or fifo status of FLEXCAN.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param mbs FLEXCAN buffer status to be cleared. See to @ref FLEXCAN_MB_STATUS.
 * @return None.
 */
void FLEXCAN_ClearMbStatus(FLEXCAN_Type * FLEXCANx, uint32_t mbs);

/*!
 * @brief Enable Fifo DMA of FLEXCAN fifo.
 *
 * Should be done under freeze mode.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param enable 'true' to enable the DMA, 'false' to disable the DMA.
 * @return None.
 */
void FLEXCAN_EnableFifoDMA(FLEXCAN_Type * FLEXCANx, bool enable);

/*!
 * @brief Get the Tx error counter value.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @return Tx error counter value.
 */
uint32_t FLEXCAN_GetTxError(FLEXCAN_Type * FLEXCANx);

/*!
 * @brief Get the Rx error counter value.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @return Rx error counter value.
 */
uint32_t FLEXCAN_GetRxError(FLEXCAN_Type * FLEXCANx);

/*!
 * @brief Get the fifo address.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @return fifo address of the FLEXCAN.
 */
uint32_t FLEXCAN_GetFifoAddr(FLEXCAN_Type * FLEXCANx);

/*!
 * @brief Write the Mb frame into the indicated Mb channel.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param channel The indicated FLEXCAN Mb channel.
 * @param mb The prepared Mb for Tx. See to @ref FLEXCAN_Mb_Type.
 * @return 'true' to put the Mb frame successfully, 'false' to fail.
 */
bool FLEXCAN_WriteTxMb(FLEXCAN_Type * FLEXCANx, uint32_t channel, FLEXCAN_Mb_Type * mb);

/*!
 * @brief Read the Mb frame from the indicated Mb channel.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param channel The indicated FLEXCAN Mb channel.
 * @param mb The Mb for Rx. See to @ref FLEXCAN_Mb_Type.
 * @return 'true' to get the Mb frame successfully, 'false' to fail.
 */
bool FLEXCAN_ReadRxMb(FLEXCAN_Type * FLEXCANx, uint32_t channel, FLEXCAN_Mb_Type * mb);

/*!
 * @brief Read the fifo Mb frame from fifo.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param mb Pointer to the Mb structure. See to @ref FLEXCAN_Mb_Type.
 * @return 'true' to get the Mb frame successfully, 'false' to fail.
 */
bool FLEXCAN_ReadRxFifo(FLEXCAN_Type * FLEXCANx, FLEXCAN_Mb_Type * mb);

/*!
 * @brief Get the number of buffer corresponding to the latest transmitted crc.
 *
 * @param FLEXCANx FLEXCAN instance.
 * @param channel Channel match the latest transmission crc value.
 * @return Latest transmission crc value.
 */
uint32_t FLEXCAN_GetMatchCrcValue(FLEXCAN_Type * FLEXCANx, uint32_t * channel);

/*!
 *@}
 */

#endif /* __HAL_FLEXCAN_H__ */
