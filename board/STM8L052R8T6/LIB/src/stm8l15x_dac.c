/**
  ******************************************************************************
  * @file    stm8l15x_dac.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Digital-to-Analog Converter (DAC) peripheral: 
  *           - DAC channels configuration: trigger, output buffer, data format
  *           - DMA management      
  *           - Interrupts and flags management
  *
  *  @verbatim
  *    
  *          ===================================================================
  *                             DAC Peripheral features
  *          ===================================================================
  *          The device integrates two 12-bit Digital Analog Converters that can 
  *          be used independently or simultaneously (dual mode):
  *            1- DAC channel1 with DAC_OUT1 (PF0) as output
  *            1- DAC channel2 with DAC_OUT2 (PF1) as output
  *
  *          Digital to Analog conversion can be non-triggered using DAC_Trigger_None
  *          and DAC_OUT1/DAC_OUT2 is available once writing to DHRx register using 
  *          DAC_SetChannel1Data()/DAC_SetChannel2Data.
  *   
  *         Digital to Analog conversion can be triggered by:
  *             1- External event: PE4 using DAC_Trigger_Ext.
  *                This pin (PE4) must be configured in input mode.
  *
  *             2- Timers TRGO: TIM4, TIM5 
  *                (DAC_Trigger_T4_TRGO, DAC_Trigger_T5_TRGO)
  *                The timer TRGO event should be selected using TIMx_SelectOutputTrigger()
  *
  *             3- Software using DAC_Trigger_Software
  *
  *          Each DAC channel integrates an output buffer that can be used to 
  *          reduce the output impedance, and to drive external loads directly
  *          without having to add an external operational amplifier.
  *          
  *          Refer to the device datasheet for more details about output impedance
  *          value with and without output buffer.
  *
  *          Both DAC channels can be used to generate
  *             1- Noise wave using DAC_Wave_Noise
  *             2- Triangle wave using DAC_Wave_Triangle
  *        
  *
  *          The DAC data format can be:
  *             1- 8-bit right alignment using DAC_Align_8b_R
  *             2- 12-bit left alignment using DAC_Align_12b_L
  *             3- 12-bit right alignment using DAC_Align_12b_R
  *
  *          The analog output voltage on each DAC channel pin is determined
  *          by the following equation: DAC_OUTx = VREF+ * DOR / 4095
  *             with  DOR is the Data Output Register
  *                   VEF+ is the input voltage reference (refer to the device datasheet)
  *          e.g. To set DAC_OUT1 to 0.7V, use
  *            DAC_SetChannel1Data(DAC_Align_12b_R, 868);
  *          Assuming that VREF+ = 3.3, DAC_OUT1 = (3.3 * 868) / 4095 = 0.7V
  *
  *          A DMA1 request can be generated when an external trigger (but not
  *          a software trigger) occurs if DMA1 requests are enabled using
  *          DAC_DMACmd()
  *          DMA1 requests are mapped as following:
  *             1- DAC channel1 is mapped on DMA1 channel3 which must be already 
  *                configured
  *             2- DAC channel2 is mapped on DMA1 channel1 which must be already 
  *                configured
  *
  *          ===================================================================      
  *                              How to use this driver 
  *          ===================================================================          
  *            - DAC clock must be enabled to get write access to DAC registers using
  *              CLK_PeripheralClockConfig(CLK_Peripheral_DAC, ENABLE)
  *            - Configure DAC_OUTx (DAC_OUT1: PF0, DAC_OUT2: PF1) in analog mode.
  *            - Configure the DAC channel using DAC_Init()
  *            - Enable the DAC channel using DAC_Cmd()
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
#include "stm8l15x_dac.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup DAC 
  * @brief DAC driver modules
  * @{
  */ 
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup DAC_Private_Functions
  * @{
  */ 

/** @defgroup DAC_Group1 DAC channels configuration
 *  @brief   DAC channels configuration: trigger, output buffer, data format 
 *
@verbatim   
 ===============================================================================
          DAC channels configuration: trigger, output buffer, data format
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the DAC peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void DAC_DeInit(void)
{
  /*  Set Channel1  the Configuration registers to their reset values */
  DAC->CH1CR1 = DAC_CR1_RESET_VALUE;
  DAC->CH1CR2 = DAC_CR2_RESET_VALUE;

  /*  Set Channel2  the Configuration registers to their reset values */
  DAC->CH2CR1 = DAC_CR1_RESET_VALUE;
  DAC->CH2CR2 = DAC_CR2_RESET_VALUE;

  /*  Set the Software Trigger configuration registers to their reset values */
  DAC->SWTRIGR = DAC_SWTRIGR_RESET_VALUE;

  /*  Set the Status registers to their reset values */
  DAC->SR = (uint8_t)~DAC_SR_RESET_VALUE;

  /*  Set the Channel1 Data holding registers to their reset values */
  DAC->CH1RDHRH = DAC_RDHRH_RESET_VALUE;
  DAC->CH1RDHRL = DAC_RDHRL_RESET_VALUE;
  DAC->CH1LDHRH = DAC_LDHRH_RESET_VALUE;
  DAC->CH1LDHRL = DAC_LDHRL_RESET_VALUE;
  DAC->CH1DHR8 = DAC_DHR8_RESET_VALUE;

  /*  Set the Channel2 Data holding registers to their reset values */
  DAC->CH2RDHRH = DAC_RDHRH_RESET_VALUE;
  DAC->CH2RDHRL = DAC_RDHRL_RESET_VALUE;
  DAC->CH2LDHRH = DAC_LDHRH_RESET_VALUE;
  DAC->CH2LDHRL = DAC_LDHRL_RESET_VALUE;
  DAC->CH2DHR8 = DAC_DHR8_RESET_VALUE;

  /*  Set the Dual mode 12bit Right Data holding registers to their reset values */
  DAC->DCH1RDHRH = DAC_RDHRH_RESET_VALUE;
  DAC->DCH1RDHRL = DAC_RDHRL_RESET_VALUE;
  DAC->DCH2RDHRH = DAC_RDHRH_RESET_VALUE;
  DAC->DCH2RDHRL = DAC_RDHRL_RESET_VALUE;

  /*  Set the Dual mode 12bit Left Data holding registers to their reset values */
  DAC->DCH1LDHRH = DAC_LDHRH_RESET_VALUE;
  DAC->DCH1LDHRL = DAC_LDHRL_RESET_VALUE;
  DAC->DCH2LDHRH = DAC_LDHRH_RESET_VALUE;
  DAC->DCH2LDHRL = DAC_LDHRL_RESET_VALUE;

  /*  Set the Dual mode 8bit Data holding registers to their reset values */
  DAC->DCH1DHR8 = DAC_DHR8_RESET_VALUE;
  DAC->DCH2DHR8 = DAC_DHR8_RESET_VALUE;
}

