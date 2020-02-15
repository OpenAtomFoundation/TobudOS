/**
  ******************************************************************************
  * @file    stm8l15x.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the peripheral register's definitions, bits
  *          definitions and memory mapping for STM8L15x devices.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8L15x_H
 #define __STM8L15x_H

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */
/* Uncomment the line below according to the target STM8L15x device used in your
   application
  */
/* #define STM8L15X_LD */     /*!< STM8L15X_LD: STM8L15x Low density devices */
/* #define STM8L15X_MD */     /*!< STM8L15X_MD: STM8L15x Medium density devices */
/* #define STM8L15X_MDP */    /*!< STM8L15X_MDP: STM8L15x Medium density plus devices */
/* #define STM8L15X_HD */     /*!< STM8L15X_HD: STM8L15x/16x High density devices */
/* #define STM8L05X_LD_VL */  /*!< STM8L05X_LD_VL: STM8L051xx3 Low density value line devices */
/* #define STM8L05X_MD_VL */  /*!< STM8L05X_MD_VL: STM8L052xx6 Medium density value line devices */
/* #define STM8L05X_HD_VL */  /*!< STM8L05X_HD_VL: STM8L052xx8 High density value line devices */
/* #define STM8AL31_L_MD */   /*!< STM8AL31_L_MD: STM8AL3x Medium density devices */


/*  Tip: To avoid modifying this file each time you need to switch between these
        devices, you can define the device in your toolchain compiler preprocessor.

 - Low density STM8L15x devices are STM8L151C3, STM8L151K3, STM8L151G3, STM8L151F3,
   STM8L151C2, STM8L151K2, STM8L151G2 and STM8L151F2 microcontrollers where the
   Flash memory density ranges between 4 and 8 Kbytes.
 - Medium density STM8L15x devices are STM8L151C4, STM8L151C6, STM8L152C4,
   STM8L152C6, STM8L151K4, STM8L151K6, STM8L152K4, STM8L152K6, STM8L151G4,
   STM8L151G6, STM8L152G4 and STM8L152G6 microcontrollers where the Flash memory
   density ranges between 16 and 32 Kbytes.
 - Medium density Plus devices are STM8L151R6, STM8L152R6 microcontrollers where
  the Flash memory density is fixed and equal to 32 Kbytes and a wider range of
  peripheral than the medium density devices.
 - High density STM8L15x devices are STM8L151x8, STM8L152x8, STM8L162R8 and STM8L162M8
   microcontrollers where the Flash memory density is fixed and equal to 64 Kbytes with
   the same peripheral set than Medium Density Plus devices.
 - Value line low density STM8L05xx devices are STM8L051x3 microcontrollers
   with 8-KB Flash
 - Value line medium density STM8L05xx devices are STM8L052x6 microcontrollers
   with 32-KB Flash
 - Value line high density STM8L05xx devices: STM8L052x8 microcontrollers
   with 64-KB Flash
 - Medium density STM8AL31xx/STM8AL3Lxx devices: STM8AL3168, STM8AL3166,
   STM8AL3148,STM8AL3146, STM8AL3138, STM8AL3136, STM8AL3L68, STM8AL3L66,
   STM8AL3L48, STM8AL3L46 microcontrollers with 8-KB, 16-KB or 32-KB Flash
 */

#if !defined (STM8L15X_MD) && !defined (STM8L15X_MDP) && !defined (STM8L15X_HD) && !defined (STM8L15X_LD) \
&& !defined (STM8L05X_LD_VL) && !defined (STM8L05X_MD_VL) && !defined (STM8L05X_HD_VL) && !defined (STM8AL31_L_MD)
 #error "Please select first the target STM8L device used in your application (in stm8l15x.h file)"
#endif

/******************************************************************************/
/*                   Library configuration section                            */
/******************************************************************************/
/* Check the used compiler */
#if defined(__CSMC__)
 #define _COSMIC_
#elif defined(__RCSTM8__)
 #define _RAISONANCE_
#elif defined(__ICCSTM8__)
 #define _IAR_
#else
 #error "Unsupported Compiler!"          /* Compiler defines not found */
#endif

#if !defined  USE_STDPERIPH_DRIVER
/* Comment the line below if you will not use the peripherals drivers.
   In this case, these drivers will not be included and the application code will be
   based on direct access to peripherals registers */
 #define USE_STDPERIPH_DRIVER
#endif

/**
 * @brief In the following line adjust the value of External High Speed oscillator (HSE)
   used in your application

   Tip: To avoid modifying this file each time you need to use different HSE, you
        can define the HSE value in your toolchain compiler preprocessor.
  */
#if !defined  HSE_VALUE
 #define HSE_VALUE   ((uint32_t)16000000) /*!< Typical Value of the HSE in Hz */
#endif /* HSE_VALUE */

/**
  * @brief Definition of External Low Speed oscillator (LSE) frequency
  */
#if !defined  LSE_VALUE
 #define LSE_VALUE   ((uint32_t)32768)   /*!< Typical Value of the LSE in Hz */
#endif /* LSE_VALUE */
/**
  * @brief Definition of Device on-chip RC oscillator frequencies
  */
#if !defined  HSI_VALUE
 #define HSI_VALUE   ((uint32_t)16000000) /*!< Typical Value of the HSI in Hz */
#endif /* HSI_VALUE */

#if !defined  LSI_VALUE
 #define LSI_VALUE   ((uint32_t)38000)    /*!< Typical Value of the LSI in Hz */
#endif /* LSI_VALUE */

#ifdef _COSMIC_
 #define FAR  @far
 #define NEAR @near
 #define TINY @tiny
 #define EEPROM @eeprom
 #define CONST  const
#elif defined (_RAISONANCE_) /* __RCSTM8__ */
 #define FAR  far
 #define NEAR data
 #define TINY page0
 #define EEPROM eeprom
 #define CONST  code
 #if defined (STM8L15X_MD) || defined (STM8L15X_MDP) || defined (STM8L05X_MD_VL) || \
 defined (STM8AL31_L_MD)
  /*!< Used with memory Models for code less than 64K */
  #define MEMCPY memcpy
 #elif defined (STM8L15X_HD) || defined (STM8L05X_HD_VL)
   /*!< Used with memory Models for code higher than 64K */
  #define MEMCPY fmemcpy
 #endif /* STM8L15X_MD or STM8L15X_MDP or STM8L05X_MD_VL or STM8AL31_L_MD*/
#else /*_IAR_*/
 #define FAR  __far
 #define NEAR __near
 #define TINY __tiny
 #define EEPROM __eeprom
 #define CONST  const
#endif /* __CSMC__ */

/**
  * @brief Legacy definition
  */
#define __CONST  CONST

#if defined (STM8L15X_MD) || defined (STM8L15X_MDP) || defined (STM8L15X_LD) || \
defined (STM8L05X_LD_VL) || defined (STM8L05X_MD_VL) || defined (STM8AL31_L_MD)
/*!< Used with memory Models for code smaller than 64K */
 #define PointerAttr NEAR
 #define MemoryAddressCast uint16_t
#elif defined (STM8L15X_HD) || defined (STM8L05X_HD_VL)
/*!< Used with memory Models for code higher than 64K */
 #define PointerAttr FAR
 #define MemoryAddressCast uint32_t
#endif /* STM8L15X_MD or STM8L15X_MDP or STM8L15X_LD or STM8L05X_LD_VL or STM8L05X_MD_VL
          or STM8AL31_L_MD */

/* Uncomment the line below to enable the FLASH functions execution from RAM */
#if !defined (RAM_EXECUTION)
/* #define RAM_EXECUTION  (1) */
#endif /* RAM_EXECUTION */

#ifdef RAM_EXECUTION
 #ifdef _COSMIC_
   #define IN_RAM(a) a
 #elif defined (_RAISONANCE_) /* __RCSTM8__ */
   #define IN_RAM(a) a inram
 #else /*_IAR_*/
  #define IN_RAM(a) __ramfunc a
 #endif /* _COSMIC_ */
#else
  #define IN_RAM(a) a
#endif /* RAM_EXECUTION */

/*!< [31:16] STM8L15X Standard Peripheral Library main version */
#define __STM8L15X_STDPERIPH_VERSION_MAIN   ((uint8_t)0x01) /*!< [31:24] main version */
#define __STM8L15X_STDPERIPH_VERSION_SUB1   ((uint8_t)0x06) /*!< [23:16] sub1 version */
#define __STM8L15X_STDPERIPH_VERSION_SUB2   ((uint8_t)0x01) /*!< [15:8]  sub2 version */
#define __STM8L15X_STDPERIPH_VERSION_RC     ((uint8_t)0x00) /*!< [7:0]  release candidate */
#define __STM8L15X_STDPERIPH_VERSION       ( (__STM8L15X_STDPERIPH_VERSION_MAIN << 24)\
                                          |(__STM8L15X_STDPERIPH_VERSION_SUB1 << 16)\
                                          |(__STM8L15X_STDPERIPH_VERSION_SUB2 << 8)\
                                          |(__STM8L15X_STDPERIPH_VERSION_RC))

/******************************************************************************/

/* Includes ------------------------------------------------------------------*/

/* Exported types and constants ----------------------------------------------*/

/** @addtogroup Exported_types
  * @{
  */

/**
 * IO definitions
 *
 * define access restrictions to peripheral registers
 */
#define     __I     volatile const   /*!< defines 'read only' permissions     */
#define     __O     volatile         /*!< defines 'write only' permissions    */
#define     __IO    volatile         /*!< defines 'read / write' permissions  */

/*!< Signed integer types  */
#include "stdint.h"
#if 0
typedef   signed char     int8_t;
typedef   signed short    int16_t;
typedef   signed long     int32_t;

/*!< Unsigned integer types  */
typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned long     uint32_t;
#endif

/*!< STM8Lx Standard Peripheral Library old types (maintained for legacy purpose) */

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;


typedef enum {FALSE = 0, TRUE = !FALSE} bool;

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus, BitStatus, BitAction;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

#define U8_MAX     (255)
#define S8_MAX     (127)
#define S8_MIN     (-128)
#define U16_MAX    (65535u)
#define S16_MAX    (32767)
#define S16_MIN    (-32768)
#define U32_MAX    (4294967295uL)
#define S32_MAX    (2147483647)
#define S32_MIN    (-2147483648uL)

/**
  * @}
  */

/** @addtogroup MAP_FILE_Exported_Types_and_Constants
  * @{
  */

/******************************************************************************/
/*                          IP registers structures                           */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/**
  * @brief General Purpose I/Os (GPIO)
  */
typedef struct GPIO_struct
{
  __IO uint8_t ODR; /*!< Output Data Register */
  __IO uint8_t IDR; /*!< Input Data Register */
  __IO uint8_t DDR; /*!< Data Direction Register */
  __IO uint8_t CR1; /*!< Configuration Register 1 */
  __IO uint8_t CR2; /*!< Configuration Register 2 */
}
GPIO_TypeDef;

/** @addtogroup GPIO_Registers_Reset_Value
  * @{
  */
#define GPIO_ODR_RESET_VALUE ((uint8_t)0x00)
#define GPIO_DDR_RESET_VALUE ((uint8_t)0x00)
#define GPIO_CR1_RESET_VALUE ((uint8_t)0x00)
#define GPIO_CR2_RESET_VALUE ((uint8_t)0x00)
/**
  * @}
  */
/*----------------------------------------------------------------------------*/

/**
  * @brief Real-Time Clock (RTC) peripheral registers.
  */
typedef struct RTC_struct
{
  __IO uint8_t TR1; /*!<  Time  Register  1*/
  __IO uint8_t TR2; /*!<  Time  Register  2*/
  __IO uint8_t TR3; /*!<  Time  Register  3*/

  uint8_t RESERVED0;

  __IO uint8_t DR1; /*!<  Date  Register  1*/
  __IO uint8_t DR2; /*!<  Date  Register  2*/
  __IO uint8_t DR3; /*!<  Date  Register  3*/

  uint8_t RESERVED1;

  __IO uint8_t CR1; /*!<  Control  Register  1*/
  __IO uint8_t CR2; /*!<  Control  Register  2*/
  __IO uint8_t CR3; /*!<  Control  Register  3*/

  uint8_t RESERVED2;

  __IO uint8_t ISR1; /*!<  Initialisation and Status  Register 1 */
  __IO uint8_t ISR2; /*!<  Initialisation and Status  Register 2 */

  uint8_t RESERVED3;
  uint8_t RESERVED4;

  __IO uint8_t SPRERH; /*!<  Synchronous Prediv high  Register  */
  __IO uint8_t SPRERL; /*!<  Synchronous Prediv Low Register  */
  __IO uint8_t APRER;  /*!<  Asynchronous Prediv  Register  */

  uint8_t RESERVED5;

  __IO uint8_t WUTRH; /*!<  Wake-Up Timer High Register  */
  __IO uint8_t WUTRL; /*!<  Wake-Up Timer  Low Register  */

  uint8_t RESERVED6;

  __IO uint8_t SSRH; /*!< Sub Second High Register  */
  __IO uint8_t SSRL; /*!< Sub Second Low Register  */

  __IO uint8_t WPR;  /*!< Write Protection Register  */

  __IO uint8_t SHIFTRH; /*!< Shift control High Register  */
  __IO uint8_t SHIFTRL; /*!<  Shift control Low Register  */

  __IO uint8_t ALRMAR1; /*!<  ALARM A  Register 1  */
  __IO uint8_t ALRMAR2; /*!<  ALARM A  Register 2 */
  __IO uint8_t ALRMAR3; /*!<  ALARM A  Register 3 */
  __IO uint8_t ALRMAR4; /*!<  ALARM A  Register 4 */

  uint8_t RESERVED7[4];

  __IO uint8_t ALRMASSRH; /*!<  ALARM A Subsecond Register High */
  __IO uint8_t ALRMASSRL; /*!<  ALARM A Subsecond Register Low  */
  __IO uint8_t ALRMASSMSKR; /*!<  ALARM A Subsecond Mask Register  */

  uint8_t RESERVED8[3];

  __IO uint8_t CALRH; /*!<  Calibration register high  */
  __IO uint8_t CALRL; /*!<  Calibration register low  */

  __IO uint8_t TCR1; /*!<  Tamper control register 1   */
  __IO uint8_t TCR2; /*!<  Tamper control register 2  */
}
RTC_TypeDef;

/** @addtogroup RTC_Registers_Reset_Value
  * @{
  */
#define RTC_TR1_RESET_VALUE       ((uint8_t)0x00)
#define RTC_TR2_RESET_VALUE       ((uint8_t)0x00)
#define RTC_TR3_RESET_VALUE       ((uint8_t)0x00)

#define RTC_DR1_RESET_VALUE       ((uint8_t)0x01)
#define RTC_DR2_RESET_VALUE       ((uint8_t)0x21)
#define RTC_DR3_RESET_VALUE       ((uint8_t)0x00)

#define RTC_CR1_RESET_VALUE       ((uint8_t)0x00)
#define RTC_CR2_RESET_VALUE       ((uint8_t)0x00)
#define RTC_CR3_RESET_VALUE       ((uint8_t)0x00)

#define RTC_ISR1_RESET_VALUE      ((uint8_t)0x07)
#define RTC_ISR2_RESET_VALUE      ((uint8_t)0x00)

#define RTC_SPRERH_RESET_VALUE    ((uint8_t)0x00)
#define RTC_SPRERL_RESET_VALUE    ((uint8_t)0xFF)
#define RTC_APRER_RESET_VALUE     ((uint8_t)0x7F)

#define RTC_WUTRH_RESET_VALUE     ((uint8_t)0xFF)
#define RTC_WUTRL_RESET_VALUE     ((uint8_t)0xFF)

#define RTC_WPR_RESET_VALUE       ((uint8_t)0x00)

#define RTC_ALRMAR1_RESET_VALUE   ((uint8_t)0x00)
#define RTC_ALRMAR2_RESET_VALUE   ((uint8_t)0x00)
#define RTC_ALRMAR3_RESET_VALUE   ((uint8_t)0x00)
#define RTC_ALRMAR4_RESET_VALUE   ((uint8_t)0x00)

#define RTC_SHIFTRH_RESET_VALUE   ((uint8_t)0x00)
#define RTC_SHIFTRL_RESET_VALUE   ((uint8_t)0x00)

#define RTC_ALRMASSRH_RESET_VALUE   ((uint8_t)0x00)
#define RTC_ALRMASSRL_RESET_VALUE   ((uint8_t)0x00)
#define RTC_ALRMASSMSKR_RESET_VALUE   ((uint8_t)0x00)

#define RTC_CALRH_RESET_VALUE   ((uint8_t)0x00)
#define RTC_CALRL_RESET_VALUE   ((uint8_t)0x00)

#define RTC_TCR1_RESET_VALUE   ((uint8_t)0x00)
#define RTC_TCR2_RESET_VALUE   ((uint8_t)0x00)

/**
  * @}
  */

/** @addtogroup RTC_Registers_Bits_Definition
  * @{
  */

/* Bits definition for RTC_TR1 register*/
#define RTC_TR1_ST               ((uint8_t)0x70)
#define RTC_TR1_SU               ((uint8_t)0x0F)

/* Bits definition for RTC_TR2 register*/
#define RTC_TR2_MNT              ((uint8_t)0x70)
#define RTC_TR2_MNU              ((uint8_t)0x0F)

/* Bits definition for RTC_TR3 register*/
#define RTC_TR3_PM               ((uint8_t)0x40)
#define RTC_TR3_HT               ((uint8_t)0x30)
#define RTC_TR3_HU               ((uint8_t)0x0F)

/* Bits definition for RTC_DR1 register*/
#define RTC_DR1_DT               ((uint8_t)0x30)
#define RTC_DR1_DU               ((uint8_t)0x0F)

/* Bits definition for RTC_DR2 register*/
#define RTC_DR2_WDU              ((uint8_t)0xE0)
#define RTC_DR2_MT               ((uint8_t)0x10)
#define RTC_DR2_MU               ((uint8_t)0x0F)

/* Bits definition for RTC_DR3 register*/
#define RTC_DR3_YT               ((uint8_t)0xF0)
#define RTC_DR3_YU               ((uint8_t)0x0F)

/* Bits definition for RTC_CR1 register*/
#define RTC_CR1_FMT              ((uint8_t)0x40)
#define RTC_CR1_RATIO            ((uint8_t)0x20)
#define RTC_CR1_WUCKSEL          ((uint8_t)0x07)
#define RTC_CR1_BYPSHAD           ((uint8_t)0x10)


/* Bits definition for RTC_CR2 register*/
#define RTC_CR2_WUTIE            ((uint8_t)0x40)
#define RTC_CR2_ALRAIE           ((uint8_t)0x10)
#define RTC_CR2_WUTE             ((uint8_t)0x04)
#define RTC_CR2_ALRAE            ((uint8_t)0x01)
#define RTC_CR2_ALRIE            ((uint8_t)0x20)



/* Bits definition for RTC_CR3 register*/
#define RTC_CR3_COE              ((uint8_t)0x80)
#define RTC_CR3_OSEL             ((uint8_t)0x60)
#define RTC_CR3_POL              ((uint8_t)0x10)
#define RTC_CR3_COSEL            ((uint8_t)0x08)
#define RTC_CR3_BCK              ((uint8_t)0x04)
#define RTC_CR3_SUB1H            ((uint8_t)0x02)
#define RTC_CR3_ADD1H            ((uint8_t)0x01)


/* Bits definition for RTC_ISR1 register*/
#define RTC_ISR1_INIT            ((uint8_t)0x80)
#define RTC_ISR1_INITF           ((uint8_t)0x40)
#define RTC_ISR1_RSF             ((uint8_t)0x20)
#define RTC_ISR1_INITS           ((uint8_t)0x10)
#define RTC_ISR1_SHPF            ((uint8_t)0x08)
#define RTC_ISR1_WUTWF           ((uint8_t)0x04)
#define RTC_ISR1_RECALPF         ((uint8_t)0x02)
#define RTC_ISR1_ALRAWF          ((uint8_t)0x01)


/* Bits definition for RTC_ISR2 register*/
#define RTC_ISR2_WUTF            ((uint8_t)0x04)
#define RTC_ISR2_ALRAF           ((uint8_t)0x01)
#define RTC_ISR2_TAMP3F          ((uint8_t)0x80)
#define RTC_ISR2_TAMP2F          ((uint8_t)0x40)
#define RTC_ISR2_TAMP1F          ((uint8_t)0x20)

/* Bits definition for RTC_SHIFTRH register*/
#define RTC_SHIFTRH_ADD1S        ((uint8_t)0x80)
#define RTC_SHIFTRH_SUBFS        ((uint8_t)0x7F)

/* Bits definition for RTC_SHIFTRL register*/
#define RTC_SHIFTRL_SUBFS        ((uint8_t)0xFF)


/* Bits definition for RTC_ALRMAR1 register*/
#define RTC_ALRMAR1_MSK1         ((uint8_t)0x80)
#define RTC_ALRMAR1_ST           ((uint8_t)0x70)
#define RTC_ALRMAR1_SU           ((uint8_t)0x0F)

/* Bits definition for RTC_ALRMAR2 register*/
#define RTC_ALRMAR2_MSK2         ((uint8_t)0x80)
#define RTC_ALRMAR2_MNT          ((uint8_t)0x70)
#define RTC_ALRMAR2_MNU          ((uint8_t)0x0F)

/* Bits definition for RTC_ALRMAR3 register*/
#define RTC_ALRMAR3_MSK3         ((uint8_t)0x80)
#define RTC_ALRMAR3_PM           ((uint8_t)0x40)
#define RTC_ALRMAR3_HT           ((uint8_t)0x30)
#define RTC_ALRMAR3_HU           ((uint8_t)0x0F)

