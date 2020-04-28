/**
  ******************************************************************************
  * @file    stm8l15x_tim5.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the TIM5 peripheral:
  *            - TimeBase management
  *            - Output Compare management
  *            - Input Capture management
  *            - Interrupts, DMA and flags management
  *            - Clocks management
  *            - Synchronization management
  *            - Specific interface management
  *              
  *  @verbatim
  *  
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================
  *          This driver provides functions to configure and initialize the TIM5
  *          peripheral
  *          These functions are split in 7 groups: 
  *   
  *          1. TIM5 TimeBase management: this group includes all needed functions 
  *             to configure the TIM Timebase unit:
  *                   - Set/Get Prescaler
  *                   - Set/Get Autoreload  
  *                   - Counter modes configuration
  *                   - Select the One Pulse mode
  *                   - Update Request Configuration
  *                   - Update Disable Configuration
  *                   - Auto-Preload Configuration 
  *                   - Enable/Disable the counter
  *                 
  *          2. TIM5 Output Compare management: this group includes all needed 
  *             functions to configure the Capture/Compare unit used in Output 
  *             compare mode: 
  *                   - Configure each channel, independently, in Output Compare mode
  *                   - Select the output compare modes
  *                   - Select the Polarities of each channel
  *                   - Set/Get the Capture/Compare register values
  *                   - Select the Output Compare Fast mode 
  *                   - Select the Output Compare Forced mode  
  *                   - Output Compare-Preload Configuration 
  *                   - Enable/Disable the Capture/Compare Channels    
  *                   
  *          3. TIM5 Input Capture management: this group includes all needed 
  *             functions to configure the Capture/Compare unit used in 
  *             Input Capture mode:
  *                   - Configure each channel in input capture mode
  *                   - Configure Channel1/2 in PWM Input mode
  *                   - Set the Input Capture Prescaler
  *                   - Get the Capture/Compare values      
  *        
  *          4. TIM5 interrupts, DMA and flags management
  *                   - Enable/Disable interrupt sources
  *                   - Get flags status
  *                   - Clear flags/ Pending bits
  *                   - Enable/Disable DMA requests 
  *                   - Select CaptureCompare DMA request  
  *              
  *          5. TIM5 clocks management: this group includes all needed functions 
  *             to configure the clock controller unit:
  *                   - Select internal/External clock
  *                   - Select the external clock mode: ETR(Mode1/Mode2) or TIx
  *         
  *          6. TIM5 synchronization management: this group includes all needed 
  *             functions to configure the Synchronization unit:
  *                   - Select Input Trigger  
  *                   - Select Output Trigger  
  *                   - Select Master Slave Mode 
  *                   - ETR Configuration when used as external trigger   
  *     
  *          7. TIM5 specific interface management, this group includes all 
  *             needed functions to use the specific TIM5 interface:
  *                   - Encoder Interface Configuration
  *                   - Select Hall Sensor        
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
#include "stm8l15x_TIM5.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup TIM5 
  * @brief TIM5 driver modules
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void TI1_Config(TIM5_ICPolarity_TypeDef TIM5_ICPolarity,
                       TIM5_ICSelection_TypeDef TIM5_ICSelection,
                       uint8_t TIM5_ICFilter);
static void TI2_Config(TIM5_ICPolarity_TypeDef TIM5_ICPolarity,
                       TIM5_ICSelection_TypeDef TIM5_ICSelection,
                       uint8_t TIM5_ICFilter);


/** @defgroup TIM5_Private_Functions
  * @{
  */

/** @defgroup TIM5_Group1 TimeBase management functions
 *  @brief   TimeBase management functions 
 *
@verbatim   
 ===============================================================================
                       TimeBase management functions
 ===============================================================================  
  
       ===================================================================      
              TIM5 Driver: how to use it in Timing(Time base) Mode
       =================================================================== 
       To use the Timer in Timing(Time base) mode, the following steps are mandatory:
       
       1. Enable TIM5 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM5, ENABLE) function.
        
       2. Call TIM5_TimeBaseInit() to configure the Time Base unit with the
          corresponding configuration.
          
       3. Enable global interrupts if you need to generate the update interrupt.
          
       4. Enable the corresponding interrupt using the function TIM5_ITConfig(TIM5_IT_Update) 
          
       5. Call the TIM5_Cmd(ENABLE) function to enable the TIM5 counter.
       
       Note1: All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 

@endverbatim
  * @{
  */

/**
  * @brief  Deinitialize the TIM5 peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void TIM5_DeInit(void)
{
  TIM5->CR1 = TIM_CR1_RESET_VALUE;
  TIM5->CR2 = TIM_CR2_RESET_VALUE;
  TIM5->SMCR = TIM_SMCR_RESET_VALUE;
  TIM5->ETR = TIM_ETR_RESET_VALUE;
  TIM5->IER = TIM_IER_RESET_VALUE;
  TIM5->SR2 = TIM_SR2_RESET_VALUE;

  /* Disable channels */
  TIM5->CCER1 = TIM_CCER1_RESET_VALUE;
  /* Configure channels as inputs: it is necessary if lock level is equal to 2 or 3 */
  TIM5->CCMR1 = 0x01;/*TIM5_ICxSource_TIxFPx */
  TIM5->CCMR2 = 0x01;/*TIM5_ICxSource_TIxFPx */

  /* Then reset channel registers: it also works if lock level is equal to 2 or 3 */
  TIM5->CCER1 = TIM_CCER1_RESET_VALUE;
  TIM5->CCMR1 = TIM_CCMR1_RESET_VALUE;
  TIM5->CCMR2 = TIM_CCMR2_RESET_VALUE;

  TIM5->CNTRH = TIM_CNTRH_RESET_VALUE;
  TIM5->CNTRL = TIM_CNTRL_RESET_VALUE;

  TIM5->PSCR = TIM_PSCR_RESET_VALUE;

  TIM5->ARRH = TIM_ARRH_RESET_VALUE;
  TIM5->ARRL = TIM_ARRL_RESET_VALUE;

  TIM5->CCR1H = TIM_CCR1H_RESET_VALUE;
  TIM5->CCR1L = TIM_CCR1L_RESET_VALUE;
  TIM5->CCR2H = TIM_CCR2H_RESET_VALUE;
  TIM5->CCR2L = TIM_CCR2L_RESET_VALUE;


  TIM5->OISR = TIM_OISR_RESET_VALUE;
  TIM5->EGR = 0x01;/* TIM_EGR_UG */
  TIM5->BKR = TIM_BKR_RESET_VALUE;
  TIM5->SR1 = TIM_SR1_RESET_VALUE;
}

/**
  * @brief  Initializes the TIM5 Time Base Unit according to the specified  parameters.
  * @param  TIM5_Prescaler: Prescaler 
  *          This parameter can be one of the following values:
  *            @arg TIM5_Prescaler_1: Time base Prescaler = 1 (No effect)
  *            @arg TIM5_Prescaler_2: Time base Prescaler = 2
  *            @arg TIM5_Prescaler_4: Time base Prescaler = 4
  *            @arg TIM5_Prescaler_8: Time base Prescaler = 8
  *            @arg TIM5_Prescaler_16: Time base Prescaler = 16
  *            @arg TIM5_Prescaler_32: Time base Prescaler = 32
  *            @arg TIM5_Prescaler_64: Time base Prescaler = 64
  *            @arg TIM5_Prescaler_128: Time base Prescaler = 128                            
  * @param  TIM5_CounterMode: Counter mode
  *          This parameter can be one of the following values:
  *            @arg TIM5_CounterMode_Up: Counter Up Mode
  *            @arg TIM5_CounterMode_Down: Counter Down Mode
  *            @arg TIM5_CounterMode_CenterAligned1: Counter Central aligned Mode 1
  *            @arg TIM5_CounterMode_CenterAligned2: Counter Central aligned Mode 2
  *            @arg TIM5_CounterMode_CenterAligned3: Counter Central aligned Mode 3        
  * @param  TIM5_Period: This parameter must be a value between 0x0000 and 0xFFFF.
  * @retval None
  */

void TIM5_TimeBaseInit(TIM5_Prescaler_TypeDef TIM5_Prescaler,
                       TIM5_CounterMode_TypeDef TIM5_CounterMode,
                       uint16_t TIM5_Period)
{

  assert_param(IS_TIM5_PRESCALER(TIM5_Prescaler));
  assert_param(IS_TIM5_COUNTER_MODE(TIM5_CounterMode));



  /* Set the Autoreload value */
  TIM5->ARRH = (uint8_t)(TIM5_Period >> 8) ;
  TIM5->ARRL = (uint8_t)(TIM5_Period);

  /* Set the Prescaler value */
  TIM5->PSCR = (uint8_t)(TIM5_Prescaler);

  /* Select the Counter Mode */
  TIM5->CR1 &= (uint8_t)((uint8_t)(~TIM_CR1_CMS)) & ((uint8_t)(~TIM_CR1_DIR));
  TIM5->CR1 |= (uint8_t)(TIM5_CounterMode);

  /* Generate an update event to reload the Prescaler value immediately */
  TIM5->EGR = TIM5_EventSource_Update;
}

