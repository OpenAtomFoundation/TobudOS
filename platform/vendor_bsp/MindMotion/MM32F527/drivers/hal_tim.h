/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_TIM_H__
#define __HAL_TIM_H__

#include "hal_common.h"

/*!
 * @addtogroup TIM
 * @{
 */

/*!
 * @brief TIM driver version number.
 */
#define TIM_DRIVER_VERSION 1u /*!< TIM_1. */

/*!
 * @addtogroup TIM_CHANNEL_SELECTION
 * @{
 */
#define TIM_CHN_NUM    (5u) /*!< The total number of TIM channel. */
#define TIM_CHN_1      (0u) /*!< The selection of TIM Channel 1. */
#define TIM_CHN_2      (1u) /*!< The selection of TIM Channel 2. */
#define TIM_CHN_3      (2u) /*!< The selection of TIM Channel 3. */
#define TIM_CHN_4      (3u) /*!< The selection of TIM Channel 4. */
#define TIM_CHN_5      (4u) /*!< The selection of TIM Channel 5. */
/*!
 *@}
 */

/*!
 * @addtogroup TIM_INT
 * @brief Enable interrupt.
 * @{
 */
#define TIM_INT_UPDATE_PERIOD   (1u << 0u)  /*!< Timer update period interrupt switchers. */
#define TIM_INT_CHN1_EVENT      (1u << 1u)  /*!< Channel 1 event for output compare or input capture interrupt switchers. */
#define TIM_INT_CHN2_EVENT      (1u << 2u)  /*!< Channel 2 event for output compare or input capture interrupt switchers. */
#define TIM_INT_CHN3_EVENT      (1u << 3u)  /*!< Channel 3 event for output compare or input capture interrupt switchers. */
#define TIM_INT_CHN4_EVENT      (1u << 4u)  /*!< Channel 4 event for output compare or input capture interrupt switchers. */
#define TIM_INT_COMP_EVENT      (1u << 5u)  /*!< Channel complementary pwm commutation interrupt switchers. */
#define TIM_INT_TRGOUT_EVENT    (1u << 6u)  /*!< Trigger out event interrupt switchers. */
#define TIM_INT_BREAK_EVENT     (1u << 7u)  /*!< Break event interrupt switchers. */
#define TIM_INT_CHN5_EVENT      (1u << 16u) /*!< Channel 5 event for output compare or input capture interrupt switchers. */
/*!
 * @}
 */

/*!
 * @addtogroup TIM_DMA
 * @brief Enable DMA.
 * @{
 */
#define TIM_DMA_UPDATE_PERIOD    (1u << 8u)  /*!< Timer update period DMA switchers. */
#define TIM_DMA_CHN1_EVENT       (1u << 9u)  /*!< Channel 1 event for output compare or input capture DMA switchers. */
#define TIM_DMA_CHN2_EVENT       (1u << 10u) /*!< Channel 2 event for output compare or input capture DMA switchers. */
#define TIM_DMA_CHN3_EVENT       (1u << 11u) /*!< Channel 3 event for output compare or input capture DMA switchers. */
#define TIM_DMA_CHN4_EVENT       (1u << 12u) /*!< Channel 4 event for output compare or input capture DMA switchers. */
#define TIM_DMA_COMP_EVENT       (1u << 13u) /*!< Channel complementary pwm commutation interrupt switchers. */
#define TIM_DMA_TRGOUT_EVENT     (1u << 14u) /*!< Trigger out event DMA switchers. */
#define TIM_DMA_CHN5_EVENT       (1u << 17u) /*!< Channel 5 event for output compare or input capture DMA switchers. */
/*!
 * @}
 */

/*!
 * @addtogroup TIM_INT_STATUS
 * @brief Check interrupt status.
 * @{
 */
#define TIM_STATUS_UPDATE_PERIOD    (1u << 0u) /*!< Timer update period interrupt flag. */
#define TIM_STATUS_CHN1_EVENT       (1u << 1u) /*!< Channel 1 event for output compare or input capture interrupt flag. */
#define TIM_STATUS_CHN2_EVENT       (1u << 2u) /*!< Channel 2 event for output compare or input capture interrupt flag. */
#define TIM_STATUS_CHN3_EVENT       (1u << 3u) /*!< Channel 3 event for output compare or input capture interrupt flag. */
#define TIM_STATUS_CHN4_EVENT       (1u << 4u) /*!< Channel 4 event for output compare or input capture interrupt flag. */
#define TIM_STATUS_COMP_EVENT       (1u << 5u) /*!< Channel complementary pwm commutation interrupt flag. */
#define TIM_STATUS_TRGOUT_EVENT     (1u << 6u) /*!< Trigger out event interrupt flag. */
#define TIM_STATUS_BREAK_EVENT      (1u << 7u) /*!< Break event interrupt flag. */
#define TIM_STATUS_CHN1_OVER_EVENT  (1u << 9u) /*!< Channel 1 event for output overcompare or input overcapture interrupt flag. */
#define TIM_STATUS_CHN2_OVER_EVENT  (1u << 10u) /*!< Channel 2 event for output overcompare or input overcapture interrupt flag. */
#define TIM_STATUS_CHN3_OVER_EVENT  (1u << 11u) /*!< Channel 3 event for output overcompare or input overcapture interrupt flag. */
#define TIM_STATUS_CHN4_OVER_EVENT  (1u << 12u) /*!< Channel 4 event for output overcompare or input overcapture interrupt flag. */
#define TIM_STATUS_CHN5_EVENT       (1u << 16u) /*!< Channel 5 event for output compare or input capture interrupt flag. */
/*!
 * @}
 */

