/**
  ******************************************************************************
  * @file    stm8l15x_clk.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the clock controller (CLK) peripheral:
  *           - Internal/external clocks, CSS and CCO configuration
  *           - System clocks configuration
  *           - Peripheral clocks configuration
  *           - CSS on LSE configuration 
  *           - Low power clock configuration  
  *           - Interrupts and flags management
  *
  *  @verbatim
  *               
  *          ===================================================================
  *                               CLK specific features
  *          ===================================================================
  *    
  *          After reset the device is running from Internal High Speed oscillator
  *          divided by 8 (HSI/8 = 2MHz) with all peripherals off.
  *           - The clock for all peripherals is switched off, except for the BootROM 
  *             clock used for bootloader, in which case, the software should be properly 
  *             written to switch off that clock after the bootloader execution.     
  *
  *          Once the device starts from reset, the user application has to:
  *           - Configure the clock source to be used to drive the System clock
  *             (if the application needs higher frequency/performance)
  *           - Configure the System clock frequency  
  *           - Enable the clock for the peripheral(s) to be used
  *           - Configure the clock source(s) for peripherals which clocks are not
  *             derived from the System clock (RTC/LCD, BEEP)
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

#include "stm8l15x_clk.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @defgroup CLK 
  * @brief CLK driver modules
  * @{
  */ 
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private Variables ---------------------------------------------------------*/
/* Private Constant ---------------------------------------------------------*/
CONST uint8_t SYSDivFactor[5] = {1, 2, 4, 8, 16}; /*!< Holds the different Master clock Divider factors */
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CLK_Private_Functions
  * @{
  */ 

/** @defgroup CLK_Group1 Internal and external clocks, CSS and CCO configuration functions
 *  @brief   Internal and external clocks, CSS and CCO configuration functions 
 *
@verbatim   
 ===============================================================================
      Internal/external clocks, CSS and CCO configuration functions
 ===============================================================================  

  This section provides functions allowing to configure the internal/external clocks,
  CSS and CCO pins.
  
  1. HSI (high-speed internal), 16 MHz factory-trimmed RC used directly as System 
     clock source.

  2. LSI (low-speed internal), 32 KHz low consumption RC used as IWDG and/or RTC
     and/or BEEP clock source.

  3. HSE (high-speed external), 1 to 16 MHz crystal oscillator used directly as 
     System clock source. Can be used also as RTC/LCD clock source.

  4. LSE (low-speed external), 32 KHz oscillator used as RTC/LCD and/or BEEP clock source.

  5. CSS (Clock security system), once enabled and if a HSE clock failure occurs 
     (HSE used as System clock source), the System clock is automatically switched
     to HSI and an interrupt is generated if enabled. 

  6. CCO (configurable clock output), used to output HSI, LSE, HSE, HSI, LSI
     clock (through a configurable prescaler) on PC4 pin.

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the CLK peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void CLK_DeInit(void)
{
  CLK->ICKCR = CLK_ICKCR_RESET_VALUE;
  CLK->ECKCR = CLK_ECKCR_RESET_VALUE;
  CLK->CRTCR = CLK_CRTCR_RESET_VALUE;
  CLK->CBEEPR = CLK_CBEEPR_RESET_VALUE;
  CLK->SWR  = CLK_SWR_RESET_VALUE;
  CLK->SWCR = CLK_SWCR_RESET_VALUE;
  CLK->CKDIVR = CLK_CKDIVR_RESET_VALUE;
  CLK->PCKENR1 = CLK_PCKENR1_RESET_VALUE;
  CLK->PCKENR2 = CLK_PCKENR2_RESET_VALUE;
  CLK->PCKENR3 = CLK_PCKENR3_RESET_VALUE;
  CLK->CSSR  = CLK_CSSR_RESET_VALUE;
  CLK->CCOR = CLK_CCOR_RESET_VALUE;
  CLK->HSITRIMR = CLK_HSITRIMR_RESET_VALUE;
  CLK->HSICALR = CLK_HSICALR_RESET_VALUE;
  CLK->HSIUNLCKR = CLK_HSIUNLCKR_RESET_VALUE;
  CLK->REGCSR = CLK_REGCSR_RESET_VALUE;
}

/**
  * @brief  Enables or disables the Internal High Speed oscillator (HSI).
  * @note   The HSI is stopped by hardware when entering Halt and active Halt modes.
  *         It is used (enabled by hardware) as system clock source after startup
  *         from Reset, wakeup from Halt and active Halt mode when the FHWU bit is
  *         set in the ICKCR register, or in case of HSE failure used as system clock
  *         (if the Clock Security System CSS is enabled).             
  * @note   HSI can not be stopped if it is used as active CCO source, as active 
  *         RTC clock, if the safe oscillator (AUX) is enabled or as system clock source,
  *         In this case, you have to select another source of the system clock 
  *         then stop the HSI.
  * @note   After enabling the HSI, the application software should wait on HSIRDY
  *         flag to be set indicating that HSI clock is stable and can be used as
  *         system clock source.  
  * @param  NewState: new state of the HSI.
    *         This parameter can be: ENABLE or DISABLE.
  * @note   When the HSI is stopped, HSIRDY flag goes low after 6 HSI oscillator
  *         clock cycles.  
  * @retval None
  */
void CLK_HSICmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Set HSION bit */
    CLK->ICKCR |= CLK_ICKCR_HSION;
  }
  else
  {
    /* Reset HSION bit */
    CLK->ICKCR &= (uint8_t)(~CLK_ICKCR_HSION);
  }
}

