/**
  ******************************************************************************
  * @file    stm8l15x_comp.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the comparators (COMP1 and COMP2) peripheral: 
  *           - Comparators configuration
  *           - Window mode control
  *           - Internal Reference Voltage (VREFINT) output
  *           - Comparator channels trigger configuration
  *           - Interrupts and flags management  
  *           
  *  @verbatim
  *
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================
  *          1- Enable comparators clock using CLK_PeripheralClockConfig(CLK_Peripheral_COMP, ENABLE);
  *            
  *          When using COMP1:
  *          2- Connect internal reference voltage to COMP1 inverting input
  *             using COMP_VrefintToCOMP1Connect()
  *          3- Close the analog switch number 14 using SYSCFG_RIAnalogSwitchConfig()
  *          4- Close the analog switch that corresponds to the pin to be used as 
  *             non inverting input using SYSCFG_RIAnalogSwitchConfig()
  *          5- Close the I/O switch of the pin to be used as non inverting input
  *             using SYSCFG_RIIOSwitchConfig()
  *          6- Configure the event detection using COMP_EdgeConfig()    
  *
  *          When using COMP2:
  *          2- Select the COMP2 inverting input, configure the speed and COMP2
  *             output redirection using COMP_Init()
  *             If the inverting input is an external pin, close the I/O channel
  *             switch using SYSCFG_RIIOSwitchConfig()
  *          3- Close I/O Switch that corresponds to the selected pin as
  *             comparator 2 non inverting input using SYSCFG_RIIOSwitchConfig()    
  *          4- Configure the event detection using COMP_EdgeConfig()   
  *
  * @note
  *          1- COMP1 comparator and ADC can't be used at the same time since
  *             they share the same ADC switch matrix (analog switches).
  *
  *          2- When an I/O is used as comparator input, the corresponding GPIO 
  *             registers should be configured in input floating.
  *
  *          3- Comparators outputs (CMP1OUT and CMP2OUT) are not mapped on
  *             GPIO pin. They are only internal.
  *             To get the comparator output level, use COMP_GetOutputLevel() function    
  *  @endverbatim      
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
#include "stm8l15x_comp.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @defgroup COMP 
  * @brief COMP driver modules
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup COMP_Private_Functions
  * @{
  */

/** @defgroup COMP_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim   
 ===============================================================================
                        Initialization and Configuration functions
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the COMPx peripheral registers to their default reset values.
  * @param  None.
  * @retval None.
  */
void COMP_DeInit(void)
{
  /* Set COMP->CSR1 to reset value 0x00 */
  COMP->CSR1 = (uint8_t) COMP_CSR1_RESET_VALUE;

  /* Set COMP->CSR2 to reset value 0x00 */
  COMP->CSR2 = (uint8_t) COMP_CSR2_RESET_VALUE;

  /* Set COMP->CSR3 to reset value 0xC0 */
  COMP->CSR3 = (uint8_t) COMP_CSR3_RESET_VALUE;

  /* Set COMP->CSR4 to reset value 0x00 */
  COMP->CSR4 = (uint8_t) COMP_CSR4_RESET_VALUE;

  /* Set COMP->CSR5 to reset value 0x00 */
  COMP->CSR5 = (uint8_t) COMP_CSR5_RESET_VALUE;
}

/**
  * @brief  Initializes the comparator inverting input, output and speed.
  * @note   This function configures only COMP2.
  * @param  COMP_InvertingInput : selects the comparator inverting input.
  *          This parameter can be one of the following values:
  *            @arg COMP_InvertingInput_IO: Input/Output on comparator inverting input enable
  *            @arg COMP_InvertingInput_VREFINT: VREFINT on comparator inverting input enable
  *            @arg COMP_InvertingInput_3_4VREFINT: 3/4 VREFINT on comparator inverting input enable
  *            @arg COMP_InvertingInput_1_2VREFINT: 1/2 VREFINT on comparator inverting input enable
  *            @arg COMP_InvertingInput_1_4VREFINT: 1/4 VREFINT on comparator inverting input enable
  *            @arg COMP_InvertingInput_DAC1: DAC1 output on comparator inverting input enable
  *            @arg COMP_InvertingInput_DAC2: DAC2 output on comparator inverting input enable
  * @param  COMP_OutputSelect : selects the comparator output
  *          This parameter can be one of the following values:
  *            @arg COMP_OutputSelect_TIM2IC2: COMP2 output connected to TIM2 Input Capture 2
  *            @arg COMP_OutputSelect_TIM3IC2: COMP2 output connected to TIM3 Input Capture 2
  *            @arg COMP_OutputSelect_TIM1BRK: COMP2 output connected to TIM1 Break Input
  *            @arg COMP_OutputSelect_TIM1OCREFCLR: COMP2 output connected to TIM1 OCREF Clear
  * @param  COMP_Speed selects the comparator speed
  *          This parameter can be one of the following values:
  *            @arg COMP_Speed_Slow: Comparator speed: slow
  *            @arg COMP_Speed_Fast: Comparator speed: fast
  * @retval None.
  */