/*!
 * @addtogroup TIM_SWTRG
 * @brief Events to be triggered by software.
 * @{
 */
#define TIM_SWTRG_UPDATE_PERIOD   (1u << 0u) /*!< Timer update period software trigger. */
#define TIM_SWTRG_CHN1_EVENT      (1u << 1u) /*!< Channel 1 event for output compare or input capture software trigger. */
#define TIM_SWTRG_CHN2_EVENT      (1u << 2u) /*!< Channel 2 event for output compare or input capture software trigger. */
#define TIM_SWTRG_CHN3_EVENT      (1u << 3u) /*!< Channel 3 event for output compare or input capture software trigger. */
#define TIM_SWTRG_CHN4_EVENT      (1u << 4u) /*!< Channel 4 event for output compare or input capture software trigger. */
#define TIM_SWTRG_COMP_EVENT      (1u << 5u) /*!< Channel complementary pwm commutation software trigger. */
#define TIM_SWTRG_TRGOUT_EVENT    (1u << 6u) /*!< Trigger out event software trigger. */
#define TIM_SWTRG_BREAK_EVENT     (1u << 7u) /*!< Break event software trigger. */
#define TIM_SWTRG_CHN5_EVENT      (1u << 16u) /*!< Channel 5 event for output compare or input capture software trigger. */
/*!
 * @}
 */

/*!
 * @addtogroup TIM_BKEAK_IN
 * @brief Enable interrupt.
 * @{
 */
#define TIM_BKEAK_IN_MASK0   (1u << 5u)   /*!< TIM ADV break in source mask 0. */
#define TIM_BKEAK_IN_MASK1   (1u << 6u)   /*!< TIM ADV break in source mask 1. */
#define TIM_BKEAK_IN_MASK2   (1u << 7u)   /*!< TIM ADV break in source mask 2. */
#define TIM_BKEAK_IN_MASK3   (1u << 8u)   /*!< TIM ADV break in source mask 3. */
#define TIM_BKEAK_IN_MASK4   (1u << 9u)   /*!< TIM ADV break in source mask 4. */
#define TIM_BKEAK_IN_MASK5   (1u << 10u)  /*!< TIM ADV break in source mask 5. */
#define TIM_BKEAK_IN_MASK6   (1u << 11u)  /*!< TIM ADV break in source mask 6. */
#define TIM_BKEAK_IN_MASK7   (1u << 13u)  /*!< TIM ADV break in source mask 7. */
#define TIM_BKEAK_IN_MASK8   (1u << 13u)  /*!< TIM ADV break in source mask 8. */
#define TIM_BKEAK_IN_MASK9   (1u << 14u)  /*!< TIM ADV break in source mask 9. */
#define TIM_BKEAK_IN_MASK10  (1u << 15u)  /*!< TIM ADV break in source mask 10. */
#define TIM_BKEAK_IN_MASK11  (1u << 16u)  /*!< TIM ADV break in source mask 11. */
/*!
 * @}
 */

/*!
 * @brief Counter period working mode.
 */
typedef enum
{
    TIM_PeriodMode_Continuous = 0u, /*!< Counter works circularly once started. */
    TIM_PeriodMode_OneTimeRun = 1u, /*!< Counter only works one time once started. */
} TIM_PeriodMode_Type;

/*!
 * @brief Counter count direction.
 */
typedef enum
{
    TIM_CountMode_Increasing = 0u,     /*!< Counter counts incrementally from 0 to period and generates an update. */
    TIM_CountMode_Decreasing = 1u,     /*!< Counter counts decrementally from period to 0 and generates an update. */
    TIM_CountMode_CenterAligned1 = 2u, /*!< Counter counts center-aligned from 0 to period - 1 then from period to 0 and generates an update. */
    TIM_CountMode_CenterAligned2 = 3u, /*!< Counter counts center-aligned from 0 to period - 1 and generates an update then from period to 0. */
    TIM_CountMode_CenterAligned3 = 4u, /*!< Counter counts center-aligned from 0 to period - 1 and generates an update then from period to 0 and generates another update. */
} TIM_CountMode_Type;

/*!
 * @brief TIM output compare reference polairty event.
 */
typedef enum
{
    TIM_OutputCompareRefOut_None = 0u,        /*!< Reference will not be changed. */
    TIM_OutputCompareRefOut_HighOnMatch = 1u, /*!< Reference will be high when counter value equals channel value. */
    TIM_OutputCompareRefOut_LowOnMatch = 2u,  /*!< Reference will be low when counter value equals channel value. */
    TIM_OutputCompareRefOut_ToggleOnMatch = 3u, /*!< Reference will toggle. */
    TIM_OutputCompareRefOut_LowAlways = 4u,    /*!< Reference will always be low. */
    TIM_OutputCompareRefOut_HighAlways = 5u,     /*!< Reference will always be high. */
    TIM_OutputCompareRefOut_FallingEdgeOnMatch = 6u, /*!< PWM mode 1. */
    TIM_OutputCompareRefOut_RisingEdgeOnMatch = 7u,  /*!< PWM mode 2. */
} TIM_OutputCompareRefOut_Type;

/*!
 * @brief TIM input capture active edge selection and output compare active polarity selection.
 * | conf                    | TIM_PinPolarity_Rising | TIM_PinPolarity_Falling |
 * | ----------------------- | -------------------------- | --------------------------- |
 * | input capture           | detects rising edge        | detects falling edge        |
 * | output compare          | high polarity is active    | low polarity is active      |
 * | encoder mode            | input remains              | input reverses              |
 * | output under idle state | output high polarity       | output low polarity         |
 */
