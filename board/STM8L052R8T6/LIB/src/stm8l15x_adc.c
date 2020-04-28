/**
  ******************************************************************************
  * @file    stm8l15x_adc.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Analog to Digital Convertor (ADC) peripheral:           
  *           - Initialization and Configuration
  *           - Power saving
  *           - Analog Watchdog configuration              
  *           - Temperature Sensor & Vrefint (Voltage Reference internal) management 
  *           - Regular Channels Configuration
  *           - Regular Channels DMA Configuration
  *           - Injected channels Configuration      
  *           - Interrupts and flags management       
  *         
  *  @verbatim
  *                               
  *          ===================================================================      
  *                                   How to use this driver
  *          ===================================================================      
  *          1. Enable The ADC controller clock using CLK_PeripheralClockConfig() 
  *            function : CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE).
  *
  *            2. Configure the ADC Prescaler, conversion resolution and data 
  *              alignment using the ADC_Init() function.
  * 
  *            3. Activate the ADC peripheral using ADC_Cmd() function.  
  *
  *          Regular channels group configuration
  *          ====================================    
  *            - To configure the ADC regular channels group features, use 
  *              ADC_Init() and ADC_RegularChannelConfig() functions.
  *            - To activate the continuous mode, use the ADC_continuousModeCmd()
  *              function.
  *            - To configure and activate the Discontinuous mode, use the 
  *              ADC_DiscModeChannelCountConfig() and ADC_DiscModeCmd() functions.        
  *            - To read the ADC converted values, use the ADC_GetConversionValue()
  *              function.
  *
  *          DMA for Regular channels group features configuration
  *          ====================================================== 
  *           - To enable the DMA mode for regular channels group, use the 
  *             ADC_DMACmd() function.
  *           - To enable the generation of DMA requests continuously at the end
  *             of the last DMA transfer, use the ADC_DMARequestAfterLastTransferCmd() 
  *             function.    
             
  *          Injected channels group configuration
  *          =====================================    
  *            - To configure the ADC Injected channels group features, use 
  *              ADC_InjectedChannelConfig() and  ADC_InjectedSequencerLengthConfig()
  *              functions.
  *            - To activate the continuous mode, use the ADC_continuousModeCmd()
  *              function.
  *            - To activate the Injected Discontinuous mode, use the 
  *              ADC_InjectedDiscModeCmd() function.  
  *            - To activate the AutoInjected mode, use the ADC_AutoInjectedConvCmd() 
  *              function.        
  *            - To read the ADC converted values, use the ADC_GetInjectedConversionValue() 
  *              function.
  *              
  *  @endverbatim
  *         
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x_adc.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup ADC 
  * @brief ADC driver modules
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup ADC_Private_Functions
  * @{
  */ 
    
    
/** @defgroup ADC_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
                      Initialization and Configuration functions
 ===============================================================================  
  This section provides functions allowing to:
   - Enable or disable the ADC peripheral,
   - Initialize and configure the ADC Prescaler, ADC Conversion Resolution 
     (12bit..6bit), ADC Continuous Conversion Mode (Continuous or Single 
     conversion),
   - Configure External trigger Sensitivity and source,
   - Start ADC conversion, by software trigger.


@endverbatim
  * @{
  */
/**
  * @brief  Deinitializes the ADC peripheral registers to their default reset 
  *         values.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @retval None
  */
void ADC_DeInit(ADC_TypeDef* ADCx)
{
  /*  Set the Configuration registers to their reset values */
  ADCx->CR1 =  ADC_CR1_RESET_VALUE;
  ADCx->CR2 =  ADC_CR2_RESET_VALUE;
  ADCx->CR3 =  ADC_CR3_RESET_VALUE;

  /*  Set the status registers to their reset values */
  ADCx->SR =  (uint8_t)~ADC_SR_RESET_VALUE;

  /*  Set the High threshold registers to their reset values */
  ADCx->HTRH =  ADC_HTRH_RESET_VALUE;
  ADCx->HTRL =  ADC_HTRL_RESET_VALUE;

  /*  Set the low threshold registers to their reset values */
  ADCx->LTRH =  ADC_LTRH_RESET_VALUE;
  ADCx->LTRL =  ADC_LTRL_RESET_VALUE;

  /*  Set the channels sequence registers to their reset values */
  ADCx->SQR[0] =  ADC_SQR1_RESET_VALUE;
  ADCx->SQR[1] =  ADC_SQR2_RESET_VALUE;
  ADCx->SQR[2] =  ADC_SQR3_RESET_VALUE;
  ADCx->SQR[3] =  ADC_SQR4_RESET_VALUE;

  /*  Set the channels Trigger registers to their reset values */
  ADCx->TRIGR[0] =  ADC_TRIGR1_RESET_VALUE;
  ADCx->TRIGR[1] =  ADC_TRIGR2_RESET_VALUE;
  ADCx->TRIGR[2] =  ADC_TRIGR3_RESET_VALUE;
  ADCx->TRIGR[3] =  ADC_TRIGR4_RESET_VALUE;
}

