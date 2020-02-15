/**
  ******************************************************************************
  * @file    stm8l15x_syscfg.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the RI and SYSCFG:
  *            - RI configuration
  *            - SYSCFG configuration
  *
  *  @verbatim
  *  
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================
  *          This driver provides functions to configure the RI and SYSCFG
  *          These functions are split in 2 groups: 
  *   
  *          1. RI configuration: this group includes all needed functions 
  *             to configure the RI:
  *                   - Routing the TIM1 Input capture channels
  *                   - Close and Open the I/O switches
  *                   - Close and Open the analog switches
  *                   - Configure the internal Pull-up and Pull-down resistors
  *           
  *          2. SYSCFG configuration: this group includes all needed functions 
  *             to configure the SYSCFG:
  *                   - Configure the alternate function remapping of some peripherals
  *                     such as: USART, SPI, TIMs...
  *                     Remapping possibilities depends on the device (low-density,
  *                     medium-density or high density) so it is recommended to   
  *                     refer to the product reference manual.
  *                   - Configure the DMA channels remapping of ADC1 and TIM4
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
#include "stm8l15x_syscfg.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @defgroup SYSCFG
  * @brief SYSCFG driver modules
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup SYSCFG_Private_Functions
  * @{
  */

/** @defgroup SYSCFG_Group1 RI configuration
 *  @brief   RI configuration
 *
@verbatim   
 ===============================================================================
                           RI configuration functions
 ===============================================================================  
  
       ===================================================================      
                        SYSCFG Driver: how to configure RI
       =================================================================== 
       
         1. By default, TIM1 Input Capture channels 2 and 3 are connected to the
            I/O port assigned in the datasheet pinout (default routing).
            TIM1 Input Capture channels 2 and 3 can be routed by calling 
            SYSCFG_RITIMInputCaptureConfig()
            Refer to the device reference manual for more details about 
            TIM1 input capture routing
          
         2. For more details about using SYSCFG_RIIOSwitchConfig()
            and SYSCFG_RIAnalogSwitchConfig() with COMP1 and COMP2, refer to
            comparators driver
            
         3. To output the Internal reference voltage VREFINT, configure corresponding
            GPIO pin in input mode using GPIO_Init() then use SYSCFG_RIIOSwitchConfig()
       
         4. To use the internal Pull-up and Pull-down resistors call
            SYSCFG_RIResistorConfig()

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the RI registers to their default reset values.
  * @param  None
  * @retval None
  */
void SYSCFG_RIDeInit(void)
{
  RI->ICR1   = RI_ICR1_RESET_VALUE;     /*!< Set RI->ICR1 to reset value */
  RI->ICR2   = RI_ICR2_RESET_VALUE;     /*!< Set RI->ICR2 to reset value */
  RI->IOSR1  = RI_IOSR1_RESET_VALUE;    /*!< Set RI->IOSR1 to reset value */
  RI->IOSR2  = RI_IOSR2_RESET_VALUE;    /*!< Set RI->IOSR2 to reset value */
  RI->IOSR3  = RI_IOSR3_RESET_VALUE;    /*!< Set RI->IOSR3 to reset value */
  RI->IOSR4  = RI_IOSR4_RESET_VALUE;    /*!< Set RI->IOSR3 to reset value */
  RI->ASCR1  = RI_ASCR1_RESET_VALUE;    /*!< Set RI->ASCR1 to reset value */
  RI->ASCR2  = RI_ASCR2_RESET_VALUE;    /*!< Set RI->ASCR2 to reset value */
  RI->RCR    = RI_RCR_RESET_VALUE;      /*!< Set RI->RCR to reset value */
}

/**
  * @brief  Configures the routing interface to select which Input Output pin
  *         to be routed to TIM1 Input Capture.
  * @param  RI_InputCapture: selects the TIM1 input capture2 RI_InputCapture_IC2
  *         or the TIM1 input capture3 RI_InputCapture_IC3
  * @param  RI_InputCaptureRouting: selects the value to set in TIM1 Input Capture
  *         routing register ICRx and can be from RI_InputCaptureRouting_0 to
  *         RI_InputCaptureRouting_22.
  * @retval None.
  */
