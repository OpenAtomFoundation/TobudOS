/**
  ******************************************************************************
  * @file    stm8l15x_rtc.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Real-Time Clock (RTC) peripheral:           
  *           - Initialization
  *           - Calendar (Time and Date) configuration
  *           - Alarm configuration
  *           - WakeUp Timer configuration
  *           - Daylight Saving configuration
  *           - Output pin Configuration
  *           - Smooth Calibration configuration
  *           - Tampers configuration
  *           - Output Type configuration
  *           - Interrupts and flags management       
  *                     
  *  @verbatim
  *       
  *          ===================================================================      
  *                                    RTC Domain Reset
  *          ===================================================================          
  *          After power-on reset, the RTC domain (RTC registers) is reset.        
  *         
  *          ===================================================================      
  *                                  RTC Operating Condition      
  *          ===================================================================            
  *          As long as the supply voltage remains in the operating range, 
  *          the RTC never stops, regardless of the device status (Run mode, 
  *          low power modes or under reset).
  *              
  *          ===================================================================      
  *                                   RTC Domain Access     
  *          ===================================================================         
  *          After reset, the RTC domain (RTC registers) is protected against 
  *          possible stray write accesses. 
  *          To enable access to the RTC registers, proceed as follows:
  *            - Select the RTC clock source using the CLK_RTCClockConfig()
  *              function : HSE, HSI, LSE or LSI.
  *              CLK_RTCClockConfig(CLK_RTCCLKSource_HSI, CLK_RTCCLKDiv_1)    
  *            - Enable RTC Clock using the CLK_PeripheralClockConfig() function 
  *               : CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE).
  *              
  *          ===================================================================      
  *                                   RTC Driver: how to use it
  *          ===================================================================          
  *            - Enable the RTC domain access (see description in the section above)
  *            - Configure the RTC Prescaler (Asynchronous and Synchronous) and
  *              RTC hour format using the RTC_Init() function.
  *                
  *          Time and Date configuration
  *          ===========================    
  *            - To configure the RTC Calendar (Time and Date) use the RTC_SetTime()
  *              and RTC_SetDate() functions.
  *            - To read the RTC Calendar, use the RTC_GetTime() and RTC_GetDate()
  *              functions.
  *            - Use the RTC_DayLightSavingConfig() function to add or sub 1hour
  *              to the RTC Calendar.    
  *                
  *          Alarm configuration
  *          ===================    
  *            - To configure the RTC Alarm use the RTC_SetAlarm() function.
  *            - Enable the selected RTC Alarm using the RTC_AlarmCmd() function  
  *            - To read the RTC Alarm, use the RTC_GetAlarm() function.
  *              
  *          RTC Wakeup configuration
  *          ========================    
  *            - Configure the RTC Wakeup Clock source use the RTC_WakeUpClockConfig()
  *              function.
  *            - Configure the RTC WakeUp Counter using the RTC_SetWakeUpCounter() 
  *              function  
  *            - Enable the RTC WakeUp using the RTC_WakeUpCmd() function  
  *            - To read the RTC WakeUp Counter register, use the RTC_GetWakeUpCounter() 
  *              function.
  *                
  *          Outputs configuration
  *          =====================  
  *          The RTC has 2 different outputs:
  *            - AFO_ALARM: this output is used to manage the RTC Alarm and 
  *              WaKeUp signals.          
  *              To output the selected RTC signal on RTC_ALARM pin, use the 
  *              RTC_OutputConfig() function.                
  *            - AFO_CALIB: this output is used to manage the RTC Clock divided 
  *              by 64 (512Hz) signal or divided by 32768 (1Hz) which can be 
  *              configured using RTC_CalibOutputConfig() function.
  *              To output the RTC Clock on RTC_CALIB pin, use the 
  *              RTC_CalibOutputCmd() function.                
  *                                                           
  *          Smooth Calibration configuration                    
  *          =================================    
  *            - Configure the RTC Smooth Calibration Value and the corresponding
  *              sign using the RTC_SmoothCalibConfig() function.

  *                
  *
  *          Tamper configuration                     
  *          ====================    
  *            - Configure the RTC Tamper Level using the RTC_TamperConfig() 
  *              function.
  *            - Enable the RTC Tamper using the RTC_TamperCmd() function.
  *
  *                                  
  *          ===================================================================      
  *                                  RTC and low power modes
  *          =================================================================== 
  *           The MCU can be woken up from a low power mode by an RTC alternate 
  *           function.
  *           The RTC alternate functions are the RTC alarms, 
  *           RTC wakeup and RTC tamper event detection.
  *           These RTC alternate functions can wake up the system from the 
  *           lowpower modes.
  *           The system can also wake up from low power modes without depending 
  *           on an external interrupt (Auto-wakeup mode), by using the RTC alarm 
  *           or the RTC wakeup events.
  *           The RTC provides a programmable time base for waking up from the 
  *           halt or wfe or wfi modes at regular intervals.
  *           
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
#include "stm8l15x_rtc.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
  
/** @defgroup RTC 
  * @brief RTC driver modules
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define INIT_TIMEOUT       ((uint16_t)0xFFFF)
#define RSF_TIMEOUT        ((uint16_t)0xFFFF)
#define INITF_TIMEOUT      ((uint16_t)0xFFFF)
#define WUTWF_TIMEOUT      ((uint16_t)0xFFFF)
#define ALRAWF_TIMEOUT     ((uint16_t)0xFFFF)
#define RECALPF_TIMEOUT    ((uint16_t)0xFFFF)
#define SHPF_TIMEOUT       ((uint16_t)0xFFFF)

#define TEN_VALUE_BCD      ((uint8_t)0x10)
#define TEN_VALUE_BIN      ((uint8_t)0x0A)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/** @defgroup RTC_Private_Functions_Prototype
  * @{
  */
static uint8_t ByteToBcd2(uint8_t Value);
static uint8_t Bcd2ToByte(uint8_t Value);

/**
 * @}
 */

/** @defgroup RTC_Private_Functions
  * @{
  */

/** @defgroup RTC_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim   
 ===============================================================================
                 Initialization and Configuration functions
 ===============================================================================  

  This section provide functions allowing to initialize and configure the RTC
  Prescaler (Synchronous and Asynchronous), RTC Hour format, disable RTC registers
  Write protection, enter and exit the RTC initialization mode, RTC registers
  synchronization check and reference clock detection enable.
  
  1. The RTC Prescaler is programmed to generate the RTC 1Hz time base. It is
     split into 2 programmable prescaler to minimize power consumption.
     - A 7-bit asynchronous prescaler and A 13-bit or 15-bit synchronous 
       prescaler depending to density of the mcu package.
     - When both prescaler are used, it is recommended to configure the 
       asynchronous prescaler to a high value to minimize consumption.

  2. All RTC registers are Write protected. Writing to the RTC registers
     is enabled by writing a key into the Write Protection register, RTC_WPR.

  3. To Configure the RTC Calendar, user application should enter initialization
     mode. In this mode, the Calendar counter is stopped and its value can be 
     updated. When the initialization sequence is complete, the Calendar restarts 
     counting after 4 RTCCLK cycles.

  4. To read the Calendar through the shadow registers after Calendar initialization,
     Calendar update or after wakeup from low power modes the software must first 
     clear the RSF flag. The software must then wait until it is set again before 
     reading the Calendar, which means that the Calendar registers have been 
     correctly copied into the RTC_TR and RTC_DR shadow registers.
     The RTC_WaitForSynchro() function implements the above software sequence 
     (RSF clear and RSF check).

@endverbatim
  * @{
  */