/**
  * @brief  Initializes the DAC according to the specified parameters.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_Trigger : the selected DAC trigger. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Trigger_None: DAC trigger None 
  *            @arg DAC_Trigger_T4_TRGO: DAC trigger TIM4 TRGO
  *            @arg DAC_Trigger_T5_TRGO: DAC trigger TIM5 TRGO
  *            @arg DAC_Trigger_Ext: DAC trigger External Trigger (PE4)  
  *            @arg DAC_Trigger_Software: DAC trigger software 
  * @param  DAC_OutputBuffer : the status of DAC load Buffer
  *          This parameter can be one of the following values:
  *            @arg DAC_OutputBuffer_Enable: DAC output buffer Enabled
  *            @arg DAC_OutputBuffer_Disable: DAC output buffer Disabled  
  * @retval None
  */
void DAC_Init(DAC_Channel_TypeDef DAC_Channel,
              DAC_Trigger_TypeDef DAC_Trigger,
              DAC_OutputBuffer_TypeDef DAC_OutputBuffer)
{
  uint8_t tmpreg = 0;
  uint16_t tmpreg2 = 0;

  /* Check the DAC parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_DAC_TRIGGER(DAC_Trigger));
  assert_param(IS_DAC_OUTPUT_BUFFER_STATE(DAC_OutputBuffer));

  /* Get the DAC CHxCR1 value */
  tmpreg2 =  (uint16_t)((uint8_t)((uint8_t)DAC_Channel << 1));
  tmpreg = *(uint8_t*)((uint16_t)(DAC_BASE + CR1_Offset + tmpreg2));

  /* Clear BOFFx, TENx, TSELx bits */
  tmpreg &= (uint8_t)~(DAC_CR1_BOFF | DAC_CR1_TEN | DAC_CR1_TSEL );

  /* Set BOFFx bit according to DAC_OutputBuffer value */
  tmpreg |= (uint8_t)(DAC_OutputBuffer);


  /* Configure for the selected DAC channel trigger*/
  if (DAC_Trigger != DAC_Trigger_None)
  {
    /* Set TSELx and TEN  bits according to DAC_Trigger value */
    tmpreg |= (uint8_t)(DAC_CR1_TEN | DAC_Trigger) ;
  }

  /* Write to DAC CHxCR1 */
  *(uint8_t*)((uint16_t)(DAC_BASE + CR1_Offset + tmpreg2)) = (uint8_t)tmpreg;
}