void SYSCFG_RITIMInputCaptureConfig(RI_InputCapture_TypeDef RI_InputCapture,
                                    RI_InputCaptureRouting_TypeDef RI_InputCaptureRouting)
{
  /* Check the parameters */
  assert_param(IS_RI_INPUTCAPTURE(RI_InputCapture));
  assert_param(IS_RI_INPUTCAPTUREROUTING(RI_InputCaptureRouting));

  /* Check whether Input Capture 2 is selected */
  if (RI_InputCapture == RI_InputCapture_IC2)
  {
    /* Set the value in ICR1 register to route TIM input Capture 2 */
    RI->ICR1 = (uint8_t) RI_InputCaptureRouting;
  }

  else /* The Input Capture 3 is selected */
  {
    /* Set the value in ICR2 register to route TIM input capture 3 */
    RI->ICR2 = (uint8_t) RI_InputCaptureRouting;
  }
}

/**
  * @brief  Enables or disables the Routing Interface Analog switch.
  * @param  RI_AnalogSwitch: Analog Switch
  *          This parameter can be one of the following values:
  *            @arg RI_AnalogSwitch_0: Analog switch 0
  *            @arg RI_AnalogSwitch_1: Analog switch 1
  *            @arg RI_AnalogSwitch_2: Analog switch 2
  *            @arg RI_AnalogSwitch_3: Analog switch 3
  *            @arg RI_AnalogSwitch_4: Analog switch 4
  *            @arg RI_AnalogSwitch_5: Analog switch 5
  *            @arg RI_AnalogSwitch_6: Analog switch 6
  *            @arg RI_AnalogSwitch_7: Analog switch 7
  *            @arg RI_AnalogSwitch_8: Analog switch 8
  *            @arg RI_AnalogSwitch_9: Analog switch 9
  *            @arg RI_AnalogSwitch_10: Analog switch 10
  *            @arg RI_AnalogSwitch_11: Analog switch 11
  *            @arg RI_AnalogSwitch_12: Analog switch 14                          
  * @param  NewState: new state of the analog switch.
  *         This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void SYSCFG_RIAnalogSwitchConfig(RI_AnalogSwitch_TypeDef RI_AnalogSwitch,
                                 FunctionalState NewState)
{
  uint8_t AnalogSwitchRegister, AnalogSwitchIndex = 0;

  /* Check the parameters */
  assert_param(IS_RI_ANALOGSWITCH(RI_AnalogSwitch));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Get the analog switch register ASCR1 or ASCR2 */
  AnalogSwitchRegister = (uint8_t) (RI_AnalogSwitch & (uint8_t) 0xF0);

  /* Get the analog switch bit index in ASCRx register */
  AnalogSwitchIndex = (uint8_t) (RI_AnalogSwitch & (uint8_t) 0x0F);

  if (NewState != DISABLE)
  {
    if (AnalogSwitchRegister == (uint8_t) 0x10)
    {
      /* Enable the analog switch */
      RI->ASCR1 |= (uint8_t) ((uint8_t)1 << (uint8_t) AnalogSwitchIndex);
    }
    else
    {
      /* Enable the analog switch */
      RI->ASCR2 |= (uint8_t) ((uint8_t)1 << (uint8_t) AnalogSwitchIndex);
    }
  }
  else
  {
    if (AnalogSwitchRegister == (uint8_t) 0x10)
    {
      /* Disable the analog switch */
      RI->ASCR1 &= (uint8_t) (~(uint8_t)((uint8_t)1 <<  AnalogSwitchIndex));
    }
    else
    {
      /* Disable the analog switch */
      RI->ASCR2 &= (uint8_t) (~ (uint8_t)((uint8_t)1 << AnalogSwitchIndex));
    }
  }
}