/**
  * @brief  Initializes the specified ADC peripheral according to the specified 
  *         parameters.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  ADC_ConversionMode : specifies the ADC conversion mode,
  *          This parameter can be one of the following values:
  *            @arg ADC_ConversionMode_Single: Single Conversion Mode
  *            @arg ADC_ConversionMode_Continuous: Continuous Conversion Mode
  * @param  ADC_Resolution : specifies the ADC Data resolution,
  *          This parameter can be one of the following values:
  *            @arg ADC_Resolution_12Bit: 12 bit resolution
  *            @arg ADC_Resolution_10Bit: 10 bit resolution
  *            @arg ADC_Resolution_8Bit: 8 bit resolution
  *            @arg ADC_Resolution_6Bit: 6 bit resolution      
  * @param  ADC_Prescaler : specifies the ADC Prescaler,
  *          This parameter can be one of the following values:
  *            @arg ADC_Prescaler_1: ADC Clock frequency is divided by 1
  *            @arg ADC_Prescaler_2: ADC Clock frequency is divided by 2
  * @retval None
  */
void ADC_Init(ADC_TypeDef* ADCx,
              ADC_ConversionMode_TypeDef ADC_ConversionMode,
              ADC_Resolution_TypeDef ADC_Resolution,
              ADC_Prescaler_TypeDef ADC_Prescaler)
{
  /* Check the parameters */
  assert_param(IS_ADC_CONVERSION_MODE(ADC_ConversionMode));
  assert_param(IS_ADC_RESOLUTION(ADC_Resolution));
  assert_param(IS_ADC_PRESCALER(ADC_Prescaler));

  /*clear CR1 register */
  ADCx->CR1 &= (uint8_t)~(ADC_CR1_CONT | ADC_CR1_RES);

  /* set the resolution and the conversion mode */
  ADCx->CR1 |= (uint8_t)((uint8_t)ADC_ConversionMode | (uint8_t)ADC_Resolution);

  /*clear CR2 register */
  ADCx->CR2 &= (uint8_t)~(ADC_CR2_PRESC);

  /* set the Prescaler */
  ADCx->CR2 |= (uint8_t) ADC_Prescaler;
}

/**
  * @brief  Enables or disables the selected ADC peripheral.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  NewState : new state of the specified ADC peripheral.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_Cmd(ADC_TypeDef* ADCx,
             FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Set the ADON bit to wake up the specified ADC from power down mode */
    ADCx->CR1 |= ADC_CR1_ADON;
  }
  else
  {
    /* Disable the selected ADC peripheral */
    ADCx->CR1 &= (uint8_t)~ADC_CR1_ADON;
  }
}

/**
  * @brief  Starts ADC conversion, by software trigger.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @retval None
  */
void ADC_SoftwareStartConv(ADC_TypeDef* ADCx)
{
  /*  Start the ADC software conversion */
  ADCx->CR1 |= ADC_CR1_START;
}

/**
  * @brief  Configures the ADC conversion through external trigger.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  ADC_ExtEventSelection : Specifies the external trigger.
  *          This parameter can be one of the following values:
  *            @arg ADC_ExtEventSelection_None: Conversion starts only by software start
  *            @arg ADC_ExtEventSelection_Trigger1: Trigger 1  Enables conversion
  *            @arg ADC_ExtEventSelection_Trigger2: Trigger 2  Enables conversion
  *            @arg ADC_ExtEventSelection_Trigger3: Trigger 3  Enables conversion      
  * @param  ADC_ExtTRGSensitivity : Specifies the external trigger sensitivity.
  *          This parameter can be one of the following values:
  *            @arg ADC_ExtTRGSensitivity_Rising: External Trigger Sensitivity is Rising Edge
  *            @arg ADC_ExtTRGSensitivity_Falling: External Trigger Sensitivity is Falling Edge
  *            @arg ADC_ExtTRGSensitivity_All: External Trigger Sensitivity is Falling and Rising Edge
  * @retval None
  */
void ADC_ExternalTrigConfig(ADC_TypeDef* ADCx,
                            ADC_ExtEventSelection_TypeDef ADC_ExtEventSelection,
                            ADC_ExtTRGSensitivity_TypeDef ADC_ExtTRGSensitivity)
{
  /* Check the parameters */
  assert_param(IS_ADC_EXT_EVENT_SELECTION(ADC_ExtEventSelection));
  assert_param(IS_ADC_EXT_TRG_SENSITIVITY(ADC_ExtTRGSensitivity));

  /*clear old configuration of CR2 register */
  ADCx->CR2 &= (uint8_t)~(ADC_CR2_TRIGEDGE | ADC_CR2_EXTSEL);

  /* set the External Trigger Edge Sensitivity  and the external event 
     selection */
  ADCx->CR2 |= (uint8_t)( (uint8_t)ADC_ExtTRGSensitivity | \
                          (uint8_t)ADC_ExtEventSelection);
}

 /**
  * @}
  */