/**
  * @brief  Adjusts the Internal High Speed oscillator (HSI) calibration value.
  * @note   The calibration is used to compensate for the variations in voltage
  *         and temperature that influence the frequency of the internal HSI RC.  
  * @param  CLK_HSICalibrationValue : calibration trimming value.
  *         This parameter must be a number between [(HSICALR register value) -12]
  *         and [(HSICALR register value) + 8 ]  
  * @note   Once HSITRIMR register configured, its value is used instead of the HSICALR
  *         register values. 
  * @retval None
  */
void CLK_AdjustHSICalibrationValue(uint8_t CLK_HSICalibrationValue)
{
  /* two consecutive write access to HSIUNLCKR register to unlock HSITRIMR */
  CLK->HSIUNLCKR = 0xAC;
  CLK->HSIUNLCKR = 0x35;

  /* Store the new value */
  CLK->HSITRIMR = (uint8_t)CLK_HSICalibrationValue;
}

/**
  * @brief  Enables or disables the Internal Low Speed oscillator (LSI).
  * @note   After enabling the LSI, the application software should wait on 
  *         LSIRDY flag to be set indicating that LSI clock is stable and can
  *         be used to clock the IWDG and/or the RTC.
  * @note   LSI can not be disabled if used as system clock source, as active CCO 
  *         source, as BEEP clock source while BEEPAHALT bit is set or, as RTC active 
  *         clock source.     
  * @param  NewState: new state of the LSI.
  *          This parameter can be: ENABLE or DISABLE.
  * @note   When the LSI is stopped, LSIRDY flag goes low after 6 LSI oscillator
  *         clock cycles. 
  * @retval None
  */
void CLK_LSICmd(FunctionalState NewState)
{

  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Set LSION bit */
    CLK->ICKCR |= CLK_ICKCR_LSION;
  }
  else
  {
    /* Reset LSION bit */
    CLK->ICKCR &= (uint8_t)(~CLK_ICKCR_LSION);
  }
}

/**
  * @brief  Configures the External High Speed oscillator (HSE).
  * @note   After enabling the HSE (CLK_HSE_ON or CLK_HSE_Bypass), the application
  *         software should wait on HSERDY flag to be set indicating that HSE clock
  *         is stable and can be used to clock the system.
  * @note   HSE state can not be changed if it is used as system clock. In this case,
  *         you have to select another source of the system clock then change 
  *         the HSE state (ex. disable it).
  * @note   The HSE is stopped by hardware when entering HALT and active HALT modes.  
  * @param  CLK_HSE: specifies the new state of the HSE.
  *         This parameter can be one of the following values:
  *            @arg CLK_HSE_OFF: turn OFF the HSE oscillator, HSERDY flag goes low after
  *                              6 HSE oscillator clock cycles.
  *            @arg CLK_HSE_ON: turn ON the HSE oscillator
  *            @arg CLK_HSE_Bypass: HSE oscillator bypassed with external clock
  * @note   In case of Enabling HSE Bypass make sure that the HSE clock source is
  *         not used by the RTC, output or involved in a switching operation.
  * @retval None
  */
void CLK_HSEConfig(CLK_HSE_TypeDef CLK_HSE)
{
  /* Check the parameters */
  assert_param(IS_CLK_HSE(CLK_HSE));

  /* Reset HSEON and HSEBYP bits before configuring the HSE ------------------*/
  /* Reset HSEON bit */
  CLK->ECKCR &= (uint8_t)~CLK_ECKCR_HSEON;

  /* Reset HSEBYP bit */
  CLK->ECKCR &= (uint8_t)~CLK_ECKCR_HSEBYP;

  /* Configure HSE */
  CLK->ECKCR |= (uint8_t)CLK_HSE;
}

/**
  * @brief  Configures the External Low Speed oscillator (LSE).
  * @note   After enabling the LSE (CLK_LSE_ON or CLK_LSE_Bypass), the application
  *         software should wait on LSERDY flag to be set indicating that LSE clock
  *         is stable and can be used to clock the RTC.
  * @param  CLK_LSE: specifies the new state of the LSE.
  *         This parameter can be one of the following values:
  *            @arg CLK_LSE_OFF: turn OFF the LSE oscillator, LSERDY flag goes low after
  *                              6 LSE oscillator clock cycles.
  *            @arg CLK_LSE_ON: turn ON the LSE oscillator
  *            @arg CLK_LSE_Bypass: LSE oscillator bypassed with external clock
  * @note   In case of Enabling LSE Bypass make sure that the LSE clock source is
  *         not used by the RTC, output or involved in a switching operation.  
  * @retval None
  */
