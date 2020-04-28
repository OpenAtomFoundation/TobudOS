/**
  ******************************************************************************
  * @file    stm8l15x_tim2.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the TIM2 peripheral:
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
  *          This driver provides functions to configure and initialize the TIM2
  *          peripheral
  *          These functions are split in 7 groups: 
  *   
  *          1. TIM2 TimeBase management: this group includes all needed functions 
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
  *          2. TIM2 Output Compare management: this group includes all needed 
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
  *          3. TIM2 Input Capture management: this group includes all needed 
  *             functions to configure the Capture/Compare unit used in 
  *             Input Capture mode:
  *                   - Configure each channel in input capture mode
  *                   - Configure Channel1/2 in PWM Input mode
  *                   - Set the Input Capture Prescaler
  *                   - Get the Capture/Compare values      
  *        
  *          4. TIM2 interrupts, DMA and flags management
  *                   - Enable/Disable interrupt sources
  *                   - Get flags status
  *                   - Clear flags/ Pending bits
  *                   - Enable/Disable DMA requests 
  *                   - Select CaptureCompare DMA request  
  *              
  *          5. TIM2 clocks management: this group includes all needed functions 
  *             to configure the clock controller unit:
  *                   - Select internal/External clock
  *                   - Select the external clock mode: ETR(Mode1/Mode2) or TIx
  *         
  *          6. TIM2 synchronization management: this group includes all needed 
  *             functions to configure the Synchronization unit:
  *                   - Select Input Trigger  
  *                   - Select Output Trigger  
  *                   - Select Master Slave Mode 
  *                   - ETR Configuration when used as external trigger   
  *     
  *          7. TIM2 specific interface management, this group includes all 
  *             needed functions to use the specific TIM2 interface:
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
#include "stm8l15x_TIM2.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup TIM2 
  * @brief TIM2 driver modules
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void TI1_Config(TIM2_ICPolarity_TypeDef TIM2_ICPolarity,
                       TIM2_ICSelection_TypeDef TIM2_ICSelection,
                       uint8_t TIM2_ICFilter);
static void TI2_Config(TIM2_ICPolarity_TypeDef TIM2_ICPolarity,
                       TIM2_ICSelection_TypeDef TIM2_ICSelection,
                       uint8_t TIM2_ICFilter);


/** @defgroup TIM2_Private_Functions
  * @{
  */

/** @defgroup TIM2_Group1 TimeBase management functions
 *  @brief   TimeBase management functions 
 *
@verbatim   
 ===============================================================================
                       TimeBase management functions
 ===============================================================================  
  
       ===================================================================      
              TIM2 Driver: how to use it in Timing(Time base) Mode
       =================================================================== 
       To use the Timer in Timing(Time base) mode, the following steps are mandatory:
       
       1. Enable TIM2 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE) function.
        
       2. Call TIM2_TimeBaseInit() to configure the Time Base unit with the
          corresponding configuration.
          
       3. Enable global interrupts if you need to generate the update interrupt.
          
       4. Enable the corresponding interrupt using the function TIM2_ITConfig(TIM2_IT_Update) 
          
       5. Call the TIM2_Cmd(ENABLE) function to enable the TIM2 counter.
       
       Note1: All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 

@endverbatim
  * @{
  */

/**
  * @brief  Deinitialize the TIM2 peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void TIM2_DeInit(void)
{
  TIM2->CR1 = TIM_CR1_RESET_VALUE;
  TIM2->CR2 = TIM_CR2_RESET_VALUE;
  TIM2->SMCR = TIM_SMCR_RESET_VALUE;
  TIM2->ETR = TIM_ETR_RESET_VALUE;
  TIM2->IER = TIM_IER_RESET_VALUE;
  TIM2->SR2 = TIM_SR2_RESET_VALUE;

  /* Disable channels */
  TIM2->CCER1 = TIM_CCER1_RESET_VALUE;
  /* Configure channels as inputs: it is necessary if lock level is equal to 2 or 3 */
  TIM2->CCMR1 = 0x01;/*TIM2_ICxSource_TIxFPx */
  TIM2->CCMR2 = 0x01;/*TIM2_ICxSource_TIxFPx */

  /* Then reset channel registers: it also works if lock level is equal to 2 or 3 */
  TIM2->CCER1 = TIM_CCER1_RESET_VALUE;
  TIM2->CCMR1 = TIM_CCMR1_RESET_VALUE;
  TIM2->CCMR2 = TIM_CCMR2_RESET_VALUE;

  TIM2->CNTRH = TIM_CNTRH_RESET_VALUE;
  TIM2->CNTRL = TIM_CNTRL_RESET_VALUE;

  TIM2->PSCR = TIM_PSCR_RESET_VALUE;

  TIM2->ARRH = TIM_ARRH_RESET_VALUE;
  TIM2->ARRL = TIM_ARRL_RESET_VALUE;

  TIM2->CCR1H = TIM_CCR1H_RESET_VALUE;
  TIM2->CCR1L = TIM_CCR1L_RESET_VALUE;
  TIM2->CCR2H = TIM_CCR2H_RESET_VALUE;
  TIM2->CCR2L = TIM_CCR2L_RESET_VALUE;


  TIM2->OISR = TIM_OISR_RESET_VALUE;
  TIM2->EGR = 0x01;/* TIM_EGR_UG */
  TIM2->BKR = TIM_BKR_RESET_VALUE;
  TIM2->SR1 = TIM_SR1_RESET_VALUE;
}

/**
  * @brief  Initializes the TIM2 Time Base Unit according to the specified  parameters.
  * @param  TIM2_Prescaler: Prescaler 
  *          This parameter can be one of the following values:
  *            @arg TIM2_Prescaler_1: Time base Prescaler = 1 (No effect)
  *            @arg TIM2_Prescaler_2: Time base Prescaler = 2
  *            @arg TIM2_Prescaler_4: Time base Prescaler = 4
  *            @arg TIM2_Prescaler_8: Time base Prescaler = 8
  *            @arg TIM2_Prescaler_16: Time base Prescaler = 16
  *            @arg TIM2_Prescaler_32: Time base Prescaler = 32
  *            @arg TIM2_Prescaler_64: Time base Prescaler = 64
  *            @arg TIM2_Prescaler_128: Time base Prescaler = 128                            
  * @param  TIM2_CounterMode: Counter mode
  *          This parameter can be one of the following values:
  *            @arg TIM2_CounterMode_Up: Counter Up Mode
  *            @arg TIM2_CounterMode_Down: Counter Down Mode
  *            @arg TIM2_CounterMode_CenterAligned1: Counter Central aligned Mode 1
  *            @arg TIM2_CounterMode_CenterAligned2: Counter Central aligned Mode 2
  *            @arg TIM2_CounterMode_CenterAligned3: Counter Central aligned Mode 3        
  * @param  TIM2_Period: This parameter must be a value between 0x0000 and 0xFFFF.
  * @retval None
  */

void TIM2_TimeBaseInit(TIM2_Prescaler_TypeDef TIM2_Prescaler,
                       TIM2_CounterMode_TypeDef TIM2_CounterMode,
                       uint16_t TIM2_Period)
{

  assert_param(IS_TIM2_PRESCALER(TIM2_Prescaler));
  assert_param(IS_TIM2_COUNTER_MODE(TIM2_CounterMode));



  /* Set the Autoreload value */
  TIM2->ARRH = (uint8_t)(TIM2_Period >> 8) ;
  TIM2->ARRL = (uint8_t)(TIM2_Period);

  /* Set the Prescaler value */
  TIM2->PSCR = (uint8_t)(TIM2_Prescaler);

  /* Select the Counter Mode */
  TIM2->CR1 &= (uint8_t)((uint8_t)(~TIM_CR1_CMS)) & ((uint8_t)(~TIM_CR1_DIR));
  TIM2->CR1 |= (uint8_t)(TIM2_CounterMode);

  /* Generate an update event to reload the Prescaler value immediately */
  TIM2->EGR = TIM2_EventSource_Update;
}

/**
  * @brief  Configures the TIM2 Prescaler.
  * @param  Prescaler: Specifies the Prescaler Register value
  *          This parameter can be one of the following values:
  *            @arg TIM2_Prescaler_1: Time base Prescaler = 1 (No effect)
  *            @arg TIM2_Prescaler_2: Time base Prescaler = 2
  *            @arg TIM2_Prescaler_4: Time base Prescaler = 4
  *            @arg TIM2_Prescaler_8: Time base Prescaler = 8
  *            @arg TIM2_Prescaler_16: Time base Prescaler = 16
  *            @arg TIM2_Prescaler_32: Time base Prescaler = 32
  *            @arg TIM2_Prescaler_64: Time base Prescaler = 64
  *            @arg TIM2_Prescaler_128: Time base Prescaler = 128  
  * @param  TIM2_PSCReloadMode: Specifies the TIM2 Prescaler Reload mode.
  *          This parameter can be one of the following values:
  *            @arg TIM2_PSCReloadMode_Update: Prescaler value is reloaded at every update
  *            @arg TIM2_PSCReloadMode_Immediate: Prescaler value is reloaded at every update  
  * @retval None
  */