/** @defgroup ADC_Group2 Analog Watchdog configuration functions
 *  @brief   Analog Watchdog configuration functions 
 *
@verbatim   
 ===============================================================================
                    Analog Watchdog configuration functions
 ===============================================================================  

  This section provides functions allowing to configure the Analog Watchdog
  (AWD) feature in the ADC.
  
  A typical configuration Analog Watchdog is done following these steps :
   1. the ADC guarded channel is selected using the 
      ADC_AnalogWatchdogChannelSelect() function.
   2. The Analog watchdog lower and higher threshold are configured using the  
      ADC_AnalogWatchdogThresholdsConfig() function.
   
   Note : Both AWD selection and thresholds can be configured with one unique 
          function ADC_AnalogWatchdogConfig(), which is kept for firmware 
          compatibility reason.

@endverbatim
  * @{
  */
/**
  * @brief  Configures the channel to be checked by the Analog watchdog.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  ADC_AnalogWatchdogSelection : Specifies the channel to be checked 
  *         by the Analog watchdog.
  *          This parameter can be one of the following values:
  *            @arg ADC_AnalogWatchdogSelection_Channel0: AWD affected to Channel 0
  *            @arg ADC_AnalogWatchdogSelection_Channel1: AWD affected to Channel 1
  *            @arg ADC_AnalogWatchdogSelection_Channel2: AWD affected to Channel 2
  *            @arg ADC_AnalogWatchdogSelection_Channel3: AWD affected to Channel 3
  *            @arg ADC_AnalogWatchdogSelection_Channel4: AWD affected to Channel 4
  *            @arg ADC_AnalogWatchdogSelection_Channel5: AWD affected to Channel 5
  *            @arg ADC_AnalogWatchdogSelection_Channel6: AWD affected to Channel 6
  *            @arg ADC_AnalogWatchdogSelection_Channel7: AWD affected to Channel 7
  *            @arg ADC_AnalogWatchdogSelection_Channel8: AWD affected to Channel 8
  *            @arg ADC_AnalogWatchdogSelection_Channel9: AWD affected to Channel 9
  *            @arg ADC_AnalogWatchdogSelection_Channel10: AWD affected to Channel 10
  *            @arg ADC_AnalogWatchdogSelection_Channel11: AWD affected to Channel 11
  *            @arg ADC_AnalogWatchdogSelection_Channel12: AWD affected to Channel 12
  *            @arg ADC_AnalogWatchdogSelection_Channel13: AWD affected to Channel 13
  *            @arg ADC_AnalogWatchdogSelection_Channel14: AWD affected to Channel 14
  *            @arg ADC_AnalogWatchdogSelection_Channel15: AWD affected to Channel 15
  *            @arg ADC_AnalogWatchdogSelection_Channel16: AWD affected to Channel 16
  *            @arg ADC_AnalogWatchdogSelection_Channel17: AWD affected to Channel 17
  *            @arg ADC_AnalogWatchdogSelection_Channel18: AWD affected to Channel 18
  *            @arg ADC_AnalogWatchdogSelection_Channel19: AWD affected to Channel 19
  *            @arg ADC_AnalogWatchdogSelection_Channel20: AWD affected to Channel 20
  *            @arg ADC_AnalogWatchdogSelection_Channel21: AWD affected to Channel 21
  *            @arg ADC_AnalogWatchdogSelection_Channel22: AWD affected to Channel 22
  *            @arg ADC_AnalogWatchdogSelection_Channel23: AWD affected to Channel 23
  *         @ref ADC_AnalogWatchdogSelection_TypeDef enumeration.
  * @retval None
  */
void ADC_AnalogWatchdogChannelSelect(ADC_TypeDef* ADCx,
                                     ADC_AnalogWatchdogSelection_TypeDef ADC_AnalogWatchdogSelection)
{
  /* Check the parameters */
  assert_param(IS_ADC_ANALOGWATCHDOG_SELECTION(ADC_AnalogWatchdogSelection));

  /* Reset the CHSEL bits */
  ADCx->CR3 &= ((uint8_t)~ADC_CR3_CHSEL);

  /* Select the channel to be checked by the Analog watchdog */
  ADCx->CR3 |= (uint8_t)ADC_AnalogWatchdogSelection;
}

