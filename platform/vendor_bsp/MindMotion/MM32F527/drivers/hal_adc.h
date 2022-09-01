/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_ADC_H__
#define __HAL_ADC_H__

#include "hal_common.h"

/*!
 * @addtogroup ADC
 * @{
 */

/*!
 * @brief ADC driver version number.
 */
#define ADC_DRIVER_VERSION 1u /*!< adc_1. */

/*!
 * @addtogroup ADC_STATUS
 * Define ADC status.
 * @{
 */

#define ADC_STATUS_CONV_SLOT_DONE       (1u << 0u) /*!< Status flag when ADC slot conversion done. */
#define ADC_STATUS_CONV_SAMPLE_DONE     (1u << 1u) /*!< Status flag when ADC sample done. */
#define ADC_STATUS_CONV_SEQ_DONE        (1u << 2u) /*!< Status flag when ADC sequence conversion done. */
#define ADC_STATUS_CONV_COMPARE_DONE    (1u << 3u) /*!< Status flag when ADC compare done. */
#define ADC_STATUS_CONV_CALIB_DONE      (1u << 4u) /*!< Status flag when ADC calibration done */
#define ADC_STATUS_JCONV_SLOT_DONE      (1u << 5u) /*!< Status flag when ADC inject slot conversion done. */
#define ADC_STATUS_JCONV_SAMPLE_DONE    (1u << 6u) /*!< Status flag when ADC inject sample done. */
#define ADC_STATUS_JCONV_SEQ_DONE       (1u << 7u) /*!< Status flag when ADC inject sequence conversion done. */

/*!
 * @}
 */

/*!
 * @addtogroup ADC_INT
 * ADC interrupt define.
 * @{
 */

#define ADC_INT_CONV_SLOT_DONE          (1u << 0u)  /*!< Interrupt when ADC slot conversion done. */
#define ADC_INT_CONV_SAMPLE_DONE        (1u << 1u)  /*!< Interrupt enable when ADC sample done. */
#define ADC_INT_CONV_SEQ_DONE           (1u << 2u)  /*!< Interrupt enable when ADC sequence conversion done. */
#define ADC_INT_CONV_COMPARE_DONE       (1u << 3u)  /*!< Interrupt enable when ADC compare done. */
#define ADC_INT_CONV_CALIB_DONE         (1u << 4u)  /*!< Interrupt enable when ADC calibration done */
#define ADC_INT_JCONV_SLOT_DONE         (1u << 5u)  /*!< Interrupt enable when ADC inject slot conversion done. */
#define ADC_INT_JCONV_SAMPLE_DONE       (1u << 6u)  /*!< Interrupt enable when ADC inject sample done. */
#define ADC_INT_JCONV_SEQ_DONE          (1u << 7u)  /*!< Interrupt enable when ADC inject sequence conversion done. */

/*!
 * @}
 */

/*!
 * @addtogroup ADC_RESULT_FLAGS
 * Define ADC Convert result flags.
 * @{
 */

#define ADC_CONV_RESULT_FLAG_OVERRUN (1u << 0u) /*!< Result flag when adc conversion result is overrun. */
#define ADC_CONV_RESULT_FLAG_VALID   (1u << 1u) /*!< Result flag when adc conversion result valid. */

/*!
 * @}
 */


/*!
 * @brief ADC Resolution type.
 *
 * Select ADC conversion data resolution.
 */
typedef enum
{
    ADC_Resolution_Alt0  = 0u,  /*!< source 0 as a resolution source for ADC. */
    ADC_Resolution_Alt1  = 1u,  /*!< source 1 as a resolution source for ADC. */
    ADC_Resolution_Alt2  = 2u,  /*!< source 2 as a resolution source for ADC. */
    ADC_Resolution_Alt3  = 3u,  /*!< source 3 as a resolution source for ADC. */
    ADC_Resolution_Alt4  = 4u,  /*!< source 4 as a resolution source for ADC. */
} ADC_Resolution_Type;


/*!
 * @brief ADC HwTriggerEdge type.
 *
 * Use hardware trigger in ADC, this trigger edge must be rising edge.
 */
typedef enum
{
    ADC_HwTriggerEdge_RisingEdge  = 2u,   /*!< Rising edge trigger. */
} ADC_HwTriggerEdge_Type;

/*!
 * @brief ADC HwTriggerDelayCycle type.
 *
 * After the hardware trigger signal is generated, delay N PCLK2 clock cycles before starting the first sample.
 */