void TIM2_PrescalerConfig(TIM2_Prescaler_TypeDef Prescaler,
                          TIM2_PSCReloadMode_TypeDef TIM2_PSCReloadMode)
{
  /* Check the parameters */
  assert_param(IS_TIM2_PRESCALER(Prescaler));
  assert_param(IS_TIM2_PRESCALER_RELOAD(TIM2_PSCReloadMode));

  /* Set the Prescaler value */
  TIM2->PSCR = (uint8_t)(Prescaler);

  /* Set or reset the UG Bit */
  if (TIM2_PSCReloadMode == TIM2_PSCReloadMode_Immediate)
  {
    TIM2->EGR |= TIM_EGR_UG ;
  }
  else
  {
    TIM2->EGR &= (uint8_t)(~TIM_EGR_UG) ;
  }
}

/**
  * @brief  Specifies the TIM2 Counter Mode to be used.
  * @param  TIM2_CounterMode: Specifies the Counter Mode to be used
  *          This parameter can be one of the following values:
  *            @arg TIM2_CounterMode_Up: Counter Up Mode
  *            @arg TIM2_CounterMode_Down: Counter Down Mode
  *            @arg TIM2_CounterMode_CenterAligned1: Counter Central aligned Mode 1
  *            @arg TIM2_CounterMode_CenterAligned2: Counter Central aligned Mode 2
  *            @arg TIM2_CounterMode_CenterAligned3: Counter Central aligned Mode 3      
  * @retval None
  */
void TIM2_CounterModeConfig(TIM2_CounterMode_TypeDef TIM2_CounterMode)
{
  uint8_t tmpcr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_COUNTER_MODE(TIM2_CounterMode));

  tmpcr1 = TIM2->CR1;

  /* Reset the CMS and DIR Bits */
  tmpcr1 &= (uint8_t)((uint8_t)(~TIM_CR1_CMS) & (uint8_t)(~TIM_CR1_DIR));

  /* Set the Counter Mode */
  tmpcr1 |= (uint8_t)TIM2_CounterMode;

  TIM2->CR1 = tmpcr1;
}

/**
  * @brief  Sets the TIM2 Counter Register value.
  * @param  Counter: Specifies the Counter register new value.
  *          This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM2_SetCounter(uint16_t Counter)
{

  /* Set the Counter Register value */
  TIM2->CNTRH = (uint8_t)(Counter >> 8);
  TIM2->CNTRL = (uint8_t)(Counter);
}

/**
  * @brief  Sets the TIM2 Autoreload Register value.
  * @param  Autoreload: Specifies the Autoreload register new value.
  *          This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM2_SetAutoreload(uint16_t Autoreload)
{
  /* Set the Autoreload Register value */
  TIM2->ARRH = (uint8_t)(Autoreload >> 8);
  TIM2->ARRL = (uint8_t)(Autoreload);
}

/**
  * @brief  Gets the TIM2 Counter value.
  * @param  None
  * @retval Counter Register value.
  */
uint16_t TIM2_GetCounter(void)
{
  uint16_t tmpcnt = 0;
  uint8_t tmpcntrl, tmpcntrh;

  tmpcntrh = TIM2->CNTRH;
  tmpcntrl = TIM2->CNTRL;

  tmpcnt = (uint16_t)(tmpcntrl);
  tmpcnt |= (uint16_t)((uint16_t)tmpcntrh << 8);
  /* Get the Counter Register value */
  return ((uint16_t)tmpcnt);
}

/**
  * @brief  Gets the TIM2 Prescaler value.
  * @param  None
  * @retval TIM2 Prescaler, it can be one of the following values:
  *            - TIM2_Prescaler_1: Time base Prescaler = 1 (No effect)
  *            - TIM2_Prescaler_2: Time base Prescaler = 2
  *            - TIM2_Prescaler_4: Time base Prescaler = 4
  *            - TIM2_Prescaler_8: Time base Prescaler = 8
  *            - TIM2_Prescaler_16: Time base Prescaler = 16
  *            - TIM2_Prescaler_32: Time base Prescaler = 32
  *            - TIM2_Prescaler_64: Time base Prescaler = 64
  *            - TIM2_Prescaler_128: Time base Prescaler = 128    
  */
TIM2_Prescaler_TypeDef TIM2_GetPrescaler(void)
{
  /* Get the Prescaler Register value */
  return ((TIM2_Prescaler_TypeDef)TIM2->PSCR);
}

/**
  * @brief  Enables or Disables the TIM2 Update event.
  * @param  NewState: The new state of the TIM2 peripheral Preload register.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */

void TIM2_UpdateDisableConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the UDIS Bit */
  if (NewState != DISABLE)
  {
    TIM2->CR1 |= TIM_CR1_UDIS;
  }
  else
  {
    TIM2->CR1 &= (uint8_t)(~TIM_CR1_UDIS);
  }
}

/**
  * @brief  Selects the TIM2 Update Request Interrupt source.
  * @param  TIM2_UpdateSource: Specifies the Update source.
  *          This parameter can be one of the following values:
  *            @arg TIM2_UpdateSource_Global: Global Update request source
  *            @arg TIM2_UpdateSource_Regular: Regular Update request source 
  * @retval None
  */
void TIM2_UpdateRequestConfig(TIM2_UpdateSource_TypeDef TIM2_UpdateSource)
{
  /* Check the parameters */
  assert_param(IS_TIM2_UPDATE_SOURCE(TIM2_UpdateSource));

  /* Set or Reset the URS Bit */
  if (TIM2_UpdateSource == TIM2_UpdateSource_Regular)
  {
    TIM2->CR1 |= TIM_CR1_URS ;
  }
  else
  {
    TIM2->CR1 &= (uint8_t)(~TIM_CR1_URS);
  }
}

/**
  * @brief  Enables or disables TIM2 peripheral Preload register on ARR.
  * @param  NewState: The new state of the TIM2 peripheral Preload register.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM2_ARRPreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the ARPE Bit */
  if (NewState != DISABLE)
  {
    TIM2->CR1 |= TIM_CR1_ARPE;
  }
  else
  {
    TIM2->CR1 &= (uint8_t)(~TIM_CR1_ARPE);
  }
}

/**
  * @brief  Selects the TIM’s One Pulse Mode.
  * @param  TIM2_OPMode: Specifies the OPM Mode to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM2_OPMode_Single: Single one Pulse mode (OPM Active)
  *            @arg TIM2_OPMode_Repetitive: Single one Pulse mode (OPM Active)  
  * @retval None
  */
void TIM2_SelectOnePulseMode(TIM2_OPMode_TypeDef TIM2_OPMode)
{
  /* Check the parameters */
  assert_param(IS_TIM2_OPM_MODE(TIM2_OPMode));

  /* Set or Reset the OPM Bit */
  if (TIM2_OPMode == TIM2_OPMode_Single)
  {
    TIM2->CR1 |= TIM_CR1_OPM ;
  }
  else
  {
    TIM2->CR1 &= (uint8_t)(~TIM_CR1_OPM);
  }
}

/**
  * @brief  Enables or disables the TIM2 peripheral.
  * @param  NewState: The new state of the TIM2 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM2_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* set or Reset the CEN Bit */
  if (NewState != DISABLE)
  {
    TIM2->CR1 |= TIM_CR1_CEN;
  }
  else
  {
    TIM2->CR1 &= (uint8_t)(~TIM_CR1_CEN);
  }
}

/**
  * @}
  */

/** @defgroup TIM2_Group2 Output Compare management functions
 *  @brief    Output Compare management functions 
 *
@verbatim   
 ===============================================================================
                        Output Compare management functions
 ===============================================================================  
   
       ===================================================================      
              TIM2 Driver: how to use it in Output Compare Mode
       =================================================================== 
       To use the Timer in Output Compare mode, the following steps are mandatory:
       
       1. Enable TIM2 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE) function.
       
       2. Configure the TIM2 pins in output mode by configuring the corresponding GPIO pins
          
       3. Configure the Time base unit as described in the first part of this driver, if needed,
          otherwise the Timer will run with the default configuration:
          - Autoreload value = 0xFFFF
          - Prescaler value = 0x0000
          - Counter mode = Up counting
      
       4. Call TIM2_OCxInit() to configure the channel x with the desired parameters
          including:
          - TIM2 Output Compare mode: TIM2_OCMode
          - TIM2 Output State: TIM2_OutputState
          - TIM2 Pulse value: TIM2_Pulse
          - TIM2 Output Compare Polarity: TIM2_OCPolarity
          - TIM2 Output Idle State: TIM2_OCIdleState
       
       5. Call the TIM2_Cmd(ENABLE) function to enable the TIM2 counter.
       
       Note1: All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 
       
       Note2: If the corresponding interrupt or DMA request are needed, the user should:
              1. Enable global interrupts (or the DMA) to use the TIM2 interrupts (or DMA requests). 
              2. Enable the corresponding interrupt (or DMA request) using the function 
              TIM2_ITConfig(TIM2_IT_CCx) (or TIM2_DMACmd(TIM2_DMASource_CCx))   

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the TIM2 Channel1 according to the specified parameters.
  * @param  TIM2_OCMode: Output Compare Mode 
  *          This parameter can be one of the following values:
  *            @arg TIM2_OCMode_Timing: Timing (Frozen) Mode
  *            @arg TIM2_OCMode_Active: Active Mode
  *            @arg TIM2_OCMode_Inactive: Inactive Mode
  *            @arg TIM2_OCMode_Toggle: Toggle Mode
  *            @arg TIM2_OCMode_PWM1: PWM Mode 1
  *            @arg TIM2_OCMode_PWM2: PWM Mode 2          
  * @param  TIM2_OutputState: Output state
  *          This parameter can be one of the following values:
  *            @arg TIM2_OutputState_Disable: Output compare State disabled (channel output disabled)
  *            @arg TIM2_OutputState_Enable: Output compare State enabled (channel output enabled)
  * @param  TIM2_Pulse: This parameter must be a value between 0x0000 and 0xFFFF.
  * @param  TIM2_OCPolarity: Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM2_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM2_OCPolarity_Low: Output compare polarity  = Low 
  * @param  TIM2_OCIdleState: Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM2_OCIdleState_Reset: Output Compare Idle state  = Reset
  *            @arg TIM2_OCIdleState_Set: Output Compare Idle state  = Set
  * @retval None
  */