void COMP_Init(COMP_InvertingInput_Typedef COMP_InvertingInput,
               COMP_OutputSelect_Typedef COMP_OutputSelect, COMP_Speed_TypeDef COMP_Speed)
{
  /* Check the parameters */
  assert_param(IS_COMP_INVERTING_INPUT(COMP_InvertingInput));
  assert_param(IS_COMP_OUTPUT(COMP_OutputSelect));
  assert_param(IS_COMP_SPEED(COMP_Speed));

  /* Reset the INSEL[2:0] bits in CSR3 register */
  COMP->CSR3 &= (uint8_t) (~COMP_CSR3_INSEL);
  /* Select the comparator inverting input */
  COMP->CSR3 |= (uint8_t) COMP_InvertingInput;

  /* Reset the OUTSEL[1:0] bits in CSR3 register */
  COMP->CSR3 &= (uint8_t) (~COMP_CSR3_OUTSEL);
  /* Redirect the comparator output */
  COMP->CSR3 |= (uint8_t) COMP_OutputSelect;

  /* Reset the comparator speed bit */
  COMP->CSR2 &= (uint8_t) (~COMP_CSR2_SPEED);
  /* Select the comparator speed */
  COMP->CSR2 |= (uint8_t) COMP_Speed;
}

/**
  * @brief  Enables or disables connection between VREFINT and COMP1 inverting input.
  * @param  NewState new state of the VREFINT connection to COMP1 inverting input.
  *         This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void COMP_VrefintToCOMP1Connect(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the comparator */
    COMP->CSR3 |= COMP_CSR3_VREFEN;
  }
  else
  {
    /* Disable the comparator */
    COMP->CSR3 &= (uint8_t)(~COMP_CSR3_VREFEN);
  }
}

/**
  * @brief  Configures the COMP edge detection.
  * @param  COMP_Selection: selects the comparator.
  *          This parameter can be one of the following values:
  *            @arg COMP_Selection_COMP1: Selection of Comparator 1
  *            @arg COMP_Selection_COMP2: Selection of Comparator 2
  * @param  COMP_Edge: This parameter can be one of the following values:
  *            @arg COMP_Edge_Falling: Falling edge selection
  *            @arg COMP_Edge_Rising: Rising edge selection  
  *            @arg COMP_Edge_Rising_Falling: Rising and Falling edge selection
  * @retval None.
  */
void COMP_EdgeConfig(COMP_Selection_TypeDef COMP_Selection, COMP_Edge_TypeDef COMP_Edge)
{
  /* Check the parameters */
  assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));
  assert_param(IS_COMP_EDGE(COMP_Edge));

  /* Check if comparator 1 is selected */
  if (COMP_Selection == COMP_Selection_COMP1)
  {
    /* Reset the comparator 1 edge control bits */
    COMP->CSR1 &= (uint8_t) (~COMP_CSR1_CMP1);

    /* Select the edge detection of comparator 1 output */
    COMP->CSR1 |= (uint8_t) COMP_Edge;
  }
  /* The comparator 2 is selected */
  else
  {
    /* Reset the comparator 2 edge control bits */
    COMP->CSR2 &= (uint8_t) (~COMP_CSR2_CMP2);

    /* Select the edge detection of comparator 2 output */
    COMP->CSR2 |= (uint8_t) COMP_Edge;
  }
}

/**
  * @brief  Returns the output level of the comparator.
  * @note   Comparators outputs aren't available on GPIO (outputs levels are 
  *         only internal).
  * @param  COMP_Selection: selects the comparator.
  *          This parameter can be one of the following values:
  *            @arg COMP_Selection_COMP1: Selection of Comparator 1
  *            @arg COMP_Selection_COMP2: Selection of Comparator 2
  * @retval Returns the comparator output level
  *          This value can be one of the following:
  *            - COMP_OutputLevel_Low: Comparator output level is low
  *            - COMP_OutputLevel_High: Comparator output level is high
  */