void CLK_LSEConfig(CLK_LSE_TypeDef CLK_LSE)
{
  /* Check the parameters */
  assert_param(IS_CLK_LSE(CLK_LSE));

  /* Reset LSEON and LSEBYP bits before configuring the LSE ------------------*/
  /* Reset LSEON bit */
  CLK->ECKCR &= (uint8_t)~CLK_ECKCR_LSEON;

  /* Reset LSEBYP bit */
  CLK->ECKCR &= (uint8_t)~CLK_ECKCR_LSEBYP;

  /* Configure LSE */
  CLK->ECKCR |= (uint8_t)CLK_LSE;

}

/**
  * @brief  Enables the Clock Security System.
  * @note   If a failure is detected on the HSE oscillator clock, this oscillator
  *         is automatically disabled and an interrupt is generated to inform the
  *         software about the failure allowing the MCU to perform rescue operations.
  * @note   Once CSS is enabled it cannot be disabled until the next reset.  
  * @param  None
  * @retval None
  */
void CLK_ClockSecuritySystemEnable(void)
{
  /* Set CSSEN bit */
  CLK->CSSR |= CLK_CSSR_CSSEN;
}

/**
  * @brief  Enables the Clock Security System deglitcher system.
  * @param  None
  * @retval None
  */
void CLK_ClockSecuritySytemDeglitchCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Set CSSDGON bit */
    CLK->CSSR |= CLK_CSSR_CSSDGON;
  }
  else
  {
    /* Reset CSSDGON  bit */
    CLK->CSSR &= (uint8_t)(~CLK_CSSR_CSSDGON);
  }
}

/**
  * @brief  Selects the clock source to output on CCO pin(PC4).
  * @note   PC4 should be configured output push-pull with the speed that matches 
  *         maximum output speed of the desired clock.
  * @param  CLK_CCOSource: specifies the clock source to output.
  *          This parameter can be one of the following values:
  *            @arg CLK_CCOSource_Off: No clock selected as CCO source
  *            @arg CLK_CCOSource_HSI: HSI clock selected as CCO source
  *            @arg CLK_CCOSource_LSI: LSI clock selected as CCO source
  *            @arg CLK_CCOSource_LSE: LSE clock selected as CCO source
  *            @arg CLK_CCOSource_HSE: HSE clock selected as CCO source
  * @param  CLK_CCODiv: specifies the CCO prescaler.
  *          This parameter can be one of the following values:
  *            @arg CLK_CCODiv_1: no division applied to CCO clock
  *            @arg CLK_CCODiv_2: division by 2 applied to CCO clock
  *            @arg CLK_CCODiv_4: division by 4 applied to CCO clock
  *            @arg CLK_CCODiv_8: division by 8 applied to CCO clock
  *            @arg CLK_CCODiv_16: division by 16 applied to CCO clock
  *            @arg CLK_CCODiv_32: division by 32 applied to CCO clock
  *            @arg CLK_CCODiv_64: division by 64 applied to CCO clock
  * @retval None
  */
void CLK_CCOConfig(CLK_CCOSource_TypeDef CLK_CCOSource, CLK_CCODiv_TypeDef CLK_CCODiv)
{
  /* check teh parameters */
  assert_param(IS_CLK_OUTPUT(CLK_CCOSource));
  assert_param(IS_CLK_OUTPUT_DIVIDER(CLK_CCODiv));

  /* Selects the source provided on cco_ck output and its divider*/
  CLK->CCOR = (uint8_t)((uint8_t)CLK_CCOSource | (uint8_t)CLK_CCODiv);
}

/**
  * @}
  */

/** @defgroup CLK_Group2 System clock configuration functions
 *  @brief   System clock configuration functions
 *
@verbatim   
 ===============================================================================
                    System clock configuration functions
 ===============================================================================  

  This section provides functions allowing to configure the System clock.
  
  1. Several clock sources can be used to drive the System clock (SYSCLK): HSI,
     HSE, LSI and LSE.
     You can use "CLK_GetClocksFreq()" function to retrieve the frequencies of these clocks.  

@note All the peripheral clocks are derived from the System clock (SYSCLK) except:
       - BEEP: the Beeper clock can be derived either from a LSE or LSI clock sources. 
          You have to use CLK_BEEPClockConfig() function to configure this clock. 
       - RTC: the RTC clock can be derived either from the LSI, LSE, HSI or HSE clock
          divided by 1 to 64. You have to use CLK_RTCClockConfig() functions to 
          configure this clock.
       - LCD : LCD clock is the RTC Clock divided by 2. 
       - IWDG clock which is always the LSI clock.
       
  2. The maximum frequency of the SYSCLK is 16 MHz.

@endverbatim
  * @{
  */
  