/**
  * @brief  Configures the high and low thresholds of the Analog watchdog.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  HighThreshold: Analog watchdog High threshold value.
    *       This parameter must be a 12bit value.
  * @param  LowThreshold: Analog watchdog Low threshold value.
    *       This parameter must be a 12bit value.
  * @retval None
  */
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef* ADCx, uint16_t HighThreshold, uint16_t LowThreshold)
{
  /* Check the parameters */
  assert_param(IS_ADC_THRESHOLD(HighThreshold));
  assert_param(IS_ADC_THRESHOLD(LowThreshold));

  /* Set the ADC high threshold */
  ADCx->HTRH = (uint8_t)(HighThreshold >> 8);
  ADCx->HTRL = (uint8_t)(HighThreshold);

  /* Set the ADC low threshold */
  ADCx->LTRH = (uint8_t)(LowThreshold >> 8);
  ADCx->LTRL = (uint8_t)(LowThreshold);
}

/**
  * @brief  Configures the Analog watchdog.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  ADC_AnalogWatchdogSelection : Specifies the channel to be checked 
  *         by the Analog watchdog.
  *          This parameter can be one of the following values:
  *            @arg ADC_AnalogWatchdogSelection_Channel0: AWD affected to Channel 0
  *            @arg ADC_AnalogWatchdogSelection_Channel1: AWD affected to Channel 1
  *            @arg ADC_AnalogWatchdogSelection_Channel2: AWD affected to Channel 2
  *            @arg ADC_AnalogWatchdogSelection_Channel3: AWD affected to Channel 3
  *            @arg ADC_AnalogWatchdogSelection_Channel4: AWD affected to Channel 4
  *            @arg ADC_AnalogWatchdogSelection_Channel5: AWD affected to Channel 5
  *            @arg ADC_AnalogWatchdogSelection_Channel6: AWD affected to Channel 6
  *            @arg ADC_AnalogWatchdogSelection_Channel7: AWD affected to Channel 7
  *            @arg ADC_AnalogWatchdogSelection_Channel8: AWD affected to Channel 8
  *            @arg ADC_AnalogWatchdogSelection_Channel9: AWD affected to Channel 9
  *            @arg ADC_AnalogWatchdogSelection_Channel10: AWD affected to Channel 10
  *            @arg ADC_AnalogWatchdogSelection_Channel11: AWD affected to Channel 11
  *            @arg ADC_AnalogWatchdogSelection_Channel12: AWD affected to Channel 12
  *            @arg ADC_AnalogWatchdogSelection_Channel13: AWD affected to Channel 13
  *            @arg ADC_AnalogWatchdogSelection_Channel14: AWD affected to Channel 14
  *            @arg ADC_AnalogWatchdogSelection_Channel15: AWD affected to Channel 15
  *            @arg ADC_AnalogWatchdogSelection_Channel16: AWD affected to Channel 16
  *            @arg ADC_AnalogWatchdogSelection_Channel17: AWD affected to Channel 17
  *            @arg ADC_AnalogWatchdogSelection_Channel18: AWD affected to Channel 18
  *            @arg ADC_AnalogWatchdogSelection_Channel19: AWD affected to Channel 19
  *            @arg ADC_AnalogWatchdogSelection_Channel20: AWD affected to Channel 20
  *            @arg ADC_AnalogWatchdogSelection_Channel21: AWD affected to Channel 21
  *            @arg ADC_AnalogWatchdogSelection_Channel22: AWD affected to Channel 22
  *            @arg ADC_AnalogWatchdogSelection_Channel23: AWD affected to Channel 23  
  * @param  HighThreshold: Analog watchdog High threshold value.
    *       This parameter must be a 12bit value.
  * @param  LowThreshold: Analog watchdog Low threshold value.
    *       This parameter must be a 12bit value.
  * @retval None
  */
void ADC_AnalogWatchdogConfig(ADC_TypeDef* ADCx,
                              ADC_AnalogWatchdogSelection_TypeDef ADC_AnalogWatchdogSelection,
                              uint16_t HighThreshold,
                              uint16_t LowThreshold)
{
  /* Check the parameters */
  assert_param(IS_ADC_ANALOGWATCHDOG_SELECTION(ADC_AnalogWatchdogSelection));
  assert_param(IS_ADC_THRESHOLD(HighThreshold));
  assert_param(IS_ADC_THRESHOLD(LowThreshold));

  /*Reset the CHSEL bits */
  ADCx->CR3 &= ((uint8_t)~ADC_CR3_CHSEL);

  /* Select the channel to be checked by the Analog watchdog.*/
  ADCx->CR3 |= (uint8_t)ADC_AnalogWatchdogSelection;

  /* Set the ADC high threshold */
  ADCx->HTRH = (uint8_t)(HighThreshold >> 8);
  ADCx->HTRL = (uint8_t)(HighThreshold);

  /* Set the ADC low threshold */
  ADCx->LTRH = (uint8_t)(LowThreshold >> 8);
  ADCx->LTRL = (uint8_t)LowThreshold;
}

 /**
  * @}
  */