/* Bits definition for RTC_ALRMAR4 register*/
#define RTC_ALRMAR4_MSK4         ((uint8_t)0x80)
#define RTC_ALRMAR4_WDSEL        ((uint8_t)0x40)
#define RTC_ALRMAR4_DT           ((uint8_t)0x30)
#define RTC_ALRMAR4_DU           ((uint8_t)0x0F)

/* Bits definition for RTC_ALRMASSRH register*/
#define RTC_ALRMASSRH_ALSS         ((uint8_t)0x7F)

/* Bits definition for RTC_ALRMASSRL register*/
#define RTC_ALRMASSRL_ALSS         ((uint8_t)0xFF)

/* Bits definition for RTC_ALRMASSMSKR register*/
#define RTC_ALRMASSMSKR_MASKSS   ((uint8_t)0x1F)


/* Bits definition for RTC_CALRH register*/
#define RTC_CALRH_CALP          ((uint8_t)0x80)
#define RTC_CALRH_CALW8         ((uint8_t)0x40)
#define RTC_CALRH_CALW16        ((uint8_t)0x20)
#define RTC_CALRH_CALWx         ((uint8_t)0x60)
#define RTC_CALRH_CALM          ((uint8_t)0x01)

/* Bits definition for RTC_CALRL register*/
#define RTC_CALRL_CALM          ((uint8_t)0xFF)

/* Bits definition for RTC_TCR1 register*/
#define RTC_TCR1_TAMP3LEVEL     ((uint8_t)0x40)
#define RTC_TCR1_TAMP3E         ((uint8_t)0x20)
#define RTC_TCR1_TAMP2LEVEL     ((uint8_t)0x10)
#define RTC_TCR1_TAMP2E         ((uint8_t)0x08)
#define RTC_TCR1_TAMP1LEVEL     ((uint8_t)0x04)
#define RTC_TCR1_TAMP1E         ((uint8_t)0x02)
#define RTC_TCR1_TAMPIE         ((uint8_t)0x01)

/* Bits definition for RTC_TCR2 register*/
#define RTC_TCR2_TAMPPUDIS         ((uint8_t)0x80)
#define RTC_TCR2_TAMPPRCH          ((uint8_t)0x60)
#define RTC_TCR2_TAMPFLT           ((uint8_t)0x18)
#define RTC_TCR2_TAMPFREQ          ((uint8_t)0x07)


/*RTC special defines */
#define RTC_WPR_EnableKey        ((uint8_t)0xFF)
#define RTC_WPR_DisableKey1      ((uint8_t)0xCA)
#define RTC_WPR_DisableKey2      ((uint8_t)0x53)

/**
  * @}
  */

/**
  * @brief CSS on LSE registers.
  */
typedef struct CSSLSE_struct
{
  __IO uint8_t CSR; /*!<  Control and Status Register*/
}
CSSLSE_TypeDef;

/** @addtogroup CSSLSE_Registers_Reset_Value
  * @{
  */
#define CSSLSE_CSR_RESET_VALUE       ((uint8_t)0x00)

/**
  * @}
  */

/** @addtogroup CSSLSE_Registers_Bits_Definition
  * @{
  */

/* Bits definition for CSSLSE_CSR register*/
#define CSSLSE_CSR_SWITCHF            ((uint8_t)0x10)
#define CSSLSE_CSR_CSSF               ((uint8_t)0x08)
#define CSSLSE_CSR_CSSIE              ((uint8_t)0x04)
#define CSSLSE_CSR_SWITCHEN           ((uint8_t)0x02)
#define CSSLSE_CSR_CSSEN              ((uint8_t)0x01)

/**
  * @}
  */
/*----------------------------------------------------------------------------*/
/**
  * @brief Beeper (BEEP) peripheral registers.
  */

typedef struct BEEP_struct
{
  __IO uint8_t CSR1; /*!< BEEP Control status register1 */
  uint8_t RSERVED1;
  uint8_t RESERVED2;
  __IO uint8_t CSR2; /*!< BEEP Control status register2 */
}
BEEP_TypeDef;

/** @addtogroup BEEP_Registers_Reset_Value
  * @{
  */
#define BEEP_CSR1_RESET_VALUE ((uint8_t)0x00)
#define BEEP_CSR2_RESET_VALUE ((uint8_t)0x1F)

/**
  * @}
  */

/** @addtogroup BEEP_Registers_Bits_Definition
  * @{
  */

#define BEEP_CSR1_MSR     ((uint8_t)0x01) /*!< Measurement enable mask */

#define BEEP_CSR2_BEEPSEL ((uint8_t)0xC0) /*!< Beeper frequency selection mask */
#define BEEP_CSR2_BEEPEN  ((uint8_t)0x20) /*!< Beeper enable mask */
#define BEEP_CSR2_BEEPDIV ((uint8_t)0x1F) /*!< Beeper Divider prescalar mask */

/**
  * @}
  */

/*----------------------------------------------------------------------------ok*/

/**
  * @brief Configuration Registers (CFG)
  */

typedef struct CFG_struct
{
  __IO uint8_t GCR; /*!< Global Configuration register */
}
CFG_TypeDef;

/** @addtogroup CFG_Registers_Reset_Value
  * @{
  */

#define CFG_GCR_RESET_VALUE ((uint8_t)0x00)

/**
  * @}
  */

/** @addtogroup CFG_Registers_Bits_Definition
  * @{
  */

#define CFG_GCR_SWD ((uint8_t)0x01) /*!< Swim disable bit mask */
#define CFG_GCR_AL  ((uint8_t)0x02) /*!< Activation Level bit mask */

/**
  * @}
  */
/*----------------------------------------------------------------------------ok*/

/**
  * @brief SYSCFG
  */

typedef struct SYSCFG_struct
{
  __IO uint8_t RMPCR3; /*!< Remap control register 3 */
  __IO uint8_t RMPCR1; /*!< Remap control register 1 */
  __IO uint8_t RMPCR2; /*!< Remap control register 2 */
}
SYSCFG_TypeDef;

/** @addtogroup SYSCFG_Registers_Reset_Value
  * @{
  */
#define SYSCFG_RMPCR1_RESET_VALUE ((uint8_t)0x0C)
#define SYSCFG_RMPCR2_RESET_VALUE ((uint8_t)0x00)
#define SYSCFG_RMPCR3_RESET_VALUE ((uint8_t)0x00)

/**
  * @}
  */

/** @addtogroup SYSCFG_Registers_Bits_Definition
  * @{
  */

/* For DMA Channel Mapping*/
#define SYSCFG_RMPCR1_ADC1DMA_REMAP     ((uint8_t)0x03) /*!< ADC1 DMA channel remapping */
#define SYSCFG_RMPCR1_TIM4DMA_REMAP     ((uint8_t)0x0C) /*!< TIM4 DMA channel remapping */


/* For GPIO Reapping*/
#define SYSCFG_RMPCR1_USART1TR_REMAP    ((uint8_t)0x30) /*!< USART1_TX and USART1_RX remapping */
#define SYSCFG_RMPCR1_USART1CK_REMAP    ((uint8_t)0x40) /*!< USART1_CK remapping */
#define SYSCFG_RMPCR1_SPI1_REMAP        ((uint8_t)0x80) /*!< SPI1 remapping */

#define SYSCFG_RMPCR2_ADC1TRIG_REMAP    ((uint8_t)0x01) /*!< ADC1 External Trigger remap */
#define SYSCFG_RMPCR2_TIM2TRIG_REMAP    ((uint8_t)0x02) /*!< TIM2 Trigger remap */
#define SYSCFG_RMPCR2_TIM3TRIG_REMAP1   ((uint8_t)0x04) /*!< TIM3 Trigger remap 1 */
#define SYSCFG_RMPCR2_TIM2TRIG_LSE      ((uint8_t)0x08) /*!< TIM2 Trigger remap to LSE */
#define SYSCFG_RMPCR2_TIM3TRIG_LSE      ((uint8_t)0x10) /*!< TIM3 Trigger remap to LSE */
#define SYSCFG_RMPCR2_SPI2_REMAP        ((uint8_t)0x20) /*!< SPI2 remapping */
#define SYSCFG_RMPCR2_TIM3TRIG_REMAP2   ((uint8_t)0x40) /*!< TIM3 Trigger remap 2 */
#define SYSCFG_RMPCR2_TIM23BKIN_REMAP   ((uint8_t)0x80) /*!< TIM2 & TIM3 Break input remap */

#define SYSCFG_RMPCR3_SPI1_REMAP        ((uint8_t)0x01) /*!< SPI1 remapping */
#define SYSCFG_RMPCR3_USART3TR_REMAP    ((uint8_t)0x02) /*!< USART3_TX and USART3_RX remapping */
#define SYSCFG_RMPCR3_USART3CK_REMAP    ((uint8_t)0x04) /*!< USART3_CK remapping */
#define SYSCFG_RMPCR3_TIM3CH1_REMAP     ((uint8_t)0x08) /*!< TIM3 channel 1 remapping */
#define SYSCFG_RMPCR3_TIM3CH2_REMAP     ((uint8_t)0x10) /*!< TIM3 channel 2 remapping */
#define SYSCFG_RMPCR3_CCO_REMAP         ((uint8_t)0x20) /*!< CCO remapping */

/**
  * @}
  */
/*----------------------------------------------------------------------------ok*/

/**
  * @brief Clock Controller (CLK)
  */
typedef struct CLK_struct
{
  __IO uint8_t CKDIVR;      /*!< Clock Master Divider Register */
  __IO uint8_t CRTCR;  /*!< RTC Clock selection Register */
  __IO uint8_t ICKCR;     /*!< Internal Clocks Control Register */
  __IO uint8_t PCKENR1;  /*!< Peripheral Clock Gating Register 1 */
  __IO uint8_t PCKENR2;  /*!< Peripheral Clock Gating Register 2 */
  __IO uint8_t CCOR;       /*!< Configurable Clock Output Register */
  __IO uint8_t ECKCR;     /*!< External Clocks Control Register */
  __IO uint8_t SCSR;     /*!< System clock status Register */
  __IO uint8_t SWR;      /*!< System clock Switch Register */
  __IO uint8_t SWCR;     /*!< Switch Control Register */
  __IO uint8_t CSSR;     /*!< Clock Security Sytem Register */
  __IO uint8_t CBEEPR;     /*!< Clock BEEP Register */
  __IO uint8_t HSICALR;     /*!< HSI Calibration Register */
  __IO uint8_t HSITRIMR; /*!< HSI clock Calibration Trimmer Register */
  __IO uint8_t HSIUNLCKR; /*!< HSI Unlock  Register */
  __IO uint8_t REGCSR;  /*!< Main regulator control status register */
  __IO uint8_t PCKENR3;  /*!< Peripheral Clock Gating Register 3 */
}
CLK_TypeDef;

/** @addtogroup CLK_Registers_Reset_Value
  * @{
  */
#define CLK_CKDIVR_RESET_VALUE    ((uint8_t)0x03)
#define CLK_CRTCR_RESET_VALUE     ((uint8_t)0x00)
#define CLK_ICKCR_RESET_VALUE     ((uint8_t)0x11)
#define CLK_PCKENR1_RESET_VALUE   ((uint8_t)0x00)
#define CLK_PCKENR2_RESET_VALUE   ((uint8_t)0x80)
#define CLK_PCKENR3_RESET_VALUE   ((uint8_t)0x00)
#define CLK_CCOR_RESET_VALUE      ((uint8_t)0x00)
#define CLK_ECKCR_RESET_VALUE     ((uint8_t)0x00)
#define CLK_SCSR_RESET_VALUE      ((uint8_t)0x01)
#define CLK_SWR_RESET_VALUE       ((uint8_t)0x01)
#define CLK_SWCR_RESET_VALUE      ((uint8_t)0x00)
#define CLK_CSSR_RESET_VALUE      ((uint8_t)0x00)
#define CLK_CBEEPR_RESET_VALUE    ((uint8_t)0x00)
#define CLK_HSICALR_RESET_VALUE   ((uint8_t)0x00)
#define CLK_HSITRIMR_RESET_VALUE  ((uint8_t)0x00)
#define CLK_HSIUNLCKR_RESET_VALUE ((uint8_t)0x00)
#define CLK_REGCSR_RESET_VALUE    ((uint8_t)0xB9)
/**
  * @}
  */

/** @addtogroup CLK_Registers_Bits_Definition
  * @{
  */

#define CLK_CKDIVR_CKM            ((uint8_t)0x07) /*!< System clock prescaler mask */

#define CLK_CRTCR_RTCDIV          ((uint8_t)0xE0) /*!< RTC clock prescaler  mask*/
#define CLK_CRTCR_RTCSEL          ((uint8_t)0x1E) /*!< RTC clock output selection  mask */
#define CLK_CRTCR_RTCSWBSY        ((uint8_t)0x01) /*!< RTC clock switch busy */

#define CLK_ICKCR_BEEPAHALT       ((uint8_t)0x40) /*!< BEEP clock Active Halt/Halt mode */
#define CLK_ICKCR_FHWU            ((uint8_t)0x20) /*!< Fast Wake-up from Active Halt/Halt mode */
#define CLK_ICKCR_SAHALT          ((uint8_t)0x10) /*!< Slow Active-halt mode */
#define CLK_ICKCR_LSIRDY          ((uint8_t)0x08) /*!< Low speed internal RC oscillator ready */
#define CLK_ICKCR_LSION           ((uint8_t)0x04) /*!< Low speed internal RC oscillator enable */
#define CLK_ICKCR_HSIRDY          ((uint8_t)0x02) /*!< High speed internal RC oscillator ready */
#define CLK_ICKCR_HSION           ((uint8_t)0x01) /*!< High speed internal RC oscillator enable */

#define CLK_PCKENR1_TIM2          ((uint8_t)0x01) /*!< Timer 2 clock enable */
#define CLK_PCKENR1_TIM3          ((uint8_t)0x02) /*!< Timer 3 clock enable */
#define CLK_PCKENR1_TIM4          ((uint8_t)0x04) /*!< Timer 4 clock enable */
#define CLK_PCKENR1_I2C1          ((uint8_t)0x08) /*!< I2C1 clock enable */
#define CLK_PCKENR1_SPI1          ((uint8_t)0x10) /*!< SPI1 clock enable */
#define CLK_PCKENR1_USART1        ((uint8_t)0x20) /*!< USART1 clock enable */
#define CLK_PCKENR1_BEEP          ((uint8_t)0x40) /*!< BEEP clock enable */
#define CLK_PCKENR1_DAC           ((uint8_t)0x80) /*!< DAC clock enable */

#define CLK_PCKENR2_ADC1          ((uint8_t)0x01) /*!< ADC1 clock enable */
#define CLK_PCKENR2_TIM1          ((uint8_t)0x02) /*!< TIM1 clock enable */
#define CLK_PCKENR2_RTC           ((uint8_t)0x04) /*!< RTC clock enable */
#define CLK_PCKENR2_LCD           ((uint8_t)0x08) /*!< LCD clock enable */
#define CLK_PCKENR2_DMA1          ((uint8_t)0x10) /*!< DMA1 clock enable */
#define CLK_PCKENR2_COMP          ((uint8_t)0x20) /*!< Comparator clock enable */
#define CLK_PCKENR2_BOOTROM       ((uint8_t)0x80) /*!< Boot ROM clock enable */

#define CLK_PCKENR3_AES           ((uint8_t)0x01) /*!< AES clock enable */
#define CLK_PCKENR3_TIM5          ((uint8_t)0x02) /*!< Timer 5 clock enable */
#define CLK_PCKENR3_SPI2          ((uint8_t)0x04) /*!< SPI2 clock enable */
#define CLK_PCKENR3_UASRT2        ((uint8_t)0x08) /*!< USART2 clock enable */
#define CLK_PCKENR3_USART3        ((uint8_t)0x10) /*!< USART3 clock enable */

#define CLK_CCOR_CCODIV           ((uint8_t)0xE0) /*!< Configurable Clock output prescaler */
#define CLK_CCOR_CCOSEL           ((uint8_t)0x1E) /*!< Configurable clock output selection */
#define CLK_CCOR_CCOSWBSY         ((uint8_t)0x01) /*!< Configurable clock output switch busy  flag */

#define CLK_ECKCR_LSEBYP          ((uint8_t)0x20) /*!< Low speed external clock bypass */
#define CLK_ECKCR_HSEBYP          ((uint8_t)0x10) /*!< High speed external clock bypass */
#define CLK_ECKCR_LSERDY          ((uint8_t)0x08) /*!< Low speed external crystal oscillator ready */
#define CLK_ECKCR_LSEON           ((uint8_t)0x04) /*!< Low speed external crystal oscillator enable */
#define CLK_ECKCR_HSERDY          ((uint8_t)0x02) /*!< High speed external crystal oscillator ready */
#define CLK_ECKCR_HSEON           ((uint8_t)0x01) /*!< High speed external crystal oscillator enable */

#define CLK_SCSR_CKM              ((uint8_t)0x0F) /*!< System clock status bits */

#define CLK_SWR_SWI               ((uint8_t)0x0F) /*!< System clock selection bits */

#define CLK_SWCR_SWIF             ((uint8_t)0x08) /*!< Clock switch interrupt flag */
#define CLK_SWCR_SWIEN            ((uint8_t)0x04) /*!< Clock switch interrupt enable */
#define CLK_SWCR_SWEN             ((uint8_t)0x02) /*!< Switch start/stop */
#define CLK_SWCR_SWBSY            ((uint8_t)0x01) /*!< Switch busy */

#define CLK_CSSR_CSSDGON          ((uint8_t)0x10) /*!< Clock security sytem deglitcher system */
#define CLK_CSSR_CSSD             ((uint8_t)0x08) /*!< Clock security sytem detection */
#define CLK_CSSR_CSSDIE           ((uint8_t)0x04) /*!< Clock security system detection interrupt enable */
#define CLK_CSSR_AUX              ((uint8_t)0x02) /*!< Auxiliary oscillator connected to master clock */
#define CLK_CSSR_CSSEN            ((uint8_t)0x01) /*!< Clock security system enable */

#define CLK_CBEEPR_CLKBEEPSEL     ((uint8_t)0x06) /*!< Configurable BEEP clock source selection */
#define CLK_CBEEPR_BEEPSWBSY      ((uint8_t)0x01) /*!< BEEP clock busy in switch  */

#define CLK_HSICALR_HSICAL        ((uint8_t)0xFF) /*!<  Copy of otpion byte trimming HSI oscillator */

#define CLK_HSITRIMR_HSITRIM      ((uint8_t)0xFF) /*!< High speed internal oscillator trimmer */

#define CLK_HSIUNLCKR_HSIUNLCK    ((uint8_t)0xFF) /*!< High speed internal oscillator trimmer unlock  */

#define CLK_REGCSR_EEREADY        ((uint8_t)0x80) /*!< Flash program memory and Data EEPROM ready */
#define CLK_REGCSR_EEBUSY         ((uint8_t)0x40) /*!< Flash program memory and Data EEPROM busy */
#define CLK_REGCSR_LSEPD          ((uint8_t)0x20) /*!< LSE power-down */
#define CLK_REGCSR_HSEPD          ((uint8_t)0x10) /*!< HSE power-down */
#define CLK_REGCSR_LSIPD          ((uint8_t)0x08) /*!< LSI power-down */
#define CLK_REGCSR_HSIPD          ((uint8_t)0x04) /*!< HSI power-down */
#define CLK_REGCSR_REGOFF         ((uint8_t)0x02) /*!< Main regulator OFF */
#define CLK_REGCSR_REGREADY       ((uint8_t)0x01) /*!< Main regulator ready */

/**
  * @}
  */
/*----------------------------------------------------------------------------ok*/

/**
  * @brief Comparator interface (COMP)
  */

typedef struct COMP_struct
{
  __IO uint8_t CSR1;   /*!< Control status register 1  */
  __IO uint8_t CSR2;   /*!< Control status register 2  */
  __IO uint8_t CSR3;   /*!< Control status register 3  */
  __IO uint8_t CSR4;   /*!< Control status register 4  */
  __IO uint8_t CSR5;   /*!< Control status register 5  */
}
COMP_TypeDef;


/** @addtogroup COMP_Registers_Reset_Value
  * @{
  */
#define COMP_CSR1_RESET_VALUE  ((uint8_t)0x00)
#define COMP_CSR2_RESET_VALUE  ((uint8_t)0x00)
#define COMP_CSR3_RESET_VALUE  ((uint8_t)0xC0)
#define COMP_CSR4_RESET_VALUE  ((uint8_t)0x00)
#define COMP_CSR5_RESET_VALUE  ((uint8_t)0x00)

/**
  * @}
  */

/** @addtogroup COMP_Registers_Bits_Definition
  * @{
  */

/* CSR1 */
#define COMP_CSR1_IE1        ((uint8_t)0x20) /*!<  Comparator 1 Interrupt Enable Mask. */
#define COMP_CSR1_EF1        ((uint8_t)0x10) /*!<  Comparator 1 Event Flag Mask. */
#define COMP_CSR1_CMP1OUT    ((uint8_t)0x08) /*!<  Comparator 1 Ouptput Mask. */
#define COMP_CSR1_STE        ((uint8_t)0x04) /*!<  Schmitt trigger enable Mask. */
#define COMP_CSR1_CMP1       ((uint8_t)0x03) /*!<  Comparator 1 Configuration Mask. */

