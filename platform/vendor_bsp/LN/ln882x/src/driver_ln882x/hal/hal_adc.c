#include "hal/hal_adc.h"
#include "hal/hal_common.h"

/**
  * @brief  Deinitializes the ADCx peripheral registers to their default reset values.
  * @param
  * @retval None
  */
void HAL_ADC_DeInit(ADC_Instance* ADCx)
{
    // TODO: 外设复位，RCC 模块复位使能
}

/**
  * @brief  Initializes the ADCx peripheral according to the specified parameters
  *         in the ADC_InitStruct.
  * @param
  * @param  ADC_InitStruct: pointer to an ADC_InitTypeDef structure that contains
  *         the configuration information for the specified ADC peripheral.
  * @retval None
  */
void HAL_ADC_Init(ADC_Instance* ADCx, ADC_InitTypeDef* ADC_InitStruct)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_Autoff));
    assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_WaitMode));
    assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_ContinuousConvMode));
    assert_param(IS_ADC_DATA_ALIGN(ADC_InitStruct->ADC_DataAlign));

    LL_ADC_AutoOff_Enable(ADCx, ADC_InitStruct->ADC_Autoff);
    LL_ADC_WaitConvMode_Enable(ADCx, ADC_InitStruct->ADC_WaitMode);
    LL_ADC_ContinuousMode_Enable(ADCx, ADC_InitStruct->ADC_ContinuousConvMode);
    LL_ADC_DataAlignLeft_Enable(ADCx, ADC_InitStruct->ADC_DataAlign);

    //LL_ADC_ExtTrigSelect(ADCx, ADC_InitStruct->ADC_ExternalTrigConv);
}

/**
  * @brief  Fills each ADC_InitStruct member with its default value.
  * @param  ADC_InitStruct : pointer to an ADC_InitTypeDef structure which will be initialized.
  * @retval None
  */
void HAL_ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct)
{
    assert_param(ADC_InitStruct != NULL);

    ADC_InitStruct->ADC_Autoff = FDISABLE;
    ADC_InitStruct->ADC_WaitMode = FDISABLE;
    ADC_InitStruct->ADC_ContinuousConvMode = FDISABLE;
    ADC_InitStruct->ADC_DataAlign = ADC_DataAlign_Right;
}

/**
  * @brief  Enables or disables the specified ADC peripheral.
  * @param
  * @param  NewState: new state of the ADCx peripheral.
  *   This parameter can be: FENABLE or FDISABLE.
  * @retval None
  */
void HAL_ADC_Cmd(ADC_Instance* ADCx, FunctionalState NewState)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    // wake up ADC from power down mode.
    LL_ADC_Enable(ADCx, NewState);
}


/**
  * @brief  Enables or disables the specified ADC interrupts.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_IT: specifies the ADC interrupt sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg ADC_IT_EOC: End of conversion interrupt mask
  *     @arg ADC_IT_AWD: Analog watchdog interrupt mask
  * @param  NewState: new state of the specified ADC interrupts.
  *   This parameter can be: FENABLE or FDISABLE.
  * @retval None
  */