/**
  * @brief  Configures the TIM5 Prescaler.
  * @param  Prescaler: Specifies the Prescaler Register value
  *          This parameter can be one of the following values:
  *            @arg TIM5_Prescaler_1: Time base Prescaler = 1 (No effect)
  *            @arg TIM5_Prescaler_2: Time base Prescaler = 2
  *            @arg TIM5_Prescaler_4: Time base Prescaler = 4
  *            @arg TIM5_Prescaler_8: Time base Prescaler = 8
  *            @arg TIM5_Prescaler_16: Time base Prescaler = 16
  *            @arg TIM5_Prescaler_32: Time base Prescaler = 32
  *            @arg TIM5_Prescaler_64: Time base Prescaler = 64
  *            @arg TIM5_Prescaler_128: Time base Prescaler = 128  
  * @param  TIM5_PSCReloadMode: Specifies the TIM5 Prescaler Reload mode.
  *          This parameter can be one of the following values:
  *            @arg TIM5_PSCReloadMode_Update: Prescaler value is reloaded at every update
  *            @arg TIM5_PSCReloadMode_Immediate: Prescaler value is reloaded at every update  
  * @retval None
  */
void TIM5_PrescalerConfig(TIM5_Prescaler_TypeDef Prescaler,
                          TIM5_PSCReloadMode_TypeDef TIM5_PSCReloadMode)
{
  /* Check the parameters */
  assert_param(IS_TIM5_PRESCALER(Prescaler));
  assert_param(IS_TIM5_PRESCALER_RELOAD(TIM5_PSCReloadMode));

  /* Set the Prescaler value */
  TIM5->PSCR = (uint8_t)(Prescaler);

  /* Set or reset the UG Bit */
  if (TIM5_PSCReloadMode == TIM5_PSCReloadMode_Immediate)
  {
    TIM5->EGR |= TIM_EGR_UG ;
  }
  else
  {
    TIM5->EGR &= (uint8_t)(~TIM_EGR_UG) ;
  }
}

/**
  * @brief  Specifies the TIM5 Counter Mode to be used.
  * @param  TIM5_CounterMode: Specifies the Counter Mode to be used
  *          This parameter can be one of the following values:
  *            @arg TIM5_CounterMode_Up: Counter Up Mode
  *            @arg TIM5_CounterMode_Down: Counter Down Mode
  *            @arg TIM5_CounterMode_CenterAligned1: Counter Central aligned Mode 1
  *            @arg TIM5_CounterMode_CenterAligned2: Counter Central aligned Mode 2
  *            @arg TIM5_CounterMode_CenterAligned3: Counter Central aligned Mode 3      
  * @retval None
  */
void TIM5_CounterModeConfig(TIM5_CounterMode_TypeDef TIM5_CounterMode)
{
  uint8_t tmpcr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM5_COUNTER_MODE(TIM5_CounterMode));

  tmpcr1 = TIM5->CR1;

  /* Reset the CMS and DIR Bits */
  tmpcr1 &= (uint8_t)((uint8_t)(~TIM_CR1_CMS) & (uint8_t)(~TIM_CR1_DIR));

  /* Set the Counter Mode */
  tmpcr1 |= (uint8_t)TIM5_CounterMode;

  TIM5->CR1 = tmpcr1;
}

/**
  * @brief  Sets the TIM5 Counter Register value.
  * @param  Counter: Specifies the Counter register new value.
  *          This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM5_SetCounter(uint16_t Counter)
{

  /* Set the Counter Register value */
  TIM5->CNTRH = (uint8_t)(Counter >> 8);
  TIM5->CNTRL = (uint8_t)(Counter);
}

/**
  * @brief  Sets the TIM5 Autoreload Register value.
  * @param  Autoreload: Specifies the Autoreload register new value.
  *          This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM5_SetAutoreload(uint16_t Autoreload)
{
  /* Set the Autoreload Register value */
  TIM5->ARRH = (uint8_t)(Autoreload >> 8);
  TIM5->ARRL = (uint8_t)(Autoreload);
}

/**
  * @brief  Gets the TIM5 Counter value.
  * @param  None
  * @retval Counter Register value.
  */
uint16_t TIM5_GetCounter(void)
{
  uint16_t tmpcnt = 0;
  uint8_t tmpcntrl, tmpcntrh;

  tmpcntrh = TIM5->CNTRH;
  tmpcntrl = TIM5->CNTRL;

  tmpcnt = (uint16_t)(tmpcntrl);
  tmpcnt |= (uint16_t)((uint16_t)tmpcntrh << 8);
  /* Get the Counter Register value */
  return ((uint16_t)tmpcnt);
}

/**
  * @brief  Gets the TIM5 Prescaler value.
  * @param  None
  * @retval TIM5 Prescaler, it can be one of the following values:
  *            - TIM5_Prescaler_1: Time base Prescaler = 1 (No effect)
  *            - TIM5_Prescaler_2: Time base Prescaler = 2
  *            - TIM5_Prescaler_4: Time base Prescaler = 4
  *            - TIM5_Prescaler_8: Time base Prescaler = 8
  *            - TIM5_Prescaler_16: Time base Prescaler = 16
  *            - TIM5_Prescaler_32: Time base Prescaler = 32
  *            - TIM5_Prescaler_64: Time base Prescaler = 64
  *            - TIM5_Prescaler_128: Time base Prescaler = 128    
  */
TIM5_Prescaler_TypeDef TIM5_GetPrescaler(void)
{
  /* Get the Prescaler Register value */
  return ((TIM5_Prescaler_TypeDef)TIM5->PSCR);
}

/**
  * @brief  Enables or Disables the TIM5 Update event.
  * @param  NewState: The new state of the TIM5 peripheral Preload register.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */

void TIM5_UpdateDisableConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the UDIS Bit */
  if (NewState != DISABLE)
  {
    TIM5->CR1 |= TIM_CR1_UDIS;
  }
  else
  {
    TIM5->CR1 &= (uint8_t)(~TIM_CR1_UDIS);
  }
}

/**
  * @brief  Selects the TIM5 Update Request Interrupt source.
  * @param  TIM5_UpdateSource: Specifies the Update source.
  *          This parameter can be one of the following values:
  *            @arg TIM5_UpdateSource_Global: Global Update request source
  *            @arg TIM5_UpdateSource_Regular: Regular Update request source 
  * @retval None
  */
void TIM5_UpdateRequestConfig(TIM5_UpdateSource_TypeDef TIM5_UpdateSource)
{
  /* Check the parameters */
  assert_param(IS_TIM5_UPDATE_SOURCE(TIM5_UpdateSource));

  /* Set or Reset the URS Bit */
  if (TIM5_UpdateSource == TIM5_UpdateSource_Regular)
  {
    TIM5->CR1 |= TIM_CR1_URS ;
  }
  else
  {
    TIM5->CR1 &= (uint8_t)(~TIM_CR1_URS);
  }
}

/**
  * @brief  Enables or disables TIM5 peripheral Preload register on ARR.
  * @param  NewState: The new state of the TIM5 peripheral Preload register.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM5_ARRPreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the ARPE Bit */
  if (NewState != DISABLE)
  {
    TIM5->CR1 |= TIM_CR1_ARPE;
  }
  else
  {
    TIM5->CR1 &= (uint8_t)(~TIM_CR1_ARPE);
  }
}

/**
  * @brief  Selects the TIM’s One Pulse Mode.
  * @param  TIM5_OPMode: Specifies the OPM Mode to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM5_OPMode_Single: Single one Pulse mode (OPM Active)
  *            @arg TIM5_OPMode_Repetitive: Single one Pulse mode (OPM Active)  
  * @retval None
  */
void TIM5_SelectOnePulseMode(TIM5_OPMode_TypeDef TIM5_OPMode)
{
  /* Check the parameters */
  assert_param(IS_TIM5_OPM_MODE(TIM5_OPMode));

  /* Set or Reset the OPM Bit */
  if (TIM5_OPMode == TIM5_OPMode_Single)
  {
    TIM5->CR1 |= TIM_CR1_OPM ;
  }
  else
  {
    TIM5->CR1 &= (uint8_t)(~TIM_CR1_OPM);
  }
}

/**
  * @brief  Enables or disables the TIM5 peripheral.
  * @param  NewState: The new state of the TIM5 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM5_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* set or Reset the CEN Bit */
  if (NewState != DISABLE)
  {
    TIM5->CR1 |= TIM_CR1_CEN;
  }
  else
  {
    TIM5->CR1 &= (uint8_t)(~TIM_CR1_CEN);
  }
}

/**
  * @}
  */

