/**
  ******************************************************************************
  * @file    stm8l15x_tim1.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the TIM1 peripheral:
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
  *          This driver provides functions to configure and initialize the TIM1 
  *          peripheral
  *          These functions are split in 7 groups: 
  *   
  *          1. TIM1 TimeBase management: this group includes all needed functions 
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
  *          2. TIM1 Output Compare management: this group includes all needed 
  *             functions to configure the Capture/Compare unit used in Output 
  *             compare mode: 
  *                   - Configure each channel, independently, in Output Compare mode
  *                   - Select the output compare modes
  *                   - Select the Polarities of each channel
  *                   - Set/Get the Capture/Compare register values
  *                   - Select the Output Compare Fast mode 
  *                   - Select the Output Compare Forced mode  
  *                   - Output Compare-Preload Configuration 
  *                   - Clear Output Compare Reference
  *                   - Select the OCREF Clear signal
  *                   - Enable/Disable the Capture/Compare Channels    
  *                   
  *          3. TIM1 Input Capture management: this group includes all needed 
  *             functions to configure the Capture/Compare unit used in 
  *             Input Capture mode:
  *                   - Configure each channel in input capture mode
  *                   - Configure Channel1/2 in PWM Input mode
  *                   - Set the Input Capture Prescaler
  *                   - Get the Capture/Compare values      
  *        
  *          4. TIM1 interrupts, DMA and flags management
  *                   - Enable/Disable interrupt sources
  *                   - Get flags status
  *                   - Clear flags/ Pending bits
  *                   - Enable/Disable DMA requests 
  *                   - Configure DMA burst mode
  *                   - Select CaptureCompare DMA request  
  *              
  *          5. TIM1 clocks management: this group includes all needed functions 
  *             to configure the clock controller unit:
  *                   - Select internal/External clock
  *                   - Select the external clock mode: ETR(Mode1/Mode2) or TIx
  *         
  *          6. TIM1 synchronization management: this group includes all needed 
  *             functions to configure the Synchronization unit:
  *                   - Select Input Trigger  
  *                   - Select Output Trigger  
  *                   - Select Master Slave Mode 
  *                   - ETR Configuration when used as external trigger   
  *     
  *          7. TIM1 specific interface management, this group includes all 
  *             needed functions to use the specific TIM1 interface:
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
#include "stm8l15x_tim1.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup TIM1
  * @brief TIM1 driver modules
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup TIM1_Private_Functions_Prototypes
  * @{
  */

static void TI1_Config(uint8_t TIM1_ICPolarity, uint8_t TIM1_ICSelection,
                       uint8_t TIM1_ICFilter);
static void TI2_Config(uint8_t TIM1_ICPolarity, uint8_t TIM1_ICSelection,
                       uint8_t TIM1_ICFilter);
static void TI3_Config(uint8_t TIM1_ICPolarity, uint8_t TIM1_ICSelection,
                       uint8_t TIM1_ICFilter);
static void TI4_Config(uint8_t TIM1_ICPolarity, uint8_t TIM1_ICSelection,
                       uint8_t TIM1_ICFilter);

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/  
/** @defgroup TIM1_Private_Functions
  * @{
  */

/** @defgroup TIM1_Group1 TimeBase management functions
 *  @brief   TimeBase management functions 
 *
@verbatim   
 ===============================================================================
                       TimeBase management functions
 ===============================================================================  
  
       ===================================================================      
              TIM1 Driver: how to use it in Timing(Time base) Mode
       =================================================================== 
       To use the Timer in Timing(Time base) mode, the following steps are mandatory:
       
       1. Enable TIM1 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE) function.
        
       2. Call TIM1_TimeBaseInit() to configure the Time Base unit with the
          corresponding configuration.
          
       3. Enable global interrupts if you need to generate the update interrupt.
          
       4. Enable the corresponding interrupt using the function TIM1_ITConfig(TIM1_IT_Update) 
          
       5. Call the TIM1_Cmd(ENABLE) function to enable the TIM1 counter.
       
       Note1: All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the TIM1 peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void TIM1_DeInit(void)
{
  TIM1->CR1  = TIM1_CR1_RESET_VALUE;
  TIM1->CR2  = TIM1_CR2_RESET_VALUE;
  TIM1->SMCR = TIM1_SMCR_RESET_VALUE;
  TIM1->ETR  = TIM1_ETR_RESET_VALUE;
  TIM1->IER  = TIM1_IER_RESET_VALUE;

  /* Disable channels */
  TIM1->CCER1 = TIM1_CCER1_RESET_VALUE;
  TIM1->CCER2 = TIM1_CCER2_RESET_VALUE;
  /* Configure channels as inputs: it is necessary if lock level is equal to 2 or 3 */
  TIM1->CCMR1 = 0x01;
  TIM1->CCMR2 = 0x01;
  TIM1->CCMR3 = 0x01;
  TIM1->CCMR4 = 0x01;
  /* Then reset channel registers: it also works if lock level is equal to 2 or 3 */
  TIM1->CCER1 = TIM1_CCER1_RESET_VALUE;
  TIM1->CCER2 = TIM1_CCER2_RESET_VALUE;
  TIM1->CCMR1 = TIM1_CCMR1_RESET_VALUE;
  TIM1->CCMR2 = TIM1_CCMR2_RESET_VALUE;
  TIM1->CCMR3 = TIM1_CCMR3_RESET_VALUE;
  TIM1->CCMR4 = TIM1_CCMR4_RESET_VALUE;
  TIM1->CNTRH = TIM1_CNTRH_RESET_VALUE;
  TIM1->CNTRL = TIM1_CNTRL_RESET_VALUE;
  TIM1->PSCRH = TIM1_PSCRH_RESET_VALUE;
  TIM1->PSCRL = TIM1_PSCRL_RESET_VALUE;
  TIM1->ARRH  = TIM1_ARRH_RESET_VALUE;
  TIM1->ARRL  = TIM1_ARRL_RESET_VALUE;
  TIM1->CCR1H = TIM1_CCR1H_RESET_VALUE;
  TIM1->CCR1L = TIM1_CCR1L_RESET_VALUE;
  TIM1->CCR2H = TIM1_CCR2H_RESET_VALUE;
  TIM1->CCR2L = TIM1_CCR2L_RESET_VALUE;
  TIM1->CCR3H = TIM1_CCR3H_RESET_VALUE;
  TIM1->CCR3L = TIM1_CCR3L_RESET_VALUE;
  TIM1->CCR4H = TIM1_CCR4H_RESET_VALUE;
  TIM1->CCR4L = TIM1_CCR4L_RESET_VALUE;
  TIM1->OISR  = TIM1_OISR_RESET_VALUE;
  TIM1->EGR   = 0x01; /* TIM1_EGR_UG */
  TIM1->DTR   = TIM1_DTR_RESET_VALUE;
  TIM1->BKR   = TIM1_BKR_RESET_VALUE;
  TIM1->RCR   = TIM1_RCR_RESET_VALUE;
  TIM1->SR1   = TIM1_SR1_RESET_VALUE;
  TIM1->SR2   = TIM1_SR2_RESET_VALUE;
}

/**
  * @brief  Initializes the TIM1 Time Base Unit according to the specified parameters.
  * @param  TIM1_Prescaler specifies the Prescaler value.
  * @param  TIM1_CounterMode specifies the counter mode from
  *          This parameter can be one of the following values:
  *            @arg TIM1_CounterMode_Up: Counter mode Up
  *            @arg TIM1_CounterMode_Down: Counter mode Down
  *            @arg TIM1_CounterMode_CenterAligned1: Counter mode center aligned
  *            @arg TIM1_CounterMode_CenterAligned2: Counter mode center aligned
  *            @arg TIM1_CounterMode_CenterAligned3: Counter mode center aligned
  * @param  TIM1_Period specifies the Period value.
  * @param  TIM1_RepetitionCounter specifies the Repetition counter value
  * @retval None
  */
void TIM1_TimeBaseInit(uint16_t TIM1_Prescaler,
                       TIM1_CounterMode_TypeDef TIM1_CounterMode,
                       uint16_t TIM1_Period,
                       uint8_t TIM1_RepetitionCounter)
{
  /* Check parameters */
  assert_param(IS_TIM1_COUNTER_MODE(TIM1_CounterMode));


  /* Set the Autoreload value */
  TIM1->ARRH = (uint8_t)(TIM1_Period >> 8);
  TIM1->ARRL = (uint8_t)(TIM1_Period);

  /* Set the Prescaler value */
  TIM1->PSCRH = (uint8_t)(TIM1_Prescaler >> 8);
  TIM1->PSCRL = (uint8_t)(TIM1_Prescaler);

  /* Select the Counter Mode */
  TIM1->CR1 = (uint8_t)((uint8_t)(TIM1->CR1 & (uint8_t)(~(TIM1_CR1_CMS | TIM1_CR1_DIR)))
                        | (uint8_t)(TIM1_CounterMode));

  /* Set the Repetition Counter value */
  TIM1->RCR = TIM1_RepetitionCounter;

}

/**
  * @brief  Configures the TIM1 Prescaler.
  * @param  Prescaler specifies the Prescaler Register value
  *          This parameter must be a value between 0x0000 and 0xFFFF
  * @param  TIM1_PSCReloadMode specifies the TIM1 Prescaler Reload mode.
  *          This parameter can be one of the following values
  *            @arg TIM1_PSCReloadMode_Immediate: The Prescaler is loaded immediately.
  *            @arg TIM1_PSCReloadMode_Update: The Prescaler is loaded at the update
  *         event.
  * @retval None
  */

void TIM1_PrescalerConfig(uint16_t Prescaler, TIM1_PSCReloadMode_TypeDef TIM1_PSCReloadMode)
{
  /* Check the parameters */
  assert_param(IS_TIM1_PRESCALER_RELOAD(TIM1_PSCReloadMode));

  /* Set the Prescaler value */
  TIM1->PSCRH = (uint8_t)(Prescaler >> 8);
  TIM1->PSCRL = (uint8_t)(Prescaler);

  /* Set or reset the UG Bit */
  TIM1->EGR = (uint8_t)TIM1_PSCReloadMode;
}

/**
  * @brief  Specifies the TIM1 Counter Mode to be used.
  * @param  TIM1_CounterMode specifies the Counter Mode to be used
  *          This parameter can be one of the following values:
  *            @arg TIM1_CounterMode_Up: TIM1 Up Counting Mode
  *            @arg TIM1_CounterMode_Down: TIM1 Down Counting Mode
  *            @arg TIM1_CounterMode_CenterAligned1: TIM1 Center Aligned Mode1
  *            @arg TIM1_CounterMode_CenterAligned2: TIM1 Center Aligned Mode2
  *            @arg IM1_CounterMode_CenterAligned3: TIM1 Center Aligned Mode3
  * @retval None
  */
void TIM1_CounterModeConfig(TIM1_CounterMode_TypeDef TIM1_CounterMode)
{
  /* Check the parameters */
  assert_param(IS_TIM1_COUNTER_MODE(TIM1_CounterMode));


  /* Reset the CMS and DIR Bits & Set the Counter Mode */
  TIM1->CR1 = (uint8_t)((uint8_t)(TIM1->CR1 & (uint8_t)((uint8_t)(~TIM1_CR1_CMS) & (uint8_t)(~TIM1_CR1_DIR)))
                        | (uint8_t)TIM1_CounterMode);
}

/**
  * @brief  Sets the TIM1 Counter Register value.
  * @param  Counter specifies the Counter register new value.
  *          This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM1_SetCounter(uint16_t Counter)
{
  /* Set the Counter Register value */
  TIM1->CNTRH = (uint8_t)(Counter >> 8);
  TIM1->CNTRL = (uint8_t)(Counter);
}

/**
  * @brief  Sets the TIM1 Autoreload Register value.
  * @param  Autoreload specifies the Autoreload register new value.
  *          This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM1_SetAutoreload(uint16_t Autoreload)
{
  /* Set the Autoreload Register value */
  TIM1->ARRH = (uint8_t)(Autoreload >> 8);
  TIM1->ARRL = (uint8_t)(Autoreload);
}

/**
  * @brief  Gets the TIM1 Counter value.
  * @param  None
  * @retval Counter Register value.
  */
uint16_t TIM1_GetCounter(void)
{
  /* Get the Counter Register value */
  uint16_t tmpcntr = 0;
  uint8_t tmpcntrl = 0, tmpcntrh = 0;

  tmpcntrh = TIM1->CNTRH;
  tmpcntrl = TIM1->CNTRL;

  tmpcntr  = (uint16_t)(tmpcntrl);
  tmpcntr |= (uint16_t)((uint16_t)tmpcntrh << 8);

  /* Get the Counter Register value */
  return (uint16_t)tmpcntr;
}

/**
  * @brief  Gets the TIM1 Prescaler value.
  * @param  None
  * @retval Prescaler Register value.
  */