/**
* @brief  Deinitializes the RTC registers to their default reset values.
* @note   This function doesn't reset the RTC Clock source.  
* @param  None
* @retval An ErrorStatus enumeration value:
*          - SUCCESS: RTC registers are deinitialized
*          - ERROR: RTC registers are not deinitialized
*/
ErrorStatus RTC_DeInit(void)
{
  ErrorStatus status = ERROR;
  uint16_t wutwfcount = 0;
  uint16_t recalpfcount = 0;

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Set Initialization mode */
  if (RTC_EnterInitMode() == ERROR)
  {
    status = ERROR;
    /* Enable the write protection for RTC registers */
    RTC->WPR = 0xFF; 
  }
  else
  {
    /* Reset TR registers */
    RTC->TR1 = RTC_TR1_RESET_VALUE;
    RTC->TR2 = RTC_TR2_RESET_VALUE;
    RTC->TR3 = RTC_TR3_RESET_VALUE;

    /* Reset DR registers */
    RTC->DR1 = RTC_DR1_RESET_VALUE;
    RTC->DR2 = RTC_DR2_RESET_VALUE;
    RTC->DR3 = RTC_DR3_RESET_VALUE;

    /* Reset SPER & ARPER registers */
    RTC->SPRERH = RTC_SPRERH_RESET_VALUE;
    RTC->SPRERL = RTC_SPRERL_RESET_VALUE;
    RTC->APRER  = RTC_APRER_RESET_VALUE;

    RTC->TCR1 = RTC_TCR1_RESET_VALUE;
    RTC->TCR2 = RTC_TCR2_RESET_VALUE;

    /* Reset All CR1 bits except CR1[2:0] */

    RTC->CR1 = RTC_CR1_RESET_VALUE;
    RTC->CR2 = RTC_CR2_RESET_VALUE;
    RTC->CR3 = RTC_CR3_RESET_VALUE;

    /* Wait till RTC WUTWF flag is set or if Time out is reached exit */
    while (((RTC->ISR1 & RTC_ISR1_WUTWF) == RESET) && ( wutwfcount != WUTWF_TIMEOUT))
    {
      wutwfcount++;
      RTC->ISR1 = 0;
    }

    if ((RTC->ISR1 & RTC_ISR1_WUTWF) == RESET)
    {
      status = ERROR;
      /* Enable the write protection for RTC registers */
      RTC->WPR = 0xFF; 
    }
    else
    {
      /* Reset All CR1 bits */
      RTC->CR1 = RTC_CR1_RESET_VALUE;

      /* Reset WUTR registers */
      RTC->WUTRH = RTC_WUTRH_RESET_VALUE;
      RTC->WUTRL = RTC_WUTRL_RESET_VALUE;

      /* Reset ALARM registers */
      RTC->ALRMAR1 = RTC_ALRMAR1_RESET_VALUE;
      RTC->ALRMAR2 = RTC_ALRMAR2_RESET_VALUE;
      RTC->ALRMAR3 = RTC_ALRMAR3_RESET_VALUE;
      RTC->ALRMAR4 = RTC_ALRMAR4_RESET_VALUE;

      RTC->ALRMASSRH = RTC_ALRMASSRH_RESET_VALUE;
      RTC->ALRMASSRL = RTC_ALRMASSRL_RESET_VALUE;
      RTC->ALRMASSMSKR = RTC_ALRMASSMSKR_RESET_VALUE;

      /* Reset ISR register and exit initialization mode */
      RTC->ISR1 = (uint8_t)0x00;
      RTC->ISR2 = RTC_ISR2_RESET_VALUE;

      if ((RTC->ISR1 & RTC_ISR1_RECALPF) != RESET)
      {
        while (((RTC->ISR1 & RTC_ISR1_RECALPF) != RESET) && (recalpfcount != RECALPF_TIMEOUT))
        {
          recalpfcount++;
        }
      }
      if ((RTC->ISR1 & RTC_ISR1_RECALPF) == RESET)
      {
        RTC->CALRH = RTC_CALRH_RESET_VALUE;
        RTC->CALRL = RTC_CALRL_RESET_VALUE;

        if (RTC_WaitForSynchro() == ERROR)
        {
          status = ERROR;
        }
        else
        {
          status = SUCCESS;
        }
      }
      else
      {
        status = ERROR;
      }

      /* Enable the write protection for RTC registers */
      RTC->WPR = 0xFF; 
    }
  }

  /* return Deinitialize RTC registers status*/
  return (ErrorStatus)status;
}



/**
* @brief  Initializes the RTC registers according to the specified parameters
*         in RTC_InitStruct.
* @param  RTC_InitStruct: pointer to a RTC_InitTypeDef structure that contains
*         the configuration information for the RTC peripheral.
* @note   The RTC Prescaler register is write protected and can be written in 
*         initialization mode only.  
* @retval An ErrorStatus enumeration value:
*          - SUCCESS: RTC registers are initialized
*          - ERROR: RTC registers are not initialized
*/
ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct)
{
  ErrorStatus status = ERROR;

  /* Check the parameters */
  assert_param(IS_RTC_HOUR_FORMAT(RTC_InitStruct->RTC_HourFormat));
  assert_param(IS_RTC_ASYNCH_PREDIV(RTC_InitStruct->RTC_AsynchPrediv));
  assert_param(IS_RTC_SYNCH_PREDIV(RTC_InitStruct->RTC_SynchPrediv));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Set Initialization mode */
  if (RTC_EnterInitMode() == ERROR)
  {
    status = ERROR;
  }
  else
  {
    /* Clear the bits to be configured first */
    RTC->CR1 &= ((uint8_t)~( RTC_CR1_FMT ));

    /* Set RTC_CR1 register */
    RTC->CR1 |=  ((uint8_t)(RTC_InitStruct->RTC_HourFormat));

    /* Set Prescalers registers */
    RTC->SPRERH = (uint8_t)(RTC_InitStruct->RTC_SynchPrediv >> 8);
    RTC->SPRERL = (uint8_t)(RTC_InitStruct->RTC_SynchPrediv);
    RTC->APRER =  (uint8_t)(RTC_InitStruct->RTC_AsynchPrediv);

    /* Exit Initialization mode */
    RTC->ISR1 &= (uint8_t)~RTC_ISR1_INIT;

    status = SUCCESS;
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 

  /* return Initialize the RTC registers status*/
  return (ErrorStatus)(status);
}

/**
* @brief  Fills each RTC_InitStruct member with its default value
*         Hour format = 24h / Prescalers configured to their reset values.
* @param  RTC_InitStruct: pointer to a RTC_InitTypeDef structure which will be
*         initialized.
* @retval None
*/
void RTC_StructInit(RTC_InitTypeDef* RTC_InitStruct)
{
  /* Initialize the RTC_Hourformat member */
  RTC_InitStruct->RTC_HourFormat = RTC_HourFormat_24;

  /* Initialize the RTC_AsynchPrediv member */
  RTC_InitStruct->RTC_AsynchPrediv = RTC_APRER_RESET_VALUE;

  /* Initialize the RTC_SynchPrediv member */
  RTC_InitStruct->RTC_SynchPrediv = RTC_SPRERL_RESET_VALUE;
}

/**
* @brief  Enables or disables the RTC registers write protection.
* @param  NewState: new state of the write protection.
  *         This parameter can be: ENABLE or DISABLE.
* @note   Writing a wrong key reactivates the write protection.
* @note   The protection mechanism is not affected by system reset.  
* @retval None
*/
void RTC_WriteProtectionCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the write protection for RTC registers */
    RTC->WPR = RTC_WPR_EnableKey;
  }
  else
  {
    /* Disable the write protection for RTC registers */
    RTC->WPR = RTC_WPR_DisableKey1;
    RTC->WPR = RTC_WPR_DisableKey2;
  }
}


/**
* @brief  Enters the RTC Initialization mode.
* @note   The RTC Initialization mode is write protected, use the 
*         RTC_WriteProtectionCmd(DISABLE) before calling this function. 
* @param  None
* @retval An ErrorStatus enumeration value:
*          - SUCCESS: RTC is in Init mode
*          - ERROR: RTC is not in Init mode
*/
ErrorStatus RTC_EnterInitMode(void)
{
  ErrorStatus status = ERROR;
  uint16_t initfcount = 0;

  /* Check if the Initialization mode is set */
  if ((RTC->ISR1 & RTC_ISR1_INITF) == RESET)
  {
    /* Set the Initialization mode */
    RTC->ISR1 = (uint8_t)RTC_ISR1_INIT;

    /* Wait until INITF flag is set */
    while (((RTC->ISR1 & RTC_ISR1_INITF) == RESET) && ( initfcount != INITF_TIMEOUT))
    {
      initfcount++;
    }
  }

  if ((RTC->ISR1 & RTC_ISR1_INITF) == RESET)
  {
    status = ERROR;
  }
  else
  {
    status = SUCCESS;
  }

  return (ErrorStatus)status;
}


/**
* @brief  Exits the RTC Initialization mode.
* @note   When the initialization sequence is complete, the Calendar restarts 
*         counting after 4 RTCCLK cycles.  
* @note   The RTC Initialization mode is write protected, use the 
*         RTC_WriteProtectionCmd(DISABLE) before calling this function.     
* @param  None
* @retval None
*/
void RTC_ExitInitMode(void)
{
  /* Exit Initialization mode */
  RTC->ISR1 &= (uint8_t)~RTC_ISR1_INIT;
}