/**
  * @brief  Closes or Opens the routing interface Input Output switches.
  * @param  RI_IOSwitch: can be one of RI_IOSwitch_1..RI_IOSwitch_24
  * @param  NewState: new state of the Input Output Switch.
  *         This parameter can be ENABLE or DISABLE.
  * @retval None.
  */
void SYSCFG_RIIOSwitchConfig(RI_IOSwitch_TypeDef RI_IOSwitch,
                             FunctionalState NewState)
{
  uint8_t IOSwitchRegister, IOSwitchIndex = 0;

  /* Check the parameters */
  assert_param(IS_RI_IOSWITCH(RI_IOSwitch));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Get the Input Output switch bit index in IOSRx register */
  IOSwitchIndex = (uint8_t) (RI_IOSwitch & (uint8_t) 0x0F);

  /* Get the Input Output switch register IOSR1, IOSR2 or IOSR3 */
  IOSwitchRegister = (uint8_t) (RI_IOSwitch & (uint8_t) 0xF0);

  /* Check whether the Input Output switch control bit is in the IOSR1 register */
  if (IOSwitchRegister == (uint8_t) 0x10)
  {
    if (NewState != DISABLE)
    {
      /* Close the Input Output switch */
      RI->IOSR1 |= (uint8_t) ((uint8_t)1 << IOSwitchIndex);
    }
    else
    {
      /* Open the Input Output switch */
      RI->IOSR1 &= (uint8_t) (~ (uint8_t)((uint8_t)1 << IOSwitchIndex));
    }
  }

  /* Check whether the Input Output switch control bit is in the IOSR2 register */
  else if (IOSwitchRegister == (uint8_t) 0x20)
  {
    if (NewState != DISABLE)
    {
      /* Close the Input Output switch */
      RI->IOSR2 |= (uint8_t) ((uint8_t)1 << IOSwitchIndex);
    }
    else
    {
      /* Open the Input Output switch */
      RI->IOSR2 &= (uint8_t) (~(uint8_t)((uint8_t)1 <<  IOSwitchIndex));
    }
  }

  /* The Input Output switch control bit is in the IOSR3 register */
  else if (IOSwitchRegister == (uint8_t) 0x30)
  {
    if (NewState != DISABLE)
    {
      /* Close the Input Output switch */
      RI->IOSR3 |= (uint8_t) ((uint8_t)1 << IOSwitchIndex);
    }
    else
    {
      /* Open the Input Output switch */
      RI->IOSR3 &= (uint8_t) (~ (uint8_t) ((uint8_t) 1 << IOSwitchIndex));
    }
  }

  /* The Input Output switch control bit is in the IOSR4 register */
  else
  {
    if (NewState != DISABLE)
    {
      /* Close the Input Output switch */
      RI->IOSR4 |= (uint8_t) ((uint8_t)1 << IOSwitchIndex);
    }
    else
    {
      /* Open the Input Output switch */
      RI->IOSR4 &= (uint8_t) (~ (uint8_t) ((uint8_t) 1 << IOSwitchIndex));
    }
  }
}

/**
  * @brief  Configures the Pull-up and Pull-down Resistors
  * @param  RI_Resistor: selects the resistor to connect,
  *          This parameter can be one of the following values:
  *            @arg RI_Resistor_10KPU: 10K pull up
  *            @arg RI_Resistor_400KPU: 400K pull up
  *            @arg RI_Resistor_10KPD: 10K pull down
  *            @arg RI_Resistor_400KPD: 400K pull down      
  * @retval None
  */
void SYSCFG_RIResistorConfig(RI_Resistor_TypeDef RI_Resistor, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RI_RESISTOR(RI_Resistor));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the resistor */
    RI->RCR |= (uint8_t) RI_Resistor;
  }
  else
  {
    /* Disable the Resistor */
    RI->RCR &= (uint8_t) (~RI_Resistor);
  }
}

/**
  * @}
  */