/**
  * @brief  Configures the system clock (SYSCLK).
  * @note   The HSI is used (enabled by hardware) as system clock source after
  *         startup from Reset, wake-up from Halt and active Halt modes, or in case
  *         of failure of the HSE used as system clock (if the Clock Security System CSS is enabled).
  * @note   A switch from one clock source to another occurs only if the target
  *         clock source is ready (clock stable after startup delay or PLL locked). 
  *         You can use CLK_GetSYSCLKSource() function to know which clock is
  *         currently used as system clock source. 
  * @param  CLK_SYSCLKSource: specifies the clock source used as system clock.
  *          This parameter can be one of the following values:
  *            @arg CLK_SYSCLKSource_HSI: HSI selected as system clock source
  *            @arg CLK_SYSCLKSource_HSE: HSE selected as system clock source
  *            @arg CLK_SYSCLKSource_LSI: LSI selected as system clock source
  *            @arg CLK_SYSCLKSource_LSE: LSE selected as system clock source
  * @retval None
  */
void CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_TypeDef CLK_SYSCLKSource)
{
  /* check teh parameters */
  assert_param(IS_CLK_SOURCE(CLK_SYSCLKSource));

  /* Selection of the target clock source */
  CLK->SWR = (uint8_t)CLK_SYSCLKSource;
}

/**
  * @brief  Returns the clock source used as system clock.
  * @param  None
  * @retval Clock used as System clock (SYSCLK) source.
  *         The returned value can be one of the following:
  *         - CLK_SYSCLKSource_HSI: HSI used as system clock
  *         - CLK_SYSCLKSource_LSI: LSI used as system clock
  *         - CLK_SYSCLKSource_HSE: HSE used as system clock
  *         - CLK_SYSCLKSource_LSE: LSE used as system clock
  */
CLK_SYSCLKSource_TypeDef CLK_GetSYSCLKSource(void)
{
  return ((CLK_SYSCLKSource_TypeDef)(CLK->SCSR));
}

/**
  * @brief  Returns the frequencies of different the SYSCLK
  * 
  * @note   The system frequency computed by this function is not the real 
  *         frequency in the chip. It is calculated based on the predefined 
  *         constant and the selected clock source:
  * @note     If SYSCLK source is HSI, function returns values based on HSI_VALUE(*)
  * @note     If SYSCLK source is HSE, function returns values based on HSE_VALUE(**)
  * @note     If SYSCLK source is LSE, function returns values based on LSE_VALUE(***) 
  * @note     If SYSCLK source is LSI, function returns values based on LSI_VALUE(****)
  * @note     (*) HSI_VALUE is a constant defined in stm8l15x.h file (default value
  *               16 MHz) but the real value may vary depending on the variations
  *               in voltage and temperature.
  * @note     (**) HSE_VALUE is a constant defined in stm8l15x.h file (default value
  *                16 MHz), user has to ensure that HSE_VALUE is same as the real
  *                frequency of the crystal used. Otherwise, this function may
  *                have wrong result.
  * @note     (***) LSI_VALUE is a constant defined in stm8l15x.h file (default value
  *               38 KHz) but the real value may vary depending on the variations
  *               in voltage and temperature.
  * @note     (****) LSE_VALUE is a constant defined in stm8l15x.h file (default value
  *                32,768 KHz), user has to ensure that LSE_VALUE is same as the real
  *                frequency of the crystal used. Otherwise, this function may
  *                have wrong result.
  *
  * @note   The result of this function could be not correct when using fractional
  *         value for HSE crystal.
  *   
  * @param  None
  *
  * @note   This function can be used by the user application to compute the 
  *         baudrate for the communication peripherals or configure other parameters.
  * @note   Each time SYSCLK clock changes, this function must be called to update
  *         the returned value. Otherwise, any configuration based on this 
  *         function will be incorrect.
  *
  * @retval System Clock frequency value
  */
uint32_t CLK_GetClockFreq(void)
{
  uint32_t clockfrequency = 0;
  uint32_t sourcefrequency = 0;
  CLK_SYSCLKSource_TypeDef clocksource = CLK_SYSCLKSource_HSI;
  uint8_t tmp = 0, presc = 0;

  /* Get SYSCLK source. */
  clocksource = (CLK_SYSCLKSource_TypeDef)CLK->SCSR;

  if ( clocksource == CLK_SYSCLKSource_HSI)
  {
    sourcefrequency = HSI_VALUE;
  }
  else if ( clocksource == CLK_SYSCLKSource_LSI)
  {
    sourcefrequency = LSI_VALUE;
  }
  else if ( clocksource == CLK_SYSCLKSource_HSE)
  {
    sourcefrequency = HSE_VALUE;
  }
  else
  {
    clockfrequency = LSE_VALUE;
  }

  /* Get System clock divider factor*/
  tmp = (uint8_t)(CLK->CKDIVR & CLK_CKDIVR_CKM);
  presc = SYSDivFactor[tmp];

  /* Get System clock frequency */
  clockfrequency = sourcefrequency / presc;

  return((uint32_t)clockfrequency);
}