COMP_OutputLevel_TypeDef COMP_GetOutputLevel(COMP_Selection_TypeDef COMP_Selection)
{
  uint8_t compout;

  /* Check the parameters */
  assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));

  /* Check if Comparator 1 is selected */
  if (COMP_Selection == COMP_Selection_COMP1)
  {
    /* Check if comparator 1 output level is high */
    if ((COMP->CSR1 & COMP_CSR1_CMP1OUT) != (uint8_t) RESET)
    {
      /* Get Comparator 1 output level */
      compout = (COMP_OutputLevel_TypeDef) COMP_OutputLevel_High;
    }
    /* comparator 1 output level is low */
    else
    {
      /* Get Comparator 1 output level */
      compout = (COMP_OutputLevel_TypeDef) COMP_OutputLevel_Low;
    }
  }
  /* Comparator 2 is selected */
  else
  {
    /* Check if comparator 2 output level is high */
    if ((COMP->CSR2 & COMP_CSR2_CMP2OUT) != (uint8_t) RESET)
    {
      /* Get Comparator output level */
      compout = (COMP_OutputLevel_TypeDef) COMP_OutputLevel_High;
    }
    /* comparator 2 output level is low */
    else
    {
      /* Get Comparator 2 output level */
      compout = (COMP_OutputLevel_TypeDef) COMP_OutputLevel_Low;
    }
  }

  /* Return the comparator output level */
  return (COMP_OutputLevel_TypeDef)(compout);
}

/**
  * @}
  */

/** @defgroup COMP_Group2 Window mode control function
 *  @brief   Window mode control function 
 *
@verbatim   
 ===============================================================================
                              Window mode control function
 ===============================================================================  

  In window mode:
      - COMP1 inverting input is fixed to VREFINT defining the first
        threshold
      - COMP2 inverting input is configurable (DAC_OUT1, VREFINT sub-multiples, ...)
        defining the second threshold
      - COMP1 and COMP2 non inverting inputs are connected together.
         
@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the window mode.
  * @param  NewState new state of the window mode.
  *         This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void COMP_WindowCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the window mode */
    COMP->CSR3 |= (uint8_t) COMP_CSR3_WNDWE;
  }
  else
  {
    /* Disable the window mode */
    COMP->CSR3 &= (uint8_t)(~COMP_CSR3_WNDWE);
  }
}
/**
  * @}
  */
  
/** @defgroup COMP_Group3 Internal Reference Voltage output function
 *  @brief   Internal Reference Voltage (VREFINT) output function 
 *
@verbatim   
 ===============================================================================
             Internal Reference Voltage (VREFINT) output function
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the output of the internal reference voltage.
  * @param  NewState : new state of the Vrefint output.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void COMP_VrefintOutputCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the output of internal reference voltage */
    COMP->CSR3 |= (uint8_t) COMP_CSR3_VREFOUTEN;
  }
  else
  {
    /* Disable the output of internal reference voltage */
    COMP->CSR3 &= (uint8_t) (~COMP_CSR3_VREFOUTEN);
  }
}

/**
  * @}
  */

/** @defgroup COMP_Group4 Comparator channels trigger configuration
 *  @brief  Comparator channels trigger configuration
 *
@verbatim   
 ===============================================================================
                Comparator channels trigger configuration
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the schmitt trigger.
  * @param  NewState : new state of the schmitt trigger.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void COMP_SchmittTriggerCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable Schmitt trigger on Input Output switches Channels */
    COMP->CSR1 |= (uint8_t) COMP_CSR1_STE;
  }
  else
  {
    /* Enable Schmitt trigger on Input Output switches Channels */
    COMP->CSR1 &= (uint8_t) (~COMP_CSR1_STE);
  }
}