/**
  * @brief  Enables or disables the specified DAC channel.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  NewState: new state of the DAC channel. 
  *      This parameter can be: ENABLE or DISABLE.
  * @note When the DAC channel is enabled the trigger source can no more
  *       be modified.
  * @retval None
  */
void DAC_Cmd(DAC_Channel_TypeDef DAC_Channel, FunctionalState NewState)
{
  uint16_t cr1addr = 0;
  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Find CHxCR1 register Address */
  cr1addr = DAC_BASE + CR1_Offset + (uint8_t)((uint8_t)DAC_Channel << 1);

  if (NewState != DISABLE)
  {
    /* Enable the selected DAC channel */
    (*(uint8_t*)(cr1addr)) |= DAC_CR1_EN;
  }
  else
  {
    /* Disable the selected DAC channel */
    (*(uint8_t*)(cr1addr)) &= (uint8_t) ~(DAC_CR1_EN);
  }
}

/**
  * @brief  Enables or disables the selected DAC channel software trigger.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  NewState: new state of the selected DAC channel software trigger.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void DAC_SoftwareTriggerCmd(DAC_Channel_TypeDef DAC_Channel, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable software trigger for the selected DAC channel */
    DAC->SWTRIGR |= (uint8_t)(DAC_SWTRIGR_SWTRIG1 << DAC_Channel);
  }
  else
  {
    /* Disable software trigger for the selected DAC channel */
    DAC->SWTRIGR &= (uint8_t)~((uint8_t)(DAC_SWTRIGR_SWTRIG1 << DAC_Channel));
  }
}

/**
  * @brief  Enables or disables simultaneously the two DAC channels software
  *         triggers.
  * @param  NewState: new state of the DAC channels software triggers.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None.
  */
void DAC_DualSoftwareTriggerCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable software trigger for both DAC channels */
    DAC->SWTRIGR |= (DAC_SWTRIGR_SWTRIG1 | DAC_SWTRIGR_SWTRIG2) ;
  }
  else
  {
    /* Disable software trigger for both DAC channels */
    DAC->SWTRIGR &= (uint8_t)~(DAC_SWTRIGR_SWTRIG1 | DAC_SWTRIGR_SWTRIG2);
  }
}

/**
  * @brief  Enables or disables the selected DAC channel wave generation.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_Wave: Specifies the wave type to enable or disable.
  *          This parameter can be one of the following values:
  *            @arg DAC_Wave_Noise: noise wave generation
  *            @arg DAC_Wave_Triangle: triangle wave generation
  * @param  NewState: new state of the selected DAC channel wave generation.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DAC_WaveGenerationCmd(DAC_Channel_TypeDef DAC_Channel,
                           DAC_Wave_TypeDef DAC_Wave,
                           FunctionalState NewState)
{
  uint8_t tmpreg = 0;

  /* Check the DAC parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_DAC_WAVE(DAC_Wave));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Get the DAC CHxCR1 value & Clear WAVEN bits */
  tmpreg = (uint8_t)((*(uint8_t*)(uint16_t)(DAC_BASE + CR1_Offset + (uint8_t)((uint8_t)DAC_Channel << 1))) & (uint8_t)~(DAC_CR1_WAVEN));

  if (NewState != DISABLE)
  {
    tmpreg |= (uint8_t)(DAC_Wave);
  }

  /* Write to DAC CHxCR1 */
  (*(uint8_t*) (uint16_t)(DAC_BASE + CR1_Offset +  (uint8_t)((uint8_t)DAC_Channel << 1))) = tmpreg;

}