typedef enum
{
    TIM_PinPolarity_Disabled = 0u, /*!< Disable channel. */
    TIM_PinPolarity_Rising = 1u, /*!< See the table above. */
    TIM_PinPolarity_Falling = 2u, /*!< See the table above.*/
    TIM_PinPolarity_RisingOrFalling = 3u, /*!< For input capture, detect both the rising and falling edge. */
}TIM_PinPolarity_Type;

/*!
 * @brief TIM Input active times to evoke capture.
 */
typedef enum
{
    TIM_InputCaptureInDiv_OnEach1Capture = 0u, /*!< Every input call capture. */
    TIM_InputCaptureInDiv_OnEach2Capture = 1u, /*!< Every two input call capture. */
    TIM_InputCaptureInDiv_OnEach4Capture = 2u, /*!< Every four input call capture. */
    TIM_InputCaptureInDiv_OnEach8Capture = 3u, /*!< Every eight input call capture. */
} TIM_InputCaptureInDiv_Type;

/*!
 * @brief TIM Input sampling filter.
 */
typedef enum
{
    TIM_InputCaptureInFilter_Alt0  = 0u, /*!< Filter swicther 0. */
    TIM_InputCaptureInFilter_Alt1  = 1u, /*!< Filter swicther 1. */
    TIM_InputCaptureInFilter_Alt2  = 2u, /*!< Filter swicther 2. */
    TIM_InputCaptureInFilter_Alt3  = 3u, /*!< Filter swicther 3. */
    TIM_InputCaptureInFilter_Alt4  = 4u, /*!< Filter swicther 4. */
    TIM_InputCaptureInFilter_Alt5  = 5u, /*!< Filter swicther 5. */
    TIM_InputCaptureInFilter_Alt6  = 6u, /*!< Filter swicther 6. */
    TIM_InputCaptureInFilter_Alt7  = 7u, /*!< Filter swicther 7. */
    TIM_InputCaptureInFilter_Alt8  = 8u, /*!< Filter swicther 8. */
    TIM_InputCaptureInFilter_Alt9  = 9u, /*!< Filter swicther 9. */
    TIM_InputCaptureInFilter_Alt10 = 10u, /*!< Filter swicther 10. */
    TIM_InputCaptureInFilter_Alt11 = 11u, /*!< Filter swicther 11. */
    TIM_InputCaptureInFilter_Alt12 = 12u, /*!< Filter swicther 12. */
    TIM_InputCaptureInFilter_Alt13 = 13u, /*!< Filter swicther 13. */
    TIM_InputCaptureInFilter_Alt14 = 14u, /*!< Filter swicther 14. */
    TIM_InputCaptureInFilter_Alt15 = 15u, /*!< Filter swicther 15. */
} TIM_InputCaptureInFilter_Type;

/*!
 * @brief TIM channel IO mode.
 */
typedef enum
{
    TIM_ChannelIOMode_Alt0 = 0u, /*!< Channel output. */
    TIM_ChannelIOMode_Alt1 = 1u, /*!< Channel input, capture signal from direct channel. */
    TIM_ChannelIOMode_Alt2 = 2u, /*!< Channel input, capture signal from indirect channel. */
    TIM_ChannelIOMode_Alt3 = 3u, /*!< Channel input, capture signal from trc. */

    TIM_ChannelIOMode_Out  = TIM_ChannelIOMode_Alt0, /*!< Channel output. */
    TIM_ChannelIOMode_In   = TIM_ChannelIOMode_Alt1, /*!< Channel input, capture signal from direct channel. */
} TIM_ChannelIOMode_Type;

/*!
 * @brief TIM external trigger input active edge selection.
 */
typedef enum
{
    TIM_ExtTriggerPinPolarity_Disabled = 0u, /*!< Disable external trigger input. */
    TIM_ExtTriggerPinPolarity_Rising = 1u,   /*!< Detect the rising edge from external trigger. */
    TIM_ExtTriggerPinPolarity_Falling = 2u,  /*!< Detect the falling edge from external trigger. */
}TIM_ExtTriggerPinPolarity_Type;

/*!
 * @brief TIM external trigger active input times to drive counter.
 */
typedef enum
{
    TIM_ExtTriggerInDiv_OnEach1Capture = 0u, /*!< Every input call capture. */
    TIM_ExtTriggerInDiv_OnEach2Capture = 1u, /*!< Every two input call capture. */
    TIM_ExtTriggerInDiv_OnEach4Capture = 2u, /*!< Every four input call capture. */
    TIM_ExtTriggerInDiv_OnEach8Capture = 3u, /*!< Every eight input call capture. */
} TIM_ExtTriggerInDiv_Type;

/*!
 * @brief TIM external trigger sampling filter.
 */