uint16_t TIM1_GetPrescaler(void)
{
  uint16_t tmpreg = 0;
  tmpreg = (uint16_t)((uint16_t)TIM1->PSCRH << 8);
  /* Get the Prescaler Register value */
  return (uint16_t)(tmpreg | TIM1->PSCRL);
}

/**
  * @brief  Enables or Disables the TIM1 Update event.
  * @param  NewState new state of the TIM1 peripheral Preload register.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_UpdateDisableConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the UDIS Bit */
  if (NewState != DISABLE)
  {
    TIM1->CR1 |= TIM1_CR1_UDIS;
  }
  else
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_UDIS);
  }
}

/**
  * @brief  Selects the TIM1 Update Request Interrupt source.
  * @param  TIM1_UpdateSource specifies the Update source.
  *          This parameter can be one of the following values
  *            @arg TIM1_UpdateSource_Regular
  *            @arg TIM1_UpdateSource_Global
  * @retval None
  */
void TIM1_UpdateRequestConfig(TIM1_UpdateSource_TypeDef TIM1_UpdateSource)
{
  /* Check the parameters */
  assert_param(IS_TIM1_UPDATE_SOURCE(TIM1_UpdateSource));

  /* Set or Reset the URS Bit */
  if (TIM1_UpdateSource != TIM1_UpdateSource_Global)
  {
    TIM1->CR1 |= TIM1_CR1_URS;
  }
  else
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_URS);
  }
}

/**
  * @brief  Enables or disables TIM1 peripheral Preload register on ARR.
  * @param  NewState new state of the TIM1 peripheral Preload register.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_ARRPreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the ARPE Bit */
  if (NewState != DISABLE)
  {
    TIM1->CR1 |= TIM1_CR1_ARPE;
  }
  else
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_ARPE);
  }
}

/**
  * @brief  Selects the TIM1’s One Pulse Mode.
  * @param  TIM1_OPMode specifies the OPM Mode to be used.
  *          This parameter can be one of the following values
  *            @arg TIM1_OPMode_Single
  *            @arg TIM1_OPMode_Repetitive
  * @retval None
  */
void TIM1_SelectOnePulseMode(TIM1_OPMode_TypeDef TIM1_OPMode)
{
  /* Check the parameters */
  assert_param(IS_TIM1_OPM_MODE(TIM1_OPMode));

  /* Set or Reset the OPM Bit */
  if (TIM1_OPMode != TIM1_OPMode_Repetitive)
  {
    TIM1->CR1 |= TIM1_CR1_OPM;
  }
  else
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_OPM);
  }
}

/**
  * @brief  Enables or disables the TIM1 peripheral.
  * @param  NewState new state of the TIM1 peripheral. 
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* set or Reset the CEN Bit */
  if (NewState != DISABLE)
  {
    TIM1->CR1 |= TIM1_CR1_CEN;
  }
  else
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_CEN);
  }
}

/**
  * @}
  */

/** @defgroup TIM1_Group2 Output Compare management functions
 *  @brief    Output Compare management functions 
 *
@verbatim   
 ===============================================================================
                        Output Compare management functions
 ===============================================================================  
   
       ===================================================================      
              TIM1 Driver: how to use it in Output Compare Mode
       =================================================================== 
       To use the Timer in Output Compare mode, the following steps are mandatory:
       
       1. Enable TIM1 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE) function.
       
       2. Configure the TIM1 pins in output mode by configuring the corresponding GPIO pins
          
       3. Configure the Time base unit as described in the first part of this driver, if needed,
          otherwise the Timer will run with the default configuration:
          - Autoreload value = 0xFFFF
          - Prescaler value = 0x0000
          - Counter mode = Up counting
      
       4. Call TIM1_OCxInit() to configure the channel x with the desired parameters
          including:
          - TIM1 Output Compare mode: TIM1_OCMode
          - TIM1 Output State: TIM1_OutputState
          - TIM1 Complementary Output State: TIM1_OutputNState
          - TIM1 Pulse value: TIM1_Pulse
          - TIM1 Output Compare Polarity : TIM1_OCPolarity
          - TIM1 Complementary Output Compare Polarity : TIM1_OCNPolarity
          - TIM1 Output Idle State: TIM1_OCIdleState
          - TIM1 Complementary Output Idle State: TIM1_OCNIdleState
       
       5. Call the TIM1_Cmd(ENABLE) function to enable the TIM1 counter.
       
       Note1: All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 
       
       Note2: If the corresponding interrupt or DMA request are needed, the user should:
              1. Enable global interrupts (or the DMA) to use the TIM1 interrupts (or DMA requests). 
              2. Enable the corresponding interrupt (or DMA request) using the function 
              TIM1_ITConfig(TIM1_IT_CCx) (or TIM1_DMACmd(TIM1_DMASource_CCx))   

@endverbatim
  * @{
  */
  
/**
  * @brief  Initializes the TIM1 Channel1 according to the specified parameters.
  * @param  TIM1_OCMode specifies the Output Compare mode
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCMode_Timing: OC Mode Timing
  *            @arg TIM1_OCMode_Active: OC Mode Active
  *            @arg TIM1_OCMode_Inactive: OC Mode Inactive
  *            @arg TIM1_OCMode_Toggle: OC Mode Toggle
  *            @arg TIM1_OCMode_PWM1: OC Mode PWM1
  *            @arg TIM1_OCMode_PWM2: OC Mode PWM2          
  * @param  TIM1_OutputState specifies the Output State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OutputState_Disable: Output state disable
  *            @arg TIM1_OutputState_Enable: Output state enable
  * @param  TIM1_OutputNState specifies the Complementary Output State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OutputNState_Disable: Output N state disable
  *            @arg TIM1_OutputNState_Enable: Output N state enable
  * @param  TIM1_Pulse specifies the Pulse width value.
  * @param  TIM1_OCPolarity specifies the Output Compare Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCPolarity_High: Output Compare active low
  *            @arg TIM1_OCPolarity_Low: Output Compare active high
  * @param  TIM1_OCNPolarity specifies the Complementary Output Compare Polarity 
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCNPolarity_High: Complementary Output Compare active low
  *            @arg TIM1_OCNPolarity_Low: Complementary Output Compare active high
  * @param  TIM1_OCIdleState specifies the Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCIdleState_Set: Output Compare Idle state set
  *            @arg TIM1_OCIdleState_Reset: Output Compare Idle state reset
  * @param  TIM1_OCNIdleState specifies the Complementary Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCNIdleState_Set: Complementary Output Compare Idle state set
  *            @arg TIM1_OCNIdleState_Reset: Complementary Output Compare Idle state reset
  * @retval None
  */
void TIM1_OC1Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  TIM1_OutputNState_TypeDef TIM1_OutputNState,
                  uint16_t TIM1_Pulse,
                  TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState,
                  TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState)
{
  /* Check the parameters */
  assert_param(IS_TIM1_OC_MODE(TIM1_OCMode));
  assert_param(IS_TIM1_OUTPUT_STATE(TIM1_OutputState));
  assert_param(IS_TIM1_OUTPUTN_STATE(TIM1_OutputNState));
  assert_param(IS_TIM1_OC_POLARITY(TIM1_OCPolarity));
  assert_param(IS_TIM1_OCN_POLARITY(TIM1_OCNPolarity));
  assert_param(IS_TIM1_OCIDLE_STATE(TIM1_OCIdleState));
  assert_param(IS_TIM1_OCNIDLE_STATE(TIM1_OCNIdleState));

  /* Disable the Channel 1: Reset the CCE Bit, Set the Output State ,
  the Output N State, the Output Polarity & the Output N Polarity*/
  TIM1->CCER1 &= (uint8_t)(~(TIM1_CCER1_CC1E | TIM1_CCER1_CC1NE
                             | TIM1_CCER1_CC1P | TIM1_CCER1_CC1NP));
  /* Set the Output State & Set the Output N State & Set the Output Polarity
  & Set the Output N Polarity */
  TIM1->CCER1 |= (uint8_t)((uint8_t)((uint8_t)(TIM1_OutputState & TIM1_CCER1_CC1E)
                                     | (uint8_t)(TIM1_OutputNState & TIM1_CCER1_CC1NE))
                           | (uint8_t)( (uint8_t)(TIM1_OCPolarity  & TIM1_CCER1_CC1P)
                                        | (uint8_t)(TIM1_OCNPolarity & TIM1_CCER1_CC1NP)));

  /* Reset the Output Compare Bits & Set the Output Compare Mode */
  TIM1->CCMR1 = (uint8_t)((uint8_t)(TIM1->CCMR1 & (uint8_t)(~TIM1_CCMR_OCM))
                          | (uint8_t)TIM1_OCMode);

  /* Reset the Output Idle state & the Output N Idle state bits */
  TIM1->OISR &= (uint8_t)(~(TIM1_OISR_OIS1 | TIM1_OISR_OIS1N));
  /* Set the Output Idle state & the Output N Idle state configuration */
  TIM1->OISR |= (uint8_t)((uint8_t)(TIM1_OCIdleState & TIM1_OISR_OIS1)
                          | (uint8_t)(TIM1_OCNIdleState & TIM1_OISR_OIS1N));

  /* Set the Pulse value */
  TIM1->CCR1H = (uint8_t)(TIM1_Pulse >> 8);
  TIM1->CCR1L = (uint8_t)(TIM1_Pulse);
}

/**
  * @brief  Initializes the TIM1 Channel2 according to the specified parameters.
  * @param  TIM1_OCMode specifies the Output Compare mode
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCMode_Timing: OC Mode Timing
  *            @arg TIM1_OCMode_Active: OC Mode Active
  *            @arg TIM1_OCMode_Inactive: OC Mode Inactive
  *            @arg TIM1_OCMode_Toggle: OC Mode Toggle
  *            @arg TIM1_OCMode_PWM1: OC Mode PWM1
  *            @arg TIM1_OCMode_PWM2: OC Mode PWM2  
  * @param  TIM1_OutputState specifies the Output State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OutputState_Disable: Output state disable
  *            @arg TIM1_OutputState_Enable: Output state enable
  * @param  TIM1_OutputNState specifies the Complementary Output State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OutputNState_Disable: Output N state disable
  *            @arg TIM1_OutputNState_Enable: Output N state enable
  * @param  TIM1_Pulse specifies the Pulse width value.
  * @param  TIM1_OCPolarity specifies the Output Compare Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCPolarity_High: Output Compare active low
  *            @arg TIM1_OCPolarity_Low: Output Compare active high
  * @param  TIM1_OCNPolarity specifies the Complementary Output Compare
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCNPolarity_High: Complementary Output Compare active low
  *            @arg TIM1_OCNPolarity_Low: Complementary Output Compare active high
  * @param  TIM1_OCIdleState specifies the Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCIdleState_Set: Output Compare Idle state set
  *            @arg TIM1_OCIdleState_Reset: Output Compare Idle state reset
  * @param  TIM1_OCNIdleState specifies the Complementary Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCNIdleState_Set: Complementary Output Compare Idle state set
  *            @arg TIM1_OCNIdleState_Reset: Complementary Output Compare Idle state reset
  * @retval None
  */
void TIM1_OC2Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  TIM1_OutputNState_TypeDef TIM1_OutputNState,
                  uint16_t TIM1_Pulse,
                  TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState,
                  TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState)
{

  /* Check the parameters */
  assert_param(IS_TIM1_OC_MODE(TIM1_OCMode));
  assert_param(IS_TIM1_OUTPUT_STATE(TIM1_OutputState));
  assert_param(IS_TIM1_OUTPUTN_STATE(TIM1_OutputNState));
  assert_param(IS_TIM1_OC_POLARITY(TIM1_OCPolarity));
  assert_param(IS_TIM1_OCN_POLARITY(TIM1_OCNPolarity));
  assert_param(IS_TIM1_OCIDLE_STATE(TIM1_OCIdleState));
  assert_param(IS_TIM1_OCNIDLE_STATE(TIM1_OCNIdleState));

  /* Disable the Channel 1: Reset the CCE Bit, Set the Output State,
     the Output N State, the Output Polarity & the Output N Polarity*/
  TIM1->CCER1 &= (uint8_t)(~(TIM1_CCER1_CC2E | TIM1_CCER1_CC2NE | TIM1_CCER1_CC2P | TIM1_CCER1_CC2NP));

  /* Set the Output State & Set the Output N State & Set the Output Polarity & Set the Output N Polarity */
  TIM1->CCER1 |= (uint8_t)((uint8_t)((uint8_t)(TIM1_OutputState & TIM1_CCER1_CC2E) | (uint8_t)(TIM1_OutputNState & TIM1_CCER1_CC2NE))
                           | (uint8_t) ((uint8_t)(TIM1_OCPolarity & TIM1_CCER1_CC2P) | (uint8_t)(TIM1_OCNPolarity & TIM1_CCER1_CC2NP)));

  /* Reset the Output Compare Bits & Set the Output Compare Mode */
  TIM1->CCMR2 = (uint8_t)((uint8_t)(TIM1->CCMR2 & (uint8_t)(~TIM1_CCMR_OCM)) | (uint8_t)TIM1_OCMode);

  /* Reset the Output Idle state & the Output N Idle state bits */
  TIM1->OISR &= (uint8_t)(~(TIM1_OISR_OIS2 | TIM1_OISR_OIS2N));
  /* Set the Output Idle state & the Output N Idle state configuration */
  TIM1->OISR |= (uint8_t)((uint8_t)(TIM1_OISR_OIS2 & TIM1_OCIdleState) | (uint8_t)(TIM1_OISR_OIS2N & TIM1_OCNIdleState));

  /* Set the Pulse value */
  TIM1->CCR2H = (uint8_t)(TIM1_Pulse >> 8);
  TIM1->CCR2L = (uint8_t)(TIM1_Pulse);
}