/**
  * @brief  Select DAC Noise Wave Generation LFSR according to the specified parameters.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_LFSRUnmask : the selected unmasked bit.
  *          This parameter can be one of the following values:
  *            @arg DAC_LFSRUnmask_Bit0: Noise LFSR Unmask 1 LSB
  *            @arg DAC_LFSRUnmask_Bits1_0: Noise LFSR Unmask 2 LSB  
  *            @arg DAC_LFSRUnmask_Bit2_0: Noise LFSR Unmask 3 LSB
  *            @arg DAC_LFSRUnmask_Bit3_0: Noise LFSR Unmask 4 LSB
  *            @arg DAC_LFSRUnmask_Bit4_0: Noise LFSR Unmask 5 LSB
  *            @arg DAC_LFSRUnmask_Bit5_0: Noise LFSR Unmask 6 LSB
  *            @arg DAC_LFSRUnmask_Bit6_0: Noise LFSR Unmask 7 LSB
  *            @arg DAC_LFSRUnmask_Bit7_0: Noise LFSR Unmask 8 LSB
  *            @arg DAC_LFSRUnmask_Bit8_0: Noise LFSR Unmask 9 LSB
  *            @arg DAC_LFSRUnmask_Bit9_0: Noise LFSR Unmask 10 LSB
  *            @arg DAC_LFSRUnmask_Bit10_0: Noise LFSR Unmask 11 LSB
  *            @arg DAC_LFSRUnmask_Bit11_0: Noise LFSR Unmask 12 LSB                    
  * @retval None
  */
void DAC_SetNoiseWaveLFSR(DAC_Channel_TypeDef DAC_Channel, DAC_LFSRUnmask_TypeDef DAC_LFSRUnmask)
{
  uint8_t tmpreg = 0;
  uint16_t cr2addr = 0;

  /* Check the DAC parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_DAC_LFSR_UNMASK_TRIANGLE_AMPLITUDE(DAC_LFSRUnmask));

  /* Get the DAC CHxCR2 value  &  Clear MAMPx bits */
  cr2addr = (uint16_t)(DAC_BASE + CR2_Offset + (uint8_t)((uint8_t)DAC_Channel << 1));
  tmpreg = (uint8_t)((*(uint8_t*)(cr2addr)) & (uint8_t)~(DAC_CR2_MAMPx));

  /* Write to DAC CHxCR2 */
  (*(uint8_t*)(cr2addr)) = (uint8_t)( tmpreg | DAC_LFSRUnmask);
}

/**
  * @brief  Select DAC Triangle Wave Generation Amplitude according to the specified parameters.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_TriangleAmplitude : the selected Amplitude
  *          This parameter can be one of the following values:
  *            @arg DAC_TriangleAmplitude_1: Triangle Amplitude = Vref.(1/4096)
  *            @arg DAC_TriangleAmplitude_3: Triangle Amplitude = Vref.(3/4096)  
  *            @arg DAC_TriangleAmplitude_7: Triangle Amplitude = Vref.(7/4096)
  *            @arg DAC_TriangleAmplitude_15: Triangle Amplitude = Vref.(15/4096)
  *            @arg DAC_TriangleAmplitude_31: Triangle Amplitude = Vref.(31/4096)
  *            @arg DAC_TriangleAmplitude_63: Triangle Amplitude = Vref.(63/4096)
  *            @arg DAC_TriangleAmplitude_127: Triangle Amplitude = Vref.(127/4096)
  *            @arg DAC_TriangleAmplitude_255: Triangle Amplitude = Vref.(255/4096)
  *            @arg DAC_TriangleAmplitude_511: Triangle Amplitude = Vref.(511/4096)
  *            @arg DAC_TriangleAmplitude_1023: Triangle Amplitude = Vref.(1023/4096)
  *            @arg DAC_TriangleAmplitude_2047: Triangle Amplitude = Vref.(2047/4096)
  *            @arg DAC_TriangleAmplitude_4095: Triangle Amplitude = Vref.(4095/4096) 
  * @retval None
  */
void DAC_SetTriangleWaveAmplitude(DAC_Channel_TypeDef DAC_Channel, DAC_TriangleAmplitude_TypeDef DAC_TriangleAmplitude)
{
  uint8_t tmpreg = 0;
  uint16_t cr2addr = 0;

  /* Check the DAC parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_DAC_LFSR_UNMASK_TRIANGLE_AMPLITUDE(DAC_TriangleAmplitude));


  /* Get the DAC CHxCR2 value  &  Clear MAMPx bits */
  cr2addr = (uint16_t)(DAC_BASE + CR2_Offset + (uint8_t)((uint8_t)DAC_Channel << 1));
  tmpreg = (uint8_t)((*(uint8_t*)(cr2addr)) & (uint8_t)~(DAC_CR2_MAMPx));

  /* Write to DAC CHxCR2 */
  (*(uint8_t*)(cr2addr)) = (uint8_t)( tmpreg | DAC_TriangleAmplitude);
}