/** @defgroup TIM5_Group2 Output Compare management functions
 *  @brief    Output Compare management functions 
 *
@verbatim   
 ===============================================================================
                        Output Compare management functions
 ===============================================================================  
   
       ===================================================================      
              TIM5 Driver: how to use it in Output Compare Mode
       =================================================================== 
       To use the Timer in Output Compare mode, the following steps are mandatory:
       
       1. Enable TIM5 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM5, ENABLE) function.
       
       2. Configure the TIM5 pins in output mode by configuring the corresponding GPIO pins
          
       3. Configure the Time base unit as described in the first part of this driver, if needed,
          otherwise the Timer will run with the default configuration:
          - Autoreload value = 0xFFFF
          - Prescaler value = 0x0000
          - Counter mode = Up counting
      
       4. Call TIM5_OCxInit() to configure the channel x with the desired parameters
          including:
          - TIM5 Output Compare mode: TIM5_OCMode
          - TIM5 Output State: TIM5_OutputState
          - TIM5 Pulse value: TIM5_Pulse
          - TIM5 Output Compare Polarity: TIM5_OCPolarity
          - TIM5 Output Idle State: TIM5_OCIdleState
       
       5. Call the TIM5_Cmd(ENABLE) function to enable the TIM5 counter.
       
       Note1: All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 
       
       Note2: If the corresponding interrupt or DMA request are needed, the user should:
              1. Enable global interrupts (or the DMA) to use the TIM5 interrupts (or DMA requests). 
              2. Enable the corresponding interrupt (or DMA request) using the function 
              TIM5_ITConfig(TIM5_IT_CCx) (or TIM5_DMACmd(TIM5_DMASource_CCx))   

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the TIM5 Channel1 according to the specified parameters.
  * @param  TIM5_OCMode: Output Compare Mode 
  *          This parameter can be one of the following values:
  *            @arg TIM5_OCMode_Timing: Timing (Frozen) Mode
  *            @arg TIM5_OCMode_Active: Active Mode
  *            @arg TIM5_OCMode_Inactive: Inactive Mode
  *            @arg TIM5_OCMode_Toggle: Toggle Mode
  *            @arg TIM5_OCMode_PWM1: PWM Mode 1
  *            @arg TIM5_OCMode_PWM2: PWM Mode 2          
  * @param  TIM5_OutputState: Output state
  *          This parameter can be one of the following values:
  *            @arg TIM5_OutputState_Disable: Output compare State disabled (channel output disabled)
  *            @arg TIM5_OutputState_Enable: Output compare State enabled (channel output enabled)
  * @param  TIM5_Pulse: This parameter must be a value between 0x0000 and 0xFFFF.
  * @param  TIM5_OCPolarity: Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM5_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM5_OCPolarity_Low: Output compare polarity  = Low 
  * @param  TIM5_OCIdleState: Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM5_OCIdleState_Reset: Output Compare Idle state  = Reset
  *            @arg TIM5_OCIdleState_Set: Output Compare Idle state  = Set
  * @retval None
  */
void TIM5_OC1Init(TIM5_OCMode_TypeDef TIM5_OCMode,
                  TIM5_OutputState_TypeDef TIM5_OutputState,
                  uint16_t TIM5_Pulse,
                  TIM5_OCPolarity_TypeDef TIM5_OCPolarity,
                  TIM5_OCIdleState_TypeDef TIM5_OCIdleState)
{
  uint8_t tmpccmr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM5_OC_MODE(TIM5_OCMode));
  assert_param(IS_TIM5_OUTPUT_STATE(TIM5_OutputState));
  assert_param(IS_TIM5_OC_POLARITY(TIM5_OCPolarity));
  assert_param(IS_TIM5_OCIDLE_STATE(TIM5_OCIdleState));

  tmpccmr1 = TIM5->CCMR1;

  /* Disable the Channel 1: Reset the CCE Bit */
  TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E);
  /* Reset the Output Compare Bits */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Set the Output Compare Mode */
  tmpccmr1 |= (uint8_t)TIM5_OCMode;

  TIM5->CCMR1 = tmpccmr1;

  /* Set the Output State */
  if (TIM5_OutputState == TIM5_OutputState_Enable)
  {
    TIM5->CCER1 |= TIM_CCER1_CC1E;
  }
  else
  {
    TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E);
  }

  /* Set the Output Polarity */
  if (TIM5_OCPolarity == TIM5_OCPolarity_Low)
  {
    TIM5->CCER1 |= TIM_CCER1_CC1P;
  }
  else
  {
    TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P);
  }

  /* Set the Output Idle state */
  if (TIM5_OCIdleState == TIM5_OCIdleState_Set)
  {
    TIM5->OISR |= TIM_OISR_OIS1;
  }
  else
  {
    TIM5->OISR &= (uint8_t)(~TIM_OISR_OIS1);
  }

  /* Set the Pulse value */
  TIM5->CCR1H = (uint8_t)(TIM5_Pulse >> 8);
  TIM5->CCR1L = (uint8_t)(TIM5_Pulse);
}

/**
  * @brief  Initializes the TIM5 Channel2 according to the specified parameters.
  * @param  TIM5_OCMode: Output Compare Mode 
  *          This parameter can be one of the following values:
  *            @arg TIM5_OCMode_Timing: Timing (Frozen) Mode
  *            @arg TIM5_OCMode_Active: Active Mode
  *            @arg TIM5_OCMode_Inactive: Inactive Mode
  *            @arg TIM5_OCMode_Toggle: Toggle Mode
  *            @arg TIM5_OCMode_PWM1: PWM Mode 1
  *            @arg TIM5_OCMode_PWM2: PWM Mode 2          
  * @param  TIM5_OutputState: Output state
  *          This parameter can be one of the following values:
  *            @arg TIM5_OutputState_Disable: Output compare State disabled (channel output disabled)
  *            @arg TIM5_OutputState_Enable: Output compare State enabled (channel output enabled)
  * @param  TIM5_Pulse: This parameter must be a value between 0x0000 and 0xFFFF.
  * @param  TIM5_OCPolarity: Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM5_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM5_OCPolarity_Low: Output compare polarity  = Low 
  * @param  TIM5_OCIdleState: Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM5_OCIdleState_Reset: Output Compare Idle state  = Reset
  *            @arg TIM5_OCIdleState_Set: Output Compare Idle state  = Set
  * @retval None
  */
void TIM5_OC2Init(TIM5_OCMode_TypeDef TIM5_OCMode,
                  TIM5_OutputState_TypeDef TIM5_OutputState,
                  uint16_t TIM5_Pulse,
                  TIM5_OCPolarity_TypeDef TIM5_OCPolarity,
                  TIM5_OCIdleState_TypeDef TIM5_OCIdleState)
{
  uint8_t tmpccmr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM5_OC_MODE(TIM5_OCMode));
  assert_param(IS_TIM5_OUTPUT_STATE(TIM5_OutputState));
  assert_param(IS_TIM5_OC_POLARITY(TIM5_OCPolarity));
  assert_param(IS_TIM5_OCIDLE_STATE(TIM5_OCIdleState));

  tmpccmr2 = TIM5->CCMR2;

  /* Disable the Channel 2: Reset the CCE Bit */
  TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC2E);

  /* Reset the Output Compare Bits */
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Set the Output Compare Mode */
  tmpccmr2 |= (uint8_t)TIM5_OCMode;

  TIM5->CCMR2 = tmpccmr2;

  /* Set the Output State */
  if (TIM5_OutputState == TIM5_OutputState_Enable)
  {
    TIM5->CCER1 |= TIM_CCER1_CC2E;
  }
  else
  {
    TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC2E);
  }

  /* Set the Output Polarity */
  if (TIM5_OCPolarity == TIM5_OCPolarity_Low)
  {
    TIM5->CCER1 |= TIM_CCER1_CC2P;
  }
  else
  {
    TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC2P);
  }


  /* Set the Output Idle state */
  if (TIM5_OCIdleState == TIM5_OCIdleState_Set)
  {
    TIM5->OISR |= TIM_OISR_OIS2;
  }
  else
  {
    TIM5->OISR &= (uint8_t)(~TIM_OISR_OIS2);
  }

  /* Set the Pulse value */
  TIM5->CCR2H = (uint8_t)(TIM5_Pulse >> 8);
  TIM5->CCR2L = (uint8_t)(TIM5_Pulse);
}

/**
  * @brief  Configures the Break feature, dead time, Lock level, the OSSI,
  *         and the AOE(automatic output enable).
  * @param  TIM5_OSSIState: Off-State Selection for Idle mode states.
  *          This parameter can be one of the following values:
  *            @arg TIM5_OSSIState_Enable: Off-State Selection for Idle mode enabled
  *            @arg TIM5_OSSIState_Disable: Off-State Selection for Idle mode disabled 
  * @param  TIM5_LockLevel: Lock level.
  *          This parameter can be one of the following values:
  *            @arg TIM5_LockLevel_Off: Lock option disabled
  *            @arg TIM5_LockLevel_1: Select Lock Level 1
  *            @arg TIM5_LockLevel_2: Select Lock Level 2
  *            @arg TIM5_LockLevel_3: Select Lock Level 3    
  * @param  TIM5_BreakState: Break Input enable/disable .
  *          This parameter can be one of the following values:
  *            @arg TIM5_BreakState_Disable: Break State disabled (break option disabled)
  *            @arg TIM5_BreakState_Enable: Break State enabled (break option enabled) 
  * @param  TIM5_BreakPolarity: Break Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM5_BreakPolarity_High: if Break, channel polarity = High
  *            @arg TIM5_BreakPolarity_Low: if Break, channel polarity = Low   
  * @param  TIM5_AutomaticOutput: TIM5 AOE Bit Set/Reset .
  *          This parameter can be one of the following values:
  *            @arg TIM5_AutomaticOutput_Enable: Automatic Output option enabled
  *            @arg TIM5_AutomaticOutput_Disable: Automatic Output option disabled
  * @retval None
  */
void TIM5_BKRConfig(TIM5_OSSIState_TypeDef TIM5_OSSIState,
                    TIM5_LockLevel_TypeDef TIM5_LockLevel,
                    TIM5_BreakState_TypeDef TIM5_BreakState,
                    TIM5_BreakPolarity_TypeDef TIM5_BreakPolarity,
                    TIM5_AutomaticOutput_TypeDef TIM5_AutomaticOutput)

{
  /* Check the parameters */
  assert_param(IS_TIM5_OSSI_STATE(TIM5_OSSIState));
  assert_param(IS_TIM5_LOCK_LEVEL(TIM5_LockLevel));
  assert_param(IS_TIM5_BREAK_STATE(TIM5_BreakState));
  assert_param(IS_TIM5_BREAK_POLARITY(TIM5_BreakPolarity));
  assert_param(IS_TIM5_AUTOMATIC_OUTPUT_STATE(TIM5_AutomaticOutput));



  /* Set the Lock level, the Break enable Bit and the Polarity, the OSSI State,
  the dead time value and the Automatic Output Enable Bit */
  TIM5->BKR = (uint8_t)((uint8_t)((uint8_t)((uint8_t)((uint8_t)TIM5_OSSIState | (uint8_t)TIM5_LockLevel) | \
                                  (uint8_t)((uint8_t)TIM5_BreakState | (uint8_t)TIM5_BreakPolarity)) | \
                                  TIM5_AutomaticOutput));
}