void TIM2_OC1Init(TIM2_OCMode_TypeDef TIM2_OCMode,
                  TIM2_OutputState_TypeDef TIM2_OutputState,
                  uint16_t TIM2_Pulse,
                  TIM2_OCPolarity_TypeDef TIM2_OCPolarity,
                  TIM2_OCIdleState_TypeDef TIM2_OCIdleState)
{
  uint8_t tmpccmr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_OC_MODE(TIM2_OCMode));
  assert_param(IS_TIM2_OUTPUT_STATE(TIM2_OutputState));
  assert_param(IS_TIM2_OC_POLARITY(TIM2_OCPolarity));
  assert_param(IS_TIM2_OCIDLE_STATE(TIM2_OCIdleState));

  tmpccmr1 = TIM2->CCMR1;

  /* Disable the Channel 1: Reset the CCE Bit */
  TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E);
  /* Reset the Output Compare Bits */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Set the Output Compare Mode */
  tmpccmr1 |= (uint8_t)TIM2_OCMode;

  TIM2->CCMR1 = tmpccmr1;

  /* Set the Output State */
  if (TIM2_OutputState == TIM2_OutputState_Enable)
  {
    TIM2->CCER1 |= TIM_CCER1_CC1E;
  }
  else
  {
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E);
  }

  /* Set the Output Polarity */
  if (TIM2_OCPolarity == TIM2_OCPolarity_Low)
  {
    TIM2->CCER1 |= TIM_CCER1_CC1P;
  }
  else
  {
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P);
  }

  /* Set the Output Idle state */
  if (TIM2_OCIdleState == TIM2_OCIdleState_Set)
  {
    TIM2->OISR |= TIM_OISR_OIS1;
  }
  else
  {
    TIM2->OISR &= (uint8_t)(~TIM_OISR_OIS1);
  }

  /* Set the Pulse value */
  TIM2->CCR1H = (uint8_t)(TIM2_Pulse >> 8);
  TIM2->CCR1L = (uint8_t)(TIM2_Pulse);
}

/**
  * @brief  Initializes the TIM2 Channel2 according to the specified parameters.
  * @param  TIM2_OCMode: Output Compare Mode 
  *          This parameter can be one of the following values:
  *            @arg TIM2_OCMode_Timing: Timing (Frozen) Mode
  *            @arg TIM2_OCMode_Active: Active Mode
  *            @arg TIM2_OCMode_Inactive: Inactive Mode
  *            @arg TIM2_OCMode_Toggle: Toggle Mode
  *            @arg TIM2_OCMode_PWM1: PWM Mode 1
  *            @arg TIM2_OCMode_PWM2: PWM Mode 2          
  * @param  TIM2_OutputState: Output state
  *          This parameter can be one of the following values:
  *            @arg TIM2_OutputState_Disable: Output compare State disabled (channel output disabled)
  *            @arg TIM2_OutputState_Enable: Output compare State enabled (channel output enabled)
  * @param  TIM2_Pulse: This parameter must be a value between 0x0000 and 0xFFFF.
  * @param  TIM2_OCPolarity: Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM2_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM2_OCPolarity_Low: Output compare polarity  = Low 
  * @param  TIM2_OCIdleState: Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM2_OCIdleState_Reset: Output Compare Idle state  = Reset
  *            @arg TIM2_OCIdleState_Set: Output Compare Idle state  = Set
  * @retval None
  */
void TIM2_OC2Init(TIM2_OCMode_TypeDef TIM2_OCMode,
                  TIM2_OutputState_TypeDef TIM2_OutputState,
                  uint16_t TIM2_Pulse,
                  TIM2_OCPolarity_TypeDef TIM2_OCPolarity,
                  TIM2_OCIdleState_TypeDef TIM2_OCIdleState)
{
  uint8_t tmpccmr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_OC_MODE(TIM2_OCMode));
  assert_param(IS_TIM2_OUTPUT_STATE(TIM2_OutputState));
  assert_param(IS_TIM2_OC_POLARITY(TIM2_OCPolarity));
  assert_param(IS_TIM2_OCIDLE_STATE(TIM2_OCIdleState));

  tmpccmr2 = TIM2->CCMR2;

  /* Disable the Channel 2: Reset the CCE Bit */
  TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC2E);

  /* Reset the Output Compare Bits */
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Set the Output Compare Mode */
  tmpccmr2 |= (uint8_t)TIM2_OCMode;

  TIM2->CCMR2 = tmpccmr2;

  /* Set the Output State */
  if (TIM2_OutputState == TIM2_OutputState_Enable)
  {
    TIM2->CCER1 |= TIM_CCER1_CC2E;
  }
  else
  {
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC2E);
  }

  /* Set the Output Polarity */
  if (TIM2_OCPolarity == TIM2_OCPolarity_Low)
  {
    TIM2->CCER1 |= TIM_CCER1_CC2P;
  }
  else
  {
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC2P);
  }


  /* Set the Output Idle state */
  if (TIM2_OCIdleState == TIM2_OCIdleState_Set)
  {
    TIM2->OISR |= TIM_OISR_OIS2;
  }
  else
  {
    TIM2->OISR &= (uint8_t)(~TIM_OISR_OIS2);
  }

  /* Set the Pulse value */
  TIM2->CCR2H = (uint8_t)(TIM2_Pulse >> 8);
  TIM2->CCR2L = (uint8_t)(TIM2_Pulse);
}

/**
  * @brief  Configures the Break feature, dead time, Lock level, the OSSI,
  *         and the AOE(automatic output enable).
  * @param  TIM2_OSSIState: Off-State Selection for Idle mode states.
  *          This parameter can be one of the following values:
  *            @arg TIM2_OSSIState_Enable: Off-State Selection for Idle mode enabled
  *            @arg TIM2_OSSIState_Disable: Off-State Selection for Idle mode disabled 
  * @param  TIM2_LockLevel: Lock level.
  *          This parameter can be one of the following values:
  *            @arg TIM2_LockLevel_Off: Lock option disabled
  *            @arg TIM2_LockLevel_1: Select Lock Level 1
  *            @arg TIM2_LockLevel_2: Select Lock Level 2
  *            @arg TIM2_LockLevel_3: Select Lock Level 3    
  * @param  TIM2_BreakState: Break Input enable/disable .
  *          This parameter can be one of the following values:
  *            @arg TIM2_BreakState_Disable: Break State disabled (break option disabled)
  *            @arg TIM2_BreakState_Enable: Break State enabled (break option enabled) 
  * @param  TIM2_BreakPolarity: Break Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM2_BreakPolarity_High: if Break, channel polarity = High
  *            @arg TIM2_BreakPolarity_Low: if Break, channel polarity = Low   
  * @param  TIM2_AutomaticOutput: TIM2 AOE Bit Set/Reset .
  *          This parameter can be one of the following values:
  *            @arg TIM2_AutomaticOutput_Enable: Automatic Output option enabled
  *            @arg TIM2_AutomaticOutput_Disable: Automatic Output option disabled
  * @retval None
  */
void TIM2_BKRConfig(TIM2_OSSIState_TypeDef TIM2_OSSIState,
                    TIM2_LockLevel_TypeDef TIM2_LockLevel,
                    TIM2_BreakState_TypeDef TIM2_BreakState,
                    TIM2_BreakPolarity_TypeDef TIM2_BreakPolarity,
                    TIM2_AutomaticOutput_TypeDef TIM2_AutomaticOutput)

{
  /* Check the parameters */
  assert_param(IS_TIM2_OSSI_STATE(TIM2_OSSIState));
  assert_param(IS_TIM2_LOCK_LEVEL(TIM2_LockLevel));
  assert_param(IS_TIM2_BREAK_STATE(TIM2_BreakState));
  assert_param(IS_TIM2_BREAK_POLARITY(TIM2_BreakPolarity));
  assert_param(IS_TIM2_AUTOMATIC_OUTPUT_STATE(TIM2_AutomaticOutput));



  /* Set the Lock level, the Break enable Bit and the Polarity, the OSSI State,
  the dead time value and the Automatic Output Enable Bit */
  TIM2->BKR = (uint8_t)((uint8_t)((uint8_t)((uint8_t)((uint8_t)TIM2_OSSIState | (uint8_t)TIM2_LockLevel) | \
                                  (uint8_t)((uint8_t)TIM2_BreakState | (uint8_t)TIM2_BreakPolarity)) | \
                                  TIM2_AutomaticOutput));
}