/**
  * @brief  Initializes the TIM1 Channel3 according to the specified parameters.
  * @param  TIM1_OCMode specifies the Output Compare mode
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCMode_Timing: OC Mode Timing
  *            @arg TIM1_OCMode_Active: OC Mode Active
  *            @arg TIM1_OCMode_Inactive: OC Mode Inactive
  *            @arg TIM1_OCMode_Toggle: OC Mode Toggle
  *            @arg TIM1_OCMode_PWM1: OC Mode PWM1
  *            @arg TIM1_OCMode_PWM2: OC Mode PWM2     
  * @param  TIM1_OutputState specifies the Output State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OutputState_Disable: Output state disable
  *            @arg TIM1_OutputState_Enable: Output state enable
  * @param  TIM1_OutputNState specifies the Complementary Output State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OutputNState_Disable: Output N state disable
  *            @arg TIM1_OutputNState_Enable: Output N state enable
  * @param  TIM1_Pulse specifies the Pulse width value.
  * @param  TIM1_OCPolarity specifies the Output Compare Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCPolarity_High: Output Compare active low
  *            @arg TIM1_OCPolarity_Low: Output Compare active high
  * @param  TIM1_OCNPolarity specifies the Complementary Output Compare Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCNPolarity_High: Complementary Output Compare active low
  *            @arg TIM1_OCNPolarity_Low: Complementary Output Compare active high
  * @param  TIM1_OCIdleState specifies the Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCIdleState_Set: Output Compare Idle state set
  *            @arg TIM1_OCIdleState_Reset: Output Compare Idle state reset
  * @param  TIM1_OCNIdleState specifies the Complementary Output Compare Idle State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCNIdleState_Set: Complementary Output Compare Idle state set
  *            @arg TIM1_OCNIdleState_Reset: Complementary Output Compare Idle state reset
  * @retval None
  */
void TIM1_OC3Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  TIM1_OutputNState_TypeDef TIM1_OutputNState,
                  uint16_t TIM1_Pulse,
                  TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState,
                  TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState)
{

  /* Check the parameters */
  assert_param(IS_TIM1_OC_MODE(TIM1_OCMode));
  assert_param(IS_TIM1_OUTPUT_STATE(TIM1_OutputState));
  assert_param(IS_TIM1_OUTPUTN_STATE(TIM1_OutputNState));
  assert_param(IS_TIM1_OC_POLARITY(TIM1_OCPolarity));
  assert_param(IS_TIM1_OCN_POLARITY(TIM1_OCNPolarity));
  assert_param(IS_TIM1_OCIDLE_STATE(TIM1_OCIdleState));
  assert_param(IS_TIM1_OCNIDLE_STATE(TIM1_OCNIdleState));

  /* Disable the Channel 1: Reset the CCE Bit, Set the Output State,
     the Output N State, the Output Polarity & the Output N Polarity */
  TIM1->CCER2 &= (uint8_t)(~( TIM1_CCER2_CC3E | TIM1_CCER2_CC3NE | TIM1_CCER2_CC3P | TIM1_CCER2_CC3NP));

  /* Set the Output State & Set the Output N State & Set the Output Polarity & Set the Output N Polarity */
  TIM1->CCER2 |= (uint8_t)((uint8_t)((uint8_t)(TIM1_OutputState & TIM1_CCER2_CC3E) | (uint8_t)(TIM1_OutputNState & TIM1_CCER2_CC3NE))
                           | (uint8_t)((uint8_t)(TIM1_OCPolarity & TIM1_CCER2_CC3P) | (uint8_t)(TIM1_OCNPolarity & TIM1_CCER2_CC3NP)));

  /* Reset the Output Compare Bits & Set the Output Compare Mode */
  TIM1->CCMR3 = (uint8_t)((uint8_t)(TIM1->CCMR3 & (uint8_t)(~TIM1_CCMR_OCM)) | (uint8_t)TIM1_OCMode);

  /* Reset the Output Idle state & the Output N Idle state bits */
  TIM1->OISR &= (uint8_t)(~(TIM1_OISR_OIS3 | TIM1_OISR_OIS3N));
  /* Set the Output Idle state & the Output N Idle state configuration */
  TIM1->OISR |= (uint8_t)((uint8_t)(TIM1_OISR_OIS3 & TIM1_OCIdleState) | (uint8_t) (TIM1_OISR_OIS3N & TIM1_OCNIdleState));

  /* Set the Pulse value */
  TIM1->CCR3H = (uint8_t)(TIM1_Pulse >> 8);
  TIM1->CCR3L = (uint8_t)(TIM1_Pulse);
}

/**
  * @brief  Configures the Break feature, dead time, Lock level, the OSSI,
  *         and the AOE(automatic output enable).
  * @param  TIM1_OSSIState specifies the OSSIS State
  *          This parameter can be one of the following values:
  *            @arg TIM1_OSSIState_Enable: OSSIS State enabled
  *            @arg TIM1_OSSIState_Disable: OSSIS State disabled   
  * @param  TIM1_LockLevel specifies the lock level
  *          This parameter can be one of the following values:
  *            @arg TIM1_LockLevel_Off: No lock level
  *            @arg TIM1_LockLevel_1: Lock level 1  
  *            @arg TIM1_LockLevel_2: Lock level 2
  *            @arg TIM1_LockLevel_3: Lock level 3
  * @param  TIM1_DeadTime specifies the dead time value.
  * @param  TIM1_Break specifies the Break state
  *          This parameter can be one of the following values:
  *            @arg TIM1_BreakState_Enable: Break enabled
  *            @arg TIM1_BreakState_Disable: Break disabled  
  * @param  TIM1_BreakPolarity specifies the Break polarity
  *          This parameter can be one of the following values:
  *            @arg TIM1_BreakPolarity_Low: Break polarity high
  *            @arg TIM1_BreakPolarity_High: Break polarity low
  * @param  TIM1_AutomaticOutput specifies the Automatic Output configuration
  *          This parameter can be one of the following values:
  *            @arg TIM1_AutomaticOutput_Enable: Automatic Output enabled
  *            @arg TIM1_AutomaticOutput_Disable: Automatic Output disabled  
  * @retval None
  */
void TIM1_BDTRConfig(TIM1_OSSIState_TypeDef TIM1_OSSIState,
                     TIM1_LockLevel_TypeDef TIM1_LockLevel,
                     uint8_t TIM1_DeadTime,
                     TIM1_BreakState_TypeDef TIM1_Break,
                     TIM1_BreakPolarity_TypeDef TIM1_BreakPolarity,
                     TIM1_AutomaticOutput_TypeDef TIM1_AutomaticOutput)
{

  /* Check the parameters */
  assert_param(IS_TIM1_OSSI_STATE(TIM1_OSSIState));
  assert_param(IS_TIM1_LOCK_LEVEL(TIM1_LockLevel));
  assert_param(IS_TIM1_BREAK_STATE(TIM1_Break));
  assert_param(IS_TIM1_BREAK_POLARITY(TIM1_BreakPolarity));
  assert_param(IS_TIM1_AUTOMATIC_OUTPUT_STATE(TIM1_AutomaticOutput));

  TIM1->DTR = (uint8_t)(TIM1_DeadTime);
  /* Set the Lock level, the Break enable Bit and the Polarity, the OSSI State,
     the dead time value  and the Automatic Output Enable Bit */

  TIM1->BKR  =  (uint8_t)((uint8_t)((uint8_t)TIM1_OSSIState | (uint8_t)TIM1_LockLevel)
                          | (uint8_t)((uint8_t)((uint8_t)TIM1_Break | (uint8_t)TIM1_BreakPolarity)
                                      | (uint8_t)TIM1_AutomaticOutput));

}

/**
  * @brief  Enables or disables the TIM1 peripheral Main Outputs.
  * @param  NewState new state of the TIM1 peripheral. 
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_CtrlPWMOutputs(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the MOE Bit */

  if (NewState != DISABLE)
  {
    TIM1->BKR |= TIM1_BKR_MOE;
  }
  else
  {
    TIM1->BKR &= (uint8_t)(~TIM1_BKR_MOE);
  }
}

/**
  * @brief  Selects the TIM1 Output Compare Mode. This function disables the
  *         selected channel before changing the Output Compare Mode. 
  * @note   User has to enable this channel using TIM1_CCxCmd and TIM1_CCxNCmd functions.
  * @param  TIM1_Channel specifies the TIM1 Channel.
  *          This parameter can be one of the following values:
  *            @arg TIM1_Channel_1: TIM1 Channel1
  *            @arg TIM1_Channel_2: TIM1 Channel2
  *            @arg TIM1_Channel_3: TIM1 Channel3
  *            @arg TIM1_Channel_4: TIM1 Channel4
  * @param  TIM1_OCMode specifies the TIM1 Output Compare Mode.
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCMode_Timing: OC Mode Timing
  *            @arg TIM1_OCMode_Active: OC Mode Active
  *            @arg TIM1_OCMode_Inactive: OC Mode Inactive
  *            @arg TIM1_OCMode_Toggle: OC Mode Toggle
  *            @arg TIM1_OCMode_PWM1: OC Mode PWM1
  *            @arg TIM1_OCMode_PWM2: OC Mode PWM2  
  * @retval None
  */
void TIM1_SelectOCxM(TIM1_Channel_TypeDef TIM1_Channel, TIM1_OCMode_TypeDef TIM1_OCMode)
{
  /* Check the parameters */
  assert_param(IS_TIM1_CHANNEL(TIM1_Channel));
  assert_param(IS_TIM1_OCM(TIM1_OCMode));

  if (TIM1_Channel == TIM1_Channel_1)
  {
    /* Disable the Channel 1: Reset the CCE Bit */
    TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC1E);

    /* Reset the Output Compare Bits & Set the Output Compare Mode */
    TIM1->CCMR1 = (uint8_t)((uint8_t)(TIM1->CCMR1 & (uint8_t)(~TIM1_CCMR_OCM)) | (uint8_t)TIM1_OCMode);
  }
  else if (TIM1_Channel == TIM1_Channel_2)
  {
    /* Disable the Channel 2: Reset the CCE Bit */
    TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC2E);

    /* Reset the Output Compare Bits & Set the Output Compare Mode */
    TIM1->CCMR2 = (uint8_t)((uint8_t)(TIM1->CCMR2 & (uint8_t)(~TIM1_CCMR_OCM)) | (uint8_t)TIM1_OCMode);
  }
  else
  {
    /* Disable the Channel 3: Reset the CCE Bit */
    TIM1->CCER2 &= (uint8_t)(~TIM1_CCER2_CC3E);

    /* Reset the Output Compare Bits & Set the Output Compare Mode */
    TIM1->CCMR3 = (uint8_t)((uint8_t)(TIM1->CCMR3 & (uint8_t)(~TIM1_CCMR_OCM)) | (uint8_t)TIM1_OCMode);

  }
}

/**
  * @brief  Sets the TIM1 Capture Compare1 Register value.
  * @param  Compare1 specifies the Capture Compare1 register new value.
  *          This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM1_SetCompare1(uint16_t Compare1)
{
  /* Set the Capture Compare1 Register value */
  TIM1->CCR1H = (uint8_t)(Compare1 >> 8);
  TIM1->CCR1L = (uint8_t)(Compare1);

}

/**
  * @brief  Sets the TIM1 Capture Compare2 Register value.
  * @param  Compare2 specifies the Capture Compare2 register new value.
  *          This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM1_SetCompare2(uint16_t Compare2)
{
  /* Set the Capture Compare2 Register value */
  TIM1->CCR2H = (uint8_t)(Compare2 >> 8);
  TIM1->CCR2L = (uint8_t)(Compare2);
}

/**
  * @brief  Sets the TIM1 Capture Compare3 Register value.
  * @param  Compare3 specifies the Capture Compare3 register new value.
  *          This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM1_SetCompare3(uint16_t Compare3)
{
  /* Set the Capture Compare3 Register value */
  TIM1->CCR3H = (uint8_t)(Compare3 >> 8);
  TIM1->CCR3L = (uint8_t)(Compare3);
}