typedef enum
{
    ADC_HwTriggerDelayCycle_0   = 0u,    /*!< Delay 0 cycle.    */
    ADC_HwTriggerDelayCycle_4   = 1u,    /*!< Delay 4  cycle.   */
    ADC_HwTriggerDelayCycle_16  = 2u,    /*!< Delay 16 cycle.   */
    ADC_HwTriggerDelayCycle_32  = 3u,    /*!< Delay 32 cycle.   */
    ADC_HwTriggerDelayCycle_64  = 4u,    /*!< Delay 64 cycle.   */
    ADC_HwTriggerDelayCycle_128 = 5u,    /*!< Delay 128 cycle. */
    ADC_HwTriggerDelayCycle_256 = 6u,    /*!< Delay 256 cycle.  */
    ADC_HwTriggerDelayCycle_512 = 7u,    /*!< Delay 512 cycle.  */
} ADC_HwTriggerDelayCycle_Type;

/*!
 * @brief ADC Align type.
 *
 * Choose to store the converted data as left-aligned or right-aligned.
 */
typedef enum
{
    ADC_Align_Right = 0u,     /*!< Data right align. */
    ADC_Align_Left  = 1u,     /*!< Data left align.  */
} ADC_Align_Type;

/*!
 * @brief ADC SeqFixedDirection type.
 *
 * In single-cycle scan or continuous scan mode, set the order of scan channels.
 */
typedef enum
{
    ADC_SeqFixedDirection_LowFirst  = 0u,   /*!< ADC scan direction from low channel to high channel. */
    ADC_SeqFixedDirection_HighFirst = 1u,   /*!< ADC scan direction from high channel to low channel. */
} ADC_SeqFixedDirection_Type;

/*!
 * @brief ADC ConvMode type.
 *
 * Select the adc channel or sequence convert mode.
 */
typedef enum
{
    ADC_ConvMode_SingleSlot = 0u,    /*!< Run the conversion by channel.  */
    ADC_ConvMode_SeqOneTime = 1u,    /*!< Run the conversion by sequence once. */
    ADC_ConvMode_SeqContinues = 2u,  /*!< Run the conversion by sequence again and again. */
} ADC_ConvMode_Type;

/*!
 * @brief ADC SampleTime type.
 *
 * Select channel sample time.
 */
typedef enum
{
    ADC_SampleTime_Alt0 = 0u,    /*!< Channel sample time mode  0 . */
    ADC_SampleTime_Alt1 = 1u,    /*!< Channel sample time mode  1 . */
    ADC_SampleTime_Alt2 = 2u,    /*!< Channel sample time mode  2 . */
    ADC_SampleTime_Alt3 = 3u,    /*!< Channel sample time mode  3 . */
    ADC_SampleTime_Alt4 = 4u,    /*!< Channel sample time mode  4 . */
    ADC_SampleTime_Alt5 = 5u,    /*!< Channel sample time mode  5 . */
    ADC_SampleTime_Alt6 = 6u,    /*!< Channel sample time mode  6 . */
    ADC_SampleTime_Alt7 = 7u,    /*!< Channel sample time mode  7 . */
    ADC_SampleTime_Alt8 = 8u,    /*!< Channel sample time mode  8 . */
    ADC_SampleTime_Alt9 = 9u,    /*!< Channel sample time mode  9 . */
    ADC_SampleTime_Alt10 = 10u,  /*!< Channel sample time mode  10. */
    ADC_SampleTime_Alt11 = 11u,  /*!< Channel sample time mode  11. */
    ADC_SampleTime_Alt12 = 12u,  /*!< Channel sample time mode  12. */
    ADC_SampleTime_Alt13 = 13u,  /*!< Channel sample time mode  13. */
    ADC_SampleTime_Alt14 = 14u,  /*!< Channel sample time mode  14. */
    ADC_SampleTime_Alt15 = 15u,  /*!< Channel sample time mode  15. */
} ADC_SampleTime_Type;

/*!
 * @brief ADC SingDiffConvMode type.
 *
 * Select ADC channel single-ended or difference conversion mode.
 */
typedef enum
{
    ADC_SingleDiffConvMode_SingleEnd = 1u,         /*!< Channel conversion mode is single-ended conversion. */
    ADC_SingleDiffConvMode_Diff = 2u,              /*!< Channel conversion mode is differential conversion. */
    ADC_SingleDiffConvMode_PseudoDiff = 3u,        /*!< Channel conversion mode is pseudo-difference conversion. */
}ADC_SingleDiffConvMode_Type;