/**
  * @brief  Set the specified data holding register value for DAC channel1.
  * @param  DAC_Align: Specifies the data alignment for DAC channel1.
  *          This parameter can be one of the following values:
  *            @arg DAC_Align_8b_R: 8bit right data alignment selected
  *            @arg DAC_Align_12b_L: 12bit left data alignment selected
  *            @arg DAC_Align_12b_R: 12bit right data alignment selected
  * @param  Data : Data to be loaded in the selected data holding register.
  * @retval None.
  */
void DAC_SetChannel1Data(DAC_Align_TypeDef DAC_Align, uint16_t DAC_Data)
{
  /* Check the parameters */
  assert_param(IS_DAC_ALIGN(DAC_Align));

  if (DAC_Align != DAC_Align_8b_R)
  {
    /* Set the DAC channel1 selected data holding register */
    *(uint8_t*)((uint16_t)(DAC_BASE + CH1RDHRH_Offset + DAC_Align )) = (uint8_t)(((uint16_t)DAC_Data) >> 8);
    *(uint8_t*)((uint16_t)(DAC_BASE + CH1RDHRH_Offset + 1 + DAC_Align )) = (uint8_t)DAC_Data;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_DAC_DATA_08R(DAC_Data));

    /* Set the DAC channel1 selected data holding register */
    DAC->CH1DHR8 = (uint8_t)(DAC_Data);
  }
}

/**
  * @brief  Set the specified data holding register value for DAC channel2.
  * @param  DAC_Align: Specifies the data alignment for DAC channel2.
  *          This parameter can be one of the following values:
  *            @arg DAC_Align_8b_R: 8bit right data alignment selected
  *            @arg DAC_Align_12b_L: 12bit left data alignment selected
  *            @arg DAC_Align_12b_R: 12bit right data alignment selected
  * @param  Data : Data to be loaded in the selected data holding register.
  * @retval None.
  */
void DAC_SetChannel2Data(DAC_Align_TypeDef DAC_Align, uint16_t DAC_Data)
{
  /* Check the parameters */
  assert_param(IS_DAC_ALIGN(DAC_Align));

  if (DAC_Align != DAC_Align_8b_R)
  {
    /* Set the DAC channel2 selected data holding register */
    *(uint8_t*)((uint16_t)(DAC_BASE + CH2RDHRH_Offset + DAC_Align )) = (uint8_t)(((uint16_t)DAC_Data) >> 8);
    *(uint8_t*)((uint16_t)(DAC_BASE + CH2RDHRH_Offset + 1 + DAC_Align )) = (uint8_t)DAC_Data;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_DAC_DATA_08R(DAC_Data));

    /* Set the DAC channel2 selected data holding register */
    DAC->CH2DHR8 = (uint8_t)(DAC_Data);
  }
}

/**
  * @brief  Set the specified data holding register value for dual channel DAC.
  * @param  DAC_Align: Specifies the data alignment for dual channel DAC.
  *          This parameter can be one of the following values:
  *            @arg DAC_Align_8b_R: 8bit right data alignment selected
  *            @arg DAC_Align_12b_L: 12bit left data alignment selected
  *            @arg DAC_Align_12b_R: 12bit right data alignment selected
  * @param  Data2: Data for DAC Channel2 to be loaded in the selected data 
  *         holding register.
  * @param  Data1: Data for DAC Channel1 to be loaded in the selected data 
  *         holding register.
  * @note In dual mode, a unique register access is required to write in both
  *       DAC channels at the same time.
  * @retval None.
  */