typedef enum
{
    TIM_ExtTriggerInFilter_Alt0  = 0u, /*!< Filter swicther 0. */
    TIM_ExtTriggerInFilter_Alt1  = 1u, /*!< Filter swicther 1. */
    TIM_ExtTriggerInFilter_Alt2  = 2u, /*!< Filter swicther 2. */
    TIM_ExtTriggerInFilter_Alt3  = 3u, /*!< Filter swicther 3. */
    TIM_ExtTriggerInFilter_Alt4  = 4u, /*!< Filter swicther 4. */
    TIM_ExtTriggerInFilter_Alt5  = 5u, /*!< Filter swicther 5. */
    TIM_ExtTriggerInFilter_Alt6  = 6u, /*!< Filter swicther 6. */
    TIM_ExtTriggerInFilter_Alt7  = 7u, /*!< Filter swicther 7. */
    TIM_ExtTriggerInFilter_Alt8  = 8u, /*!< Filter swicther 8. */
    TIM_ExtTriggerInFilter_Alt9  = 9u, /*!< Filter swicther 9. */
    TIM_ExtTriggerInFilter_Alt10 = 10u, /*!< Filter swicther 10. */
    TIM_ExtTriggerInFilter_Alt11 = 11u, /*!< Filter swicther 11. */
    TIM_ExtTriggerInFilter_Alt12 = 12u, /*!< Filter swicther 12. */
    TIM_ExtTriggerInFilter_Alt13 = 13u, /*!< Filter swicther 13. */
    TIM_ExtTriggerInFilter_Alt14 = 14u, /*!< Filter swicther 14. */
    TIM_ExtTriggerInFilter_Alt15 = 15u, /*!< Filter swicther 15. */
} TIM_ExtTriggerInFilter_Type;

/*!
 * @brief TIM Dead area / ETR / TI time step length during sampling.
 */
typedef enum
{
    TIM_ClockDiv_Alt0 = 0u, /*!< Dead area / ETR / TI step = clock source step. */
    TIM_ClockDiv_Alt1 = 1u, /*!< Dead area / ETR / TI step = 2 * clock source step. */
    TIM_ClockDiv_Alt2 = 2u, /*!< Dead area / ETR / TI step = 4 * clock source step. */
} TIM_ClockDiv_Type;

/*!
 * @brief TIM master mode output signal source.
 */
typedef enum
{
    TIM_MasterOut_Alt0 = 0u, /*!< Software update period trigger signal works as master output. */
    TIM_MasterOut_Alt1 = 1u, /*!< Counter enable signal works as master output. */
    TIM_MasterOut_Alt2 = 2u, /*!< Update period event works as master output. */
    TIM_MasterOut_Alt3 = 3u, /*!< Channel capture or compare event works as master output. */
    TIM_MasterOut_Alt4 = 4u, /*!< Reference signal of channel 1 works as master output. */
    TIM_MasterOut_Alt5 = 5u, /*!< Reference signal of channel 2 works as master output. */
    TIM_MasterOut_Alt6 = 6u, /*!< Reference signal of channel 3 works as master output. */
    TIM_MasterOut_Alt7 = 7u, /*!< Reference signal of channel 4 works as master output. */
} TIM_MasterOut_Type;

/*!
 * @brief TIM slave mode input object source.
 */
typedef enum
{
    TIM_SlaveIn_Alt0 = 0u, /*!< Internal trigger 0. */
    TIM_SlaveIn_Alt1 = 1u, /*!< Internal trigger 1. */
    TIM_SlaveIn_Alt2 = 2u, /*!< Internal trigger 2.*/
    TIM_SlaveIn_Alt3 = 3u, /*!< Internal trigger 3. */
    TIM_SlaveIn_Alt4 = 4u, /*!< Edge detector of timer input channel 1. */
    TIM_SlaveIn_Alt5 = 5u, /*!< Timer input channel 1. */
    TIM_SlaveIn_Alt6 = 6u, /*!< Timer input channel 2. */
    TIM_SlaveIn_Alt7 = 7u, /*!< External trigger input. */
} TIM_SlaveIn_Type;

/*!
 * @brief TIM slave mode counter response.
 */
typedef enum
{
    TIM_SlaveResp_Disabled = 0u, /*!< Disable the slave mode. */
    TIM_SlaveResp_Alt1 = 1u, /*!< Counting in encoder mode 1. */
    TIM_SlaveResp_Alt2 = 2u, /*!< Counting in encoder mode 2.*/
    TIM_SlaveResp_Alt3 = 3u, /*!< Counting in encoder mode 3. */
    TIM_SlaveResp_Alt4 = 4u, /*!< Reset counter. */
    TIM_SlaveResp_Alt5 = 5u, /*!< Counter treat the signal as a gate. */
    TIM_SlaveResp_Alt6 = 6u, /*!< Start the counter. */
    TIM_SlaveResp_Alt7 = 7u, /*!< Counter counts on rising edges. */
} TIM_SlaveResp_Type;

/*!
 * @brief TIM DMA Burst transfer base address.
 */
typedef enum
{
    TIM_DMABurstBaseAddr_Alt0 = 0u, /*!< Start from CR1 register. */
    TIM_DMABurstBaseAddr_Alt1 = 1u, /*!< Start from CR2 register. */
    TIM_DMABurstBaseAddr_Alt2 = 2u, /*!< Start from SMCR register. */
    TIM_DMABurstBaseAddr_Alt3 = 3u, /*!< Start from DIER register. */
    TIM_DMABurstBaseAddr_Alt4 = 4u, /*!< Start from SR register. */
    TIM_DMABurstBaseAddr_Alt5 = 5u, /*!< Start from EGR register. */
    TIM_DMABurstBaseAddr_Alt6 = 6u, /*!< Start from CCMR1 register. */
    TIM_DMABurstBaseAddr_Alt7 = 7u, /*!< Start from CCMR2 register. */
    TIM_DMABurstBaseAddr_Alt8 = 8u, /*!< Start from CCER register. */
    TIM_DMABurstBaseAddr_Alt9 = 9u, /*!< Start from CNT register. */
    TIM_DMABurstBaseAddr_Alt10 = 10u, /*!< Start from PSC register. */
    TIM_DMABurstBaseAddr_Alt11 = 11u, /*!< Start from ARR register. */
    TIM_DMABurstBaseAddr_Alt12 = 12u, /*!< Start from CCR1 register. */
    TIM_DMABurstBaseAddr_Alt13 = 13u, /*!< Start from CCR2 register. */
    TIM_DMABurstBaseAddr_Alt14 = 14u, /*!< Start from CCR3 register. */
    TIM_DMABurstBaseAddr_Alt15 = 15u, /*!< Start from CCR4 register. */
} TIM_DMABurstBaseAddr_Type;