/**
  * @brief  Enables or disables the TIM2 peripheral Main Outputs.
  * @param  NewState: The new state of the TIM2 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM2_CtrlPWMOutputs(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the MOE Bit */

  if (NewState != DISABLE)
  {
    TIM2->BKR |= TIM_BKR_MOE ;
  }
  else
  {
    TIM2->BKR &= (uint8_t)(~TIM_BKR_MOE) ;
  }
}

/**
  * @brief  Selects the TIM2 Output Compare Mode. This function disables the
  *         selected channel before changing the Output Compare Mode. User has to
  *         enable this channel using TIM2_CCxCmd and TIM2_CCxNCmd functions.
  * @param  TIM2_Channel: Specifies the TIM2 Channel.
  *          This parameter can be one of the following values:
  *            @arg TIM2_Channel_1: Channel 1
  *            @arg TIM2_Channel_2: Channel 2  
  * @param  TIM2_OCMode: Specifies the TIM2 Output Compare Mode.
  *          This parameter can be one of the following values:
  *            @arg TIM2_OCMode_Timing: Timing (Frozen) Mode
  *            @arg TIM2_OCMode_Active: Active Mode
  *            @arg TIM2_OCMode_Inactive: Inactive Mode
  *            @arg TIM2_OCMode_Toggle: Toggle Mode
  *            @arg TIM2_OCMode_PWM1: PWM Mode 1
  *            @arg TIM2_OCMode_PWM2: PWM Mode 2    
  * @retval None
  */
void TIM2_SelectOCxM(TIM2_Channel_TypeDef TIM2_Channel,
                     TIM2_OCMode_TypeDef TIM2_OCMode)
{
  /* Check the parameters */
  assert_param(IS_TIM2_CHANNEL(TIM2_Channel));
  assert_param(IS_TIM2_OCM(TIM2_OCMode));

  if (TIM2_Channel == TIM2_Channel_1)
  {
    /* Disable the Channel 1: Reset the CCE Bit */
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E);

    /* Reset the Output Compare Bits */
    TIM2->CCMR1 &= (uint8_t)(~TIM_CCMR_OCM);

    /* Set the Output Compare Mode */
    TIM2->CCMR1 |= (uint8_t)TIM2_OCMode;
  }
  else /* if (TIM2_Channel == TIM2_Channel_2) */
  {
    /* Disable the Channel 2: Reset the CCE Bit */
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC2E);

    /* Reset the Output Compare Bits */
    TIM2->CCMR2 &= (uint8_t)(~TIM_CCMR_OCM);

    /* Set the Output Compare Mode */
    TIM2->CCMR2 |= (uint8_t)TIM2_OCMode;
  }
}

/**
  * @brief  Sets the TIM2 Capture Compare1 Register value.
  * @param  Compare: Specifies the Capture Compare1 register new value.
  *         This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM2_SetCompare1(uint16_t Compare)
{
  /* Set the Capture Compare1 Register value */
  TIM2->CCR1H = (uint8_t)(Compare >> 8);
  TIM2->CCR1L = (uint8_t)(Compare);
}

/**
  * @brief  Sets the TIM2 Capture Compare2 Register value.
  * @param  Compare: Specifies the Capture Compare2 register new value.
  *         This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM2_SetCompare2(uint16_t Compare)
{
  /* Set the Capture Compare2 Register value */
  TIM2->CCR2H = (uint8_t)(Compare >> 8);
  TIM2->CCR2L = (uint8_t)(Compare);
}

/**
  * @brief  Forces the TIM2 Channel1 output waveform to active or inactive level.
  * @param  TIM2_ForcedAction: Specifies the forced Action to be set to the output waveform.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ForcedAction_Active: Output Reference is forced low 
  *            @arg TIM2_ForcedAction_Inactive: Output Reference is forced high 
  * @retval None
  */
void TIM2_ForcedOC1Config(TIM2_ForcedAction_TypeDef TIM2_ForcedAction)
{
  uint8_t tmpccmr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_FORCED_ACTION(TIM2_ForcedAction));

  tmpccmr1 = TIM2->CCMR1;

  /* Reset the OCM Bits */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Configure The Forced output Mode */
  tmpccmr1 |= (uint8_t)TIM2_ForcedAction;

  TIM2->CCMR1 = tmpccmr1;
}

/**
  * @brief  Forces the TIM2 Channel2 output waveform to active or inactive level.
  * @param  TIM2_ForcedAction: Specifies the forced Action to be set to the output waveform.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ForcedAction_Active: Output Reference is forced low 
  *            @arg TIM2_ForcedAction_Inactive: Output Reference is forced high 
  * @retval None
  */
void TIM2_ForcedOC2Config(TIM2_ForcedAction_TypeDef TIM2_ForcedAction)
{
  uint8_t tmpccmr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_FORCED_ACTION(TIM2_ForcedAction));

  tmpccmr2 = TIM2->CCMR2;

  /* Reset the OCM Bits */
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Configure The Forced output Mode */
  tmpccmr2 |= (uint8_t)TIM2_ForcedAction;

  TIM2->CCMR2 = tmpccmr2;
}

/**
  * @brief  Enables or disables the TIM2 peripheral Preload Register on CCR1.
  * @param  NewState: The new state of the Capture Compare Preload register.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM2_OC1PreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC1PE Bit */
  if (NewState != DISABLE)
  {
    TIM2->CCMR1 |= TIM_CCMR_OCxPE ;
  }
  else
  {
    TIM2->CCMR1 &= (uint8_t)(~TIM_CCMR_OCxPE) ;
  }
}

/**
  * @brief  Enables or disables the TIM2 peripheral Preload Register on CCR2.
  * @param  NewState: The new state of the Capture Compare Preload register.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM2_OC2PreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC2PE Bit */
  if (NewState != DISABLE)
  {
    TIM2->CCMR2 |= TIM_CCMR_OCxPE ;
  }
  else
  {
    TIM2->CCMR2 &= (uint8_t)(~TIM_CCMR_OCxPE) ;
  }
}

/**
  * @brief  Configures the TIM2 Capture Compare 1 Fast feature.
  * @param  NewState: The new state of the Output Compare Fast Enable bit.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM2_OC1FastConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC1FE Bit */
  if (NewState != DISABLE)
  {
    TIM2->CCMR1 |= TIM_CCMR_OCxFE ;
  }
  else
  {
    TIM2->CCMR1 &= (uint8_t)(~TIM_CCMR_OCxFE) ;
  }
}

/**
  * @brief  Configures the TIM2 Capture Compare 2 Fast feature.
  * @param  NewState: The new state of the Output Compare Fast Enable bit.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */

void TIM2_OC2FastConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC2FE Bit */
  if (NewState != DISABLE)
  {
    TIM2->CCMR2 |= TIM_CCMR_OCxFE ;
  }
  else
  {
    TIM2->CCMR2 &= (uint8_t)(~TIM_CCMR_OCxFE) ;
  }
}

/**
  * @brief  Configures the TIM2 Channel 1 polarity.
  * @param  TIM2_OCPolarity: Specifies the OC1 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM2_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM2_OCPolarity_Low: Output compare polarity  = Low 
  * @retval None
  */
void TIM2_OC1PolarityConfig(TIM2_OCPolarity_TypeDef TIM2_OCPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM2_OC_POLARITY(TIM2_OCPolarity));

  /* Set or Reset the CC1P Bit */
  if (TIM2_OCPolarity == TIM2_OCPolarity_Low)
  {
    TIM2->CCER1 |= TIM_CCER1_CC1P ;
  }
  else
  {
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P) ;
  }
}

/**
  * @brief  Configures the TIM2 Channel 2 polarity.
  * @param  TIM2_OCPolarity: Specifies the OC2 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM2_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM2_OCPolarity_Low: Output compare polarity  = Low 
  * @retval None
  */
void TIM2_OC2PolarityConfig(TIM2_OCPolarity_TypeDef TIM2_OCPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM2_OC_POLARITY(TIM2_OCPolarity));

  /* Set or Reset the CC2P Bit */
  if (TIM2_OCPolarity == TIM2_OCPolarity_Low)
  {
    TIM2->CCER1 |= TIM_CCER1_CC2P ;
  }
  else
  {
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC2P) ;
  }
}

/**
  * @brief  Enables or disables the TIM2 Capture Compare Channel x.
  * @param  TIM2_Channel: Specifies the TIM2 Channel.
  *          This parameter can be one of the following values:
  *            @arg TIM2_Channel_1: Channel 1
  *            @arg TIM2_Channel_2: Channel 2  
  * @param  NewState: Specifies the TIM2 Channel CCxE bit new state.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM2_CCxCmd(TIM2_Channel_TypeDef TIM2_Channel,
                 FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM2_CHANNEL(TIM2_Channel));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (TIM2_Channel == TIM2_Channel_1)
  {
    /* Set or Reset the CC1E Bit */
    if (NewState != DISABLE)
    {
      TIM2->CCER1 |= TIM_CCER1_CC1E ;
    }
    else
    {
      TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E) ;
    }

  }
  else /* if (TIM2_Channel == TIM2_Channel_2) */
  {
    /* Set or Reset the CC2E Bit */
    if (NewState != DISABLE)
    {
      TIM2->CCER1 |= TIM_CCER1_CC2E;
    }
    else
    {
      TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC2E) ;
    }
  }
}

