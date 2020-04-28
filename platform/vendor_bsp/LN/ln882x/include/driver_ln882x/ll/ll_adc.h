#ifndef __LL_ADC_LN882X_H__
#define __LL_ADC_LN882X_H__

#include "types.h"

#ifdef __cplusplus
    extern "C" {
#endif

typedef     void    ADC_Instance;
#define     ADC     ((ADC_Instance *) REG_ADC_BASE)

///////////////////////////////////////////////////////////////////////////////
////// Reg:  ADC_ISR
////// Desc: ADC interrupt and status register
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Check ADC status, including:
 * 1. awd, at bit10, Analog watchdog flag;
 * 2. ovr, at bit9, ADC overrun;
 * 3. eos, at bit8, End of sequence flag;
 * 4. eoc, at bit7~bit0, End of Conversion flag.
 *
 * @param ADCx
 * @return uint16_t
 */
uint16_t LL_ADC_IT_Status(ADC_Instance *ADCx);

void LL_ADC_IT_ClrAll(ADC_Instance *ADCx);

bool LL_ADC_IT_IsAnaWatchDog(ADC_Instance *ADCx);

bool LL_ADC_IT_IsOverrun(ADC_Instance *ADCx);

bool LL_ADC_IT_IsEndOfSeq(ADC_Instance *ADCx);

uint8_t LL_ADC_IT_EndOfConv(ADC_Instance *ADCx);

void LL_ADC_IT_Clr_AnaWatchDog(ADC_Instance *ADCx);

void LL_ADC_IT_Clr_Overrun(ADC_Instance *ADCx);

void LL_ADC_IT_Clr_EndOfSeq(ADC_Instance *ADCx);

void LL_ADC_IT_Clr_EndOfConv(ADC_Instance *ADCx);

///////////////////////////////////////////////////////////////////////////////
////// Reg:  ADC_IER
////// Desc: ADC interrupt enable register
///////////////////////////////////////////////////////////////////////////////

uint16_t LL_ADC_IT_EnabledStatus(ADC_Instance *ADCx);

void LL_ADC_IT_AnaWatchDog_Enable(ADC_Instance *ADCx, bool en);

bool LL_ADC_IT_IsAnaWatchDog_Enabled(ADC_Instance *ADCx);

void LL_ADC_IT_Overrun_Enable(ADC_Instance *ADCx, bool en);

bool LL_ADC_IT_IsOverrun_Enabled(ADC_Instance *ADCx);

void LL_ADC_IT_EndOfSeq_Enable(ADC_Instance *ADCx, bool en);

bool LL_ADC_IT_IsEndOfSeq_Enabled(ADC_Instance *ADCx);

void LL_ADC_IT_EndOfConv_Enable(ADC_Instance *ADCx, uint8_t eoc_bitmap);

uint8_t LL_ADC_IT_EndOfConv_EnabledStatus(ADC_Instance *ADCx);

///////////////////////////////////////////////////////////////////////////////
////// Reg:  ADC_CR
////// Desc: ADC control register
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief ADC stop conversion command.
 *
 * @param ADCx
 */
void LL_ADC_StopConv(ADC_Instance *ADCx);

bool LL_ADC_IsStopped(ADC_Instance *ADCx);

void LL_ADC_StartConv(ADC_Instance *ADCx);

bool LL_ADC_IsStarted(ADC_Instance *ADCx);

void LL_ADC_Enable(ADC_Instance *ADCx, bool en);

bool LL_ADC_IsEnabled(ADC_Instance *ADCx);

///////////////////////////////////////////////////////////////////////////////
////// Reg:  ADC_CFGR1
////// Desc: ADC configuration register 1
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Select one channel monitored by AWD.
 *
 * The channel selected must be also set into the CHSELR register.
 * @param ADCx
 * @param awdch valid value is 0~7, for chan0~chan7.
 */
void LL_ADC_AnaWatchDogChan_Set(ADC_Instance *ADCx, uint8_t awdch);

uint8_t LL_ADC_AnaWatchDogChan_Get(ADC_Instance *ADCx);

void LL_ADC_AnaWatchDog_Enable(ADC_Instance *ADCx, bool en);

bool LL_ADC_IsAnaWatchDog_Enabled(ADC_Instance *ADCx);

/**
 * @brief Enable the watchdog on a single channel or on all channels.
 *
 * @param ADCx
 * @param en 0 for all channels; 1 for single channel.
 */
void LL_ADC_AnaWatchDog_SingleChan_Enable(ADC_Instance *ADCx, bool en);

void LL_ADC_AutoOff_Enable(ADC_Instance *ADCx, bool en);

void LL_ADC_WaitConvMode_Enable(ADC_Instance *ADCx, bool en);

void LL_ADC_ContinuousMode_Enable(ADC_Instance *ADCx, bool en);

/**
 * @brief ADC data alignment.
 *
 * @param ADCx
 * @param en 0 -- right alignment; 1 -- left alignment.
 */
void LL_ADC_DataAlignLeft_Enable(ADC_Instance *ADCx, bool en);

/**
 * @brief External trigger enable and polarity selection.
 *
 * @param ADCx
 * @param extcfg valid values are:
 * 0 -- hardware trigger detection disabled (conversions can be started by software)
 * 1 -- hardware trigger detection on the rising edge
 * 2 -- hardware trigger detection on the falling edge
 * 3 -- hardware trigger detection on both rising and falling edges.
 */
void LL_ADC_ExtTrig_Cfg(ADC_Instance *ADCx, uint8_t extcfg);

/**
 * @brief External trigger selection
 *
 * @param ADCx
 * @param extsel valid values are:
 * 0 -- TRG0
 * 1 -- TRG1
 * 2 -- TRG2
 * 3 -- TRG3
 */
void LL_ADC_ExtTrigSelect(ADC_Instance *ADCx, uint8_t extsel);

///////////////////////////////////////////////////////////////////////////////
////// Reg:  ADC_CFGR2
////// Desc: ADC configuration register 2
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Analog related.
 *
 * @param ADCx
 * @param en
 */
void LL_ADC_InputBufBypass_Enable(ADC_Instance *ADCx, bool en);

/**
 * @brief Analog related.
 *
 * @param ADCx
 * @param vset
 */
void LL_ADC_VSET_Cfg(ADC_Instance *ADCx, uint8_t vset);

/**
 * @brief Calibration enable. When set, CH0 input is connected to VREF and CH1
 * input is connected to VCM.
 *
 * @param ADCx
 * @param en
 */
void LL_ADC_CalibrateEnable(ADC_Instance *ADCx, bool en);

/**
 * @brief Stable time set.
 *
 * @param ADCx
 * @param stab
 * default value is 9, means 9*16 ADC clock time.
 */
void LL_ADC_StableTime_Set(ADC_Instance *ADCx, uint8_t stab);

/**
 * @brief AUXADC reference voltage select.
 *
 * @param ADCx
 * @param vrefsel
 * 0 -- internal reference
 * 1 -- external reference
 */
void LL_ADC_Vrefsel_Set(ADC_Instance *ADCx, uint8_t vrefsel);

/**
 * @brief AUXADC input mode control.
 *
 * @param ADCx
 * @param mode_sel
 * 0 -- differential input
 * 1 -- single end input
 */
void LL_ADC_InputModeSel_Set(ADC_Instance *ADCx, uint8_t mode_sel);

/**
 * @brief Set the number of oversampling ratio.
 *
 * @param ADCx
 * @param ratio
 * 0 -- 2x
 * 1 -- 4x
 * 2 -- 8x
 * 3 -- 16x
 * 4 -- 32x
 * others -- 64x
 */
void LL_ADC_OverSampleRatio_Set(ADC_Instance *ADCx, uint8_t ratio);

void LL_ADC_OverSampleChan_Cfg(ADC_Instance *ADCx, uint8_t chan_map);

///////////////////////////////////////////////////////////////////////////////
////// Reg:  ADC_TR
////// Desc: ADC watchdog threshold register
///////////////////////////////////////////////////////////////////////////////

void LL_ADC_AnaWatchDogThreshold_Cfg(ADC_Instance *ADCx, uint16_t high, uint16_t low);

///////////////////////////////////////////////////////////////////////////////
////// Reg:  ADC_CHSELR
////// Desc: ADC channel selection register
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Select which channels are part of the sequence of channels to be converted.
 *
 * @param ADCx
 * @param chan_map one bit for one channel, 0 -- not selected; 1 -- selected.
 */
void LL_ADC_SeqChanSelect_Cfg(ADC_Instance *ADCx, uint8_t chan_map);

uint8_t LL_ADC_SeqChanSelect_Get(ADC_Instance *ADCx);

///////////////////////////////////////////////////////////////////////////////
////// Reg:  ADC_CCR
////// Desc: ADC common configuration register
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief ADC prescaler.
 * Set and cleared by software to select the frequency of the clock to the ADC.
 * The clock is common for all the ADCs.
 *
 * @param ADCx
 * @param presc
 * 1 -- ADC clock divided by (1+1) * 2;
 * 2 -- ADC clock divided by (2+1) * 2;
 * 3 -- ADC clock divided by (3+1) * 2;
 */
void LL_ADC_Prescaler_Cfg(ADC_Instance *ADCx, uint8_t presc);

///////////////////////////////////////////////////////////////////////////////
////// Reg:  ADC_DR0-7
////// Desc: ADC data register
///////////////////////////////////////////////////////////////////////////////

uint16_t LL_ADC_Data0_Get(ADC_Instance *ADCx);

uint16_t LL_ADC_Data1_Get(ADC_Instance *ADCx);

uint16_t LL_ADC_Data2_Get(ADC_Instance *ADCx);

uint16_t LL_ADC_Data3_Get(ADC_Instance *ADCx);

uint16_t LL_ADC_Data4_Get(ADC_Instance *ADCx);

uint16_t LL_ADC_Data5_Get(ADC_Instance *ADCx);

uint16_t LL_ADC_Data6_Get(ADC_Instance *ADCx);

uint16_t LL_ADC_Data7_Get(ADC_Instance *ADCx);

#ifdef __cplusplus
    }
#endif
#endif // !__LL_ADC_LN882X_H__