/**
* @brief  Waits until the RTC Calendar registers (Time and Date)
*         are synchronized with RTC clock.
* @note   This function is meaningless when BYPASS feature is enabled in RTC_CR1
*         register.
* @note   To read the Calendar through the shadow registers after Calendar 
*         initialization, Calendar update or after wakeup from low power modes 
*         the software must first clear the RSF flag. 
*         The software must then wait until it is set again before reading 
*         the Calendar (if not yet done), which means that the Calendar registers
*         have been correctly copied into the RTC_TRx and RTC_DRx shadow registers.
* @note   RTC_SetTime() and RTC_SetDate() functions call RTC_WaitForSynchro() function
*         after updating the Calendar. In Run mode, User can use RTC_GetDate(), 
*         RTC_GetTime() and/or RTC_GetSubSecond() without need to call 
*         RTC_WaitForSynchro() function. After waking up from low power mode, this 
*         function must be called before calling RTC_GetDate(), RTC_GetTime() or 
*         RTC_GetSubSecond() functions.   
* @param  None
* @retval An ErrorStatus enumeration value:
*          - SUCCESS: RTC registers are synchronized
*          - ERROR: RTC registers are not synchronized
*/
ErrorStatus RTC_WaitForSynchro(void)
{
  uint16_t rsfcount = 0;
  ErrorStatus status = ERROR;
  
  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Clear RSF flag by writing 0 in RSF bit  */
  RTC->ISR1 &= (uint8_t)~(RTC_ISR1_RSF | RTC_ISR1_INIT);

  /* Wait the registers to be synchronised */
  while (((RTC->ISR1 & RTC_ISR1_RSF) == RESET) && ( rsfcount != RSF_TIMEOUT))
  {
    rsfcount++;
  }

  /* Check if RSF flag occurs*/
  if ((RTC->ISR1 & RTC_ISR1_RSF) != RESET)
  {
    status = SUCCESS;
  }
  else
  {
    status = ERROR;
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
  
  return (ErrorStatus)status;
}


/**
* @brief  Enables or Disables the RTC Ratio.
* @param  NewState: new state of the Ratio feature.
  *         This parameter can be: ENABLE or DISABLE.
* @retval None
*/
void RTC_RatioCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  if (NewState != DISABLE)
  {
    /* Set the RATIO bit */
    RTC->CR1 |= (uint8_t)RTC_CR1_RATIO;
  }
  else
  {
    /* Reset the RATIO bit */
    RTC->CR1 &= (uint8_t)~RTC_CR1_RATIO;
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}

/**
* @brief  Enables or Disables the Bypass Shadow feature.
* @param  NewState: new state of the Bypass Shadow feature.
  *         This parameter can be: ENABLE or DISABLE.
* @retval None
*/
void RTC_BypassShadowCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  if (NewState != DISABLE)
  {
    /* Set the BYPSHAD bit */
    RTC->CR1 |= (uint8_t)RTC_CR1_BYPSHAD;
  }
  else
  {
    /* Reset the BYPSHAD bit */
    RTC->CR1 &= (uint8_t)~RTC_CR1_BYPSHAD;
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}




/**
  * @}
  */

/** @defgroup RTC_Group2 Time and Date configuration functions
 *  @brief   Time and Date configuration functions 
 *
@verbatim   
 ===============================================================================
                   Time and Date configuration functions
 ===============================================================================  

  This section provide functions allowing to program and read the RTC Calendar
  (Time and Date).

@endverbatim
  * @{
  */


/**
* @brief Sets the RTC current time.
* @note   After updating the Calendar, this routine clears the RSF flag and waits
*         until it is set again (using RTC_WaitForSynchro() function) , which means
*         that the Calendar registers have been correctly copied into the RTC_TRx
*         and RTC_DRx shadow registers. 
* @param  RTC_Format: specifies the format of the entered parameters.
*         This parameter can be one of the @ref RTC_Format_TypeDef enumeration.
* @param  RTC_TimeStruct:  pointer to a  @ref RTC_TimeTypeDef structure that
*         contains the time configuration information for the RTC
* @retval An ErrorStatus enumeration value:
*          - SUCCESS: RTC Time register is configured
*          - ERROR: RTC Time register is not configured
*/
ErrorStatus RTC_SetTime(RTC_Format_TypeDef RTC_Format,
                        RTC_TimeTypeDef* RTC_TimeStruct)
{
  ErrorStatus status = ERROR;
  uint8_t temp = 0;

  /* Check the parameters */
  assert_param(IS_RTC_FORMAT(RTC_Format));

  if (RTC_Format == RTC_Format_BIN)
  {
    /* Check Hour Format (24h or 12h)*/
    if ((RTC->CR1 & RTC_CR1_FMT) != RESET)
    {
      assert_param(IS_RTC_HOUR12_MAX(RTC_TimeStruct->RTC_Hours));
      assert_param(IS_RTC_HOUR12_MIN(RTC_TimeStruct->RTC_Hours));
    }
    else
    {
      assert_param(IS_RTC_HOUR24(RTC_TimeStruct->RTC_Hours));
    }
    assert_param(IS_RTC_MINUTES(RTC_TimeStruct->RTC_Minutes));
    assert_param(IS_RTC_SECONDS(RTC_TimeStruct->RTC_Seconds));
  }
  else
  {
    /* Check Hour Format (24h or 12h)*/
    if ((RTC->CR1 & RTC_CR1_FMT) != RESET)
    {
      assert_param(IS_RTC_HOUR12_MAX(Bcd2ToByte(RTC_TimeStruct->RTC_Hours)));
      assert_param(IS_RTC_HOUR12_MIN(Bcd2ToByte(RTC_TimeStruct->RTC_Hours)));
    }
    else
    {
      assert_param(IS_RTC_HOUR24(Bcd2ToByte(RTC_TimeStruct->RTC_Hours)));
    }
    assert_param(IS_RTC_MINUTES(Bcd2ToByte(RTC_TimeStruct->RTC_Minutes)));
    assert_param(IS_RTC_SECONDS(Bcd2ToByte(RTC_TimeStruct->RTC_Seconds)));
  }


  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Set Initialization mode */
  if (RTC_EnterInitMode() == ERROR)
  {
    status = ERROR;
    /* Enable the write protection for RTC registers */
    RTC->WPR = 0xFF; 
  }
  else
  {
    /* Check Hour Format is 12h)*/
    if ((RTC->CR1 & RTC_CR1_FMT) != RESET)
    {
      assert_param(IS_RTC_H12(RTC_TimeStruct->RTC_H12));
      temp = RTC_TimeStruct->RTC_H12;
    }
    else
    {
      temp = 0;
    }
    /* Check the input parameters format */
    if (RTC_Format != RTC_Format_BIN)
    {
      RTC->TR1 = (uint8_t)(RTC_TimeStruct->RTC_Seconds);
      RTC->TR2 = (uint8_t)(RTC_TimeStruct->RTC_Minutes) ;
      RTC->TR3 = (uint8_t)( temp | RTC_TimeStruct->RTC_Hours) ;
    }
    else
    {
      RTC->TR1 = (uint8_t)(ByteToBcd2(RTC_TimeStruct->RTC_Seconds));
      RTC->TR2 = (uint8_t)(ByteToBcd2(RTC_TimeStruct->RTC_Minutes)) ;
      RTC->TR3 = (uint8_t)( temp | ByteToBcd2(RTC_TimeStruct->RTC_Hours));
    }
    /* Read DR3 register to unfreeze calender registers */
    (void)(RTC->DR3);

    /* Exit Initialization mode */
    RTC->ISR1 &= (uint8_t)~RTC_ISR1_INIT;

    /* Enable the write protection for RTC registers */
    RTC->WPR = 0xFF; 

    /* if RTC_CR1_BYPSHAD bit = 0, wait for synchro else this check is not needed */
    if ((RTC->CR1 & RTC_CR1_BYPSHAD) == RESET)
    {
      if (RTC_WaitForSynchro() == ERROR)
      {
        status = ERROR;
      }
      else
      {
        status = SUCCESS;
      }
    }
    else
    {
      status = SUCCESS;
    }
  }

  return (ErrorStatus)status;
}



/**
* @brief  Fills each RTC_TimeStruct member with its default value
*         (Time = 00h:00min:00sec).
* @param  RTC_TimeStruct: pointer to a @ref RTC_TimeTypeDef structure which will be
*         initialized.
* @retval None
*/
void RTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct)
{
  /* Time = 00h:00min:00sec*/
  RTC_TimeStruct->RTC_H12 = RTC_H12_AM;
  RTC_TimeStruct->RTC_Hours = 0;
  RTC_TimeStruct->RTC_Minutes = 0;
  RTC_TimeStruct->RTC_Seconds = 0;
}

          
/**
* @brief  Gets the RTC current Time.
* @note   To read the Calendar after wake up from low power mode, user software 
*         must first check that the RSF flag is set in RTC_ISR1, using 
*         RTC_WaitForSynchro() function, which means that the Calendar registers
*         have been correctly copied into the shadow registers (RTC_TRx and RTC_DRx).
* @param  RTC_Format: specifies the format of the returned parameters.
*         This parameter can be one of the @ref RTC_Format_TypeDef enumeration.
* @param  RTC_TimeStruct: pointer to a @ref RTC_TimeTypeDef structure that
*         will contain the returned current time configuration.
* @retval None

*/
void RTC_GetTime(RTC_Format_TypeDef RTC_Format,
                 RTC_TimeTypeDef* RTC_TimeStruct)
{
  uint8_t  tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_RTC_FORMAT(RTC_Format));

  /* Fill the structure fields with the read parameters */
  /* Get RTC seconds */
  RTC_TimeStruct->RTC_Seconds = RTC->TR1;

  /* Get RTC Minutes */
  RTC_TimeStruct->RTC_Minutes = RTC->TR2;

  /* Get the RTC_TR3 register */
  tmpreg = (uint8_t)RTC->TR3;

  /* Read DR3 register to unfreeze calender registers */
  (void) (RTC->DR3) ;


  /* Get RTC Hours */
  RTC_TimeStruct->RTC_Hours = (uint8_t)(tmpreg & (uint8_t)~(RTC_TR3_PM));

  /* Get RTC H12 state */
  RTC_TimeStruct->RTC_H12 = (RTC_H12_TypeDef)(tmpreg & RTC_TR3_PM);

  /* Check the input parameters format */
  if (RTC_Format == RTC_Format_BIN)
  {
    /* Convert the structure parameters to Binary format */
    RTC_TimeStruct->RTC_Hours = (uint8_t)Bcd2ToByte(RTC_TimeStruct->RTC_Hours);
    RTC_TimeStruct->RTC_Minutes = (uint8_t)Bcd2ToByte(RTC_TimeStruct->RTC_Minutes);
    RTC_TimeStruct->RTC_Seconds = (uint8_t)Bcd2ToByte(RTC_TimeStruct->RTC_Seconds);
  }
}

/**
* @brief  Gets the RTC current Calendar Subseconds value.
* @note   To read the Calendar after wake up from low power mode, user software 
*         must first check that the RSF flag is set in RTC_ISR1, using 
*         RTC_WaitForSynchro() function, which means that the Calendar registers
*         have been correctly copied into the shadow registers (RTC_TRx and RTC_DRx).
* @param  None
* @retval RTC current Calendar Subseconds value.
*/
uint16_t RTC_GetSubSecond(void)
{
  uint8_t ssrhreg = 0, ssrlreg = 0;
  uint16_t ssrreg = 0;

  /* Get subseconds values from the correspondent registers*/
  ssrhreg = RTC->SSRH;
  ssrlreg = RTC->SSRL;

  /* Read DR3 register to unfreeze calender registers */
  (void) (RTC->DR3);


  ssrreg = (uint16_t)((uint16_t)((uint16_t)ssrhreg << 8) | (uint16_t)(ssrlreg));
  return (uint16_t)(ssrreg);
}