/** @defgroup TIM2_Group3 Input Capture management functions
 *  @brief    Input Capture management functions 
 *
@verbatim   
 ===============================================================================
                      Input Capture management functions
 ===============================================================================  
   
       ===================================================================      
              TIM2 Driver: how to use it in Input Capture Mode
       =================================================================== 
       To use the Timer in Input Capture mode, the following steps are mandatory:
       
       1. Enable TIM2 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE) function.
       
       2. Configure the TIM2 pins in input mode by configuring the corresponding GPIO pins
       
       3. Configure the Time base unit as described in the first part of this driver, if needed,
          otherwise the Timer will run with the default configuration:
          - Autoreload value = 0xFFFF
          - Prescaler value = 0x0
          - Counter mode = Up counting
       
       4. Call TIM2_ICInit() to configure the desired channel to measure only 
          frequency or duty cycle of the input signal using the corresponding configuration: 
          - TIM2 Channel: TIM2_Channel
          - TIM2 Input Capture polarity: TIM2_ICPolarity
          - TIM2 Input Capture selection: TIM2_ICSelection
          - TIM2 Input Capture Prescaler: TIM2_ICPSC
          - TIM2 Input Capture filter value
          or,
          Call TIM2_PWMIConfig() to configure the desired channels with the 
          corresponding configuration and to measure the frequency and the duty
          cycle of the input signal.
          
       5. Enable global interrupts or the DMA to read the measured frequency. 
          
       6. Enable the corresponding interrupt (or DMA request) to read the captured value,
          using the function TIM2_ITConfig(TIM2_IT_CCx) (or TIM2_DMACmd(TIM2_DMASource_CCx))
       
       7. Call the TIM2_Cmd(ENABLE) function to enable the TIM2 counter.
       
       8. Use TIM2_GetCapturex() to read the captured value corresponding to
          channel x.
       
       Note1: All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the TIM2 peripheral according to the specified parameters.
  * @param  TIM2_Channel: TIM2 Channel
  *          This parameter can be one of the following values:
  *            @arg TIM2_Channel_1: Channel 1
  *            @arg TIM2_Channel_2: Channel 2     
  * @param  TIM2_ICPolarity: Input Capture Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM2_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM2_ICSelection: Input Capture Selection
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICSelection_DirectTI: Input Capture mapped on the direct input
  *            @arg TIM2_ICSelection_IndirectTI: Input Capture mapped on the indirect input
  *            @arg TIM2_ICSelection_TRGI: Input Capture mapped on the Trigger Input   
  * @param  TIM2_ICPrescaler: Input Capture Prescaler
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM2_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM2_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM2_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events)   
  * @param  TIM2_ICFilter: This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
void TIM2_ICInit(TIM2_Channel_TypeDef TIM2_Channel,
                 TIM2_ICPolarity_TypeDef TIM2_ICPolarity,
                 TIM2_ICSelection_TypeDef TIM2_ICSelection,
                 TIM2_ICPSC_TypeDef TIM2_ICPrescaler,
                 uint8_t TIM2_ICFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM2_CHANNEL(TIM2_Channel));

  if (TIM2_Channel == TIM2_Channel_1)
  {
    /* TI1 Configuration */
    TI1_Config(TIM2_ICPolarity, TIM2_ICSelection, TIM2_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM2_SetIC1Prescaler(TIM2_ICPrescaler);
  }
  else /* if (TIM2_Channel == TIM2_Channel_2) */
  {
    /* TI2 Configuration */
    TI2_Config(TIM2_ICPolarity, TIM2_ICSelection, TIM2_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM2_SetIC2Prescaler(TIM2_ICPrescaler);
  }
}

/**
  * @brief  Configures the TIM2 peripheral in PWM Input Mode according to the
  *         specified parameters.
  * @param  TIM2_Channel: TIM2 Channel
  *          This parameter can be one of the following values:
  *            @arg TIM2_Channel_1: Channel 1
  *            @arg TIM2_Channel_2: Channel 2     
  * @param  TIM2_ICPolarity: Input Capture Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM2_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM2_ICSelection: Input Capture Selection
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICSelection_DirectTI: Input Capture mapped on the direct input
  *            @arg TIM2_ICSelection_IndirectTI: Input Capture mapped on the indirect input
  *            @arg TIM2_ICSelection_TRGI: Input Capture mapped on the Trigger Input   
  * @param  TIM2_ICPrescaler: Input Capture Prescaler
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM2_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM2_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM2_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events) 
  * @retval None
  */
void TIM2_PWMIConfig(TIM2_Channel_TypeDef TIM2_Channel,
                     TIM2_ICPolarity_TypeDef TIM2_ICPolarity,
                     TIM2_ICSelection_TypeDef TIM2_ICSelection,
                     TIM2_ICPSC_TypeDef TIM2_ICPrescaler,
                     uint8_t TIM2_ICFilter)
{
  uint8_t icpolarity = TIM2_ICPolarity_Rising;
  uint8_t icselection = TIM2_ICSelection_DirectTI;

  /* Check the parameters */
  assert_param(IS_TIM2_CHANNEL(TIM2_Channel));

  /* Select the Opposite Input Polarity */
  if (TIM2_ICPolarity == TIM2_ICPolarity_Rising)
  {
    icpolarity = TIM2_ICPolarity_Falling;
  }
  else
  {
    icpolarity = TIM2_ICPolarity_Rising;
  }

  /* Select the Opposite Input */
  if (TIM2_ICSelection == TIM2_ICSelection_DirectTI)
  {
    icselection = TIM2_ICSelection_IndirectTI;
  }
  else
  {
    icselection = TIM2_ICSelection_DirectTI;
  }

  if (TIM2_Channel == TIM2_Channel_1)
  {
    /* TI1 Configuration */
    TI1_Config(TIM2_ICPolarity, TIM2_ICSelection,
               TIM2_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM2_SetIC1Prescaler(TIM2_ICPrescaler);

    /* TI2 Configuration */
    TI2_Config((TIM2_ICPolarity_TypeDef)icpolarity, (TIM2_ICSelection_TypeDef)icselection, TIM2_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM2_SetIC2Prescaler(TIM2_ICPrescaler);
  }
  else
  {
    /* TI2 Configuration */
    TI2_Config(TIM2_ICPolarity, TIM2_ICSelection,
               TIM2_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM2_SetIC2Prescaler(TIM2_ICPrescaler);

    /* TI1 Configuration */
    TI1_Config((TIM2_ICPolarity_TypeDef)icpolarity, (TIM2_ICSelection_TypeDef)icselection, TIM2_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM2_SetIC1Prescaler(TIM2_ICPrescaler);
  }
}

/**
  * @brief  Gets the TIM2 Input Capture 1 value.
  * @param  None
  * @retval Capture Compare 1 Register value.
  */
uint16_t TIM2_GetCapture1(void)
{
  uint16_t tmpccr1 = 0;
  uint8_t tmpccr1l, tmpccr1h;

  tmpccr1h = TIM2->CCR1H;
  tmpccr1l = TIM2->CCR1L;

  tmpccr1 = (uint16_t)(tmpccr1l);
  tmpccr1 |= (uint16_t)((uint16_t)tmpccr1h << 8);
  /* Get the Capture 1 Register value */
  return ((uint16_t)tmpccr1);
}

/**
  * @brief  Gets the TIM2 Input Capture 2 value.
  * @param  None
  * @retval Capture Compare 2 Register value.
  */
uint16_t TIM2_GetCapture2(void)
{
  uint16_t tmpccr2 = 0;
  uint8_t tmpccr2l, tmpccr2h;

  tmpccr2h = TIM2->CCR2H;
  tmpccr2l = TIM2->CCR2L;

  tmpccr2 = (uint16_t)(tmpccr2l);
  tmpccr2 |= (uint16_t)((uint16_t)tmpccr2h << 8);
  /* Get the Capture 2 Register value */
  return ((uint16_t)tmpccr2);
}

/**
  * @brief  Sets the TIM2 Input Capture 1 prescaler.
  * @param  TIM2_IC1Prescaler: Specifies the Input Capture prescaler new value
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM2_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM2_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM2_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events) 
  * @retval None
  */
void TIM2_SetIC1Prescaler(TIM2_ICPSC_TypeDef TIM2_IC1Prescaler)
{
  uint8_t tmpccmr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_IC_PRESCALER(TIM2_IC1Prescaler));

  tmpccmr1 = TIM2->CCMR1;

  /* Reset the IC1PSC Bits */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_ICxPSC);

  /* Set the IC1PSC value */
  tmpccmr1 |= (uint8_t)TIM2_IC1Prescaler;

  TIM2->CCMR1 = tmpccmr1;
}

/**
  * @brief  Sets the TIM2 Input Capture 2 prescaler.
  * @param  TIM2_IC2Prescaler: Specifies the Input Capture prescaler new value
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM2_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM2_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM2_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events) 
  * @retval None
  */
void TIM2_SetIC2Prescaler(TIM2_ICPSC_TypeDef TIM2_IC2Prescaler)
{
  uint8_t tmpccmr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_IC_PRESCALER(TIM2_IC2Prescaler));

  tmpccmr2 = TIM2->CCMR2;

  /* Reset the IC2PSC Bits */
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_ICxPSC);

  /* Set the IC2PSC value */
  tmpccmr2 |= (uint8_t)TIM2_IC2Prescaler;

  TIM2->CCMR2 = tmpccmr2;
}

