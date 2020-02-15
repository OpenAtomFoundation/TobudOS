/**
  ******************************************************************************
  * @file    stm8l15x_tim3.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the TIM3 peripheral:
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
  *          This driver provides functions to configure and initialize the TIM3
  *          peripheral
  *          These functions are split in 7 groups: 
  *   
  *          1. TIM3 TimeBase management: this group includes all needed functions 
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
  *          2. TIM3 Output Compare management: this group includes all needed 
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
  *          3. TIM3 Input Capture management: this group includes all needed 
  *             functions to configure the Capture/Compare unit used in 
  *             Input Capture mode:
  *                   - Configure each channel in input capture mode
  *                   - Configure Channel1/2 in PWM Input mode
  *                   - Set the Input Capture Prescaler
  *                   - Get the Capture/Compare values      
  *        
  *          4. TIM3 interrupts, DMA and flags management
  *                   - Enable/Disable interrupt sources
  *                   - Get flags status
  *                   - Clear flags/ Pending bits
  *                   - Enable/Disable DMA requests 
  *                   - Select CaptureCompare DMA request  
  *              
  *          5. TIM3 clocks management: this group includes all needed functions 
  *             to configure the clock controller unit:
  *                   - Select internal/External clock
  *                   - Select the external clock mode: ETR(Mode1/Mode2) or TIx
  *         
  *          6. TIM3 synchronization management: this group includes all needed 
  *             functions to configure the Synchronization unit:
  *                   - Select Input Trigger  
  *                   - Select Output Trigger  
  *                   - Select Master Slave Mode 
  *                   - ETR Configuration when used as external trigger   
  *     
  *          7. TIM3 specific interface management, this group includes all 
  *             needed functions to use the specific TIM3 interface:
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
#include "stm8l15x_TIM3.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup TIM3 
  * @brief TIM3 driver modules
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void TI1_Config(TIM3_ICPolarity_TypeDef TIM3_ICPolarity,
                       TIM3_ICSelection_TypeDef TIM3_ICSelection,
                       uint8_t TIM3_ICFilter);
static void TI2_Config(TIM3_ICPolarity_TypeDef TIM3_ICPolarity,
                       TIM3_ICSelection_TypeDef TIM3_ICSelection,
                       uint8_t TIM3_ICFilter);


/** @defgroup TIM3_Private_Functions
  * @{
  */

/** @defgroup TIM3_Group1 TimeBase management functions
 *  @brief   TimeBase management functions 
 *
@verbatim   
 ===============================================================================
                       TimeBase management functions
 ===============================================================================  
  
       ===================================================================      
              TIM3 Driver: how to use it in Timing(Time base) Mode
       =================================================================== 
       To use the Timer in Timing(Time base) mode, the following steps are mandatory:
       
       1. Enable TIM3 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE) function.
        
       2. Call TIM3_TimeBaseInit() to configure the Time Base unit with the
          corresponding configuration.
          
       3. Enable global interrupts if you need to generate the update interrupt.
          
       4. Enable the corresponding interrupt using the function TIM3_ITConfig(TIM3_IT_Update) 
          
       5. Call the TIM3_Cmd(ENABLE) function to enable the TIM3 counter.
       
       Note1: All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 

@endverbatim
  * @{
  */

/**
  * @brief  Deinitialize the TIM3 peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void TIM3_DeInit(void)
{
  TIM3->CR1 = TIM_CR1_RESET_VALUE;
  TIM3->CR2 = TIM_CR2_RESET_VALUE;
  TIM3->SMCR = TIM_SMCR_RESET_VALUE;
  TIM3->ETR = TIM_ETR_RESET_VALUE;
  TIM3->IER = TIM_IER_RESET_VALUE;
  TIM3->SR2 = TIM_SR2_RESET_VALUE;

  /* Disable channels */
  TIM3->CCER1 = TIM_CCER1_RESET_VALUE;
  /* Configure channels as inputs: it is necessary if lock level is equal to 2 or 3 */
  TIM3->CCMR1 = 0x01;/*TIM3_ICxSource_TIxFPx */
  TIM3->CCMR2 = 0x01;/*TIM3_ICxSource_TIxFPx */

  /* Then reset channel registers: it also works if lock level is equal to 2 or 3 */
  TIM3->CCER1 = TIM_CCER1_RESET_VALUE;
  TIM3->CCMR1 = TIM_CCMR1_RESET_VALUE;
  TIM3->CCMR2 = TIM_CCMR2_RESET_VALUE;

  TIM3->CNTRH = TIM_CNTRH_RESET_VALUE;
  TIM3->CNTRL = TIM_CNTRL_RESET_VALUE;

  TIM3->PSCR = TIM_PSCR_RESET_VALUE;

  TIM3->ARRH = TIM_ARRH_RESET_VALUE;
  TIM3->ARRL = TIM_ARRL_RESET_VALUE;

  TIM3->CCR1H = TIM_CCR1H_RESET_VALUE;
  TIM3->CCR1L = TIM_CCR1L_RESET_VALUE;
  TIM3->CCR2H = TIM_CCR2H_RESET_VALUE;
  TIM3->CCR2L = TIM_CCR2L_RESET_VALUE;


  TIM3->OISR = TIM_OISR_RESET_VALUE;
  TIM3->EGR = 0x01;/* TIM_EGR_UG */
  TIM3->BKR = TIM_BKR_RESET_VALUE;
  TIM3->SR1 = TIM_SR1_RESET_VALUE;
}

/**
  * @brief  Initializes the TIM3 Time Base Unit according to the specified  parameters.
  * @param  TIM3_Prescaler: Prescaler 
  *          This parameter can be one of the following values:
  *            @arg TIM3_Prescaler_1: Time base Prescaler = 1 (No effect)
  *            @arg TIM3_Prescaler_2: Time base Prescaler = 2
  *            @arg TIM3_Prescaler_4: Time base Prescaler = 4
  *            @arg TIM3_Prescaler_8: Time base Prescaler = 8
  *            @arg TIM3_Prescaler_16: Time base Prescaler = 16
  *            @arg TIM3_Prescaler_32: Time base Prescaler = 32
  *            @arg TIM3_Prescaler_64: Time base Prescaler = 64
  *            @arg TIM3_Prescaler_128: Time base Prescaler = 128                            
  * @param  TIM3_CounterMode: Counter mode
  *          This parameter can be one of the following values:
  *            @arg TIM3_CounterMode_Up: Counter Up Mode
  *            @arg TIM3_CounterMode_Down: Counter Down Mode
  *            @arg TIM3_CounterMode_CenterAligned1: Counter Central aligned Mode 1
  *            @arg TIM3_CounterMode_CenterAligned2: Counter Central aligned Mode 2
  *            @arg TIM3_CounterMode_CenterAligned3: Counter Central aligned Mode 3        
  * @param  TIM3_Period: This parameter must be a value between 0x0000 and 0xFFFF.
  * @retval None
  */

void TIM3_TimeBaseInit(TIM3_Prescaler_TypeDef TIM3_Prescaler,
                       TIM3_CounterMode_TypeDef TIM3_CounterMode,
                       uint16_t TIM3_Period)
{

  assert_param(IS_TIM3_PRESCALER(TIM3_Prescaler));
  assert_param(IS_TIM3_COUNTER_MODE(TIM3_CounterMode));



  /* Set the Autoreload value */
  TIM3->ARRH = (uint8_t)(TIM3_Period >> 8) ;
  TIM3->ARRL = (uint8_t)(TIM3_Period);

  /* Set the Prescaler value */
  TIM3->PSCR = (uint8_t)(TIM3_Prescaler);

  /* Select the Counter Mode */
  TIM3->CR1 &= (uint8_t)((uint8_t)(~TIM_CR1_CMS)) & ((uint8_t)(~TIM_CR1_DIR));
  TIM3->CR1 |= (uint8_t)(TIM3_CounterMode);

  /* Generate an update event to reload the Prescaler value immediately */
  TIM3->EGR = TIM3_EventSource_Update;
}

/**
  * @brief  Configures the TIM3 Prescaler.
  * @param  Prescaler: Specifies the Prescaler Register value
  *          This parameter can be one of the following values:
  *            @arg TIM3_Prescaler_1: Time base Prescaler = 1 (No effect)
  *            @arg TIM3_Prescaler_2: Time base Prescaler = 2
  *            @arg TIM3_Prescaler_4: Time base Prescaler = 4
  *            @arg TIM3_Prescaler_8: Time base Prescaler = 8
  *            @arg TIM3_Prescaler_16: Time base Prescaler = 16
  *            @arg TIM3_Prescaler_32: Time base Prescaler = 32
  *            @arg TIM3_Prescaler_64: Time base Prescaler = 64
  *            @arg TIM3_Prescaler_128: Time base Prescaler = 128  
  * @param  TIM3_PSCReloadMode: Specifies the TIM3 Prescaler Reload mode.
  *          This parameter can be one of the following values:
  *            @arg TIM3_PSCReloadMode_Update: Prescaler value is reloaded at every update
  *            @arg TIM3_PSCReloadMode_Immediate: Prescaler value is reloaded at every update  
  * @retval None
  */