/**
* @brief  Set the RTC current date.
* @note   After updating the Calendar, this routine clears the RSF flag and waits
*         until it is set again (using RTC_WaitForSynchro() function) , which means
*         that the Calendar registers have been correctly copied into the RTC_TRx
*         and RTC_DRx shadow registers.   
* @param  RTC_Format: specifies the format of the entered parameters.
*         This parameter can be one of the @ref RTC_Format_TypeDef enumeration.
* @param  RTC_DateStruct:  pointer to a  @ref RTC_TimeTypeDef structure that
*         contains the date configuration information for the RTC.
* @retval An ErrorStatus enumeration value:
*          - SUCCESS: RTC Date register is configured
*          - ERROR: RTC Date register is not configured
*/
ErrorStatus RTC_SetDate(RTC_Format_TypeDef RTC_Format,
                        RTC_DateTypeDef* RTC_DateStruct)
{
  ErrorStatus status = ERROR;

  if ((RTC_Format == RTC_Format_BIN) && ((RTC_DateStruct->RTC_Month & TEN_VALUE_BCD) == TEN_VALUE_BCD))
  {
    RTC_DateStruct->RTC_Month = (RTC_Month_TypeDef)((RTC_DateStruct->RTC_Month & (uint8_t)~(TEN_VALUE_BCD)) + TEN_VALUE_BIN);
  }

  /* Check the parameters */
  assert_param(IS_RTC_FORMAT(RTC_Format));
  if (RTC_Format == RTC_Format_BIN)
  {
    assert_param(IS_RTC_YEAR(RTC_DateStruct->RTC_Year));
    assert_param(IS_RTC_MONTH_MIN(RTC_DateStruct->RTC_Month));
    assert_param(IS_RTC_MONTH_MAX(RTC_DateStruct->RTC_Month));
    assert_param(IS_RTC_DATE_MIN(RTC_DateStruct->RTC_Date));
    assert_param(IS_RTC_DATE_MAX(RTC_DateStruct->RTC_Date));
  }
  else
  {
    assert_param(IS_RTC_YEAR(Bcd2ToByte(RTC_DateStruct->RTC_Year)));
    assert_param(IS_RTC_MONTH_MAX(Bcd2ToByte((uint8_t)RTC_DateStruct->RTC_Month)));
    assert_param(IS_RTC_MONTH_MIN(Bcd2ToByte((uint8_t)RTC_DateStruct->RTC_Month)));
    assert_param(IS_RTC_DATE_MIN(Bcd2ToByte((uint8_t)RTC_DateStruct->RTC_Date)));
    assert_param(IS_RTC_DATE_MAX(Bcd2ToByte((uint8_t)RTC_DateStruct->RTC_Date)));
  }
  assert_param(IS_RTC_WEEKDAY(RTC_DateStruct->RTC_WeekDay));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Set Initialization mode */
  if (RTC_EnterInitMode() == ERROR)
  {
    status = ERROR;
    /* Enable the write protection for RTC registers */
    RTC->WPR = 0xFF; 
  }
  else
  {
    (void)(RTC->TR1);
    /* Set the RTC_DR registers */
    /* Check the input parameters format */
    if (RTC_Format != RTC_Format_BIN)
    {
      RTC->DR1 = (uint8_t)(RTC_DateStruct->RTC_Date);
      RTC->DR2 = (uint8_t)((RTC_DateStruct->RTC_Month) | (uint8_t)((RTC_DateStruct->RTC_WeekDay) << 5));
      RTC->DR3 = (uint8_t)((RTC_DateStruct->RTC_Year));
    }
    else
    {
      RTC->DR1 = (uint8_t)(ByteToBcd2 ((uint8_t)RTC_DateStruct->RTC_Date));
      RTC->DR2 = (uint8_t)((ByteToBcd2((uint8_t)RTC_DateStruct->RTC_Month)) | (uint8_t)((RTC_DateStruct->RTC_WeekDay) << 5));
      RTC->DR3 = (uint8_t)(ByteToBcd2((uint8_t)RTC_DateStruct->RTC_Year));
    }

    /* Exit Initialization mode */
    RTC->ISR1 &= (uint8_t)~RTC_ISR1_INIT;

    /* Enable the write protection for RTC registers */
    RTC->WPR = 0xFF; 

    /* if  RTC_CR1_BYPSHAD bit = 0, wait for synchro else this check is not needed */
    if ((RTC->CR1 & RTC_CR1_BYPSHAD) == RESET)
    {
      if (RTC_WaitForSynchro() == ERROR)
      {
        status = ERROR;
      }
      else
      {
        status = SUCCESS;
      }
    }
    else
    {
      status = SUCCESS;
    }
  }

  return (ErrorStatus)status;
}
/**
* @brief  Fills each RTC_DateStruct member with its default value
*         (Monday 01 January xx00).
* @param  RTC_DateStruct: pointer to a @ref RTC_DateTypeDef structure which will be
*         initialized.
* @retval None
*/
void RTC_DateStructInit(RTC_DateTypeDef* RTC_DateStruct)
{
  /* * (Monday 01 January xx00)*/
  RTC_DateStruct->RTC_WeekDay = RTC_Weekday_Monday;
  RTC_DateStruct->RTC_Date = 1;
  RTC_DateStruct->RTC_Month = RTC_Month_January;
  RTC_DateStruct->RTC_Year = 0;
}
/**
* @brief  Get the RTC current date.
* @note   To read the Calendar after wake up from low power mode, user software 
*         must first check that the RSF flag is set in RTC_ISR1, using 
*         RTC_WaitForSynchro() function, which means that the Calendar registers
*         have been correctly copied into the shadow registers (RTC_TRx and RTC_DRx).
* @param  RTC_Format: specifies the format of the returned parameters.
*         This parameter can be one of the @ref RTC_Format_TypeDef enumeration.
* @param  RTC_DateStruct: pointer to a @ref RTC_DateTypeDef structure that
*         will contain the returned current Date configuration.
* @retval None
*/
void RTC_GetDate(RTC_Format_TypeDef RTC_Format,
                 RTC_DateTypeDef* RTC_DateStruct)
{
  uint8_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_RTC_FORMAT(RTC_Format));

  /* Fill the structure fields with the read parameters */
  (void) (RTC->TR1) ;
  RTC_DateStruct->RTC_Date = (uint8_t)(RTC->DR1);
  tmpreg = (uint8_t)RTC->DR2;
  RTC_DateStruct->RTC_Year = (uint8_t)(RTC->DR3);


  RTC_DateStruct->RTC_Month = (RTC_Month_TypeDef)(tmpreg & (uint8_t)(RTC_DR2_MT | RTC_DR2_MU));
  RTC_DateStruct->RTC_WeekDay = (RTC_Weekday_TypeDef)((uint8_t)((uint8_t)tmpreg & (uint8_t)(RTC_DR2_WDU)) >> (uint8_t)5);

  /* Check the input parameters format */
  if (RTC_Format == RTC_Format_BIN)
  {
    /* Convert the structure parameters to Binary format */
    RTC_DateStruct->RTC_Year = (uint8_t)Bcd2ToByte((uint8_t)RTC_DateStruct->RTC_Year);
    RTC_DateStruct->RTC_Month = (RTC_Month_TypeDef)Bcd2ToByte((uint8_t)RTC_DateStruct->RTC_Month);
    RTC_DateStruct->RTC_Date = (uint8_t)(Bcd2ToByte((uint8_t)RTC_DateStruct->RTC_Date));
  }
}

/**
  * @}
  */

/** @defgroup RTC_Group3 Alarms configuration functions
 *  @brief   Alarms (Alarm A and Alarm B) configuration functions 
 *
@verbatim   
 ===============================================================================
              Alarms (Alarm A and Alarm B) configuration functions
 ===============================================================================  

  This section provide functions allowing to program and read the RTC Alarms.

@endverbatim
  * @{
  */
  
/**
  * @brief  Sets the RTC Alarm configuration.
  * @note   Before configuring the Alarm settings, the Alarm Unit must be disabled
  *         (if enabled) using RTC_AlarmCmd(DISABLE) function.
  * @param  RTC_Format: specifies the format of the entered parameters.
  *         This parameter can be one of the @ref RTC_Format_TypeDef enumeration.
  * @param  RTC_AlarmStruct:  pointer to a  @ref RTC_AlarmTypeDef structure that
  *         contains the Alarm configuration information for the RTC.
  * @retval None.
  */