/**
  * @brief  Enables or disables the TIM5 peripheral Main Outputs.
  * @param  NewState: The new state of the TIM5 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM5_CtrlPWMOutputs(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the MOE Bit */

  if (NewState != DISABLE)
  {
    TIM5->BKR |= TIM_BKR_MOE ;
  }
  else
  {
    TIM5->BKR &= (uint8_t)(~TIM_BKR_MOE) ;
  }
}

/**
  * @brief  Selects the TIM5 Output Compare Mode. This function disables the
  *         selected channel before changing the Output Compare Mode. User has to
  *         enable this channel using TIM5_CCxCmd and TIM5_CCxNCmd functions.
  * @param  TIM5_Channel: Specifies the TIM5 Channel.
  *          This parameter can be one of the following values:
  *            @arg TIM5_Channel_1: Channel 1
  *            @arg TIM5_Channel_2: Channel 2  
  * @param  TIM5_OCMode: Specifies the TIM5 Output Compare Mode.
  *          This parameter can be one of the following values:
  *            @arg TIM5_OCMode_Timing: Timing (Frozen) Mode
  *            @arg TIM5_OCMode_Active: Active Mode
  *            @arg TIM5_OCMode_Inactive: Inactive Mode
  *            @arg TIM5_OCMode_Toggle: Toggle Mode
  *            @arg TIM5_OCMode_PWM1: PWM Mode 1
  *            @arg TIM5_OCMode_PWM2: PWM Mode 2    
  * @retval None
  */
void TIM5_SelectOCxM(TIM5_Channel_TypeDef TIM5_Channel,
                     TIM5_OCMode_TypeDef TIM5_OCMode)
{
  /* Check the parameters */
  assert_param(IS_TIM5_CHANNEL(TIM5_Channel));
  assert_param(IS_TIM5_OCM(TIM5_OCMode));

  if (TIM5_Channel == TIM5_Channel_1)
  {
    /* Disable the Channel 1: Reset the CCE Bit */
    TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E);

    /* Reset the Output Compare Bits */
    TIM5->CCMR1 &= (uint8_t)(~TIM_CCMR_OCM);

    /* Set the Output Compare Mode */
    TIM5->CCMR1 |= (uint8_t)TIM5_OCMode;
  }
  else /* if (TIM5_Channel == TIM5_Channel_2) */
  {
    /* Disable the Channel 2: Reset the CCE Bit */
    TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC2E);

    /* Reset the Output Compare Bits */
    TIM5->CCMR2 &= (uint8_t)(~TIM_CCMR_OCM);

    /* Set the Output Compare Mode */
    TIM5->CCMR2 |= (uint8_t)TIM5_OCMode;
  }
}

/**
  * @brief  Sets the TIM5 Capture Compare1 Register value.
  * @param  Compare: Specifies the Capture Compare1 register new value.
  *         This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM5_SetCompare1(uint16_t Compare)
{
  /* Set the Capture Compare1 Register value */
  TIM5->CCR1H = (uint8_t)(Compare >> 8);
  TIM5->CCR1L = (uint8_t)(Compare);
}

/**
  * @brief  Sets the TIM5 Capture Compare2 Register value.
  * @param  Compare: Specifies the Capture Compare2 register new value.
  *         This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM5_SetCompare2(uint16_t Compare)
{
  /* Set the Capture Compare2 Register value */
  TIM5->CCR2H = (uint8_t)(Compare >> 8);
  TIM5->CCR2L = (uint8_t)(Compare);
}

/**
  * @brief  Forces the TIM5 Channel1 output waveform to active or inactive level.
  * @param  TIM5_ForcedAction: Specifies the forced Action to be set to the output waveform.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ForcedAction_Active: Output Reference is forced low 
  *            @arg TIM5_ForcedAction_Inactive: Output Reference is forced high 
  * @retval None
  */
void TIM5_ForcedOC1Config(TIM5_ForcedAction_TypeDef TIM5_ForcedAction)
{
  uint8_t tmpccmr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM5_FORCED_ACTION(TIM5_ForcedAction));

  tmpccmr1 = TIM5->CCMR1;

  /* Reset the OCM Bits */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Configure The Forced output Mode */
  tmpccmr1 |= (uint8_t)TIM5_ForcedAction;

  TIM5->CCMR1 = tmpccmr1;
}

/**
  * @brief  Forces the TIM5 Channel2 output waveform to active or inactive level.
  * @param  TIM5_ForcedAction: Specifies the forced Action to be set to the output waveform.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ForcedAction_Active: Output Reference is forced low 
  *            @arg TIM5_ForcedAction_Inactive: Output Reference is forced high 
  * @retval None
  */
void TIM5_ForcedOC2Config(TIM5_ForcedAction_TypeDef TIM5_ForcedAction)
{
  uint8_t tmpccmr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM5_FORCED_ACTION(TIM5_ForcedAction));

  tmpccmr2 = TIM5->CCMR2;

  /* Reset the OCM Bits */
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Configure The Forced output Mode */
  tmpccmr2 |= (uint8_t)TIM5_ForcedAction;

  TIM5->CCMR2 = tmpccmr2;
}

/**
  * @brief  Enables or disables the TIM5 peripheral Preload Register on CCR1.
  * @param  NewState: The new state of the Capture Compare Preload register.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM5_OC1PreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC1PE Bit */
  if (NewState != DISABLE)
  {
    TIM5->CCMR1 |= TIM_CCMR_OCxPE ;
  }
  else
  {
    TIM5->CCMR1 &= (uint8_t)(~TIM_CCMR_OCxPE) ;
  }
}

/**
  * @brief  Enables or disables the TIM5 peripheral Preload Register on CCR2.
  * @param  NewState: The new state of the Capture Compare Preload register.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM5_OC2PreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC2PE Bit */
  if (NewState != DISABLE)
  {
    TIM5->CCMR2 |= TIM_CCMR_OCxPE ;
  }
  else
  {
    TIM5->CCMR2 &= (uint8_t)(~TIM_CCMR_OCxPE) ;
  }
}

/**
  * @brief  Configures the TIM5 Capture Compare 1 Fast feature.
  * @param  NewState: The new state of the Output Compare Fast Enable bit.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM5_OC1FastConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC1FE Bit */
  if (NewState != DISABLE)
  {
    TIM5->CCMR1 |= TIM_CCMR_OCxFE ;
  }
  else
  {
    TIM5->CCMR1 &= (uint8_t)(~TIM_CCMR_OCxFE) ;
  }
}

/**
  * @brief  Configures the TIM5 Capture Compare 2 Fast feature.
  * @param  NewState: The new state of the Output Compare Fast Enable bit.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */

void TIM5_OC2FastConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC2FE Bit */
  if (NewState != DISABLE)
  {
    TIM5->CCMR2 |= TIM_CCMR_OCxFE ;
  }
  else
  {
    TIM5->CCMR2 &= (uint8_t)(~TIM_CCMR_OCxFE) ;
  }
}

/**
  * @brief  Configures the TIM5 Channel 1 polarity.
  * @param  TIM5_OCPolarity: Specifies the OC1 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM5_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM5_OCPolarity_Low: Output compare polarity  = Low 
  * @retval None
  */
void TIM5_OC1PolarityConfig(TIM5_OCPolarity_TypeDef TIM5_OCPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM5_OC_POLARITY(TIM5_OCPolarity));

  /* Set or Reset the CC1P Bit */
  if (TIM5_OCPolarity == TIM5_OCPolarity_Low)
  {
    TIM5->CCER1 |= TIM_CCER1_CC1P ;
  }
  else
  {
    TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P) ;
  }
}

/**
  * @brief  Configures the TIM5 Channel 2 polarity.
  * @param  TIM5_OCPolarity: Specifies the OC2 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM5_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM5_OCPolarity_Low: Output compare polarity  = Low 
  * @retval None
  */
void TIM5_OC2PolarityConfig(TIM5_OCPolarity_TypeDef TIM5_OCPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM5_OC_POLARITY(TIM5_OCPolarity));

  /* Set or Reset the CC2P Bit */
  if (TIM5_OCPolarity == TIM5_OCPolarity_Low)
  {
    TIM5->CCER1 |= TIM_CCER1_CC2P ;
  }
  else
  {
    TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC2P) ;
  }
}

/**
  * @brief  Enables or disables the TIM5 Capture Compare Channel x.
  * @param  TIM5_Channel: Specifies the TIM5 Channel.
  *          This parameter can be one of the following values:
  *            @arg TIM5_Channel_1: Channel 1
  *            @arg TIM5_Channel_2: Channel 2  
  * @param  NewState: Specifies the TIM5 Channel CCxE bit new state.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM5_CCxCmd(TIM5_Channel_TypeDef TIM5_Channel,
                 FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM5_CHANNEL(TIM5_Channel));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (TIM5_Channel == TIM5_Channel_1)
  {
    /* Set or Reset the CC1E Bit */
    if (NewState != DISABLE)
    {
      TIM5->CCER1 |= TIM_CCER1_CC1E ;
    }
    else
    {
      TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E) ;
    }

  }
  else /* if (TIM5_Channel == TIM5_Channel_2) */
  {
    /* Set or Reset the CC2E Bit */
    if (NewState != DISABLE)
    {
      TIM5->CCER1 |= TIM_CCER1_CC2E;
    }
    else
    {
      TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC2E) ;
    }
  }
}