/**
  * @brief  Sets the TIM1 Capture Compare4 Register value.
  * @param  Compare4 specifies the Capture Compare4 register new value.
  *          This parameter is between 0x0000 and 0xFFFF.
  * @retval None
  */
void TIM1_SetCompare4(uint16_t Compare4)
{
  /* Set the Capture Compare4 Register value */
  TIM1->CCR4H = (uint8_t)(Compare4 >> 8);
  TIM1->CCR4L = (uint8_t)(Compare4);
}

/**
  * @brief  Sets or Resets the TIM1 peripheral Capture Compare Preload Control bit.
  * @param  NewState new state of the Capture Compare Preload Control bit.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_CCPreloadControl(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the CCPC Bit */
  if (NewState != DISABLE)
  {
    TIM1->CR2 |= TIM1_CR2_CCPC;
  }
  else
  {
    TIM1->CR2 &= (uint8_t)(~TIM1_CR2_CCPC);
  }
}

/**
  * @brief  Forces the TIM1 Channel1 output waveform to active or inactive level.
  * @param  TIM1_ForcedAction specifies the forced Action to be set to the output waveform.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ForcedAction_Active: Force active level on OC1REF
  *            @arg TIM1_ForcedAction_Inactive: Force inactive level on OC1REF.
  * @retval None
  */
void TIM1_ForcedOC1Config(TIM1_ForcedAction_TypeDef TIM1_ForcedAction)
{
  /* Check the parameters */
  assert_param(IS_TIM1_FORCED_ACTION(TIM1_ForcedAction));

  /* Reset the OCM Bits & Configure the Forced output Mode */
  TIM1->CCMR1 =  (uint8_t)((uint8_t)(TIM1->CCMR1 & (uint8_t)(~TIM1_CCMR_OCM)) | (uint8_t)TIM1_ForcedAction);
}

/**
  * @brief  Forces the TIM1 Channel2 output waveform to active or inactive level.
  * @param  TIM1_ForcedAction specifies the forced Action to be set to the output waveform.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ForcedAction_Active: Force active level on OC2REF
  *            @arg TIM1_ForcedAction_Inactive: Force inactive level on OC2REF.
  * @retval None
  */
void TIM1_ForcedOC2Config(TIM1_ForcedAction_TypeDef TIM1_ForcedAction)
{
  /* Check the parameters */
  assert_param(IS_TIM1_FORCED_ACTION(TIM1_ForcedAction));

  /* Reset the OCM Bits & Configure the Forced output Mode */
  TIM1->CCMR2  =  (uint8_t)((uint8_t)(TIM1->CCMR2 & (uint8_t)(~TIM1_CCMR_OCM)) | (uint8_t)TIM1_ForcedAction);
}

/**
  * @brief  Forces the TIM1 Channel3 output waveform to active or inactive level.
  * @param  TIM1_ForcedAction specifies the forced Action to be set to the output waveform.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ForcedAction_Active: Force active level on OC3REF
  *            @arg TIM1_ForcedAction_Inactive: Force inactive level on OC3REF.
  * @retval None
  */
void TIM1_ForcedOC3Config(TIM1_ForcedAction_TypeDef TIM1_ForcedAction)
{
  /* Check the parameters */
  assert_param(IS_TIM1_FORCED_ACTION(TIM1_ForcedAction));

  /* Reset the OCM Bits */ /* Configure The Forced output Mode */
  TIM1->CCMR3 = (uint8_t)((uint8_t)(TIM1->CCMR3 & (uint8_t)(~TIM1_CCMR_OCM)) | (uint8_t)TIM1_ForcedAction);
}

/**
  * @brief  Enables or disables the TIM1 peripheral Preload Register on CCR1.
  * @param  NewState new state of the Capture Compare Preload register.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_OC1PreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC1PE Bit */
  if (NewState != DISABLE)
  {
    TIM1->CCMR1 |= TIM1_CCMR_OCxPE;
  }
  else
  {
    TIM1->CCMR1 &= (uint8_t)(~TIM1_CCMR_OCxPE);
  }
}

/**
  * @brief  Enables or disables the TIM1 peripheral Preload Register on CCR2.
  * @param  NewState new state of the Capture Compare Preload register.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_OC2PreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC2PE Bit */
  if (NewState != DISABLE)
  {
    TIM1->CCMR2 |= TIM1_CCMR_OCxPE;
  }
  else
  {
    TIM1->CCMR2 &= (uint8_t)(~TIM1_CCMR_OCxPE);
  }
}

/**
  * @brief  Enables or disables the TIM1 peripheral Preload Register on CCR3.
  * @param  NewState new state of the Capture Compare Preload register.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_OC3PreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC3PE Bit */
  if (NewState != DISABLE)
  {
    TIM1->CCMR3 |= TIM1_CCMR_OCxPE;
  }
  else
  {
    TIM1->CCMR3 &= (uint8_t)(~TIM1_CCMR_OCxPE);
  }
}

/**
  * @brief  Enables or disables the TIM1 peripheral Preload Register on CCR4.
  * @param  NewState new state of the Capture Compare Preload register.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_OC4PreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC4PE Bit */
  if (NewState != DISABLE)
  {
    TIM1->CCMR4 |= TIM1_CCMR_OCxPE;
  }
  else
  {
    TIM1->CCMR4 &= (uint8_t)(~TIM1_CCMR_OCxPE);
  }
}

/**
  * @brief  Configures the TIM1 Capture Compare 1 Fast feature.
  * @param  NewState new state of the Output Compare Fast Enable bit.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_OC1FastConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC1FE Bit */
  if (NewState != DISABLE)
  {
    TIM1->CCMR1 |= TIM1_CCMR_OCxFE;
  }
  else
  {
    TIM1->CCMR1 &= (uint8_t)(~TIM1_CCMR_OCxFE);
  }
}

/**
  * @brief  Configures the TIM1 Capture Compare 2 Fast feature.
  * @param  NewState new state of the Output Compare Fast Enable bit.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_OC2FastConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC2FE Bit */
  if (NewState != DISABLE)
  {
    TIM1->CCMR2 |= TIM1_CCMR_OCxFE;
  }
  else
  {
    TIM1->CCMR2 &= (uint8_t)(~TIM1_CCMR_OCxFE);
  }
}

/**
  * @brief  Configures the TIM1 Capture Compare 3 Fast feature.
  * @param  NewState new state of the Output Compare Fast Enable bit.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_OC3FastConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC3FE Bit */
  if (NewState != DISABLE)
  {
    TIM1->CCMR3 |= TIM1_CCMR_OCxFE;
  }
  else
  {
    TIM1->CCMR3 &= (uint8_t)(~TIM1_CCMR_OCxFE);
  }
}

/**
  * @brief   Clears or safeguards the OC1REF signal.
  * @param  NewState new state of the Output Compare 1 Clear Enable bit.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_ClearOC1Ref(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC1CE Bit */
  if (NewState != DISABLE)
  {
    TIM1->CCMR1 |= TIM1_CCMR_OCxCE;
  }
  else
  {
    TIM1->CCMR1 &= (uint8_t)(~TIM1_CCMR_OCxCE);
  }
}

/**
  * @brief  Clears or safeguards the OC2REF signal.
  * @param  NewState new state of the Output Compare 2 Clear Enable bit.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_ClearOC2Ref(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC2CE Bit */
  if (NewState != DISABLE)
  {
    TIM1->CCMR2 |= TIM1_CCMR_OCxCE;
  }
  else
  {
    TIM1->CCMR2 &= (uint8_t)(~TIM1_CCMR_OCxCE);
  }
}

/**
  * @brief  Clears or safeguards the OC3REF signal.
  * @param  NewState new state of the Output Compare 3 Clear Enable bit.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_ClearOC3Ref(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC3CE Bit */
  if (NewState != DISABLE)
  {
    TIM1->CCMR3 |= TIM1_CCMR_OCxCE;
  }
  else
  {
    TIM1->CCMR3 &= (uint8_t)(~TIM1_CCMR_OCxCE);
  }
}

/**
  * @brief  Clears or safeguards the OC4REF signal.
  * @param  NewState new state of the Output Compare 4 Clear Enable bit.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_ClearOC4Ref(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the OC4CE Bit */
  if (NewState != DISABLE)
  {
    TIM1->CCMR4 |= TIM1_CCMR_OCxCE;
  }
  else
  {
    TIM1->CCMR4 &= (uint8_t)(~TIM1_CCMR_OCxCE);
  }
}

/**
  * @brief  Configures the TIM1 Channel 1 polarity.
  * @param  TIM1_OCPolarity specifies the OC1 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCPolarity_High: Output Compare active low
  *            @arg TIM1_OCPolarity_Low: Output Compare active high
  * @retval None
  */
void TIM1_OC1PolarityConfig(TIM1_OCPolarity_TypeDef TIM1_OCPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM1_OC_POLARITY(TIM1_OCPolarity));

  /* Set or Reset the CC1P Bit */
  if (TIM1_OCPolarity != TIM1_OCPolarity_High)
  {
    TIM1->CCER1 |= TIM1_CCER1_CC1P;
  }
  else
  {
    TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC1P);
  }
}

/**
  * @brief  Configures the TIM1 Channel 1N polarity.
  * @param  TIM1_OCNPolarity specifies the OC1N Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCNPolarity_High: Complementary Output Compare active low
  *            @arg TIM1_OCNPolarity_Low: Complementary Output Compare active high
  * @retval None
  */
void TIM1_OC1NPolarityConfig(TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM1_OCN_POLARITY(TIM1_OCNPolarity));

  /* Set or Reset the CC3P Bit */
  if (TIM1_OCNPolarity != TIM1_OCNPolarity_High)
  {
    TIM1->CCER1 |= TIM1_CCER1_CC1NP;
  }
  else
  {
    TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC1NP);
  }
}

/**
  * @brief  Configures the TIM1 Channel 2 polarity.
  * @param  TIM1_OCPolarity specifies the OC2 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCPolarity_High: Output Compare active low
  *            @arg TIM1_OCPolarity_Low: Output Compare active high
  * @retval None
  */
void TIM1_OC2PolarityConfig(TIM1_OCPolarity_TypeDef TIM1_OCPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM1_OC_POLARITY(TIM1_OCPolarity));

  /* Set or Reset the CC2P Bit */
  if (TIM1_OCPolarity != TIM1_OCPolarity_High)
  {
    TIM1->CCER1 |= TIM1_CCER1_CC2P;
  }
  else
  {
    TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC2P);
  }
}

/**
  * @brief  Configures the TIM1 Channel 2N polarity.
  * @param  TIM1_OCNPolarity specifies the OC2N Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCNPolarity_High: Complementary Output Compare active low
  *            @arg TIM1_OCNPolarity_Low: Complementary Output Compare active high
  * @retval None
  */
void TIM1_OC2NPolarityConfig(TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM1_OCN_POLARITY(TIM1_OCNPolarity));

  /* Set or Reset the CC3P Bit */
  if (TIM1_OCNPolarity != TIM1_OCNPolarity_High)
  {
    TIM1->CCER1 |= TIM1_CCER1_CC2NP;
  }
  else
  {
    TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC2NP);
  }
}

/**
  * @brief  Configures the TIM1 Channel 3 polarity.
  * @param  TIM1_OCPolarity specifies the OC3 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCPolarity_High: Output Compare active low
  *            @arg TIM1_OCPolarity_Low: Output Compare active high
  * @retval None
  */
void TIM1_OC3PolarityConfig(TIM1_OCPolarity_TypeDef TIM1_OCPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM1_OC_POLARITY(TIM1_OCPolarity));

  /* Set or Reset the CC3P Bit */
  if (TIM1_OCPolarity != TIM1_OCPolarity_High)
  {
    TIM1->CCER2 |= TIM1_CCER2_CC3P;
  }
  else
  {
    TIM1->CCER2 &= (uint8_t)(~TIM1_CCER2_CC3P);
  }
}

/**
  * @brief  Configures the TIM1 Channel 3N polarity.
  * @param  TIM1_OCNPolarity specifies the OC3N Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCNPolarity_High: Complementary Output Compare active low
  *            @arg TIM1_OCNPolarity_Low: Complementary Output Compare active high
  * @retval None
  */
void TIM1_OC3NPolarityConfig(TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM1_OCN_POLARITY(TIM1_OCNPolarity));

  /* Set or Reset the CC3P Bit */
  if (TIM1_OCNPolarity != TIM1_OCNPolarity_High)
  {
    TIM1->CCER2 |= TIM1_CCER2_CC3NP;
  }
  else
  {
    TIM1->CCER2 &= (uint8_t)(~TIM1_CCER2_CC3NP);
  }
}

/**
  * @brief  Selects the OCReference Clear source.
  * @param  TIM1_OCReferenceClear: specifies the OCReference Clear source.
  *          This parameter can be one of the following values:
  *            @arg TIM1_OCReferenceClear_ETRF: OCReference Clear source ETR
  *            @arg TIM1_OCReferenceClear_OCREFCLR: OCReference Clear source OCREF 
  * @retval None
  */
