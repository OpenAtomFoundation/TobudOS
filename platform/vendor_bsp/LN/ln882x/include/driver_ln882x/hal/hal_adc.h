#ifndef __HAL_ADC_LN882X_H__
#define __HAL_ADC_LN882X_H__

#include "types.h"
#include "ln88xx.h"
#include "ll/ll_adc.h"

#ifdef __cplusplus
    extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
/////////////////////////     Macros and Constants   //////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define IS_ADC_ALL_PERIPH(PERIPH)   ( (PERIPH) == ADC )

typedef enum {FDISABLE = 0, FENABLE = !FDISABLE} FunctionalState;

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == FDISABLE) || ((STATE) == FENABLE))

///////////////////////////////////////////////////////////////////////////////
/////////////////////////          DataTypes    ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////

typedef struct
{

    FunctionalState ADC_Autoff;             /*!< When AUTOFF = 1, the ADC is always powered off when not converting
                                                and automatically wakes-up when a conversion is started (by software
                                                or hardware trigger) */
    FunctionalState ADC_WaitMode;           /*!< When WaitMode = 1, a new conversion can start only if the previous
                                                data has been treated, once the ADC_DR register has been read or if the
                                                EOC bit has been cleared. */

    FunctionalState ADC_ContinuousConvMode; /*!< Specifies whether the conversion is performed in
                                                Continuous or Single mode.
                                                This parameter can be set to FENABLE or FDISABLE */
    // TODO:
    // uint32_t        ADC_ExternalTrigConv;   /*!< Defines the external trigger used to start the analog to digital conversion of regular channels. */

    uint32_t        ADC_DataAlign;          /*!< Specifies whether the ADC data alignment is left or right.
                                                This parameter can be a value of @ref ADC_data_align. */
} ADC_InitTypeDef;


#define ADC_DataAlign_Right     (0)
#define ADC_DataAlign_Left      (1)
#define IS_ADC_DATA_ALIGN(align)    ( ((align) == ADC_DataAlign_Right) || \
                                    ((align) == ADC_DataAlign_Left) )


#define ADC_IT_AWD              (0x0400)
#define ADC_IT_OVR              (0x0200)
#define ADC_IT_EOS              (0x0100)
#define ADC_IT_EOC(chan_map)    ((chan_map) & 0xFF)
#define IS_ADC_IT(it)           ( ((it) & ADC_IT_AWD) || \
                                ((it) & ADC_IT_OVR) || \
                                ((it) & ADC_IT_EOS) || \
                                ADC_IT_EOC(it) )

typedef enum {
    ADC_CHAN_0 = (0x01),
    ADC_CHAN_1 = (0x02),
    ADC_CHAN_2 = (0x04),
    ADC_CHAN_3 = (0x08),
    ADC_CHAN_4 = (0x10),
    ADC_CHAN_5 = (0x20),
    ADC_CHAN_6 = (0x40),
    ADC_CHAN_7 = (0x80),
    ADC_CHAN_MAX = 8
} adc_chan_t;
#define IS_ADC_CHANMAP(chmap)   ( (chmap) & 0xFF )
#define IS_ADC_CHANX(chx)       (((chx) == ADC_CHAN_0) || \
                                ((chx) == ADC_CHAN_1) || \
                                ((chx) == ADC_CHAN_2) || \
                                ((chx) == ADC_CHAN_3) || \
                                ((chx) == ADC_CHAN_4) || \
                                ((chx) == ADC_CHAN_5) || \
                                ((chx) == ADC_CHAN_6) || \
                                ((chx) == ADC_CHAN_7) )

#define ADC_OVERSAMPLE_2X       (0x00)
#define ADC_OVERSAMPLE_4X       (0x01)
#define ADC_OVERSAMPLE_8X       (0x02)
#define ADC_OVERSAMPLE_16X      (0x03)
#define ADC_OVERSAMPLE_32X      (0x04)
#define ADC_OVERSAMPLE_64X      (0x05)
#define IS_ADC_OVERSAMPLE(ovsr)     ( ((ovsr) == ADC_OVERSAMPLE_2X) || \
                                    ((ovsr) == ADC_OVERSAMPLE_4X) || \
                                    ((ovsr) == ADC_OVERSAMPLE_8X) || \
                                    ((ovsr) == ADC_OVERSAMPLE_16X) || \
                                    ((ovsr) == ADC_OVERSAMPLE_32X) || \
                                    ((ovsr) == ADC_OVERSAMPLE_64X) )


// #define ADC_EXTTRIG_DISABLE         (0)
// #define ADC_EXTTRIG_RISING_EDGE     (1)
// #define ADC_EXTTRIG_FALLING_EDGE    (2)
// #define ADC_EXTTRIG_BOTH_EDGE       (3)