/*!
 * @brief TIM DMA Burst transfer length.
 */
typedef enum
{
    TIM_DMABurstBaseLen_Alt0 = 0u, /*!< Sequentially visit 1 register from base register */
    TIM_DMABurstBaseLen_Alt1 = 1u, /*!< Sequentially visit 2 registers from base register */
    TIM_DMABurstBaseLen_Alt2 = 2u, /*!< Sequentially visit 3 registers from base register */
    TIM_DMABurstBaseLen_Alt3 = 3u, /*!< Sequentially visit 4 registers from base register */
    TIM_DMABurstBaseLen_Alt4 = 4u, /*!< Sequentially visit 5 registers from base register */
    TIM_DMABurstBaseLen_Alt5 = 5u, /*!< Sequentially visit 6 registers from base register */
    TIM_DMABurstBaseLen_Alt6 = 6u, /*!< Sequentially visit 7 registers from base register */
    TIM_DMABurstBaseLen_Alt7 = 7u, /*!< Sequentially visit 8 registers from base register */
    TIM_DMABurstBaseLen_Alt8 = 8u, /*!< Sequentially visit 9 registers from base register */
    TIM_DMABurstBaseLen_Alt9 = 9u, /*!< Sequentially visit 10 registers from base register */
    TIM_DMABurstBaseLen_Alt10 = 10u, /*!< Sequentially visit 11 registers from base register */
    TIM_DMABurstBaseLen_Alt11 = 11u, /*!< Sequentially visit 12 registers from base register */
    TIM_DMABurstBaseLen_Alt12 = 12u, /*!< Sequentially visit 13 registers from base register */
    TIM_DMABurstBaseLen_Alt13 = 13u, /*!< Sequentially visit 14 registers from base register */
    TIM_DMABurstBaseLen_Alt14 = 14u, /*!< Sequentially visit 15 registers from base register */
    TIM_DMABurstBaseLen_Alt15 = 15u, /*!< Sequentially visit 16 registers from base register */
    TIM_DMABurstBaseLen_Alt16 = 16u, /*!< Sequentially visit 17 registers from base register */
    TIM_DMABurstBaseLen_Alt17 = 17u, /*!< Sequentially visit 18 registers from base register */
} TIM_DMABurstBaseLen_Type;

/*!
 * @brief TIM encoder direction.
 */
typedef enum
{
    TIM_EncoderDirection_Forward  = 0u, /*!< Encoder rotates in forward direction. */
    TIM_EncoderDirection_Backward = 1u, /*!< Encoder rotates in backward direction. */
} TIM_EncoderDirection_Type;

/*!
 * @brief TIM break input sampling filter.
 */
typedef enum
{
    TIM_BreakInFilter_Alt0  = 0u,  /*!< Break In Filter swicther 0. */
    TIM_BreakInFilter_Alt1  = 1u,  /*!< Break In Filter swicther 1. */
    TIM_BreakInFilter_Alt2  = 2u,  /*!< Break In Filter swicther 2. */
    TIM_BreakInFilter_Alt3  = 3u,  /*!< Break In Filter swicther 3. */
    TIM_BreakInFilter_Alt4  = 4u,  /*!< Break In Filter swicther 4. */
    TIM_BreakInFilter_Alt5  = 5u,  /*!< Break In Filter swicther 5. */
    TIM_BreakInFilter_Alt6  = 6u,  /*!< Break In Filter swicther 6. */
    TIM_BreakInFilter_Alt7  = 7u,  /*!< Break In Filter swicther 7. */
    TIM_BreakInFilter_Alt8  = 8u,  /*!< Break In Filter swicther 8. */
    TIM_BreakInFilter_Alt9  = 9u,  /*!< Break In Filter swicther 9. */
    TIM_BreakInFilter_Alt10 = 10u, /*!< Break In Filter swicther 10. */
    TIM_BreakInFilter_Alt11 = 11u, /*!< Break In Filter swicther 11. */
    TIM_BreakInFilter_Alt12 = 12u, /*!< Break In Filter swicther 12. */
    TIM_BreakInFilter_Alt13 = 13u, /*!< Break In Filter swicther 13. */
    TIM_BreakInFilter_Alt14 = 14u, /*!< Break In Filter swicther 14. */
    TIM_BreakInFilter_Alt15 = 15u, /*!< Break In Filter swicther 15. */
    TIM_BreakInFilter_Disabled = 16u, /*!< Not using Break In Filter. */
} TIM_BreakInFilter_Type;

/*!
 * @brief TIM break input source switcher.
 */
typedef enum
{
    TIM_BreakInSource_Alt0  = 1u,  /*!< Break In Source swicther 0. */
    TIM_BreakInSource_Alt1  = 2u,  /*!< Break In Source swicther 1. */
} TIM_BreakInSource_Type;

/*!
 * @brief TIM lock level.
 */
typedef enum
{
    TIM_LockLevel_Disabled  = 0u, /*!< Unlock. */
    TIM_LockLevel_Alt0 = 1u, /*!< Lock level 1. */
    TIM_LockLevel_Alt1 = 2u, /*!< Lock level 2. */
    TIM_LockLevel_Alt2 = 3u, /*!< Lock level 3. */
} TIM_LockLevel_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref TIM_Init() to initialize the TIM module time base unit.
 */