void TIM1_SelectOCREFClear(TIM1_OCReferenceClear_TypeDef TIM1_OCReferenceClear)
{
  /* Check the parameters */
  assert_param(IS_TIM1_OCREFERENCECECLEAR_SOURCE(TIM1_OCReferenceClear));

  /* Set the TIM1_OCReferenceClear source */
  TIM1->SMCR &=  (uint8_t) (~TIM1_SMCR_OCCS);
  TIM1->SMCR |=  (uint8_t) TIM1_OCReferenceClear;
}

/**
  * @brief  Selects the TIM1 peripheral Commutation event.
  * @param  NewState new state of the Commutation event.
  *          This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_SelectCOM(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the CCUS Bit */
  if (NewState != DISABLE)
  {
    TIM1->CR2 |= TIM1_CR2_CCUS;
  }
  else
  {
    TIM1->CR2 &= (uint8_t)(~TIM1_CR2_CCUS);
  }
}

/**
  * @brief  Enables or disables the TIM1 Capture Compare Channel x (x=1,..,4).
  * @param  TIM1_Channel specifies the TIM1 Channel.
  *          This parameter can be one of the following values:
  *            @arg TIM1_Channel_1: TIM1 Channel1
  *            @arg TIM1_Channel_2: TIM1 Channel2
  *            @arg TIM1_Channel_3: TIM1 Channel3
  *            @arg TIM1_CHANNEL_4: TIM1 Channel4
  * @param  NewState specifies the TIM1 Channel CCxE bit new state.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM1_CCxCmd(TIM1_Channel_TypeDef TIM1_Channel, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM1_CHANNEL(TIM1_Channel));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (TIM1_Channel == TIM1_Channel_1)
  {
    /* Set or Reset the CC1E Bit */
    if (NewState != DISABLE)
    {
      TIM1->CCER1 |= TIM1_CCER1_CC1E;
    }
    else
    {
      TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC1E);
    }
  }
  else if (TIM1_Channel == TIM1_Channel_2)
  {
    /* Set or Reset the CC2E Bit */
    if (NewState != DISABLE)
    {
      TIM1->CCER1 |= TIM1_CCER1_CC2E;
    }
    else
    {
      TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC2E);
    }
  }
  else if (TIM1_Channel == TIM1_Channel_3)
  {
    /* Set or Reset the CC3E Bit */
    if (NewState != DISABLE)
    {
      TIM1->CCER2 |= TIM1_CCER2_CC3E;
    }
    else
    {
      TIM1->CCER2 &= (uint8_t)(~TIM1_CCER2_CC3E);
    }
  }
  else
  {
    /* Set or Reset the CC4E Bit */
    if (NewState != DISABLE)
    {
      TIM1->CCER2 |= TIM1_CCER2_CC4E;
    }
    else
    {
      TIM1->CCER2 &= (uint8_t)(~TIM1_CCER2_CC4E);
    }
  }
}

/**
  * @brief  Enables or disables the TIM1 Capture Compare Channel xN (xN=1,..,3).
  * @param  TIM1_Channel specifies the TIM1 Channel.
  *          This parameter can be one of the following values:
  *            @arg TIM1_Channel_1: TIM1 Channel1
  *            @arg TIM1_Channel_2: TIM1 Channel2
  *            @arg TIM1_Channel_3: TIM1 Channel3
  * @param  NewState specifies the TIM1 Channel CCxNE bit new state.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM1_CCxNCmd(TIM1_Channel_TypeDef TIM1_Channel, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM1_COMPLEMENTARY_CHANNEL(TIM1_Channel));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (TIM1_Channel == TIM1_Channel_1)
  {
    /* Set or Reset the CC1NE Bit */
    if (NewState != DISABLE)
    {
      TIM1->CCER1 |= TIM1_CCER1_CC1NE;
    }
    else
    {
      TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC1NE);
    }
  }
  else if (TIM1_Channel == TIM1_Channel_2)
  {
    /* Set or Reset the CC2NE Bit */
    if (NewState != DISABLE)
    {
      TIM1->CCER1 |= TIM1_CCER1_CC2NE;
    }
    else
    {
      TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC2NE);
    }
  }
  else
  {
    /* Set or Reset the CC3NE Bit */
    if (NewState != DISABLE)
    {
      TIM1->CCER2 |= TIM1_CCER2_CC3NE;
    }
    else
    {
      TIM1->CCER2 &= (uint8_t)(~TIM1_CCER2_CC3NE);
    }
  }
}

/**
  * @}
  */

/** @defgroup TIM1_Group3 Input Capture management functions
 *  @brief    Input Capture management functions 
 *
@verbatim   
 ===============================================================================
                      Input Capture management functions
 ===============================================================================  
   
       ===================================================================      
              TIM1 Driver: how to use it in Input Capture Mode
       =================================================================== 
       To use the Timer in Input Capture mode, the following steps are mandatory:
       
       1. Enable TIM1 clock using CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE) function.
       
       2. Configure the TIM1 pins in input mode by configuring the corresponding GPIO pins
       
       3. Configure the Time base unit as described in the first part of this driver, if needed,
          otherwise the Timer will run with the default configuration:
          - Autoreload value = 0xFFFF
          - Prescaler value = 0x0000
          - Counter mode = Up counting
       
       4. Call TIM1_ICInit() to configure the desired channel to measure only 
          frequency or duty cycle of the input signal using the corresponding configuration: 
          - TIM1 Channel: TIM1_Channel
          - TIM1 Input Capture polarity: TIM1_ICPolarity
          - TIM1 Input Capture selection: TIM1_ICSelection
          - TIM1 Input Capture Prescaler: TIM1_ICPSC
          - TIM1 Input Capture filter value
          or,
          Call TIM1_PWMIConfig() to configure the desired channels with the 
          corresponding configuration and to measure the frequency and the duty
          cycle of the input signal.
          
       5. Enable global interrupts or the DMA to read the measured frequency. 
          
       6. Enable the corresponding interrupt (or DMA request) to read the captured value,
          using the function TIM1_ITConfig(TIM1_IT_CCx) (or TIM1_DMACmd(TIM1_DMASource_CCx))
       
       7. Call the TIM1_Cmd(ENABLE) function to enable the TIM1 counter.
       
       8. Use TIM1_GetCapturex() to read the captured value corresponding to
          channel x.
       
       Note1: All other functions can be used separately to modify, if needed,
          a specific feature of the Timer. 

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the TIM1 peripheral according to the specified parameters.
  * @param  TIM1_Channel specifies the input capture channel
  *          This parameter can be one of the following values:
  *            @arg TIM1_Channel_1: TIM1 Channel1
  *            @arg TIM1_Channel_2: TIM1 Channel2
  *            @arg TIM1_Channel_3: TIM1 Channel3
  *            @arg TIM1_Channel_4: TIM1 Channel4
  * @param  TIM1_ICPolarity specifies the Input capture polarity
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPolarity_Rising: Input capture polarity rising
  *            @arg TIM1_ICPolarity_Falling: Input capture polarity falling
  * @param  TIM1_ICSelection specifies the Input capture source selection
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICSelection_DirectTI: TIM1 Input x is selected to be connected to ICx.
  *            @arg TIM1_ICSelection_IndirectTI: TIM1 Input x is selected to be connected to ICy.
  *            @arg TIM1_ICSelection_TRGI: TIM1 Input x is selected to be connected to the TRGI   
  * @param  TIM1_ICPrescaler specifies the Input capture Prescaler
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM1_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM1_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM1_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events)   
  * @param  TIM1_ICFilter specifies the Input capture filter value.
  * @note   If the channel 3 is selected the TIM1_ICSelection_IndirectTI parameter is forbidden.
  * @note   If the channel 4 is selected the TIM1_ICSelection_DirectTI parameter is forbidden.
  * @retval None
  */

void TIM1_ICInit(TIM1_Channel_TypeDef TIM1_Channel,
                 TIM1_ICPolarity_TypeDef TIM1_ICPolarity,
                 TIM1_ICSelection_TypeDef TIM1_ICSelection,
                 TIM1_ICPSC_TypeDef TIM1_ICPrescaler,
                 uint8_t TIM1_ICFilter)
{

  /* Check the parameters */
  assert_param(IS_TIM1_CHANNEL(TIM1_Channel));
  assert_param(IS_TIM1_IC_POLARITY(TIM1_ICPolarity));
  assert_param(IS_TIM1_IC_SELECTION(TIM1_ICSelection));
  assert_param(IS_TIM1_IC_PRESCALER(TIM1_ICPrescaler));
  assert_param(IS_TIM1_IC_FILTER(TIM1_ICFilter));

  if (TIM1_Channel == TIM1_Channel_1)
  {
    /* TI1 Configuration */
    TI1_Config(TIM1_ICPolarity, TIM1_ICSelection, TIM1_ICFilter);
    /* Set the Input Capture Prescaler value */
    TIM1_SetIC1Prescaler(TIM1_ICPrescaler);
  }
  else if (TIM1_Channel == TIM1_Channel_2)
  {
    /* TI2 Configuration */
    TI2_Config(TIM1_ICPolarity, TIM1_ICSelection, TIM1_ICFilter);
    /* Set the Input Capture Prescaler value */
    TIM1_SetIC2Prescaler(TIM1_ICPrescaler);
  }
  else if (TIM1_Channel == TIM1_Channel_3)
  {
    /* TI3 Configuration */
    TI3_Config(TIM1_ICPolarity, TIM1_ICSelection, TIM1_ICFilter);
    /* Set the Input Capture Prescaler value */
    TIM1_SetIC3Prescaler(TIM1_ICPrescaler);
  }
  else
  {
    /* TI4 Configuration */
    TI4_Config(TIM1_ICPolarity, TIM1_ICSelection, TIM1_ICFilter);
    /* Set the Input Capture Prescaler value */
    TIM1_SetIC4Prescaler(TIM1_ICPrescaler);
  }
}

/**
  * @brief  Configures the TIM1 peripheral in PWM Input Mode according to the
  *         specified parameters.
  * @param  TIM1_Channel specifies the input capture channel
  *          This parameter can be one of the following values:
  *            @arg TIM1_Channel_1: TIM1 Channel1
  *            @arg TIM1_Channel_2: TIM1 Channel2
  * @param  TIM1_ICPolarity specifies the Input capture polarity
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPolarity_Rising: Input capture polarity rising
  *            @arg TIM1_ICPolarity_Falling: Input capture polarity falling
  * @param  TIM1_ICSelection specifies the Input capture source selection 
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICSelection_DirectTI: TIM1 Input x is selected to be connected to ICx.
  *            @arg TIM1_ICSelection_IndirectTI: TIM1 Input x is selected to be connected to ICy.
  *            @arg TIM1_ICSelection_TRGI: TIM1 Input x is selected to be connected to the TRGI     
  * @param  TIM1_ICPrescaler specifies the Input capture Prescaler
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM1_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM1_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM1_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events)   
  * @param  TIM1_ICFilter specifies the Input capture filter value.
  * @retval None
  */