void TIM3_PrescalerConfig(TIM3_Prescaler_TypeDef Prescaler,
                          TIM3_PSCReloadMode_TypeDef TIM3_PSCReloadMode)
{
  /* Check the parameters */
  assert_param(IS_TIM3_PRESCALER(Prescaler));
  assert_param(IS_TIM3_PRESCALER_RELOAD(TIM3_PSCReloadMode));

  /* Set the Prescaler value */
  TIM3->PSCR = (uint8_t)(Prescaler);

  /* Set or reset the UG Bit */
  if (TIM3_PSCReloadMode == TIM3_PSCReloadMode_Immediate)
  {
    TIM3->EGR |= TIM_EGR_UG ;
  }
  else
  {
    TIM3->EGR &= (uint8_t)(~TIM_EGR_UG) ;
  }
}

/**
  * @brief  Specifies the TIM3 Counter Mode to be used.
  * @param  TIM3_CounterMode: Specifies the Counter Mode to be used
  *          This parameter can be one of the following values:
  *            @arg TIM3_CounterMode_Up: Counter Up Mode
  *            @arg TIM3_CounterMode_Down: Counter Down Mode
  *            @arg TIM3_CounterMode_CenterAligned1: Counter Central aligned Mode 1
  *            @arg TIM3_CounterMode_CenterAligned2: Counter Central aligned Mode 2
  *            @arg TIM3_CounterMode_CenterAligned3: Counter Central aligned Mode 3      
  * @retval None
  */
void TIM3_CounterModeConfig(TIM3_CounterMode_TypeDef TIM3_CounterMode)
{
  uint8_t tmpcr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM3_COUNTER_MODE(TIM3_CounterMode));

  tmpcr1 = TIM3->CR1;

  /* Reset the CMS and DIR Bits */
  tmpcr1 &= (uint8_t)((uint8_t)(~TIM_CR1_CMS) & (uint8_t)(~TIM_CR1_DIR));

  /* Set the Counter Mode */
  tmpcr1 |= (uint8_t)TIM3_CounterMode;

  TIM3->CR1 = tmpcr1;
}

/**
  * @brief  Sets the TIM3 Counter Register value.
  * @param  Counter: Specifies the Counter register new value.
  *          This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM3_SetCounter(uint16_t Counter)
{

  /* Set the Counter Register value */
  TIM3->CNTRH = (uint8_t)(Counter >> 8);
  TIM3->CNTRL = (uint8_t)(Counter);
}

/**
  * @brief  Sets the TIM3 Autoreload Register value.
  * @param  Autoreload: Specifies the Autoreload register new value.
  *          This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM3_SetAutoreload(uint16_t Autoreload)
{
  /* Set the Autoreload Register value */
  TIM3->ARRH = (uint8_t)(Autoreload >> 8);
  TIM3->ARRL = (uint8_t)(Autoreload);
}

/**
  * @brief  Gets the TIM3 Counter value.
  * @param  None
  * @retval Counter Register value.
  */
uint16_t TIM3_GetCounter(void)
{
  uint16_t tmpcnt = 0;
  uint8_t tmpcntrl, tmpcntrh;

  tmpcntrh = TIM3->CNTRH;
  tmpcntrl = TIM3->CNTRL;

  tmpcnt = (uint16_t)(tmpcntrl);
  tmpcnt |= (uint16_t)((uint16_t)tmpcntrh << 8);
  /* Get the Counter Register value */
  return ((uint16_t)tmpcnt);
}

/**
  * @brief  Gets the TIM3 Prescaler value.
  * @param  None
  * @retval TIM3 Prescaler, it can be one of the following values:
  *            - TIM3_Prescaler_1: Time base Prescaler = 1 (No effect)
  *            - TIM3_Prescaler_2: Time base Prescaler = 2
  *            - TIM3_Prescaler_4: Time base Prescaler = 4
  *            - TIM3_Prescaler_8: Time base Prescaler = 8
  *            - TIM3_Prescaler_16: Time base Prescaler = 16
  *            - TIM3_Prescaler_32: Time base Prescaler = 32
  *            - TIM3_Prescaler_64: Time base Prescaler = 64
  *            - TIM3_Prescaler_128: Time base Prescaler = 128    
  */
TIM3_Prescaler_TypeDef TIM3_GetPrescaler(void)
{
  /* Get the Prescaler Register value */
  return ((TIM3_Prescaler_TypeDef)TIM3->PSCR);
}

/**
  * @brief  Enables or Disables the TIM3 Update event.
  * @param  NewState: The new state of the TIM3 peripheral Preload register.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */

void TIM3_UpdateDisableConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the UDIS Bit */
  if (NewState != DISABLE)
  {
    TIM3->CR1 |= TIM_CR1_UDIS;
  }
  else
  {
    TIM3->CR1 &= (uint8_t)(~TIM_CR1_UDIS);
  }
}

/**
  * @brief  Selects the TIM3 Update Request Interrupt source.
  * @param  TIM3_UpdateSource: Specifies the Update source.
  *          This parameter can be one of the following values:
  *            @arg TIM3_UpdateSource_Global: Global Update request source
  *            @arg TIM3_UpdateSource_Regular: Regular Update request source 
  * @retval None
  */
void TIM3_UpdateRequestConfig(TIM3_UpdateSource_TypeDef TIM3_UpdateSource)
{
  /* Check the parameters */
  assert_param(IS_TIM3_UPDATE_SOURCE(TIM3_UpdateSource));

  /* Set or Reset the URS Bit */
  if (TIM3_UpdateSource == TIM3_UpdateSource_Regular)
  {
    TIM3->CR1 |= TIM_CR1_URS ;
  }
  else
  {
    TIM3->CR1 &= (uint8_t)(~TIM_CR1_URS);
  }
}

/**
  * @brief  Enables or disables TIM3 peripheral Preload register on ARR.
  * @param  NewState: The new state of the TIM3 peripheral Preload register.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM3_ARRPreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the ARPE Bit */
  if (NewState != DISABLE)
  {
    TIM3->CR1 |= TIM_CR1_ARPE;
  }
  else
  {
    TIM3->CR1 &= (uint8_t)(~TIM_CR1_ARPE);
  }
}

/**
  * @brief  Selects the TIM’s One Pulse Mode.
  * @param  TIM3_OPMode: Specifies the OPM Mode to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM3_OPMode_Single: Single one Pulse mode (OPM Active)
  *            @arg TIM3_OPMode_Repetitive: Single one Pulse mode (OPM Active)  
  * @retval None
  */
void TIM3_SelectOnePulseMode(TIM3_OPMode_TypeDef TIM3_OPMode)
{
  /* Check the parameters */
  assert_param(IS_TIM3_OPM_MODE(TIM3_OPMode));

  /* Set or Reset the OPM Bit */
  if (TIM3_OPMode == TIM3_OPMode_Single)
  {
    TIM3->CR1 |= TIM_CR1_OPM ;
  }
  else
  {
    TIM3->CR1 &= (uint8_t)(~TIM_CR1_OPM);
  }
}

/**
  * @brief  Enables or disables the TIM3 peripheral.
  * @param  NewState: The new state of the TIM3 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM3_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* set or Reset the CEN Bit */
  if (NewState != DISABLE)
  {
    TIM3->CR1 |= TIM_CR1_CEN;
  }
  else
  {
    TIM3->CR1 &= (uint8_t)(~TIM_CR1_CEN);
  }
}

/**
  * @}
  */

/** @defgroup TIM3_Group2 Output Compare management functions
 *  @brief    Output Compare management functions 
 *
@verbatim   
 ===============================================================================
                        Output Compare management functions
 ===============================================================================  
   
       ===================================================================      
              TIM3 Driver: how to use it in Output Compare Mode
       =================================================================== 
       To use the Timer in Output Compare mode, the following steps are mandatory:
       
       1. Enable TIM3 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE) function.
       
       2. Configure the TIM3 pins in output mode by configuring the corresponding GPIO pins
          
       3. Configure the Time base unit as described in the first part of this driver, if needed,
          otherwise the Timer will run with the default configuration:
          - Autoreload value = 0xFFFF
          - Prescaler value = 0x0000
          - Counter mode = Up counting
      
       4. Call TIM3_OCxInit() to configure the channel x with the desired parameters
          including:
          - TIM3 Output Compare mode: TIM3_OCMode
          - TIM3 Output State: TIM3_OutputState
          - TIM3 Pulse value: TIM3_Pulse
          - TIM3 Output Compare Polarity: TIM3_OCPolarity
          - TIM3 Output Idle State: TIM3_OCIdleState
       
       5. Call the TIM3_Cmd(ENABLE) function to enable the TIM3 counter.
       
       Note1: All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 
       
       Note2: If the corresponding interrupt or DMA request are needed, the user should:
              1. Enable global interrupts (or the DMA) to use the TIM3 interrupts (or DMA requests). 
              2. Enable the corresponding interrupt (or DMA request) using the function 
              TIM3_ITConfig(TIM3_IT_CCx) (or TIM3_DMACmd(TIM3_DMASource_CCx))   

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the TIM3 Channel1 according to the specified parameters.
  * @param  TIM3_OCMode: Output Compare Mode 
  *          This parameter can be one of the following values:
  *            @arg TIM3_OCMode_Timing: Timing (Frozen) Mode
  *            @arg TIM3_OCMode_Active: Active Mode
  *            @arg TIM3_OCMode_Inactive: Inactive Mode
  *            @arg TIM3_OCMode_Toggle: Toggle Mode
  *            @arg TIM3_OCMode_PWM1: PWM Mode 1
  *            @arg TIM3_OCMode_PWM2: PWM Mode 2          
  * @param  TIM3_OutputState: Output state
  *          This parameter can be one of the following values:
  *            @arg TIM3_OutputState_Disable: Output compare State disabled (channel output disabled)
  *            @arg TIM3_OutputState_Enable: Output compare State enabled (channel output enabled)
  * @param  TIM3_Pulse: This parameter must be a value between 0x0000 and 0xFFFF.
  * @param  TIM3_OCPolarity: Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM3_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM3_OCPolarity_Low: Output compare polarity  = Low 
  * @param  TIM3_OCIdleState: Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM3_OCIdleState_Reset: Output Compare Idle state  = Reset
  *            @arg TIM3_OCIdleState_Set: Output Compare Idle state  = Set
  * @retval None
  */