void RTC_SetAlarm(RTC_Format_TypeDef RTC_Format,
                  RTC_AlarmTypeDef* RTC_AlarmStruct)
{
  uint8_t tmpreg1 = 0;
  uint8_t tmpreg2 = 0;
  uint8_t tmpreg3 = 0;
  uint8_t tmpreg4 = 0;

  /* Check the parameters */
  assert_param(IS_RTC_ALARM_MASK(RTC_AlarmStruct->RTC_AlarmMask));
  assert_param(IS_RTC_FORMAT(RTC_Format));
  assert_param(IS_RTC_H12(RTC_AlarmStruct->RTC_AlarmTime.RTC_H12));
  assert_param(IS_RTC_ALARM_DATEWEEKDAY_SEL(RTC_AlarmStruct->RTC_AlarmDateWeekDaySel));


  if (RTC_Format == RTC_Format_BIN)
  {
    /* Check Hour Format (24h or 12h)*/
    if ((RTC->CR1 & RTC_CR1_FMT) != RESET)
    {
      assert_param(IS_RTC_HOUR12_MAX(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours));
      assert_param(IS_RTC_HOUR12_MIN(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours));
    }
    else
    {
      assert_param(IS_RTC_HOUR24(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours));
    }
    assert_param(IS_RTC_MINUTES(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes));
    assert_param(IS_RTC_SECONDS(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds));
  }
  else
  {
    /* Check Hour Format (24h or 12h)*/
    if ((RTC->CR1 & RTC_CR1_FMT) != RESET)
    {
      assert_param(IS_RTC_HOUR12_MAX(Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours)));
      assert_param(IS_RTC_HOUR12_MIN(Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours)));
    }
    else
    {
      assert_param(IS_RTC_HOUR24(Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours)));
    }

    assert_param(IS_RTC_MINUTES(Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes)));

    assert_param(IS_RTC_SECONDS(Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds)));

  }

  /* if Date/Weekday field is not masked */
  if ((RTC_AlarmStruct->RTC_AlarmMask & RTC_AlarmMask_DateWeekDay) == RESET )
  {
    if (RTC_AlarmStruct->RTC_AlarmDateWeekDaySel == RTC_AlarmDateWeekDaySel_WeekDay)
    {
      assert_param(IS_RTC_WEEKDAY(RTC_AlarmStruct->RTC_AlarmDateWeekDay));
    }
    else
    {
      assert_param(IS_RTC_DATE_MIN(RTC_AlarmStruct->RTC_AlarmDateWeekDay));
      assert_param(IS_RTC_DATE_MAX(RTC_AlarmStruct->RTC_AlarmDateWeekDay));
    }
  }

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;


  /* Check the input parameters format & Configure the Alarm register */
  if (RTC_Format != RTC_Format_BIN)
  {
    tmpreg1 = (uint8_t)((uint8_t)(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds) | \
                        (uint8_t)((uint8_t)(RTC_AlarmStruct->RTC_AlarmMask) & (uint8_t)RTC_ALRMAR1_MSK1));

    tmpreg2 = (uint8_t)((uint8_t)(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes) | \
                        (uint8_t)((uint8_t)(RTC_AlarmStruct->RTC_AlarmMask << 1) & (uint8_t)RTC_ALRMAR2_MSK2));

    tmpreg3 = (uint8_t)((uint8_t)((uint8_t)(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours) | \
                                  (uint8_t)(RTC_AlarmStruct->RTC_AlarmTime.RTC_H12)) | \
                        (uint8_t)((uint8_t)(RTC_AlarmStruct->RTC_AlarmMask << 2) & (uint8_t)RTC_ALRMAR3_MSK3));

    tmpreg4 = (uint8_t)((uint8_t)((uint8_t)(RTC_AlarmStruct->RTC_AlarmDateWeekDay) | \
                                  (uint8_t)(RTC_AlarmStruct->RTC_AlarmDateWeekDaySel)) | \
                        (uint8_t)((uint8_t)(RTC_AlarmStruct->RTC_AlarmMask << 3) & (uint8_t)RTC_ALRMAR4_MSK4));

  }
  else
  {
    tmpreg1 = (uint8_t)((ByteToBcd2(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds)) | \
                        (uint8_t)(RTC_AlarmStruct->RTC_AlarmMask & RTC_ALRMAR1_MSK1));

    tmpreg2 = (uint8_t)((ByteToBcd2(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes)) | \
                        (uint8_t)((uint8_t)(RTC_AlarmStruct->RTC_AlarmMask << 1) & (uint8_t)RTC_ALRMAR2_MSK2));

    tmpreg3 = (uint8_t)((uint8_t)((ByteToBcd2(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours)) | \
                                  (uint8_t)(RTC_AlarmStruct->RTC_AlarmTime.RTC_H12)) | \
                        (uint8_t)((uint8_t)(RTC_AlarmStruct->RTC_AlarmMask << 2) & (uint8_t)RTC_ALRMAR3_MSK3));

    tmpreg4 = (uint8_t)((uint8_t)((ByteToBcd2(RTC_AlarmStruct->RTC_AlarmDateWeekDay)) | \
                                  (uint8_t)(RTC_AlarmStruct->RTC_AlarmDateWeekDaySel)) | \
                        (uint8_t)((uint8_t)(RTC_AlarmStruct->RTC_AlarmMask << 3) & (uint8_t)(RTC_ALRMAR4_MSK4)));
  }

  /* Configure the Alarm register */
  RTC->ALRMAR1 = tmpreg1;
  RTC->ALRMAR2 = tmpreg2;
  RTC->ALRMAR3 = tmpreg3;
  RTC->ALRMAR4 = tmpreg4;

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}

/**
* @brief  Fills each RTC_AlarmStruct member with its default value
*         (Time = 00h:00mn:00sec / Date = 1st day of the month/Mask =
*         all fields are masked).
* @param  RTC_AlarmStruct: pointer to a @ref RTC_AlarmTypeDef structure which
*         will be initialized.
* @retval None
*/
void RTC_AlarmStructInit(RTC_AlarmTypeDef* RTC_AlarmStruct)
{
  /* Alarm Time Settings : Time = 00h:00mn:00sec */
  RTC_AlarmStruct->RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
  RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours = 0;
  RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes = 0;
  RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds = 0;

  /* Alarm Date Settings : Date = 1st day of the month*/
  RTC_AlarmStruct->RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  RTC_AlarmStruct->RTC_AlarmDateWeekDay = 1;

  /* Alarm Masks Settings : Mask =  all fields are masked*/
  RTC_AlarmStruct->RTC_AlarmMask = RTC_AlarmMask_All;
}
/**
  * @brief  Gets the RTC Alarm configuration.
  * @param  RTC_Format: specifies the format of the entered parameters.
  *         This parameter can be one of the @ref RTC_Format_TypeDef enumeration.
  * @param  RTC_AlarmStruct:  pointer to a  @ref RTC_AlarmTypeDef structure that
  *         will contain the Alarm configuration information of  the RTC.
  * @retval None
  */
void RTC_GetAlarm(RTC_Format_TypeDef RTC_Format,
                  RTC_AlarmTypeDef* RTC_AlarmStruct)
{
  uint8_t tmpreg1 = 0;
  uint8_t tmpreg2 = 0;
  uint8_t tmpreg3 = 0;
  uint8_t tmpreg4 = 0;
  uint8_t alarmmask = 0;

  /* Check the parameters */
  assert_param(IS_RTC_FORMAT(RTC_Format));

  /* Get Alarm registers data */
  tmpreg1 = (uint8_t)RTC->ALRMAR1;
  tmpreg2 = (uint8_t)RTC->ALRMAR2;
  tmpreg3 = (uint8_t)RTC->ALRMAR3;
  tmpreg4 = (uint8_t)RTC->ALRMAR4;

  /* Fill the structure with the read parameters */
  RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds = (uint8_t)((uint8_t)tmpreg1 & (uint8_t)((uint8_t)RTC_ALRMAR1_ST | (uint8_t)RTC_ALRMAR1_SU));
  alarmmask = (uint8_t)(tmpreg1 & RTC_ALRMAR1_MSK1);

  /* Fill the structure with the read parameters */
  RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes = (uint8_t)((uint8_t)tmpreg2 & (uint8_t)((uint8_t)RTC_ALRMAR2_MNT | (uint8_t)RTC_ALRMAR2_MNU));
  alarmmask = (uint8_t)((alarmmask) | (uint8_t)((uint8_t)(tmpreg2 & RTC_ALRMAR2_MSK2) >> 1));

  /* Fill the structure with the read parameters */
  RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours = (uint8_t)((uint8_t)tmpreg3 & (uint8_t)((uint8_t)RTC_ALRMAR3_HT | (uint8_t)RTC_ALRMAR3_HU));
  RTC_AlarmStruct->RTC_AlarmTime.RTC_H12 = (RTC_H12_TypeDef)((uint8_t)tmpreg3 & (uint8_t)RTC_ALRMAR3_PM);
  alarmmask = (uint8_t)((alarmmask) | (uint8_t)((uint8_t)((uint8_t)tmpreg3 & (uint8_t)RTC_ALRMAR3_MSK3) >> 2));

  /* Fill the structure with the read parameters */
  RTC_AlarmStruct->RTC_AlarmDateWeekDay = (uint8_t)((uint8_t)tmpreg4 & (uint8_t)((uint8_t)RTC_ALRMAR4_DT | (uint8_t)RTC_ALRMAR4_DU));
  RTC_AlarmStruct->RTC_AlarmDateWeekDaySel = (RTC_AlarmDateWeekDaySel_TypeDef)((uint8_t)tmpreg4 & (uint8_t)RTC_ALRMAR4_WDSEL);
  alarmmask = (uint8_t)((alarmmask) | (uint8_t)((uint8_t)((uint8_t)tmpreg4 & RTC_ALRMAR4_MSK4) >> 3));

  RTC_AlarmStruct->RTC_AlarmMask = alarmmask;

  if (RTC_Format == RTC_Format_BIN)
  {
    RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours = Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Hours);
    RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes = Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Minutes);
    RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds = Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmTime.RTC_Seconds);
    RTC_AlarmStruct->RTC_AlarmDateWeekDay = Bcd2ToByte(RTC_AlarmStruct->RTC_AlarmDateWeekDay);
  }
}

/**
  * @brief  Enables or disables the RTC Alarm.
  * @param  NewState: new state of the alarm. This parameter can be: ENABLE or DISABLE.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: RTC Alarm is enabled/disabled
  *          - ERROR: RTC Alarm is not enabled/disabled
  */