/** @defgroup TIM5_Group3 Input Capture management functions
 *  @brief    Input Capture management functions 
 *
@verbatim   
 ===============================================================================
                      Input Capture management functions
 ===============================================================================  
   
       ===================================================================      
              TIM5 Driver: how to use it in Input Capture Mode
       =================================================================== 
       To use the Timer in Input Capture mode, the following steps are mandatory:
       
       1. Enable TIM5 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM5, ENABLE) function.
       
       2. Configure the TIM5 pins in input mode by configuring the corresponding GPIO pins
       
       3. Configure the Time base unit as described in the first part of this driver, if needed,
          otherwise the Timer will run with the default configuration:
          - Autoreload value = 0xFFFF
          - Prescaler value = 0x0
          - Counter mode = Up counting
       
       4. Call TIM5_ICInit() to configure the desired channel to measure only 
          frequency or duty cycle of the input signal using the corresponding configuration: 
          - TIM5 Channel: TIM5_Channel
          - TIM5 Input Capture polarity: TIM5_ICPolarity
          - TIM5 Input Capture selection: TIM5_ICSelection
          - TIM5 Input Capture Prescaler: TIM5_ICPSC
          - TIM5 Input Capture filter value
          or,
          Call TIM5_PWMIConfig() to configure the desired channels with the 
          corresponding configuration and to measure the frequency and the duty
          cycle of the input signal.
          
       5. Enable global interrupts or the DMA to read the measured frequency. 
          
       6. Enable the corresponding interrupt (or DMA request) to read the captured value,
          using the function TIM5_ITConfig(TIM5_IT_CCx) (or TIM5_DMACmd(TIM5_DMASource_CCx))
       
       7. Call the TIM5_Cmd(ENABLE) function to enable the TIM5 counter.
       
       8. Use TIM5_GetCapturex() to read the captured value corresponding to
          channel x.
       
       Note1: All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the TIM5 peripheral according to the specified parameters.
  * @param  TIM5_Channel: TIM5 Channel
  *          This parameter can be one of the following values:
  *            @arg TIM5_Channel_1: Channel 1
  *            @arg TIM5_Channel_2: Channel 2     
  * @param  TIM5_ICPolarity: Input Capture Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM5_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM5_ICSelection: Input Capture Selection
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICSelection_DirectTI: Input Capture mapped on the direct input
  *            @arg TIM5_ICSelection_IndirectTI: Input Capture mapped on the indirect input
  *            @arg TIM5_ICSelection_TRGI: Input Capture mapped on the Trigger Input   
  * @param  TIM5_ICPrescaler: Input Capture Prescaler
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM5_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM5_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM5_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events)   
  * @param  TIM5_ICFilter: This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
void TIM5_ICInit(TIM5_Channel_TypeDef TIM5_Channel,
                 TIM5_ICPolarity_TypeDef TIM5_ICPolarity,
                 TIM5_ICSelection_TypeDef TIM5_ICSelection,
                 TIM5_ICPSC_TypeDef TIM5_ICPrescaler,
                 uint8_t TIM5_ICFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM5_CHANNEL(TIM5_Channel));

  if (TIM5_Channel == TIM5_Channel_1)
  {
    /* TI1 Configuration */
    TI1_Config(TIM5_ICPolarity, TIM5_ICSelection, TIM5_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM5_SetIC1Prescaler(TIM5_ICPrescaler);
  }
  else /* if (TIM5_Channel == TIM5_Channel_2) */
  {
    /* TI2 Configuration */
    TI2_Config(TIM5_ICPolarity, TIM5_ICSelection, TIM5_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM5_SetIC2Prescaler(TIM5_ICPrescaler);
  }
}

/**
  * @brief  Configures the TIM5 peripheral in PWM Input Mode according to the
  *         specified parameters.
  * @param  TIM5_Channel: TIM5 Channel
  *          This parameter can be one of the following values:
  *            @arg TIM5_Channel_1: Channel 1
  *            @arg TIM5_Channel_2: Channel 2     
  * @param  TIM5_ICPolarity: Input Capture Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM5_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM5_ICSelection: Input Capture Selection
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICSelection_DirectTI: Input Capture mapped on the direct input
  *            @arg TIM5_ICSelection_IndirectTI: Input Capture mapped on the indirect input
  *            @arg TIM5_ICSelection_TRGI: Input Capture mapped on the Trigger Input   
  * @param  TIM5_ICPrescaler: Input Capture Prescaler
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM5_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM5_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM5_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events) 
  * @retval None
  */
void TIM5_PWMIConfig(TIM5_Channel_TypeDef TIM5_Channel,
                     TIM5_ICPolarity_TypeDef TIM5_ICPolarity,
                     TIM5_ICSelection_TypeDef TIM5_ICSelection,
                     TIM5_ICPSC_TypeDef TIM5_ICPrescaler,
                     uint8_t TIM5_ICFilter)
{
  uint8_t icpolarity = TIM5_ICPolarity_Rising;
  uint8_t icselection = TIM5_ICSelection_DirectTI;

  /* Check the parameters */
  assert_param(IS_TIM5_CHANNEL(TIM5_Channel));

  /* Select the Opposite Input Polarity */
  if (TIM5_ICPolarity == TIM5_ICPolarity_Rising)
  {
    icpolarity = TIM5_ICPolarity_Falling;
  }
  else
  {
    icpolarity = TIM5_ICPolarity_Rising;
  }

  /* Select the Opposite Input */
  if (TIM5_ICSelection == TIM5_ICSelection_DirectTI)
  {
    icselection = TIM5_ICSelection_IndirectTI;
  }
  else
  {
    icselection = TIM5_ICSelection_DirectTI;
  }

  if (TIM5_Channel == TIM5_Channel_1)
  {
    /* TI1 Configuration */
    TI1_Config(TIM5_ICPolarity, TIM5_ICSelection,
               TIM5_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM5_SetIC1Prescaler(TIM5_ICPrescaler);

    /* TI2 Configuration */
    TI2_Config((TIM5_ICPolarity_TypeDef)icpolarity, (TIM5_ICSelection_TypeDef)icselection, TIM5_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM5_SetIC2Prescaler(TIM5_ICPrescaler);
  }
  else
  {
    /* TI2 Configuration */
    TI2_Config(TIM5_ICPolarity, TIM5_ICSelection,
               TIM5_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM5_SetIC2Prescaler(TIM5_ICPrescaler);

    /* TI1 Configuration */
    TI1_Config((TIM5_ICPolarity_TypeDef)icpolarity, (TIM5_ICSelection_TypeDef)icselection, TIM5_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM5_SetIC1Prescaler(TIM5_ICPrescaler);
  }
}

/**
  * @brief  Gets the TIM5 Input Capture 1 value.
  * @param  None
  * @retval Capture Compare 1 Register value.
  */
uint16_t TIM5_GetCapture1(void)
{
  uint16_t tmpccr1 = 0;
  uint8_t tmpccr1l, tmpccr1h;

  tmpccr1h = TIM5->CCR1H;
  tmpccr1l = TIM5->CCR1L;

  tmpccr1 = (uint16_t)(tmpccr1l);
  tmpccr1 |= (uint16_t)((uint16_t)tmpccr1h << 8);
  /* Get the Capture 1 Register value */
  return ((uint16_t)tmpccr1);
}

/**
  * @brief  Gets the TIM5 Input Capture 2 value.
  * @param  None
  * @retval Capture Compare 2 Register value.
  */
uint16_t TIM5_GetCapture2(void)
{
  uint16_t tmpccr2 = 0;
  uint8_t tmpccr2l, tmpccr2h;

  tmpccr2h = TIM5->CCR2H;
  tmpccr2l = TIM5->CCR2L;

  tmpccr2 = (uint16_t)(tmpccr2l);
  tmpccr2 |= (uint16_t)((uint16_t)tmpccr2h << 8);
  /* Get the Capture 2 Register value */
  return ((uint16_t)tmpccr2);
}

/**
  * @brief  Sets the TIM5 Input Capture 1 prescaler.
  * @param  TIM5_IC1Prescaler: Specifies the Input Capture prescaler new value
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM5_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM5_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM5_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events) 
  * @retval None
  */
void TIM5_SetIC1Prescaler(TIM5_ICPSC_TypeDef TIM5_IC1Prescaler)
{
  uint8_t tmpccmr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM5_IC_PRESCALER(TIM5_IC1Prescaler));

  tmpccmr1 = TIM5->CCMR1;

  /* Reset the IC1PSC Bits */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_ICxPSC);

  /* Set the IC1PSC value */
  tmpccmr1 |= (uint8_t)TIM5_IC1Prescaler;

  TIM5->CCMR1 = tmpccmr1;
}

/**
  * @brief  Sets the TIM5 Input Capture 2 prescaler.
  * @param  TIM5_IC2Prescaler: Specifies the Input Capture prescaler new value
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM5_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM5_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM5_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events) 
  * @retval None
  */
void TIM5_SetIC2Prescaler(TIM5_ICPSC_TypeDef TIM5_IC2Prescaler)
{
  uint8_t tmpccmr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM5_IC_PRESCALER(TIM5_IC2Prescaler));

  tmpccmr2 = TIM5->CCMR2;

  /* Reset the IC2PSC Bits */
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_ICxPSC);

  /* Set the IC2PSC value */
  tmpccmr2 |= (uint8_t)TIM5_IC2Prescaler;

  TIM5->CCMR2 = tmpccmr2;
}