/* CSR2 */
#define COMP_CSR2_IE2        ((uint8_t)0x20) /*!<  Comparator 2 Interrupt Enable Mask. */
#define COMP_CSR2_EF2        ((uint8_t)0x10) /*!<  Comparator 2 Event Flag Mask. */
#define COMP_CSR2_CMP2OUT    ((uint8_t)0x08) /*!<  Comparator 2 Ouptput Mask. */
#define COMP_CSR2_SPEED      ((uint8_t)0x04) /*!<  Comparator 2 speed modeMask. */
#define COMP_CSR2_CMP2       ((uint8_t)0x03) /*!<  Comparator 2 Configuration Mask. */

/* CSR3 */
#define COMP_CSR3_OUTSEL     ((uint8_t)0xC0) /*!<  Comparator 2 output selection Mask. */
#define COMP_CSR3_INSEL      ((uint8_t)0x38) /*!<  Inversion input selection Mask. */
#define COMP_CSR3_VREFEN     ((uint8_t)0x04) /*!<  Internal reference voltage Enable Mask. */
#define COMP_CSR3_WNDWE      ((uint8_t)0x02) /*!<  Window Mode Enable Mask. */
#define COMP_CSR3_VREFOUTEN  ((uint8_t)0x01) /*!<  VREF Output Enable Mask. */

/* CSR4 */
#define COMP_CSR4_NINVTRIG   ((uint8_t)0x38) /*!<  COMP2 non-inverting input Mask. */
#define COMP_CSR4_INVTRIG    ((uint8_t)0x07) /*!<  COMP2 inverting input Mask. */

/* CSR5 */
#define COMP_CSR5_DACTRIG    ((uint8_t)0x38) /*!<  DAC outputs Mask. */
#define COMP_CSR5_VREFTRIG   ((uint8_t)0x07) /*!<  VREF outputs Mask. */

/**
  * @}
  */

/*----------------------------------------------------------------------------ok*/

/**
  * @brief External Interrupt Controller (EXTI)
  */
typedef struct EXTI_struct
{
  __IO uint8_t CR1;           /*!<  The four LSB EXTI  pin sensitivity */
  __IO uint8_t CR2;           /*!<  The four MSB EXTI  pin sensitivity */
  __IO uint8_t CR3;           /*!<  EXTI port B & port D sensitivity */
  __IO uint8_t SR1;           /*!<  Pins Status flag register 1 */
  __IO uint8_t SR2;           /*!<  Ports Status flage register 2 */
  __IO uint8_t CONF1;         /*!<  Port interrupt selector */
  uint8_t RESERVED[4];  /*!<  reserved area */
  __IO uint8_t CR4;           /*!<  EXTI port G & port H sensitivity */
  __IO uint8_t CONF2;         /*!<  Port interrupt selector */
}
EXTI_TypeDef;

/** @addtogroup EXTI_Registers_Reset_Value
  * @{
  */

#define EXTI_CR1_RESET_VALUE   ((uint8_t)0x00)
#define EXTI_CR2_RESET_VALUE   ((uint8_t)0x00)
#define EXTI_CR3_RESET_VALUE   ((uint8_t)0x00)
#define EXTI_CONF1_RESET_VALUE ((uint8_t)0x00)
#define EXTI_SR1_RESET_VALUE   ((uint8_t)0x00)
#define EXTI_SR2_RESET_VALUE   ((uint8_t)0x00)
#define EXTI_CR4_RESET_VALUE   ((uint8_t)0x00)
#define EXTI_CONF2_RESET_VALUE ((uint8_t)0x00)

/**
  * @}
  */

/** @addtogroup EXTI_Registers_Bits_Definition
  * @{
  */
/* CR1 */
#define EXTI_CR1_P3IS      ((uint8_t)0xC0) /*!< EXTI Pin 3 external interrupt sensitivity bit Mask */
#define EXTI_CR1_P2IS      ((uint8_t)0x30) /*!< EXTI Pin 2 external interrupt sensitivity bit Mask */
#define EXTI_CR1_P1IS      ((uint8_t)0x0C) /*!< EXTI Pin 1  external interrupt sensitivity bit Mask */
#define EXTI_CR1_P0IS      ((uint8_t)0x03) /*!< EXTI Pin 0 external interrupt sensitivity bit Mask */

/* CR2 */
#define EXTI_CR2_P7IS      ((uint8_t)0xC0) /*!< EXTI Pin 7 external interrupt sensitivity bit Mask */
#define EXTI_CR2_P6IS      ((uint8_t)0x30) /*!< EXTI Pin 6 external interrupt sensitivity bit Mask */
#define EXTI_CR2_P5IS      ((uint8_t)0x0C) /*!< EXTI Pin 5  external interrupt sensitivity bit Mask */
#define EXTI_CR2_P4IS      ((uint8_t)0x03) /*!< EXTI Pin 4 external interrupt sensitivity bit Mask */

/* CR3 */
#define EXTI_CR3_PBIS      ((uint8_t)0x03) /*!< EXTI PORTB external interrupt sensitivity bits Mask */
#define EXTI_CR3_PDIS      ((uint8_t)0x0C) /*!< EXTI PORTD external interrupt sensitivity bits Mask */
#define EXTI_CR3_PEIS      ((uint8_t)0x30) /*!< EXTI PORTE external interrupt sensitivity bits Mask */
#define EXTI_CR3_PFIS      ((uint8_t)0xC0) /*!< EXTI PORTF external interrupt sensitivity bits Mask */

/* CONF1 */
#define EXTI_CONF1_PBLIS   ((uint8_t)0x01) /*!< EXTI PORTB low interrupt selector bit Mask */
#define EXTI_CONF1_PBHIS   ((uint8_t)0x02) /*!< EXTI PORTB high interrupt selector bit Mask */
#define EXTI_CONF1_PDLIS   ((uint8_t)0x04) /*!< EXTI PORTD low interrupt selector bit Mask */
#define EXTI_CONF1_PDHIS   ((uint8_t)0x08) /*!< EXTI PORTD high interrupt selector bit Mask */
#define EXTI_CONF1_PELIS   ((uint8_t)0x10) /*!< EXTI PORTE low interrupt selector bit Mask */
#define EXTI_CONF1_PEHIS   ((uint8_t)0x20) /*!< EXTI PORTE high interrupt selector bit Mask */
#define EXTI_CONF1_PFLIS   ((uint8_t)0x40) /*!< EXTI PORTF low interrupt selector bit Mask */
#define EXTI_CONF1_PFES    ((uint8_t)0x80) /*!< EXTI PORTF or PORTE interrupt selector bit Mask */

/* CR4 */
#define EXTI_CR4_PGIS      ((uint8_t)0x03) /*!< EXTI PORTG external interrupt sensitivity bits Mask */
#define EXTI_CR4_PHIS      ((uint8_t)0x0C) /*!< EXTI PORTH external interrupt sensitivity bits Mask */

/* CONF2 */
#define EXTI_CONF2_PFHIS   ((uint8_t)0x01) /*!< EXTI PORTF high interrupt selector bit Mask */
#define EXTI_CONF2_PGLIS   ((uint8_t)0x02) /*!< EXTI PORTG low interrupt selector bit Mask */
#define EXTI_CONF2_PGHIS   ((uint8_t)0x04) /*!< EXTI PORTG high interrupt selector bit Mask */
#define EXTI_CONF2_PHLIS   ((uint8_t)0x08) /*!< EXTI PORTH low interrupt selector bit Mask */
#define EXTI_CONF2_PHHIS   ((uint8_t)0x10) /*!< EXTI PORTH high interrupt selector bit Mask */
#define EXTI_CONF2_PGBS    ((uint8_t)0x20) /*!< EXTI PORTB or PORTG interrupt selector bit Mask */
#define EXTI_CONF2_PHDS    ((uint8_t)0x40) /*!< EXTI PORTD or PORTH interrupt selector bit Mask */

/**
  * @}
  */

/*----------------------------------------------------------------------------ok*/

/**
  * @brief FLASH and Data EEPROM
  */
typedef struct FLASH_struct
{
  __IO uint8_t CR1;        /*!< Flash control register 1 */
  __IO uint8_t CR2;        /*!< Flash control register 2 */
  __IO uint8_t PUKR;       /*!< Flash program memory unprotection register */
  __IO uint8_t DUKR;       /*!< Data EEPROM unprotection register */
  __IO uint8_t IAPSR;      /*!< Flash in-application programming status register */
}
FLASH_TypeDef;

/** @addtogroup FLASH_Registers_Reset_Value
  * @{
  */
#define FLASH_CR1_RESET_VALUE    ((uint8_t)0x00)
#define FLASH_CR2_RESET_VALUE    ((uint8_t)0x00)
#define FLASH_PUKR_RESET_VALUE   ((uint8_t)0xAE)
#define FLASH_DUKR_RESET_VALUE   ((uint8_t)0x56)
#define FLASH_IAPSR_RESET_VALUE  ((uint8_t)0x40)


/**
  * @}
  */

/** @addtogroup FLASH_Registers_Bits_Definition
  * @{
  */
#define FLASH_CR1_EEPM           ((uint8_t)0x08) /*!< Flash low power selection during Run and Low power run mode Mask */
#define FLASH_CR1_WAITM          ((uint8_t)0x04) /*!< Flash low power selection during Wait and Low power wait mode Mask */
#define FLASH_CR1_IE             ((uint8_t)0x02) /*!< Flash Interrupt enable Mask */
#define FLASH_CR1_FIX            ((uint8_t)0x01) /*!< Fix programming time Mask */

#define FLASH_CR2_OPT            ((uint8_t)0x80) /*!< Enable write access to option bytes*/
#define FLASH_CR2_WPRG           ((uint8_t)0x40) /*!< Word write once Mask */
#define FLASH_CR2_ERASE          ((uint8_t)0x20) /*!< Erase block Mask */
#define FLASH_CR2_FPRG           ((uint8_t)0x10) /*!< Fast programming mode Mask */
#define FLASH_CR2_PRG            ((uint8_t)0x01) /*!< Program block Mask */

#define FLASH_IAPSR_HVOFF        ((uint8_t)0x40) /*!< End of high voltage flag Mask */
#define FLASH_IAPSR_DUL          ((uint8_t)0x08) /*!< Data EEPROM unlocked flag Mask */
#define FLASH_IAPSR_EOP          ((uint8_t)0x04) /*!< End of operation flag Mask */
#define FLASH_IAPSR_PUL          ((uint8_t)0x02) /*!< Program memory unlocked flag Mask */
#define FLASH_IAPSR_WR_PG_DIS    ((uint8_t)0x01) /*!< Write attempted to protected page Mask */

#define FLASH_PUKR_PUK           ((uint8_t)0xFF) /*!< Flash Program memory unprotection mask */

#define FLASH_DUKR_DUK           ((uint8_t)0xFF) /*!< Data EEPROM unprotection mask */


/**
  * @}
  */

/*----------------------------------------------------------------------------*/

/**
  * @brief Inter-Integrated Circuit (I2C)
  */
typedef struct I2C_struct
{
  __IO uint8_t CR1;    /*!< I2C control register 1 */
  __IO uint8_t CR2;    /*!< I2C control register 2 */
  __IO uint8_t FREQR;  /*!< I2C frequency register */
  __IO uint8_t OARL;   /*!< I2C own address register 1 LSB */
  __IO uint8_t OARH;   /*!< I2C own address register 1 MSB */
  __IO uint8_t OAR2;   /*!< I2C own address register 2 */
  __IO uint8_t DR;     /*!< I2C data register */
  __IO uint8_t SR1;    /*!< I2C status register 1 */
  __IO uint8_t SR2;    /*!< I2C status register 2 */
  __IO uint8_t SR3;    /*!< I2C status register 3 */
  __IO uint8_t ITR;    /*!< I2C interrupt & DMA register */
  __IO uint8_t CCRL;   /*!< I2C clock control register low */
  __IO uint8_t CCRH;   /*!< I2C clock control register high */
  __IO uint8_t TRISER; /*!< I2C maximum rise time register */
  __IO uint8_t PECR;   /*!< I2CPacket Error Checking register */
}
I2C_TypeDef;

/** @addtogroup I2C_Registers_Reset_Value
  * @{
  */
#define I2C_CR1_RESET_VALUE    ((uint8_t)0x00)
#define I2C_CR2_RESET_VALUE    ((uint8_t)0x00)
#define I2C_FREQR_RESET_VALUE  ((uint8_t)0x00)
#define I2C_OARL_RESET_VALUE   ((uint8_t)0x00)
#define I2C_OARH_RESET_VALUE   ((uint8_t)0x00)
#define I2C_OAR2_RESET_VALUE   ((uint8_t)0x00)
#define I2C_DR_RESET_VALUE     ((uint8_t)0x00)
#define I2C_SR1_RESET_VALUE    ((uint8_t)0x00)
#define I2C_SR2_RESET_VALUE    ((uint8_t)0x00)
#define I2C_SR3_RESET_VALUE    ((uint8_t)0x00)
#define I2C_ITR_RESET_VALUE    ((uint8_t)0x00)
#define I2C_CCRL_RESET_VALUE   ((uint8_t)0x00)
#define I2C_CCRH_RESET_VALUE   ((uint8_t)0x00)
#define I2C_TRISER_RESET_VALUE ((uint8_t)0x02)
#define I2C_PECR_RESET_VALUE   ((uint8_t)0x00)

/**
  * @}
  */

/** @addtogroup I2C_Registers_Bits_Definition
  * @{
  */

#define I2C_CR1_NOSTRETCH   ((uint8_t)0x80) /*!< Clock Stretching Disable (Slave mode) */
#define I2C_CR1_ENGC        ((uint8_t)0x40) /*!< General Call Enable */
#define I2C_CR1_ENPEC       ((uint8_t)0x20) /*!< PEC Enable */
#define I2C_CR1_ARP         ((uint8_t)0x10) /*!< ARP Enable */
#define I2C_CR1_SMBTYPE     ((uint8_t)0x08) /*!< SMBus type */
#define I2C_CR1_SMBUS       ((uint8_t)0x02) /*!< SMBus mode */
#define I2C_CR1_PE          ((uint8_t)0x01) /*!< Peripheral Enable */

#define I2C_CR2_SWRST       ((uint8_t)0x80) /*!< Software Reset */
#define I2C_CR2_ALERT       ((uint8_t)0x20) /*!< SMBus Alert*/
#define I2C_CR2_PEC         ((uint8_t)0x10) /*!< Packet Error Checking */
#define I2C_CR2_POS         ((uint8_t)0x08) /*!< Acknowledge */
#define I2C_CR2_ACK         ((uint8_t)0x04) /*!< Acknowledge Enable */
#define I2C_CR2_STOP        ((uint8_t)0x02) /*!< Stop Generation */
#define I2C_CR2_START       ((uint8_t)0x01) /*!< Start Generation */

#define I2C_FREQR_FREQ      ((uint8_t)0x3F) /*!< Peripheral Clock Frequency */

#define I2C_OARL_ADD        ((uint8_t)0xFE) /*!< Interface Address bits [7..1] */
#define I2C_OARL_ADD0       ((uint8_t)0x01) /*!< Interface Address bit0 */

#define I2C_OARH_ADDMODE    ((uint8_t)0x80) /*!< Addressing Mode (Slave mode) */
#define I2C_OARH_ADDCONF    ((uint8_t)0x40) /*!< Address mode configuration */
#define I2C_OARH_ADD        ((uint8_t)0x06) /*!< Interface Address bits [9..8] */

#define I2C_OAR2_ADD2       ((uint8_t)0xFE) /*!< Interface Address bits [7..1] */
#define I2C_OAR2_ENDUAL     ((uint8_t)0x01) /*!< Dual addressing mode enable */

#define I2C_DR_DR           ((uint8_t)0xFF) /*!< Data Register */

#define I2C_SR1_TXE         ((uint8_t)0x80) /*!< Data Register Empty (transmitters) */
#define I2C_SR1_RXNE        ((uint8_t)0x40) /*!< Data Register not Empty (receivers) */
#define I2C_SR1_STOPF       ((uint8_t)0x10) /*!< Stop detection (Slave mode) */
#define I2C_SR1_ADD10       ((uint8_t)0x08) /*!< 10-bit header sent (Master mode) */
#define I2C_SR1_BTF         ((uint8_t)0x04) /*!< Byte Transfer Finished */
#define I2C_SR1_ADDR        ((uint8_t)0x02) /*!< Address sent (master mode)/matched (slave mode) */
#define I2C_SR1_SB          ((uint8_t)0x01) /*!< Start Bit (Master mode) */

#define I2C_SR2_SMBALERT    ((uint8_t)0x80) /*!< SMBus Alert */
#define I2C_SR2_TIMEOUT     ((uint8_t)0x40) /*!< Time out or TLow error */
#define I2C_SR2_WUFH        ((uint8_t)0x20) /*!< Wake-up from Halt */
#define I2C_SR2_PECERR      ((uint8_t)0x10) /*!< PEC error in reception */
#define I2C_SR2_OVR         ((uint8_t)0x08) /*!< Overrun/Underrun */
#define I2C_SR2_AF          ((uint8_t)0x04) /*!< Acknowledge Failure */
#define I2C_SR2_ARLO        ((uint8_t)0x02) /*!< Arbitration Lost (master mode) */
#define I2C_SR2_BERR        ((uint8_t)0x01) /*!< Bus Error */

#define I2C_SR3_DUALF       ((uint8_t)0x80) /*!< Dual flag (Slave mode) */
#define I2C_SR3_SMBHOST     ((uint8_t)0x40) /*!< SMBus Host Header (Slave mode) */
#define I2C_SR3_SMBDEFAULT  ((uint8_t)0x20) /*!< SMBus Default Header (Slave mode) */
#define I2C_SR3_GENCALL     ((uint8_t)0x10) /*!< General Call Header (Slave mode) */
#define I2C_SR3_TRA         ((uint8_t)0x04) /*!< Transmitter/Receiver */
#define I2C_SR3_BUSY        ((uint8_t)0x02) /*!< Bus Busy */
#define I2C_SR3_MSL         ((uint8_t)0x01) /*!< Master/Slave */

#define I2C_ITR_LAST        ((uint8_t)0x10) /*!< DMA Last transfer */
#define I2C_ITR_DMAEN       ((uint8_t)0x08) /*!< DMA request Enable */
#define I2C_ITR_ITBUFEN     ((uint8_t)0x04) /*!< Buffer Interrupt Enable */
#define I2C_ITR_ITEVTEN     ((uint8_t)0x02) /*!< Event Interrupt Enable */
#define I2C_ITR_ITERREN     ((uint8_t)0x01) /*!< Error Interrupt Enable */

#define I2C_CCRL_CCR        ((uint8_t)0xFF) /*!< Clock Control Register (Master mode) */

#define I2C_CCRH_FS         ((uint8_t)0x80) /*!< Master Mode Selection */
#define I2C_CCRH_DUTY       ((uint8_t)0x40) /*!< Fast Mode Duty Cycle */
#define I2C_CCRH_CCR        ((uint8_t)0x0F) /*!< Clock Control Register in Fast/Standard mode (Master mode) bits [11..8] */

#define I2C_TRISER_TRISE    ((uint8_t)0x3F) /*!< Maximum Rise Time in Fast/Standard mode (Master mode) */

#define I2C_PECR_PEC        ((uint8_t)0xFF) /*!< Packet error checking */

/**
  * @}
  */

/*----------------------------------------------------------------------------*/

/**
  * @brief IR digital interface (IRTIM)
  */
typedef struct IRTIM_struct
{
  __IO uint8_t CR; /*!< control register */
}
IRTIM_TypeDef;
/** @addtogroup IRTIM_Registers_Reset_Value
  * @{
  */
#define IRTIM_CR_RESET_VALUE    ((uint8_t)0x00)


/**
* @}
*/

/** @addtogroup IRTIM_Registers_Bits_Definition
  * @{
  */
/* CR*/
#define IRTIM_CR_EN     ((uint8_t)0x01) /*!< IRTIM_OUT enable Mask. */
#define IRTIM_CR_HSEN   ((uint8_t)0x02) /*!< High sink open drain buffer enable Mask */

/**
  * @}
  */

/*----------------------------------------------------------------------------*/

/**
  * @brief Interrupt Controller (ITC)
  */
typedef struct ITC_struct
{
  __IO uint8_t ISPR1; /*!<  Interrupt Software Priority register 1 */
  __IO uint8_t ISPR2; /*!<  Interrupt Software Priority register 2 */
  __IO uint8_t ISPR3; /*!<  Interrupt Software Priority register 3 */
  __IO uint8_t ISPR4; /*!<  Interrupt Software Priority register 4 */
  __IO uint8_t ISPR5; /*!<  Interrupt Software Priority register 5 */
  __IO uint8_t ISPR6; /*!<  Interrupt Software Priority register 6 */
  __IO uint8_t ISPR7; /*!<  Interrupt Software Priority register 7 */
  __IO uint8_t ISPR8; /*!<  Interrupt Software Priority register 8 */
}
ITC_TypeDef;

/** @addtogroup ITC_Registers_Reset_Value
  * @{
  */
#define ITC_SPRX_RESET_VALUE ((uint8_t)0xFF) /*!< Reset value of Software Priority registers 0 to 7 */
/**
  * @}
  */

/*----------------------------------------------------------------------------*/

/**
  * @brief Internal Low Speed Watchdog (IWDG)
  */
typedef struct IWDG_struct
{
  __IO uint8_t KR;  /*!<  Low Speed Watchdog Key Register */
  __IO uint8_t PR;  /*!<  Low Speed Watchdog Prescaler Register */
  __IO uint8_t RLR; /*!<  Low Speed Watchdog Reload Register */
}
IWDG_TypeDef;