/**
  * @brief  Configures the System clock (SYSCLK) dividers.
  * @param  CLK_SYSCLKDiv : Specifies the system clock divider to apply.
  *          This parameter can be one of the following values:
  *            @arg CLK_SYSCLKDiv_1 
  *            @arg CLK_SYSCLKDiv_2
  *            @arg CLK_SYSCLKDiv_4
  *            @arg CLK_SYSCLKDiv_8
  *            @arg CLK_SYSCLKDiv_16
  *            @arg CLK_SYSCLKDiv_64
  *            @arg CLK_SYSCLKDiv_128
  * @retval None
  */
void CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_TypeDef CLK_SYSCLKDiv)
{
  /* check the parameters */
  assert_param(IS_CLK_SYSTEM_DIVIDER(CLK_SYSCLKDiv));

  CLK->CKDIVR = (uint8_t)(CLK_SYSCLKDiv);
}

/**
  * @brief  Enables or disables the clock switch execution.
  * @param  NewState : new state of clock switch, value accepted ENABLE, DISABLE.
  * @retval None
  */
void CLK_SYSCLKSourceSwitchCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Set SWEN bit */
    CLK->SWCR |= CLK_SWCR_SWEN;
  }
  else
  {
    /* Reset SWEN  bit */
    CLK->SWCR &= (uint8_t)(~CLK_SWCR_SWEN);
  }
}

/**
  * @}
  */

/** @defgroup CLK_Group3 Peripheral clocks configuration functions
 *  @brief   Peripheral clocks configuration functions 
 *
@verbatim   
 ===============================================================================
                   Peripheral clocks configuration functions
 ===============================================================================  

  This section provides functions allowing to configure the Peripheral clocks. 
  
  1. The RTC clock which is derived from the LSI, LSE, HSI or HSE clock divided by 1 to 64.
     
  2. The BEEP clock which is derived from the LSI or LSE clocks.

  3. After restart from Reset or wakeup from HALT, all peripherals are off
     Before to start using a peripheral you have to enable its interface clock. 
     You can do this using CLK_AHBPeriphClockCmd()
     , CLK_APB2PeriphClockCmd() and CLK_APB1PeriphClockCmd() functions.
     
  4. To reset the peripherals configuration (to the default state after device reset)
     you can use CLK_PeripheralClockConfig() function.  

@endverbatim
  * @{
  */

/**
  * @brief  Configures the RTC clock (RTCCLK).
  * @param  CLK_RTCCLKSource: specifies the RTC clock source.
  *          This parameter can be one of the following values:
  *            @arg CLK_RTCCLKSource_Off: RTC clock Off  
  *            @arg CLK_RTCCLKSource_LSE: LSE selected as RTC clock
  *            @arg CLK_RTCCLKSource_LSI: LSI selected as RTC clock
  *            @arg CLK_RTCCLKSource_HSE: HSE selected as RTC clock
  *            @arg CLK_RTCCLKSource_HSI: HSI selected as RTC clock  
  *  
  * @param  CLK_RTCCLKDiv: specifies the RTC clock source divider.
  *          This parameter can be one of the following values:
  *            @arg CLK_RTCCLKDiv_1: Clock RTC Div 1  
  *            @arg CLK_RTCCLKDiv_2: Clock RTC Div 2
  *            @arg CLK_RTCCLKDiv_4: Clock RTC Div 4
  *            @arg CLK_RTCCLKDiv_8: Clock RTC Div 8
  *            @arg CLK_RTCCLKDiv_16: Clock RTC Div 16
  *            @arg CLK_RTCCLKDiv_32: Clock RTC Div 32
  *            @arg CLK_RTCCLKDiv_64: Clock RTC Div 64
  *  
  * @note   If the LSE or LSI is used as RTC clock source, the RTC continues to
  *         work in HALT and Active HALT modes, and can be used as wakeup source.
  *         However, when the HSE clock is used as RTC clock source.    
  * @note   The maximum input clock frequency for RTC is 1MHz (when using HSE/HSI as
  *         RTC clock source).
  *  
  * @retval None
  */
void CLK_RTCClockConfig(CLK_RTCCLKSource_TypeDef CLK_RTCCLKSource, CLK_RTCCLKDiv_TypeDef CLK_RTCCLKDiv)
{
  /* check the parameters */
  assert_param(IS_CLK_CLOCK_RTC(CLK_RTCCLKSource));
  assert_param(IS_CLK_CLOCK_RTC_DIV(CLK_RTCCLKDiv));

  /* Selects the source provided on to RTC and its divider*/
  CLK->CRTCR = (uint8_t)((uint8_t)CLK_RTCCLKSource | (uint8_t)CLK_RTCCLKDiv);
}