typedef struct
{
    uint32_t ClockFreqHz;               /*!< Frequence of clock source for counter. */
    uint32_t StepFreqHz;                /*!< Step length value. Counter freq = ClockSourceFreqHz / (ClockSourceDiv+1). */
    uint32_t Period;                    /*!< Counter counting period length, from 0 to Period. */
    bool EnablePreloadPeriod;           /*!< Enable the preload of period value. If enable, it will be updated in next period, otherwise immediately. */
    TIM_PeriodMode_Type PeriodMode; /*!< Counter working period times. */
    TIM_CountMode_Type CountMode;  /*!< Counter count direction. */
} TIM_Init_Type;

/*!
 * @brief This type of structure instance is used to setup the output compare feature when calling the @ref TIM_EnableOutputCompare() to enable the TIM module output compare channel.
 */
typedef struct
{
    uint32_t ChannelValue; /*!< Value to be compared with counter. */
    bool EnableFastOutput; /*!< Enable fast response to the trigger input event. */
    bool EnablePreLoadChannelValue; /*!< Enable the preload of channel value. If enable, it will be updated in next period, otherwise immediately. */
    TIM_OutputCompareRefOut_Type RefOutMode; /*!< The reference output when counter matching the channel value. */
    bool ClearRefOutOnExtTrigger; /*!< External trigger high will clear the reference. */
    TIM_PinPolarity_Type PinPolarity; /*!< Setup the output pin's polarity. */
} TIM_OutputCompareConf_Type;

/*!
 * @brief This type of structure instance is used to setup the input capture feature when calling the @ref TIM_EnableInputCapture() to enable the TIM module input capture channel.
 */
typedef struct
{
    TIM_InputCaptureInDiv_Type    InDiv; /*!< The division of filtered input. */
    TIM_InputCaptureInFilter_Type InFilter; /*!< The filter sampling time for initial input. */
    TIM_PinPolarity_Type        PinPolarity; /*!< Setup the output pin's polarity. */
} TIM_InputCaptureConf_Type;

/*!
 * @brief This type of structure instance is used to setup the dead time feature when calling the @ref TIM_EnableDeadPeriod() to enable the TIM module dead time area set.
 * | DeadPeriodCoef | Real Dead Area Time                                 |
 * | ------------ | --------------------------------------------------- |
 * | 0xxxxxxx     | DeadPeriodCoef * DeadPeriodStep                         |
 * | 10xxxxxx     | ( 64 +  DeadPeriodCoef[5:0] ) * ( 2 * DeadPeriodStep )  |
 * | 110xxxxx     | ( 32 +  DeadPeriodCoef[4:0] ) * ( 8 * DeadPeriodStep ） |
 * | 111xxxxx     | ( 32 +  DeadPeriodCoef[4:0] ) * ( 16 * DeadPeriodStep ) |
 */
typedef struct
{
    uint8_t DeadPeriodCoef; /*!< The coefficient with bias to multiply with StepFreqHz to get the final dead area time. */
} TIM_DeadPeriodConf_Type;

/*!
 * @brief This type of structure instance is used to setup the master mode configuration when calling the @ref TIM_EnableMasterMode().
 */
typedef struct
{
    TIM_MasterOut_Type Out; /*!< Master mode output selection. */
    bool EnableSync; /*!< Enable sychronize with slave timers. */
} TIM_MasterModeConf_Type;

/*!
 * @brief This type of structure instance is used to setup the slave mode configuration when calling the @ref TIM_EnableSlaveMode().
 */
typedef struct
{
    TIM_SlaveIn_Type In; /*!< Input trigger selection for slave timer. */
    TIM_SlaveResp_Type Resp;  /*!< Slave timer counter response for its input trigger. */
} TIM_SlaveModeConf_Type;

/*!
 * @brief This type of structure instance is used to setup the external trigger input configuration when calling the @ref TIM_EnableExtTriggerIn().
 */
typedef struct
{
    TIM_ExtTriggerInDiv_Type    InDiv;     /*!< The division of filtered input. */
    TIM_ExtTriggerInFilter_Type InFilter;  /*!< The filter sampling time for initial input. */
    TIM_ExtTriggerPinPolarity_Type PinPolarity; /*!< Setup the output pin's polarity. */
} TIM_ExtTriggerInConf_Type;

/*!
 * @brief This type of structure instance is used to setup the dma burst configuration when calling the @ref TIM_EnableDMABurst().
 */
typedef struct
{
    TIM_DMABurstBaseAddr_Type    BaseAddr; /*!< The base address to start transfer. */
    TIM_DMABurstBaseLen_Type  Length;   /*!< Length of sequential registers DMA burst visits. */
} TIM_DMABurstConf_Type;

/*!
 * @brief This type of structure instance is used to setup the idle state output when calling the @ref TIM_EnableIdleOut().
 */
typedef struct
{
    TIM_PinPolarity_Type     PinPolarity; /*!< The output state of channel under idle mode. */
    TIM_PinPolarity_Type CompPinPolarity; /*!< The output state of complementary channel under idle mode. */
} TIM_IdleOut_Type;

/*!
 * @brief This type of structure instance is used to setup the break in configuration when calling the @ref TIM_EnableBreakIn().
 */