/**
  * @}
  */

/** @defgroup TIM2_Group4 Interrupts DMA and flags management functions
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
  * @brief  Enables or disables the specified TIM2 interrupts.
  * @param  TIM2_IT: Specifies the TIM2 interrupts sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg TIM2_IT_Update: Update
  *            @arg TIM2_IT_CC1: Capture Compare Channel1
  *            @arg TIM2_IT_CC2: Capture Compare Channel2 
  *            @arg TIM2_IT_Trigger: Trigger 
  *            @arg TIM2_IT_Break: Break  
  * @param  NewState: The new state of the TIM2 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM2_ITConfig(TIM2_IT_TypeDef TIM2_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM2_IT(TIM2_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the Interrupt sources */
    TIM2->IER |= (uint8_t)TIM2_IT;
  }
  else
  {
    /* Disable the Interrupt sources */
    TIM2->IER &= (uint8_t)(~(uint8_t)TIM2_IT);
  }
}

/**
  * @brief  Configures the TIM2 event to be generated by software.
  * @param  TIM2_EventSource: Specifies the event source.
  *          This parameter can be any combination of the following values:
  *            @arg TIM2_EventSource_Update: Update
  *            @arg TIM2_EventSource_CC1: Capture Compare Channel1
  *            @arg TIM2_EventSource_CC2: Capture Compare Channel2 
  *            @arg TIM2_EventSource_Trigger: Trigger 
  *            @arg TIM2_EventSource_Break: Break  
  * @retval None
  */
void TIM2_GenerateEvent(TIM2_EventSource_TypeDef TIM2_EventSource)
{
  /* Check the parameters */
  assert_param(IS_TIM2_EVENT_SOURCE((uint8_t)TIM2_EventSource));

  /* Set the event sources */
  TIM2->EGR |= (uint8_t)TIM2_EventSource;
}

/**
  * @brief  Checks whether the specified TIM2 flag is set or not.
  * @param  TIM2_FLAG: Specifies the flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg TIM2_FLAG_Update: Update
  *            @arg TIM2_FLAG_CC1: Capture Compare Channel1
  *            @arg TIM2_FLAG_CC2: Capture Compare Channel2 
  *            @arg TIM2_FLAG_Trigger: Trigger 
  *            @arg TIM2_FLAG_Break: Break  
  *            @arg TIM2_FLAG_CC1OF: Capture compare 1 over capture
  *            @arg TIM2_FLAG_CC2OF: Capture compare 2 over capture   
  * @retval FlagStatus: The new state of TIM2_FLAG (SET or RESET)
  */
FlagStatus TIM2_GetFlagStatus(TIM2_FLAG_TypeDef TIM2_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint8_t tim2_flag_l = 0, tim2_flag_h = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_GET_FLAG(TIM2_FLAG));

  tim2_flag_l = (uint8_t)(TIM2->SR1 & (uint8_t)(TIM2_FLAG));
  tim2_flag_h = (uint8_t)(TIM2->SR2 & (uint8_t)((uint16_t)TIM2_FLAG >> 8));

  if ((uint8_t)(tim2_flag_l | tim2_flag_h) != 0)
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
  * @param  TIM2_FLAG: Specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM2_FLAG_Update: Update
  *            @arg TIM2_FLAG_CC1: Capture Compare Channel1
  *            @arg TIM2_FLAG_CC2: Capture Compare Channel2 
  *            @arg TIM2_FLAG_Trigger: Trigger 
  *            @arg TIM2_FLAG_Break: Break  
  * @retval None
  */
void TIM2_ClearFlag(TIM2_FLAG_TypeDef TIM2_FLAG)
{
  /* Check the parameters */
  assert_param(IS_TIM2_CLEAR_FLAG((uint16_t)TIM2_FLAG));
  /* Clear the flags (rc_w0) clear this bit by writing 0. Writing ‘1’ has no effect*/
  TIM2->SR1 = (uint8_t)(~(uint8_t)(TIM2_FLAG));
  TIM2->SR2 = (uint8_t)(~(uint8_t)((uint16_t)TIM2_FLAG >> 8));
}

/**
  * @brief  Checks whether the TIM2 interrupt has occurred or not.
  * @param  TIM2_IT: Specifies the TIM2 interrupt source to check.
  *          This parameter can be any combination of the following values:
  *            @arg TIM2_IT_Update: Update
  *            @arg TIM2_IT_CC1: Capture Compare Channel1
  *            @arg TIM2_IT_CC2: Capture Compare Channel2 
  *            @arg TIM2_IT_Trigger: Trigger 
  *            @arg TIM2_IT_Break: Break  
  * @retval ITStatus: The new state of the TIM2_IT (SET or RESET)
  */
ITStatus TIM2_GetITStatus(TIM2_IT_TypeDef TIM2_IT)
{
  ITStatus bitstatus = RESET;

  uint8_t TIM2_itStatus = 0x0, TIM2_itEnable = 0x0;

  /* Check the parameters */
  assert_param(IS_TIM2_GET_IT(TIM2_IT));

  TIM2_itStatus = (uint8_t)(TIM2->SR1 & (uint8_t)TIM2_IT);

  TIM2_itEnable = (uint8_t)(TIM2->IER & (uint8_t)TIM2_IT);

  if ((TIM2_itStatus != (uint8_t)RESET ) && (TIM2_itEnable != (uint8_t)RESET))
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
  * @param  TIM2_IT: Specifies the pending bit to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM2_IT_Update: Update
  *            @arg TIM2_IT_CC1: Capture Compare Channel1
  *            @arg TIM2_IT_CC2: Capture Compare Channel2 
  *            @arg TIM2_IT_Trigger: Trigger 
  *            @arg TIM2_IT_Break: Break  
  * @retval None
  */
void TIM2_ClearITPendingBit(TIM2_IT_TypeDef TIM2_IT)
{
  /* Check the parameters */
  assert_param(IS_TIM2_IT(TIM2_IT));

  /* Clear the IT pending Bit */
  TIM2->SR1 = (uint8_t)(~(uint8_t)TIM2_IT);
}

/**
  * @brief  Enables or disables the TIM2 DMA Requests.
  * @param  TIM2_DMASource: specifies the DMA Request sources.
  *          This parameter can be any combination of the following values:
  *            @arg TIM2_DMASource_Update: TIM2 DMA Update Request
  *            @arg TIM2_DMASource_CC1: TIM2 DMA CC1 Request
  *            @arg TIM2_DMASource_CC2: TIM2 DMA CC2 Request 
  * @param  NewState: new state of the DMA Request sources.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM2_DMACmd( TIM2_DMASource_TypeDef TIM2_DMASource, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  assert_param(IS_TIM2_DMA_SOURCE(TIM2_DMASource));

  if (NewState != DISABLE)
  {
    /* Enable the DMA sources */
    TIM2->DER |= TIM2_DMASource;
  }
  else
  {
    /* Disable the DMA sources */
    TIM2->DER &= (uint8_t)(~TIM2_DMASource);
  }
}

/**
  * @brief  Selects the TIM2 peripheral Capture Compare DMA source.
  * @param   NewState: new state of the Capture Compare DMA source.
  *           This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM2_SelectCCDMA(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Set the CCDS Bit */
    TIM2->CR2 |= TIM_CR2_CCDS;
  }
  else
  {
    /* Reset the CCDS Bit */
    TIM2->CR2 &= (uint8_t)(~TIM_CR2_CCDS);
  }
}

/**
  * @}
  */

/** @defgroup TIM2_Group5 Clocks management functions
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
  * @brief  Enables the TIM2 internal Clock.
  * @par Parameters:
  * None
  * @retval None
  */
void TIM2_InternalClockConfig(void)
{
  /* Disable slave mode to clock the prescaler directly with the internal clock */
  TIM2->SMCR &=  (uint8_t)(~TIM_SMCR_SMS);
}

/**
  * @brief  Configures the TIM2 Trigger as External Clock.
  * @param  TIM2_TIxExternalCLKSource: Specifies Trigger source.
  *          This parameter can be one of the following values:
  *            @arg TIM2_TIxExternalCLK1Source_TI1ED: External Clock mode 1 source = TI1ED
  *            @arg TIM2_TIxExternalCLK1Source_TI1: External Clock mode 1 source = TI1 
  *            @arg TIM2_TIxExternalCLK1Source_TI2: External Clock mode 1 source = TI2  
  * @param  TIM2_ICPolarity: Specifies the TIx Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM2_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  ICFilter: Specifies the filter value.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM2_TIxExternalClockConfig(TIM2_TIxExternalCLK1Source_TypeDef TIM2_TIxExternalCLKSource,
                                 TIM2_ICPolarity_TypeDef TIM2_ICPolarity,
                                 uint8_t ICFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM2_TIXCLK_SOURCE(TIM2_TIxExternalCLKSource));
  assert_param(IS_TIM2_IC_POLARITY(TIM2_ICPolarity));
  assert_param(IS_TIM2_IC_FILTER(ICFilter));

  /* Configure the TIM2 Input Clock Source */
  if (TIM2_TIxExternalCLKSource == TIM2_TIxExternalCLK1Source_TI2)
  {
    TI2_Config(TIM2_ICPolarity, TIM2_ICSelection_DirectTI, ICFilter);
  }
  else
  {
    TI1_Config(TIM2_ICPolarity, TIM2_ICSelection_DirectTI, ICFilter);
  }

  /* Select the Trigger source */
  TIM2_SelectInputTrigger((TIM2_TRGSelection_TypeDef)TIM2_TIxExternalCLKSource);

  /* Select the External clock mode1 */
  TIM2->SMCR |= (uint8_t)(TIM2_SlaveMode_External1);
}