/** @addtogroup IWDG_Registers_Reset_Value
  * @{
  */
#define IWDG_RLR_RESET_VALUE    ((uint8_t)0xFF)  /*! <Reload Register Default Value */
#define IWDG_PR_RESET_VALUE     ((uint8_t)0x00)  /*! <Prescaler Register Default Value */
/**
  * @}
  */
/*----------------------------------------------------------------------------*/


/**
  * @brief Wait For Event (WFE) peripheral registers.
  */
/** @addtogroup WFE_Registers
  * @{
  */
typedef struct WFE_struct
{
  __IO uint8_t CR1;  /*!<  Wait for event control register 1 */
  __IO uint8_t CR2;  /*!<  Wait for event control register 2 */
  __IO uint8_t CR3;  /*!<  Wait for event control register 3 */
  __IO uint8_t CR4;  /*!<  Wait for event control register 4 */
}
WFE_TypeDef;
/**
  * @}
  */
/** @addtogroup WFE_Registers_Reset_Value
  * @{
  */

#define WFE_CRX_RESET_VALUE ((uint8_t)0x00) /*!< Reset value wait for event control register */
/**
  * @}
  */

/** @addtogroup WFE_Registers_Bits_Definition
  * @{
  */

#define WFE_CR1_EXTI_EV3      ((uint8_t)0x80)  /*!< External interrupt event 3 Mask */
#define WFE_CR1_EXTI_EV2      ((uint8_t)0x40)  /*!< External interrupt event 2 Mask */
#define WFE_CR1_EXTI_EV1      ((uint8_t)0x20)  /*!< External interrupt event 1 Mask */
#define WFE_CR1_EXTI_EV0      ((uint8_t)0x10)  /*!< External interrupt event 0 Mask */
#define WFE_CR1_TIM1_EV1      ((uint8_t)0x08)  /*!< TIM1 event 1 Mask */
#define WFE_CR1_TIM1_EV0      ((uint8_t)0x04)  /*!< TIM1 event 0 Mask */
#define WFE_CR1_TIM2_EV1      ((uint8_t)0x02)  /*!< TIM2 event 1 Mask */
#define WFE_CR1_TIM2_EV0      ((uint8_t)0x01)  /*!< TIM2 event 0 Mask */

#define WFE_CR2_ADC1_COMP_EV  ((uint8_t)0x80)  /*!< ADC / COMP event Mask*/
#define WFE_CR2_EXTI_EVEF     ((uint8_t)0x40)  /*!< External interrupt event on Port E or Port F Mask */
#define WFE_CR2_EXTI_EVDH     ((uint8_t)0x20)  /*!< External interrupt event on Port D or Port H Mask */
#define WFE_CR2_EXTI_EVBG     ((uint8_t)0x10)  /*!< External interrupt event on Port B or Port G Mask */
#define WFE_CR2_EXTI_EV7      ((uint8_t)0x08)  /*!< External interrupt event 7 Mask */
#define WFE_CR2_EXTI_EV6      ((uint8_t)0x04)  /*!< External interrupt event 6 Mask */
#define WFE_CR2_EXTI_EV5      ((uint8_t)0x02)  /*!< External interrupt event 5 Mask */
#define WFE_CR2_EXTI_EV4      ((uint8_t)0x01)  /*!< External interrupt event 4 Mask */

#define WFE_CR3_DMA1CH23_EV   ((uint8_t)0x80)  /*!< DMA1 channel 2 and 3 interrupt event Mask */
#define WFE_CR3_DMA1CH01_EV   ((uint8_t)0x40)  /*!< DMA1 channel 0 and 1 interrupt event Mask */
#define WFE_CR3_USART1_EV     ((uint8_t)0x20)  /*!< USART1 Rx and Tx interrupt event Mask */
#define WFE_CR3_I2C1_EV       ((uint8_t)0x10)  /*!< I2C1 Rx and Tx interrupt event Mask */
#define WFE_CR3_SPI1_EV       ((uint8_t)0x08)  /*!< SPI1 Rx and Tx interrupt event Mask */
#define WFE_CR3_TIM4_EV       ((uint8_t)0x04)  /*!< TIM4 event Mask */
#define WFE_CR3_TIM3_EV1      ((uint8_t)0x02)  /*!< TIM3 event 1 Mask */
#define WFE_CR3_TIM3_EV0      ((uint8_t)0x01)  /*!< TIM3 event 0 Mask */

#define WFE_CR4_AES_EV        ((uint8_t)0x40)  /*!< AES event Mask */
#define WFE_CR4_TIM5_EV1      ((uint8_t)0x20)  /*!< TIM5 event 1 Mask */
#define WFE_CR4_TIM5_EV0      ((uint8_t)0x10)  /*!< TIM5 event 0 Mask */
#define WFE_CR4_USART3_EV     ((uint8_t)0x08)  /*!< USART3 Rx and Tx interrupt event Mask */
#define WFE_CR4_USART2_EV     ((uint8_t)0x04)  /*!< USART2 Rx and Tx interrupt event Mask */
#define WFE_CR4_SPI2_EV       ((uint8_t)0x02)  /*!< SPI2 Rx and Tx interrupt event Mask */
#define WFE_CR4_RTC_CSS_EV    ((uint8_t)0x01)  /*!< RTC or CSS on LSE interrupt event Mask */


/**
  * @}
  */

/*----------------------------------------------------------------------------*/
/**
  * @brief Option Bytes (OPT)
  */
typedef struct OPT_struct
{
  __IO uint8_t ROP;        /*!< Read-out protection*/
  uint8_t  RESERVED1;
  __IO uint8_t UBC;        /*!< User Boot code size*/
  uint8_t  RESERVED2;
  uint8_t  RESERVED3;
  uint8_t  RESERVED4;
  uint8_t  RESERVED5;
  __IO uint8_t PCODESIZE;
  __IO uint8_t WDG;        /*!< Independent and Window watchdog option */
  __IO uint8_t XTSTARTUP;  /*!< HSE and LSE option */
  __IO uint8_t BOR;        /*!< Brownout option */
}
OPT_TypeDef;

/**
  * @}
  */
/*----------------------------------------------------------------------------*/

/**
  * @brief Reset Controller (RST)
  */
typedef struct RST_struct
{
  __IO uint8_t CR;  /*!< Multiplex Reset Pad */
  __IO uint8_t SR;  /*!< Reset status register */

}
RST_TypeDef;
/**
  * @}
  */

/** @addtogroup RST_Registers_Reset_Value
  * @{
  */

#define RST_CR_RESET_VALUE    ((uint8_t)0x00) /*!< Reset pin configuration register  1 reset value */
#define RST_SR_RESET_VALUE    ((uint8_t)0x01) /*!< Reset status register 2 reset value */

/**
  * @}
  */

/** @addtogroup RST_Registers_Bits_Definition
  * @{
  */
#define RST_SR_BORF           ((uint8_t)0x20) /*!< Brownout reset flag mask           */
#define RST_SR_WWDGF          ((uint8_t)0x10) /*!< Window Watchdog reset flag mask */
#define RST_SR_SWIMF          ((uint8_t)0x08) /*!< SWIM reset flag mask            */
#define RST_SR_ILLOPF         ((uint8_t)0x04) /*!< Illegal opcode reset flag mask    */
#define RST_SR_IWDGF          ((uint8_t)0x02) /*!< Independent Watchdog reset flag mask    */
#define RST_SR_PORF           ((uint8_t)0x01) /*!< Power On Reset (POR) flag mask    */

/**
  * @}
  */
/*----------------------------------------------------------------------------*/

/**
  * @brief Power Control (PWR)
  */
typedef struct PWR_struct
{
  __IO uint8_t CSR1;          /*!< PWR control status register 1 */
  __IO uint8_t CSR2;          /*!< PWR control status register 2 */

}
PWR_TypeDef;
/**
  * @}
  */

/** @addtogroup PWR_Registers_Reset_Value
  * @{
  */

#define PWR_CSR1_RESET_VALUE    ((uint8_t)0x00) /*!< Control Status Register 1 reset value */
#define PWR_CSR2_RESET_VALUE    ((uint8_t)0x00) /*!< Control Status Register 2 reset value */

/**
  * @}
  */

/** @addtogroup PWR_Registers_Bits_Definition
  * @{
  */
#define PWR_CSR1_PVDOF          ((uint8_t)0x40) /*!< PVD output flag mask         */
#define PWR_CSR1_PVDIF          ((uint8_t)0x20) /*!< PVD interrupt flag mask */
#define PWR_CSR1_PVDIEN         ((uint8_t)0x10) /*!< PVD interrupt enable mask            */
#define PWR_CSR1_PLS            ((uint8_t)0x0E) /*!< PVD Level thresholds selector mask    */
#define PWR_CSR1_PVDE           ((uint8_t)0x01) /*!< Power Voltage Detector (PVD) enable mask */


#define PWR_CSR2_FWU            ((uint8_t)0x04) /*!< Fast wake up configuration mask */
#define PWR_CSR2_ULP            ((uint8_t)0x02) /*!< Ultra Low power configuration mask  */
#define PWR_CR2_VREFINTF        ((uint8_t)0x01) /*!< Internal reference voltage status flag mask  */
/**
  * @}
  */
/*----------------------------------------------------------------------------*/

/**
  * @brief Routing Interface (RI)
  */
typedef struct RI_struct
{
  uint8_t RESERVED;
  __IO uint8_t ICR1;   /*!< Timer input capture routing register 1 */
  __IO uint8_t ICR2;   /*!< Timer input capture routing register 2 */
  __IO uint8_t IOIR1;  /*!< I/O input register 1 */
  __IO uint8_t IOIR2;  /*!< I/O input register 2 */
  __IO uint8_t IOIR3;  /*!< I/O input register 3 */
  __IO uint8_t IOCMR1; /*!< I/O control mode register 1 */
  __IO uint8_t IOCMR2; /*!< I/O control mode register 2 */
  __IO uint8_t IOCMR3; /*!< I/O control mode register 3 */
  __IO uint8_t IOSR1;  /*!< I/O switch register 1*/
  __IO uint8_t IOSR2;  /*!< I/O switch register 2*/
  __IO uint8_t IOSR3;  /*!< I/O switch register 3*/
  __IO uint8_t IOGCR;  /*!< I/O group control register */
  __IO uint8_t ASCR1;  /*!< Analog Switch Control register 1 */
  __IO uint8_t ASCR2;  /*!< Analog Switch Control register 2 */
  __IO uint8_t RCR;    /*!< Resistor control register  */
  uint8_t RESERVED1[16];
  __IO uint8_t CR;     /*!< Control Register  */
  __IO uint8_t IOMR1;   /*!< IO Mask Register 1 */
  __IO uint8_t IOMR2;   /*!< IO Mask Register 2 */
  __IO uint8_t IOMR3;   /*!< IO Mask Register 3 */
  __IO uint8_t IOMR4;   /*!< IO Mask Register 4*/
  __IO uint8_t IOIR4;  /*!< I/O input register 4 */
  __IO uint8_t IOCMR4; /*!< I/O control mode register 4 */
  __IO uint8_t IOSR4;  /*!< I/O switch register 4 */
}RI_TypeDef;
/**
  * @}
  */

/** @addtogroup RI_Registers_Reset_Value
  * @{
  */

#define RI_ICR1_RESET_VALUE     ((uint8_t)0x00) /*!< Timer input capture routing register 1 reset value */
#define RI_ICR2_RESET_VALUE     ((uint8_t)0x00) /*!< Timer input capture routing register 2 reset value */

#define RI_IOCMR1_RESET_VALUE   ((uint8_t)0x00) /*!< I/O control mode register 1 reset value */
#define RI_IOCMR2_RESET_VALUE   ((uint8_t)0x00) /*!< I/O control mode register 2 reset value */
#define RI_IOCMR3_RESET_VALUE   ((uint8_t)0x00) /*!< I/O control mode register 3 reset value */

#define RI_IOSR1_RESET_VALUE    ((uint8_t)0x00) /*!< I/O switch register 1 reset value */
#define RI_IOSR2_RESET_VALUE    ((uint8_t)0x00) /*!< I/O switch register 2 reset value */
#define RI_IOSR3_RESET_VALUE    ((uint8_t)0x00) /*!< I/O switch register 3 reset value */

#define RI_IOGCR_RESET_VALUE    ((uint8_t)0xFF) /*!< IO group control register reset value */

#define RI_ASCR1_RESET_VALUE    ((uint8_t)0x00) /*!< Analog switch register 1 reset value */
#define RI_ASCR2_RESET_VALUE    ((uint8_t)0x00) /*!< Analog switch register 2 reset value */
#define RI_RCR_RESET_VALUE      ((uint8_t)0x00) /*!< Resistor control register reset value */

#define RI_IOCMR4_RESET_VALUE   ((uint8_t)0x00) /*!< I/O control mode register 4 reset value */
#define RI_IOSR4_RESET_VALUE    ((uint8_t)0x00) /*!< I/O switch register 4 reset value */

/**
  * @}
  */

/** @addtogroup RI_Registers_Bits_Definition
  * @{
  */
#define RI_ICR1_IC2CS          ((uint8_t)0x1F) /*!< TIM1 Input Capture 2 I/O selection mask */

#define RI_ICR2_IC3CS          ((uint8_t)0x1F) /*!< TIM1 Input Capture 3 I/O selection mask */

#define RI_IOIR1_CH1I          ((uint8_t)0x01) /*!< Channel 1 I/O pin input value */
#define RI_IOIR1_CH4I          ((uint8_t)0x02) /*!< Channel 4 I/O pin input value */
#define RI_IOIR1_CH7I          ((uint8_t)0x04) /*!< Channel 7 I/O pin input value */
#define RI_IOIR1_CH10I         ((uint8_t)0x08) /*!< Channel 10 I/O pin input value */
#define RI_IOIR1_CH13I         ((uint8_t)0x10) /*!< Channel 13 I/O pin input value */
#define RI_IOIR1_CH16I         ((uint8_t)0x20) /*!< Channel 16 I/O pin input value */
#define RI_IOIR1_CH19I         ((uint8_t)0x40) /*!< Channel 19 I/O pin input value */
#define RI_IOIR1_CH22I         ((uint8_t)0x80) /*!< Channel 22 I/O pin input value */

#define RI_IOIR2_CH2I          ((uint8_t)0x01) /*!< Channel 2 I/O pin input value */
#define RI_IOIR2_CH5I          ((uint8_t)0x02) /*!< Channel 5 I/O pin input value */
#define RI_IOIR2_CH8I          ((uint8_t)0x04) /*!< Channel 8 I/O pin input value */
#define RI_IOIR2_CH11I         ((uint8_t)0x08) /*!< Channel 11 I/O pin input value */
#define RI_IOIR2_CH14I         ((uint8_t)0x10) /*!< Channel 14 I/O pin input value */
#define RI_IOIR2_CH17I         ((uint8_t)0x20) /*!< Channel 17 I/O pin input value */
#define RI_IOIR2_CH20I         ((uint8_t)0x40) /*!< Channel 20 I/O pin input value */
#define RI_IOIR2_CH23I         ((uint8_t)0x80) /*!< Channel 23 I/O pin input value */

#define RI_IOIR3_CH3I          ((uint8_t)0x01) /*!< Channel 3 I/O pin input value */
#define RI_IOIR3_CH6I          ((uint8_t)0x02) /*!< Channel 6 I/O pin input value */
#define RI_IOIR3_CH9I          ((uint8_t)0x04) /*!< Channel 9 I/O pin input value */
#define RI_IOIR3_CH12I         ((uint8_t)0x08) /*!< Channel 12 I/O pin input value */
#define RI_IOIR3_CH15I         ((uint8_t)0x10) /*!< Channel 15 I/O pin input value */
#define RI_IOIR3_CH18I         ((uint8_t)0x20) /*!< Channel 18 I/O pin input value */
#define RI_IOIR3_CH21I         ((uint8_t)0x40) /*!< Channel 21 I/O pin input value */
#define RI_IOIR3_CH24I         ((uint8_t)0x80) /*!< Channel 24 I/O pin input value */

#define RI_IOCMR1_CH1M          ((uint8_t)0x01) /*!< Channel 1 I/O control mode */
#define RI_IOCMR1_CH4M          ((uint8_t)0x02) /*!< Channel 4 I/O control mode */
#define RI_IOCMR1_CH7M          ((uint8_t)0x04) /*!< Channel 7 I/O control mode */
#define RI_IOCMR1_CH10M         ((uint8_t)0x08) /*!< Channel 10 I/O control mode */
#define RI_IOCMR1_CH13M         ((uint8_t)0x10) /*!< Channel 13 I/O control mode */
#define RI_IOCMR1_CH16M         ((uint8_t)0x20) /*!< Channel 16 I/O control mode */
#define RI_IOCMR1_CH19M         ((uint8_t)0x40) /*!< Channel 19 I/O control mode */
#define RI_IOCMR1_CH22M         ((uint8_t)0x80) /*!< Channel 22 I/O control mode */

#define RI_IOCMR2_CH2M          ((uint8_t)0x01) /*!< Channel 2 I/O control mode */
#define RI_IOCMR2_CH5M          ((uint8_t)0x02) /*!< Channel 5 I/O control mode */
#define RI_IOCMR2_CH8M          ((uint8_t)0x04) /*!< Channel 8 I/O control mode */
#define RI_IOCMR2_CH11M         ((uint8_t)0x08) /*!< Channel 11 I/O control mode */
#define RI_IOCMR2_CH14M         ((uint8_t)0x10) /*!< Channel 14 I/O control mode */
#define RI_IOCMR2_CH17M         ((uint8_t)0x20) /*!< Channel 17 I/O control mode */
#define RI_IOCMR2_CH20M         ((uint8_t)0x40) /*!< Channel 20 I/O control mode */
#define RI_IOCMR2_CH23M         ((uint8_t)0x80) /*!< Channel 23 I/O control mode */

#define RI_IOCMR3_CH3M          ((uint8_t)0x01) /*!< Channel 3 I/O control mode */
#define RI_IOCMR3_CH6M          ((uint8_t)0x02) /*!< Channel 6 I/O control mode */
#define RI_IOCMR3_CH9M          ((uint8_t)0x04) /*!< Channel 9 I/O control mode */
#define RI_IOCMR3_CH12M         ((uint8_t)0x08) /*!< Channel 12 I/O control mode */
#define RI_IOCMR3_CH15M         ((uint8_t)0x10) /*!< Channel 15 I/O control mode */
#define RI_IOCMR3_CH18M         ((uint8_t)0x20) /*!< Channel 18 I/O control mode */
#define RI_IOCMR3_CH21M         ((uint8_t)0x40) /*!< Channel 21 I/O control mode */
#define RI_IOCMR3_CH24M         ((uint8_t)0x80) /*!< Channel 24 I/O control mode */

#define RI_IOSR1_CH1E          ((uint8_t)0x01) /*!< Channel 1 I/O switch control */
#define RI_IOSR1_CH4E          ((uint8_t)0x02) /*!< Channel 4 I/O switch control */
#define RI_IOSR1_CH7E          ((uint8_t)0x04) /*!< Channel 7 I/O switch control */
#define RI_IOSR1_CH10E         ((uint8_t)0x08) /*!< Channel 10 I/O switch control */
#define RI_IOSR1_CH13E         ((uint8_t)0x10) /*!< Channel 13 I/O switch control */
#define RI_IOSR1_CH16E         ((uint8_t)0x20) /*!< Channel 16 I/O switch control */
#define RI_IOSR1_CH19E         ((uint8_t)0x40) /*!< Channel 19 I/O switch control */
#define RI_IOSR1_CH22E         ((uint8_t)0x80) /*!< Channel 22 I/O switch control */

#define RI_IOSR2_CH2E          ((uint8_t)0x01) /*!< Channel 2 I/O switch control */
#define RI_IOSR2_CH5E          ((uint8_t)0x02) /*!< Channel 5 I/O switch control */
#define RI_IOSR2_CH8E          ((uint8_t)0x04) /*!< Channel 8 I/O switch control */
#define RI_IOSR2_CH11E         ((uint8_t)0x08) /*!< Channel 11 I/O switch control */
#define RI_IOSR2_CH14E         ((uint8_t)0x10) /*!< Channel 14 I/O switch control */
#define RI_IOSR2_CH17E         ((uint8_t)0x20) /*!< Channel 17 I/O switch control */
#define RI_IOSR2_CH20E         ((uint8_t)0x40) /*!< Channel 20 I/O switch control */
#define RI_IOSR2_CH23E         ((uint8_t)0x80) /*!< Channel 23 I/O switch control */

#define RI_IOSR3_CH3E          ((uint8_t)0x01) /*!< Channel 3 I/O switch control */
#define RI_IOSR3_CH6E          ((uint8_t)0x02) /*!< Channel 6 I/O switch control */
#define RI_IOSR3_CH9E          ((uint8_t)0x04) /*!< Channel 9 I/O switch control */
#define RI_IOSR3_CH12E         ((uint8_t)0x08) /*!< Channel 12 I/O switch control */
#define RI_IOSR3_CH15E         ((uint8_t)0x10) /*!< Channel 15 I/O switch control */
#define RI_IOSR3_CH18E         ((uint8_t)0x20) /*!< Channel 18 I/O switch control */
#define RI_IOSR3_CH21E         ((uint8_t)0x40) /*!< Channel 21 I/O switch control */
#define RI_IOSR3_CH24E         ((uint8_t)0x80) /*!< Channel 24 I/O switch control */