void TIM3_OC1Init(TIM3_OCMode_TypeDef TIM3_OCMode,
                  TIM3_OutputState_TypeDef TIM3_OutputState,
                  uint16_t TIM3_Pulse,
                  TIM3_OCPolarity_TypeDef TIM3_OCPolarity,
                  TIM3_OCIdleState_TypeDef TIM3_OCIdleState)
{
  uint8_t tmpccmr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM3_OC_MODE(TIM3_OCMode));
  assert_param(IS_TIM3_OUTPUT_STATE(TIM3_OutputState));
  assert_param(IS_TIM3_OC_POLARITY(TIM3_OCPolarity));
  assert_param(IS_TIM3_OCIDLE_STATE(TIM3_OCIdleState));

  tmpccmr1 = TIM3->CCMR1;

  /* Disable the Channel 1: Reset the CCE Bit */
  TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E);
  /* Reset the Output Compare Bits */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Set the Output Compare Mode */
  tmpccmr1 |= (uint8_t)TIM3_OCMode;

  TIM3->CCMR1 = tmpccmr1;

  /* Set the Output State */
  if (TIM3_OutputState == TIM3_OutputState_Enable)
  {
    TIM3->CCER1 |= TIM_CCER1_CC1E;
  }
  else
  {
    TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E);
  }

  /* Set the Output Polarity */
  if (TIM3_OCPolarity == TIM3_OCPolarity_Low)
  {
    TIM3->CCER1 |= TIM_CCER1_CC1P;
  }
  else
  {
    TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P);
  }

  /* Set the Output Idle state */
  if (TIM3_OCIdleState == TIM3_OCIdleState_Set)
  {
    TIM3->OISR |= TIM_OISR_OIS1;
  }
  else
  {
    TIM3->OISR &= (uint8_t)(~TIM_OISR_OIS1);
  }

  /* Set the Pulse value */
  TIM3->CCR1H = (uint8_t)(TIM3_Pulse >> 8);
  TIM3->CCR1L = (uint8_t)(TIM3_Pulse);
}

/**
  * @brief  Initializes the TIM3 Channel2 according to the specified parameters.
  * @param  TIM3_OCMode: Output Compare Mode 
  *          This parameter can be one of the following values:
  *            @arg TIM3_OCMode_Timing: Timing (Frozen) Mode
  *            @arg TIM3_OCMode_Active: Active Mode
  *            @arg TIM3_OCMode_Inactive: Inactive Mode
  *            @arg TIM3_OCMode_Toggle: Toggle Mode
  *            @arg TIM3_OCMode_PWM1: PWM Mode 1
  *            @arg TIM3_OCMode_PWM2: PWM Mode 2          
  * @param  TIM3_OutputState: Output state
  *          This parameter can be one of the following values:
  *            @arg TIM3_OutputState_Disable: Output compare State disabled (channel output disabled)
  *            @arg TIM3_OutputState_Enable: Output compare State enabled (channel output enabled)
  * @param  TIM3_Pulse: This parameter must be a value between 0x0000 and 0xFFFF.
  * @param  TIM3_OCPolarity: Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM3_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM3_OCPolarity_Low: Output compare polarity  = Low 
  * @param  TIM3_OCIdleState: Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM3_OCIdleState_Reset: Output Compare Idle state  = Reset
  *            @arg TIM3_OCIdleState_Set: Output Compare Idle state  = Set
  * @retval None
  */
void TIM3_OC2Init(TIM3_OCMode_TypeDef TIM3_OCMode,
                  TIM3_OutputState_TypeDef TIM3_OutputState,
                  uint16_t TIM3_Pulse,
                  TIM3_OCPolarity_TypeDef TIM3_OCPolarity,
                  TIM3_OCIdleState_TypeDef TIM3_OCIdleState)
{
  uint8_t tmpccmr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM3_OC_MODE(TIM3_OCMode));
  assert_param(IS_TIM3_OUTPUT_STATE(TIM3_OutputState));
  assert_param(IS_TIM3_OC_POLARITY(TIM3_OCPolarity));
  assert_param(IS_TIM3_OCIDLE_STATE(TIM3_OCIdleState));

  tmpccmr2 = TIM3->CCMR2;

  /* Disable the Channel 2: Reset the CCE Bit */
  TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC2E);

  /* Reset the Output Compare Bits */
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Set the Output Compare Mode */
  tmpccmr2 |= (uint8_t)TIM3_OCMode;

  TIM3->CCMR2 = tmpccmr2;

  /* Set the Output State */
  if (TIM3_OutputState == TIM3_OutputState_Enable)
  {
    TIM3->CCER1 |= TIM_CCER1_CC2E;
  }
  else
  {
    TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC2E);
  }

  /* Set the Output Polarity */
  if (TIM3_OCPolarity == TIM3_OCPolarity_Low)
  {
    TIM3->CCER1 |= TIM_CCER1_CC2P;
  }
  else
  {
    TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC2P);
  }


  /* Set the Output Idle state */
  if (TIM3_OCIdleState == TIM3_OCIdleState_Set)
  {
    TIM3->OISR |= TIM_OISR_OIS2;
  }
  else
  {
    TIM3->OISR &= (uint8_t)(~TIM_OISR_OIS2);
  }

  /* Set the Pulse value */
  TIM3->CCR2H = (uint8_t)(TIM3_Pulse >> 8);
  TIM3->CCR2L = (uint8_t)(TIM3_Pulse);
}

/**
  * @brief  Configures the Break feature, dead time, Lock level, the OSSI,
  *         and the AOE(automatic output enable).
  * @param  TIM3_OSSIState: Off-State Selection for Idle mode states.
  *          This parameter can be one of the following values:
  *            @arg TIM3_OSSIState_Enable: Off-State Selection for Idle mode enabled
  *            @arg TIM3_OSSIState_Disable: Off-State Selection for Idle mode disabled 
  * @param  TIM3_LockLevel: Lock level.
  *          This parameter can be one of the following values:
  *            @arg TIM3_LockLevel_Off: Lock option disabled
  *            @arg TIM3_LockLevel_1: Select Lock Level 1
  *            @arg TIM3_LockLevel_2: Select Lock Level 2
  *            @arg TIM3_LockLevel_3: Select Lock Level 3    
  * @param  TIM3_BreakState: Break Input enable/disable .
  *          This parameter can be one of the following values:
  *            @arg TIM3_BreakState_Disable: Break State disabled (break option disabled)
  *            @arg TIM3_BreakState_Enable: Break State enabled (break option enabled) 
  * @param  TIM3_BreakPolarity: Break Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM3_BreakPolarity_High: if Break, channel polarity = High
  *            @arg TIM3_BreakPolarity_Low: if Break, channel polarity = Low   
  * @param  TIM3_AutomaticOutput: TIM3 AOE Bit Set/Reset .
  *          This parameter can be one of the following values:
  *            @arg TIM3_AutomaticOutput_Enable: Automatic Output option enabled
  *            @arg TIM3_AutomaticOutput_Disable: Automatic Output option disabled
  * @retval None
  */
void TIM3_BKRConfig(TIM3_OSSIState_TypeDef TIM3_OSSIState,
                    TIM3_LockLevel_TypeDef TIM3_LockLevel,
                    TIM3_BreakState_TypeDef TIM3_BreakState,
                    TIM3_BreakPolarity_TypeDef TIM3_BreakPolarity,
                    TIM3_AutomaticOutput_TypeDef TIM3_AutomaticOutput)

{
  /* Check the parameters */
  assert_param(IS_TIM3_OSSI_STATE(TIM3_OSSIState));
  assert_param(IS_TIM3_LOCK_LEVEL(TIM3_LockLevel));
  assert_param(IS_TIM3_BREAK_STATE(TIM3_BreakState));
  assert_param(IS_TIM3_BREAK_POLARITY(TIM3_BreakPolarity));
  assert_param(IS_TIM3_AUTOMATIC_OUTPUT_STATE(TIM3_AutomaticOutput));



  /* Set the Lock level, the Break enable Bit and the Polarity, the OSSI State,
  the dead time value and the Automatic Output Enable Bit */
  TIM3->BKR = (uint8_t)((uint8_t)((uint8_t)((uint8_t)((uint8_t)TIM3_OSSIState | (uint8_t)TIM3_LockLevel) | \
                                  (uint8_t)((uint8_t)TIM3_BreakState | (uint8_t)TIM3_BreakPolarity)) | \
                                  TIM3_AutomaticOutput));
}