/** @defgroup ADC_Group3 Temperature Sensor & Vrefint (Voltage Reference 
 *            internal) management functions
 *  @brief   Temperature Sensor & Vrefint (Voltage Reference internal) 
 *            management functions 
 *
@verbatim   
 ===============================================================================
  Temperature Sensor & Vrefint (Voltage Reference internal) management functions
 ===============================================================================  

  This section provides functions allowing to enable/ disable the internal 
  connections between the ADC and the Temperature Sensor and the Vrefint source.
     
  A typical configuration to get the Temperature sensor or/and Vrefint channels 
  voltages is done following these steps :
   1. Enable the internal connection of Temperature sensor or/and Vrefint sources 
      with the ADC channels: 
      - for the Temperature sensor using ADC_TempSensorCmd() function.
      - for the Internal Voltage reference using ADC_VrefintCmd() function. 
   2. Enable the ADC_Channel_TempSensor and/or ADC_Channel_Vrefint channels 
      using ADC_ChannelCmd()function. 
   3. Get the voltage values, using ADC_GetConversionValue().
 
@endverbatim
  * @{
  */
  
/**
  * @brief  Enables or disables the Temperature sensor internal reference.
  * @param  NewState : new state of the Temperature sensor internal reference.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_TempSensorCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /*Enable the Temperature sensor internal reference.*/
    ADC1->TRIGR[0] |= (uint8_t)(ADC_TRIGR1_TSON);
  }
  else
  {
    /*Disable the Temperature sensor internal reference.*/
    ADC1->TRIGR[0] &= (uint8_t)(~ADC_TRIGR1_TSON);
  }
}

/**
  * @brief  Enables or disables the Internal Voltage reference.
  * @param  NewState : new state of the Internal Voltage reference.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_VrefintCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the Internal Voltage reference.*/
    ADC1->TRIGR[0] |= (uint8_t)(ADC_TRIGR1_VREFINTON);
  }
  else
  {
    /* Disable the Internal Voltage reference.*/
    ADC1->TRIGR[0] &= (uint8_t)(~ADC_TRIGR1_VREFINTON);
  }
}

 /**
  * @}
  */
/** @defgroup ADC_Group4 Channels Configuration functions
 *  @brief   Channels Configuration functions
 *
@verbatim    
 ===============================================================================
                      Channels Configuration functions
 ===============================================================================  
  This section provides functions allowing to:
   - Enable or disable the ADC channel using ADC_ChannelCmd() function,
   - Configure the channels sampling times using ADC_SamplingTimeConfig() 
     function.
     Note: there are 2 sampling times configuration values : 
            - 1st Group value : for channels 0..23  
            - 2nd Group value : for channels 24..27 (depending on the MCU 
              package density) and Temperature Sensor and Vrefint channels. 
   - Configure the channels Schmitt Trigger for each channel using 
     ADC_SchmittTriggerConfig() function.
   - Get the current ADC conversion value.
   
 @endverbatim
  * @{
  */
    
/**
  * @brief  Enables or disables the selected ADC channel(s).
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  ADC_Channels: specifies the ADC channels to be initialized
  *          This parameter can be one of the following values:
  *            @arg ADC_Channel_0: Channel 0
  *            @arg ADC_Channel_1: Channel 1
  *            @arg ADC_Channel_2: Channel 2
  *            @arg ADC_Channel_3: Channel 3
  *            @arg ADC_Channel_4: Channel 4
  *            @arg ADC_Channel_5: Channel 5
  *            @arg ADC_Channel_6: Channel 6
  *            @arg ADC_Channel_7: Channel 7
  *            @arg ADC_Channel_8: Channel 8
  *            @arg ADC_Channel_9: Channel 9
  *            @arg ADC_Channel_10: Channel 10
  *            @arg ADC_Channel_11: Channel 11
  *            @arg ADC_Channel_12: Channel 12
  *            @arg ADC_Channel_13: Channel 13
  *            @arg ADC_Channel_14: Channel 14
  *            @arg ADC_Channel_15: Channel 15
  *            @arg ADC_Channel_16: Channel 16
  *            @arg ADC_Channel_17: Channel 17
  *            @arg ADC_Channel_18: Channel 18
  *            @arg ADC_Channel_19: Channel 19
  *            @arg ADC_Channel_20: Channel 20
  *            @arg ADC_Channel_21: Channel 21
  *            @arg ADC_Channel_22: Channel 22
  *            @arg ADC_Channel_23: Channel 23
  *            @arg ADC_Channel_24: Channel 24
  *            @arg ADC_Channel_25: Channel 25
  *            @arg ADC_Channel_26: Channel 26
  *            @arg ADC_Channel_27: Channel 27
  *            @arg ADC_Channel_Vrefint: Vrefint Channel
  *            @arg ADC_Channel_TempSensor: Temperature sensor Channel
  *            @arg ADC_Channel_00To07: select from channel00 to channel07
  *            @arg ADC_Channel_08To15: select from channel08 to channel15
  *            @arg ADC_Channel_16To23: select from channel16 to channel23
  *            @arg ADC_Channel_24To27: select from channel24 to channel27
  * @param  NewState : new state of the specified ADC channel(s).
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_ChannelCmd(ADC_TypeDef* ADCx, ADC_Channel_TypeDef ADC_Channels, FunctionalState NewState)
{
  uint8_t regindex = 0;
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  regindex = (uint8_t)((uint16_t)ADC_Channels >> 8);

  if (NewState != DISABLE)
  {
    /* Enable the selected ADC channel(s). */
    ADCx->SQR[regindex] |= (uint8_t)(ADC_Channels);
  }
  else
  {
    /* Disable the selected ADC channel(s). */
    ADCx->SQR[regindex] &= (uint8_t)(~(uint8_t)(ADC_Channels));
  }
}