/** @defgroup SYSCFG_Group2 SYSCFG configuration
 *  @brief   SYSCFG configuration
 *
@verbatim   
 ===============================================================================
                           SYSCFG configuration functions
 ===============================================================================  
  
       ===================================================================      
                        SYSCFG Driver: how to use it for remapping
       =================================================================== 
       
         1. To remap the alternate function of some peripherals (such as: USART,
            SPI, TIMs...), use SYSCFG_REMAPPinConfig()
          
         2. To remap the DMA channels of ADC1 and TIM4, use SYSCFG_REMAPDMAChannelConfig()

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the Remapping registers to their default reset values.
  * @param  None
  * @retval None
  */
void SYSCFG_REMAPDeInit(void)
{
  /*!< Set RMPCR1 to reset value */
  SYSCFG->RMPCR1 = SYSCFG_RMPCR1_RESET_VALUE;

  /*!< Set RMPCR2 to reset value */
  SYSCFG->RMPCR2 = SYSCFG_RMPCR2_RESET_VALUE;

  /*!< Set RMPCR3 to reset value */
  SYSCFG->RMPCR3 = SYSCFG_RMPCR3_RESET_VALUE;
}

/**
  * @brief  Changes the mapping of the specified pins.
  * @param  REMAP_Pin: selects the pin to remap.
  *          This parameter can be one of the following values:
  *            @arg REMAP_Pin_USART1TxRxPortA: USART1 Tx- Rx (PC3- PC2) remapping to PA2- PA3
  *            @arg REMAP_Pin_USART1TxRxPortC: USART1 Tx- Rx (PC3- PC2) remapping to PC5- PC6 
  *            @arg REMAP_Pin_USART1Clk: USART1 CK (PC4) remapping to PA0
  *            @arg REMAP_Pin_SPI1Full: SPI1 MISO- MOSI- SCK- NSS(PB7- PB6- PB5- PB4)
  *                                     remapping to PA2- PA3- PC6- PC5
  *            @arg REMAP_Pin_ADC1ExtTRIG1: ADC1 External Trigger 1 (PA6) remapping to PD0
  *            @arg REMAP_Pin_TIM2TRIGPortA: TIM2 Trigger (PB3) remapping to PA4
  *            @arg REMAP_Pin_TIM3TRIGPortA: TIM3 Trigger (PD1) remapping to PA5
  *            @arg REMAP_Pin_TIM2TRIGLSE: TIM2 Trigger remapping to LSE
  *            @arg REMAP_Pin_TIM3TRIGLSE: TIM3 Trigger remapping to LSE
  *            @arg REMAP_Pin_SPI2Full: SPI2 MISO- MOSI- SCK- NSS(PG7- PG6- PG5- PG4)
  *                                     remapping to PI3- PI2- PI1- PI0
  *            @arg REMAP_Pin_TIM3TRIGPortG: TIM3 Trigger (PD1) remapping to PG3
  *            @arg REMAP_Pin_TIM23BKIN: TIM2 Break Input (PA4) remapping to PG0
  *                                      and TIM3 Break Input (PA5) remapping to PG1
  *            @arg REMAP_Pin_SPI1PortF: SPI1 MISO- MOSI- SCK- NSS(PB7- PB6- PB5- PB4)
  *                                      remapping to PF0- PF1- PF2- PF3
  *            @arg REMAP_Pin_USART3TxRxPortF: USART3 Tx- Rx (PG1- PG0) remapping to PF0- PF1
  *            @arg REMAP_Pin_USART3Clk: USART3 CK (PG2) remapping to PF2
  *            @arg REMAP_Pin_TIM3Channel1: TIM3 Channel 1 (PB1) remapping to PI0 
  *            @arg REMAP_Pin_TIM3Channel2: TIM3 Channel 2 (PD0) remapping to PI3
  *            @arg REMAP_Pin_CCO: CCO (PC4) remapping to PE2
  *            @arg REMAP_Pin_TIM2Channel1: TIM2 Channel 1 (PB0) remapping to PC5
  *            @arg REMAP_Pin_TIM2Channel2: TIM2 Channel 2 (PB2) remapping to PC6                                       
  * @param  NewState: This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void SYSCFG_REMAPPinConfig(REMAP_Pin_TypeDef REMAP_Pin, FunctionalState NewState)
{
  uint8_t regindex = 0;
  /* Check the parameters */
  assert_param(IS_REMAP_PIN(REMAP_Pin));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Read register index */
  regindex = (uint8_t) ((uint16_t) REMAP_Pin >> 8);

  /* Check if REMAP_Pin is in RMPCR1 register */
  if (regindex == 0x01)
  {
    SYSCFG->RMPCR1 &= (uint8_t)((uint8_t)((uint8_t)REMAP_Pin << 4) | (uint8_t)0x0F);
    if (NewState != DISABLE)
    {
      SYSCFG->RMPCR1 |= (uint8_t)((uint16_t)REMAP_Pin & (uint16_t)0x00F0);
    }
  }
  /* Check if REMAP_Pin is in RMPCR2 register */
  else if (regindex == 0x02)
  {
    if (NewState != DISABLE)
    {
      SYSCFG->RMPCR2 |= (uint8_t) REMAP_Pin;
    }
    else
    {
      SYSCFG->RMPCR2 &= (uint8_t)((uint16_t)(~(uint16_t)REMAP_Pin));
    }
  }
  /* REMAP_Pin is in RMPCR3 register */
  else
  {
    if (NewState != DISABLE)
    {
      SYSCFG->RMPCR3 |= (uint8_t) REMAP_Pin;
    }
    else
    {
      SYSCFG->RMPCR3 &= (uint8_t)((uint16_t)(~(uint16_t)REMAP_Pin));
    }
  }
}