/**
  * @brief  Enables or disables the TIM3 peripheral Main Outputs.
  * @param  NewState: The new state of the TIM3 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM3_CtrlPWMOutputs(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the MOE Bit */

  if (NewState != DISABLE)
  {
    TIM3->BKR |= TIM_BKR_MOE ;
  }
  else
  {
    TIM3->BKR &= (uint8_t)(~TIM_BKR_MOE) ;
  }
}

/**
  * @brief  Selects the TIM3 Output Compare Mode. This function disables the
  *         selected channel before changing the Output Compare Mode. User has to
  *         enable this channel using TIM3_CCxCmd and TIM3_CCxNCmd functions.
  * @param  TIM3_Channel: Specifies the TIM3 Channel.
  *          This parameter can be one of the following values:
  *            @arg TIM3_Channel_1: Channel 1
  *            @arg TIM3_Channel_2: Channel 2  
  * @param  TIM3_OCMode: Specifies the TIM3 Output Compare Mode.
  *          This parameter can be one of the following values:
  *            @arg TIM3_OCMode_Timing: Timing (Frozen) Mode
  *            @arg TIM3_OCMode_Active: Active Mode
  *            @arg TIM3_OCMode_Inactive: Inactive Mode
  *            @arg TIM3_OCMode_Toggle: Toggle Mode
  *            @arg TIM3_OCMode_PWM1: PWM Mode 1
  *            @arg TIM3_OCMode_PWM2: PWM Mode 2    
  * @retval None
  */
void TIM3_SelectOCxM(TIM3_Channel_TypeDef TIM3_Channel,
                     TIM3_OCMode_TypeDef TIM3_OCMode)
{
  /* Check the parameters */
  assert_param(IS_TIM3_CHANNEL(TIM3_Channel));
  assert_param(IS_TIM3_OCM(TIM3_OCMode));

  if (TIM3_Channel == TIM3_Channel_1)
  {
    /* Disable the Channel 1: Reset the CCE Bit */
    TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E);

    /* Reset the Output Compare Bits */
    TIM3->CCMR1 &= (uint8_t)(~TIM_CCMR_OCM);

    /* Set the Output Compare Mode */
    TIM3->CCMR1 |= (uint8_t)TIM3_OCMode;
  }
  else /* if (TIM3_Channel == TIM3_Channel_2) */
  {
    /* Disable the Channel 2: Reset the CCE Bit */
    TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC2E);

    /* Reset the Output Compare Bits */
    TIM3->CCMR2 &= (uint8_t)(~TIM_CCMR_OCM);

    /* Set the Output Compare Mode */
    TIM3->CCMR2 |= (uint8_t)TIM3_OCMode;
  }
}

/**
  * @brief  Sets the TIM3 Capture Compare1 Register value.
  * @param  Compare: Specifies the Capture Compare1 register new value.
  *         This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM3_SetCompare1(uint16_t Compare)
{
  /* Set the Capture Compare1 Register value */
  TIM3->CCR1H = (uint8_t)(Compare >> 8);
  TIM3->CCR1L = (uint8_t)(Compare);
}

/**
  * @brief  Sets the TIM3 Capture Compare2 Register value.
  * @param  Compare: Specifies the Capture Compare2 register new value.
  *         This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM3_SetCompare2(uint16_t Compare)
{
  /* Set the Capture Compare2 Register value */
  TIM3->CCR2H = (uint8_t)(Compare >> 8);
  TIM3->CCR2L = (uint8_t)(Compare);
}

/**
  * @brief  Forces the TIM3 Channel1 output waveform to active or inactive level.
  * @param  TIM3_ForcedAction: Specifies the forced Action to be set to the output waveform.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ForcedAction_Active: Output Reference is forced low 
  *            @arg TIM3_ForcedAction_Inactive: Output Reference is forced high 
  * @retval None
  */
void TIM3_ForcedOC1Config(TIM3_ForcedAction_TypeDef TIM3_ForcedAction)
{
  uint8_t tmpccmr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM3_FORCED_ACTION(TIM3_ForcedAction));

  tmpccmr1 = TIM3->CCMR1;

  /* Reset the OCM Bits */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Configure The Forced output Mode */
  tmpccmr1 |= (uint8_t)TIM3_ForcedAction;

  TIM3->CCMR1 = tmpccmr1;
}

/**
  * @brief  Forces the TIM3 Channel2 output waveform to active or inactive level.
  * @param  TIM3_ForcedAction: Specifies the forced Action to be set to the output waveform.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ForcedAction_Active: Output Reference is forced low 
  *            @arg TIM3_ForcedAction_Inactive: Output Reference is forced high 
  * @retval None
  */
void TIM3_ForcedOC2Config(TIM3_ForcedAction_TypeDef TIM3_ForcedAction)
{
  uint8_t tmpccmr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM3_FORCED_ACTION(TIM3_ForcedAction));

  tmpccmr2 = TIM3->CCMR2;

  /* Reset the OCM Bits */
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_OCM);

  /* Configure The Forced output Mode */
  tmpccmr2 |= (uint8_t)TIM3_ForcedAction;

  TIM3->CCMR2 = tmpccmr2;
}

/**
  * @brief  Enables or disables the TIM3 peripheral Preload Register on CCR1.
  * @param  NewState: The new state of the Capture Compare Preload register.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM3_OC1PreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC1PE Bit */
  if (NewState != DISABLE)
  {
    TIM3->CCMR1 |= TIM_CCMR_OCxPE ;
  }
  else
  {
    TIM3->CCMR1 &= (uint8_t)(~TIM_CCMR_OCxPE) ;
  }
}

/**
  * @brief  Enables or disables the TIM3 peripheral Preload Register on CCR2.
  * @param  NewState: The new state of the Capture Compare Preload register.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM3_OC2PreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC2PE Bit */
  if (NewState != DISABLE)
  {
    TIM3->CCMR2 |= TIM_CCMR_OCxPE ;
  }
  else
  {
    TIM3->CCMR2 &= (uint8_t)(~TIM_CCMR_OCxPE) ;
  }
}

/**
  * @brief  Configures the TIM3 Capture Compare 1 Fast feature.
  * @param  NewState: The new state of the Output Compare Fast Enable bit.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM3_OC1FastConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC1FE Bit */
  if (NewState != DISABLE)
  {
    TIM3->CCMR1 |= TIM_CCMR_OCxFE ;
  }
  else
  {
    TIM3->CCMR1 &= (uint8_t)(~TIM_CCMR_OCxFE) ;
  }
}

/**
  * @brief  Configures the TIM3 Capture Compare 2 Fast feature.
  * @param  NewState: The new state of the Output Compare Fast Enable bit.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */

void TIM3_OC2FastConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC2FE Bit */
  if (NewState != DISABLE)
  {
    TIM3->CCMR2 |= TIM_CCMR_OCxFE ;
  }
  else
  {
    TIM3->CCMR2 &= (uint8_t)(~TIM_CCMR_OCxFE) ;
  }
}

/**
  * @brief  Configures the TIM3 Channel 1 polarity.
  * @param  TIM3_OCPolarity: Specifies the OC1 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM3_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM3_OCPolarity_Low: Output compare polarity  = Low 
  * @retval None
  */
void TIM3_OC1PolarityConfig(TIM3_OCPolarity_TypeDef TIM3_OCPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM3_OC_POLARITY(TIM3_OCPolarity));

  /* Set or Reset the CC1P Bit */
  if (TIM3_OCPolarity == TIM3_OCPolarity_Low)
  {
    TIM3->CCER1 |= TIM_CCER1_CC1P ;
  }
  else
  {
    TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P) ;
  }
}

/**
  * @brief  Configures the TIM3 Channel 2 polarity.
  * @param  TIM3_OCPolarity: Specifies the OC2 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM3_OCPolarity_High: Output compare polarity  = High
  *            @arg TIM3_OCPolarity_Low: Output compare polarity  = Low 
  * @retval None
  */
void TIM3_OC2PolarityConfig(TIM3_OCPolarity_TypeDef TIM3_OCPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM3_OC_POLARITY(TIM3_OCPolarity));

  /* Set or Reset the CC2P Bit */
  if (TIM3_OCPolarity == TIM3_OCPolarity_Low)
  {
    TIM3->CCER1 |= TIM_CCER1_CC2P ;
  }
  else
  {
    TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC2P) ;
  }
}

/**
  * @brief  Enables or disables the TIM3 Capture Compare Channel x.
  * @param  TIM3_Channel: Specifies the TIM3 Channel.
  *          This parameter can be one of the following values:
  *            @arg TIM3_Channel_1: Channel 1
  *            @arg TIM3_Channel_2: Channel 2  
  * @param  NewState: Specifies the TIM3 Channel CCxE bit new state.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM3_CCxCmd(TIM3_Channel_TypeDef TIM3_Channel,
                 FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM3_CHANNEL(TIM3_Channel));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (TIM3_Channel == TIM3_Channel_1)
  {
    /* Set or Reset the CC1E Bit */
    if (NewState != DISABLE)
    {
      TIM3->CCER1 |= TIM_CCER1_CC1E ;
    }
    else
    {
      TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC1E) ;
    }

  }
  else /* if (TIM3_Channel == TIM3_Channel_2) */
  {
    /* Set or Reset the CC2E Bit */
    if (NewState != DISABLE)
    {
      TIM3->CCER1 |= TIM_CCER1_CC2E;
    }
    else
    {
      TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC2E) ;
    }
  }
}