/**
  * @brief  Enables or disables trigger on the specified input/output group.
  * @param  COMP_TriggerGroup : specifies the input/output group
  *          This parameter can be one of the following values:
  *            @arg COMP_TriggerGroup_InvertingInput: Trigger on comparator 2 inverting input
  *            @arg COMP_TriggerGroup_NonInvertingInput: Trigger on comparator 2 non inverting input
  *            @arg COMP_TriggerGroup_VREFINTOutput: Trigger on VREFINT output
  *            @arg COMP_TriggerGroup_DACOutput: Trigger on DAC output      
  * @param  COMP_TriggerPin : specifies the pin(s) within the input/output group
  *          This parameter can be one of the following values:
  *            @arg COMP_TriggerPin_0: Trigger Pin 0
  *            @arg COMP_TriggerPin_0: Trigger Pin 1
  *            @arg COMP_TriggerPin_0: Trigger Pin 2    
  * @param  NewState : enable or disable the trigger on the selected pin(s)
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void COMP_TriggerConfig(COMP_TriggerGroup_TypeDef COMP_TriggerGroup,
                        COMP_TriggerPin_TypeDef COMP_TriggerPin,
                        FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_COMP_TRIGGERGROUP(COMP_TriggerGroup));
  assert_param(IS_COMP_TRIGGERPIN(COMP_TriggerPin));

  switch (COMP_TriggerGroup)
  {
    case COMP_TriggerGroup_InvertingInput:

      if (NewState != DISABLE)
      {
        COMP->CSR4 &= (uint8_t) ~COMP_TriggerPin;
      }
      else
      {
        COMP->CSR4 |= (uint8_t) COMP_TriggerPin;
      }
      break;

    case COMP_TriggerGroup_NonInvertingInput:
      if (NewState != DISABLE)
      {
        COMP->CSR4 &= (uint8_t) ~((uint8_t)(COMP_TriggerPin << 3));
      }
      else
      {
        COMP->CSR4 |= (uint8_t) (COMP_TriggerPin << 3);
      }
      break;

    case COMP_TriggerGroup_VREFINTOutput:
      if (NewState != DISABLE)
      {
        COMP->CSR5 &= (uint8_t) ~COMP_TriggerPin;
      }
      else
      {
        COMP->CSR5 |= (uint8_t) COMP_TriggerPin;
      }
      break;

    case COMP_TriggerGroup_DACOutput:
      if (NewState != DISABLE)
      {
        COMP->CSR5 &= (uint8_t) ~((uint8_t)(COMP_TriggerPin << 3));
      }
      else
      {
        COMP->CSR5 |= (uint8_t) (COMP_TriggerPin << 3);
      }
      break;

    default:
      break;
  }
}

/**
  * @}
  */
  
/** @defgroup COMP_Group5 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions 
 *
@verbatim   
 ===============================================================================
                   Interrupts and flags management functions
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the interrupt generation when an event is detected.
  * @param  COMP_Selection : selects the comparator
  *          This parameter can be one of the following values:
  *            @arg COMP_Selection_COMP1: Selection of Comparator 1
  *            @arg COMP_Selection_COMP2: Selection of Comparator 2
  * @param  NewState : new state of the COMPx interrupt.
    *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void COMP_ITConfig(COMP_Selection_TypeDef COMP_Selection, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Check if Comparator 1 is selected */
  if (COMP_Selection == COMP_Selection_COMP1)
  {
    if (NewState != DISABLE)
    {
      /* Enable the COMP1 Interrupt source */
      COMP->CSR1 |= (uint8_t) COMP_CSR1_IE1;
    }
    else
    {
      /* Disable the COMP1 Interrupt source */
      COMP->CSR1 &= (uint8_t)(~COMP_CSR1_IE1);
    }
  }
  else /* Comparator 2 is selected */
  {
    if (NewState != DISABLE)
    {
      /* Enable the COMP2 Interrupt source */
      COMP->CSR2 |= (uint8_t) COMP_CSR2_IE2;
    }
    else
    {
      /* Disable the COMP2 Interrupt source */
      COMP->CSR2 &= (uint8_t)(~COMP_CSR2_IE2);
    }
  }
}

/**
  * @brief  Checks whether the comparator flag is set or not.
  * @param  COMP_Selection : selects the comparator
  *          This parameter can be one of the following values:
  *            @arg COMP_Selection_COMP1: Selection of Comparator 1
  *            @arg COMP_Selection_COMP2: Selection of Comparator 2
  * @retval The new state of COMPx event flag (SET or RESET).
  */