/**
  * @brief  Configures the BEEP clock (BEEPCLK).
  * @param  CLK_BEEPCLKSource: specifies the BEEP clock source.
  *          This parameter can be one of the following values:
  *            @arg CLK_BEEPCLKSource_Off: BEEP clock Off  
  *            @arg CLK_BEEPCLKSource_LSE: LSE selected as BEEP clock
  *            @arg CLK_BEEPCLKSource_LSI: LSI selected as BEEP clock  
  * @retval None
  */
void CLK_BEEPClockConfig(CLK_BEEPCLKSource_TypeDef CLK_BEEPCLKSource)
{
  /* check the parameters */
  assert_param(IS_CLK_CLOCK_BEEP(CLK_BEEPCLKSource));

  /* Selects the source provided to BEEP*/
  CLK->CBEEPR = (uint8_t)(CLK_BEEPCLKSource);

}

/**
* @brief  Enables or disables the specified peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.   
  * @param  CLK_Peripheral: specifies the peripheral to gate its clock.
  *          This parameter can be any combination of the following values:
  *            @arg CLK_Peripheral_TIM2:       TIM2 clock
  *            @arg CLK_Peripheral_TIM3:       TIM3 clock 
  *            @arg CLK_Peripheral_TIM4:       TIM4 clock
  *            @arg CLK_Peripheral_I2C1:       I2C1 clock
  *            @arg CLK_Peripheral_SPI1:       SPI1 clock
  *            @arg CLK_Peripheral_USART1:     USART1 clock
  *            @arg CLK_Peripheral_BEEP:       BEEP clock
  *            @arg CLK_Peripheral_DAC:        DAC clock
  *            @arg CLK_Peripheral_ADC1:       ADC1 clock
  *            @arg CLK_Peripheral_TIM1:       TIM1 clock
  *            @arg CLK_Peripheral_RTC:        RTC clock
  *            @arg CLK_Peripheral_LCD:        LCD clock
  *            @arg CLK_Peripheral_DMA1:       DMA1 clock
  *            @arg CLK_Peripheral_COMP:       COMP clock
  *            @arg CLK_Peripheral_BOOTROM:    BOOTROM clock
  *            @arg CLK_Peripheral_AES:        AES clock
  *            @arg CLK_Peripheral_TIM5:       TIM5 clock
  *            @arg CLK_Peripheral_SPI2:       SPI2 clock
  *            @arg CLK_Peripheral_USART2:     USART2 clock
  *            @arg CLK_Peripheral_USART3:     USART3 clock
  *            @arg CLK_Peripheral_CSSLSE:     CSS on LSE clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CLK_PeripheralClockConfig(CLK_Peripheral_TypeDef CLK_Peripheral, FunctionalState NewState)
{
  uint8_t reg = 0;

  /* Check the parameters */
  assert_param(IS_CLK_PERIPHERAL(CLK_Peripheral));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* get flag register */
  reg = (uint8_t)((uint8_t)CLK_Peripheral & (uint8_t)0xF0);

  if ( reg == 0x00)
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR1 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR1 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
  else if (reg == 0x10)
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR2 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR2 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
  else
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR3 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR3 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
}

/**
  * @}
  */
  
/** @defgroup CLK_Group4 CSS on LSE configuration functions
 *  @brief   CSS on LSE configuration functions 
 *
@verbatim   
 ===============================================================================
                        CSS on LSE configuration functions
 ===============================================================================  

  This section provides functions allowing to configure the CSS on LSE capability. 
  
  1. The LSE crystal clock source failures can be monitored when used as RTC clock
     by the mean of the LSI oscillator.
     
  2. The CSS on LSE is a feature implemented externally to RTC peripheral and though 
     has no impact the clock controller registers.

  3. To enable the CSS on LSE you can use CLK_LSEClockSecuritySystemEnable() function
     
  4. To configure the action to perform at RTC clock failure you can use 
     CLK_RTCCLKSwitchOnLSEFailureEnable() function that allows to switch the RTC clock
     from LSE to LSI.

@endverbatim
  * @{
  */

/**
  * @brief  Enables the clock CSS on LSE.
  * @note   Once Enabled, only POR can Disable it.
  * @param  None
  * @retval None
  */
void CLK_LSEClockSecuritySystemEnable(void)
{
  /* Set CSSEN bit */
  CSSLSE->CSR |= CSSLSE_CSR_CSSEN;
}

/**
  * @brief  Enables RTC clock switch to LSI in case of LSE failure.
  * @note   Once Enabled, only POR can Disable it.
  * @param  None
  * @retval None
  */
void CLK_RTCCLKSwitchOnLSEFailureEnable(void)
{
  /* Set SWITCHEN bit */
  CSSLSE->CSR |= CSSLSE_CSR_SWITCHEN;
}

/**
  * @}
  */
  