/*!
 * @brief ADC SingleConvVref type.
 *
 * Select the reference voltage for single-ended conversion mode.
 */
typedef enum
{
    ADC_SingleConvVref_Internal = 1u,     /*!< Select the internal reference voltage as the reference voltage for single-ended conversion. */
    ADC_SingleConvVref_External = 0u,     /*!< Select the external reference voltage as the reference voltage for single-ended conversion. */
}ADC_SingleConvVref_Type;

/*!
 * @brief ADC DiffPair type.
 *
 * Select channel source as ADC differential or pseudo-difference conversion channels.
 */
typedef enum
{
    ADC_DiffChnPair_0_1  = 1u,       /*!< Select channel 0 and 1 for a group of differential or pseudo-difference channels. */
    ADC_DiffChnPair_2_3  = 2u,       /*!< Select channel 2 and 3 for a group of differential or pseudo-difference channels */
    ADC_DiffChnPair_4_5  = 4u,       /*!< Select channel 4 and 5 for a group of differential or pseudo-difference channels. */
    ADC_DiffChnPair_6_7  = 8u,       /*!< Select channel 6 and 7 for a group of differential or pseudo-difference channels. */
}ADC_DiffPair_Type;

/*!
 * @brief ADC OverSampleRatio type.
 *
 * Select ADC oversampling rate, to confirm the number of conversions performed by ADC, the sum of these conversion data is calculated as ADC oversampling conversion data (before potential shift).
 */
typedef enum
{
    ADC_OverSampleRatio_2   = 0u,    /*!< Select the oversample ratio of 2, 2 ADC conversions are performed, sum of these conversions data is computed to result as the ADC oversampling conversion data,before potential shift. */
    ADC_OverSampleRatio_4   = 1u,    /*!< Select the oversample ratio of 4, 4 ADC conversions are performed, sum of these conversions data is computed to result as the ADC oversampling conversion data,before potential shift. */
    ADC_OverSampleRatio_8   = 2u,    /*!< Select the oversample ratio of 8, 8 ADC conversions are performed, sum of these conversions data is computed to result as the ADC oversampling conversion data,before potential shift. */
    ADC_OverSampleRatio_16  = 3u,    /*!< Select the oversample ratio of 16, 16 ADC conversions are performed, sum of these conversions data is computed to result as the ADC oversampling conversion data,before potential shift. */
    ADC_OverSampleRatio_32  = 4u,    /*!< Select the oversample ratio of 32, 32 ADC conversions are performed, sum of these conversions data is computed to result as the ADC oversampling conversion data,before potential shift. */
    ADC_OverSampleRatio_64  = 5u,    /*!< Select the oversample ratio of 64, 64 ADC conversions are performed, sum of these conversions data is computed to result as the ADC oversampling conversion data,before potential shift. */
    ADC_OverSampleRatio_128 = 6u,    /*!< Select the oversample ratio of 128,128 ADC conversions are performed, sum of these conversions data is computed to result as the ADC oversampling conversion data,before potential shift. */
    ADC_OverSampleRatio_256 = 7u,    /*!< Select the oversample ratio of 256, 256 ADC conversions are performed, sum of these conversions data is computed to result as the ADC oversampling conversion data,before potential shift. */
}ADC_OverSampleRatio_Type;

/*!
 * @brief ADC OverSampleDataDiv type.
 *
 * Select ADC oversampling rate, to confirm the number of conversions performed by ADC, the sum of these conversion data is calculated as ADC oversampling conversion data (before potential shift).
 */
typedef enum
{
    ADC_OverSampleDataDiv_0   = 0u,    /*!< Select ADC oversampling no shift (sum of the ADC conversions data is not divided to result as the ADC oversampling conversion data). */
    ADC_OverSampleDataDiv_1   = 1u,    /*!< Select ADC oversampling 1 shift (sum of the ADC conversions data is divided by 2 to result as the ADC oversampling conversion data). */
    ADC_OverSampleDataDiv_2   = 2u,    /*!< Select ADC oversampling 2 shift (sum of the ADC conversions data is divided by 4 to result as the ADC oversampling conversion data). */
    ADC_OverSampleDataDiv_3   = 3u,    /*!< Select ADC oversampling 3 shift (sum of the ADC conversions data is divided by 8 to  result as the ADC oversampling conversion data). */
    ADC_OverSampleDataDiv_4   = 4u,    /*!< Select ADC oversampling 4 shift (sum of the ADC conversions data is divided by 16 to result as the ADC oversampling conversion data). */
    ADC_OverSampleDataDiv_5   = 5u,    /*!< Select ADC oversampling 5 shift (sum of the ADC conversions data is divided by 32 to result as the ADC oversampling conversion data). */
    ADC_OverSampleDataDiv_6   = 6u,    /*!< Select ADC oversampling 6 shift (sum of the ADC conversions data is divided by 64 to result as the ADC oversampling conversion data). */
    ADC_OverSampleDataDiv_7   = 7u,    /*!< Select ADC oversampling 7 shift (sum of the ADC conversions data is divided by 128 to result as the ADC oversampling conversion data). */
    ADC_OverSampleDataDiv_8   = 8u,    /*!< Select ADC oversampling 8 shift (sum of the ADC conversions data is divided by 256 to result as the ADC oversampling conversion data). */
}ADC_OverSampleDataDiv_Type;