void DAC_SetDualChannelData(DAC_Align_TypeDef DAC_Align, uint16_t DAC_Data2, uint16_t DAC_Data1)
{
  uint16_t dchxrdhrhaddr = 0;

  /* Check the parameters */
  assert_param(IS_DAC_ALIGN(DAC_Align));

  if (DAC_Align != DAC_Align_8b_R)
  {
    /* Identify the DCHxRDHRH address*/
    dchxrdhrhaddr = (uint16_t)(DAC_BASE + DCH1RDHRH_Offset + DAC_Align);

    /* Set the DAC channels Dual data holding registers */
    *(uint8_t*)(uint16_t)dchxrdhrhaddr = (uint8_t)(((uint16_t)DAC_Data1) >> 8);
    *(uint8_t*)(uint16_t)(dchxrdhrhaddr + 1) = (uint8_t)DAC_Data1;
    *(uint8_t*)(uint16_t)(dchxrdhrhaddr + 2) = (uint8_t)(((uint16_t)DAC_Data2) >> 8);
    *(uint8_t*)(uint16_t)(dchxrdhrhaddr + 3) = (uint8_t)DAC_Data2;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_DAC_DATA_08R(DAC_Data1 | DAC_Data2));

    /* Set the DAC channels Dual data holding registers */
    DAC->DCH1DHR8 = (uint8_t)(DAC_Data1);
    DAC->DCH2DHR8 = (uint8_t)(DAC_Data2);
  }
}

/**
  * @brief  Returns the last data output value of the selected DAC channel.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @retval The selected DAC channel data output value.
  */
uint16_t DAC_GetDataOutputValue(DAC_Channel_TypeDef DAC_Channel)
{
  uint16_t outputdata = 0;
  uint16_t tmp = 0;

  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));

  if ( DAC_Channel ==  DAC_Channel_1)
  {
    /* Returns the DAC channel data output register value */
    tmp = (uint16_t)((uint16_t)DAC->CH1DORH << 8);
    outputdata = (uint16_t)(tmp | (DAC->CH1DORL));
  }
  else
  {
    /* Returns the DAC channel data output register value */
    tmp = (uint16_t)((uint16_t)DAC->CH2DORH << 8);
    outputdata = (uint16_t)(tmp | (DAC->CH2DORL));
  }

  /* return the selected DAC channel data output value.*/
  return (uint16_t)outputdata;
}

/**
  * @}
  */

/** @defgroup DAC_Group2 DMA management functions
 *  @brief   DMA management functions
 *
@verbatim   
 ===============================================================================
                          DMA management function
 ===============================================================================  

@endverbatim
  * @{
  */
  
/**
  * @brief  Enables or disables the specified DAC channel DMA request.
  *         When enabled DMA1 is generated when an external trigger occurs
  * @param  DAC_Channel: the selected DAC channel.
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  NewState: new state of the selected DAC channel DMA request.
  *          This parameter can be: ENABLE or DISABLE.
  *  The DAC channel1 (channel2) is mapped on DMA1 channel3 (channel1) which 
  *  must be already configured. 
  * @retval None
  */
void DAC_DMACmd(DAC_Channel_TypeDef DAC_Channel, FunctionalState NewState)
{
  uint16_t cr2addr = 0;

  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Find CHxCR2 register Address */
  cr2addr = DAC_BASE + CR2_Offset + (uint8_t)((uint8_t)DAC_Channel << 1);

  if (NewState != DISABLE)
  {
    /* Enable the selected DAC channel DMA request */
    (*(uint8_t*)(cr2addr)) |= DAC_CR2_DMAEN;
  }
  else
  {
    /* Disable the selected DAC channel DMA request */
    (*(uint8_t*)(cr2addr)) &= (uint8_t)~(DAC_CR2_DMAEN);
  }
}

/**
  * @}
  */

/** @defgroup DAC_Group3 Interrupts and flags management functions
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
  * @brief  Enables or disables the specified DAC interrupts.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_IT: specifies the DAC interrupt sources to be enabled or disabled. 
  *   This parameter can be the following values:
  *            @arg DAC_IT_DMAUDR: DMA underrun interrupt mask
  * @note The DMA underrun occurs when a second external trigger arrives before
  *       the acknowledgement for the first external trigger is received (first request).
  * @param  NewState: new state of the specified DAC interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DAC_ITConfig(DAC_Channel_TypeDef DAC_Channel, DAC_IT_TypeDef DAC_IT, FunctionalState NewState)
{
  uint16_t cr2addr = 0;

  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  assert_param(IS_DAC_IT(DAC_IT));

  /* Find CHxCR2 register Address */
  cr2addr = DAC_BASE + CR2_Offset + (uint8_t)((uint8_t)DAC_Channel << 1);

  if (NewState != DISABLE)
  {
    /* Enable the selected DAC interrupts */
    (*(uint8_t*)(cr2addr)) |=  (uint8_t)(DAC_IT);
  }
  else
  {
    /* Disable the selected DAC interrupts */
    (*(uint8_t*)(cr2addr)) &= (uint8_t)(~(DAC_IT));
  }
}