#define RI_IOGCR_IOM1          ((uint8_t)0x03) /*!< I/O mode 1 */
#define RI_IOGCR_IOM2          ((uint8_t)0x0C) /*!< I/O mode 2 */
#define RI_IOGCR_IOM3          ((uint8_t)0x30) /*!< I/O mode 3 */
#define RI_IOGCR_IOM4          ((uint8_t)0xC0) /*!< I/O mode 4 */

#define RI_ASCR1_AS0           ((uint8_t)0x01) /*!< Analog switch AS0 control */
#define RI_ASCR1_AS1           ((uint8_t)0x02) /*!< Analog switch AS1 control */
#define RI_ASCR1_AS2           ((uint8_t)0x04) /*!< Analog switch AS2 control */
#define RI_ASCR1_AS3           ((uint8_t)0x08) /*!< Analog switch AS3 control */
#define RI_ASCR1_AS4           ((uint8_t)0x10) /*!< Analog switch AS4 control */
#define RI_ASCR1_AS5           ((uint8_t)0x20) /*!< Analog switch AS5 control */
#define RI_ASCR1_AS6           ((uint8_t)0x40) /*!< Analog switch AS6 control */
#define RI_ASCR1_AS7           ((uint8_t)0x80) /*!< Analog switch AS7 control */

#define RI_ASCR2_AS8           ((uint8_t)0x01) /*!< Analog switch AS8 control */
#define RI_ASCR2_AS9           ((uint8_t)0x02) /*!< Analog switch AS9 control */
#define RI_ASCR2_AS10          ((uint8_t)0x04) /*!< Analog switch AS10 control */
#define RI_ASCR2_AS11          ((uint8_t)0x08) /*!< Analog switch AS11 control */
#define RI_ASCR2_AS14          ((uint8_t)0x40) /*!< Analog switch AS14 control */

#define RI_RCR_400KPD          ((uint8_t)0x08) /*!< 400K pull-down resistor Mask. */
#define RI_RCR_10KPD           ((uint8_t)0x04) /*!< 10K pull-down resistor Mask. */
#define RI_RCR_400KPU          ((uint8_t)0x02) /*!< 400K pull-up resistor Mask. */
#define RI_RCR_10KPU           ((uint8_t)0x01) /*!< 10K pull-up resistor Mask. */

#define RI_IOSR4_CH29E         ((uint8_t)0x01) /*!< Channel 29 I/O switch control */
#define RI_IOSR4_CH26E         ((uint8_t)0x02) /*!< Channel 26 I/O switch control */
#define RI_IOSR4_CH27E         ((uint8_t)0x40) /*!< Channel 27 I/O switch control */
#define RI_IOSR4_CH28E         ((uint8_t)0x80) /*!< Channel 28 I/O switch control */

/**
  * @}
  */
/*----------------------------------------------------------------------------ok*/

/**
  * @brief Serial Peripheral Interface (SPI)
  */
typedef struct SPI_struct
{
  __IO uint8_t CR1;    /*!< SPI control register 1 */
  __IO uint8_t CR2;    /*!< SPI control register 2 */
  __IO uint8_t CR3;    /*!< SPI DMA and interrupt control register */
  __IO uint8_t SR;     /*!< SPI status register */
  __IO uint8_t DR;     /*!< SPI data I/O register */
  __IO uint8_t CRCPR;  /*!< SPI CRC polynomial register */
  __IO uint8_t RXCRCR; /*!< SPI Rx CRC register */
  __IO uint8_t TXCRCR; /*!< SPI Tx CRC register */
}
SPI_TypeDef;

/** @addtogroup SPI_Registers_Reset_Value
  * @{
  */

#define SPI_CR1_RESET_VALUE    ((uint8_t)0x00) /*!< Control Register 1 reset value */
#define SPI_CR2_RESET_VALUE    ((uint8_t)0x00) /*!< Control Register 2 reset value */
#define SPI_CR3_RESET_VALUE    ((uint8_t)0x00) /*!< DMA and Interrupt Control Register reset value */
#define SPI_SR_RESET_VALUE     ((uint8_t)0x02) /*!< Status Register reset value */
#define SPI_DR_RESET_VALUE     ((uint8_t)0x00) /*!< Data Register reset value */
#define SPI_CRCPR_RESET_VALUE  ((uint8_t)0x07) /*!< Polynomial Register reset value */
#define SPI_RXCRCR_RESET_VALUE ((uint8_t)0x00) /*!< RX CRC Register reset value */
#define SPI_TXCRCR_RESET_VALUE ((uint8_t)0x00) /*!< TX CRC Register reset value */

/**
  * @}
  */

/** @addtogroup SPI_Registers_Bits_Definition
  * @{
  */

#define SPI_CR1_LSBFIRST ((uint8_t)0x80) /*!< Frame format mask */
#define SPI_CR1_SPE      ((uint8_t)0x40) /*!< Enable bits mask */
#define SPI_CR1_BR       ((uint8_t)0x38) /*!< Baud rate control mask */
#define SPI_CR1_MSTR     ((uint8_t)0x04) /*!< Master Selection mask */
#define SPI_CR1_CPOL     ((uint8_t)0x02) /*!< Clock Polarity mask */
#define SPI_CR1_CPHA     ((uint8_t)0x01) /*!< Clock Phase mask */

#define SPI_CR2_BDM      ((uint8_t)0x80) /*!< Bi-directional data mode enable mask */
#define SPI_CR2_BDOE     ((uint8_t)0x40) /*!< Output enable in bi-directional mode mask */
#define SPI_CR2_CRCEN    ((uint8_t)0x20) /*!< Hardware CRC calculation enable mask */
#define SPI_CR2_CRCNEXT  ((uint8_t)0x10) /*!< Transmit CRC next mask */
#define SPI_CR2_RXONLY   ((uint8_t)0x04) /*!< Receive only mask */
#define SPI_CR2_SSM      ((uint8_t)0x02) /*!< Software slave management mask */
#define SPI_CR2_SSI      ((uint8_t)0x01) /*!< Internal slave select mask */

#define SPI_CR3_TXIE     ((uint8_t)0x80) /*!< Tx buffer empty interrupt enable mask */
#define SPI_CR3_RXIE     ((uint8_t)0x40) /*!< Rx buffer empty interrupt enable mask */
#define SPI_CR3_ERRIE    ((uint8_t)0x20) /*!< Error interrupt enable mask */
#define SPI_CR3_WKIE     ((uint8_t)0x10) /*!< Wake-up interrupt enable mask */
#define SPI_CR3_TXDMAEN  ((uint8_t)0x02) /*!< Tx Buffer DMA enable mask */
#define SPI_CR3_RXDMAEN  ((uint8_t)0x01) /*!< Rx Buffer DMA enable mask */

#define SPI_SR_BSY       ((uint8_t)0x80) /*!< Busy flag */
#define SPI_SR_OVR       ((uint8_t)0x40) /*!< Overrun flag */
#define SPI_SR_MODF      ((uint8_t)0x20) /*!< Mode fault */
#define SPI_SR_CRCERR    ((uint8_t)0x10) /*!< CRC error flag */
#define SPI_SR_WKUP      ((uint8_t)0x08) /*!< Wake-Up flag */
#define SPI_SR_TXE       ((uint8_t)0x02) /*!< Transmit buffer empty */
#define SPI_SR_RXNE      ((uint8_t)0x01) /*!< Receive buffer not empty */
/**
  * @}
  */

/*----------------------------------------------------------------------------ok*/

/**
  * @brief Advanced 16 bit timer with complementary PWM outputs (TIM1)
  */
typedef struct TIM1_struct
{
  __IO uint8_t CR1;   /*!< control register 1 */
  __IO uint8_t CR2;   /*!< control register 2 */
  __IO uint8_t SMCR;  /*!< Synchro mode control register */
  __IO uint8_t ETR;   /*!< external trigger register */
  __IO uint8_t DER;   /*!< DMA requests enable register */
  __IO uint8_t IER;   /*!< interrupt enable register*/
  __IO uint8_t SR1;   /*!< status register 1 */
  __IO uint8_t SR2;   /*!< status register 2 */
  __IO uint8_t EGR;   /*!< event generation register */
  __IO uint8_t CCMR1; /*!< CC mode register 1 */
  __IO uint8_t CCMR2; /*!< CC mode register 2 */
  __IO uint8_t CCMR3; /*!< CC mode register 3 */
  __IO uint8_t CCMR4; /*!< CC mode register 4 */
  __IO uint8_t CCER1; /*!< CC enable register 1 */
  __IO uint8_t CCER2; /*!< CC enable register 2 */
  __IO uint8_t CNTRH; /*!< counter high */
  __IO uint8_t CNTRL; /*!< counter low */
  __IO uint8_t PSCRH; /*!< prescaler high */
  __IO uint8_t PSCRL; /*!< prescaler low */
  __IO uint8_t ARRH;  /*!< auto-reload register high */
  __IO uint8_t ARRL;  /*!< auto-reload register low */
  __IO uint8_t RCR;   /*!< Repetition Counter register */
  __IO uint8_t CCR1H; /*!< capture/compare register 1 high */
  __IO uint8_t CCR1L; /*!< capture/compare register 1 low */
  __IO uint8_t CCR2H; /*!< capture/compare register 2 high */
  __IO uint8_t CCR2L; /*!< capture/compare register 2 low */
  __IO uint8_t CCR3H; /*!< capture/compare register 3 high */
  __IO uint8_t CCR3L; /*!< capture/compare register 3 low */
  __IO uint8_t CCR4H; /*!< capture/compare register 3 high */
  __IO uint8_t CCR4L; /*!< capture/compare register 3 low */
  __IO uint8_t BKR;   /*!< Break Register */
  __IO uint8_t DTR;   /*!< dead-time register */
  __IO uint8_t OISR;  /*!< Output idle register */
  __IO uint8_t DCR1;  /*!< DMA control register 1 */
  __IO uint8_t DCR2;  /*!< DMA control register 2 */
  __IO uint8_t DMAR;  /*!< DMA Address for brust mode */
}
TIM1_TypeDef;

/** @addtogroup TIM1_Registers_Reset_Value
  * @{
  */
#define TIM1_CR1_RESET_VALUE   ((uint8_t)0x00)
#define TIM1_CR2_RESET_VALUE   ((uint8_t)0x00)
#define TIM1_SMCR_RESET_VALUE  ((uint8_t)0x00)
#define TIM1_ETR_RESET_VALUE   ((uint8_t)0x00)
#define TIM1_DER_RESET_VALUE   ((uint8_t)0x00)
#define TIM1_IER_RESET_VALUE   ((uint8_t)0x00)
#define TIM1_SR1_RESET_VALUE   ((uint8_t)0x00)
#define TIM1_SR2_RESET_VALUE   ((uint8_t)0x00)
#define TIM1_EGR_RESET_VALUE   ((uint8_t)0x00)
#define TIM1_CCMR1_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CCMR2_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CCMR3_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CCMR4_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CCER1_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CCER2_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CNTRH_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CNTRL_RESET_VALUE ((uint8_t)0x00)
#define TIM1_PSCRH_RESET_VALUE ((uint8_t)0x00)
#define TIM1_PSCRL_RESET_VALUE ((uint8_t)0x00)
#define TIM1_ARRH_RESET_VALUE  ((uint8_t)0xFF)
#define TIM1_ARRL_RESET_VALUE  ((uint8_t)0xFF)
#define TIM1_RCR_RESET_VALUE   ((uint8_t)0x00)
#define TIM1_CCR1H_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CCR1L_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CCR2H_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CCR2L_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CCR3H_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CCR3L_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CCR4H_RESET_VALUE ((uint8_t)0x00)
#define TIM1_CCR4L_RESET_VALUE ((uint8_t)0x00)
#define TIM1_BKR_RESET_VALUE   ((uint8_t)0x00)
#define TIM1_DTR_RESET_VALUE   ((uint8_t)0x00)
#define TIM1_OISR_RESET_VALUE  ((uint8_t)0x00)
#define TIM1_DCR1_RESET_VALUE  ((uint8_t)0x00)
#define TIM1_DCR2_RESET_VALUE  ((uint8_t)0x00)
#define TIM1_DMAR_RESET_VALUE  ((uint8_t)0x00)

/**
  * @}
  */

/** @addtogroup TIM1_Registers_Bits_Definition
  * @{
  */
/* CR1*/
#define TIM1_CR1_ARPE    ((uint8_t)0x80) /*!< Auto-Reload Preload Enable mask. */
#define TIM1_CR1_CMS     ((uint8_t)0x60) /*!< Center-aligned Mode Selection mask. */
#define TIM1_CR1_DIR     ((uint8_t)0x10) /*!< Direction mask. */
#define TIM1_CR1_OPM     ((uint8_t)0x08) /*!< One Pulse Mode mask. */
#define TIM1_CR1_URS     ((uint8_t)0x04) /*!< Update Request Source mask. */
#define TIM1_CR1_UDIS    ((uint8_t)0x02) /*!< Update DIsable mask. */
#define TIM1_CR1_CEN     ((uint8_t)0x01) /*!< Counter Enable mask. */
/* CR2*/
#define TIM1_CR2_TI1S    ((uint8_t)0x80) /*!< TI1 selection  */
#define TIM1_CR2_MMS     ((uint8_t)0x70) /*!< MMS Selection mask. */
#define TIM1_CR2_CCDS    ((uint8_t)0x08) /*!< Capture/Compare DMA Selection */
#define TIM1_CR2_CCUS    ((uint8_t)0x04) /*!< Capture/Compare Control Update Selection mask. */
#define TIM1_CR2_CCPC    ((uint8_t)0x01) /*!< Capture/Compare Preloaded Control mask. */
/* SMCR*/
#define TIM1_SMCR_MSM    ((uint8_t)0x80) /*!< Master/Slave Mode mask. */
#define TIM1_SMCR_TS     ((uint8_t)0x70) /*!< Trigger Selection mask. */
#define TIM1_SMCR_OCCS   ((uint8_t)0x08) /*!< OCREFCLR Selection mask. */
#define TIM1_SMCR_SMS    ((uint8_t)0x07) /*!< Slave Mode Selection mask. */
/* ETR*/
#define TIM1_ETR_ETP     ((uint8_t)0x80) /*!< External Trigger Polarity mask. */
#define TIM1_ETR_ECE     ((uint8_t)0x40) /*!< External Clock mask. */
#define TIM1_ETR_ETPS    ((uint8_t)0x30) /*!< External Trigger Prescaler mask. */
#define TIM1_ETR_ETF     ((uint8_t)0x0F) /*!< External Trigger Filter mask. */
/* DER*/
#define TIM1_DER_COMDE   ((uint8_t)0x20) /*!< Commutation DMA request Enable  mask.*/
#define TIM1_DER_CC4DE   ((uint8_t)0x10) /*!< Capture/Compare 4 DMA request Enable  mask.*/
#define TIM1_DER_CC3DE   ((uint8_t)0x08) /*!< Capture/Compare 3 DMA request Enable  mask.*/
#define TIM1_DER_CC2DE   ((uint8_t)0x04) /*!< Capture/Compare 2 DMA request Enable  mask.*/
#define TIM1_DER_CC1DE   ((uint8_t)0x02) /*!< Capture/Compare 1 DMA request Enable  mask.*/
#define TIM1_DER_UDE     ((uint8_t)0x01) /*!< Update DMA request Enable  mask. */
/* IER*/
#define TIM1_IER_BIE     ((uint8_t)0x80) /*!< Break Interrupt Enable mask. */
#define TIM1_IER_TIE     ((uint8_t)0x40) /*!< Trigger Interrupt Enable mask. */
#define TIM1_IER_COMIE   ((uint8_t)0x20) /*!< Commutation Interrupt Enable mask.*/
#define TIM1_IER_CC4IE   ((uint8_t)0x10) /*!< Capture/Compare 4 Interrupt Enable mask. */
#define TIM1_IER_CC3IE   ((uint8_t)0x08) /*!< Capture/Compare 3 Interrupt Enable mask. */
#define TIM1_IER_CC2IE   ((uint8_t)0x04) /*!< Capture/Compare 2 Interrupt Enable mask. */
#define TIM1_IER_CC1IE   ((uint8_t)0x02) /*!< Capture/Compare 1 Interrupt Enable mask. */
#define TIM1_IER_UIE     ((uint8_t)0x01) /*!< Update Interrupt Enable mask. */
/* SR1*/
#define TIM1_SR1_BIF     ((uint8_t)0x80) /*!< Break Interrupt Flag mask. */
#define TIM1_SR1_TIF     ((uint8_t)0x40) /*!< Trigger Interrupt Flag mask. */
#define TIM1_SR1_COMIF   ((uint8_t)0x20) /*!< Commutation Interrupt Flag mask. */
#define TIM1_SR1_CC4IF   ((uint8_t)0x10) /*!< Capture/Compare 4 Interrupt Flag mask. */
#define TIM1_SR1_CC3IF   ((uint8_t)0x08) /*!< Capture/Compare 3 Interrupt Flag mask. */
#define TIM1_SR1_CC2IF   ((uint8_t)0x04) /*!< Capture/Compare 2 Interrupt Flag mask. */
#define TIM1_SR1_CC1IF   ((uint8_t)0x02) /*!< Capture/Compare 1 Interrupt Flag mask. */
#define TIM1_SR1_UIF     ((uint8_t)0x01) /*!< Update Interrupt Flag mask. */
/* SR2*/
#define TIM1_SR2_CC4OF   ((uint8_t)0x10) /*!< Capture/Compare 4 Overcapture Flag mask. */
#define TIM1_SR2_CC3OF   ((uint8_t)0x08) /*!< Capture/Compare 3 Overcapture Flag mask. */
#define TIM1_SR2_CC2OF   ((uint8_t)0x04) /*!< Capture/Compare 2 Overcapture Flag mask. */
#define TIM1_SR2_CC1OF   ((uint8_t)0x02) /*!< Capture/Compare 1 Overcapture Flag mask. */
/*EGR*/
#define TIM1_EGR_BG      ((uint8_t)0x80) /*!< Break Generation mask. */
#define TIM1_EGR_TG      ((uint8_t)0x40) /*!< Trigger Generation mask. */
#define TIM1_EGR_COMG    ((uint8_t)0x20) /*!< Capture/Compare Control Update Generation mask. */
#define TIM1_EGR_CC4G    ((uint8_t)0x10) /*!< Capture/Compare 4 Generation mask. */
#define TIM1_EGR_CC3G    ((uint8_t)0x08) /*!< Capture/Compare 3 Generation mask. */
#define TIM1_EGR_CC2G    ((uint8_t)0x04) /*!< Capture/Compare 2 Generation mask. */
#define TIM1_EGR_CC1G    ((uint8_t)0x02) /*!< Capture/Compare 1 Generation mask. */
#define TIM1_EGR_UG      ((uint8_t)0x01) /*!< Update Generation mask. */
/*CCMR*/
#define TIM1_CCMR_ICxPSC ((uint8_t)0x0C) /*!< Input Capture x Prescaler mask. */
#define TIM1_CCMR_ICxF   ((uint8_t)0xF0) /*!< Input Capture x Filter mask. */
#define TIM1_CCMR_OCM    ((uint8_t)0x70) /*!< Output Compare x Mode mask. */
#define TIM1_CCMR_OCxPE  ((uint8_t)0x08) /*!< Output Compare x Preload Enable mask. */
#define TIM1_CCMR_OCxFE  ((uint8_t)0x04) /*!< Output Compare x Fast Enable mask. */
#define TIM1_CCMR_CCxS   ((uint8_t)0x03) /*!< Capture/Compare x Selection mask. */
#define TIM1_CCMR_OCxCE  ((uint8_t)0x80) /*!< Output Compare x Clear Enable mask. */