typedef struct
{
    TIM_PinPolarity_Type PinPolarity; /*!< The active edge of the break in signal. */
    bool AutoSwitchOutput; /*!< If input signal is inactive, switch the output compare at next update. */
    bool DirectIdleOutput; /*!< If enable, output the set idle state directly. */
    TIM_BreakInFilter_Type Filter; /*!< Break input filter type switcher. */
    uint32_t Source; /*!< Break input source mask. See to macro TIM_BKEAK_IN_MASKx. */
} TIM_BreakIn_Type;

/*!
 * @brief Set the timer's step for indicated TIM module.
 *
 * @param TIMx TIM instance.
 * @param init Pointer to the initialization structure. See to @ref TIM_Init_Type.
 * @return val 'true' to set the timer's step successfully, 'false' to fail because the invalid StepFreqHz.
 */
bool TIM_Init(TIM_Type * TIMx, TIM_Init_Type * init);

/*!
 * @brief Start counting.
 *
 * @param TIMx TIM instance.
 * @return None.
 */
void TIM_Start(TIM_Type * TIMx);

/*!
 * @brief Stop counting immediately.
 *
 * @param TIMx TIM instance.
 * @return None.
 */
void TIM_Stop(TIM_Type * TIMx);

/*!
 * @brief Get the indicated TIMx Counter value.
 *
 * @param TIMx TIM_Type instance.
 * @return The current value of the counter register.
 */
uint32_t TIM_GetCounterValue(TIM_Type * TIMx);

/*!
 * @brief Clear the indicated TIMx Counter.
 *
 * @param TIMx TIM_Type instance.
 * @return None.
 */
void TIM_ClearCounterValue(TIM_Type * TIMx);

/*!
 * @brief Enable the TIM interrupts.
 *
 * @param TIMx TIM_Type instance.
 * @param interrupts Interrupts to be enabled.See to @ref TIM_INT.
 * @param enable 'true' to enable the indicated interrupts or DMAs, 'false' to disable the indicated interrupts or DMAs.
 * @return None.
 */
void TIM_EnableInterrupts(TIM_Type * TIMx, uint32_t interrupts, bool enable);

/*!
 * @brief Enable the TIM DMAs.
 *
 * @param TIMx TIM_Type instance.
 * @param dmas DMA masks to be enabled. See to @ref TIM_DMA.
 * @param enable 'true' to enable the indicated interrupts or DMAs, 'false' to disable the indicated interrupts or DMAs.
 * @return None.
 */
void TIM_EnableDMA(TIM_Type * TIMx, uint32_t dmas, bool enable);

/*!
 * @brief Software trigger events to call interrupts or DMAs.
 *
 * Can simultaneously trigger multiple events.
 *
 * @param TIMx TIM instance. See to @ref TIM_SWTRG.
 * @param swtrgs Trigger the events to generate interrupts.
 */
void TIM_DoSwTrigger(TIM_Type * TIMx, uint32_t swtrgs);

/*!
 * @brief Get the status of the indicated timer.
 *
 * @param TIMx TIM_Type instance.
 * @return Interrupt status flags. See to @ref TIM_INT_STATUS.
 */
uint32_t TIM_GetInterruptStatus(TIM_Type * TIMx);

/*!
 * @brief Clear the status of indicated interrupt.
 *
 * @param TIMx TIM_Type instance.
 * @param status The required clear flag type. See to @ref TIM_INT_STATUS.
 * @return None.
 */
void TIM_ClearInterruptStatus(TIM_Type * TIMx, uint32_t status);

/*!
 * @brief Set up the output compare feature of indicated timer channel.
 *
 * If need to change the IO mode of the working channel, channel must be disabled first, then modify.
 *
 * @param TIMx TIM_Type instance.
 * @param channel Output channel instance. See to @ref TIM_CHANNEL_SELECTION.
 * @param conf Pointer to the configuration structure. See to @ref TIM_OutputCompareConf_Type.
 * @return None.
 */
void TIM_EnableOutputCompare(TIM_Type * TIMx, uint32_t channel, TIM_OutputCompareConf_Type * conf);

/*!
 * @brief Set up the complementary output compare feature of indicated timer channel.
 *
 * If need to change the IO mode of the working channel, channel must be disabled first, then modify.
 *
 * @param TIMx TIM_Type instance.
 * @param channel Output channel instance. See to @ref TIM_CHANNEL_SELECTION.
 * @param enable 'true' to enable the complementary channel output, 'false' to disable the complementary channel output.
 * @return None.
 */
void TIM_EnableCompOutput(TIM_Type * TIMx, uint32_t channel, bool enable);

/*!
 * @brief Enable the TIM channel output for pulse generation.
 *
 * @param TIMx TIM_Type instance.
 * @param enable 'true' to enable the channel output, 'false' to disable the channel output.
 * @return None.
 */
void TIM_EnableOutputCompareSwitch(TIM_Type * TIMx, bool enable);

/*!
 * @brief Enable the input capture feature of indicated timer channel.
 *
 * If need to change the IO mode of the working channel, channel must be disabled first, then modify.
 *
 * @param TIMx TIM_Type instance.
 * @param channel Input channel instance. See to @ref TIM_CHANNEL_SELECTION.
 * @param conf Pointer to the configuration structure. See to @ref TIM_InputCaptureConf_Type.
 * @return None.
 */
void TIM_EnableInputCapture(TIM_Type * TIMx, uint32_t channel, TIM_InputCaptureConf_Type * conf);

/*!
 * @brief Get the value of the indicated timer channel.
 *
 * When capture event happens, counter value will be kept in its indicated channel.
 *
 * @param TIMx TIM_Type instance.
 * @param channel Channel instance to get the value. See to @ref TIM_CHANNEL_SELECTION.
 * @return Value of the indiacted timer channel.
 */