ErrorStatus RTC_AlarmCmd(FunctionalState NewState)
{
  __IO uint16_t alrawfcount = 0;
  ErrorStatus status = ERROR;
  uint8_t temp1 = 0;

  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Configure the Alarm state */
  if (NewState != DISABLE)
  { /*Enable the Alarm*/
    RTC->CR2 |= (uint8_t)(RTC_CR2_ALRAE);
    status = SUCCESS;
  }
  else
  {  /* Disable the Alarm */
    RTC->CR2 &= (uint8_t)~(RTC_CR2_ALRAE) ;

    /* Wait until ALRxWF flag is set */
    temp1 = (uint8_t)(RTC->ISR1 & RTC_ISR1_ALRAWF);
    while ((alrawfcount != ALRAWF_TIMEOUT) && (temp1 == RESET))
    {
      alrawfcount++;
    }

    if ((RTC->ISR1 &  RTC_ISR1_ALRAWF) == RESET)
    {
      status = ERROR;
    }
    else
    {
      status = SUCCESS;
    }
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 

  /* Return the status*/
  return (ErrorStatus)status;
}
/**
* @brief  Configure the RTC Alarm Subseconds value and mask.
* @param  RTC_AlarmSubSecondValue: specifies the Subseconds value.
*         This parameter can be a value from 0 to 0x7FFF.
* @param  RTC_AlarmSubSecondMask:  specifies the Subseconds Mask.
*         This parameter can be one of the @ref RTC_AlarmSubSecondMask_TypeDef enumeration.
* @retval An ErrorStatus enumeration value:
*          - SUCCESS: Alarm Subseconds value and mask are configured
*          - ERROR: Alarm Subseconds value and mask are  not configured
*/
ErrorStatus RTC_AlarmSubSecondConfig(uint16_t RTC_AlarmSubSecondValue,
                                     RTC_AlarmSubSecondMask_TypeDef RTC_AlarmSubSecondMask)
{
  uint8_t alarmstatus = 0;
  ErrorStatus status = ERROR;

  /* Check the parameters */
  assert_param(IS_RTC_ALARM_SS_VALUE(RTC_AlarmSubSecondValue));
  assert_param(IS_RTC_ALARM_SS_MASK(RTC_AlarmSubSecondMask));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Check if the initialization mode is not set */
  if ((RTC->ISR1 & RTC_ISR1_INITF) == RESET)
  {
    /* save  Alarm status */
    alarmstatus = (uint8_t)(RTC->CR2 | RTC_CR2_ALRAE);

    /* Disable the Alarm */
    RTC->CR2 &= (uint8_t)~(RTC_CR2_ALRAE);

    /* Configure the Alarm register */
    RTC->ALRMASSRH = (uint8_t)(RTC_AlarmSubSecondValue >> 8);
    RTC->ALRMASSRL = (uint8_t)(RTC_AlarmSubSecondValue);
    RTC->ALRMASSMSKR = (uint8_t)RTC_AlarmSubSecondMask;

    /* restore the saved  Alarm status */
    RTC->CR2 |= alarmstatus;

    status = SUCCESS;
  }
  else
  {
    status = ERROR;
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 

  return (ErrorStatus)status;
}



/**
  * @}
  */

/** @defgroup RTC_Group4 WakeUp Timer configuration functions
 *  @brief   WakeUp Timer configuration functions 
 *
@verbatim   
 ===============================================================================
                     WakeUp Timer configuration functions
 ===============================================================================  

  This section provide functions allowing to program and read the RTC WakeUp.

@endverbatim
  * @{
  */
/**
  * @brief  Configures the RTC Wakeup clock source.
  * @pre    Before configuring the wakeup unit Clock source, the wake up Unit must
  *         be disabled (if enabled) using RTC_WakeUpCmd(Disable) .
  * @param  RTC_WakeupClockSrc: specifies the Wakeup clock source,
  *         this parameter  can be one of the @ref RTC_WakeupClockSrc_TypeDef enumeration.
  * @retval None
  */
void RTC_WakeUpClockConfig(RTC_WakeUpClock_TypeDef RTC_WakeUpClock)
{

  /* Check the parameters */
  assert_param(IS_RTC_WAKEUP_CLOCK(RTC_WakeUpClock));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Disable the Wakeup timer in RTC_CR2 register */
  RTC->CR2 &= (uint8_t)~RTC_CR2_WUTE;

  /* Clear the Wakeup Timer clock source bits in CR1 register */
  RTC->CR1 &= (uint8_t)~RTC_CR1_WUCKSEL;

  /* Configure the clock source */
  RTC->CR1 |= (uint8_t)RTC_WakeUpClock;

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}

/**
  * @brief  Sets the RTC Wakeup counter.
  * @note   Before configuring the wakeup unit counter, the wake up Unit must be
  *         disabled (if enabled) using RTC_WakeUpCmd(Disable).
  * @param  RTC_WakeupCounter: specifies the Wake up counter,
  *         This parameter can be a value from 0x0000 to 0xFFFF.
  * @retval None.
  */
void RTC_SetWakeUpCounter(uint16_t RTC_WakeupCounter)
{
  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;


  /* Configure the Wakeup Timer counter */
  RTC->WUTRH = (uint8_t)(RTC_WakeupCounter >> 8);
  RTC->WUTRL = (uint8_t)(RTC_WakeupCounter);

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}

/**
  * @brief  Returns the RTC Wakeup timer counter value.
  * @param  None.
  * @retval RTC Wakeup Counter value.
  */
uint16_t RTC_GetWakeUpCounter(void)
{
  uint16_t tmpreg = 0;

  /* Get the counter value */
  tmpreg = ((uint16_t)RTC->WUTRH) << 8;
  tmpreg |= RTC->WUTRL;

  /* return RTC Wakeup Counter value*/
  return (uint16_t)tmpreg;
}

/**
  * @brief  Enables or Disables the RTC Wakeup Unit.
  * @param  NewState: new state of the Wakeup Unit. This parameter can 
  *                   be: ENABLE or DISABLE.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS : RTC Wakeup Unit is enabled/disabled
  *          - ERROR    : RTC Wakeup Unit is not enabled/disabled
  */
ErrorStatus RTC_WakeUpCmd(FunctionalState NewState)
{
  ErrorStatus status = ERROR;
  uint16_t wutwfcount = 0;

  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  if (NewState != DISABLE)
  {
    /* Enable the Wakeup Timer */
    RTC->CR2 |= (uint8_t)RTC_CR2_WUTE;

    status = SUCCESS;
  }
  else
  {
    /* Disable the Wakeup Timer */
    RTC->CR2 &= (uint8_t)~RTC_CR2_WUTE;

    /* Wait until WUTWF flag is set */
    while (((RTC->ISR1 & RTC_ISR1_WUTWF) == RESET) && ( wutwfcount != WUTWF_TIMEOUT))
    {
      wutwfcount++;
    }

    /* Check WUTWF flag is set or not */
    if ((RTC->ISR1 & RTC_ISR1_WUTWF) == RESET)
    {
      status = ERROR;
    }
    else
    {
      status = SUCCESS;
    }
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 

  /* Return the status*/
  return (ErrorStatus)status;
}






/**
  * @}
  */

/** @defgroup RTC_Group5 Daylight Saving configuration functions
 *  @brief   Daylight Saving configuration functions 
 *
@verbatim   
 ===============================================================================
                    Daylight Saving configuration functions
 ===============================================================================  

  This section provide functions allowing to configure the RTC DayLight Saving.

@endverbatim
  * @{
  */

/**
  * @brief  Adds or subtracts one hour from the current time depending on
  *         the daylight saving parameter.
  * @param  RTC_DayLightSaving: the day light saving Mode
  *         This parameter can be one of the @ref RTC_DayLightSaving_TypeDef 
  *         enumeration.
  * @param  RTC_StoreOperation: the day light saving store operation
  *         This parameter can be one of the @ref RTC_StoreOperation_TypeDef 
  *         enumeration.
  * @retval None
  */
void RTC_DayLightSavingConfig(RTC_DayLightSaving_TypeDef RTC_DayLightSaving,
                              RTC_StoreOperation_TypeDef RTC_StoreOperation)
{
  /* Check the parameters */
  assert_param(IS_RTC_DAYLIGHT_SAVING(RTC_DayLightSaving));
  assert_param(IS_RTC_STORE_OPERATION(RTC_StoreOperation));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Clear the bits to be configured */
  RTC->CR3 &= (uint8_t)~(RTC_CR3_BCK);

  /* Configure the RTC_CR3 register */
  RTC->CR3 |= (uint8_t)((uint8_t)RTC_DayLightSaving | (uint8_t)RTC_StoreOperation);

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}

/**
  * @brief  Returns the stored operation.
  * @param  None
  * @retval the store operation, this parameter can be one of
  *         the @ref RTC_StoreOperation_TypeDef enumeration.
  */
RTC_StoreOperation_TypeDef  RTC_GetStoreOperation(void)
{
  /* Return the stored operation*/
  return (RTC_StoreOperation_TypeDef)(RTC->CR3 & RTC_CR3_BCK);
}






/**
  * @}
  */

/** @defgroup RTC_Group6 Output pin Configuration function
 *  @brief   Output pin Configuration function 
 *
@verbatim   
 ===============================================================================
                         Output pin Configuration function
 ===============================================================================  

  This section provide functions allowing to configure the RTC Output source.

@endverbatim
  * @{
  */
/**
  * @brief  Configures the RTC output for the output pin (RTC_ALARM output).
  * @param  RTC_OutputSel: Specifies which signal will be mapped to the output.
  *         This parameter can be one parameter from the 
  *         @ref RTC_OutputSel_TypeDef enumeration.
  * @param  RTC_OutputPolarity: Specifies the polarity of the output signal.
  *         This parameter can be one parameter from the 
  *         @ref RTC_OutputPolarity_TypeDef enumeration.
  * @retval None
  */
void RTC_OutputConfig(RTC_Output_TypeDef RTC_Output,
                      RTC_OutputPolarity_TypeDef RTC_OutputPolarity)
{
  /* Check the parameters */
  assert_param(IS_RTC_OUTPUT_SEL(RTC_Output));
  assert_param(IS_RTC_OUTPUT_POL(RTC_OutputPolarity));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Clear the bits to be configured */
  RTC->CR3 &= (uint8_t)~(RTC_CR3_OSEL | RTC_CR3_POL);

  /* Configure the output selection and polarity */
  RTC->CR3 |= (uint8_t)((uint8_t)RTC_Output | (uint8_t)RTC_OutputPolarity);

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}




/**
  * @}
  */

/** @defgroup RTC_Group7 Output pin Configuration function
 *  @brief   Output pin Configuration function 
 *
@verbatim   
 ===============================================================================
                         Output pin Configuration function
 ===============================================================================  

  This section provide functions allowing to configure the RTC Output source.

@endverbatim
  * @{
  */
/**
* @brief  Configures the Synchronization Shift Control Settings.
* @param  RTC_ShiftAdd1S : Select to add or not 1 second to the time Calendar.
*         This parameter can be one parameter from the @ref RTC_ShiftAdd1S_TypeDef
*         enumeration.
* @param  RTC_ShiftSubFS: Select the number of Second Fractions to Substitute.
*         This parameter can be one any value from 0 to 0x7FFF.
 * @retval An ErrorStatus enumeration value:
*          - SUCCESS: RTC Shift registers are configured
*          - ERROR: RTC Shift registers are not configured
*/
ErrorStatus RTC_SynchroShiftConfig(RTC_ShiftAdd1S_TypeDef RTC_ShiftAdd1S,
                                   uint16_t RTC_ShiftSubFS)
{
  uint8_t shiftrhreg = 0;
  ErrorStatus status = ERROR;
  uint16_t shpfcount = 0;

  /* Check the parameters */
  assert_param(IS_RTC_SHIFT_ADD1S(RTC_ShiftAdd1S));
  assert_param(IS_RTC_SHIFT_SUBFS(RTC_ShiftSubFS));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Check if a Shift is pending*/
  if ((RTC->ISR1 & RTC_ISR1_SHPF) != RESET)
  {
    /* wait until the shift is completed*/
    while (((RTC->ISR1 & RTC_ISR1_SHPF) != RESET) && (shpfcount != SHPF_TIMEOUT))
    {
      shpfcount++;
    }
  }

  /* check if the Shift pending is completed or if there is no Shift operation at all*/
  if ((RTC->ISR1 & RTC_ISR1_SHPF) == RESET)
  {
    /* Configure the Shift settings */
    shiftrhreg = (uint8_t)((uint8_t)(RTC_ShiftSubFS >> 8) | (uint8_t)(RTC_ShiftAdd1S));
    RTC->SHIFTRH = (uint8_t)(shiftrhreg);
    RTC->SHIFTRL = (uint8_t)(RTC_ShiftSubFS);

    status = SUCCESS;
  }
  else
  {
    status = ERROR;
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 

  return (ErrorStatus)(status);
}



/**
  * @}
  */

/** @defgroup RTC_Group8 Output pin Configuration function
 *  @brief   Output pin Configuration function 
 *
@verbatim   
 ===============================================================================
                         Output pin Configuration function
 ===============================================================================  

  This section provide functions allowing to configure the RTC Output source.

@endverbatim
  * @{
  */
/**
* @brief  Configures the Smooth Calibration Settings.
* @param  RTC_SmoothCalibPeriod : Select the Smooth Calibration Period.
*         This parameter can be one parameter from
*         the @ref RTC_SmoothCalibPeriod_TypeDef enumeration.
* @param  RTC_SmoothCalibPlusPulses : Select to Set or reset the CALP bit.
*         This parameter can be one parameter from the
*         @ref RTC_SmoothCalibPlusPulses_TypeDef enumeration.
* @param  RTC_SmouthCalibMinusPulsesValue: Select the value of CALM[8:0] bits.
*         This parameter can be one any value from 0 to 0x01FF.
* @retval An ErrorStatus enumeration value:
*          - SUCCESS: RTC Calib registers are configured
*          - ERROR: RTC Calib registers are not configured
*/
ErrorStatus RTC_SmoothCalibConfig(RTC_SmoothCalibPeriod_TypeDef RTC_SmoothCalibPeriod,
                                  RTC_SmoothCalibPlusPulses_TypeDef RTC_SmoothCalibPlusPulses,
                                  uint16_t RTC_SmouthCalibMinusPulsesValue)
{
  ErrorStatus status = ERROR;
  uint16_t recalpfcount = 0;

  /* Check the parameters */
  assert_param(IS_RTC_SMOOTH_CALIB_PERIOD(RTC_SmoothCalibPeriod));
  assert_param(IS_RTC_SMOOTH_CALIB_PLUS(RTC_SmoothCalibPlusPulses));
  assert_param(IS_RTC_SMOOTH_CALIB_MINUS(RTC_SmouthCalibMinusPulsesValue));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* check if a calibration is pending*/
  if ((RTC->ISR1 & RTC_ISR1_RECALPF) != RESET)
  {
    /* wait until the Calibration is completed*/
    while (((RTC->ISR1 & RTC_ISR1_RECALPF) != RESET) && (recalpfcount != RECALPF_TIMEOUT))
    {
      recalpfcount++;
    }
  }

  /* check if the calibration pending is completed or if there is 
     no calibration operation at all*/
  if ((RTC->ISR1 & RTC_ISR1_RECALPF) == RESET)
  {
    /* Configure the Smooth calibration settings */
    RTC->CALRH = (uint8_t)((uint8_t)((uint8_t)RTC_SmoothCalibPeriod | \
                           (uint8_t)RTC_SmoothCalibPlusPulses) | \
                           (uint8_t)((uint16_t)RTC_SmouthCalibMinusPulsesValue >> 8));
    RTC->CALRL = (uint8_t)(RTC_SmouthCalibMinusPulsesValue);

    status = SUCCESS;
  }
  else
  {
    status = ERROR;
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 

  return (ErrorStatus)(status);
}


/**
  * @}
  */

/** @defgroup RTC_Group9 Digital Calibration configuration functions
 *  @brief   Digital Calibration configuration functions 
 *
@verbatim   
 ===============================================================================
                  Calibration configuration functions
 ===============================================================================  

@endverbatim
  * @{
  */
/**
* @brief  Configure the Calibration Pin-out (RTC_CALIB) Selection (1Hz or 512Hz).
* @param  RTC_CalibOutput : Select the Calibration output Selection .
*         This parameter can be one parameter from the
*         @ref RTC_CalibOutput_TypeDef enumeration.
* @retval None
*/
void RTC_CalibOutputConfig(RTC_CalibOutput_TypeDef RTC_CalibOutput)
{
  /* Check the parameters */
  assert_param(IS_RTC_CALOUTPUT_SELECT(RTC_CalibOutput));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  if (RTC_CalibOutput != RTC_CalibOutput_512Hz)
  {
    /* Enable the RTC clock output */
    RTC->CR3 |= (uint8_t)RTC_CR3_COSEL;
  }
  else
  {
    /* Disable the RTC clock output */
    RTC->CR3 &= (uint8_t)~RTC_CR3_COSEL;
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}

/**
* @brief  Enables or disables the RTC clock to be output through the relative pin.
* @param  NewState: new state of the RTC Calibration output
  *         This parameter can be: ENABLE or DISABLE.
* @retval None
*/
void RTC_CalibOutputCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  if (NewState != DISABLE)
  {
    /* Enable the RTC clock output */
    RTC->CR3 |= (uint8_t)RTC_CR3_COE;
  }
  else
  {
    /* Disable the RTC clock output */
    RTC->CR3 &= (uint8_t)~RTC_CR3_COE;
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}


/**
  * @}
  */

/** @defgroup RTC_Group10 Tampers configuration functions
 *  @brief   Tampers configuration functions 
 *
@verbatim   
 ===============================================================================
                       Tampers configuration functions
 ===============================================================================  

@endverbatim
  * @{
  */

/**
* @brief  Configures the Tamper Sensitive Level.
* @param  RTC_Tamper: Select the tamper to configure.
*         This parameter can be one parameter from the @ref RTC_Tamper_TypeDef
*         enumeration.
* @param  RTC_TamperLevel: Select the tamper Sensitive Level.
*         This parameter can be one parameter from the 
*         @ref RTC_TamperLevel_TypeDef enumeration.
* @retval None
*/
void RTC_TamperLevelConfig(RTC_Tamper_TypeDef RTC_Tamper,
                           RTC_TamperLevel_TypeDef RTC_TamperLevel)
{
  /* Check the parameters */
  assert_param(IS_RTC_TAMPER((uint8_t)RTC_Tamper));
  assert_param(IS_RTC_TAMPER_LEVEL(RTC_TamperLevel));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  if (RTC_TamperLevel != RTC_TamperLevel_Low)
  {
    /* Enable the selected Tampers */
    RTC->TCR1 |= (uint8_t)(RTC_Tamper << 1);
  }
  else
  {
    /* Disable the selected Tampers */
    RTC->TCR1 &= (uint8_t)~(uint8_t)(RTC_Tamper << 1);
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}
/**
  * @brief  Configures the Tampers Filter.
  * @param  RTC_TamperFilter: Specifies the tampers filter.
  *         This parameter can be one parameter from the
  *         @ref RTC_TamperFilter_TypeDef enumeration.
  * @retval None
  */
void RTC_TamperFilterConfig(RTC_TamperFilter_TypeDef RTC_TamperFilter)
{

  /* Check the parameters */
  assert_param(IS_RTC_TAMPER_FILTER(RTC_TamperFilter));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /*clear flags before configuration */
  RTC->TCR2 &= (uint8_t)~(RTC_TCR2_TAMPFLT);

  /* Configure the RTC_TCR register */
  RTC->TCR2 |= (uint8_t)RTC_TamperFilter;

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 

}

/**
  * @brief  Configures the Tampers Sampling Frequency.
  * @param  RTC_TamperSamplingFreq: Specifies the tampers Sampling Frequency.
  *         This parameter can be one parameter from the
  *         @ref RTC_TamperSamplingFreq_TypeDef enumeration.
  * @retval None
  */
void RTC_TamperSamplingFreqConfig(RTC_TamperSamplingFreq_TypeDef RTC_TamperSamplingFreq)
{
  /* Check the parameters */
  assert_param(IS_RTC_TAMPER_SAMPLING_FREQ(RTC_TamperSamplingFreq));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Clear flags before configuration */
  RTC->TCR2 &= (uint8_t)~(RTC_TCR2_TAMPFREQ);

  /* Configure the RTC_TCR register */
  RTC->TCR2 |= (uint8_t)RTC_TamperSamplingFreq;

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}

/**
* @brief  Configures the Tampers Pins input Precharge Duration.
* @param  RTC_TamperPrechargeDuration: Specifies the Tampers Pins input
*         Precharge Duration.
*         This parameter can be one parameter from the
*         @ref RTC_TamperPrechargeDuration_TypeDef enumeration.
* @retval None
*/
void RTC_TamperPinsPrechargeDuration(RTC_TamperPrechargeDuration_TypeDef RTC_TamperPrechargeDuration)
{
  /* Check the parameters */
  assert_param(IS_RTC_TAMPER_PINS_PRECHAR_DURATION(RTC_TamperPrechargeDuration));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Clear the tampers  pull-up  and precharge/discharge duration Old settings*/
  RTC->TCR2 &= (uint8_t)~(RTC_TCR2_TAMPPUDIS | RTC_TCR2_TAMPPRCH);

  /* Configure the Tampers Precharge Duration  and pull-up New settings*/
  RTC->TCR2 |= (uint8_t)RTC_TamperPrechargeDuration;

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}


/**
* @brief  Enables or Disables the Tamper detection.
* @param  RTC_Tamper: Select the tamper to configure.
*         This parameter can be one parameter from the @ref RTC_Tamper_TypeDef
*         enumeration.
* @param  NewState: new state of the tamper pin.
  *         This parameter can be: ENABLE or DISABLE.
* @retval None
*/
void RTC_TamperCmd(RTC_Tamper_TypeDef RTC_Tamper,
                   FunctionalState NewState)
{

  /* Check the parameters */
  assert_param(IS_RTC_TAMPER((uint8_t)RTC_Tamper));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;


  if (NewState != DISABLE)
  {
    /* Enable the selected Tampers */
    RTC->TCR1 |= (uint8_t)RTC_Tamper;
  }
  else
  {
    /* Disable the selected Tampers */
    RTC->TCR1 &= (uint8_t)~RTC_Tamper;
  }


  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}



/**
  * @}
  */

/** @defgroup RTC_Group11 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions  
 *
@verbatim   
 ===============================================================================
                       Interrupts and flags management functions
 ===============================================================================  
 
 To enable the RTC Alarm interrupt, the following sequence is required:
   - Configure the RTC to generate RTC alarm (Alarm A) using the RTC_SetAlarm() 
     and RTC_AlarmCmd() functions.

 To enable the RTC Wakeup interrupt, the following sequence is required:
   - Configure the RTC to generate the RTC wakeup timer event using the 
     RTC_WakeUpClockConfig(), RTC_SetWakeUpCounter() and RTC_WakeUpCmd() functions.

 To enable the RTC Tamper interrupt, the following sequence is required:
   - Configure the RTC to detect the RTC tamper event using the 
     RTC_TamperTriggerConfig() and RTC_TamperCmd() functions.



@endverbatim
  * @{
  */
/**
* @brief  Enables or Disables the specified RTC interrupts.
* @param  RTC_IT: specifies the RTC interrupt sources to be enabled or disabled.
*         This parameter can be any combination of the following values:
*         @arg RTC_IT_ALRA:  Alarm A interrupt
*         @arg RTC_IT_TAMP:  Tampers interrupt
*         @arg RTC_IT_WUT:  WakeUp Timer interrupt
* @param  NewState: new state of the specified RTC interrupts.
  *         This parameter can be: ENABLE or DISABLE.
* @retval None
*/
void RTC_ITConfig(RTC_IT_TypeDef RTC_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RTC_CONFIG_IT(RTC_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* Disable the write protection for RTC registers */
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  if (NewState != DISABLE)
  {
    /* Enable the Interrupts */
    RTC->CR2 |= (uint8_t)((uint16_t)RTC_IT & (uint16_t)0x00F0);
    RTC->TCR1 |= (uint8_t)((uint16_t)RTC_IT & RTC_TCR1_TAMPIE);
  }
  else
  {
    /* Disable the Interrupts */
    RTC->CR2  &= (uint8_t)~(uint8_t)((uint16_t)RTC_IT & (uint16_t)0x00F0);
    RTC->TCR1 &= (uint8_t)~(uint8_t)((uint16_t)RTC_IT & (uint16_t)RTC_TCR1_TAMPIE);
  }

  /* Enable the write protection for RTC registers */
  RTC->WPR = 0xFF; 
}
/**
* @brief  Checks whether the specified RTC flag is set or not.
* @param  RTC_FLAG: specifies the flag to check.
*         This parameter can be one of the @ref RTC_Flag_TypeDef enumeration.
* @retval Status of RTC_FLAG (SET or RESET)
*/
FlagStatus RTC_GetFlagStatus(RTC_Flag_TypeDef RTC_FLAG)
{
  FlagStatus flagstatus = RESET;
  uint16_t tmpreg1 = 0;
  uint16_t tmpreg2 = 0;

  /* Check the parameters */
  assert_param(IS_RTC_GET_FLAG(RTC_FLAG));

  /* Get all the flags */
  tmpreg2 = (uint16_t)((uint16_t)RTC->ISR1 << 8);
  tmpreg1 = (uint16_t)((uint16_t)((uint16_t)(RTC->ISR2)) | tmpreg2);

  /* Return the status of the flag */
  if ((tmpreg1 & (uint16_t)RTC_FLAG) != RESET)
  {
    flagstatus = SET;
  }
  else
  {
    flagstatus = RESET;
  }
  return (FlagStatus)flagstatus;
}

/**
* @brief  Clears the RTC's pending flags.
* @param  RTC_FLAG: specifies the RTC flag to clear.
*         This parameter can be any combination of the @ref RTC_Flag_TypeDef
*         enumeration.
* @retval None
*/
void RTC_ClearFlag(RTC_Flag_TypeDef RTC_FLAG)
{
  /* Check the parameters */
  assert_param(IS_RTC_CLEAR_FLAG((uint16_t)RTC_FLAG));

  /* Clear the Flags in the RTC_ISR registers */
  RTC->ISR2 = (uint8_t)~((uint8_t)RTC_FLAG) ;
  RTC->ISR1 = (uint8_t)(((uint8_t)~(uint8_t)((uint16_t)RTC_FLAG >> (uint8_t)8)) & ((uint8_t)~(uint8_t)(RTC_ISR1_INIT)));
}

/**
* @brief  Checks whether the specified RTC interrupt has occurred or not.
* @param  RTC_IT: specifies the RTC interrupt source to check.
*         This parameter can be one of the @ref RTC_IT_TypeDef enumeration.
* @retval Status of RTC_IT (SET or RESET).
*/
ITStatus RTC_GetITStatus(RTC_IT_TypeDef RTC_IT)
{
  ITStatus itstatus = RESET;
  uint8_t enablestatus = 0, tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_RTC_GET_IT(RTC_IT));

  /* Get the Interrupt enable Status */
  enablestatus = (uint8_t)(RTC->CR2 & (uint16_t)RTC_IT);

  /* Get the Interrupt pending bit */
  tmpreg = (uint8_t)(RTC->ISR2 & (uint8_t)((uint16_t)RTC_IT >> 4));

  /* Get the status of the Interrupt */
  if ((enablestatus != (uint8_t)RESET) && (tmpreg != (uint8_t)RESET))
  {
    itstatus = SET;
  }
  else
  {
    itstatus = RESET;
  }

  return (ITStatus)itstatus;
}

/**
* @brief  Clears the RTC's interrupt pending bits.
* @param  RTC_IT: specifies the RTC interrupt pending bit to clear.
*         This parameter can be any combination of the @ref RTC_IT_TypeDef
*         enumeration.
* @retval None
*/
void RTC_ClearITPendingBit(RTC_IT_TypeDef RTC_IT)
{
  /* Check the parameters */
  assert_param(IS_RTC_CLEAR_IT((uint16_t)RTC_IT));

  /* Clear the interrupt pending bits in the RTC_ISR registers */
  RTC->ISR2 = (uint8_t)~(uint8_t)((uint16_t)RTC_IT >> 4);
}

/**
  * @}
  */









/**
 * @addtogroup RTC_Private_Functions
 * @{
 */
/**
* @brief  Converts a 2 digit decimal to BCD format
* @param  Value: Byte to be converted.
* @retval Converted byte
*/
static uint8_t ByteToBcd2(uint8_t Value)
{
  uint8_t bcdhigh = 0;

  while (Value >= 10)
  {
    bcdhigh++;
    Value -= 10;
  }

  return  (uint8_t)((uint8_t)(bcdhigh << 4) | Value);
}

/**
* @brief  Converts from 2 digit BCD to Binary format
* @param  Value: BCD value to be converted.
* @retval Converted word
*/
static uint8_t Bcd2ToByte(uint8_t Value)
{
  uint8_t tmp = 0;

  tmp = (uint8_t)((uint8_t)((uint8_t)(Value & (uint8_t)0xF0) >> 4) * (uint8_t)10);

  return (uint8_t)(tmp + (Value & (uint8_t)0x0F));
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