/** @defgroup CLK_Group5 Low power clock configuration functions
 *  @brief   Low power clock configuration functions 
 *
@verbatim   
 ===============================================================================
                      Low power clock configuration functions
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Configures clock during halt and active halt modes.
  * @param  CLK_Halt : Specifies the clock state and wake-up mode from halt way.
  *         This parameter can be a value of @ref CLK_Halt_TypeDef.
  * @param  NewState : Specifies the System clock (SYSCLK) state in active halt mode.
  *         This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void CLK_HaltConfig(CLK_Halt_TypeDef CLK_Halt, FunctionalState NewState)
{
  /* check the parameters */
  assert_param(IS_CLK_HALT(CLK_Halt));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    CLK->ICKCR |= (uint8_t)(CLK_Halt);
  }
  else
  {
    CLK->ICKCR &= (uint8_t)(~CLK_Halt);
  }
}

/**
  * @brief  Configures the main voltage regulator
  * @param  NewState: specifies the MVR  state.
  *         This parameter can be one of the following values:
  *         - DISABLE: MVR disabled;
  *         - ENABLE:  MVR enabled.
  * @retval None
  */
void CLK_MainRegulatorCmd(FunctionalState NewState)
{
  /* check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Reset REGUOFF bit */
    CLK->REGCSR &= (uint8_t)(~CLK_REGCSR_REGOFF);
  }
  else
  {
    /* Set REGUOFF bit */
    CLK->REGCSR |= CLK_REGCSR_REGOFF;
  }
}

/**
  * @}
  */

/** @defgroup CLK_Group6 Interrupts and flags management functions
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
  * @brief  Enables or disables the specified CLK interrupts.
  * @param  CLK_IT: specifies the CLK interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg CLK_IT_CSSD: Clock security system detection interrupt
  *            @arg CLK_IT_SWIF: Clock switch interrupt 
  *            @arg CLK_IT_LSECSSF: LSE Clock security system detection interrupt
  * @param  NewState: new state of the specified CLK interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CLK_ITConfig(CLK_IT_TypeDef CLK_IT, FunctionalState NewState)
{

  /* check the parameters */
  assert_param(IS_CLK_IT(CLK_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    if (CLK_IT == CLK_IT_SWIF)
    {
      /* Enable the clock switch interrupt */
      CLK->SWCR |= CLK_SWCR_SWIEN;
    }
    else if (CLK_IT == CLK_IT_LSECSSF)
    {
      /* Enable the CSS on LSE  interrupt */
      CSSLSE->CSR |= CSSLSE_CSR_CSSIE;
    }
    else
    {
      /* Enable the clock security system detection interrupt */
      CLK->CSSR |= CLK_CSSR_CSSDIE;
    }
  }
  else  /*(NewState == DISABLE)*/
  {
    if (CLK_IT == CLK_IT_SWIF)
		{
      /* Disable the clock switch interrupt */
      CLK->SWCR  &= (uint8_t)(~CLK_SWCR_SWIEN);
    }
    else if (CLK_IT == CLK_IT_LSECSSF)
    {
      /* Disable the CSS on LSE  interrupt */
      CSSLSE->CSR &= (uint8_t)(~CSSLSE_CSR_CSSIE);
    }
    else
    {
      /* Disable the clock security system detection interrupt */
      CLK->CSSR &= (uint8_t)(~CLK_CSSR_CSSDIE);
    }
  }
}

/**
  * @brief  Checks whether the specified CLK flag is set or not.
  * @param  CLK_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg CLK_FLAG_LSIRDY: LSI oscillator clock ready
  *            @arg CLK_FLAG_HSIRDY: HSI oscillator clock ready
  *            @arg CLK_FLAG_HSERDY: HSE oscillator clock ready
  *            @arg CLK_FLAG_SWBSY: Switch busy
  *            @arg CLK_FLAG_CSSD: Clock security system detection
  *            @arg CLK_FLAG_AUX: Auxiliary oscillator connected to master clock
  *            @arg CLK_FLAG_LSERDY: LSE oscillator clock ready
  *            @arg CLK_FLAG_CCOBSY: Configurable clock output busy
  *            @arg CLK_FLAG_RTCSWBSY: RTC clock busy in switch
  *            @arg CLK_FLAG_EEREADY: Flash program memory and Data EEPROM ready
  *            @arg CLK_FLAG_EEBUSY: Flash program memory and Data EEPROM busy
  *            @arg CLK_FLAG_LSEPD: LSE power-down
  *            @arg CLK_FLAG_LSIPD: LSI power-down
  *            @arg CLK_FLAG_HSEPD: HSE power-down
  *            @arg CLK_FLAG_HSIPD: HSI power-down
  *            @arg CLK_FLAG_REGREADY: REGREADY
  *            @arg CLK_FLAG_BEEPSWBSY: BEEP clock busy in switch
  *            @arg CLK_FLAG_LSECSSF: CSS on LSE detection
  *            @arg CLK_FLAG_RTCCLKSWF: RTCCLK switch completed on LSE failure
  * @retval The new state of CLK_FLAG (SET or RESET).
  */