/*!
 * @brief ADC ADC_OverSampleTrigMode type.
 *
 * Select ADC oversampling continuous or discontinuous mode
 */
typedef enum
{
   ADC_OverSampleTriggerMode_Multi  = 0u,         /*!< All conversions of oversampling ratio are done from 1 trigger. */
   ADC_OverSampleTriggerMode_Single = 1u,         /*!< Each conversion of oversampling ratio needs a trigger. */
}ADC_OverSampleTriggerMode_Type;


/*!
 * @brief This type of structure instance is used to keep the settings
 *  when calling the @ref ADC_Init() to initialize the ADC module.
 */
typedef struct
{
    ADC_Resolution_Type Resolution;             /*!< Specify the available bits for the conversion result data. */
    ADC_Align_Type Align;                       /*!< Specify the data alignment. */
    ADC_ConvMode_Type ConvMode;                 /*!< Specify the adc conversion mode. */
    ADC_SingleDiffConvMode_Type SingleDiffMode; /*!< Select ADC channel single-ended or difference conversion mode.*/
    ADC_DiffPair_Type DiffPair;                 /*!< Select channel source as ADC differential or pseudo-difference conversion channels. */
    ADC_SingleConvVref_Type SingleVolt;         /*!< Select the single-ended conversion vref source. */
} ADC_Init_Type;

/*!
 * @brief This type of structure instance is used to keep the settings
 *  when calling the @ref ADC_EnableHwTrigger() to initialize the ADC hardware trigger module.
 */
typedef struct
{
    ADC_HwTriggerEdge_Type       Edge;        /*!< Select the trigger edge. */
    ADC_HwTriggerDelayCycle_Type DelayCycle;  /*!< Select the hardware trigger shift sample. */
} ADC_HwTriggerConf_Type;

/*!
 * @brief This type of structure instance is used to keep the settings
 * when calling the @ref ADC_EnableHwComp() to enable the ADC windows compare mode.
 */
typedef struct
{
    uint32_t ChnNum;        /*!< Select the channel number binding to the compare. */
    uint32_t HighLimit;     /*!< The comparator high limit. */
    uint32_t LowLimit;      /*!< The comparator low limit. */
} ADC_HwCompConf_Type;

/*!
 * @brief This type of structure instance is used to keep the settings
 * This function just only used in any channel or inject channel.
 *  when calling the @ref ADC_SetOverSample() to initialize the ADC  oversample module.
 */
typedef struct
{
    ADC_OverSampleRatio_Type Ratio;                 /*!< Configures the oversampling ratio. */
    ADC_OverSampleDataDiv_Type Div;                 /*!< Configures the division coefficient for the Oversampler. */
    ADC_OverSampleTriggerMode_Type Mode;            /*!< Select the oversample trigger mode. */
    bool injectOverSampleMode;                      /*!< Whether injection channel oversampling is enable, ture is enable, false is disable. */
}ADC_OverSampleConf_Type;

/*!
 * @brief Initialize the ADC module.
 *
 * @param ADCx ADC instance.
 * @param init  Pointer to the initialization structure. See to @ref ADC_Init_Type.
 * @return None.
 */
void       ADC_Init(ADC_Type * ADCx, ADC_Init_Type * init);

/*!
 * @brief Enable the ADC module.
 *
 * The ADC module should be enabled before conversion data.
 *
 * @param ADCx ADC instance.
 * @param enable 'true' to enable the module, 'false' to disable the module.
 * @return None.
 */
void       ADC_Enable(ADC_Type * ADCx, bool enable);