/** @defgroup TIM3_Group3 Input Capture management functions
 *  @brief    Input Capture management functions 
 *
@verbatim   
 ===============================================================================
                      Input Capture management functions
 ===============================================================================  
   
       ===================================================================      
              TIM3 Driver: how to use it in Input Capture Mode
       =================================================================== 
       To use the Timer in Input Capture mode, the following steps are mandatory:
       
       1. Enable TIM3 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE) function.
       
       2. Configure the TIM3 pins in input mode by configuring the corresponding GPIO pins
       
       3. Configure the Time base unit as described in the first part of this driver, if needed,
          otherwise the Timer will run with the default configuration:
          - Autoreload value = 0xFFFF
          - Prescaler value = 0x0
          - Counter mode = Up counting
       
       4. Call TIM3_ICInit() to configure the desired channel to measure only 
          frequency or duty cycle of the input signal using the corresponding configuration: 
          - TIM3 Channel: TIM3_Channel
          - TIM3 Input Capture polarity: TIM3_ICPolarity
          - TIM3 Input Capture selection: TIM3_ICSelection
          - TIM3 Input Capture Prescaler: TIM3_ICPSC
          - TIM3 Input Capture filter value
          or,
          Call TIM3_PWMIConfig() to configure the desired channels with the 
          corresponding configuration and to measure the frequency and the duty
          cycle of the input signal.
          
       5. Enable global interrupts or the DMA to read the measured frequency. 
          
       6. Enable the corresponding interrupt (or DMA request) to read the captured value,
          using the function TIM3_ITConfig(TIM3_IT_CCx) (or TIM3_DMACmd(TIM3_DMASource_CCx))
       
       7. Call the TIM3_Cmd(ENABLE) function to enable the TIM3 counter.
       
       8. Use TIM3_GetCapturex() to read the captured value corresponding to
          channel x.
       
       Note1: All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the TIM3 peripheral according to the specified parameters.
  * @param  TIM3_Channel: TIM3 Channel
  *          This parameter can be one of the following values:
  *            @arg TIM3_Channel_1: Channel 1
  *            @arg TIM3_Channel_2: Channel 2     
  * @param  TIM3_ICPolarity: Input Capture Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM3_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM3_ICSelection: Input Capture Selection
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICSelection_DirectTI: Input Capture mapped on the direct input
  *            @arg TIM3_ICSelection_IndirectTI: Input Capture mapped on the indirect input
  *            @arg TIM3_ICSelection_TRGI: Input Capture mapped on the Trigger Input   
  * @param  TIM3_ICPrescaler: Input Capture Prescaler
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM3_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM3_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM3_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events)   
  * @param  TIM3_ICFilter: This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
void TIM3_ICInit(TIM3_Channel_TypeDef TIM3_Channel,
                 TIM3_ICPolarity_TypeDef TIM3_ICPolarity,
                 TIM3_ICSelection_TypeDef TIM3_ICSelection,
                 TIM3_ICPSC_TypeDef TIM3_ICPrescaler,
                 uint8_t TIM3_ICFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM3_CHANNEL(TIM3_Channel));

  if (TIM3_Channel == TIM3_Channel_1)
  {
    /* TI1 Configuration */
    TI1_Config(TIM3_ICPolarity, TIM3_ICSelection, TIM3_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM3_SetIC1Prescaler(TIM3_ICPrescaler);
  }
  else /* if (TIM3_Channel == TIM3_Channel_2) */
  {
    /* TI2 Configuration */
    TI2_Config(TIM3_ICPolarity, TIM3_ICSelection, TIM3_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM3_SetIC2Prescaler(TIM3_ICPrescaler);
  }
}

/**
  * @brief  Configures the TIM3 peripheral in PWM Input Mode according to the
  *         specified parameters.
  * @param  TIM3_Channel: TIM3 Channel
  *          This parameter can be one of the following values:
  *            @arg TIM3_Channel_1: Channel 1
  *            @arg TIM3_Channel_2: Channel 2     
  * @param  TIM3_ICPolarity: Input Capture Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM3_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM3_ICSelection: Input Capture Selection
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICSelection_DirectTI: Input Capture mapped on the direct input
  *            @arg TIM3_ICSelection_IndirectTI: Input Capture mapped on the indirect input
  *            @arg TIM3_ICSelection_TRGI: Input Capture mapped on the Trigger Input   
  * @param  TIM3_ICPrescaler: Input Capture Prescaler
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM3_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM3_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM3_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events) 
  * @retval None
  */
void TIM3_PWMIConfig(TIM3_Channel_TypeDef TIM3_Channel,
                     TIM3_ICPolarity_TypeDef TIM3_ICPolarity,
                     TIM3_ICSelection_TypeDef TIM3_ICSelection,
                     TIM3_ICPSC_TypeDef TIM3_ICPrescaler,
                     uint8_t TIM3_ICFilter)
{
  uint8_t icpolarity = TIM3_ICPolarity_Rising;
  uint8_t icselection = TIM3_ICSelection_DirectTI;

  /* Check the parameters */
  assert_param(IS_TIM3_CHANNEL(TIM3_Channel));

  /* Select the Opposite Input Polarity */
  if (TIM3_ICPolarity == TIM3_ICPolarity_Rising)
  {
    icpolarity = TIM3_ICPolarity_Falling;
  }
  else
  {
    icpolarity = TIM3_ICPolarity_Rising;
  }

  /* Select the Opposite Input */
  if (TIM3_ICSelection == TIM3_ICSelection_DirectTI)
  {
    icselection = TIM3_ICSelection_IndirectTI;
  }
  else
  {
    icselection = TIM3_ICSelection_DirectTI;
  }

  if (TIM3_Channel == TIM3_Channel_1)
  {
    /* TI1 Configuration */
    TI1_Config(TIM3_ICPolarity, TIM3_ICSelection,
               TIM3_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM3_SetIC1Prescaler(TIM3_ICPrescaler);

    /* TI2 Configuration */
    TI2_Config((TIM3_ICPolarity_TypeDef)icpolarity, (TIM3_ICSelection_TypeDef)icselection, TIM3_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM3_SetIC2Prescaler(TIM3_ICPrescaler);
  }
  else
  {
    /* TI2 Configuration */
    TI2_Config(TIM3_ICPolarity, TIM3_ICSelection,
               TIM3_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM3_SetIC2Prescaler(TIM3_ICPrescaler);

    /* TI1 Configuration */
    TI1_Config((TIM3_ICPolarity_TypeDef)icpolarity, (TIM3_ICSelection_TypeDef)icselection, TIM3_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM3_SetIC1Prescaler(TIM3_ICPrescaler);
  }
}

/**
  * @brief  Gets the TIM3 Input Capture 1 value.
  * @param  None
  * @retval Capture Compare 1 Register value.
  */
uint16_t TIM3_GetCapture1(void)
{
  uint16_t tmpccr1 = 0;
  uint8_t tmpccr1l, tmpccr1h;

  tmpccr1h = TIM3->CCR1H;
  tmpccr1l = TIM3->CCR1L;

  tmpccr1 = (uint16_t)(tmpccr1l);
  tmpccr1 |= (uint16_t)((uint16_t)tmpccr1h << 8);
  /* Get the Capture 1 Register value */
  return ((uint16_t)tmpccr1);
}

/**
  * @brief  Gets the TIM3 Input Capture 2 value.
  * @param  None
  * @retval Capture Compare 2 Register value.
  */
uint16_t TIM3_GetCapture2(void)
{
  uint16_t tmpccr2 = 0;
  uint8_t tmpccr2l, tmpccr2h;

  tmpccr2h = TIM3->CCR2H;
  tmpccr2l = TIM3->CCR2L;

  tmpccr2 = (uint16_t)(tmpccr2l);
  tmpccr2 |= (uint16_t)((uint16_t)tmpccr2h << 8);
  /* Get the Capture 2 Register value */
  return ((uint16_t)tmpccr2);
}

/**
  * @brief  Sets the TIM3 Input Capture 1 prescaler.
  * @param  TIM3_IC1Prescaler: Specifies the Input Capture prescaler new value
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM3_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM3_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM3_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events) 
  * @retval None
  */
void TIM3_SetIC1Prescaler(TIM3_ICPSC_TypeDef TIM3_IC1Prescaler)
{
  uint8_t tmpccmr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM3_IC_PRESCALER(TIM3_IC1Prescaler));

  tmpccmr1 = TIM3->CCMR1;

  /* Reset the IC1PSC Bits */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_ICxPSC);

  /* Set the IC1PSC value */
  tmpccmr1 |= (uint8_t)TIM3_IC1Prescaler;

  TIM3->CCMR1 = tmpccmr1;
}

/**
  * @brief  Sets the TIM3 Input Capture 2 prescaler.
  * @param  TIM3_IC2Prescaler: Specifies the Input Capture prescaler new value
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM3_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM3_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM3_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events) 
  * @retval None
  */
void TIM3_SetIC2Prescaler(TIM3_ICPSC_TypeDef TIM3_IC2Prescaler)
{
  uint8_t tmpccmr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM3_IC_PRESCALER(TIM3_IC2Prescaler));

  tmpccmr2 = TIM3->CCMR2;

  /* Reset the IC2PSC Bits */
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_ICxPSC);

  /* Set the IC2PSC value */
  tmpccmr2 |= (uint8_t)TIM3_IC2Prescaler;

  TIM3->CCMR2 = tmpccmr2;
}