/**
  * @brief  Configures the sampling time for the selected ADC channel group.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  ADC_GroupChannels : ADC channel group to configure.
  *          This parameter can be one of the following values:
  *            @arg ADC_Group_SlowChannels: Slow Channels group
  *            @arg ADC_Group_FastChannels: Fast Channels group  
  * @note   The channels of 1st ADC Group can be channels 0..23  
  * @note   The channels of 2nd ADC Group can be channels 24..27 (depending on the MCU 
  *         package density) and Temperature Sensor and Vrefint channels.  
  * @param  ADC_SamplingTime : Specifies the sample time value
  *          This parameter can be one of the following values:
  *            @arg ADC_SamplingTime_4Cycles: Sampling Time Cycles is 4
  *            @arg ADC_SamplingTime_9Cycles: Sampling Time Cycles is 9
  *            @arg ADC_SamplingTime_16Cycles: Sampling Time Cycles is 16
  *            @arg ADC_SamplingTime_24Cycles: Sampling Time Cycles is 24
  *            @arg ADC_SamplingTime_48Cycles: Sampling Time Cycles is 48
  *            @arg ADC_SamplingTime_96Cycles: Sampling Time Cycles is 96
  *            @arg ADC_SamplingTime_192Cycles: Sampling Time Cycles is 192
  *            @arg ADC_SamplingTime_384Cycles: Sampling Time Cycles is 384
  * @retval None
  */
void ADC_SamplingTimeConfig(ADC_TypeDef* ADCx,
                            ADC_Group_TypeDef ADC_GroupChannels,
                            ADC_SamplingTime_TypeDef ADC_SamplingTime)
{
  /* Check the parameters */
  assert_param(IS_ADC_GROUP(ADC_GroupChannels));
  assert_param(IS_ADC_SAMPLING_TIME_CYCLES(ADC_SamplingTime));

  if ( ADC_GroupChannels != ADC_Group_SlowChannels)
  {
    /* Configures the sampling time for the Fast ADC channel group. */
    ADCx->CR3 &= (uint8_t)~ADC_CR3_SMPT2;
    ADCx->CR3 |= (uint8_t)(ADC_SamplingTime << 5);
  }
  else
  {
    /* Configures the sampling time for the Slow ADC channel group. */
    ADCx->CR2 &= (uint8_t)~ADC_CR2_SMPT1;
    ADCx->CR2 |= (uint8_t)ADC_SamplingTime;
  }
}

/**
  * @brief  Configures the status of the Schmitt Trigger for the selected ADC 
  *         channel(s).
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  ADC_Channels: specifies the ADC channels to be initialized,
  *          This parameter can be one of the following values:
  *            @arg ADC_Channel_0: Channel 0
  *            @arg ADC_Channel_1: Channel 1
  *            @arg ADC_Channel_2: Channel 2
  *            @arg ADC_Channel_3: Channel 3
  *            @arg ADC_Channel_4: Channel 4
  *            @arg ADC_Channel_5: Channel 5
  *            @arg ADC_Channel_6: Channel 6
  *            @arg ADC_Channel_7: Channel 7
  *            @arg ADC_Channel_8: Channel 8
  *            @arg ADC_Channel_9: Channel 9
  *            @arg ADC_Channel_10: Channel 10
  *            @arg ADC_Channel_11: Channel 11
  *            @arg ADC_Channel_12: Channel 12
  *            @arg ADC_Channel_13: Channel 13
  *            @arg ADC_Channel_14: Channel 14
  *            @arg ADC_Channel_15: Channel 15
  *            @arg ADC_Channel_16: Channel 16
  *            @arg ADC_Channel_17: Channel 17
  *            @arg ADC_Channel_18: Channel 18
  *            @arg ADC_Channel_19: Channel 19
  *            @arg ADC_Channel_20: Channel 20
  *            @arg ADC_Channel_21: Channel 21
  *            @arg ADC_Channel_22: Channel 22
  *            @arg ADC_Channel_23: Channel 23
  *            @arg ADC_Channel_24: Channel 24
  *            @arg ADC_Channel_25: Channel 25
  *            @arg ADC_Channel_26: Channel 26
  *            @arg ADC_Channel_27: Channel 27
  *            @arg ADC_Channel_Vrefint: Vrefint Channel
  *            @arg ADC_Channel_TempSensor: Temperature sensor Channel
  *            @arg ADC_Channel_00To07: select from channel00 to channel07
  *            @arg ADC_Channel_08To15: select from channel08 to channel15
  *            @arg ADC_Channel_16To23: select from channel16 to channel23
  *            @arg ADC_Channel_24To27: select from channel24 to channel27
  * @param  NewState : new state of the Schmitt Trigger
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_SchmittTriggerConfig(ADC_TypeDef* ADCx, ADC_Channel_TypeDef ADC_Channels,
                              FunctionalState NewState)
{
  uint8_t regindex = 0;
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  regindex = (uint8_t)((uint16_t)ADC_Channels >> 8);

  if (NewState != DISABLE)
  {
    /* Enable the Schmitt Trigger for the selected ADC channel(s).*/
    ADCx->TRIGR[regindex] &= (uint8_t)(~(uint8_t)ADC_Channels);
  }
  else
  {
    /* Disable the Schmitt Trigger for the selected ADC channel(s).*/
    ADCx->TRIGR[regindex] |= (uint8_t)(ADC_Channels);
  }
}