void HAL_ADC_ITConfig(ADC_Instance* ADCx, uint16_t ADC_IT, FunctionalState NewState)
{
    uint8_t eoc_bitmap = 0;

    assert_param(IS_ADC_ALL_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    assert_param(IS_ADC_IT(ADC_IT));

    if (ADC_IT & ADC_IT_AWD) {
        LL_ADC_IT_AnaWatchDog_Enable(ADCx, NewState);
    }

    if (ADC_IT & ADC_IT_OVR) {
        LL_ADC_IT_Overrun_Enable(ADCx, NewState);
    }

    if (ADC_IT & ADC_IT_EOS) {
        LL_ADC_IT_EndOfSeq_Enable(ADCx, NewState);
    }

    if (ADC_IT_EOC(ADC_IT)) {
        eoc_bitmap = ADC_IT_EOC(ADC_IT);
        LL_ADC_IT_EndOfConv_Enable(ADCx, eoc_bitmap);
    }
}

/**
  * @brief  Enables or disables the selected ADC software start conversion .
  * @retval None
  */
void HAL_ADC_SoftwareStartConvCmd(ADC_Instance* ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    LL_ADC_StartConv(ADCx);
}

/**
 * @brief Stop and discard an ongoing conversion.
 *
 * @param ADCx
 */
void HAL_ADC_StopConvCmd(ADC_Instance *ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    LL_ADC_StopConv(ADCx);
}
/**
  * @brief  Gets the selected ADC Software start conversion Status.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval The new state of ADC software start conversion (SET or RESET).
  */
uint8_t HAL_ADC_GetSoftwareStartConvStatus(ADC_Instance* ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    return (uint8_t) LL_ADC_IsStarted(ADCx);
}

/**
 * @brief In WAITM mode, A new conversion can start only if the previous data has been treated,
 * once the data register has been read or if the EOC bit has been cleared.
 *
 * @param ADCx
 * @param NewState
 */
void HAL_ADC_WaitModeCmd(ADC_Instance *ADCx, FunctionalState NewState)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    LL_ADC_WaitConvMode_Enable(ADCx, NewState);
}

/**
  * @brief  Enables or disables the discontinuous mode on regular group
  *         channel for the specified ADC
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  NewState: new state of the selected ADC discontinuous mode
  *         on regular group channel.
  *         This parameter can be: FENABLE or FDISABLE.
  * @retval None
  */
void HAL_ADC_DiscModeCmd(ADC_Instance* ADCx, FunctionalState NewState)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    LL_ADC_ContinuousMode_Enable(ADCx, !NewState);
}

/**
 * @brief Select which channels are part of the sequence of channesl to be converted.
 *
 * @param ADCx
 * @param chan_map
 */
void HAL_ADC_SeqChanSelect_Cfg(ADC_Instance *ADCx, uint8_t chan_map)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));
    assert_param(IS_ADC_CHANMAP(chan_map));

    LL_ADC_SeqChanSelect_Cfg(ADCx, chan_map);
}

/**
  * @brief  Configures for the selected ADC regular channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  Chan_map: the ADC channel to configure
  * @param  ratio: over sample ratio

  * @retval None
  */
void HAL_ADC_RegularChannelConfig(ADC_Instance* ADCx, uint8_t Chan_map, uint8_t ratio)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));
    assert_param(IS_ADC_CHANMAP(Chan_map));
    assert_param(IS_ADC_OVERSAMPLE(ratio));

    LL_ADC_OverSampleRatio_Set(ADCx, ratio);
    LL_ADC_OverSampleChan_Cfg(ADCx, Chan_map);
}

/**
  * @brief  Enables or disables the ADCx conversion through external trigger, and choose which edge to trigger.
  * @retval None
  */
void HAL_ADC_ExternalTrigConvCmd(ADC_Instance* ADCx, uint8_t exttrigcfg)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));
    assert_param(IS_ADC_EXTTRIG(exttrigcfg));

    LL_ADC_ExtTrig_Cfg(ADCx, exttrigcfg);
}

/**
  * @brief  Returns the last ADCx conversion result data for regular channel.
  * @retval The Data conversion value.
  */
uint16_t HAL_ADC_GetConversionValue(ADC_Instance* ADCx, uint8_t ChanX)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));
    assert_param(IS_ADC_CHANX(ChanX));

    uint16_t data = 0;

    switch (ChanX)
    {
    case ADC_CHAN_0:
        data = LL_ADC_Data0_Get(ADCx);
        break;
    case ADC_CHAN_1:
        data = LL_ADC_Data1_Get(ADCx);
        break;
    case ADC_CHAN_2:
        data = LL_ADC_Data2_Get(ADCx);
        break;
    case ADC_CHAN_3:
        data = LL_ADC_Data3_Get(ADCx);
        break;
    case ADC_CHAN_4:
        data = LL_ADC_Data4_Get(ADCx);
        break;
    case ADC_CHAN_5:
        data = LL_ADC_Data5_Get(ADCx);
        break;
    case ADC_CHAN_6:
        data = LL_ADC_Data6_Get(ADCx);
        break;
    case ADC_CHAN_7:
        data = LL_ADC_Data7_Get(ADCx);
        break;
    default:
        break;
    }

    return data;
}