/**
  * @}
  */

/** @defgroup TIM3_Group4 Interrupts DMA and flags management functions
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
  * @brief  Enables or disables the specified TIM3 interrupts.
  * @param  TIM3_IT: Specifies the TIM3 interrupts sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg TIM3_IT_Update: Update
  *            @arg TIM3_IT_CC1: Capture Compare Channel1
  *            @arg TIM3_IT_CC2: Capture Compare Channel2 
  *            @arg TIM3_IT_Trigger: Trigger 
  *            @arg TIM3_IT_Break: Break  
  * @param  NewState: The new state of the TIM3 peripheral.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM3_ITConfig(TIM3_IT_TypeDef TIM3_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM3_IT(TIM3_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the Interrupt sources */
    TIM3->IER |= (uint8_t)TIM3_IT;
  }
  else
  {
    /* Disable the Interrupt sources */
    TIM3->IER &= (uint8_t)(~(uint8_t)TIM3_IT);
  }
}

/**
  * @brief  Configures the TIM3 event to be generated by software.
  * @param  TIM3_EventSource: Specifies the event source.
  *          This parameter can be any combination of the following values:
  *            @arg TIM3_EventSource_Update: Update
  *            @arg TIM3_EventSource_CC1: Capture Compare Channel1
  *            @arg TIM3_EventSource_CC2: Capture Compare Channel2 
  *            @arg TIM3_EventSource_Trigger: Trigger 
  *            @arg TIM3_EventSource_Break: Break  
  * @retval None
  */
void TIM3_GenerateEvent(TIM3_EventSource_TypeDef TIM3_EventSource)
{
  /* Check the parameters */
  assert_param(IS_TIM3_EVENT_SOURCE((uint8_t)TIM3_EventSource));

  /* Set the event sources */
  TIM3->EGR |= (uint8_t)TIM3_EventSource;
}

/**
  * @brief  Checks whether the specified TIM3 flag is set or not.
  * @param  TIM3_FLAG: Specifies the flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg TIM3_FLAG_Update: Update
  *            @arg TIM3_FLAG_CC1: Capture Compare Channel1
  *            @arg TIM3_FLAG_CC2: Capture Compare Channel2 
  *            @arg TIM3_FLAG_Trigger: Trigger 
  *            @arg TIM3_FLAG_Break: Break  
  *            @arg TIM3_FLAG_CC1OF: Capture compare 1 over capture
  *            @arg TIM3_FLAG_CC2OF: Capture compare 2 over capture   
  * @retval FlagStatus: The new state of TIM3_FLAG (SET or RESET)
  */
FlagStatus TIM3_GetFlagStatus(TIM3_FLAG_TypeDef TIM3_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint8_t tim3_flag_l = 0, tim3_flag_h = 0;

  /* Check the parameters */
  assert_param(IS_TIM3_GET_FLAG(TIM3_FLAG));

  tim3_flag_l = (uint8_t)(TIM3->SR1 & (uint8_t)(TIM3_FLAG));
  tim3_flag_h = (uint8_t)(TIM3->SR2 & (uint8_t)((uint16_t)TIM3_FLAG >> 8));

  if ((uint8_t)(tim3_flag_l | tim3_flag_h) != 0)
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
  * @param  TIM3_FLAG: Specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM3_FLAG_Update: Update
  *            @arg TIM3_FLAG_CC1: Capture Compare Channel1
  *            @arg TIM3_FLAG_CC2: Capture Compare Channel2 
  *            @arg TIM3_FLAG_Trigger: Trigger 
  *            @arg TIM3_FLAG_Break: Break  
  * @retval None
  */
void TIM3_ClearFlag(TIM3_FLAG_TypeDef TIM3_FLAG)
{
  /* Check the parameters */
  assert_param(IS_TIM3_CLEAR_FLAG((uint16_t)TIM3_FLAG));
  /* Clear the flags (rc_w0) clear this bit by writing 0. Writing ‘1’ has no effect*/
  TIM3->SR1 = (uint8_t)(~(uint8_t)(TIM3_FLAG));
  TIM3->SR2 = (uint8_t)(~(uint8_t)((uint16_t)TIM3_FLAG >> 8));
}

/**
  * @brief  Checks whether the TIM3 interrupt has occurred or not.
  * @param  TIM3_IT: Specifies the TIM3 interrupt source to check.
  *          This parameter can be any combination of the following values:
  *            @arg TIM3_IT_Update: Update
  *            @arg TIM3_IT_CC1: Capture Compare Channel1
  *            @arg TIM3_IT_CC2: Capture Compare Channel2 
  *            @arg TIM3_IT_Trigger: Trigger 
  *            @arg TIM3_IT_Break: Break  
  * @retval ITStatus: The new state of the TIM3_IT (SET or RESET)
  */
ITStatus TIM3_GetITStatus(TIM3_IT_TypeDef TIM3_IT)
{
  ITStatus bitstatus = RESET;

  uint8_t TIM3_itStatus = 0x0, TIM3_itEnable = 0x0;

  /* Check the parameters */
  assert_param(IS_TIM3_GET_IT(TIM3_IT));

  TIM3_itStatus = (uint8_t)(TIM3->SR1 & (uint8_t)TIM3_IT);

  TIM3_itEnable = (uint8_t)(TIM3->IER & (uint8_t)TIM3_IT);

  if ((TIM3_itStatus != (uint8_t)RESET ) && (TIM3_itEnable != (uint8_t)RESET))
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
  * @param  TIM3_IT: Specifies the pending bit to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM3_IT_Update: Update
  *            @arg TIM3_IT_CC1: Capture Compare Channel1
  *            @arg TIM3_IT_CC2: Capture Compare Channel2 
  *            @arg TIM3_IT_Trigger: Trigger 
  *            @arg TIM3_IT_Break: Break  
  * @retval None
  */
void TIM3_ClearITPendingBit(TIM3_IT_TypeDef TIM3_IT)
{
  /* Check the parameters */
  assert_param(IS_TIM3_IT(TIM3_IT));

  /* Clear the IT pending Bit */
  TIM3->SR1 = (uint8_t)(~(uint8_t)TIM3_IT);
}

/**
  * @brief  Enables or disables the TIM3 DMA Requests.
  * @param  TIM3_DMASource: specifies the DMA Request sources.
  *          This parameter can be any combination of the following values:
  *            @arg TIM3_DMASource_Update: TIM3 DMA Update Request
  *            @arg TIM3_DMASource_CC1: TIM3 DMA CC1 Request
  *            @arg TIM3_DMASource_CC2: TIM3 DMA CC2 Request 
  * @param  NewState: new state of the DMA Request sources.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM3_DMACmd( TIM3_DMASource_TypeDef TIM3_DMASource, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  assert_param(IS_TIM3_DMA_SOURCE(TIM3_DMASource));

  if (NewState != DISABLE)
  {
    /* Enable the DMA sources */
    TIM3->DER |= TIM3_DMASource;
  }
  else
  {
    /* Disable the DMA sources */
    TIM3->DER &= (uint8_t)(~TIM3_DMASource);
  }
}

/**
  * @brief  Selects the TIM3 peripheral Capture Compare DMA source.
  * @param   NewState: new state of the Capture Compare DMA source.
  *           This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM3_SelectCCDMA(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Set the CCDS Bit */
    TIM3->CR2 |= TIM_CR2_CCDS;
  }
  else
  {
    /* Reset the CCDS Bit */
    TIM3->CR2 &= (uint8_t)(~TIM_CR2_CCDS);
  }
}

/**
  * @}
  */

/** @defgroup TIM3_Group5 Clocks management functions
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
  * @brief  Enables the TIM3 internal Clock.
  * @par Parameters:
  * None
  * @retval None
  */
void TIM3_InternalClockConfig(void)
{
  /* Disable slave mode to clock the prescaler directly with the internal clock */
  TIM3->SMCR &=  (uint8_t)(~TIM_SMCR_SMS);
}

/**
  * @brief  Configures the TIM3 Trigger as External Clock.
  * @param  TIM3_TIxExternalCLKSource: Specifies Trigger source.
  *          This parameter can be one of the following values:
  *            @arg TIM3_TIxExternalCLK1Source_TI1ED: External Clock mode 1 source = TI1ED
  *            @arg TIM3_TIxExternalCLK1Source_TI1: External Clock mode 1 source = TI1 
  *            @arg TIM3_TIxExternalCLK1Source_TI2: External Clock mode 1 source = TI2  
  * @param  TIM3_ICPolarity: Specifies the TIx Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM3_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  ICFilter: Specifies the filter value.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM3_TIxExternalClockConfig(TIM3_TIxExternalCLK1Source_TypeDef TIM3_TIxExternalCLKSource,
                                 TIM3_ICPolarity_TypeDef TIM3_ICPolarity,
                                 uint8_t ICFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM3_TIXCLK_SOURCE(TIM3_TIxExternalCLKSource));
  assert_param(IS_TIM3_IC_POLARITY(TIM3_ICPolarity));
  assert_param(IS_TIM3_IC_FILTER(ICFilter));

  /* Configure the TIM3 Input Clock Source */
  if (TIM3_TIxExternalCLKSource == TIM3_TIxExternalCLK1Source_TI2)
  {
    TI2_Config(TIM3_ICPolarity, TIM3_ICSelection_DirectTI, ICFilter);
  }
  else
  {
    TI1_Config(TIM3_ICPolarity, TIM3_ICSelection_DirectTI, ICFilter);
  }

  /* Select the Trigger source */
  TIM3_SelectInputTrigger((TIM3_TRGSelection_TypeDef)TIM3_TIxExternalCLKSource);

  /* Select the External clock mode1 */
  TIM3->SMCR |= (uint8_t)(TIM3_SlaveMode_External1);
}