/**
  * @brief  Returns the last ADC converted data.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @retval The Data conversion value.
  */
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx)
{
  uint16_t tmpreg = 0;

  /* Get last ADC converted data.*/
  tmpreg = (uint16_t)(ADCx->DRH);
  tmpreg = (uint16_t)((uint16_t)((uint16_t)tmpreg << 8) | ADCx->DRL);

  /* Return the selected ADC conversion value */
  return (uint16_t)(tmpreg) ;
}

 /**
  * @}
  */

/** @defgroup ADC_Group5 ADC Channels DMA Configuration function
 *  @brief   ADC Channels DMA Configuration function 
 *
@verbatim   
 ===============================================================================
                   ADC Channels DMA Configuration function
 ===============================================================================  

  This section provides a function allowing to configure the DMA for ADC 
  channel.
  Since converted channel values are stored into a unique data register, 
  it is useful to use DMA for conversion of more than one channel. This 
  avoids the loss of the data already stored in the ADC Data register. 
  
  When the DMA mode is enabled (using the ADC_DMACmd() function), after each
  conversion of a channel, a DMA request is generated.

@endverbatim
  * @{
  */
/**
  * @brief  Enables or disables the specified ADC DMA request.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  NewState : new state of the specified ADC DMA transfer.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the specified ADC DMA request */
    ADCx->SQR[0] &= (uint8_t)~ADC_SQR1_DMAOFF;
  }
  else
  {
    /* Disable the specified ADC DMA request */
    ADCx->SQR[0] |= ADC_SQR1_DMAOFF;
  }
}

/**
  * @}
  */

/** @defgroup ADC_Group6 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim   
 ===============================================================================
                   Interrupts and flags management functions
 ===============================================================================  

  This section provides functions allowing to configure the ADC Interrupts and 
  get the status and clear flags and Interrupts pending bits.
  
  The ADC provides 3 Interrupt sources and 3 Flags:
  
  Flags :
  ---------- 
     1. ADC_FLAG_OVR : Overrun detection when ADC channel converted data is lost

     2. ADC_FLAG_EOC : End of conversion - to indicate the end of a regular 
                  CHANNEL conversion or a GROUP conversions, depending of the 
                  ADC Continuous Conversion Mode (Continuous or Single 
                  conversion) and of the DMA usage.
                  Note : if DMA is used, EOC occurs at the end of the sequence 
                         conversion, else it occurs after each conversion
     3. ADC_FLAG_AWD: to indicate if the converted voltage crosses the 
                programmed Analog watchdog thresholds values.

     
  Interrupts :
  ------------
     1. ADC_IT_OVR : specifies the interrupt source for the Overrun detection event.
     2. ADC_IT_EOC : specifies the interrupt source for the End of conversion event.
     3. ADC_IT_AWD : specifies the interrupt source for the Analog watchdog event.

@endverbatim
  * @{
  */ 

/**
  * @brief  Enables or disables the specified ADC interrupts.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  ADC_IT : specifies the ADC interrupt sources to be enabled or 
  *         disabled. 
  *          This parameter can be one of the following values:
  *            @arg ADC_IT_EOC: End of Conversion Interrupt
  *            @arg ADC_IT_AWD: Analog Watchdog Interrupt
  *            @arg ADC_IT_OVER: Over Run Interrupt
  * @param  NewState : new state of the specified ADC interrupts.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_ITConfig(ADC_TypeDef* ADCx, ADC_IT_TypeDef ADC_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  assert_param(IS_ADC_IT(ADC_IT));

  if (NewState != DISABLE)
  {
    /* Enable the selected ADC interrupts */
    ADCx->CR1 |= (uint8_t) ADC_IT;
  }
  else
  {
    /* Disable the selected ADC interrupts */
    ADCx->CR1 &= (uint8_t)(~ADC_IT);
  }
}