#define CCMR_TIxDirect_Set ((uint8_t)0x01)
/*CCER1*/
#define TIM1_CCER1_CC2NP ((uint8_t)0x80) /*!< Capture/Compare 2 Complementary output Polarity mask. */
#define TIM1_CCER1_CC2NE ((uint8_t)0x40) /*!< Capture/Compare 2 Complementary output enable mask. */
#define TIM1_CCER1_CC2P  ((uint8_t)0x20) /*!< Capture/Compare 2 output Polarity mask. */
#define TIM1_CCER1_CC2E  ((uint8_t)0x10) /*!< Capture/Compare 2 output enable mask. */
#define TIM1_CCER1_CC1NP ((uint8_t)0x08) /*!< Capture/Compare 1 Complementary output Polarity mask. */
#define TIM1_CCER1_CC1NE ((uint8_t)0x04) /*!< Capture/Compare 1 Complementary output enable mask. */
#define TIM1_CCER1_CC1P  ((uint8_t)0x02) /*!< Capture/Compare 1 output Polarity mask. */
#define TIM1_CCER1_CC1E  ((uint8_t)0x01) /*!< Capture/Compare 1 output enable mask. */
/*CCER2*/
#define TIM1_CCER2_CC4P  ((uint8_t)0x20) /*!< Capture/Compare 4 Polarity mask. */
#define TIM1_CCER2_CC4E  ((uint8_t)0x10) /*!< Capture/Compare 4 Enable mask. */
#define TIM1_CCER2_CC3NP ((uint8_t)0x08) /*!< Capture/Compare 3 Complementary output Polarity mask. */
#define TIM1_CCER2_CC3NE ((uint8_t)0x04) /*!< Capture/Compare 3 Complementary output enable mask. */
#define TIM1_CCER2_CC3P  ((uint8_t)0x02) /*!< Capture/Compare 3 output Polarity mask. */
#define TIM1_CCER2_CC3E  ((uint8_t)0x01) /*!< Capture/Compare 3 output enable mask. */
/*CNTR*/
#define TIM1_CNTRH_CNT   ((uint8_t)0xFF) /*!< Counter Value (MSB) mask. */
#define TIM1_CNTRL_CNT   ((uint8_t)0xFF) /*!< Counter Value (LSB) mask. */
/*PSCR*/
#define TIM1_PSCH_PSC    ((uint8_t)0xFF) /*!< Prescaler Value (MSB) mask. */
#define TIM1_PSCL_PSC    ((uint8_t)0xFF) /*!< Prescaler Value (LSB) mask. */
/*ARR*/
#define TIM1_ARRH_ARR    ((uint8_t)0xFF) /*!< Autoreload Value (MSB) mask. */
#define TIM1_ARRL_ARR    ((uint8_t)0xFF) /*!< Autoreload Value (LSB) mask. */
/*RCR*/
#define TIM1_RCR_REP     ((uint8_t)0xFF) /*!< Repetition Counter Value mask. */
/*CCR1*/
#define TIM1_CCR1H_CCR1  ((uint8_t)0xFF) /*!< Capture/Compare 1 Value (MSB) mask. */
#define TIM1_CCR1L_CCR1  ((uint8_t)0xFF) /*!< Capture/Compare 1 Value (LSB) mask. */
/*CCR2*/
#define TIM1_CCR2H_CCR2  ((uint8_t)0xFF) /*!< Capture/Compare 2 Value (MSB) mask. */
#define TIM1_CCR2L_CCR2  ((uint8_t)0xFF) /*!< Capture/Compare 2 Value (LSB) mask. */
/*CCR3*/
#define TIM1_CCR3H_CCR3  ((uint8_t)0xFF) /*!< Capture/Compare 3 Value (MSB) mask. */
#define TIM1_CCR3L_CCR3  ((uint8_t)0xFF) /*!< Capture/Compare 3 Value (LSB) mask. */
/*CCR4*/
#define TIM1_CCR4H_CCR4  ((uint8_t)0xFF) /*!< Capture/Compare 4 Value (MSB) mask. */
#define TIM1_CCR4L_CCR4  ((uint8_t)0xFF) /*!< Capture/Compare 4 Value (LSB) mask. */
/*BKR*/
#define TIM1_BKR_MOE     ((uint8_t)0x80) /*!< Main Output Enable mask. */
#define TIM1_BKR_AOE     ((uint8_t)0x40) /*!< Automatic Output Enable mask. */
#define TIM1_BKR_BKP     ((uint8_t)0x20) /*!< Break Polarity mask. */
#define TIM1_BKR_BKE     ((uint8_t)0x10) /*!< Break Enable mask. */
#define TIM1_BKR_OSSR    ((uint8_t)0x08) /*!< Off-State Selection for Run mode mask. */
#define TIM1_BKR_OSSI    ((uint8_t)0x04) /*!< Off-State Selection for Idle mode mask. */
#define TIM1_BKR_LOCK    ((uint8_t)0x03) /*!< Lock Configuration mask. */
/*DTR*/
#define TIM1_DTR_DTG     ((uint8_t)0xFF) /*!< Dead-Time Generator set-up mask. */
/*OISR*/
#define TIM1_OISR_OIS3N  ((uint8_t)0x20) /*!< Output Idle state 3 (OC3N output) mask. */
#define TIM1_OISR_OIS3   ((uint8_t)0x10) /*!< Output Idle state 3 (OC3 output) mask. */
#define TIM1_OISR_OIS2N  ((uint8_t)0x08) /*!< Output Idle state 2 (OC2N output) mask. */
#define TIM1_OISR_OIS2   ((uint8_t)0x04) /*!< Output Idle state 2 (OC2 output) mask. */
#define TIM1_OISR_OIS1N  ((uint8_t)0x02) /*!< Output Idle state 1 (OC1N output) mask. */
#define TIM1_OISR_OIS1   ((uint8_t)0x01) /*!< Output Idle state 1 (OC1 output) mask. */

/*DCR1*/
#define TIM1_DCR1_DBA    ((uint8_t)0x1F)    /*!< DMA Base Address mask. */

/*DCR2*/
#define TIM1_DCR2_DBL    ((uint8_t)0x1F)    /*!< DMA Burst Length mask. */

/*DMAR*/
#define TIM1_DMAR_VR     ((uint8_t)0xFF)    /*!< Virtual register mask. */


/**
  * @}
  */
/*----------------------------------------------------------------------------*/

/**
  * @brief 16 bit timer :TIM2, TIM3 & TIM5
  */
typedef struct TIM_struct
{
  __IO uint8_t CR1;    /*!< control register 1   */
  __IO uint8_t CR2;    /*!< control register 2   */
  __IO uint8_t SMCR;   /*!< Synchro mode control register */
  __IO uint8_t ETR;    /*!< external trigger register */
  __IO uint8_t DER;    /*!< DMA requests enable register */
  __IO uint8_t IER;    /*!< interrupt enable register*/
  __IO uint8_t SR1;    /*!< status register 1   */
  __IO uint8_t SR2;    /*!< status register 2   */
  __IO uint8_t EGR;    /*!< event generation register */
  __IO uint8_t CCMR1;  /*!< CC mode register 1      */
  __IO uint8_t CCMR2;  /*!< CC mode register 2      */
  __IO uint8_t CCER1;  /*!< CC enable register 1     */
  __IO uint8_t CNTRH;  /*!< counterregister  high  */
  __IO uint8_t CNTRL;  /*!< counterregister  low   */
  __IO uint8_t PSCR;   /*!< prescaler  register   */
  __IO uint8_t ARRH;   /*!< auto-reload register high  */
  __IO uint8_t ARRL;   /*!< auto-reload register low    */
  __IO uint8_t CCR1H;  /*!< capture/compare register 1 high   */
  __IO uint8_t CCR1L;  /*!< capture/compare register 1 low     */
  __IO uint8_t CCR2H;  /*!< capture/compare register 2 high   */
  __IO uint8_t CCR2L;  /*!< capture/compare register 2 low     */
  __IO uint8_t BKR;    /*!< Break Register */
  __IO uint8_t OISR;   /*!< Output idle register */
}
TIM_TypeDef;

/** @addtogroup TIM2_TIM3_Registers_Reset_Value
  * @{
  */
#define TIM_CR1_RESET_VALUE    ((uint8_t)0x00)
#define TIM_CR2_RESET_VALUE    ((uint8_t)0x00)
#define TIM_SMCR_RESET_VALUE   ((uint8_t)0x00)
#define TIM_ETR_RESET_VALUE    ((uint8_t)0x00)
#define TIM_DER_RESET_VALUE    ((uint8_t)0x00)
#define TIM_IER_RESET_VALUE    ((uint8_t)0x00)
#define TIM_SR1_RESET_VALUE    ((uint8_t)0x00)
#define TIM_SR2_RESET_VALUE    ((uint8_t)0x00)
#define TIM_EGR_RESET_VALUE    ((uint8_t)0x00)
#define TIM_CCMR1_RESET_VALUE  ((uint8_t)0x00)
#define TIM_CCMR2_RESET_VALUE  ((uint8_t)0x00)

#define TIM_CCER1_RESET_VALUE  ((uint8_t)0x00)

#define TIM_CNTRH_RESET_VALUE  ((uint8_t)0x00)
#define TIM_CNTRL_RESET_VALUE  ((uint8_t)0x00)

#define TIM_PSCR_RESET_VALUE   ((uint8_t)0x00)
#define TIM_ARRH_RESET_VALUE   ((uint8_t)0xFF)
#define TIM_ARRL_RESET_VALUE   ((uint8_t)0xFF)

#define TIM_CCR1H_RESET_VALUE  ((uint8_t)0x00)
#define TIM_CCR1L_RESET_VALUE  ((uint8_t)0x00)
#define TIM_CCR2H_RESET_VALUE  ((uint8_t)0x00)
#define TIM_CCR2L_RESET_VALUE  ((uint8_t)0x00)

#define TIM_BKR_RESET_VALUE    ((uint8_t)0x00)
#define TIM_OISR_RESET_VALUE   ((uint8_t)0x00)

/**
  * @}
  */

/** @addtogroup TIM2_TIM3_TIM5_Registers_Bits_Definition
  * @{
  */
/* CR1*/
#define TIM_CR1_ARPE     ((uint8_t)0x80) /*!< Auto-Reload Preload Enable Mask. */
#define TIM_CR1_CMS      ((uint8_t)0x60) /*!< Center-aligned Mode Selection Mask. */
#define TIM_CR1_DIR      ((uint8_t)0x10) /*!< Direction Mask. */
#define TIM_CR1_OPM      ((uint8_t)0x08) /*!< One Pulse Mode Mask. */
#define TIM_CR1_URS      ((uint8_t)0x04) /*!< Update Request Source Mask. */
#define TIM_CR1_UDIS     ((uint8_t)0x02) /*!< Update DIsable Mask. */
#define TIM_CR1_CEN      ((uint8_t)0x01) /*!< Counter Enable Mask. */

/* CR2*/
#define TIM_CR2_TI1S     ((uint8_t)0x80) /*!< TI1 selection  */
#define TIM_CR2_MMS      ((uint8_t)0x70) /*!< MMS Selection Mask. */
#define TIM_CR2_CCDS     ((uint8_t)0x08) /*!< Capture/Compare DMA Selection */


/* SMCR*/
#define TIM_SMCR_MSM     ((uint8_t)0x80) /*!< Master/Slave Mode Mask. */
#define TIM_SMCR_TS      ((uint8_t)0x70) /*!< Trigger Selection Mask. */
#define TIM_SMCR_SMS     ((uint8_t)0x07) /*!< Slave Mode Selection Mask. */


/* ETR*/
#define TIM_ETR_ETP      ((uint8_t)0x80) /*!< External Trigger Polarity Mask. */
#define TIM_ETR_ECE      ((uint8_t)0x40)/*!< External Clock Mask. */
#define TIM_ETR_ETPS     ((uint8_t)0x30) /*!< External Trigger Prescaler Mask. */
#define TIM_ETR_ETF      ((uint8_t)0x0F) /*!< External Trigger Filter Mask. */

/* DER*/
#define TIM_DER_CC2DE    ((uint8_t)0x04) /*!< Capture/Compare 2 DMA request Enable mask.*/
#define TIM_DER_CC1DE    ((uint8_t)0x02) /*!< Capture/Compare 1 DMA request Enable mask.*/
#define TIM_DER_UDE      ((uint8_t)0x01) /*!< Update DMA request Enable  mask. */

/* IER*/
#define TIM_IER_BIE      ((uint8_t)0x80) /*!< Break Interrupt Enable Mask. */
#define TIM_IER_TIE      ((uint8_t)0x40) /*!< Trigger Interrupt Enable Mask. */
#define TIM_IER_CC2IE    ((uint8_t)0x04) /*!< Capture/Compare 2 Interrupt Enable Mask. */
#define TIM_IER_CC1IE    ((uint8_t)0x02) /*!< Capture/Compare 1 Interrupt Enable Mask. */
#define TIM_IER_UIE      ((uint8_t)0x01) /*!< Update Interrupt Enable Mask. */

/* SR1*/
#define TIM_SR1_BIF      ((uint8_t)0x80) /*!< Break Interrupt Flag Mask. */
#define TIM_SR1_TIF      ((uint8_t)0x40) /*!< Trigger Interrupt Flag Mask. */
#define TIM_SR1_CC2IF    ((uint8_t)0x04) /*!< Capture/Compare 2 Interrupt Flag Mask. */
#define TIM_SR1_CC1IF    ((uint8_t)0x02) /*!< Capture/Compare 1 Interrupt Flag Mask. */
#define TIM_SR1_UIF      ((uint8_t)0x01) /*!< Update Interrupt Flag Mask. */

/* SR2*/
#define TIM_SR2_CC2OF    ((uint8_t)0x04) /*!< Capture/Compare 2 Overcapture Flag Mask. */
#define TIM_SR2_CC1OF    ((uint8_t)0x02) /*!< Capture/Compare 1 Overcapture Flag Mask. */

/*EGR*/
#define TIM_EGR_BG       ((uint8_t)0x80) /*!< Break Generation Mask. */
#define TIM_EGR_TG       ((uint8_t)0x40) /*!< Trigger Generation Mask. */
#define TIM_EGR_CC2G     ((uint8_t)0x04) /*!< Capture/Compare 2 Generation Mask. */
#define TIM_EGR_CC1G     ((uint8_t)0x02) /*!< Capture/Compare 1 Generation Mask. */
#define TIM_EGR_UG       ((uint8_t)0x01) /*!< Update Generation Mask. */

/*CCMR*/
#define TIM_CCMR_ICxF    ((uint8_t)0xF0) /*!< Input Capture x Filter Mask. */
#define TIM_CCMR_ICxPSC  ((uint8_t)0x0C) /*!< Input Capture x Prescaler Mask. */
#define TIM_CCMR_CCxS    ((uint8_t)0x03) /*!< Capture/Compare x Selection Mask. */
#define TIM_CCMR_OCM     ((uint8_t)0x70) /*!< Output Compare x Mode Mask. */
#define TIM_CCMR_OCxPE   ((uint8_t)0x08) /*!< Output Compare x Preload Enable Mask. */
#define TIM_CCMR_OCxFE   ((uint8_t)0x04) /*!< Output Compare x Fast Enable Mask. */

#define TIM_CCMR_TIxDirect_Set   ((uint8_t)0x01)

/*CCER1*/
#define TIM_CCER1_CC2P   ((uint8_t)0x20) /*!< Capture/Compare 2 output Polarity Mask. */
#define TIM_CCER1_CC2E   ((uint8_t)0x10) /*!< Capture/Compare 2 output enable Mask. */
#define TIM_CCER1_CC1P   ((uint8_t)0x02) /*!< Capture/Compare 1 output Polarity Mask. */
#define TIM_CCER1_CC1E   ((uint8_t)0x01) /*!< Capture/Compare 1 output enable Mask. */

/*CNTR*/
#define TIM_CNTRH_CNT    ((uint8_t)0xFF) /*!< Counter Value (MSB) Mask. */
#define TIM_CNTRL_CNT    ((uint8_t)0xFF) /*!< Counter Value (LSB) Mask. */

/*PSCR*/
#define TIM_PSCR_PSC     ((uint8_t)0x07) /*!< Prescaler Value  Mask. */

/*ARR*/
#define TIM_ARRH_ARR     ((uint8_t)0xFF) /*!< Autoreload Value (MSB) Mask. */
#define TIM_ARRL_ARR     ((uint8_t)0xFF) /*!< Autoreload Value (LSB) Mask. */


/*CCR1*/
#define TIM_CCR1H_CCR1   ((uint8_t)0xFF) /*!< Capture/Compare 1 Value (MSB) Mask. */
#define TIM_CCR1L_CCR1   ((uint8_t)0xFF) /*!< Capture/Compare 1 Value (LSB) Mask. */

/*CCR2*/
#define TIM_CCR2H_CCR2   ((uint8_t)0xFF) /*!< Capture/Compare 2 Value (MSB) Mask. */
#define TIM_CCR2L_CCR2   ((uint8_t)0xFF) /*!< Capture/Compare 2 Value (LSB) Mask. */


/*BKR*/
#define TIM_BKR_MOE      ((uint8_t)0x80) /*!< Main Output Enable Mask. */
#define TIM_BKR_AOE      ((uint8_t)0x40) /*!< Automatic Output Enable Mask. */
#define TIM_BKR_BKP      ((uint8_t)0x20) /*!< Break Polarity Mask. */
#define TIM_BKR_BKE      ((uint8_t)0x10) /*!< Break Enable Mask. */
#define TIM_BKR_OSSI     ((uint8_t)0x04) /*!< Off-State Selection for Idle mode Mask. */
#define TIM_BKR_LOCK     ((uint8_t)0x03) /*!<Lock Configuration Mask. */

/*OISR*/
#define TIM_OISR_OIS2    ((uint8_t)0x04) /*!< Output Idle state 2 (OC2 output) Mask. */
#define TIM_OISR_OIS1    ((uint8_t)0x01) /*!< Output Idle state 1 (OC1 output) Mask. */
/**
  * @}
  */


/*----------------------------------------------------------------------------*/

/**
  * @brief 8-bit system or Low End Small Timer (TIM4)
  */
typedef struct TIM4_struct
{
  __IO uint8_t CR1;   /*!< control register 1 */
  __IO uint8_t CR2;   /*!< control register 2 */
  __IO uint8_t SMCR;  /*!< Synchro mode control register */
  __IO uint8_t DER;   /*!< DMA requests enable register */
  __IO uint8_t IER;   /*!< interrupt enable register  */
  __IO uint8_t SR1;   /*!< status register 1    */
  __IO uint8_t EGR;   /*!< event generation register */
  __IO uint8_t CNTR;  /*!< counter register  */
  __IO uint8_t PSCR;  /*!< prescaler register */
  __IO uint8_t ARR;   /*!< auto-reload register */
}
TIM4_TypeDef;
/** @addtogroup TIM4_Registers_Reset_Value
  * @{
  */
#define TIM4_CR1_RESET_VALUE    ((uint8_t)0x00)
#define TIM4_CR2_RESET_VALUE    ((uint8_t)0x00)
#define TIM4_SMCR_RESET_VALUE   ((uint8_t)0x00)
#define TIM4_DER_RESET_VALUE    ((uint8_t)0x00)
#define TIM4_IER_RESET_VALUE    ((uint8_t)0x00)
#define TIM4_SR1_RESET_VALUE    ((uint8_t)0x00)
#define TIM4_EGR_RESET_VALUE    ((uint8_t)0x00)
#define TIM4_CNTR_RESET_VALUE   ((uint8_t)0x00)
#define TIM4_PSCR_RESET_VALUE   ((uint8_t)0x00)
#define TIM4_ARR_RESET_VALUE    ((uint8_t)0xFF)

/**
* @}
*/

/** @addtogroup TIM4_Registers_Bits_Definition
  * @{
  */
/* CR1*/
#define TIM4_CR1_ARPE    ((uint8_t)0x80) /*!< Auto-Reload Preload Enable Mask. */
#define TIM4_CR1_OPM     ((uint8_t)0x08) /*!< One Pulse Mode Mask. */
#define TIM4_CR1_URS     ((uint8_t)0x04) /*!< Update Request Source Mask. */
#define TIM4_CR1_UDIS    ((uint8_t)0x02) /*!< Update DIsable Mask. */
#define TIM4_CR1_CEN     ((uint8_t)0x01) /*!< Counter Enable Mask. */

/* CR2*/
#define TIM4_CR2_MMS     ((uint8_t)0x70) /*!< MMS Selection Mask. */

/* SMCR*/
#define TIM4_SMCR_MSM    ((uint8_t)0x80) /*!< Master/Slave Mode Mask. */
#define TIM4_SMCR_TS     ((uint8_t)0x70) /*!< Trigger Selection Mask. */
#define TIM4_SMCR_SMS    ((uint8_t)0x07) /*!< Slave Mode Selection Mask. */

/* DER*/
#define TIM4_DER_UDE     ((uint8_t)0x01) /*!< Update DMA request Enable  mask. */

/* IER*/
#define TIM4_IER_TIE     ((uint8_t)0x40) /*!< Trigger Interrupt Enable Mask. */
#define TIM4_IER_UIE     ((uint8_t)0x01) /*!< Update Interrupt Enable Mask. */

/* SR1*/
#define TIM4_SR1_UIF     ((uint8_t)0x01) /*!< Update Interrupt Flag Mask. */
#define TIM4_SR1_TIF     ((uint8_t)0x40) /*!< Trigger Interrupt Flag Mask. */

/* EGR*/
#define TIM4_EGR_UG      ((uint8_t)0x01) /*!< Update Generation Mask. */
#define TIM4_EGR_TG      ((uint8_t)0x40) /*!< Trigger Generation Mask. */

/* CNTR*/
#define TIM4_CNTR_CNT    ((uint8_t)0xFF) /*!<Counter Value (LSB) Mask. */

/* PSCR*/
#define TIM4_PSCR_PSC    ((uint8_t)0x0F) /*!<Prescaler Value  Mask. */

/* ARR*/
#define TIM4_ARR_ARR     ((uint8_t)0xFF) /*!<Autoreload Value Mask. */
/**
  * @}
  */

/*----------------------------------------------------------------------------*/

/**
  * @brief USART
  */
typedef struct USART_struct
{
  __IO uint8_t SR;   /*!<  USART status register  */
  __IO uint8_t DR;   /*!<  USART data register     */
  __IO uint8_t BRR1; /*!<  USART baud rate register   */
  __IO uint8_t BRR2; /*!<  USART DIV mantissa[11:8] SCIDIV fraction */
  __IO uint8_t CR1;  /*!<  USART control register 1     */
  __IO uint8_t CR2;  /*!<  USART control register 2     */
  __IO uint8_t CR3;  /*!<  USART control register 3      */
  __IO uint8_t CR4;  /*!< USART control register 4      */
  __IO uint8_t CR5;  /*!<  USART control register 5       */
  __IO uint8_t GTR;  /*!<  USART guard time register     */
  __IO uint8_t PSCR; /*!<  USART prescaler register     */
}
USART_TypeDef;


/** @addtogroup USART_Registers_Reset_Value
  * @{
  */
#define USART_SR_RESET_VALUE ((uint8_t)0xC0)
#define USART_BRR1_RESET_VALUE ((uint8_t)0x00)
#define USART_BRR2_RESET_VALUE ((uint8_t)0x00)
#define USART_CR1_RESET_VALUE ((uint8_t)0x00)
#define USART_CR2_RESET_VALUE ((uint8_t)0x00)
#define USART_CR3_RESET_VALUE ((uint8_t)0x00)
#define USART_CR4_RESET_VALUE ((uint8_t)0x00)