FlagStatus COMP_GetFlagStatus(COMP_Selection_TypeDef COMP_Selection)
{
  FlagStatus bitstatus = RESET;

  /* Check the parameters */
  assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));

  /* Check if COMP1 is selected */
  if (COMP_Selection == COMP_Selection_COMP1)
  {
    if ((COMP->CSR1 & COMP_CSR1_EF1) != (uint8_t) RESET)
    {
      /* The comparator 1 event flag is set */
      bitstatus = SET;
    }
    else
    {
      /* The comparator 1 event flag is reset */
      bitstatus = RESET;
    }
  }
  else   /* COMP2 is selected */
  {
    if ((COMP->CSR2 & COMP_CSR2_EF2) != (uint8_t) RESET)
    {
      /* The comparator 2 event flag is set */
      bitstatus = SET;
    }
    else
    {
      /* The comparator 2 event flag is reset */
      bitstatus = RESET;
    }
  }

  /* return the comparator event flag status */
  return (FlagStatus)(bitstatus);
}

/**
  * @brief  Clears the comparator’s pending flag.
  * @param  COMP_Selection : selects the comparator
  *          This parameter can be one of the following values:
  *            @arg COMP_Selection_COMP1: Selection of Comparator 1
  *            @arg COMP_Selection_COMP2: Selection of Comparator 2
  * @retval None.
  */
void COMP_ClearFlag(COMP_Selection_TypeDef COMP_Selection)
{
  /* Check the parameters */
  assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));

  if (COMP_Selection == COMP_Selection_COMP1)
  {
    /* Clear the flag EF1 (rc_w0) clear this bit by writing 0. */
    COMP->CSR1 &= (uint8_t) (~COMP_CSR1_EF1);
  }
  else
  {
    /* Clear the flag EF2 (rc_w0) clear this bit by writing 0. */
    COMP->CSR2 &= (uint8_t) (~COMP_CSR2_EF2);
  }
}

/**
  * @brief  Checks whether the comparator interrupt has occurred or not.
  * @param  COMP_Selection : selects the comparator
  *          This parameter can be one of the following values:
  *            @arg COMP_Selection_COMP1: Selection of Comparator 1
  *            @arg COMP_Selection_COMP2: Selection of Comparator 2
  * @retval ITStatus : The state of the COMPx event flag (SET or RESET).
  */
ITStatus COMP_GetITStatus(COMP_Selection_TypeDef COMP_Selection)
{
  ITStatus bitstatus = RESET;
  uint8_t itstatus = 0x00, itenable = 0x00;

  /* Check the parameters */
  assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));

  if (COMP_Selection == COMP_Selection_COMP1)
  {
    /* Get the EF1 comparator event flag status */
    itstatus = (uint8_t) (COMP->CSR1 & COMP_CSR1_EF1);

    /* Get the IE1 interrupt enable bit status */
    itenable = (uint8_t) (COMP->CSR1 & COMP_CSR1_IE1);

    if ((itstatus != (uint8_t) RESET) && (itenable != (uint8_t) RESET))
    {
      /* the EF1 and IE1 are set */
      bitstatus = SET;
    }
    else
    {
      /* the EF1 or IE1 is reset */
      bitstatus = RESET;
    }
  }
  else
  {
    /* Get the EF2 comparator event flag value */
    itstatus = (uint8_t) (COMP->CSR2 & COMP_CSR2_EF2);

    /* Get the IE2 interrupt enable bit value */
    itenable = (uint8_t) (COMP->CSR2 & COMP_CSR2_IE2);

    if ((itstatus != (uint8_t)RESET) && (itenable != (uint8_t)RESET))
    {
      /* The EF2 and IE2 are set */
      bitstatus = SET;
    }
    else
    {
      /* The EF2 or IE2 is reset */
      bitstatus = RESET;
    }
  }

  /* Return the COMP interrupt status */
  return (ITStatus) bitstatus;
}

/**
  * @brief  Clears the interrupt pending bits of the comparator.
  * @param  COMP_Selection : selects the comparator
  *          This parameter can be one of the following values:
  *            @arg COMP_Selection_COMP1: Selection of Comparator 1
  *            @arg COMP_Selection_COMP2: Selection of Comparator 2
  * @retval None
  */
void COMP_ClearITPendingBit(COMP_Selection_TypeDef COMP_Selection)
{
  /* Check the parameters */
  assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));

  if (COMP_Selection == COMP_Selection_COMP1)
  {
    /* Clear the flag EF1 (rc_w0) clear this bit by writing 0. */
    COMP->CSR1 &= (uint8_t) (~COMP_CSR1_EF1);
  }
  else
  {
    /* Clear the flag EF2 (rc_w0) clear this bit by writing 0. */
    COMP->CSR2 &= (uint8_t) (~COMP_CSR2_EF2);
  }
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