FlagStatus CLK_GetFlagStatus(CLK_FLAG_TypeDef CLK_FLAG)
{
  uint8_t reg = 0;
  uint8_t pos = 0;
  FlagStatus bitstatus = RESET;

  /* check the parameters */
  assert_param(IS_CLK_FLAGS(CLK_FLAG));

  /* get flag register */
  reg = (uint8_t)((uint8_t)CLK_FLAG & (uint8_t)0xF0);

  /* get flag position */
  pos = (uint8_t)((uint8_t)CLK_FLAG & (uint8_t)0x0F);

  if (reg == 0x00) /* The flag to check is in CRTC Rregister */
  {
    reg = CLK->CRTCR;
  }
  else if (reg == 0x10) /* The flag to check is in ICKCR register */
  {
    reg = CLK->ICKCR;
  }
  else if (reg == 0x20) /* The flag to check is in CCOR register */
  {
    reg = CLK->CCOR;
  }
  else if (reg == 0x30) /* The flag to check is in ECKCR register */
  {
    reg = CLK->ECKCR;
  }
  else if (reg == 0x40) /* The flag to check is in SWCR register */
  {
    reg = CLK->SWCR;
  }
  else if (reg == 0x50) /* The flag to check is in CSSR register */
  {
    reg = CLK->CSSR;
  }
  else if (reg == 0x70) /* The flag to check is in REGCSR register */
  {
    reg = CLK->REGCSR;
  }
  else if (reg == 0x80) /* The flag to check is in CSSLSE_CSRregister */
  {
    reg = CSSLSE->CSR;
  }
  else /* The flag to check is in CBEEPR register */
  {
    reg = CLK->CBEEPR;
  }


  if ((reg & (uint8_t)((uint8_t)1 << (uint8_t)pos)) != (uint8_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }

  /* Return the flag status */
  return((FlagStatus)bitstatus);
}

/**
  * @brief  Clears the CSS LSE Flag.
  * @param  None
  * @retval None
  */
void CLK_ClearFlag(void)
{

  /* Clear the clock security system on LSE  detection Flag */
  CSSLSE->CSR &= (uint8_t)(~CSSLSE_CSR_CSSF);
}

/**
  * @brief  Checks whether the specified CLK interrupt has occurred or not.
  * @param  CLK_IT: specifies the CLK interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg CLK_IT_SWIF: LSI ready interrupt
  *            @arg CLK_IT_LSECSSF: LSE ready interrupt
  *            @arg CLK_IT_CSSD: HSI ready interrupt
  * @retval The new state of CLK_IT (SET or RESET).
  */
ITStatus CLK_GetITStatus(CLK_IT_TypeDef CLK_IT)
{

  ITStatus bitstatus = RESET;

  /* check the parameters */
  assert_param(IS_CLK_IT(CLK_IT));

  if (CLK_IT == CLK_IT_SWIF)
  {
    /* Check the status of the clock switch interrupt */
    if ((CLK->SWCR & (uint8_t)CLK_IT) == (uint8_t)0x0C)
    {
      bitstatus = SET;
    }
    else
    {
      bitstatus = RESET;
    }
  }
  else if (CLK_IT == CLK_IT_LSECSSF)
  {
    /* Check the status of the clock security system on LSE interrupt */
    if ((CSSLSE->CSR & (uint8_t)CLK_IT) == (uint8_t)0x0C)
    {
      bitstatus = SET;
    }
    else
    {
      bitstatus = RESET;
    }
  }
  else /* CLK_IT == CLK_IT_CSSD */
  {
    /* Check the status of the security system detection interrupt */
    if ((CLK->CSSR & (uint8_t)CLK_IT) == (uint8_t)0x0C)
    {
      bitstatus = SET;
    }
    else
    {
      bitstatus = RESET;
    }
  }

  /* Return the CLK_IT status */
  return bitstatus;
}

/**
  * @brief  Clears the CLK's interrupt pending bits.
  * @param  CLK_IT: specifies the interrupt pending bit to clear.
  *          This parameter can be any combination of the following values:
  *            @arg CLK_IT_SWIF: Clock switch interrupt
  *            @arg CLK_IT_LSECSSF: LSE Clock security system detection interrupt
  * @retval None
  */
void CLK_ClearITPendingBit(CLK_IT_TypeDef CLK_IT)
{

  /* check the parameters */
  assert_param(IS_CLK_CLEAR_IT(CLK_IT));

  if ((uint8_t)((uint8_t)CLK_IT & (uint8_t)0xF0) == (uint8_t)0x20)
  {
    /* Clear the status of the clock security system on LSE interrupt */
    CSSLSE->CSR &= (uint8_t)(~CSSLSE_CSR_CSSF);
  }
  else
  {
    /* Clear the status of the clock switch interrupt */
    CLK->SWCR &= (uint8_t)(~CLK_SWCR_SWIF);
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