/**
  * @}
  */

/** @defgroup TIM5_Group4 Interrupts DMA and flags management functions
 *  @brief    Interrupts, DMA and flags management functions 
 *
@verbatim   
 ===============================================================================
                 Interrupts, DMA and flags management functions
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified TIM5 interrupts.
  * @param  TIM5_IT: Specifies the TIM5 interrupts sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg TIM5_IT_Update: Update
  *            @arg TIM5_IT_CC1: Capture Compare Channel1
  *            @arg TIM5_IT_CC2: Capture Compare Channel2 
  *            @arg TIM5_IT_Trigger: Trigger 
  *            @arg TIM5_IT_Break: Break  
  * @param  NewState: The new state of the TIM5 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM5_ITConfig(TIM5_IT_TypeDef TIM5_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM5_IT(TIM5_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the Interrupt sources */
    TIM5->IER |= (uint8_t)TIM5_IT;
  }
  else
  {
    /* Disable the Interrupt sources */
    TIM5->IER &= (uint8_t)(~(uint8_t)TIM5_IT);
  }
}

/**
  * @brief  Configures the TIM5 event to be generated by software.
  * @param  TIM5_EventSource: Specifies the event source.
  *          This parameter can be any combination of the following values:
  *            @arg TIM5_EventSource_Update: Update
  *            @arg TIM5_EventSource_CC1: Capture Compare Channel1
  *            @arg TIM5_EventSource_CC2: Capture Compare Channel2 
  *            @arg TIM5_EventSource_Trigger: Trigger 
  *            @arg TIM5_EventSource_Break: Break  
  * @retval None
  */
void TIM5_GenerateEvent(TIM5_EventSource_TypeDef TIM5_EventSource)
{
  /* Check the parameters */
  assert_param(IS_TIM5_EVENT_SOURCE((uint8_t)TIM5_EventSource));

  /* Set the event sources */
  TIM5->EGR |= (uint8_t)TIM5_EventSource;
}

/**
  * @brief  Checks whether the specified TIM5 flag is set or not.
  * @param  TIM5_FLAG: Specifies the flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg TIM5_FLAG_Update: Update
  *            @arg TIM5_FLAG_CC1: Capture Compare Channel1
  *            @arg TIM5_FLAG_CC2: Capture Compare Channel2 
  *            @arg TIM5_FLAG_Trigger: Trigger 
  *            @arg TIM5_FLAG_Break: Break  
  *            @arg TIM5_FLAG_CC1OF: Capture compare 1 over capture
  *            @arg TIM5_FLAG_CC2OF: Capture compare 2 over capture   
  * @retval FlagStatus: The new state of TIM5_FLAG (SET or RESET)
  */
FlagStatus TIM5_GetFlagStatus(TIM5_FLAG_TypeDef TIM5_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint8_t tim5_flag_l = 0, tim5_flag_h = 0;

  /* Check the parameters */
  assert_param(IS_TIM5_GET_FLAG(TIM5_FLAG));

  tim5_flag_l = (uint8_t)(TIM5->SR1 & (uint8_t)(TIM5_FLAG));
  tim5_flag_h = (uint8_t)(TIM5->SR2 & (uint8_t)((uint16_t)TIM5_FLAG >> 8));

  if ((uint8_t)(tim5_flag_l | tim5_flag_h) != 0)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return ((FlagStatus)bitstatus);
}

/**
  * @brief  Clears the TIM’s pending flags.
  * @param  TIM5_FLAG: Specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM5_FLAG_Update: Update
  *            @arg TIM5_FLAG_CC1: Capture Compare Channel1
  *            @arg TIM5_FLAG_CC2: Capture Compare Channel2 
  *            @arg TIM5_FLAG_Trigger: Trigger 
  *            @arg TIM5_FLAG_Break: Break  
  * @retval None
  */
void TIM5_ClearFlag(TIM5_FLAG_TypeDef TIM5_FLAG)
{
  /* Check the parameters */
  assert_param(IS_TIM5_CLEAR_FLAG((uint16_t)TIM5_FLAG));
  /* Clear the flags (rc_w0) clear this bit by writing 0. Writing ‘1’ has no effect*/
  TIM5->SR1 = (uint8_t)(~(uint8_t)(TIM5_FLAG));
  TIM5->SR2 = (uint8_t)(~(uint8_t)((uint16_t)TIM5_FLAG >> 8));
}

/**
  * @brief  Checks whether the TIM5 interrupt has occurred or not.
  * @param  TIM5_IT: Specifies the TIM5 interrupt source to check.
  *          This parameter can be any combination of the following values:
  *            @arg TIM5_IT_Update: Update
  *            @arg TIM5_IT_CC1: Capture Compare Channel1
  *            @arg TIM5_IT_CC2: Capture Compare Channel2 
  *            @arg TIM5_IT_Trigger: Trigger 
  *            @arg TIM5_IT_Break: Break  
  * @retval ITStatus: The new state of the TIM5_IT (SET or RESET)
  */
ITStatus TIM5_GetITStatus(TIM5_IT_TypeDef TIM5_IT)
{
  ITStatus bitstatus = RESET;

  uint8_t TIM5_itStatus = 0x0, TIM5_itEnable = 0x0;

  /* Check the parameters */
  assert_param(IS_TIM5_GET_IT(TIM5_IT));

  TIM5_itStatus = (uint8_t)(TIM5->SR1 & (uint8_t)TIM5_IT);

  TIM5_itEnable = (uint8_t)(TIM5->IER & (uint8_t)TIM5_IT);

  if ((TIM5_itStatus != (uint8_t)RESET ) && (TIM5_itEnable != (uint8_t)RESET))
  {
    bitstatus = (ITStatus)SET;
  }
  else
  {
    bitstatus = (ITStatus)RESET;
  }
  return ((ITStatus)bitstatus);
}

/**
  * @brief  Clears the TIM's interrupt pending bits.
  * @param  TIM5_IT: Specifies the pending bit to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM5_IT_Update: Update
  *            @arg TIM5_IT_CC1: Capture Compare Channel1
  *            @arg TIM5_IT_CC2: Capture Compare Channel2 
  *            @arg TIM5_IT_Trigger: Trigger 
  *            @arg TIM5_IT_Break: Break  
  * @retval None
  */
void TIM5_ClearITPendingBit(TIM5_IT_TypeDef TIM5_IT)
{
  /* Check the parameters */
  assert_param(IS_TIM5_IT(TIM5_IT));

  /* Clear the IT pending Bit */
  TIM5->SR1 = (uint8_t)(~(uint8_t)TIM5_IT);
}

/**
  * @brief  Enables or disables the TIM5 DMA Requests.
  * @param  TIM5_DMASource: specifies the DMA Request sources.
  *          This parameter can be any combination of the following values:
  *            @arg TIM5_DMASource_Update: TIM5 DMA Update Request
  *            @arg TIM5_DMASource_CC1: TIM5 DMA CC1 Request
  *            @arg TIM5_DMASource_CC2: TIM5 DMA CC2 Request 
  * @param  NewState: new state of the DMA Request sources.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM5_DMACmd( TIM5_DMASource_TypeDef TIM5_DMASource, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  assert_param(IS_TIM5_DMA_SOURCE(TIM5_DMASource));

  if (NewState != DISABLE)
  {
    /* Enable the DMA sources */
    TIM5->DER |= TIM5_DMASource;
  }
  else
  {
    /* Disable the DMA sources */
    TIM5->DER &= (uint8_t)(~TIM5_DMASource);
  }
}

/**
  * @brief  Selects the TIM5 peripheral Capture Compare DMA source.
  * @param   NewState: new state of the Capture Compare DMA source.
  *           This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM5_SelectCCDMA(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Set the CCDS Bit */
    TIM5->CR2 |= TIM_CR2_CCDS;
  }
  else
  {
    /* Reset the CCDS Bit */
    TIM5->CR2 &= (uint8_t)(~TIM_CR2_CCDS);
  }
}

/**
  * @}
  */

/** @defgroup TIM5_Group5 Clocks management functions
 *  @brief    Clocks management functions
 *
@verbatim   
 ===============================================================================
                         Clocks management functions
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Enables the TIM5 internal Clock.
  * @par Parameters:
  * None
  * @retval None
  */
void TIM5_InternalClockConfig(void)
{
  /* Disable slave mode to clock the prescaler directly with the internal clock */
  TIM5->SMCR &=  (uint8_t)(~TIM_SMCR_SMS);
}