/**
  * @brief  Configures the TIM3 External clock Mode1.
  * @param  TIM3_ExtTRGPrescaler: Specifies the external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ExtTRGPSC_OFF: No External Trigger prescaler
  *            @arg TIM3_ExtTRGPSC_DIV2: External Trigger prescaler = 2 (ETRP frequency divided by 2)
  *            @arg TIM3_ExtTRGPSC_DIV4: External Trigger prescaler = 4 (ETRP frequency divided by 4) 
  *            @arg TIM3_ExtTRGPSC_DIV8: External Trigger prescaler = 8 (ETRP frequency divided by 8)   
  * @param  TIM3_ExtTRGPolarity: Specifies the external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ExtTRGPolarity_Inverted: External Trigger Polarity = inverted
  *            @arg TIM3_ExtTRGPolarity_NonInverted: External Trigger Polarity = non inverted
  * @param  ExtTRGFilter: Specifies the External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM3_ETRClockMode1Config(TIM3_ExtTRGPSC_TypeDef TIM3_ExtTRGPrescaler,
                              TIM3_ExtTRGPolarity_TypeDef TIM3_ExtTRGPolarity,
                              uint8_t ExtTRGFilter)
{
  /* Configure the ETR Clock source */
  TIM3_ETRConfig(TIM3_ExtTRGPrescaler, TIM3_ExtTRGPolarity, ExtTRGFilter);

  /* Select the External clock mode1 */
  TIM3->SMCR &= (uint8_t)(~TIM_SMCR_SMS);
  TIM3->SMCR |= (uint8_t)(TIM3_SlaveMode_External1);

  /* Select the Trigger selection: ETRF */
  TIM3->SMCR &= (uint8_t)(~TIM_SMCR_TS);
  TIM3->SMCR |= (uint8_t)((TIM3_TRGSelection_TypeDef)TIM3_TRGSelection_ETRF);
}

/**
  * @brief  Configures the TIM3 External clock Mode2.
  * @param  TIM3_ExtTRGPrescaler: Specifies the external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ExtTRGPSC_OFF: No External Trigger prescaler
  *            @arg TIM3_ExtTRGPSC_DIV2: External Trigger prescaler = 2 (ETRP frequency divided by 2)
  *            @arg TIM3_ExtTRGPSC_DIV4: External Trigger prescaler = 4 (ETRP frequency divided by 4) 
  *            @arg TIM3_ExtTRGPSC_DIV8: External Trigger prescaler = 8 (ETRP frequency divided by 8) 
  * @param  TIM3_ExtTRGPolarity: Specifies the external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ExtTRGPolarity_Inverted: External Trigger Polarity = inverted
  *            @arg TIM3_ExtTRGPolarity_NonInverted: External Trigger Polarity = non inverted
  * @param  ExtTRGFilter: Specifies the External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM3_ETRClockMode2Config(TIM3_ExtTRGPSC_TypeDef TIM3_ExtTRGPrescaler,
                              TIM3_ExtTRGPolarity_TypeDef TIM3_ExtTRGPolarity,
                              uint8_t ExtTRGFilter)
{
  /* Configure the ETR Clock source */
  TIM3_ETRConfig(TIM3_ExtTRGPrescaler, TIM3_ExtTRGPolarity, ExtTRGFilter);

  /* Enable the External clock mode2 */
  TIM3->ETR |= TIM_ETR_ECE ;
}

/**
  * @}
  */

/** @defgroup TIM3_Group6 Synchronization management functions
 *  @brief    Synchronization management functions 
 *
@verbatim   
 ===============================================================================
                       Synchronization management functions
 ===============================================================================  
                   
       ===================================================================      
              TIM3 Driver: how to use it in synchronization Mode
       =================================================================== 
       Case of two/several Timers
       **************************
       1. If TIM3 is used as master to other timers use the following functions:
          - TIM3_SelectOutputTrigger()
          - TIM3_SelectMasterSlaveMode()
       2. If TIM3 is used as slave to other timers use the following functions:
          - TIM3_SelectInputTrigger()
          - TIM3_SelectSlaveMode()
          
       Case of Timers and external trigger (TRIG pin)
       ********************************************       
       1. Configure the External trigger using TIM3_ETRConfig()
       2. Configure the Slave Timer using the following functions:
          - TIM3_SelectInputTrigger()
          - TIM3_SelectSlaveMode()

@endverbatim
  * @{
  */

/**
  * @brief  Selects the TIM3 Input Trigger source.
  * @param  TIM3_InputTriggerSource: Specifies Input Trigger source.
  *          This parameter can be one of the following values:
  *            @arg TIM3_TRGSelection_TIM4: TRIG Input source =  TIM TRIG Output
  *            @arg TIM3_TRGSelection_TIM1: TRIG Input source =  TIM TRIG Output
  *            @arg TIM3_TRGSelection_TIM3: TRIG Input source =  TIM TRIG Output
  *            @arg TIM3_TRGSelection_TIM2: TRIG Input source =  TIM TRIG Output
  *            @arg TIM3_TRGSelection_TI1F_ED: TRIG Input source = TI1F_ED (TI1 Edge Detector)
  *            @arg TIM3_TRGSelection_TI1FP1: TRIG Input source = TI1FP1 (Filtered Timer Input 1)
  *            @arg TIM3_TRGSelection_TI2FP2: TRIG Input source = TI2FP2 (Filtered Timer Input 2)
  *            @arg TIM3_TRGSelection_ETRF: TRIG Input source =  ETRF (External Trigger Input )      
  * @retval None
  */
void TIM3_SelectInputTrigger(TIM3_TRGSelection_TypeDef TIM3_InputTriggerSource)
{
  uint8_t tmpsmcr = 0;

  /* Check the parameters */
  assert_param(IS_TIM3_TRIGGER_SELECTION(TIM3_InputTriggerSource));

  tmpsmcr = TIM3->SMCR;

  /* Select the Trigger Source */
  tmpsmcr &= (uint8_t)(~TIM_SMCR_TS);
  tmpsmcr |= (uint8_t)TIM3_InputTriggerSource;

  TIM3->SMCR = (uint8_t)tmpsmcr;
}

/**
  * @brief  Selects the TIM3 Trigger Output Mode.
  * @param  TIM3_TRGOSource: Specifies the Trigger Output source.
  *          This parameter can be one of the following values:
  *            @arg TIM3_TRGOSource_Reset: Trigger Output source = Reset 
  *            @arg TIM3_TRGOSource_Enable: Trigger Output source = TIM3 is enabled 
  *            @arg TIM3_TRGOSource_Update: Trigger Output source = Update event
  *            @arg TIM3_TRGOSource_OC1: Trigger Output source = output compare channel1
  *            @arg TIM3_TRGOSource_OC1REF: Trigger Output source = output compare channel 1 reference
  *            @arg TIM3_TRGOSource_OC2REF: Trigger Output source = output compare channel 2 reference   
  * @retval None
  */
void TIM3_SelectOutputTrigger(TIM3_TRGOSource_TypeDef TIM3_TRGOSource)
{
  uint8_t tmpcr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM3_TRGO_SOURCE(TIM3_TRGOSource));

  tmpcr2 = TIM3->CR2;

  /* Reset the MMS Bits */
  tmpcr2 &= (uint8_t)(~TIM_CR2_MMS);

  /* Select the TRGO source */
  tmpcr2 |=  (uint8_t)TIM3_TRGOSource;

  TIM3->CR2 = tmpcr2;
}

/**
  * @brief  Selects the TIM3 Slave Mode.
  * @param  TIM3_SlaveMode: Specifies the TIM3 Slave Mode.
  *          This parameter can be one of the following values:
  *            @arg TIM3_SlaveMode_Reset: Slave Mode Selection  = Reset
  *            @arg TIM3_SlaveMode_Gated: Slave Mode Selection  = Gated
  *            @arg TIM3_SlaveMode_Trigger: Slave Mode Selection  = Trigger
  *            @arg TIM3_SlaveMode_External1: Slave Mode Selection  = External 1  
  * @retval None
  */
void TIM3_SelectSlaveMode(TIM3_SlaveMode_TypeDef TIM3_SlaveMode)
{
  uint8_t tmpsmcr = 0;

  /* Check the parameters */
  assert_param(IS_TIM3_SLAVE_MODE(TIM3_SlaveMode));

  tmpsmcr = TIM3->SMCR;

  /* Reset the SMS Bits */
  tmpsmcr &= (uint8_t)(~TIM_SMCR_SMS);

  /* Select the Slave Mode */
  tmpsmcr |= (uint8_t)TIM3_SlaveMode;

  TIM3->SMCR = tmpsmcr;
}

/**
  * @brief  Sets or Resets the TIM3 Master/Slave Mode.
  * @param  NewState: The new state of the synchronization between TIM3 and its slaves (through TRGO).
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM3_SelectMasterSlaveMode(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the MSM Bit */
  if (NewState != DISABLE)
  {
    TIM3->SMCR |= TIM_SMCR_MSM;
  }
  else
  {
    TIM3->SMCR &= (uint8_t)(~TIM_SMCR_MSM);
  }
}