/**
  * @}
  */

/** @addtogroup USART_Registers_Bits_Definition
  * @{
  */
#define USART_SR_TXE       ((uint8_t)0x80) /*!< Transmit Data Register Empty mask */
#define USART_SR_TC        ((uint8_t)0x40) /*!< Transmission Complete mask */
#define USART_SR_RXNE      ((uint8_t)0x20) /*!< Read Data Register Not Empty mask */
#define USART_SR_IDLE      ((uint8_t)0x10) /*!< IDLE line detected mask */
#define USART_SR_OR        ((uint8_t)0x08) /*!< OverRun error mask */
#define USART_SR_NF        ((uint8_t)0x04) /*!< Noise Flag mask */
#define USART_SR_FE        ((uint8_t)0x02) /*!< Framing Error mask */
#define USART_SR_PE        ((uint8_t)0x01) /*!< Parity Error mask */

#define USART_BRR1_DIVM    ((uint8_t)0xFF) /*!< LSB mantissa of USARTDIV [7:0] mask */

#define USART_BRR2_DIVM    ((uint8_t)0xF0) /*!< MSB mantissa of USARTDIV [11:8] mask */
#define USART_BRR2_DIVF    ((uint8_t)0x0F) /*!< Fraction bits of USARTDIV [3:0] mask */

#define USART_CR1_R8       ((uint8_t)0x80) /*!< Receive Data bit 8 */
#define USART_CR1_T8       ((uint8_t)0x40) /*!< Transmit data bit 8 */
#define USART_CR1_USARTD   ((uint8_t)0x20) /*!< USART Disable (for low power consumption) */
#define USART_CR1_M        ((uint8_t)0x10) /*!< Word length mask */
#define USART_CR1_WAKE     ((uint8_t)0x08) /*!< Wake-up method mask */
#define USART_CR1_PCEN     ((uint8_t)0x04) /*!< Parity Control Enable mask */
#define USART_CR1_PS       ((uint8_t)0x02) /*!< USART Parity Selection */
#define USART_CR1_PIEN     ((uint8_t)0x01) /*!< USART Parity Interrupt Enable mask */

#define USART_CR2_TIEN     ((uint8_t)0x80) /*!< Transmitter Interrupt Enable mask */
#define USART_CR2_TCIEN    ((uint8_t)0x40) /*!< TransmissionComplete Interrupt Enable mask */
#define USART_CR2_RIEN     ((uint8_t)0x20) /*!< Receiver Interrupt Enable mask */
#define USART_CR2_ILIEN    ((uint8_t)0x10) /*!< IDLE Line Interrupt Enable mask */
#define USART_CR2_TEN      ((uint8_t)0x08) /*!< Transmitter Enable mask */
#define USART_CR2_REN      ((uint8_t)0x04) /*!< Receiver Enable mask */
#define USART_CR2_RWU      ((uint8_t)0x02) /*!< Receiver Wake-Up mask */
#define USART_CR2_SBK      ((uint8_t)0x01) /*!< Send Break mask */

#define USART_CR3_STOP     ((uint8_t)0x30) /*!< STOP bits [1:0] mask */
#define USART_CR3_CLKEN    ((uint8_t)0x08) /*!< Clock Enable mask */
#define USART_CR3_CPOL     ((uint8_t)0x04) /*!< Clock Polarity mask */
#define USART_CR3_CPHA     ((uint8_t)0x02) /*!< Clock Phase mask */
#define USART_CR3_LBCL     ((uint8_t)0x01) /*!< Last Bit Clock pulse mask */

#define USART_CR4_ADD      ((uint8_t)0x0F) /*!< Address of the USART node mask */

#define USART_CR5_DMAT     ((uint8_t)0x80) /*!< DMA Enable transmitter mask */
#define USART_CR5_DMAR     ((uint8_t)0x40) /*!< DMA Enable receiver mask */
#define USART_CR5_SCEN     ((uint8_t)0x20) /*!< Smart Card Enable mask */
#define USART_CR5_NACK     ((uint8_t)0x10) /*!< Smart Card Nack Enable mask */
#define USART_CR5_HDSEL    ((uint8_t)0x08) /*!< Half-Duplex Selection mask */
#define USART_CR5_IRLP     ((uint8_t)0x04) /*!< Irda Low Power Selection mask */
#define USART_CR5_IREN     ((uint8_t)0x02) /*!< Irda Enable mask */
#define USART_CR5_EIE      ((uint8_t)0x01) /*!< Error Interrupt mask */
/**
  * @}
  */
/*----------------------------------------------------------------------------*/

/**
  * @brief Analog to Digital Converter (ADC) peripheral
  */
typedef struct ADC_struct
{
  __IO uint8_t   CR1;      /*!< Control register 1    */
  __IO uint8_t   CR2;      /*!< Control register 2    */
  __IO uint8_t   CR3;      /*!<  Control register 3    */
  __IO uint8_t   SR;       /*!< Status register    */
  __IO uint8_t   DRH;      /*!< Data register MSB    */
  __IO uint8_t   DRL;      /*!< Data register LSB    */
  __IO uint8_t   HTRH;     /*!< High voltage reference register MSB    */
  __IO uint8_t   HTRL;     /*!< High voltage reference register LSB    */
  __IO uint8_t   LTRH;     /*!< Low voltage reference register MSB    */
  __IO uint8_t   LTRL;     /*!< Low voltage reference register LSB    */
  __IO uint8_t   SQR[4];   /*!< Channel select scan registers    */
  __IO uint8_t   TRIGR[4]; /*!< Trigger disable  registers  */
}
ADC_TypeDef;

/** @addtogroup ADC_Registers_Reset_Value
  * @{
  */
#define  ADC_CR1_RESET_VALUE      ((uint8_t) 0x00)
#define  ADC_CR2_RESET_VALUE      ((uint8_t) 0x00)
#define  ADC_CR3_RESET_VALUE      ((uint8_t) 0x1F)
#define  ADC_SR_RESET_VALUE       ((uint8_t) 0x00)
#define  ADC_DRH_RESET_VALUE      ((uint8_t) 0x00)
#define  ADC_DRL_RESET_VALUE      ((uint8_t) 0x00)
#define  ADC_HTRH_RESET_VALUE     ((uint8_t) 0x0F)
#define  ADC_HTRL_RESET_VALUE     ((uint8_t) 0xFF)
#define  ADC_LTRH_RESET_VALUE     ((uint8_t) 0x00)
#define  ADC_LTRL_RESET_VALUE     ((uint8_t) 0x00)
#define  ADC_SQR1_RESET_VALUE     ((uint8_t) 0x00)
#define  ADC_SQR2_RESET_VALUE     ((uint8_t) 0x00)
#define  ADC_SQR3_RESET_VALUE     ((uint8_t) 0x00)
#define  ADC_SQR4_RESET_VALUE     ((uint8_t) 0x00)
#define  ADC_TRIGR1_RESET_VALUE   ((uint8_t) 0x00)
#define  ADC_TRIGR2_RESET_VALUE   ((uint8_t) 0x00)
#define  ADC_TRIGR3_RESET_VALUE   ((uint8_t) 0x00)
#define  ADC_TRIGR4_RESET_VALUE   ((uint8_t) 0x00)


/**
* @}
*/

/** @addtogroup ADC_Registers_Bits_Definition
  * @{
  */
#define  ADC_CR1_ADON           ((uint8_t)0x01)
#define  ADC_CR1_START          ((uint8_t)0x02)
#define  ADC_CR1_CONT           ((uint8_t)0x04)
#define  ADC_CR1_EOCIE          ((uint8_t)0x08)
#define  ADC_CR1_AWDIE          ((uint8_t)0x10)
#define  ADC_CR1_RES            ((uint8_t)0x60)
#define  ADC_CR1_OVERIE         ((uint8_t)0x80)


#define  ADC_CR2_SMPT1          ((uint8_t)0x07)
#define  ADC_CR2_EXTSEL         ((uint8_t)0x18)
#define  ADC_CR2_TRIGEDGE       ((uint8_t)0x60)
#define  ADC_CR2_PRESC          ((uint8_t)0x80)

#define  ADC_CR3_CHSEL          ((uint8_t)0x1F)
#define  ADC_CR3_SMPT2          ((uint8_t)0xE0)

#define  ADC_SR_EOC             ((uint8_t)0x01)
#define  ADC_SR_AWD             ((uint8_t)0x02)
#define  ADC_SR_OVER            ((uint8_t)0x04)

#define  ADC_DRH_CONVDATA       ((uint8_t)0x0F)
#define  ADC_DRL_CONVDATA       ((uint8_t)0xFF)

#define  ADC_HTRH_HT            ((uint8_t)0x0F)
#define  ADC_HTRL_HT            ((uint8_t)0xFF)

#define  ADC_LTRH_LT            ((uint8_t)0x0F)
#define  ADC_LTRL_LT            ((uint8_t)0xFF)

#define  ADC_SQR1_CHSELS        ((uint8_t)0x3F)
#define  ADC_SQR1_DMAOFF        ((uint8_t)0x80)
#define  ADC_SQR2_CHSELS        ((uint8_t)0xFF)
#define  ADC_SQR3_CHSELS        ((uint8_t)0xFF)
#define  ADC_SQR4_CHSELS        ((uint8_t)0xFF)

#define  ADC_TRIGR1_TRIG        ((uint8_t)0x0F)
#define  ADC_TRIGR1_VREFINTON   ((uint8_t)0x10)
#define  ADC_TRIGR1_TSON        ((uint8_t)0x20)

#define  ADC_TRIGR2_TRIG        ((uint8_t)0xFF)
#define  ADC_TRIGR3_TRIG        ((uint8_t)0xFF)
#define  ADC_TRIGR4_TRIG        ((uint8_t)0xFF)

/**
  * @}
  */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/**
 * @brief Digital to Analog Converter (DAC) peripheral
  */
typedef struct DAC_struct
{
  __IO uint8_t  CH1CR1;   /*!< DAC control register 1  */
  __IO uint8_t  CH1CR2;   /*!< DAC control register 2  */
  __IO uint8_t  CH2CR1;   /*!< DAC channel 2 control register 1  */
  __IO uint8_t  CH2CR2;   /*!< DAC channel 2 control register 2  */
  __IO uint8_t  SWTRIGR; /*!< DAC software trigger register */
  __IO uint8_t  SR;      /*!< DAC status register */

  /*Channel 1 registers*/
  __IO uint8_t  RESERVED0[2];
  __IO uint8_t  CH1RDHRH; /*!< DAC Channel 1 right aligned data holding register MSB */
  __IO uint8_t  CH1RDHRL; /*!< DAC Channel 1 right aligned data holding register LSB */
  __IO uint8_t  RESERVED1[2];
  __IO uint8_t  CH1LDHRH; /*!< DAC Channel 1 left aligned data holding register MSB */
  __IO uint8_t  CH1LDHRL; /*!< DAC Channel 1 left aligned data holding register LSB */
  __IO uint8_t  RESERVED2[2];
  __IO uint8_t  CH1DHR8; /*!< DAC Channel 1 8-bit data holding register */

  /*Channel 2 registers*/
  __IO uint8_t  RESERVED3[3];
  __IO uint8_t  CH2RDHRH; /*!< DAC Channel 2 right aligned data holding register MSB */
  __IO uint8_t  CH2RDHRL; /*!< DAC Channel 2 right aligned data holding register LSB */
  __IO uint8_t  RESERVED4[2];
  __IO uint8_t  CH2LDHRH; /*!< DAC Channel 2 left aligned data holding register MSB */
  __IO uint8_t  CH2LDHRL; /*!< DAC Channel 2 left aligned data holding register LSB */
  __IO uint8_t  RESERVED5[2];
  __IO uint8_t  CH2DHR8; /*!< DAC Channel 2 8-bit data holding register */


  /* Dual mode registers */
  __IO uint8_t  RESERVED6[3];
  __IO uint8_t  DCH1RDHRH; /*!< DAC Dual mode Channel 1 right aligned data holding register MSB */
  __IO uint8_t  DCH1RDHRL; /*!< DAC Dual mode Channel 1 right aligned data holding register LSB */
  __IO uint8_t  DCH2RDHRH; /*!< DAC Dual mode Channel 2 right aligned data holding register MSB */
  __IO uint8_t  DCH2RDHRL; /*!< DAC Dual mode Channel 2 right aligned data holding register LSB */
  __IO uint8_t  DCH1LDHRH; /*!< DAC Dual mode Channel 1 left aligned data holding register MSB */
  __IO uint8_t  DCH1LDHRL; /*!< DAC Dual mode Channel 1 left aligned data holding register LSB */
  __IO uint8_t  DCH2LDHRH; /*!< DAC Dual mode Channel 2 left aligned data holding register MSB */
  __IO uint8_t  DCH2LDHRL; /*!< DAC Dual mode Channel 2 left aligned data holding register LSB */
  __IO uint8_t  DCH1DHR8; /*!< DAC Dual mode Channel 1 8-bit data holding register */
  __IO uint8_t  DCH2DHR8; /*!< DAC Dual mode Channel 2 8-bit data holding register */

  /* DOR registers*/
  __IO uint8_t  RESERVED7[2];
  __IO uint8_t  CH1DORH; /*!< DAC Channel 1 data output register MSB */
  __IO uint8_t  CH1DORL; /*!< DAC Channel 1 data output register LSB */
  __IO uint8_t  RESERVED8[2];
  __IO uint8_t  CH2DORH; /*!< DAC Channel 2 data output register MSB */
  __IO uint8_t  CH2DORL; /*!< DAC Channel 2 data output register LSB */
}
DAC_TypeDef;

/** @addtogroup DAC_Registers_Reset_Value
  * @{
  */
#define DAC_CR1_RESET_VALUE       ((uint8_t)0x00)
#define DAC_CR2_RESET_VALUE       ((uint8_t)0x00)
#define DAC_SWTRIGR_RESET_VALUE   ((uint8_t)0x00)
#define DAC_SR_RESET_VALUE        ((uint8_t)0x00)
#define DAC_RDHRH_RESET_VALUE     ((uint8_t)0x00)
#define DAC_RDHRL_RESET_VALUE     ((uint8_t)0x00)
#define DAC_LDHRH_RESET_VALUE     ((uint8_t)0x00)
#define DAC_LDHRL_RESET_VALUE     ((uint8_t)0x00)
#define DAC_DHR8_RESET_VALUE      ((uint8_t)0x00)
#define DAC_DORH_RESET_VALUE      ((uint8_t)0x00)
#define DAC_DORL_RESET_VALUE      ((uint8_t)0x00)
/**
  * @}
  */

/** @addtogroup DAC_Registers_Bits_Definition
  * @{
  */

/* CR1*/
#define DAC_CR1_TSEL          ((uint8_t)0x38) /*!<  DAC channel trigger selection. */
#define DAC_CR1_TEN           ((uint8_t)0x04) /*!<  DAC channel trigger enable. */
#define DAC_CR1_BOFF          ((uint8_t)0x02) /*!<  DAC channel output buffer disable. */
#define DAC_CR1_EN            ((uint8_t)0x01) /*!<  DAC channel enable. */
#define DAC_CR1_WAVEN         ((uint8_t)0xC0) /*!<  DAC channel wave generation enable. */

/* CR2*/
#define DAC_CR2_DMAUDRIE      ((uint8_t)0x20) /*!<  DAC channel DMA underrun interrupt enable. */
#define DAC_CR2_DMAEN         ((uint8_t)0x10) /*!<  DAC DMA enable. */
#define DAC_CR2_MAMPx         ((uint8_t)0x0F) /*!<  DAC Dchannel wave generation config. */

/* SWTRIGR*/
#define DAC_SWTRIGR_SWTRIG1   ((uint8_t)0x01) /*!<  DAC channel 1 software trigger. */
#define DAC_SWTRIGR_SWTRIG2   ((uint8_t)0x02) /*!<  DAC channel 2 software trigger. */

/* SR*/
#define DAC_SR_DMAUDR1        ((uint8_t)0x01) /*!<  DAC channel 1 DMA underrun flag. */
#define DAC_SR_DMAUDR2        ((uint8_t)0x02) /*!<  DAC channel 2 DMA underrun flag. */

/* RDHRH*/
#define DAC_RDHRH_RDHRH       ((uint8_t)0x0F) /*!<  DAC right aligned data holding register most significant bits. */

/* RDHRL*/
#define DAC_RDHRL_RDHRL       ((uint8_t)0xFF) /*!<  DAC right aligned data holding register least significant bits. */

/* LDHRL*/
#define DAC_LDHRH_LDHRH       ((uint8_t)0xFF) /*!<  DAC left aligned data holding register most significant bits. */

/* LDHRL*/
#define DAC_LDHRL_LDHRL       ((uint8_t)0xF0) /*!<  DAC left aligned data holding register least significant bits. */

/* DHR8*/
#define DAC_DHR8_8DHR         ((uint8_t)0xFF) /*!< DAC 8bit data holding bits */

/* DORH*/
#define DAC_DORH_DORH         ((uint8_t)0x0F) /*!< DAC data output register most significant bit */

/* DORL*/
#define DAC_DORL_DORL         ((uint8_t)0xFF) /*!< DAC data output register least significant bit */

/**
  * @}
  */

/*----------------------------------------------------------------------------*/

/**
  * @brief  Direct-Memory Access (DMA)
  */
typedef struct DMA_struct
{
  __IO uint8_t  GCSR;     /*!<  Global configuration and status register  */
  __IO uint8_t  GIR1;     /*!<  Global interrupt register 1  */
}
DMA_TypeDef;
/**
  * @}
  */
typedef struct DMA_Channel_struct
{
  __IO uint8_t  CCR;       /*!<  CHx Control register  */
  __IO uint8_t  CSPR;      /*!<  CHx Status & Priority register  */
  __IO uint8_t  CNBTR;     /*!<  CHx Number of Bytes to Tranfer register  */
  __IO uint8_t  CPARH;     /*!<  Peripheral Address High register  */
  __IO uint8_t  CPARL;     /*!<  Peripheral Address Low register  */
  __IO uint8_t  CM0EAR;    /*!<  Memory 0 Extended Address register  (for channel3)*/
  __IO uint8_t  CM0ARH;    /*!<  Memory 0 Address High register  */
  __IO uint8_t  CM0ARL;    /*!<  Memory 0 Address Low register  */
}
DMA_Channel_TypeDef;
/**
  * @}
  */

/** @addtogroup DMA_Registers_Reset_Value
  * @{
  */
#define DMA_GCSR_RESET_VALUE       ((uint8_t)0xFC)
#define DMA_GIR1_RESET_VALUE       ((uint8_t)0x00)
/**
  * @}
  */

/** @addtogroup DMA_Channels_Registers_Reset_Value
  * @{
  */
#define DMA_CCR_RESET_VALUE          ((uint8_t)0x00)
#define DMA_CSPR_RESET_VALUE         ((uint8_t)0x00)
#define DMA_CNBTR_RESET_VALUE        ((uint8_t)0x00)
#define DMA_CPARH_RESET_VALUE        ((uint8_t)0x52)
#define DMA_C3PARH_RESET_VALUE       ((uint8_t)0x40)
#define DMA_CPARL_RESET_VALUE        ((uint8_t)0x00)
#define DMA_C3M0EAR_RESET_VALUE      ((uint8_t)0x00)
#define DMA_CM0ARH_RESET_VALUE       ((uint8_t)0x00)
#define DMA_CM0ARL_RESET_VALUE       ((uint8_t)0x00)
/** @addtogroup DMA_Registers_Bits_Definition
  * @{
  */

/*  Bit definition for DMA_GCSR register  */

#define  DMA_GCSR_GE        ((uint8_t)0x01)            /*!<Global Enable */
#define  DMA_GCSR_GB        ((uint8_t)0x02)            /*!<Global Busy */
#define  DMA_GCSR_TO        ((uint8_t)0xFC)            /*!<Time Out */

/*  Bit definition for DMA_GIR1 register  */
#define  DMA_GIR1_IFC0      ((uint8_t)0x01)            /*!< Interrupt Flag Channel 0 */
#define  DMA_GIR1_IFC1      ((uint8_t)0x02)            /*!< Interrupt Flag Channel 1 */
#define  DMA_GIR1_IFC2      ((uint8_t)0x04)            /*!< Interrupt Flag Channel 2 */
#define  DMA_GIR1_IFC3      ((uint8_t)0x08)            /*!< Interrupt Flag Channel 3 */


/*  Bit definition for DMA_CCR registers  */
#define  DMA_CCR_CE         ((uint8_t)0x01)            /*!<Channel enable*/
#define  DMA_CCR_TCIE       ((uint8_t)0x02)            /*!<Transfer complete interrupt enable */
#define  DMA_CCR_HTIE       ((uint8_t)0x04)            /*!<Half Transfer interrupt enable */
#define  DMA_CCR_DTD        ((uint8_t)0x08)            /*!<Data transfer direction */
#define  DMA_CCR_ARM        ((uint8_t)0x10)            /*!<Autorelaod mode Circular buffer mode */
#define  DMA_CCR_IDM        ((uint8_t)0x20)            /*!<Inc/Dec mode */
#define  DMA_CCR_MEM        ((uint8_t)0x40)            /*!<Memory Transfer Enable */


/*  Bit definition for DMA_CSPR registers  */
#define  DMA_CSPR_TCIF      ((uint8_t)0x02)            /*!<Transaction Complete Interrupt Flag*/
#define  DMA_CSPR_HTIF      ((uint8_t)0x04)            /*!<Half Transaction Interrupt Flag*/
#define  DMA_CSPR_16BM      ((uint8_t)0x08)            /*!<16 bit mode*/
#define  DMA_CSPR_PL        ((uint8_t)0x30)            /*!<Channel priority level*/
#define  DMA_CSPR_PEND      ((uint8_t)0x40)            /*!<Channel pending*/
#define  DMA_CSPR_BUSY      ((uint8_t)0x80)            /*!<Channel Busy */