/*!
 * @brief Enable Temperature sensor from the ADC module.
 *
 * The module should be enabled when Using the built-in temperature sensor to
 * detect temperature changes inside the device.
 *
 * @param ADCx ADC instance.
 * @param enable 'true' to enable the module, 'false' to disable the module.
 * @return None.
 */
void       ADC_EnableTempSensor(ADC_Type * ADCx, bool enable);

/*!
 * @brief Enable Voltage sensor from the ADC module.
 *
 * The module should be enabled when using internal reference voltage .
 *
 * @param ADCx ADC instance.
 * @param enable 'true' to enable the module, 'false' to disable the module.
 * @return None.
 */
void       ADC_EnableVoltSensor(ADC_Type * ADCx, bool enable);

/*!
 * @brief Enable the DMA from the ADC module.
 *
 * @param ADCx ADC instance.
 * @param enable 'true' to enable the DMA, 'false' to disable the DMA.
 * @return None.
 */
void       ADC_EnableDMA(ADC_Type * ADCx, bool enable);

/*!
 * @brief Enable interrupts of the ADC module.
 *
 * @param ADCx ADC instance.
 * @param interrupts Interrupt code masks.
 * @param enable 'true' to enable the indicated interrupts, 'false' to disable the indicated interrupts.
 * @return None.
 */
void       ADC_EnableInterrupts(ADC_Type * ADCx, uint32_t interrupts, bool enable);

/*!
 * @brief Get the current status flags of the ADC module.
 *
 * @param ADCx ADC instance.
 * @return Status flags.
 */
uint32_t   ADC_GetStatus(ADC_Type * ADCx);

/*!
 * @brief Clear the status flags of the ADC module.
 *
 * @param ADCx ADC instance.
 * @param flags  The mask codes of the indicated interrupt flags to be cleared.
 * @return None.
 */
void       ADC_ClearStatus(ADC_Type * ADCx, uint32_t flags);

/*!
 * @brief Get the channel convert data from the ADC module.
 *
 * @param ADCx ADC instance.
 * @param channel The convert channel.
 * @param flags The ADC convert result flags. See to @ref ADC_RESULT_FLAGS.
 * @return The data value.
 */
uint32_t   ADC_GetChnConvResult(ADC_Type * ADCx, uint32_t * channel, uint32_t * flags);

/*!
 * @brief Get the indication slot conversion data from the ADC module.
 *
 * @param ADCx ADC instance.
 * @param slot the adc conversion slot.
 * @param flags The ADC convert result flags. See to @ref ADC_RESULT_FLAGS.
 * @return The data value.
 */
uint32_t   ADC_GetSlotConvResult(ADC_Type * ADCx, uint32_t slot, uint32_t * flags);

/*!
 * @brief Get the inject channel convert data from the ADC module.
 *
 * @param ADCx ADC instance.
 * @param channel The convert channel.
 * @param flags The ADC convert result flags. See to @ref ADC_RESULT_FLAGS.
 * @return The data value.
 */
uint32_t   ADC_GetExtChnConvResult(ADC_Type * ADCx, uint32_t * channel, uint32_t * flags);

/*!
 * @brief Get the indication external slot convert data from the ADC module.
 *
 * @param ADCx ADC instance.
 * @param slot the adc external slot.
 * @param flags The ADC convert result flags. See to @ref ADC_RESULT_FLAGS.
 * @return The data value.
 */
uint32_t   ADC_GetExtSlotConvResult(ADC_Type * ADCx, uint32_t slot, uint32_t * flags);

/*!
 * @brief Configuration channel sample time of the ADC module.
 *
 * @param ADCx ADC instance.
 * @param channel the adc convert channel.
 * @param sampletime  the adc channel sample time
 * @return None.
 */
void       ADC_SetChnSampleTime(ADC_Type * ADCx, uint32_t channel, ADC_SampleTime_Type sample_time);

/*!
 * @brief Enable hardware trigger from the ADC module.
 *
 * The ADC module should be enabled before using hardware trrigger function.
 * it also have interrupt.
 *
 * @param ADCx ADC instance.
 * @param conf Select the hardware trigger source. See to @ref ADC_HwTrgConf_Type.
 * @return None.
 */
void       ADC_EnableHwTrigger(ADC_Type * ADCx, ADC_HwTriggerConf_Type * conf);

/*!
 * @brief Enable fixed slot sequence conversion from the ADC module.
 *
 * The ADC module should be enabled before regular sequence function.
 * it also have interrupt.
 *
 * @param ADCx ADC instance.
 * @param seq_slot the fixed channel, it can be ascending or descending.
 * @param seq_direction the sequence fixed scan direction.
 * @return None.
 */