/**
  * @brief  Configures the TIM3 External Trigger.
  * @param  TIM3_ExtTRGPrescaler: Specifies the external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ExtTRGPSC_OFF: No External Trigger prescaler
  *            @arg TIM3_ExtTRGPSC_DIV2: External Trigger prescaler = 2 (ETRP frequency divided by 2)
  *            @arg TIM3_ExtTRGPSC_DIV4: External Trigger prescaler = 4 (ETRP frequency divided by 4)
  *            @arg TIM3_ExtTRGPSC_DIV8: External Trigger prescaler = 8 (ETRP frequency divided by 8)    
  * @param  TIM3_ExtTRGPolarity: Specifies the external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ExtTRGPolarity_Inverted: External Trigger Polarity = inverted
  *            @arg TIM3_ExtTRGPolarity_NonInverted: External Trigger Polarity = non inverted
  * @param  ExtTRGFilter: Specifies the External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM3_ETRConfig(TIM3_ExtTRGPSC_TypeDef TIM3_ExtTRGPrescaler,
                    TIM3_ExtTRGPolarity_TypeDef TIM3_ExtTRGPolarity,
                    uint8_t ExtTRGFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM3_EXT_PRESCALER(TIM3_ExtTRGPrescaler));
  assert_param(IS_TIM3_EXT_POLARITY(TIM3_ExtTRGPolarity));
  assert_param(IS_TIM3_EXT_FILTER(ExtTRGFilter));

  /* Set the Prescaler, the Filter value and the Polarity */
  TIM3->ETR |= (uint8_t)((uint8_t)((uint8_t)TIM3_ExtTRGPrescaler | (uint8_t)TIM3_ExtTRGPolarity)
                         | (uint8_t)ExtTRGFilter);
}

/**
  * @}
  */

/** @defgroup TIM3_Group7 Specific interface management functions
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
  * @brief  Configures the TIM3 Encoder Interface.
  * @param  TIM3_EncoderMode: Specifies the TIM3 Encoder Mode.
  *          This parameter can be one of the following values:
  *            @arg TIM3_EncoderMode_TI1: Encoder mode 1
  *            @arg TIM3_EncoderMode_TI2: Encoder mode 2
  *            @arg TIM3_EncoderMode_TI12: Encoder mode 3   
  * @param  TIM3_IC1Polarity: Specifies the IC1 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM3_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM3_IC2Polarity: Specifies the IC2 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM3_ICPolarity_Falling: Input Capture on Falling Edge  
  * @retval None
  */
void TIM3_EncoderInterfaceConfig(TIM3_EncoderMode_TypeDef TIM3_EncoderMode,
                                 TIM3_ICPolarity_TypeDef TIM3_IC1Polarity,
                                 TIM3_ICPolarity_TypeDef TIM3_IC2Polarity)
{
  uint8_t tmpsmcr = 0;
  uint8_t tmpccmr1 = 0;
  uint8_t tmpccmr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM3_ENCODER_MODE(TIM3_EncoderMode));
  assert_param(IS_TIM3_IC_POLARITY(TIM3_IC1Polarity));
  assert_param(IS_TIM3_IC_POLARITY(TIM3_IC2Polarity));

  tmpsmcr = TIM3->SMCR;
  tmpccmr1 = TIM3->CCMR1;
  tmpccmr2 = TIM3->CCMR2;

  /* Set the encoder Mode */
  tmpsmcr &= (uint8_t)(TIM_SMCR_MSM | TIM_SMCR_TS)  ;
  tmpsmcr |= (uint8_t)TIM3_EncoderMode;

  /* Select the Capture Compare 1 and the Capture Compare 2 as input */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_CCxS);
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_CCxS);
  tmpccmr1 |= TIM_CCMR_TIxDirect_Set;
  tmpccmr2 |= TIM_CCMR_TIxDirect_Set;

  /* Set the TI1 and the TI2 Polarities */
  if (TIM3_IC1Polarity == TIM3_ICPolarity_Falling)
  {
    TIM3->CCER1 |= TIM_CCER1_CC1P ;
  }
  else
  {
    TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P) ;
  }

  if (TIM3_IC2Polarity == TIM3_ICPolarity_Falling)
  {
    TIM3->CCER1 |= TIM_CCER1_CC2P ;
  }
  else
  {
    TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC2P) ;
  }

  TIM3->SMCR = tmpsmcr;
  TIM3->CCMR1 = tmpccmr1;
  TIM3->CCMR2 = tmpccmr2;
}

/**
  * @brief  Enables or Disables the TIM’s Hall sensor interface.
  * @param  NewState: The new state of the TIM3 Hall sensor interface.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM3_SelectHallSensor(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the TI1S Bit */
  if (NewState != DISABLE)
  {
    TIM3->CR2 |= TIM_CR2_TI1S;
  }
  else
  {
    TIM3->CR2 &= (uint8_t)(~TIM_CR2_TI1S);
  }
}

/**
  * @}
  */

/**
  * @brief  Configure the TI1 as Input.
  * @param  TIM3_ICPolarity: Input Capture Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM3_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM3_ICSelection: Specifies the input to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICSelection_DirectTI: Input Capture mapped on the direct input
  *            @arg TIM3_ICSelection_IndirectTI: Input Capture mapped on the indirect input
  *            @arg TIM3_ICSelection_TRGI: Input Capture mapped on the Trigger Input   
  * @param  TIM3_ICFilter: Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI1_Config(TIM3_ICPolarity_TypeDef TIM3_ICPolarity, \
                       TIM3_ICSelection_TypeDef TIM3_ICSelection, \
                       uint8_t TIM3_ICFilter)
{
  uint8_t tmpccmr1 = 0;
  uint8_t tmpicpolarity = TIM3_ICPolarity;
  tmpccmr1 = TIM3->CCMR1;

  /* Check the parameters */
  assert_param(IS_TIM3_IC_POLARITY(TIM3_ICPolarity));
  assert_param(IS_TIM3_IC_SELECTION(TIM3_ICSelection));
  assert_param(IS_TIM3_IC_FILTER(TIM3_ICFilter));

  /* Disable the Channel 1: Reset the CCE Bit */
  TIM3->CCER1 &=  (uint8_t)(~TIM_CCER1_CC1E);

  /* Select the Input and set the filter */
  tmpccmr1 &= (uint8_t)(~TIM_CCMR_CCxS) & (uint8_t)(~TIM_CCMR_ICxF);
  tmpccmr1 |= (uint8_t)(((uint8_t)(TIM3_ICSelection)) | ((uint8_t)(TIM3_ICFilter << 4)));

  TIM3->CCMR1 = tmpccmr1;

  /* Select the Polarity */
  if (tmpicpolarity == (uint8_t)(TIM3_ICPolarity_Falling))
  {
    TIM3->CCER1 |= TIM_CCER1_CC1P;
  }
  else
  {
    TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC1P);
  }

  /* Set the CCE Bit */
  TIM3->CCER1 |=  TIM_CCER1_CC1E;
}

/**
  * @brief  Configure the TI2 as Input.
  * @param  TIM3_ICPolarity: Input Capture Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICPolarity_Rising: Input Capture on Rising Edge
  *            @arg TIM3_ICPolarity_Falling: Input Capture on Falling Edge  
  * @param  TIM3_ICSelection: Specifies the input to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM3_ICSelection_DirectTI: Input Capture mapped on the direct input
  *            @arg TIM3_ICSelection_IndirectTI: Input Capture mapped on the indirect input
  *            @arg TIM3_ICSelection_TRGI: Input Capture mapped on the Trigger Input   
  * @param  TIM3_ICFilter: Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI2_Config(TIM3_ICPolarity_TypeDef TIM3_ICPolarity,
                       TIM3_ICSelection_TypeDef TIM3_ICSelection,
                       uint8_t TIM3_ICFilter)
{
  uint8_t tmpccmr2 = 0;
  uint8_t tmpicpolarity = TIM3_ICPolarity;

  /* Check the parameters */
  assert_param(IS_TIM3_IC_POLARITY(TIM3_ICPolarity));
  assert_param(IS_TIM3_IC_SELECTION(TIM3_ICSelection));
  assert_param(IS_TIM3_IC_FILTER(TIM3_ICFilter));

  tmpccmr2 = TIM3->CCMR2;

  /* Disable the Channel 2: Reset the CCE Bit */
  TIM3->CCER1 &=  (uint8_t)(~TIM_CCER1_CC2E);

  /* Select the Input and set the filter */
  tmpccmr2 &= (uint8_t)(~TIM_CCMR_CCxS) & (uint8_t)(~TIM_CCMR_ICxF);
  tmpccmr2 |= (uint8_t)(((uint8_t)(TIM3_ICSelection)) | ((uint8_t)(TIM3_ICFilter << 4)));

  TIM3->CCMR2 = tmpccmr2;

  /* Select the Polarity */
  if (tmpicpolarity == TIM3_ICPolarity_Falling)
  {
    TIM3->CCER1 |= TIM_CCER1_CC2P ;
  }
  else
  {
    TIM3->CCER1 &= (uint8_t)(~TIM_CCER1_CC2P) ;
  }

  /* Set the CCE Bit */
  TIM3->CCER1 |=  TIM_CCER1_CC2E;
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