/*  Bit definition for DMA_CNBTR register */
#define  DMA_CNBTR_NDT      ((uint8_t)0xFF)            /*!<Number of data to Transfer */


/*  Bit definition for DMA_CPARH register  */
#define  DMA_CPARH_PA       ((uint8_t)0xFF)        /*!<Peripheral MSB Address Pointer */
/*  Bit definition for DMA_CPARL register  */
#define  DMA_CPARL_PA       ((uint8_t)0xFF)        /*!<Peripheral LSB Address Pointer */


/*  Bit definition for DMA_CMAR registers  */
#define  DMA_CM0EAR_MA      ((uint8_t)0x01)        /* Memory Extended Address Pointer only for channel 3 in Memory to memory transfer*/
#define  DMA_CM0ARH_MA      ((uint8_t)0xFF)        /*!<Memory MSB Address Pointer*/
#define  DMA_CM0ARL_MA      ((uint8_t)0xFF)        /*!<Memory LSB Address Pointer */


/**
  * @}
  */
/*----------------------------------------------------------------------------*/

/**
  * @brief Window Watchdog (WWDG)
  */
typedef struct WWDG_struct
{
  __IO uint8_t CR; /*!< Control Register */
  __IO uint8_t WR; /*!< Window Register */
}
WWDG_TypeDef;

/** @addtogroup WWDG_Registers_Reset_Value
  * @{
  */

#define WWDG_CR_RESET_VALUE ((uint8_t)0x7F)
#define WWDG_WR_RESET_VALUE ((uint8_t)0x7F)

/**
* @}
*/

/** @addtogroup WWDG_Registers_Bits_Definition
  * @{
  */

#define WWDG_CR_WDGA ((uint8_t)0x80) /*!< WDGA bit mask */
#define WWDG_CR_T6   ((uint8_t)0x40) /*!< T6 bit mask */
#define WWDG_CR_T    ((uint8_t)0x7F) /*!< T bits mask */

#define WWDG_WR_MSB  ((uint8_t)0x80) /*!< MSB bit mask */
#define WWDG_WR_W    ((uint8_t)0x7F) /*!< W bits mask */


/**
  * @}
  */

/*----------------------------------------------------------------------------*/
/**
  * @brief LCD Controller (LCD)
  */
typedef struct LCD_struct
{
  __IO uint8_t CR1;      /*!< LCD control register 1 */
  __IO uint8_t CR2;      /*!< LCD control register 2 */
  __IO uint8_t CR3;      /*!< LCD control register 3 */
  __IO uint8_t FRQ;      /*!< LCD frequency register */
  __IO uint8_t PM[6];    /*!< LCD portmask registers*/
  uint8_t RESERVED1[2];  /*!< Reserved */
  __IO uint8_t RAM[22];  /*!< LCD RAM registers*/
  uint8_t RESERVED2[13]; /*!< Reserved */
  __IO uint8_t CR4;      /*!< LCD control register 4 */
}
LCD_TypeDef;
/** @addtogroup LCD_Registers_Reset_Value
  * @{
  */
#define LCD_CR1_RESET_VALUE    ((uint8_t)0x00) /*!< Control Register 1 reset value */
#define LCD_CR2_RESET_VALUE    ((uint8_t)0x00) /*!< Control Register 2 reset value */
#define LCD_CR3_RESET_VALUE    ((uint8_t)0x00) /*!< Control Register 3 reset value */
#define LCD_FRQ_RESET_VALUE    ((uint8_t)0x00) /*!< Register Frequency reset value */
#define LCD_PM_RESET_VALUE     ((uint8_t)0x00) /*!< Port mask Register reset value */
#define LCD_RAM_RESET_VALUE    ((uint8_t)0x00) /*!< RAM Register reset value       */
#define LCD_CR4_RESET_VALUE    ((uint8_t)0x00) /*!< Control Register 4 reset value */


/**
* @}
*/

/** @addtogroup LCD_Registers_Bits_Definition
  * @{
  */
#define LCD_CR1_BLINK    ((uint8_t)0xC0) /*!< Blink bits mask           */
#define LCD_CR1_BLINKF   ((uint8_t)0x38) /*!< Blink frequency bits mask */
#define LCD_CR1_DUTY     ((uint8_t)0x06) /*!< Duty bits mask            */
#define LCD_CR1_B2       ((uint8_t)0x01) /*!< Bias selector bit mask    */


#define LCD_CR2_PON      ((uint8_t)0xE0) /*!< Pulse on duration bits mask */
#define LCD_CR2_HD       ((uint8_t)0x10) /*!< High drive enable bit mask  */
#define LCD_CR2_CC       ((uint8_t)0x0E) /*!< Contrast control bits mask  */
#define LCD_CR2_VSEL     ((uint8_t)0x01) /*!< Voltage source bit mask     */

#define LCD_CR3_LCDEN    ((uint8_t)0x40) /*!< Enable bit mask           */
#define LCD_CR3_SOFIE    ((uint8_t)0x20) /*!< Start of frame interrupt enable mask */
#define LCD_CR3_SOF      ((uint8_t)0x10) /*!< Start of frame bit mask              */
#define LCD_CR3_SOFC     ((uint8_t)0x08) /*!< Clear start of frame bit mask        */
#define LCD_CR3_DEAD     ((uint8_t)0x07) /*!< DEAD time bits mask                  */

#define LCD_FRQ_DIV      ((uint8_t)0x0F) /*!< Divider bits mask */
#define LCD_FRQ_PS       ((uint8_t)0xF0) /*!< 16 bits prescaler bits mask */

#define LCD_CR4_MAPCOM   ((uint8_t)0x08) /*!< Select the mapping scheme for the COM[7:4] */
#define LCD_CR4_PAGECOM  ((uint8_t)0x04) /*!< Select the LCD RAM page sector */
#define LCD_CR4_DUTY8    ((uint8_t)0x02) /*!< Enable the 1/8 duty */
#define LCD_CR4_B4       ((uint8_t)0x01) /*!< Enable the 1/4 bias */


/**
  * @}
  */

/*----------------------------------------------------------------------------*/
/* This peripheral is avilable in STM8L16x devices only*/
/**
  * @brief AES tiny (AES)
  */
typedef struct AES_struct
{
  __IO uint8_t CR;      /*!< AES control register     */
  __IO uint8_t SR;      /*!< AES status register      */
  __IO uint8_t DINR;    /*!< AES Data input register  */
  __IO uint8_t DOUTR;   /*!< AES Data output register */
}
AES_TypeDef;

/** @addtogroup AES_Registers_Reset_Value
  * @{
  */
#define AES_CR_RESET_VALUE    ((uint8_t)0x00) /*!< Control Register reset value     */
#define AES_SR_RESET_VALUE    ((uint8_t)0x00) /*!< Status Register reset value      */
#define AES_DINR_RESET_VALUE  ((uint8_t)0x00) /*!< Data input register reset value  */
#define AES_DOUTR_RESET_VALUE ((uint8_t)0x00) /*!< Data output register reset value */
/**
* @}
*/

/** @addtogroup AES_Registers_Bits_Definition
  * @{
  */
#define AES_CR_DMAEN  ((uint8_t)0x80) /*!< DMA Enable bit mask */
#define AES_CR_ERRIE  ((uint8_t)0x40) /*!< Error Interrupt Enable bit mask */
#define AES_CR_CCIE   ((uint8_t)0x20) /*!< Computation Complete Interrupt Enable bit mask */
#define AES_CR_ERRC   ((uint8_t)0x10) /*!< Error Clear bit mask  */
#define AES_CR_CCFC   ((uint8_t)0x08) /*!< Computation Complete Flag Clear bit mask */
#define AES_CR_MODE   ((uint8_t)0x06) /*!< AES Modes of Operation bits mask */
#define AES_CR_EN     ((uint8_t)0x01) /*!< AES Enable bit mask */

#define AES_SR_WRERR  ((uint8_t)0x04) /*!< Write Error Flag bit mask */
#define AES_SR_RDERR  ((uint8_t)0x02) /*!< Read Error Flag bit mask */
#define AES_SR_CCF    ((uint8_t)0x01) /*!< Computation Complete Flag bit mask */

#define AES_DINR      ((uint8_t)0xFF) /*!< Data Input bits mask */
#define AES_DOUTR     ((uint8_t)0xFF) /*!< Data Output bits mask */

/**
  * @}
  */
/******************************************************************************/
/*                          Peripherals Base Address                          */
/******************************************************************************/
#define OPT_BASE                    (uint16_t)0x4800
#define GPIOA_BASE                  (uint16_t)0x5000
#define GPIOB_BASE                  (uint16_t)0x5005
#define GPIOC_BASE                  (uint16_t)0x500A
#define GPIOD_BASE                  (uint16_t)0x500F
#define GPIOE_BASE                  (uint16_t)0x5014
#define GPIOF_BASE                  (uint16_t)0x5019
#define GPIOG_BASE                  (uint16_t)0x501E
#define GPIOH_BASE                  (uint16_t)0x5023
#define GPIOI_BASE                  (uint16_t)0x5028
#define FLASH_BASE                  (uint16_t)0x5050
#define DMA1_BASE                   (uint16_t)0x5070
#define DMA1_Channel0_BASE          (uint16_t)0x5075
#define DMA1_Channel1_BASE          (uint16_t)0x507F
#define DMA1_Channel2_BASE          (uint16_t)0x5089
#define DMA1_Channel3_BASE          (uint16_t)0x5093
#define SYSCFG_BASE                 (uint16_t)0x509D
#define EXTI_BASE                   (uint16_t)0x50A0
#define WFE_BASE                    (uint16_t)0x50A6
#define RST_BASE                    (uint16_t)0x50B0
#define PWR_BASE                    (uint16_t)0x50B2
#define CLK_BASE                    (uint16_t)0x50C0
#define WWDG_BASE                   (uint16_t)0x50D3
#define IWDG_BASE                   (uint16_t)0x50E0
#define BEEP_BASE                   (uint16_t)0x50F0
#define RTC_BASE                    (uint16_t)0x5140
#define CSSLSE_BASE                 (uint16_t)0x5190
#define SPI1_BASE                   (uint16_t)0x5200
#define SPI2_BASE                   (uint16_t)0x53C0
#define I2C1_BASE                   (uint16_t)0x5210
#define USART1_BASE                 (uint16_t)0x5230
#define USART2_BASE                 (uint16_t)0x53E0
#define USART3_BASE                 (uint16_t)0x53F0
#define TIM2_BASE                   (uint16_t)0x5250
#define TIM3_BASE                   (uint16_t)0x5280
#define TIM1_BASE                   (uint16_t)0x52B0
#define TIM4_BASE                   (uint16_t)0x52E0
#define IRTIM_BASE                  (uint16_t)0x52FF
#define TIM5_BASE                   (uint16_t)0x5300
#define ADC1_BASE                   (uint16_t)0x5340
#define DAC_BASE                    (uint16_t)0x5380
#define AES_BASE                    (uint16_t)0x53D0
#define LCD_BASE                    (uint16_t)0x5400
#define RI_BASE                     (uint16_t)0x5430
#define COMP_BASE                   (uint16_t)0x5440
#define CFG_BASE                    (uint16_t)0x7F60
#define ITC_BASE                    (uint16_t)0x7F70
#define DM_BASE                     (uint16_t)0x7F90

/******************************************************************************/
/*                          Peripherals declarations                          */
/******************************************************************************/


#define SYSCFG                      ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define GPIOA                       ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB                       ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC                       ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD                       ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE                       ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF                       ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG                       ((GPIO_TypeDef *) GPIOG_BASE)
#define GPIOH                       ((GPIO_TypeDef *) GPIOH_BASE)
#define GPIOI                       ((GPIO_TypeDef *) GPIOI_BASE)
#define RTC                         ((RTC_TypeDef *) RTC_BASE)
#define FLASH                       ((FLASH_TypeDef *) FLASH_BASE)
#define EXTI                        ((EXTI_TypeDef *) EXTI_BASE)
#define RST                         ((RST_TypeDef *) RST_BASE)
#define PWR                         ((PWR_TypeDef *) PWR_BASE)
#define CLK                         ((CLK_TypeDef *) CLK_BASE)
#define CSSLSE                      ((CSSLSE_TypeDef *) CSSLSE_BASE)
#define WWDG                        ((WWDG_TypeDef *) WWDG_BASE)
#define IWDG                        ((IWDG_TypeDef *) IWDG_BASE)
#define WFE                         ((WFE_TypeDef *) WFE_BASE)
#define BEEP                        ((BEEP_TypeDef *) BEEP_BASE)
#define SPI1                        ((SPI_TypeDef *) SPI1_BASE)
#define SPI2                        ((SPI_TypeDef *) SPI2_BASE)
#define I2C1                        ((I2C_TypeDef *) I2C1_BASE)
#define USART1                      ((USART_TypeDef *) USART1_BASE)
#define USART2                      ((USART_TypeDef *) USART2_BASE)
#define USART3                      ((USART_TypeDef *) USART3_BASE)
#define LCD                         ((LCD_TypeDef *) LCD_BASE)
#define TIM1                        ((TIM1_TypeDef *) TIM1_BASE)
#define TIM2                        ((TIM_TypeDef *) TIM2_BASE)
#define TIM3                        ((TIM_TypeDef *) TIM3_BASE)
#define TIM4                        ((TIM4_TypeDef *) TIM4_BASE)
#define TIM5                        ((TIM_TypeDef *) TIM5_BASE)
#define IRTIM                       ((IRTIM_TypeDef *) IRTIM_BASE)
#define ITC                         ((ITC_TypeDef *) ITC_BASE)
#define DAC                         ((DAC_TypeDef *) DAC_BASE)
#define DMA1                        ((DMA_TypeDef *) DMA1_BASE)
#define DMA1_Channel0               ((DMA_Channel_TypeDef *) DMA1_Channel0_BASE)
#define DMA1_Channel1               ((DMA_Channel_TypeDef *) DMA1_Channel1_BASE)
#define DMA1_Channel2               ((DMA_Channel_TypeDef *) DMA1_Channel2_BASE)
#define DMA1_Channel3               ((DMA_Channel_TypeDef *) DMA1_Channel3_BASE)
#define DM                          ((DM_TypeDef *) DM_BASE)
#define RI                          ((RI_TypeDef *) RI_BASE)
#define COMP                        ((COMP_TypeDef *) COMP_BASE)
#define AES                         ((AES_TypeDef *) AES_BASE)
#define ADC1                        ((ADC_TypeDef *) ADC1_BASE)
#define CFG                         ((CFG_TypeDef *) CFG_BASE)
#define OPT                         ((OPT_TypeDef *) OPT_BASE)

#ifdef USE_STDPERIPH_DRIVER
 #include "stm8l15x_conf.h"
#endif

/* Exported macro --------------------------------------------------------------*/

/*============================== Interrupts ====================================*/
#ifdef _RAISONANCE_
 #include <intrist7.h>
 #define enableInterrupts()  _rim_()  /*!<enable interrupts */
 #define disableInterrupts() _sim_()  /*!<disable interrupts */
 #define rim()               _rim_()  /*!<enable interrupts */
 #define sim()               _sim_()  /*!<disable interrupts */
 #define nop()               _nop_()  /*!<No Operation */
 #define trap()              _trap_() /*!<Trap (soft IT) */
 #define wfi()               _wfi_()  /*!<Wait For Interrupt */
 #define wfe()               _wfe_()  /*!<Wait For Interrupt */
 #define halt()              _halt_() /*!<Halt */
#elif defined(_COSMIC_)
 #define enableInterrupts() {_asm("rim\n");} /*!<enable interrupts */
 #define disableInterrupts() {_asm("sim\n");} /*!<disable interrupts */
 #define rim() {_asm("rim\n");} /*!<enable interrupts */
 #define sim() {_asm("sim\n");} /*!<disable interrupts */
 #define nop() {_asm("nop\n");} /*!<No Operation */
 #define trap() {_asm("trap\n");} /*!<Trap (soft IT) */
 #define wfi() {_asm("wfi\n");} /*!<Wait For Interrupt */
 #define wfe() {_asm("wfe\n");} /*!<Wait for event */
 #define halt() {_asm("halt\n");} /*!<Halt */
#else /*_IAR*/
 #include <intrinsics.h>
 #define enableInterrupts()    __enable_interrupt()   /* enable interrupts */
 #define disableInterrupts()   __disable_interrupt()  /* disable interrupts */
 #define rim()                 __enable_interrupt()   /* enable interrupts */
 #define sim()                 __disable_interrupt()  /* disable interrupts */
 #define nop()                 __no_operation()       /* No Operation */
 #define trap()                __trap()               /* Trap (soft IT) */
 #define wfi()                 __wait_for_interrupt() /* Wait For Interrupt */
 #define wfe()                 __wait_for_event();    /* Wait for event */
 #define halt()                __halt()               /* Halt */
#endif /* _RAISONANCE_ */

/*============================== Interrupt vector Handling ========================*/

#ifdef _COSMIC_
 #define INTERRUPT_HANDLER(a,b) @far @interrupt void a(void)
 #define INTERRUPT_HANDLER_TRAP(a) void @far @interrupt a(void)
#endif /* _COSMIC_ */

#ifdef _RAISONANCE_
 #define INTERRUPT_HANDLER(a,b) void a(void) interrupt b
 #define INTERRUPT_HANDLER_TRAP(a) void a(void) trap
#endif /* _RAISONANCE_ */

#ifdef _IAR_
 #define STRINGVECTOR(x) #x
 #define VECTOR_ID(x) STRINGVECTOR( vector = (x) )
 #define INTERRUPT_HANDLER( a, b )  \
 _Pragma( VECTOR_ID( (b)+2 ) )        \
 __interrupt void (a)( void )
 #define INTERRUPT_HANDLER_TRAP(a) \
 _Pragma( VECTOR_ID( 1 ) ) \
 __interrupt void (a) (void)
#endif /* _IAR_ */

/*============================== Interrupt Handler declaration ========================*/
#ifdef _COSMIC_
 #define INTERRUPT @far @interrupt
#elif defined(_IAR_)
 #define INTERRUPT __interrupt
#endif /* _COSMIC_ */

/*============================== Handling bits ====================================*/
/*-----------------------------------------------------------------------------
Method : I
Description : Handle the bit from the character variables.
Comments :    The different parameters of commands are
              - VAR : Name of the character variable where the bit is located.
              - Place : Bit position in the variable (7 6 5 4 3 2 1 0)
              - Value : Can be 0 (reset bit) or not 0 (set bit)
              The "MskBit" command allows to select some bits in a source
              variables and copy it in a destination var (return the value).
              The "ValBit" command returns the value of a bit in a char
              variable: the bit is reset if it returns 0 else the bit is set.
              This method generates not an optimised code yet.
-----------------------------------------------------------------------------*/
#define SetBit(VAR,Place)         ( (VAR) |= (uint8_t)((uint8_t)1<<(uint8_t)(Place)) )
#define ClrBit(VAR,Place)         ( (VAR) &= (uint8_t)((uint8_t)((uint8_t)1<<(uint8_t)(Place))^(uint8_t)255) )

#define ChgBit(VAR,Place)         ( (VAR) ^= (uint8_t)((uint8_t)1<<(uint8_t)(Place)) )
#define AffBit(VAR,Place,Value)   ((Value) ? \
                                   ((VAR) |= ((uint8_t)1<<(Place))) : \
                                   ((VAR) &= (((uint8_t)1<<(Place))^(uint8_t)255)))
#define MskBit(Dest,Msk,Src)      ( (Dest) = ((Msk) & (Src)) | ((~(Msk)) & (Dest)) )

#define ValBit(VAR,Place)         ((uint8_t)(VAR) & (uint8_t)((uint8_t)1<<(uint8_t)(Place)))

#define BYTE_0(n)                 ((uint8_t)((n) & (uint8_t)0xFF))        /*!< Returns the low byte of the 32-bit value */
#define BYTE_1(n)                 ((uint8_t)(BYTE_0((n) >> (uint8_t)8)))  /*!< Returns the second byte of the 32-bit value */
#define BYTE_2(n)                 ((uint8_t)(BYTE_0((n) >> (uint8_t)16))) /*!< Returns the third byte of the 32-bit value */
#define BYTE_3(n)                 ((uint8_t)(BYTE_0((n) >> (uint8_t)24))) /*!< Returns the high byte of the 32-bit value */

/*============================== Assert Macros ====================================*/
#define IS_STATE_VALUE(STATE) \
  (((STATE) == SET) || \
   ((STATE) == RESET))

/*-----------------------------------------------------------------------------
Method : II
Description : Handle directly the bit.
Comments :    The idea is to handle directly with the bit name. For that, it is
              necessary to have RAM area descriptions (example: HW register...)
              and the following command line for each area.
              This method generates the most optimized code.
-----------------------------------------------------------------------------*/

#define AREA 0x00     /* The area of bits begins at address 0x10. */

#define BitClr(BIT)  ( *((unsigned char *) (AREA+(BIT)/8)) &= (~(1<<(7-(BIT)%8))) )
#define BitSet(BIT)  ( *((unsigned char *) (AREA+(BIT)/8)) |= (1<<(7-(BIT)%8)) )
#define BitVal(BIT)  ( *((unsigned char *) (AREA+(BIT)/8)) & (1<<(7-(BIT)%8)) )


#endif /* __STM8L15x_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