void TIM1_PWMIConfig(TIM1_Channel_TypeDef TIM1_Channel,
                     TIM1_ICPolarity_TypeDef TIM1_ICPolarity,
                     TIM1_ICSelection_TypeDef TIM1_ICSelection,
                     TIM1_ICPSC_TypeDef TIM1_ICPrescaler,
                     uint8_t TIM1_ICFilter)
{
  TIM1_ICPolarity_TypeDef icpolarity = TIM1_ICPolarity_Rising;
  TIM1_ICSelection_TypeDef icselection = TIM1_ICSelection_DirectTI;

  /* Check the parameters */
  assert_param(IS_TIM1_PWMI_CHANNEL(TIM1_Channel));
  assert_param(IS_TIM1_IC_POLARITY(TIM1_ICPolarity));
  assert_param(IS_TIM1_IC_SELECTION(TIM1_ICSelection));
  assert_param(IS_TIM1_IC_PRESCALER(TIM1_ICPrescaler));

  /* Select the Opposite Input Polarity */
  if (TIM1_ICPolarity != TIM1_ICPolarity_Falling)
  {
    icpolarity = TIM1_ICPolarity_Falling;
  }
  else
  {
    icpolarity = TIM1_ICPolarity_Rising;
  }

  /* Select the Opposite Input */
  if (TIM1_ICSelection == TIM1_ICSelection_DirectTI)
  {
    icselection = TIM1_ICSelection_IndirectTI;
  }
  else
  {
    icselection = TIM1_ICSelection_DirectTI;
  }

  if (TIM1_Channel == TIM1_Channel_1)
  {
    /* TI1 Configuration */
    TI1_Config(TIM1_ICPolarity, TIM1_ICSelection, TIM1_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM1_SetIC1Prescaler(TIM1_ICPrescaler);

    /* TI2 Configuration */
    TI2_Config(icpolarity, icselection, TIM1_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM1_SetIC2Prescaler(TIM1_ICPrescaler);
  }
  else
  {
    /* TI2 Configuration */
    TI2_Config(TIM1_ICPolarity, TIM1_ICSelection, TIM1_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM1_SetIC2Prescaler(TIM1_ICPrescaler);

    /* TI1 Configuration */
    TI1_Config(icpolarity, icselection, TIM1_ICFilter);

    /* Set the Input Capture Prescaler value */
    TIM1_SetIC1Prescaler(TIM1_ICPrescaler);
  }
}

/**
  * @brief  Gets the TIM1 Input Capture 1 value.
  * @param  None
  * @retval Capture Compare 1 Register value.
  */
uint16_t TIM1_GetCapture1(void)
{
  /* Get the Capture 1 Register value */

  uint16_t tmpccr1 = 0;
  uint8_t tmpccr1l = 0, tmpccr1h = 0;

  tmpccr1h = TIM1->CCR1H;
  tmpccr1l = TIM1->CCR1L;

  tmpccr1 = (uint16_t)(tmpccr1l);
  tmpccr1 |= (uint16_t)((uint16_t)tmpccr1h << 8);
  /* Get the Capture 1 Register value */
  return (uint16_t)tmpccr1;
}

/**
  * @brief  Gets the TIM1 Input Capture 2 value.
  * @param  None
  * @retval Capture Compare 2 Register value.
  */
uint16_t TIM1_GetCapture2(void)
{
  /* Get the Capture 2 Register value */

  uint16_t tmpccr2 = 0;
  uint8_t tmpccr2l = 0, tmpccr2h = 0;

  tmpccr2h = TIM1->CCR2H;
  tmpccr2l = TIM1->CCR2L;

  tmpccr2 = (uint16_t)(tmpccr2l);
  tmpccr2 |= (uint16_t)((uint16_t)tmpccr2h << 8);
  /* Get the Capture 2 Register value */
  return (uint16_t)tmpccr2;
}

/**
  * @brief  Gets the TIM1 Input Capture 3 value.
  * @param  None
  * @retval Capture Compare 3 Register value.
  */
uint16_t TIM1_GetCapture3(void)
{
  /* Get the Capture 3 Register value */
  uint16_t tmpccr3 = 0;
  uint8_t tmpccr3l = 0, tmpccr3h = 0;

  tmpccr3h = TIM1->CCR3H;
  tmpccr3l = TIM1->CCR3L;

  tmpccr3 = (uint16_t)(tmpccr3l);
  tmpccr3 |= (uint16_t)((uint16_t)tmpccr3h << 8);
  /* Get the Capture 3 Register value */
  return (uint16_t)tmpccr3;
}

/**
  * @brief  Gets the TIM1 Input Capture 4 value.
  * @param  None
  * @retval Capture Compare 4 Register value.
  */
uint16_t TIM1_GetCapture4(void)
{
  /* Get the Capture 4 Register value */
  uint16_t tmpccr4 = 0;
  uint8_t tmpccr4l = 0, tmpccr4h = 0;

  tmpccr4h = TIM1->CCR4H;
  tmpccr4l = TIM1->CCR4L;

  tmpccr4 = (uint16_t)(tmpccr4l);
  tmpccr4 |= (uint16_t)((uint16_t)tmpccr4h << 8);
  /* Get the Capture 4 Register value */
  return (uint16_t)tmpccr4;
}

/**
  * @brief  Sets the TIM1 Input Capture 1 prescaler.
  * @param  TIM1_IC1Prescaler specifies the Input Capture prescaler new value
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM1_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM1_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM1_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events)   
  * @retval None
  */
void TIM1_SetIC1Prescaler(TIM1_ICPSC_TypeDef TIM1_IC1Prescaler)
{
  /* Check the parameters */
  assert_param(IS_TIM1_IC_PRESCALER(TIM1_IC1Prescaler));

  /* Reset the IC1PSC Bits */ /* Set the IC1PSC value */
  TIM1->CCMR1 = (uint8_t)((uint8_t)(TIM1->CCMR1 & (uint8_t)(~TIM1_CCMR_ICxPSC)) | (uint8_t)TIM1_IC1Prescaler);
}

/**
  * @brief  Sets the TIM1 Input Capture 2 prescaler.
  * @param  TIM1_IC2Prescaler specifies the Input Capture prescaler new value
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM1_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM1_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM1_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events)   
  * @retval None
  */
void TIM1_SetIC2Prescaler(TIM1_ICPSC_TypeDef TIM1_IC2Prescaler)
{
  /* Check the parameters */
  assert_param(IS_TIM1_IC_PRESCALER(TIM1_IC2Prescaler));

  /* Reset the IC1PSC Bits */ /* Set the IC1PSC value */
  TIM1->CCMR2 = (uint8_t)((uint8_t)(TIM1->CCMR2 & (uint8_t)(~TIM1_CCMR_ICxPSC)) | (uint8_t)TIM1_IC2Prescaler);
}

/**
  * @brief  Sets the TIM1 Input Capture 3 prescaler.
  * @param  TIM1_IC3Prescaler specifies the Input Capture prescaler new value
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM1_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM1_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM1_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events)   
  * @retval None
  */
void TIM1_SetIC3Prescaler(TIM1_ICPSC_TypeDef TIM1_IC3Prescaler)
{

  /* Check the parameters */
  assert_param(IS_TIM1_IC_PRESCALER(TIM1_IC3Prescaler));

  /* Reset the IC1PSC Bits & Set the IC1PSC value */
  TIM1->CCMR3 = (uint8_t)((uint8_t)(TIM1->CCMR3 & (uint8_t)(~TIM1_CCMR_ICxPSC)) | (uint8_t)TIM1_IC3Prescaler);
}

/**
  * @brief  Sets the TIM1 Input Capture 4 prescaler.
  * @param  TIM1_IC4Prescaler specifies the Input Capture prescaler new value
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPSC_DIV1: Input Capture Prescaler = 1 (one capture every 1 event)
  *            @arg TIM1_ICPSC_DIV2: Input Capture Prescaler = 2 (one capture every 2 events)
  *            @arg TIM1_ICPSC_DIV4: Input Capture Prescaler = 4 (one capture every 4 events)
  *            @arg TIM1_ICPSC_DIV8: Input Capture Prescaler = 8 (one capture every 8 events)   
  * @retval None
  */
void TIM1_SetIC4Prescaler(TIM1_ICPSC_TypeDef TIM1_IC4Prescaler)
{

  /* Check the parameters */
  assert_param(IS_TIM1_IC_PRESCALER(TIM1_IC4Prescaler));

  /* Reset the IC1PSC Bits &  Set the IC1PSC value */
  TIM1->CCMR4 = (uint8_t)((uint8_t)(TIM1->CCMR4 & (uint8_t)(~TIM1_CCMR_ICxPSC)) | (uint8_t)TIM1_IC4Prescaler);
}

/**
  * @}
  */

/** @defgroup TIM1_Group4 Interrupts DMA and flags management functions
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
  * @brief  Enables or disables the specified TIM1 interrupts.
  * @param  NewState new state of the TIM1 peripheral.
  *          This parameter can be: ENABLE or DISABLE.
  * @param  TIM1_IT specifies the TIM1 interrupts sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg TIM1_IT_Update: Update
  *            @arg TIM1_IT_CC1: Capture Compare Channel1
  *            @arg TIM1_IT_CC2: Capture Compare Channel2 
  *            @arg TIM1_IT_CC3: Capture Compare Channel3
  *            @arg TIM1_IT_CC4: Capture Compare Channel4 
  *            @arg TIM1_IT_COM: Commutation
  *            @arg TIM1_IT_Trigger: Trigger 
  *            @arg TIM1_IT_Break: Break  
  * @param  NewState new state of the TIM1 peripheral.
  * @retval None
  */
void TIM1_ITConfig(TIM1_IT_TypeDef  TIM1_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM1_IT(TIM1_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the Interrupt sources */
    TIM1->IER |= (uint8_t)TIM1_IT;
  }
  else
  {
    /* Disable the Interrupt sources */
    TIM1->IER &= (uint8_t)(~(uint8_t)TIM1_IT);
  }
}

/**
  * @brief  Configures the TIM1 event to be generated by software.
  * @param  TIM1_EventSource specifies the event source.
  *          This parameter can be any combination of the following values:
  *            @arg TIM1_EventSource_Update: Update
  *            @arg TIM1_EventSource_CC1: Capture Compare Channel1
  *            @arg TIM1_EventSource_CC2: Capture Compare Channel2 
  *            @arg TIM1_EventSource_CC3: Capture Compare Channel3
  *            @arg TIM1_EventSource_CC4: Capture Compare Channel4 
  *            @arg TIM1_EventSource_COM: Commutation
  *            @arg TIM1_EventSource_Trigger: Trigger 
  *            @arg TIM1_EventSource_Break: Break  
  * @retval None
  */
void TIM1_GenerateEvent(TIM1_EventSource_TypeDef TIM1_EventSource)
{
  /* Check the parameters */
  assert_param(IS_TIM1_EVENT_SOURCE(TIM1_EventSource));

  /* Set the event sources */
  TIM1->EGR = (uint8_t)TIM1_EventSource;
}
/**
  * @brief  Checks whether the specified TIM1 flag is set or not.
  * @param  TIM1_FLAG specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg TIM1_FLAG_Update: TIM1 update Flag
  *            @arg TIM1_FLAG_CC1: TIM1 Capture Compare 1 Flag
  *            @arg TIM1_FLAG_CC2: TIM1 Capture Compare 2 Flag
  *            @arg TIM1_FLAG_CC3: TIM1 Capture Compare 3 Flag
  *            @arg TIM1_FLAG_CC4: TIM1 Capture Compare 4 Flag
  *            @arg TIM1_FLAG_COM: TIM1 Commutation Flag
  *            @arg TIM1_FLAG_Trigger: TIM1 Trigger Flag
  *            @arg TIM1_FLAG_Break: TIM1 Break Flag
  *            @arg TIM1_FLAG_CC1OF: TIM1 Capture Compare 1 overcapture Flag
  *            @arg TIM1_FLAG_CC2OF: TIM1 Capture Compare 2 overcapture Flag
  *            @arg TIM1_FLAG_CC3OF: TIM1 Capture Compare 3 overcapture Flag
  *            @arg TIM1_FLAG_CC4OF: TIM1 Capture Compare 4 overcapture Flag
  * @retval FlagStatus The new state of TIM1_FLAG (SET or RESET).
  */
FlagStatus TIM1_GetFlagStatus(TIM1_FLAG_TypeDef TIM1_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint8_t tim1_flag_l = 0, tim1_flag_h = 0;

  /* Check the parameters */
  assert_param(IS_TIM1_GET_FLAG(TIM1_FLAG));

  tim1_flag_l = (uint8_t)(TIM1->SR1 & (uint8_t)TIM1_FLAG);
  tim1_flag_h = (uint8_t)(TIM1->SR2 & (uint8_t)((uint16_t)TIM1_FLAG >> 8));

  if ((uint8_t)((uint8_t)tim1_flag_l | (uint8_t)tim1_flag_h) != 0)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return (FlagStatus)(bitstatus);
}

/**
  * @brief  Clears the TIM1’s pending flags.
  * @param  TIM1_FLAG specifies the flag to clear.
  *          This parameter can be one of the following values:
  *            @arg TIM1_FLAG_Update: TIM1 update Flag
  *            @arg TIM1_FLAG_CC1: TIM1 Capture Compare 1 Flag
  *            @arg TIM1_FLAG_CC2: TIM1 Capture Compare 2 Flag
  *            @arg TIM1_FLAG_CC3: TIM1 Capture Compare 3 Flag
  *            @arg TIM1_FLAG_CC4: TIM1 Capture Compare 4 Flag
  *            @arg TIM1_FLAG_COM: TIM1 Commutation Flag
  *            @arg TIM1_FLAG_Trigger: TIM1 Trigger Flag
  *            @arg TIM1_FLAG_Break: TIM1 Break Flag
  *            @arg TIM1_FLAG_CC1OF: TIM1 Capture Compare 1 overcapture Flag
  *            @arg TIM1_FLAG_CC2OF: TIM1 Capture Compare 2 overcapture Flag
  *            @arg TIM1_FLAG_CC3OF: TIM1 Capture Compare 3 overcapture Flag
  *            @arg TIM1_FLAG_CC4OF: TIM1 Capture Compare 4 overcapture Flag
  * @retval None.
  */
void TIM1_ClearFlag(TIM1_FLAG_TypeDef TIM1_FLAG)
{
  /* Check the parameters */
  assert_param(IS_TIM1_CLEAR_FLAG(TIM1_FLAG));

  /* Clear the flags (rc_w0) clear this bit by writing 0. Writing ‘1’ has no effect*/
  TIM1->SR1 = (uint8_t)(~(uint8_t)(TIM1_FLAG));
  TIM1->SR2 = (uint8_t)((uint8_t)(~((uint8_t)((uint16_t)TIM1_FLAG >> 8))) & (uint8_t)0x1E);
}

/**
  * @brief  Checks whether the TIM1 interrupt has occurred or not.
  * @param  TIM1_IT specifies the TIM1 interrupt source to check.
  *          This parameter can be any combination of the following values:
  *            @arg TIM1_IT_Update: Update
  *            @arg TIM1_IT_CC1: Capture Compare Channel1
  *            @arg TIM1_IT_CC2: Capture Compare Channel2 
  *            @arg TIM1_IT_CC3: Capture Compare Channel3
  *            @arg TIM1_IT_CC4: Capture Compare Channel4 
  *            @arg TIM1_IT_COM: Commutation
  *            @arg TIM1_IT_Trigger: Trigger 
  *            @arg TIM1_IT_Break: Break  
  * @retval ITStatus The new state of the TIM1_IT(SET or RESET).
  */

ITStatus TIM1_GetITStatus(TIM1_IT_TypeDef TIM1_IT)
{
  ITStatus bitstatus = RESET;

  uint8_t TIM1_itStatus = 0x0, TIM1_itEnable = 0x0;

  /* Check the parameters */
  assert_param(IS_TIM1_GET_IT(TIM1_IT));

  TIM1_itStatus = (uint8_t)(TIM1->SR1 & (uint8_t)TIM1_IT);

  TIM1_itEnable = (uint8_t)(TIM1->IER & (uint8_t)TIM1_IT);

  if ((TIM1_itStatus != (uint8_t)RESET ) && (TIM1_itEnable != (uint8_t)RESET ))
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return (ITStatus)(bitstatus);
}

/**
  * @brief  Clears the TIM1's interrupt pending bits.
  * @param  TIM1_IT specifies the pending bit to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM1_IT_Update: Update
  *            @arg TIM1_IT_CC1: Capture Compare Channel1
  *            @arg TIM1_IT_CC2: Capture Compare Channel2 
  *            @arg TIM1_IT_CC3: Capture Compare Channel3
  *            @arg TIM1_IT_CC4: Capture Compare Channel4 
  *            @arg TIM1_IT_COM: Commutation
  *            @arg TIM1_IT_Trigger: Trigger 
  *            @arg TIM1_IT_Break: Break  
  * @retval None.
  */
void TIM1_ClearITPendingBit(TIM1_IT_TypeDef TIM1_IT)
{
  /* Check the parameters */
  assert_param(IS_TIM1_IT(TIM1_IT));

  /* Clear the IT pending Bit */
  TIM1->SR1 = (uint8_t)(~(uint8_t)TIM1_IT);
}

/**
  * @brief  Configures the TIM1’s DMA interface.
  * @param  TIM1_DMABase: DMA Base address.
  * @param  TIM1_DMABurstLength: DMA Burst length.
  * @retval None.
  */
void TIM1_DMAConfig(TIM1_DMABase_TypeDef TIM1_DMABase,
                    TIM1_DMABurstLength_TypeDef TIM1_DMABurstLength)
{
  /* Check the parameters */
  assert_param(IS_TIM1_DMABase(TIM1_DMABase));
  assert_param(IS_TIM1_DMABurstLength(TIM1_DMABurstLength));

  /* Set the DMA Base and the DMA Burst Length */
  TIM1->DCR1 = (uint8_t)TIM1_DMABase;
  TIM1->DCR2 = (uint8_t)TIM1_DMABurstLength;
}

/**
  * @brief  Enables or disables the TIM’s DMA Requests.
  * @param  TIM1_DMASources: specifies the DMA Request sources.
  *          This parameter can be any combination of the following values:
  *            @arg TIM1_DMA_Update: TIM1 update Interrupt source
  *            @arg DMA_Trigger: TIM1 Trigger DMA source
  * @param  NewState  new state of the DMA Request sources.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void TIM1_DMACmd(TIM1_DMASource_TypeDef TIM1_DMASource, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  assert_param(IS_TIM1_DMA_SOURCE(TIM1_DMASource));

  if (NewState != DISABLE)
  {
    /* Enable the DMA sources */
    TIM1->DER |= (uint8_t)TIM1_DMASource;
  }
  else
  {
    /* Disable the DMA sources */
    TIM1->DER &= (uint8_t)(~TIM1_DMASource);
  }
}

/**
  * @brief  Selects the TIM1 peripheral Capture Compare DMA source.
  * @param   NewState: new state of the Capture Compare DMA source.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM1_SelectCCDMA(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Set the CCDS Bit */
    TIM1->CR2 |= TIM1_CR2_CCDS;
  }
  else
  {
    /* Reset the CCDS Bit */
    TIM1->CR2 &= (uint8_t)(~TIM1_CR2_CCDS);
  }
}