/**
  * @brief  Checks whether the specified ADC flag is set or not.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  ADC_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg ADC_FLAG_EOC: End of Conversion flag
  *            @arg ADC_FLAG_AWD: Analog Watchdog flag
  *            @arg ADC_FLAG_OVER: Over Run flag    
  * @retval The new state of ADC_FLAG (SET or RESET).
  */
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, ADC_FLAG_TypeDef ADC_FLAG)
{
  FlagStatus flagstatus = RESET;

  /* Check the parameters */
  assert_param(IS_ADC_GET_FLAG(ADC_FLAG));

  /* Check the status of the specified ADC flag */
  if ((ADCx->SR & ADC_FLAG) != (uint8_t)RESET)
  {
    /* ADC_FLAG is set */
    flagstatus = SET;
  }
  else
  {
    /* ADC_FLAG is reset */
    flagstatus = RESET;
  }

  /* Return the ADC_FLAG status */
  return  flagstatus;
}

/**
  * @brief  Clears the ADC's pending flags.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  ADC_FLAG: specifies the flag to clear.
  *          This parameter can be one of the following values:
  *            @arg ADC_FLAG_EOC: End of Conversion flag
  *            @arg ADC_FLAG_AWD: Analog Watchdog flag
  *            @arg ADC_FLAG_OVER: Over Run flag 
  * @retval None
  */
void ADC_ClearFlag(ADC_TypeDef* ADCx,
                   ADC_FLAG_TypeDef ADC_FLAG)
{
  /* Check the parameters */
  assert_param(IS_ADC_CLEAR_FLAG(ADC_FLAG));

  /* Clear the selected ADC flags */
  ADCx->SR = (uint8_t)~ADC_FLAG;
}

/**
  * @brief  Checks whether the specified ADC interrupt has occurred or not.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  ADC_IT: specifies the ADC interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg ADC_IT_EOC: End of Conversion Interrupt
  *            @arg ADC_IT_AWD: Analog Watchdog Interrupt
  *            @arg ADC_IT_OVER: Over Run Interrupt
  * @retval Status of ADC_IT (SET or RESET).
  */
ITStatus ADC_GetITStatus(ADC_TypeDef* ADCx,
                         ADC_IT_TypeDef ADC_IT)
{
  ITStatus itstatus = RESET;
  uint8_t itmask = 0, enablestatus = 0;

  /* Check the parameters */
  assert_param(IS_ADC_GET_IT(ADC_IT));

  /* Get the ADC IT index */
  itmask = (uint8_t)(ADC_IT >> 3);
  itmask =  (uint8_t)((uint8_t)((uint8_t)(itmask & (uint8_t)0x10) >> 2) | \
                                (uint8_t)(itmask & (uint8_t)0x03));

  /* Get the ADC_IT enable bit status */
  enablestatus = (uint8_t)(ADCx->CR1 & (uint8_t)ADC_IT) ;

  /* Check the status of the specified ADC interrupt */
  if (((ADCx->SR & itmask) != (uint8_t)RESET) && enablestatus)
  {
    /* ADC_IT is set */
    itstatus = SET;
  }
  else
  {
    /* ADC_IT is reset */
    itstatus = RESET;
  }

  /* Return the ADC_IT status */
  return  itstatus;
}

/**
  * @brief  Clears the ADC’s interrupt pending bits.
  * @param  ADCx where x can be 1 to select the specified ADC peripheral.
  * @param  ADC_IT: specifies the ADC interrupt pending bit to clear.
  *          This parameter can be one of the following values:
  *            @arg ADC_IT_EOC: End of Conversion Interrupt
  *            @arg ADC_IT_AWD: Analog Watchdog Interrupt
  *            @arg ADC_IT_OVER: Over Run Interrupt
  * @retval None
  */
void ADC_ClearITPendingBit(ADC_TypeDef* ADCx,
                           ADC_IT_TypeDef ADC_IT)
{
  uint8_t itmask = 0;

  /* Check the parameters */
  assert_param(IS_ADC_IT(ADC_IT));

  /* Get the ADC IT index */
  itmask = (uint8_t)(ADC_IT >> 3);
  itmask =  (uint8_t)((uint8_t)(((uint8_t)(itmask & (uint8_t)0x10)) >> 2) | \
                                 (uint8_t)(itmask & (uint8_t)0x03));

  /* Clear the selected ADC interrupt pending bits */
  ADCx->SR = (uint8_t)~itmask;
}

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