/**
  * @brief  Configures the TIM5 Trigger as External Clock.
  * @param  TIM5_TIxExternalCLKSource: Specifies Trigger source.
  *          This parameter can be one of the following values:
  *            @arg TIM5_TIxExternalCLK1Source_TI1ED: External Clock mode 1 source = TI1ED
  *            @arg TIM5_TIxExternalCLK1Source_TI1: External Clock mode 1 source = TI1 
  *            @arg TIM5_TIxExternalCLK1Source_TI2: External Clock mode 1 source = TI2  
  * @param  TIM5_ICPolarity: Specifies the TIx Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM5_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  ICFilter: Specifies the filter value.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM5_TIxExternalClockConfig(TIM5_TIxExternalCLK1Source_TypeDef TIM5_TIxExternalCLKSource,
                                 TIM5_ICPolarity_TypeDef TIM5_ICPolarity,
                                 uint8_t ICFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM5_TIXCLK_SOURCE(TIM5_TIxExternalCLKSource));
  assert_param(IS_TIM5_IC_POLARITY(TIM5_ICPolarity));
  assert_param(IS_TIM5_IC_FILTER(ICFilter));

  /* Configure the TIM5 Input Clock Source */
  if (TIM5_TIxExternalCLKSource == TIM5_TIxExternalCLK1Source_TI2)
  {
    TI2_Config(TIM5_ICPolarity, TIM5_ICSelection_DirectTI, ICFilter);
  }
  else
  {
    TI1_Config(TIM5_ICPolarity, TIM5_ICSelection_DirectTI, ICFilter);
  }

  /* Select the Trigger source */
  TIM5_SelectInputTrigger((TIM5_TRGSelection_TypeDef)TIM5_TIxExternalCLKSource);

  /* Select the External clock mode1 */
  TIM5->SMCR |= (uint8_t)(TIM5_SlaveMode_External1);
}

/**
  * @brief  Configures the TIM5 External clock Mode1.
  * @param  TIM5_ExtTRGPrescaler: Specifies the external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ExtTRGPSC_OFF: No External Trigger prescaler
  *            @arg TIM5_ExtTRGPSC_DIV2: External Trigger prescaler = 2 (ETRP frequency divided by 2)
  *            @arg TIM5_ExtTRGPSC_DIV4: External Trigger prescaler = 4 (ETRP frequency divided by 4) 
  *            @arg TIM5_ExtTRGPSC_DIV8: External Trigger prescaler = 8 (ETRP frequency divided by 8)   
  * @param  TIM5_ExtTRGPolarity: Specifies the external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ExtTRGPolarity_Inverted: External Trigger Polarity = inverted
  *            @arg TIM5_ExtTRGPolarity_NonInverted: External Trigger Polarity = non inverted
  * @param  ExtTRGFilter: Specifies the External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM5_ETRClockMode1Config(TIM5_ExtTRGPSC_TypeDef TIM5_ExtTRGPrescaler,
                              TIM5_ExtTRGPolarity_TypeDef TIM5_ExtTRGPolarity,
                              uint8_t ExtTRGFilter)
{
  /* Configure the ETR Clock source */
  TIM5_ETRConfig(TIM5_ExtTRGPrescaler, TIM5_ExtTRGPolarity, ExtTRGFilter);

  /* Select the External clock mode1 */
  TIM5->SMCR &= (uint8_t)(~TIM_SMCR_SMS);
  TIM5->SMCR |= (uint8_t)(TIM5_SlaveMode_External1);

  /* Select the Trigger selection: ETRF */
  TIM5->SMCR &= (uint8_t)(~TIM_SMCR_TS);
  TIM5->SMCR |= (uint8_t)((TIM5_TRGSelection_TypeDef)TIM5_TRGSelection_ETRF);
}

/**
  * @brief  Configures the TIM5 External clock Mode2.
  * @param  TIM5_ExtTRGPrescaler: Specifies the external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ExtTRGPSC_OFF: No External Trigger prescaler
  *            @arg TIM5_ExtTRGPSC_DIV2: External Trigger prescaler = 2 (ETRP frequency divided by 2)
  *            @arg TIM5_ExtTRGPSC_DIV4: External Trigger prescaler = 4 (ETRP frequency divided by 4) 
  *            @arg TIM5_ExtTRGPSC_DIV8: External Trigger prescaler = 8 (ETRP frequency divided by 8) 
  * @param  TIM5_ExtTRGPolarity: Specifies the external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ExtTRGPolarity_Inverted: External Trigger Polarity = inverted
  *            @arg TIM5_ExtTRGPolarity_NonInverted: External Trigger Polarity = non inverted
  * @param  ExtTRGFilter: Specifies the External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM5_ETRClockMode2Config(TIM5_ExtTRGPSC_TypeDef TIM5_ExtTRGPrescaler,
                              TIM5_ExtTRGPolarity_TypeDef TIM5_ExtTRGPolarity,
                              uint8_t ExtTRGFilter)
{
  /* Configure the ETR Clock source */
  TIM5_ETRConfig(TIM5_ExtTRGPrescaler, TIM5_ExtTRGPolarity, ExtTRGFilter);

  /* Enable the External clock mode2 */
  TIM5->ETR |= TIM_ETR_ECE ;
}

/**
  * @}
  */

/** @defgroup TIM5_Group6 Synchronization management functions
 *  @brief    Synchronization management functions 
 *
@verbatim   
 ===============================================================================
                       Synchronization management functions
 ===============================================================================  
                   
       ===================================================================      
              TIM5 Driver: how to use it in synchronization Mode
       =================================================================== 
       Case of two/several Timers
       **************************
       1. If TIM5 is used as master to other timers use the following functions:
          - TIM5_SelectOutputTrigger()
          - TIM5_SelectMasterSlaveMode()
       2. If TIM5 is used as slave to other timers use the following functions:
          - TIM5_SelectInputTrigger()
          - TIM5_SelectSlaveMode()
          
       Case of Timers and external trigger (TRIG pin)
       ********************************************       
       1. Configure the External trigger using TIM5_ETRConfig()
       2. Configure the Slave Timer using the following functions:
          - TIM5_SelectInputTrigger()
          - TIM5_SelectSlaveMode()

@endverbatim
  * @{
  */

/**
  * @brief  Selects the TIM5 Input Trigger source.
  * @param  TIM5_InputTriggerSource: Specifies Input Trigger source.
  *          This parameter can be one of the following values:
  *            @arg TIM5_TRGSelection_TIM4: TRIG Input source =  TIM TRIG Output
  *            @arg TIM5_TRGSelection_TIM1: TRIG Input source =  TIM TRIG Output
  *            @arg TIM5_TRGSelection_TIM3: TRIG Input source =  TIM TRIG Output
  *            @arg TIM5_TRGSelection_TIM2: TRIG Input source =  TIM TRIG Output
  *            @arg TIM5_TRGSelection_TI1F_ED: TRIG Input source = TI1F_ED (TI1 Edge Detector)
  *            @arg TIM5_TRGSelection_TI1FP1: TRIG Input source = TI1FP1 (Filtered Timer Input 1)
  *            @arg TIM5_TRGSelection_TI2FP2: TRIG Input source = TI2FP2 (Filtered Timer Input 2)
  *            @arg TIM5_TRGSelection_ETRF: TRIG Input source =  ETRF (External Trigger Input )      
  * @retval None
  */
void TIM5_SelectInputTrigger(TIM5_TRGSelection_TypeDef TIM5_InputTriggerSource)
{
  uint8_t tmpsmcr = 0;

  /* Check the parameters */
  assert_param(IS_TIM5_TRIGGER_SELECTION(TIM5_InputTriggerSource));

  tmpsmcr = TIM5->SMCR;

  /* Select the Trigger Source */
  tmpsmcr &= (uint8_t)(~TIM_SMCR_TS);
  tmpsmcr |= (uint8_t)TIM5_InputTriggerSource;

  TIM5->SMCR = (uint8_t)tmpsmcr;
}

/**
  * @brief  Selects the TIM5 Trigger Output Mode.
  * @param  TIM5_TRGOSource: Specifies the Trigger Output source.
  *          This parameter can be one of the following values:
  *            @arg TIM5_TRGOSource_Reset: Trigger Output source = Reset 
  *            @arg TIM5_TRGOSource_Enable: Trigger Output source = TIM5 is enabled 
  *            @arg TIM5_TRGOSource_Update: Trigger Output source = Update event
  *            @arg TIM5_TRGOSource_OC1: Trigger Output source = output compare channel1
  *            @arg TIM5_TRGOSource_OC1REF: Trigger Output source = output compare channel 1 reference
  *            @arg TIM5_TRGOSource_OC2REF: Trigger Output source = output compare channel 2 reference   
  * @retval None
  */
void TIM5_SelectOutputTrigger(TIM5_TRGOSource_TypeDef TIM5_TRGOSource)
{
  uint8_t tmpcr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM5_TRGO_SOURCE(TIM5_TRGOSource));

  tmpcr2 = TIM5->CR2;

  /* Reset the MMS Bits */
  tmpcr2 &= (uint8_t)(~TIM_CR2_MMS);

  /* Select the TRGO source */
  tmpcr2 |=  (uint8_t)TIM5_TRGOSource;

  TIM5->CR2 = tmpcr2;
}

/**
  * @brief  Selects the TIM5 Slave Mode.
  * @param  TIM5_SlaveMode: Specifies the TIM5 Slave Mode.
  *          This parameter can be one of the following values:
  *            @arg TIM5_SlaveMode_Reset: Slave Mode Selection  = Reset
  *            @arg TIM5_SlaveMode_Gated: Slave Mode Selection  = Gated
  *            @arg TIM5_SlaveMode_Trigger: Slave Mode Selection  = Trigger
  *            @arg TIM5_SlaveMode_External1: Slave Mode Selection  = External 1  
  * @retval None
  */
void TIM5_SelectSlaveMode(TIM5_SlaveMode_TypeDef TIM5_SlaveMode)
{
  uint8_t tmpsmcr = 0;

  /* Check the parameters */
  assert_param(IS_TIM5_SLAVE_MODE(TIM5_SlaveMode));

  tmpsmcr = TIM5->SMCR;

  /* Reset the SMS Bits */
  tmpsmcr &= (uint8_t)(~TIM_SMCR_SMS);

  /* Select the Slave Mode */
  tmpsmcr |= (uint8_t)TIM5_SlaveMode;

  TIM5->SMCR = tmpsmcr;
}