/**
  * @}
  */

/** @defgroup TIM1_Group5 Clocks management functions
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
  * @brief  Configures the TIM1 internal Clock.
  * @param  None
  * @retval None
  */
void TIM1_InternalClockConfig(void)
{
  /* Disable slave mode to clock the prescaler directly with the internal clock */
  TIM1->SMCR &= (uint8_t)(~TIM1_SMCR_SMS);
}

/**
  * @brief  Configures the TIM1 Trigger as External Clock.
  * @param  TIM1_TIxExternalCLKSource specifies Trigger source.
  *          This parameter can be one of the following values:
  *            @arg TIM1_TIxExternalCLK1Source_TI1: TI1 Edge Detector
  *            @arg TIM1_TIxExternalCLK1Source_TI2: Filtered TIM1 Input 1
  *            @arg TIM1_TIxExternalCLK1Source_TI1ED: Filtered TIM1 Input 2
  * @param  TIM1_ICPolarity specifies the TIx Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPolarity_Rising: Input capture polarity rising
  *            @arg TIM1_ICPolarity_Falling: Input capture polarity falling
  * @param  ICFilter specifies the filter value.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM1_TIxExternalClockConfig(TIM1_TIxExternalCLK1Source_TypeDef TIM1_TIxExternalCLKSource,
                                 TIM1_ICPolarity_TypeDef TIM1_ICPolarity,
                                 uint8_t ICFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM1_TIXCLK_SOURCE(TIM1_TIxExternalCLKSource));
  assert_param(IS_TIM1_IC_POLARITY(TIM1_ICPolarity));
  assert_param(IS_TIM1_IC_FILTER(ICFilter));

  /* Configure the TIM1 Input Clock Source */
  if (TIM1_TIxExternalCLKSource == TIM1_TIxExternalCLK1Source_TI2)
  {
    TI2_Config(TIM1_ICPolarity, TIM1_ICSelection_DirectTI, ICFilter);
  }
  else
  {
    TI1_Config(TIM1_ICPolarity, TIM1_ICSelection_DirectTI, ICFilter);
  }

  /* Select the Trigger source */
  TIM1_SelectInputTrigger((TIM1_TRGSelection_TypeDef)TIM1_TIxExternalCLKSource);

  /* Select the External clock mode1 */
  TIM1->SMCR |= (uint8_t)(TIM1_SlaveMode_External1);
}

/**
  * @brief  Configures the TIM1 External clock Mode1.
  * @param  TIM1_ExtTRGPrescaler specifies the external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ExtTRGPSC_OFF
  *            @arg TIM1_ExtTRGPSC_DIV2
  *            @arg TIM1_ExtTRGPSC_DIV4
  *            @arg TIM1_ExtTRGPSC_DIV8.
  * @param  TIM1_ExtTRGPolarity specifies the external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ExtTRGPolarity_Inverted
  *            @arg TIM1_ExtTRGPolarity_NonInverted
  * @param  ExtTRGFilter specifies the External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM1_ETRClockMode1Config(TIM1_ExtTRGPSC_TypeDef TIM1_ExtTRGPrescaler,
                              TIM1_ExtTRGPolarity_TypeDef TIM1_ExtTRGPolarity,
                              uint8_t ExtTRGFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM1_EXT_PRESCALER(TIM1_ExtTRGPrescaler));
  assert_param(IS_TIM1_EXT_POLARITY(TIM1_ExtTRGPolarity));
  assert_param(IS_TIM1_EXT_TRG_FILTER(ExtTRGFilter));

  /* Configure the ETR Clock source */
  TIM1_ETRConfig(TIM1_ExtTRGPrescaler, TIM1_ExtTRGPolarity, ExtTRGFilter);

  /* Select the External clock mode1 & Select the Trigger selection : ETRF */
  TIM1->SMCR = (uint8_t)((uint8_t)(TIM1->SMCR & (uint8_t)(~(TIM1_SMCR_SMS | TIM1_SMCR_TS)))
                         | (uint8_t)((uint8_t)TIM1_SlaveMode_External1 | (uint8_t) TIM1_TRGSelection_ETRF));
}

/**
  * @brief  Configures the TIM1 External clock Mode2.
  * @param  TIM1_ExtTRGPrescaler specifies the external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ExtTRGPSC_OFF
  *            @arg TIM1_ExtTRGPSC_DIV2
  *            @arg TIM1_ExtTRGPSC_DIV4
  *            @arg TIM1_ExtTRGPSC_DIV8.
  * @param  TIM1_ExtTRGPolarity specifies the external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ExtTRGPolarity_Inverted
  *            @arg TIM1_ExtTRGPolarity_NonInverted
  * @param  ExtTRGFilter specifies the External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM1_ETRClockMode2Config(TIM1_ExtTRGPSC_TypeDef TIM1_ExtTRGPrescaler,
                              TIM1_ExtTRGPolarity_TypeDef TIM1_ExtTRGPolarity,
                              uint8_t ExtTRGFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM1_EXT_PRESCALER(TIM1_ExtTRGPrescaler));
  assert_param(IS_TIM1_EXT_POLARITY(TIM1_ExtTRGPolarity));

  /* Configure the ETR Clock source */
  TIM1_ETRConfig(TIM1_ExtTRGPrescaler, TIM1_ExtTRGPolarity, ExtTRGFilter);

  /* Enable the External clock mode2 */
  TIM1->ETR |= TIM1_ETR_ECE;
}

/**
  * @}
  */

/** @defgroup TIM1_Group6 Synchronization management functions
 *  @brief    Synchronization management functions 
 *
@verbatim   
 ===============================================================================
                       Synchronization management functions
 ===============================================================================  
                   
       ===================================================================      
              TIM1 Driver: how to use it in synchronization Mode
       =================================================================== 
       Case of two/several Timers
       **************************
       1. If TIM1 is used as master to other timers use the following functions:
          - TIM1_SelectOutputTrigger()
          - TIM1_SelectMasterSlaveMode()
       2. If TIM1 is used as slave to other timers use the following functions:
          - TIM1_SelectInputTrigger()
          - TIM1_SelectSlaveMode()
          
       Case of Timers and external trigger (TRIG pin)
       ********************************************       
       1. Configure the External trigger using TIM1_ETRConfig()
       2. Configure the Slave Timer using the following functions:
          - TIM1_SelectInputTrigger()
          - TIM1_SelectSlaveMode()

@endverbatim
  * @{
  */

/**
  * @brief  Selects the TIM1 Input Trigger source.
  * @param  TIM1_InputTriggerSource specifies Input Trigger source.
  *          This parameter can be one of the following values:
  *            @arg TIM1_TRGSelection_TIM4
  *            @arg TIM1_TRGSelection_TIM3
  *            @arg TIM1_TRGSelection_TIM2
  *            @arg TIM1_TRGSelection_TI1F_ED: TI1 Edge Detector
  *            @arg TIM1_TRGSelection_TI1FP1: Filtered Timer Input 1
  *            @arg TIM1_TRGSelection_TI2FP2: Filtered Timer Input 2
  *            @arg TIM1_TRGSelection_ETRF: External Trigger input
  * @retval None
  */
void TIM1_SelectInputTrigger(TIM1_TRGSelection_TypeDef TIM1_InputTriggerSource)
{
  /* Check the parameters */
  assert_param(IS_TIM1_TRIGGER_SELECTION(TIM1_InputTriggerSource));

  /* Select the Tgigger Source */
  TIM1->SMCR = (uint8_t)((uint8_t)(TIM1->SMCR & (uint8_t)(~TIM1_SMCR_TS)) | (uint8_t)TIM1_InputTriggerSource);
}

/**
  * @brief  Selects the TIM1 Trigger Output Mode.
  * @param  TIM1_TRGOSource specifies the Trigger Output source.
  *          This parameter can be one of the following values
  *            @arg TIM1_TRGOSource_Reset
  *            @arg TIM1_TRGOSource_Enable
  *            @arg TIM1_TRGOSource_Update
  *            @arg TIM1_TRGOSource_OC1
  *            @arg TIM1_TRGOSource_OC1REF
  *            @arg TIM1_TRGOSource_OC2REF
  *            @arg TIM1_TRGOSource_OC3REF
  *            @arg TIM1_TRGOSource_OC4REF
  * @retval None
  */