/**
  * @brief  Remaps the DMA Channel to the specific peripheral (ADC or TIM4)
  * @param  REMAP_DMAChannel: specifies the DMA Channel to remap.
  *          This parameter can be one of the following values:
  *            @arg REMAP_DMA1Channel_ADC1ToChannel0: ADC1 DMA1 req/ack mapped on DMA1 channel 0
  *            @arg REMAP_DMA1Channel_ADC1ToChannel1: ADC1 DMA1 req/ack mapped on DMA1 channel 1
  *            @arg REMAP_DMA1Channel_ADC1ToChannel2: ADC1 DMA1 req/ack mapped on DMA1 channel 2
  *            @arg REMAP_DMA1Channel_ADC1ToChannel3: ADC1 DMA1 req/ack mapped on DMA1 channel 3
  *            @arg REMAP_DMA1Channel_TIM4ToChannel0: TIM4 DMA1 req/ack mapped on DMA1 channel 0
  *            @arg REMAP_DMA1Channel_TIM4ToChannel1: TIM4 DMA1 req/ack mapped on DMA1 channel 1
  *            @arg REMAP_DMA1Channel_TIM4ToChannel2: TIM4 DMA1 req/ack mapped on DMA1 channel 2
  *            @arg REMAP_DMA1Channel_TIM4ToChannel3: TIM4 DMA1 req/ack mapped on DMA1 channel 3              
  * @retval None
  */
void SYSCFG_REMAPDMAChannelConfig(REMAP_DMAChannel_TypeDef REMAP_DMAChannel)
{
  /* Check the parameters */
  assert_param(IS_REMAP_DMACHANNEL(REMAP_DMAChannel));

  /* Check if the TIM4 DMA channel is selected: bits 4 --> 7 are set */
  if ((REMAP_DMAChannel & 0xF0) != RESET)
  {
    /* Reset the TIM4 DMA channels */
    SYSCFG->RMPCR1 &= (uint8_t) (~SYSCFG_RMPCR1_TIM4DMA_REMAP);
  }
  /* ADC DMA channel is selected: bits 4 --> 7 are reset */
  else
  {
    /* Reset the ADC DMA channels */
    SYSCFG->RMPCR1 &= (uint8_t) (~SYSCFG_RMPCR1_ADC1DMA_REMAP);
  }
  /* Set the DMA Channels remapping */
  SYSCFG->RMPCR1 |= (uint8_t) ((uint8_t)0x0F & REMAP_DMAChannel);
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