/**
  * @brief  Sets or Resets the TIM5 Master/Slave Mode.
  * @param  NewState: The new state of the synchronization between TIM5 and its slaves (through TRGO).
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM5_SelectMasterSlaveMode(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the MSM Bit */
  if (NewState != DISABLE)
  {
    TIM5->SMCR |= TIM_SMCR_MSM;
  }
  else
  {
    TIM5->SMCR &= (uint8_t)(~TIM_SMCR_MSM);
  }
}

/**
  * @brief  Configures the TIM5 External Trigger.
  * @param  TIM5_ExtTRGPrescaler: Specifies the external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ExtTRGPSC_OFF: No External Trigger prescaler
  *            @arg TIM5_ExtTRGPSC_DIV2: External Trigger prescaler = 2 (ETRP frequency divided by 2)
  *            @arg TIM5_ExtTRGPSC_DIV4: External Trigger prescaler = 4 (ETRP frequency divided by 4)
  *            @arg TIM5_ExtTRGPSC_DIV8: External Trigger prescaler = 8 (ETRP frequency divided by 8)    
  * @param  TIM5_ExtTRGPolarity: Specifies the external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ExtTRGPolarity_Inverted: External Trigger Polarity = inverted
  *            @arg TIM5_ExtTRGPolarity_NonInverted: External Trigger Polarity = non inverted
  * @param  ExtTRGFilter: Specifies the External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM5_ETRConfig(TIM5_ExtTRGPSC_TypeDef TIM5_ExtTRGPrescaler,
                    TIM5_ExtTRGPolarity_TypeDef TIM5_ExtTRGPolarity,
                    uint8_t ExtTRGFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM5_EXT_PRESCALER(TIM5_ExtTRGPrescaler));
  assert_param(IS_TIM5_EXT_POLARITY(TIM5_ExtTRGPolarity));
  assert_param(IS_TIM5_EXT_FILTER(ExtTRGFilter));

  /* Set the Prescaler, the Filter value and the Polarity */
  TIM5->ETR |= (uint8_t)((uint8_t)((uint8_t)TIM5_ExtTRGPrescaler | (uint8_t)TIM5_ExtTRGPolarity)
                         | (uint8_t)ExtTRGFilter);
}

/**
  * @}
  */

/** @defgroup TIM5_Group7 Specific interface management functions
 *  @brief    Specific interface management functions 
 *
@verbatim   
 ===============================================================================
                    Specific interface management functions
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Configures the TIM5 Encoder Interface.
  * @param  TIM5_EncoderMode: Specifies the TIM5 Encoder Mode.
  *          This parameter can be one of the following values:
  *            @arg TIM5_EncoderMode_TI1: Encoder mode 1
  *            @arg TIM5_EncoderMode_TI2: Encoder mode 2
  *            @arg TIM5_EncoderMode_TI12: Encoder mode 3   
  * @param  TIM5_IC1Polarity: Specifies the IC1 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM5_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM5_IC2Polarity: Specifies the IC2 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM5_ICPolarity_Falling: Input Capture on Falling Edge  
  * @retval None
  */
void TIM5_EncoderInterfaceConfig(TIM5_EncoderMode_TypeDef TIM5_EncoderMode,
                                 TIM5_ICPolarity_TypeDef TIM5_IC1Polarity,
                                 TIM5_ICPolarity_TypeDef TIM5_IC2Polarity)
{
  uint8_t tmpsmcr = 0;
  uint8_t tmpccmr1 = 0;
  uint8_t tmpccmr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM5_ENCODER_MODE(TIM5_EncoderMode));
  assert_param(IS_TIM5_IC_POLARITY(TIM5_IC1Polarity));
  assert_param(IS_TIM5_IC_POLARITY(TIM5_IC2Polarity));

  tmpsmcr = TIM5->SMCR;
  tmpccmr1 = TIM5->CCMR1;
  tmpccmr2 = TIM5->CCMR2;

  /* Set the encoder Mode */
  tmpsmcr &= (uint8_t)(TIM_SMCR_MSM | TIM_SMCR_TS)  ;
  tmpsmcr |= (uint8_t)TIM5_EncoderMode;

  /* Select the Capture Compare 1 and the Capture Compare 2 as input */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_CCxS);
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_CCxS);
  tmpccmr1 |= TIM_CCMR_TIxDirect_Set;
  tmpccmr2 |= TIM_CCMR_TIxDirect_Set;

  /* Set the TI1 and the TI2 Polarities */
  if (TIM5_IC1Polarity == TIM5_ICPolarity_Falling)
  {
    TIM5->CCER1 |= TIM_CCER1_CC1P ;
  }
  else
  {
    TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P) ;
  }

  if (TIM5_IC2Polarity == TIM5_ICPolarity_Falling)
  {
    TIM5->CCER1 |= TIM_CCER1_CC2P ;
  }
  else
  {
    TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC2P) ;
  }

  TIM5->SMCR = tmpsmcr;
  TIM5->CCMR1 = tmpccmr1;
  TIM5->CCMR2 = tmpccmr2;
}

/**
  * @brief  Enables or Disables the TIM’s Hall sensor interface.
  * @param  NewState: The new state of the TIM5 Hall sensor interface.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM5_SelectHallSensor(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the TI1S Bit */
  if (NewState != DISABLE)
  {
    TIM5->CR2 |= TIM_CR2_TI1S;
  }
  else
  {
    TIM5->CR2 &= (uint8_t)(~TIM_CR2_TI1S);
  }
}

/**
  * @}
  */

/**
  * @brief  Configure the TI1 as Input.
  * @param  TIM5_ICPolarity: Input Capture Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM5_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM5_ICSelection: Specifies the input to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICSelection_DirectTI: Input Capture mapped on the direct input
  *            @arg TIM5_ICSelection_IndirectTI: Input Capture mapped on the indirect input
  *            @arg TIM5_ICSelection_TRGI: Input Capture mapped on the Trigger Input   
  * @param  TIM5_ICFilter: Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI1_Config(TIM5_ICPolarity_TypeDef TIM5_ICPolarity, \
                       TIM5_ICSelection_TypeDef TIM5_ICSelection, \
                       uint8_t TIM5_ICFilter)
{
  uint8_t tmpccmr1 = 0;
  uint8_t tmpicpolarity = TIM5_ICPolarity;
  tmpccmr1 = TIM5->CCMR1;

  /* Check the parameters */
  assert_param(IS_TIM5_IC_POLARITY(TIM5_ICPolarity));
  assert_param(IS_TIM5_IC_SELECTION(TIM5_ICSelection));
  assert_param(IS_TIM5_IC_FILTER(TIM5_ICFilter));

  /* Disable the Channel 1: Reset the CCE Bit */
  TIM5->CCER1 &=  (uint8_t)(~TIM_CCER1_CC1E);

  /* Select the Input and set the filter */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_CCxS) & (uint8_t)(~TIM_CCMR_ICxF);
  tmpccmr1 |= (uint8_t)(((uint8_t)(TIM5_ICSelection)) | ((uint8_t)(TIM5_ICFilter << 4)));

  TIM5->CCMR1 = tmpccmr1;

  /* Select the Polarity */
  if (tmpicpolarity == (uint8_t)(TIM5_ICPolarity_Falling))
  {
    TIM5->CCER1 |= TIM_CCER1_CC1P;
  }
  else
  {
    TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P);
  }

  /* Set the CCE Bit */
  TIM5->CCER1 |=  TIM_CCER1_CC1E;
}

/**
  * @brief  Configure the TI2 as Input.
  * @param  TIM5_ICPolarity: Input Capture Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM5_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM5_ICSelection: Specifies the input to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM5_ICSelection_DirectTI: Input Capture mapped on the direct input
  *            @arg TIM5_ICSelection_IndirectTI: Input Capture mapped on the indirect input
  *            @arg TIM5_ICSelection_TRGI: Input Capture mapped on the Trigger Input   
  * @param  TIM5_ICFilter: Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI2_Config(TIM5_ICPolarity_TypeDef TIM5_ICPolarity,
                       TIM5_ICSelection_TypeDef TIM5_ICSelection,
                       uint8_t TIM5_ICFilter)
{
  uint8_t tmpccmr2 = 0;
  uint8_t tmpicpolarity = TIM5_ICPolarity;

  /* Check the parameters */
  assert_param(IS_TIM5_IC_POLARITY(TIM5_ICPolarity));
  assert_param(IS_TIM5_IC_SELECTION(TIM5_ICSelection));
  assert_param(IS_TIM5_IC_FILTER(TIM5_ICFilter));

  tmpccmr2 = TIM5->CCMR2;

  /* Disable the Channel 2: Reset the CCE Bit */
  TIM5->CCER1 &=  (uint8_t)(~TIM_CCER1_CC2E);

  /* Select the Input and set the filter */
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_CCxS) & (uint8_t)(~TIM_CCMR_ICxF);
  tmpccmr2 |= (uint8_t)(((uint8_t)(TIM5_ICSelection)) | ((uint8_t)(TIM5_ICFilter << 4)));

  TIM5->CCMR2 = tmpccmr2;

  /* Select the Polarity */
  if (tmpicpolarity == TIM5_ICPolarity_Falling)
  {
    TIM5->CCER1 |= TIM_CCER1_CC2P ;
  }
  else
  {
    TIM5->CCER1 &= (uint8_t)(~TIM_CCER1_CC2P) ;
  }

  /* Set the CCE Bit */
  TIM5->CCER1 |=  TIM_CCER1_CC2E;
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