/**
  * @brief  Checks whether the specified DAC flag is set or not.
  * @param  DAC_Channel: thee selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_FLAG: specifies the flag to check. 
  *   This parameter can be only of the following value:
  *            @arg DAC_FLAG_DMAUDR: DMA underrun flag
  * @note The DMA underrun occurs when a second external trigger arrives before
  *       the acknowledgement for the first external trigger is received (first request).
  * @retval The new state of DAC_FLAG (SET or RESET).
  */
FlagStatus DAC_GetFlagStatus(DAC_Channel_TypeDef DAC_Channel, DAC_FLAG_TypeDef DAC_FLAG)
{
  FlagStatus flagstatus = RESET;
  uint8_t flag = 0;

  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_DAC_FLAG(DAC_FLAG));

  flag = (uint8_t)(DAC_FLAG << DAC_Channel);

  /* Check the status of the specified DAC flag */
  if ((DAC->SR & flag ) != (uint8_t)RESET)
  {
    /* DAC FLAG is set */
    flagstatus = SET;
  }
  else
  {
    /* DAC FLAG is reset */
    flagstatus = RESET;
  }

  /* Return the DAC FLAG status */
  return  flagstatus;
}

/**
  * @brief  Clears the DAC channel's pending flags.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_FLAG: specifies the flag to clear. 
  *   This parameter can be of the following value:
  *            @arg DAC_FLAG_DMAUDR: DMA underrun flag                          
  * @retval None
  */
void DAC_ClearFlag(DAC_Channel_TypeDef DAC_Channel, DAC_FLAG_TypeDef DAC_FLAG)
{
  uint8_t flag = 0;

  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_DAC_FLAG(DAC_FLAG));

  /* identify the selected flag*/
  flag = (uint8_t)(DAC_FLAG << DAC_Channel);

  /* Clear the selected DAC flag */
  DAC->SR = (uint8_t)(~flag);
}

/**
  * @brief  Checks whether the specified DAC interrupt has occurred or not.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_IT: specifies the DAC interrupt source to check. 
  *   This parameter can be the following values:
  *            @arg DAC_IT_DMAUDR: DMA underrun interrupt mask
  * @note The DMA underrun occurs when a second external trigger arrives before
  *       the acknowledgement for the first external trigger is received (first request).
  * @retval The new state of DAC_IT (SET or RESET).
  */
ITStatus DAC_GetITStatus(DAC_Channel_TypeDef DAC_Channel, DAC_IT_TypeDef DAC_IT)
{
  ITStatus itstatus = RESET;
  uint8_t enablestatus = 0;
  uint8_t flagstatus = 0;
  uint8_t tempreg = 0;

  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_DAC_IT(DAC_IT));

  /* identify the status of the IT and its correspondent flag*/
  tempreg = *(uint8_t*)(uint16_t)(DAC_BASE + CR2_Offset + (uint8_t)((uint8_t)DAC_Channel << 2));
  enablestatus = (uint8_t)( tempreg & (uint8_t)((uint8_t)DAC_IT << DAC_Channel));
  flagstatus = (uint8_t)(DAC->SR & (uint8_t)(DAC_IT >> ((uint8_t)0x05 - DAC_Channel)));

  /* Check the status of the specified DAC interrupt */
  if (((flagstatus) != (uint8_t)RESET) && enablestatus)
  {
    /* DAC IT is set */
    itstatus = SET;
  }
  else
  {
    /* DAC IT is reset */
    itstatus = RESET;
  }

  /* Return the DAC IT status */
  return  itstatus;
}

/**
  * @brief  Clears the DAC channel's interrupt pending bits.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_IT: specifies the DAC interrupt pending bit to clear.
  *   This parameter can be the following values:
  *            @arg DAC_IT_DMAUDR: DMA underrun interrupt mask                         
  * @retval None
  */
void DAC_ClearITPendingBit(DAC_Channel_TypeDef DAC_Channel, DAC_IT_TypeDef DAC_IT)
{
  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_DAC_IT(DAC_IT));

  /* Clear the selected DAC interrupt pending bits */
  DAC->SR = (uint8_t)~(uint8_t)((uint8_t)DAC_IT >> (0x05 - DAC_Channel));
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