/**
  * @brief  Configures the TIM2 External clock Mode1.
  * @param  TIM2_ExtTRGPrescaler: Specifies the external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ExtTRGPSC_OFF: No External Trigger prescaler
  *            @arg TIM2_ExtTRGPSC_DIV2: External Trigger prescaler = 2 (ETRP frequency divided by 2)
  *            @arg TIM2_ExtTRGPSC_DIV4: External Trigger prescaler = 4 (ETRP frequency divided by 4) 
  *            @arg TIM2_ExtTRGPSC_DIV8: External Trigger prescaler = 8 (ETRP frequency divided by 8)   
  * @param  TIM2_ExtTRGPolarity: Specifies the external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ExtTRGPolarity_Inverted: External Trigger Polarity = inverted
  *            @arg TIM2_ExtTRGPolarity_NonInverted: External Trigger Polarity = non inverted
  * @param  ExtTRGFilter: Specifies the External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM2_ETRClockMode1Config(TIM2_ExtTRGPSC_TypeDef TIM2_ExtTRGPrescaler,
                              TIM2_ExtTRGPolarity_TypeDef TIM2_ExtTRGPolarity,
                              uint8_t ExtTRGFilter)
{
  /* Configure the ETR Clock source */
  TIM2_ETRConfig(TIM2_ExtTRGPrescaler, TIM2_ExtTRGPolarity, ExtTRGFilter);

  /* Select the External clock mode1 */
  TIM2->SMCR &= (uint8_t)(~TIM_SMCR_SMS);
  TIM2->SMCR |= (uint8_t)(TIM2_SlaveMode_External1);

  /* Select the Trigger selection: ETRF */
  TIM2->SMCR &= (uint8_t)(~TIM_SMCR_TS);
  TIM2->SMCR |= (uint8_t)((TIM2_TRGSelection_TypeDef)TIM2_TRGSelection_ETRF);
}

/**
  * @brief  Configures the TIM2 External clock Mode2.
  * @param  TIM2_ExtTRGPrescaler: Specifies the external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ExtTRGPSC_OFF: No External Trigger prescaler
  *            @arg TIM2_ExtTRGPSC_DIV2: External Trigger prescaler = 2 (ETRP frequency divided by 2)
  *            @arg TIM2_ExtTRGPSC_DIV4: External Trigger prescaler = 4 (ETRP frequency divided by 4) 
  *            @arg TIM2_ExtTRGPSC_DIV8: External Trigger prescaler = 8 (ETRP frequency divided by 8) 
  * @param  TIM2_ExtTRGPolarity: Specifies the external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ExtTRGPolarity_Inverted: External Trigger Polarity = inverted
  *            @arg TIM2_ExtTRGPolarity_NonInverted: External Trigger Polarity = non inverted
  * @param  ExtTRGFilter: Specifies the External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM2_ETRClockMode2Config(TIM2_ExtTRGPSC_TypeDef TIM2_ExtTRGPrescaler,
                              TIM2_ExtTRGPolarity_TypeDef TIM2_ExtTRGPolarity,
                              uint8_t ExtTRGFilter)
{
  /* Configure the ETR Clock source */
  TIM2_ETRConfig(TIM2_ExtTRGPrescaler, TIM2_ExtTRGPolarity, ExtTRGFilter);

  /* Enable the External clock mode2 */
  TIM2->ETR |= TIM_ETR_ECE ;
}

/**
  * @}
  */

/** @defgroup TIM2_Group6 Synchronization management functions
 *  @brief    Synchronization management functions 
 *
@verbatim   
 ===============================================================================
                       Synchronization management functions
 ===============================================================================  
                   
       ===================================================================      
              TIM2 Driver: how to use it in synchronization Mode
       =================================================================== 
       Case of two/several Timers
       **************************
       1. If TIM2 is used as master to other timers use the following functions:
          - TIM2_SelectOutputTrigger()
          - TIM2_SelectMasterSlaveMode()
       2. If TIM2 is used as slave to other timers use the following functions:
          - TIM2_SelectInputTrigger()
          - TIM2_SelectSlaveMode()
          
       Case of Timers and external trigger (TRIG pin)
       ********************************************       
       1. Configure the External trigger using TIM2_ETRConfig()
       2. Configure the Slave Timer using the following functions:
          - TIM2_SelectInputTrigger()
          - TIM2_SelectSlaveMode()

@endverbatim
  * @{
  */

/**
  * @brief  Selects the TIM2 Input Trigger source.
  * @param  TIM2_InputTriggerSource: Specifies Input Trigger source.
  *          This parameter can be one of the following values:
  *            @arg TIM2_TRGSelection_TIM4: TRIG Input source =  TIM TRIG Output
  *            @arg TIM2_TRGSelection_TIM1: TRIG Input source =  TIM TRIG Output
  *            @arg TIM2_TRGSelection_TIM2: TRIG Input source =  TIM TRIG Output
  *            @arg TIM2_TRGSelection_TIM2: TRIG Input source =  TIM TRIG Output
  *            @arg TIM2_TRGSelection_TI1F_ED: TRIG Input source = TI1F_ED (TI1 Edge Detector)
  *            @arg TIM2_TRGSelection_TI1FP1: TRIG Input source = TI1FP1 (Filtered Timer Input 1)
  *            @arg TIM2_TRGSelection_TI2FP2: TRIG Input source = TI2FP2 (Filtered Timer Input 2)
  *            @arg TIM2_TRGSelection_ETRF: TRIG Input source =  ETRF (External Trigger Input )      
  * @retval None
  */
void TIM2_SelectInputTrigger(TIM2_TRGSelection_TypeDef TIM2_InputTriggerSource)
{
  uint8_t tmpsmcr = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_TRIGGER_SELECTION(TIM2_InputTriggerSource));

  tmpsmcr = TIM2->SMCR;

  /* Select the Trigger Source */
  tmpsmcr &= (uint8_t)(~TIM_SMCR_TS);
  tmpsmcr |= (uint8_t)TIM2_InputTriggerSource;

  TIM2->SMCR = (uint8_t)tmpsmcr;
}

/**
  * @brief  Selects the TIM2 Trigger Output Mode.
  * @param  TIM2_TRGOSource: Specifies the Trigger Output source.
  *          This parameter can be one of the following values:
  *            @arg TIM2_TRGOSource_Reset: Trigger Output source = Reset 
  *            @arg TIM2_TRGOSource_Enable: Trigger Output source = TIM2 is enabled 
  *            @arg TIM2_TRGOSource_Update: Trigger Output source = Update event
  *            @arg TIM2_TRGOSource_OC1: Trigger Output source = output compare channel1
  *            @arg TIM2_TRGOSource_OC1REF: Trigger Output source = output compare channel 1 reference
  *            @arg TIM2_TRGOSource_OC2REF: Trigger Output source = output compare channel 2 reference   
  * @retval None
  */
void TIM2_SelectOutputTrigger(TIM2_TRGOSource_TypeDef TIM2_TRGOSource)
{
  uint8_t tmpcr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_TRGO_SOURCE(TIM2_TRGOSource));

  tmpcr2 = TIM2->CR2;

  /* Reset the MMS Bits */
  tmpcr2 &= (uint8_t)(~TIM_CR2_MMS);

  /* Select the TRGO source */
  tmpcr2 |=  (uint8_t)TIM2_TRGOSource;

  TIM2->CR2 = tmpcr2;
}

/**
  * @brief  Selects the TIM2 Slave Mode.
  * @param  TIM2_SlaveMode: Specifies the TIM2 Slave Mode.
  *          This parameter can be one of the following values:
  *            @arg TIM2_SlaveMode_Reset: Slave Mode Selection  = Reset
  *            @arg TIM2_SlaveMode_Gated: Slave Mode Selection  = Gated
  *            @arg TIM2_SlaveMode_Trigger: Slave Mode Selection  = Trigger
  *            @arg TIM2_SlaveMode_External1: Slave Mode Selection  = External 1  
  * @retval None
  */
void TIM2_SelectSlaveMode(TIM2_SlaveMode_TypeDef TIM2_SlaveMode)
{
  uint8_t tmpsmcr = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_SLAVE_MODE(TIM2_SlaveMode));

  tmpsmcr = TIM2->SMCR;

  /* Reset the SMS Bits */
  tmpsmcr &= (uint8_t)(~TIM_SMCR_SMS);

  /* Select the Slave Mode */
  tmpsmcr |= (uint8_t)TIM2_SlaveMode;

  TIM2->SMCR = tmpsmcr;
}