void      ADC_EnableSeqSlotFixed(ADC_Type * ADCx, uint32_t seq_slot, ADC_SeqFixedDirection_Type dir);


/*!
 * @brief Enable sequence conversion from the ADC module.
 *
 * The ADC module should be enabled before Any sequence function.
 * it also have interrupt.
 *
 * @param ADCx ADC instance.
 * @param slot_idx the sequence slot index.
 * @param channel the conversion channel, it will be placed in each slot.
 * @return None.
 */
void       ADC_EnableSeqSlot(ADC_Type * ADCx, uint32_t slot_idx, uint32_t channel);

/*!
 * @brief Enable external sequence conversion from the ADC module.
 *
 * The ADC module should be enabled before sequence conversion function.
 * it also have interrupt.
 *
 * @param ADCx ADC instance.
 * @param slot_idx the sequence slot index.
 * @param channel the conversion channel, it will be placed in each slot.
 * @return None.
 */
void       ADC_EnableExtSeqSlot(ADC_Type * ADCx, uint32_t slot_idx, uint32_t channel);

/*!
 * @brief Enable Inject sequence auto conversion from the ADC module.
 * the sequence conversion done, the inject sequence start conversion.
 *
 * @param ADCx ADC instance.
 * @param enable 'true' to enable the ADC start inject sequence convert, 'false' to disable.
 * @return None.
 */
void       ADC_EnableAutoExtSeqSlot(ADC_Type * ADCx, bool enable);

/*!
 * @brief Do external chanenel sequence conversion software trigger.
 *
 * @param ADCx ADC instance.
 * @param enable 'true' to enable the ADC start inject channel convert, 'false' to disable.
 * @return None.
 */
void       ADC_DoExtSeqSlotSwTrigger(ADC_Type * ADCx, bool enable);

/*!
 * @brief Set inject chanenel data offset compensation.
 *
 * @param ADCx ADC instance.
 * @param value difference between actual input value and converted value.
 * @param slot the inject slot.
 * @return None.
 */
void       ADC_SetExtSlotCalibParam(ADC_Type * ADCx, uint32_t slot, uint32_t value);

/*!
 * @brief Enable window comparison from the ADC module.
 *
 * The ADC module should be enabled before using analog watchdog function.
 * it also have interrupt.
 *
 * @param ADCx ADC instance.
 * @param conf Pointer to the initialization structure. See to @ref ADC_HwCompConf_Type.
 * @return None.
 */
void       ADC_EnableHwComp(ADC_Type * ADCx, ADC_HwCompConf_Type * conf);

/*!
 * @brief Do soft trigger.
 *
 * @param ADCx ADC instance.
 * @param enable 'true' to enable the ADC start convert, 'false' to disable.
 * @return None.
 */
void       ADC_DoSwTrigger(ADC_Type * ADCx, bool enable);

/*!
 * @brief Do auto self-calibration.
 *
 * @param ADCx ADC instance.
 * @return None.
 */
void       ADC_DoAutoCalib(ADC_Type * ADCx);

/*!
 * @brief Get Calibration factor through auto calibration.
 *
 * when you use the use this api to get the calibration factor when you write the calibration through the register.
 * @param ADCx ADC instance.
 * @param factor from self-calibration,the calibration parameter consists of 17 9-digit numbers.
 * @param return 1;
 */
uint32_t 	   ADC_GetCalibFactor(ADC_Type * ADCx, uint32_t * factor);

/*!
 * @brief Set Calibration factor through register.
 *
 * when you use the use this api to write the calibration factor when you write the calibration through the register.
 * @param ADCx ADC instance.
 * @param factor the calibration parameters from ADC_GetCalibFactor().
 * @return None.
 */
void       ADC_SetCalibFactor(ADC_Type * ADCx, uint32_t * factor);

/*!
 * @brief Set Set OverSample mode.
 *
 * Average value of converted data using hardware implementation.
 * @param ADCx ADC instance.
 * @param conf Pointer to the initialization structure. See to @ref ADC_OverSampleConf_Type.
 * @return None.
 */

void       ADC_SetOverSample(ADC_Type * ADCx, ADC_OverSampleConf_Type *conf);

/*!
 * @brief Reset all the slot conversion.
 *
 * @param ADCx ADC instance.
 * @return None.
 */

void       ADC_ResetAllSlot(ADC_Type * ADCx);


/*!
 *@}
 */

#endif /*__HAL_ADC_H__. */