uint32_t TIM_GetChannelValue(TIM_Type * TIMx, uint32_t channel);

/*!
 * @brief Put the value into the indicated timer channel.
 *
 * When in output compare, channel value would be the compared with counter value.
 *
 * @param TIMx TIM_Type instance.
 * @param channel Channel instance to put the value. See to @ref TIM_CHANNEL_SELECTION.
 * @param value The value to be set in the indicated timer channel.
 * @return None.
 */
void TIM_PutChannelValue(TIM_Type * TIMx, uint32_t channel, uint32_t value);

/*!
 * @brief Set the step length between TIM internal clock and dead area clock / ETR / TI sample clock.
 *
 * @param TIMx TIM_Type instance.
 * @param div Selected sample clock devision ratio.
 * @return None.
 */
void TIM_SetClockDiv(TIM_Type * TIMx, TIM_ClockDiv_Type div);

/*!
 * @brief Enable the dead area of the indicated timer.
 *
 * @param TIMx TIM_Type instance.
 * @param conf Pointer to the configuration structure. See to @ref TIM_DeadPeriodConf_Type.
 * @return None.
 */
void TIM_EnableDeadPeriod(TIM_Type * TIMx, TIM_DeadPeriodConf_Type * conf);

/*!
 * @brief Enable the master mode of the indicated timer.
 *
 * Master mode transfer the timer signal to other timers.
 *
 * @param TIMx TIM_Type instance.
 * @param conf Pointer to the configuration structure. See to @ref TIM_MasterModeConf_Type.
 * @return None.
 */
void TIM_EnableMasterMode(TIM_Type * TIMx, TIM_MasterModeConf_Type * conf);

/*!
 * @brief Enable the slave mode of the indicated timer.
 *
 * Slave mode can control the working state of counter.
 *
 * @param TIMx TIM_Type instance.
 * @param conf Pointer to the configuration structure. See to @ref TIM_SlaveModeConf_Type.
 * @return None.
 */
void TIM_EnableSlaveMode(TIM_Type * TIMx, TIM_SlaveModeConf_Type * conf);

/*!
 * @brief Enable the external trigger input of the indicated counter.
 *
 * External trigger signal will trigger counter do addition.
 * Its div and filter along with the counter StepFreqHz will affect.
 *
 * @param TIMx TIM_Type instance.
 * @param conf Pointer to the configuration structure. See to @ref TIM_ExtTriggerInConf_Type.
 * @return None.
 */
void TIM_EnableExtTriggerIn(TIM_Type * TIMx, TIM_ExtTriggerInConf_Type * conf);

/*!
 * @brief Enable the dma burst transfer of the indicated timer.
 *
 * @param TIMx TIM_Type instance.
 * @param conf Pointer to the configuration structure. See to @ref TIM_DMABurstConf_Type.
 * @return Address of DMA Burst transfer base register address.
 */
uint32_t TIM_EnableDMABurst(TIM_Type * TIMx, TIM_DMABurstConf_Type * conf);

/*!
 * @brief Get the encoder rotating direction and the counted encoder pulse times.
 *
 * @param TIMx TIM_Type instance.
 * @param value The counter value for pulse times.
 * @return Encoder rotating direction. See to @ref TIM_EncoderDirection_Type.
 */
TIM_EncoderDirection_Type TIM_GetEncoder(TIM_Type * TIMx, uint32_t * value);

/*!
 * @brief Set the indicated TIMx repetition counter value.
 *
 * @param TIMx TIM_Type instance.
 * @param value The repetition value to be set.
 * @return The current value of the counter register.
 */
void TIM_SetRepCounter(TIM_Type * TIMx, uint8_t value);

/*!
 * @brief Get the indicated TIMx Counter value.
 *
 * Real repetition's times number, meaning left repetition period times to run.
 *
 * @param TIMx TIM_Type instance.
 * @return The current value of the counter register.
 */
uint8_t TIM_GetRepCounterValue(TIM_Type * TIMx);

/*!
 * @brief Set the channel idle state output when switch off the output compare.
 *
 * @param TIMx TIM_Type instance.
 * @param channel Channel instance to set the idle state output. See to @ref TIM_CHANNEL_SELECTION.
 * @param conf Pointer to the configuration structure. See to @ref TIM_IdleOut_Type.
 * @return None.
 */
void TIM_EnableIdleOut(TIM_Type * TIMx, uint32_t channel, TIM_IdleOut_Type * conf);

/*!
 * @brief Set the lock level for protecting the timer config setting.
 *
 * @param TIMx TIM_Type instance.
 * @param lock Lock level to be set for protecting Timer set. See to @ref TIM_LockLevel_Type.
 * @return None.
 */
void TIM_EnableLock(TIM_Type * TIMx, TIM_LockLevel_Type lock);

/*!
 * @brief Set the break input of the timer.
 *
 * @param TIMx TIM_Type instance.
 * @param conf Pointer to the configuration structure. See to @ref TIM_BreakIn_Type.
 * @return None.
 */
void TIM_EnableBreakIn(TIM_Type * TIMx, TIM_BreakIn_Type * conf);

/*!
 * @brief Set the PWM shift of the indicated channel.
 *
 * @param TIMx TIM_Type instance.
 * @param channel Channel instance to set the pwm shift. See to @ref TIM_CHANNEL_SELECTION.
 * @param value The counter time when need pwm output shift. If val equals the channel value, disable the shift.
 * @return None.
 */
void TIM_EnablePWMShift(TIM_Type * TIMx, uint32_t channel, uint16_t value);

/*!
 *@}
 */
#endif /* __HAL_TIM_H__ */