/**
  * @brief  Enables or disables the analog watchdog on single/all regular
  *         or injected channels
  * @retval None
  */
void HAL_ADC_AnalogWatchdogCfg(ADC_Instance* ADCx, AnalogWatchDogCfg_t *cfg)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));
    assert_param(IS_ADC_CHANX(cfg->chanX));

    int index = 0;

    if (cfg->isSingle) {
        LL_ADC_AnaWatchDog_SingleChan_Enable(ADCx, 1);

        // bitmap to index
        for (index = 0; index < ADC_CHAN_MAX; index++ ) {
            if (_BIT(index) == cfg->chanX) {
                break;
            }
        }
        LL_ADC_AnaWatchDogChan_Set(ADCx, index);
    } else {
        LL_ADC_AnaWatchDog_SingleChan_Enable(ADCx, 0);
    }
}

/**
  * @brief  Configures the high and low thresholds of the analog watchdog.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  HighThreshold: the ADC analog watchdog High threshold value.
  *   This parameter must be a 12bit value.
  * @param  LowThreshold: the ADC analog watchdog Low threshold value.
  *   This parameter must be a 12bit value.
  * @retval None
  */
void HAL_ADC_AnalogWatchdogThresholdsConfig(ADC_Instance* ADCx, uint16_t HighThreshold, uint16_t LowThreshold)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));
    assert_param(HighThreshold > LowThreshold);

    LL_ADC_AnaWatchDogThreshold_Cfg(ADCx, HighThreshold, LowThreshold);
}

/**
 * @brief Select the frequency of the clock to the ADC.
 *
 * @param ADCx
 * @param presc
 * 0: input ADC clock divided by 1 * 2
 * 1: input ADC clock divided by 2 * 2
 * ...
 * 63: input ADC clock divided by 64 * 2
 */
void HAL_ADC_PrescCfg(ADC_Instance *ADCx, uint8_t presc)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));
    assert_param(IS_ADC_PRESC(presc));

    LL_ADC_Prescaler_Cfg(ADCx, presc);
}

uint16_t HAL_ADC_GetAllIT_Status(ADC_Instance *ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    return LL_ADC_IT_Status(ADCx);
}

uint8_t HAL_ADC_IT_IsAnaWatchDog(ADC_Instance *ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    return LL_ADC_IT_IsAnaWatchDog(ADCx);
}

uint8_t HAL_ADC_IT_IsOverrun(ADC_Instance *ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    return LL_ADC_IT_IsOverrun(ADCx);
}

uint8_t HAL_ADC_IT_IsEndofSeq(ADC_Instance *ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    return LL_ADC_IT_IsEndOfSeq(ADCx);
}

uint8_t HAL_ADC_IT_EndOfConv_Status(ADC_Instance *ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    return LL_ADC_IT_EndOfConv(ADCx);
}

void HAL_ADC_IT_ClrAll(ADC_Instance *ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    LL_ADC_IT_ClrAll(ADCx);
}

void HAL_ADC_IT_Clr_AnaWatchDog(ADC_Instance *ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    LL_ADC_IT_Clr_AnaWatchDog(ADCx);
}


void HAL_ADC_IT_Clr_Overrun(ADC_Instance *ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    LL_ADC_IT_Clr_Overrun(ADCx);
}

void HAL_ADC_IT_Clr_EndOfSeq(ADC_Instance *ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    LL_ADC_IT_Clr_EndOfSeq(ADCx);
}

void HAL_ADC_IT_Clr_EndOfConv(ADC_Instance *ADCx)
{
    assert_param(IS_ADC_ALL_PERIPH(ADCx));

    LL_ADC_IT_Clr_EndOfConv(ADCx);
}