void TIM1_SelectOutputTrigger(TIM1_TRGOSource_TypeDef TIM1_TRGOSource)
{
  /* Check the parameters */
  assert_param(IS_TIM1_TRGO_SOURCE(TIM1_TRGOSource));
  /* Reset the MMS Bits & Select the TRGO source */
  TIM1->CR2 = (uint8_t)((uint8_t)(TIM1->CR2 & (uint8_t)(~TIM1_CR2_MMS)) | (uint8_t) TIM1_TRGOSource);
}

/**
  * @brief  Selects the TIM1 Slave Mode.
  * @param  TIM1_SlaveMode specifies the TIM1 Slave Mode.
  *          This parameter can be one of the following values
  *            @arg TIM1_SlaveMode_Reset
  *            @arg TIM1_SlaveMode_Gated
  *            @arg TIM1_SlaveMode_Trigger
  *            @arg TIM1_SlaveMode_External1
  * @retval None
  */
void TIM1_SelectSlaveMode(TIM1_SlaveMode_TypeDef TIM1_SlaveMode)
{
  /* Check the parameters */
  assert_param(IS_TIM1_SLAVE_MODE(TIM1_SlaveMode));

  /* Reset the SMS Bits and Select the Slave Mode */
  TIM1->SMCR = (uint8_t)((uint8_t)(TIM1->SMCR & (uint8_t)(~TIM1_SMCR_SMS)) | (uint8_t)TIM1_SlaveMode);

}

/**
  * @brief  Sets or Resets the TIM1 Master/Slave Mode.
  * @param  NewState new state of the synchronization between TIM1 and its slaves
  *         (through TRGO). This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void TIM1_SelectMasterSlaveMode(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the MSM Bit */
  if (NewState != DISABLE)
  {
    TIM1->SMCR |= TIM1_SMCR_MSM;
  }
  else
  {
    TIM1->SMCR &= (uint8_t)(~TIM1_SMCR_MSM);
  }
}

/**
  * @brief  Configures the TIM1 External Trigger.
  * @param  TIM1_ExtTRGPrescaler specifies the external Trigger Prescaler.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ExtTRGPSC_OFF
  *            @arg TIM1_ExtTRGPSC_DIV2
  *            @arg TIM1_ExtTRGPSC_DIV4
  *            @arg TIM1_ExtTRGPSC_DIV8.
  * @param  TIM1_ExtTRGPolarity specifies the external Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ExtTRGPolarity_Inverted
  *            @arg TIM1_ExtTRGPolarity_NonInverted
  * @param  ExtTRGFilter specifies the External Trigger Filter.
  *          This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM1_ETRConfig(TIM1_ExtTRGPSC_TypeDef TIM1_ExtTRGPrescaler,
                    TIM1_ExtTRGPolarity_TypeDef TIM1_ExtTRGPolarity,
                    uint8_t ExtTRGFilter)
{
  assert_param(IS_TIM1_EXT_TRG_FILTER(ExtTRGFilter));

  /* Set the Prescaler, the Filter value and the Polarity */
  TIM1->ETR |= (uint8_t)((uint8_t)((uint8_t) TIM1_ExtTRGPrescaler
                                   | (uint8_t) TIM1_ExtTRGPolarity)
                         | (uint8_t) ExtTRGFilter);
}

/**
  * @}
  */

/** @defgroup TIM1_Group7 Specific interface management functions
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
  * @brief  Configures the TIM1 Encoder Interface.
  * @param  TIM1_EncoderMode specifies the TIM1 Encoder Mode.
  *          This parameter can be one of the following values
  *            @arg TIM1_EncoderMode_TI1: Counter counts on TI1FP1 edge
  *          depending on TI2FP2 level.
  *            @arg TIM1_EncoderMode_TI2: Counter counts on TI2FP2 edge
  *         depending on TI1FP1 level.
  *            @arg TIM1_EncoderMode_TI12: Counter counts on both TI1FP1 and
  *         TI2FP2 edges depending on the level of the other input.
  * @param  TIM1_IC1Polarity specifies the IC1 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPolarity_Rising: Input capture polarity rising
  *            @arg TIM1_ICPolarity_Falling: Input capture polarity falling
  * @param  TIM1_IC2Polarity specifies the IC2 Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPolarity_Rising: Input capture polarity rising
  *            @arg TIM1_ICPolarity_Falling: Input capture polarity falling
  * @retval None
  */
void TIM1_EncoderInterfaceConfig(TIM1_EncoderMode_TypeDef TIM1_EncoderMode,
                                 TIM1_ICPolarity_TypeDef TIM1_IC1Polarity,
                                 TIM1_ICPolarity_TypeDef TIM1_IC2Polarity)
{
  /* Check the parameters */
  assert_param(IS_TIM1_ENCODER_MODE(TIM1_EncoderMode));
  assert_param(IS_TIM1_IC_POLARITY(TIM1_IC1Polarity));
  assert_param(IS_TIM1_IC_POLARITY(TIM1_IC2Polarity));

  /* Set the TI1 and the TI2 Polarities */
  if (TIM1_IC1Polarity != TIM1_ICPolarity_Rising)
  {
    TIM1->CCER1 |= TIM1_CCER1_CC1P;
  }
  else
  {
    TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC1P);
  }

  if (TIM1_IC2Polarity != TIM1_ICPolarity_Rising)
  {
    TIM1->CCER1 |= TIM1_CCER1_CC2P;
  }
  else
  {
    TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC2P);
  }
  /* Set the encoder Mode */
  TIM1->SMCR = (uint8_t)((uint8_t)(TIM1->SMCR & (uint8_t)(TIM1_SMCR_MSM | TIM1_SMCR_TS)) | (uint8_t) TIM1_EncoderMode);

  /* Select the Capture Compare 1 and the Capture Compare 2 as input */
  TIM1->CCMR1 = (uint8_t)((uint8_t)(TIM1->CCMR1 & (uint8_t)(~TIM1_CCMR_CCxS))  | (uint8_t) CCMR_TIxDirect_Set);
  TIM1->CCMR2 = (uint8_t)((uint8_t)(TIM1->CCMR2 & (uint8_t)(~TIM1_CCMR_CCxS)) | (uint8_t) CCMR_TIxDirect_Set);

}

/**
  * @brief  Enables or Disables the TIM’s Hall sensor interface.
  * @param  NewState : The new state of the TIM1 Hall sensor interface.
  *          This parameter can be ENABLE or DISABLE
  * @retval None
  */
void TIM1_SelectHallSensor(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Set or Reset the TI1S Bit */
  if (NewState != DISABLE)
  {
    TIM1->CR2 |= TIM1_CR2_TI1S;
  }
  else
  {
    TIM1->CR2 &= (uint8_t)(~TIM1_CR2_TI1S);
  }
}

/**
  * @}
  */

/**
  * @brief  Configure the TI1 as Input.
  * @param  TIM1_ICPolarity  The Input Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPolarity_Rising: Input capture polarity rising
  *            @arg TIM1_ICPolarity_Falling: Input capture polarity falling
  * @param  TIM1_ICSelection specifies the input to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICSelection_DirectTI: TIM1 Input 1 is selected to be connected to IC1.
  *            @arg TIM1_ICSelection_IndirectTI: TIM1 Input 1 is selected to be connected to IC2.
  * @param  TIM1_ICFilter Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI1_Config(uint8_t TIM1_ICPolarity, uint8_t TIM1_ICSelection, uint8_t TIM1_ICFilter)
{

  /* Disable the Channel 1: Reset the CCE Bit */
  TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC1E);

  /* Select the Input and set the filter */
  TIM1->CCMR1 = (uint8_t)((uint8_t)(TIM1->CCMR1 & (uint8_t)(~( TIM1_CCMR_CCxS | TIM1_CCMR_ICxF)))
                          | (uint8_t)(( (TIM1_ICSelection)) | ((uint8_t)( TIM1_ICFilter << 4))));



  /* Select the Polarity */
  if (TIM1_ICPolarity != TIM1_ICPolarity_Rising)
  {
    TIM1->CCER1 |= TIM1_CCER1_CC1P;
  }
  else
  {
    TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC1P);
  }

  /* Set the CCE Bit */
  TIM1->CCER1 |=  TIM1_CCER1_CC1E;
}

/**
  * @brief  Configure the TI2 as Input.
  * @param  TIM1_ICPolarity  The Input Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPolarity_Rising: Input capture polarity rising
  *            @arg TIM1_ICPolarity_Falling: Input capture polarity falling
  * @param  TIM1_ICSelection specifies the input to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICSelection_DirectTI: TIM1 Input 2 is selected to be connected to IC2.
  *            @arg TIM1_ICSelection_IndirectTI: TIM1 Input 2 is selected to be connected to IC1.
  * @param  TIM1_ICFilter Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI2_Config(uint8_t TIM1_ICPolarity, uint8_t TIM1_ICSelection, uint8_t TIM1_ICFilter)
{
  /* Disable the Channel 2: Reset the CCE Bit */
  TIM1->CCER1 &=  (uint8_t)(~TIM1_CCER1_CC2E);

  /* Select the Input and set the filter */
  TIM1->CCMR2  = (uint8_t)((uint8_t)(TIM1->CCMR2 & (uint8_t)(~( TIM1_CCMR_CCxS | TIM1_CCMR_ICxF)))
                           | (uint8_t)(( (TIM1_ICSelection)) | ((uint8_t)( TIM1_ICFilter << 4))));
  /* Select the Polarity */
  if (TIM1_ICPolarity != TIM1_ICPolarity_Rising)
  {
    TIM1->CCER1 |= TIM1_CCER1_CC2P;
  }
  else
  {
    TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC2P);
  }
  /* Set the CCE Bit */
  TIM1->CCER1 |=  TIM1_CCER1_CC2E;
}

/**
  * @brief  Configure the TI3 as Input.
  * @param  TIM1_ICPolarity  The Input Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPolarity_Rising: Input capture polarity rising
  *            @arg TIM1_ICPolarity_Falling: Input capture polarity falling
  * @param  TIM1_ICSelection specifies the input to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICSelection_DirectTI: TIM1 Input 3 is selected to
  *                         be connected to IC3.
  * @param  TIM1_ICFilter Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI3_Config(uint8_t TIM1_ICPolarity, uint8_t TIM1_ICSelection, uint8_t TIM1_ICFilter)
{
  /* Disable the Channel 3: Reset the CCE Bit */
  TIM1->CCER2 &=  (uint8_t)(~TIM1_CCER2_CC3E);

  /* Select the Input and set the filter */
  TIM1->CCMR3 = (uint8_t)((uint8_t)(TIM1->CCMR3 & (uint8_t)(~( TIM1_CCMR_CCxS | TIM1_CCMR_ICxF)))
                          | (uint8_t)(( (TIM1_ICSelection)) | ((uint8_t)( TIM1_ICFilter << 4))));

  /* Select the Polarity */
  if (TIM1_ICPolarity != TIM1_ICPolarity_Rising)
  {
    TIM1->CCER2 |= TIM1_CCER2_CC3P;
  }
  else
  {
    TIM1->CCER2 &= (uint8_t)(~TIM1_CCER2_CC3P);
  }
  /* Set the CCE Bit */
  TIM1->CCER2 |=  TIM1_CCER2_CC3E;
}

/**
  * @brief  Configure the TI4 as Input.
  * @param  TIM1_ICPolarity  The Input Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICPolarity_Rising: Input capture polarity rising
  *            @arg TIM1_ICPolarity_Falling: Input capture polarity falling
  * @param  TIM1_ICSelection specifies the input to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM1_ICSelection_IndirectTI: TIM1 Input 4 is selected to be connected to IC3.
  * @param  TIM1_ICFilter Specifies the Input Capture Filter.
  *          This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI4_Config(uint8_t TIM1_ICPolarity, uint8_t TIM1_ICSelection, uint8_t TIM1_ICFilter)
{

  /* Disable the Channel 4: Reset the CCE Bit */
  TIM1->CCER2 &=  (uint8_t)(~TIM1_CCER2_CC4E);

  /* Select the Input and set the filter */
  TIM1->CCMR4 = (uint8_t)((uint8_t)(TIM1->CCMR4 & (uint8_t)(~( TIM1_CCMR_CCxS | TIM1_CCMR_ICxF)))
                          | (uint8_t)(( (TIM1_ICSelection)) | ((uint8_t)( TIM1_ICFilter << 4))));

  /* Select the Polarity */
  if (TIM1_ICPolarity != TIM1_ICPolarity_Rising)
  {
    TIM1->CCER2 |= TIM1_CCER2_CC4P;
  }
  else
  {
    TIM1->CCER2 &= (uint8_t)(~TIM1_CCER2_CC4P);
  }

  /* Set the CCE Bit */
  TIM1->CCER2 |=  TIM1_CCER2_CC4E;
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