typedef enum {
    ADC_EXTTRIG_DISABLE = 0,
    ADC_EXTTRIG_RISING_EDGE = 1,
    ADC_EXTTRIG_FALLING_EDGE = 2,
    ADC_EXTTRIG_BOTH_EDGE = 3
} ADC_ExtTrig_t;

#define IS_ADC_EXTTRIG(trig)    (((trig) == ADC_EXTTRIG_DISABLE) || \
                                ((trig) == ADC_EXTTRIG_RISING_EDGE) || \
                                ((trig) == ADC_EXTTRIG_FALLING_EDGE) || \
                                ((trig) == ADC_EXTTRIG_BOTH_EDGE))

typedef enum
{
    ADC_EXTTRIG_SEL_TRG0 = 0,
    ADC_EXTTRIG_SEL_TRG1 = 1,
    ADC_EXTTRIG_SEL_TRG2 = 2,
    ADC_EXTTRIG_SEL_TRG3 = 3,
} ADC_ExtTrigSel_t;

#define IS_ADC_EXT_TRIG(exttrig)    (((exttrig) == (ADC_EXTTRIG_SEL_TRG0)) || \
                                    ((exttrig) == (ADC_EXTTRIG_SEL_TRG1)) || \
                                    ((exttrig) == (ADC_EXTTRIG_SEL_TRG2)) || \
                                    ((exttrig) == (ADC_EXTTRIG_SEL_TRG3)))


#define IS_ADC_PRESC(presc)         ((presc) <= 63)


typedef struct
{
    uint8_t isSingle;   /*!< watch on single channel or all selected channels */
    adc_chan_t chanX;      /*!< single channel to watch, this chan must be in the active selected channels */
} AnalogWatchDogCfg_t;

///////////////////////////////////////////////////////////////////////////////
/////////////////////////          Functions    ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void HAL_ADC_DeInit(ADC_Instance* ADCx);

void HAL_ADC_Init(ADC_Instance* ADCx, ADC_InitTypeDef* ADC_InitStruct);

void HAL_ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct);

void HAL_ADC_Cmd(ADC_Instance* ADCx, FunctionalState NewState);

void HAL_ADC_ITConfig(ADC_Instance* ADCx, uint16_t ADC_IT, FunctionalState NewState);

void HAL_ADC_SoftwareStartConvCmd(ADC_Instance* ADCx);

void HAL_ADC_StopConvCmd(ADC_Instance *ADCx);

uint8_t HAL_ADC_GetSoftwareStartConvStatus(ADC_Instance* ADCx);

void HAL_ADC_WaitModeCmd(ADC_Instance *ADCx, FunctionalState NewState);

void HAL_ADC_DiscModeCmd(ADC_Instance* ADCx, FunctionalState NewState);

void HAL_ADC_SeqChanSelect_Cfg(ADC_Instance *ADCx, uint8_t chan_map);

void HAL_ADC_RegularChannelConfig(ADC_Instance* ADCx, uint8_t Chan_map, uint8_t ratio);

void HAL_ADC_ExternalTrigConvCmd(ADC_Instance* ADCx, uint8_t exttrigcfg);

uint16_t HAL_ADC_GetConversionValue(ADC_Instance* ADCx, uint8_t ChanX);

void HAL_ADC_AnalogWatchdogCfg(ADC_Instance* ADCx, AnalogWatchDogCfg_t *cfg);

void HAL_ADC_AnalogWatchdogThresholdsConfig(ADC_Instance* ADCx, uint16_t HighThreshold, uint16_t LowThreshold);

void HAL_ADC_PrescCfg(ADC_Instance *ADCx, uint8_t presc);

uint16_t HAL_ADC_GetAllIT_Status(ADC_Instance *ADCx);

uint8_t HAL_ADC_IT_IsAnaWatchDog(ADC_Instance *ADCx);

uint8_t HAL_ADC_IT_IsOverrun(ADC_Instance *ADCx);

uint8_t HAL_ADC_IT_IsEndofSeq(ADC_Instance *ADCx);

uint8_t HAL_ADC_IT_EndOfConv_Status(ADC_Instance *ADCx);

void HAL_ADC_IT_ClrAll(ADC_Instance *ADCx);

void HAL_ADC_IT_Clr_AnaWatchDog(ADC_Instance *ADCx);

void HAL_ADC_IT_Clr_Overrun(ADC_Instance *ADCx);

void HAL_ADC_IT_Clr_EndOfSeq(ADC_Instance *ADCx);

void HAL_ADC_IT_Clr_EndOfConv(ADC_Instance *ADCx);

#ifdef __cplusplus
    }
#endif
#endif //!__HAL_ADC_LN882X_H__