/**
  * @brief  Sets or Resets the TIM2 Master/Slave Mode.
  * @param  NewState: The new state of the synchronization between TIM2 and its slaves (through TRGO).
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM2_SelectMasterSlaveMode(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the MSM Bit */
  if (NewState != DISABLE)
  {
    TIM2->SMCR |= TIM_SMCR_MSM;
  }
  else
  {
    TIM2->SMCR &= (uint8_t)(~TIM_SMCR_MSM);
  }
}

/**
  * @brief  Configures the TIM2 External Trigger.
  * @param  TIM2_ExtTRGPrescaler: Specifies the external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ExtTRGPSC_OFF: No External Trigger prescaler
  *            @arg TIM2_ExtTRGPSC_DIV2: External Trigger prescaler = 2 (ETRP frequency divided by 2)
  *            @arg TIM2_ExtTRGPSC_DIV4: External Trigger prescaler = 4 (ETRP frequency divided by 4)
  *            @arg TIM2_ExtTRGPSC_DIV8: External Trigger prescaler = 8 (ETRP frequency divided by 8)    
  * @param  TIM2_ExtTRGPolarity: Specifies the external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ExtTRGPolarity_Inverted: External Trigger Polarity = inverted
  *            @arg TIM2_ExtTRGPolarity_NonInverted: External Trigger Polarity = non inverted
  * @param  ExtTRGFilter: Specifies the External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM2_ETRConfig(TIM2_ExtTRGPSC_TypeDef TIM2_ExtTRGPrescaler,
                    TIM2_ExtTRGPolarity_TypeDef TIM2_ExtTRGPolarity,
                    uint8_t ExtTRGFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM2_EXT_PRESCALER(TIM2_ExtTRGPrescaler));
  assert_param(IS_TIM2_EXT_POLARITY(TIM2_ExtTRGPolarity));
  assert_param(IS_TIM2_EXT_FILTER(ExtTRGFilter));

  /* Set the Prescaler, the Filter value and the Polarity */
  TIM2->ETR |= (uint8_t)((uint8_t)((uint8_t)TIM2_ExtTRGPrescaler | (uint8_t)TIM2_ExtTRGPolarity)
                         | (uint8_t)ExtTRGFilter);
}

/**
  * @}
  */

/** @defgroup TIM2_Group7 Specific interface management functions
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
  * @brief  Configures the TIM2 Encoder Interface.
  * @param  TIM2_EncoderMode: Specifies the TIM2 Encoder Mode.
  *          This parameter can be one of the following values:
  *            @arg TIM2_EncoderMode_TI1: Encoder mode 1
  *            @arg TIM2_EncoderMode_TI2: Encoder mode 2
  *            @arg TIM2_EncoderMode_TI12: Encoder mode 3   
  * @param  TIM2_IC1Polarity: Specifies the IC1 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM2_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM2_IC2Polarity: Specifies the IC2 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM2_ICPolarity_Falling: Input Capture on Falling Edge  
  * @retval None
  */
void TIM2_EncoderInterfaceConfig(TIM2_EncoderMode_TypeDef TIM2_EncoderMode,
                                 TIM2_ICPolarity_TypeDef TIM2_IC1Polarity,
                                 TIM2_ICPolarity_TypeDef TIM2_IC2Polarity)
{
  uint8_t tmpsmcr = 0;
  uint8_t tmpccmr1 = 0;
  uint8_t tmpccmr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM2_ENCODER_MODE(TIM2_EncoderMode));
  assert_param(IS_TIM2_IC_POLARITY(TIM2_IC1Polarity));
  assert_param(IS_TIM2_IC_POLARITY(TIM2_IC2Polarity));

  tmpsmcr = TIM2->SMCR;
  tmpccmr1 = TIM2->CCMR1;
  tmpccmr2 = TIM2->CCMR2;

  /* Set the encoder Mode */
  tmpsmcr &= (uint8_t)(TIM_SMCR_MSM | TIM_SMCR_TS)  ;
  tmpsmcr |= (uint8_t)TIM2_EncoderMode;

  /* Select the Capture Compare 1 and the Capture Compare 2 as input */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_CCxS);
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_CCxS);
  tmpccmr1 |= TIM_CCMR_TIxDirect_Set;
  tmpccmr2 |= TIM_CCMR_TIxDirect_Set;

  /* Set the TI1 and the TI2 Polarities */
  if (TIM2_IC1Polarity == TIM2_ICPolarity_Falling)
  {
    TIM2->CCER1 |= TIM_CCER1_CC1P ;
  }
  else
  {
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P) ;
  }

  if (TIM2_IC2Polarity == TIM2_ICPolarity_Falling)
  {
    TIM2->CCER1 |= TIM_CCER1_CC2P ;
  }
  else
  {
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC2P) ;
  }

  TIM2->SMCR = tmpsmcr;
  TIM2->CCMR1 = tmpccmr1;
  TIM2->CCMR2 = tmpccmr2;
}

/**
  * @brief  Enables or Disables the TIM’s Hall sensor interface.
  * @param  NewState: The new state of the TIM2 Hall sensor interface.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM2_SelectHallSensor(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the TI1S Bit */
  if (NewState != DISABLE)
  {
    TIM2->CR2 |= TIM_CR2_TI1S;
  }
  else
  {
    TIM2->CR2 &= (uint8_t)(~TIM_CR2_TI1S);
  }
}

/**
  * @}
  */

/**
  * @brief  Configure the TI1 as Input.
  * @param  TIM2_ICPolarity: Input Capture Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM2_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM2_ICSelection: Specifies the input to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICSelection_DirectTI: Input Capture mapped on the direct input
  *            @arg TIM2_ICSelection_IndirectTI: Input Capture mapped on the indirect input
  *            @arg TIM2_ICSelection_TRGI: Input Capture mapped on the Trigger Input   
  * @param  TIM2_ICFilter: Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI1_Config(TIM2_ICPolarity_TypeDef TIM2_ICPolarity, \
                       TIM2_ICSelection_TypeDef TIM2_ICSelection, \
                       uint8_t TIM2_ICFilter)
{
  uint8_t tmpccmr1 = 0;
  uint8_t tmpicpolarity = TIM2_ICPolarity;
  tmpccmr1 = TIM2->CCMR1;

  /* Check the parameters */
  assert_param(IS_TIM2_IC_POLARITY(TIM2_ICPolarity));
  assert_param(IS_TIM2_IC_SELECTION(TIM2_ICSelection));
  assert_param(IS_TIM2_IC_FILTER(TIM2_ICFilter));

  /* Disable the Channel 1: Reset the CCE Bit */
  TIM2->CCER1 &=  (uint8_t)(~TIM_CCER1_CC1E);

  /* Select the Input and set the filter */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_CCxS) & (uint8_t)(~TIM_CCMR_ICxF);
  tmpccmr1 |= (uint8_t)(((uint8_t)(TIM2_ICSelection)) | ((uint8_t)(TIM2_ICFilter << 4)));

  TIM2->CCMR1 = tmpccmr1;

  /* Select the Polarity */
  if (tmpicpolarity == (uint8_t)(TIM2_ICPolarity_Falling))
  {
    TIM2->CCER1 |= TIM_CCER1_CC1P;
  }
  else
  {
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P);
  }

  /* Set the CCE Bit */
  TIM2->CCER1 |=  TIM_CCER1_CC1E;
}

/**
  * @brief  Configure the TI2 as Input.
  * @param  TIM2_ICPolarity: Input Capture Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM2_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM2_ICSelection: Specifies the input to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM2_ICSelection_DirectTI: Input Capture mapped on the direct input
  *            @arg TIM2_ICSelection_IndirectTI: Input Capture mapped on the indirect input
  *            @arg TIM2_ICSelection_TRGI: Input Capture mapped on the Trigger Input   
  * @param  TIM2_ICFilter: Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI2_Config(TIM2_ICPolarity_TypeDef TIM2_ICPolarity,
                       TIM2_ICSelection_TypeDef TIM2_ICSelection,
                       uint8_t TIM2_ICFilter)
{
  uint8_t tmpccmr2 = 0;
  uint8_t tmpicpolarity = TIM2_ICPolarity;

  /* Check the parameters */
  assert_param(IS_TIM2_IC_POLARITY(TIM2_ICPolarity));
  assert_param(IS_TIM2_IC_SELECTION(TIM2_ICSelection));
  assert_param(IS_TIM2_IC_FILTER(TIM2_ICFilter));

  tmpccmr2 = TIM2->CCMR2;

  /* Disable the Channel 2: Reset the CCE Bit */
  TIM2->CCER1 &=  (uint8_t)(~TIM_CCER1_CC2E);

  /* Select the Input and set the filter */
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_CCxS) & (uint8_t)(~TIM_CCMR_ICxF);
  tmpccmr2 |= (uint8_t)(((uint8_t)(TIM2_ICSelection)) | ((uint8_t)(TIM2_ICFilter << 4)));

  TIM2->CCMR2 = tmpccmr2;

  /* Select the Polarity */
  if (tmpicpolarity == TIM2_ICPolarity_Falling)
  {
    TIM2->CCER1 |= TIM_CCER1_CC2P ;
  }
  else
  {
    TIM2->CCER1 &= (uint8_t)(~TIM_CCER1_CC2P) ;
  }

  /* Set the CCE Bit */
  TIM2->CCER1 |=  TIM_CCER1_CC2E;
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
