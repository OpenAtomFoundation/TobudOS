/**
  ******************************************************************************
  * @file    ft5336.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the
  *          ft5336.c Touch screen driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FT5336_H
#define __FT5336_H

#ifdef __cplusplus
extern "C" {
#endif

/* Set Multi-touch as supported */
#if !defined(TS_MONO_TOUCH_SUPPORTED)
#define TS_MULTI_TOUCH_SUPPORTED        1
#endif /* TS_MONO_TOUCH_SUPPORTED */

/* Includes ------------------------------------------------------------------*/
#include "../Common/ts.h"

/* Macros --------------------------------------------------------------------*/

#if defined(FT5336_ENABLE_ASSERT)
/* Assert activated */
#define FT5336_ASSERT(__condition__)      do { if(__condition__) \
                                               {  \
                                                 while(1);  \
                                               } \
                                          }while(0)
#else
/* Assert not activated : macro has no effect */
#define FT5336_ASSERT(__condition__)    do { if(__condition__) \
                                             {  \
                                                ;  \
                                             } \
                                            }while(0)
#endif /* FT5336_ENABLE_ASSERT == 1 */

/** @typedef ft5336_handle_TypeDef
 *  ft5336 Handle definition.
 */
typedef struct
{
  uint8_t i2cInitialized;

  /* field holding the current number of simultaneous active touches */
  uint8_t currActiveTouchNb;

  /* field holding the touch index currently managed */
  uint8_t currActiveTouchIdx;

} ft5336_handle_TypeDef;

  /** @addtogroup BSP
   * @{
   */

  /** @addtogroup Component
   * @{
   */

  /** @defgroup FT5336
   * @{
   */

  /* Exported types ------------------------------------------------------------*/

  /** @defgroup FT5336_Exported_Types
   * @{
   */

  /* Exported constants --------------------------------------------------------*/

  /** @defgroup FT5336_Exported_Constants
   * @{
   */

  /* I2C Slave address of touchscreen FocalTech FT5336 */
#define FT5336_I2C_SLAVE_ADDRESS            ((uint8_t)0x70)

  /* Maximum border values of the touchscreen pad */
#define FT5336_MAX_WIDTH                    ((uint16_t)480)     /* Touchscreen pad max width   */
#define FT5336_MAX_HEIGHT                   ((uint16_t)272)     /* Touchscreen pad max height  */

  /* Possible values of driver functions return status */
#define FT5336_STATUS_OK                    ((uint8_t)0x00)
#define FT5336_STATUS_NOT_OK                ((uint8_t)0x01)

  /* Possible values of global variable 'TS_I2C_Initialized' */
#define FT5336_I2C_NOT_INITIALIZED          ((uint8_t)0x00)
#define FT5336_I2C_INITIALIZED              ((uint8_t)0x01)

  /* Max detectable simultaneous touches */
#define FT5336_MAX_DETECTABLE_TOUCH         ((uint8_t)0x05)

  /**
   * @brief : Definitions for FT5336 I2C register addresses on 8 bit
   **/

  /* Current mode register of the FT5336 (R/W) */
#define FT5336_DEV_MODE_REG                 ((uint8_t)0x00)

  /* Possible values of FT5336_DEV_MODE_REG */
#define FT5336_DEV_MODE_WORKING             ((uint8_t)0x00)
#define FT5336_DEV_MODE_FACTORY             ((uint8_t)0x04)

#define FT5336_DEV_MODE_MASK                ((uint8_t)0x07)
#define FT5336_DEV_MODE_SHIFT               ((uint8_t)0x04)

  /* Gesture ID register */
#define FT5336_GEST_ID_REG                  ((uint8_t)0x01)

  /* Possible values of FT5336_GEST_ID_REG */
#define FT5336_GEST_ID_NO_GESTURE           ((uint8_t)0x00)
#define FT5336_GEST_ID_MOVE_UP              ((uint8_t)0x10)
#define FT5336_GEST_ID_MOVE_RIGHT           ((uint8_t)0x14)
#define FT5336_GEST_ID_MOVE_DOWN            ((uint8_t)0x18)
#define FT5336_GEST_ID_MOVE_LEFT            ((uint8_t)0x1C)
#define FT5336_GEST_ID_SINGLE_CLICK         ((uint8_t)0x20)
#define FT5336_GEST_ID_DOUBLE_CLICK         ((uint8_t)0x22)
#define FT5336_GEST_ID_ROTATE_CLOCKWISE     ((uint8_t)0x28)
#define FT5336_GEST_ID_ROTATE_C_CLOCKWISE   ((uint8_t)0x29)
#define FT5336_GEST_ID_ZOOM_IN              ((uint8_t)0x40)
#define FT5336_GEST_ID_ZOOM_OUT             ((uint8_t)0x49)

  /* Touch Data Status register : gives number of active touch points (0..5) */
#define FT5336_TD_STAT_REG                  ((uint8_t)0x02)

  /* Values related to FT5336_TD_STAT_REG */
#define FT5336_TD_STAT_MASK                 ((uint8_t)0x0F)
#define FT5336_TD_STAT_SHIFT                ((uint8_t)0x00)

  /* Values Pn_XH and Pn_YH related */
#define FT5336_TOUCH_EVT_FLAG_PRESS_DOWN    ((uint8_t)0x00)
#define FT5336_TOUCH_EVT_FLAG_LIFT_UP       ((uint8_t)0x01)
#define FT5336_TOUCH_EVT_FLAG_CONTACT       ((uint8_t)0x02)
#define FT5336_TOUCH_EVT_FLAG_NO_EVENT      ((uint8_t)0x03)

#define FT5336_TOUCH_EVT_FLAG_SHIFT         ((uint8_t)0x06)
#define FT5336_TOUCH_EVT_FLAG_MASK          ((uint8_t)(3 << FT5336_TOUCH_EVT_FLAG_SHIFT))

#define FT5336_TOUCH_POS_MSB_MASK           ((uint8_t)0x0F)
#define FT5336_TOUCH_POS_MSB_SHIFT          ((uint8_t)0x00)

  /* Values Pn_XL and Pn_YL related */
#define FT5336_TOUCH_POS_LSB_MASK           ((uint8_t)0xFF)
#define FT5336_TOUCH_POS_LSB_SHIFT          ((uint8_t)0x00)

#define FT5336_P1_XH_REG                    ((uint8_t)0x03)
#define FT5336_P1_XL_REG                    ((uint8_t)0x04)
#define FT5336_P1_YH_REG                    ((uint8_t)0x05)
#define FT5336_P1_YL_REG                    ((uint8_t)0x06)

/* Touch Pressure register value (R) */
#define FT5336_P1_WEIGHT_REG                ((uint8_t)0x07)

/* Values Pn_WEIGHT related  */
#define FT5336_TOUCH_WEIGHT_MASK            ((uint8_t)0xFF)
#define FT5336_TOUCH_WEIGHT_SHIFT           ((uint8_t)0x00)

/* Touch area register */
#define FT5336_P1_MISC_REG                  ((uint8_t)0x08)

/* Values related to FT5336_Pn_MISC_REG */
#define FT5336_TOUCH_AREA_MASK              ((uint8_t)(0x04 << 4))
#define FT5336_TOUCH_AREA_SHIFT             ((uint8_t)0x04)

#define FT5336_P2_XH_REG                    ((uint8_t)0x09)
#define FT5336_P2_XL_REG                    ((uint8_t)0x0A)
#define FT5336_P2_YH_REG                    ((uint8_t)0x0B)
#define FT5336_P2_YL_REG                    ((uint8_t)0x0C)
#define FT5336_P2_WEIGHT_REG                ((uint8_t)0x0D)
#define FT5336_P2_MISC_REG                  ((uint8_t)0x0E)

#define FT5336_P3_XH_REG                    ((uint8_t)0x0F)
#define FT5336_P3_XL_REG                    ((uint8_t)0x10)
#define FT5336_P3_YH_REG                    ((uint8_t)0x11)
#define FT5336_P3_YL_REG                    ((uint8_t)0x12)
#define FT5336_P3_WEIGHT_REG                ((uint8_t)0x13)
#define FT5336_P3_MISC_REG                  ((uint8_t)0x14)

#define FT5336_P4_XH_REG                    ((uint8_t)0x15)
#define FT5336_P4_XL_REG                    ((uint8_t)0x16)
#define FT5336_P4_YH_REG                    ((uint8_t)0x17)
#define FT5336_P4_YL_REG                    ((uint8_t)0x18)
#define FT5336_P4_WEIGHT_REG                ((uint8_t)0x19)
#define FT5336_P4_MISC_REG                  ((uint8_t)0x1A)

#define FT5336_P5_XH_REG                    ((uint8_t)0x1B)
#define FT5336_P5_XL_REG                    ((uint8_t)0x1C)
#define FT5336_P5_YH_REG                    ((uint8_t)0x1D)
#define FT5336_P5_YL_REG                    ((uint8_t)0x1E)
#define FT5336_P5_WEIGHT_REG                ((uint8_t)0x1F)
#define FT5336_P5_MISC_REG                  ((uint8_t)0x20)

#define FT5336_P6_XH_REG                    ((uint8_t)0x21)
#define FT5336_P6_XL_REG                    ((uint8_t)0x22)
#define FT5336_P6_YH_REG                    ((uint8_t)0x23)
#define FT5336_P6_YL_REG                    ((uint8_t)0x24)
#define FT5336_P6_WEIGHT_REG                ((uint8_t)0x25)
#define FT5336_P6_MISC_REG                  ((uint8_t)0x26)

#define FT5336_P7_XH_REG                    ((uint8_t)0x27)
#define FT5336_P7_XL_REG                    ((uint8_t)0x28)
#define FT5336_P7_YH_REG                    ((uint8_t)0x29)
#define FT5336_P7_YL_REG                    ((uint8_t)0x2A)
#define FT5336_P7_WEIGHT_REG                ((uint8_t)0x2B)
#define FT5336_P7_MISC_REG                  ((uint8_t)0x2C)

#define FT5336_P8_XH_REG                    ((uint8_t)0x2D)
#define FT5336_P8_XL_REG                    ((uint8_t)0x2E)
#define FT5336_P8_YH_REG                    ((uint8_t)0x2F)
#define FT5336_P8_YL_REG                    ((uint8_t)0x30)
#define FT5336_P8_WEIGHT_REG                ((uint8_t)0x31)
#define FT5336_P8_MISC_REG                  ((uint8_t)0x32)

#define FT5336_P9_XH_REG                    ((uint8_t)0x33)
#define FT5336_P9_XL_REG                    ((uint8_t)0x34)
#define FT5336_P9_YH_REG                    ((uint8_t)0x35)
#define FT5336_P9_YL_REG                    ((uint8_t)0x36)
#define FT5336_P9_WEIGHT_REG                ((uint8_t)0x37)
#define FT5336_P9_MISC_REG                  ((uint8_t)0x38)

#define FT5336_P10_XH_REG                   ((uint8_t)0x39)
#define FT5336_P10_XL_REG                   ((uint8_t)0x3A)
#define FT5336_P10_YH_REG                   ((uint8_t)0x3B)
#define FT5336_P10_YL_REG                   ((uint8_t)0x3C)
#define FT5336_P10_WEIGHT_REG               ((uint8_t)0x3D)
#define FT5336_P10_MISC_REG                 ((uint8_t)0x3E)

  /* Threshold for touch detection */
#define FT5336_TH_GROUP_REG                 ((uint8_t)0x80)

  /* Values FT5336_TH_GROUP_REG : threshold related  */
#define FT5336_THRESHOLD_MASK               ((uint8_t)0xFF)
#define FT5336_THRESHOLD_SHIFT              ((uint8_t)0x00)

  /* Filter function coefficients */
#define FT5336_TH_DIFF_REG                  ((uint8_t)0x85)

  /* Control register */
#define FT5336_CTRL_REG                     ((uint8_t)0x86)

  /* Values related to FT5336_CTRL_REG */

  /* Will keep the Active mode when there is no touching */
#define FT5336_CTRL_KEEP_ACTIVE_MODE        ((uint8_t)0x00)

  /* Switching from Active mode to Monitor mode automatically when there is no touching */
#define FT5336_CTRL_KEEP_AUTO_SWITCH_MONITOR_MODE  ((uint8_t)0x01

  /* The time period of switching from Active mode to Monitor mode when there is no touching */
#define FT5336_TIMEENTERMONITOR_REG         ((uint8_t)0x87)

  /* Report rate in Active mode */
#define FT5336_PERIODACTIVE_REG             ((uint8_t)0x88)

  /* Report rate in Monitor mode */
#define FT5336_PERIODMONITOR_REG            ((uint8_t)0x89)

  /* The value of the minimum allowed angle while Rotating gesture mode */
#define FT5336_RADIAN_VALUE_REG             ((uint8_t)0x91)

  /* Maximum offset while Moving Left and Moving Right gesture */
#define FT5336_OFFSET_LEFT_RIGHT_REG        ((uint8_t)0x92)

  /* Maximum offset while Moving Up and Moving Down gesture */
#define FT5336_OFFSET_UP_DOWN_REG           ((uint8_t)0x93)

  /* Minimum distance while Moving Left and Moving Right gesture */
#define FT5336_DISTANCE_LEFT_RIGHT_REG      ((uint8_t)0x94)

  /* Minimum distance while Moving Up and Moving Down gesture */
#define FT5336_DISTANCE_UP_DOWN_REG         ((uint8_t)0x95)

  /* Maximum distance while Zoom In and Zoom Out gesture */
#define FT5336_DISTANCE_ZOOM_REG            ((uint8_t)0x96)

  /* High 8-bit of LIB Version info */
#define FT5336_LIB_VER_H_REG                ((uint8_t)0xA1)

  /* Low 8-bit of LIB Version info */
#define FT5336_LIB_VER_L_REG                ((uint8_t)0xA2)

  /* Chip Selecting */
#define FT5336_CIPHER_REG                   ((uint8_t)0xA3)

  /* Interrupt mode register (used when in interrupt mode) */
#define FT5336_GMODE_REG                    ((uint8_t)0xA4)

#define FT5336_G_MODE_INTERRUPT_MASK        ((uint8_t)0x03)
#define FT5336_G_MODE_INTERRUPT_SHIFT       ((uint8_t)0x00)

  /* Possible values of FT5336_GMODE_REG */
#define FT5336_G_MODE_INTERRUPT_POLLING     ((uint8_t)0x00)
#define FT5336_G_MODE_INTERRUPT_TRIGGER     ((uint8_t)0x01)

  /* Current power mode the FT5336 system is in (R) */
#define FT5336_PWR_MODE_REG                 ((uint8_t)0xA5)

  /* FT5336 firmware version */
#define FT5336_FIRMID_REG                   ((uint8_t)0xA6)

  /* FT5336 Chip identification register */
#define FT5336_CHIP_ID_REG                  ((uint8_t)0xA8)

  /*  Possible values of FT5336_CHIP_ID_REG */
#define FT5336_ID_VALUE                     ((uint8_t)0x51)

  /* Release code version */
#define FT5336_RELEASE_CODE_ID_REG          ((uint8_t)0xAF)

  /* Current operating mode the FT5336 system is in (R) */
#define FT5336_STATE_REG                    ((uint8_t)0xBC)

  /**
   * @}
   */

  /* Exported macro ------------------------------------------------------------*/

  /** @defgroup ft5336_Exported_Macros
   * @{
   */

  /* Exported functions --------------------------------------------------------*/

  /** @defgroup ft5336_Exported_Functions
   * @{
   */

  /**
   * @brief ft5336 Control functions
   */


/**
 * @brief  Initialize the ft5336 communication bus
 *         from MCU to FT5336 : ie I2C channel initialization (if required).
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT5336).
 * @retval None
 */
void ft5336_Init(uint16_t DeviceAddr);

/**
 * @brief  Software Reset the ft5336.
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT5336).
 * @retval None
 */
void ft5336_Reset(uint16_t DeviceAddr);

/**
 * @brief  Read the ft5336 device ID, pre initialize I2C in case of need to be
 *         able to read the FT5336 device ID, and verify this is a FT5336.
 * @param  DeviceAddr: I2C FT5336 Slave address.
 * @retval The Device ID (two bytes).
 */
uint16_t ft5336_ReadID(uint16_t DeviceAddr);

/**
 * @brief  Configures the touch Screen IC device to start detecting touches
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address).
 * @retval None.
 */
void ft5336_TS_Start(uint16_t DeviceAddr);

/**
 * @brief  Return if there is touches detected or not.
 *         Try to detect new touches and forget the old ones (reset internal global
 *         variables).
 * @param  DeviceAddr: Device address on communication Bus.
 * @retval : Number of active touches detected (can be 0, 1 or 2).
 */
uint8_t ft5336_TS_DetectTouch(uint16_t DeviceAddr);

/**
 * @brief  Get the touch screen X and Y positions values
 *         Manage multi touch thanks to touch Index global
 *         variable 'ft5336_handle.currActiveTouchIdx'.
 * @param  DeviceAddr: Device address on communication Bus.
 * @param  X: Pointer to X position value
 * @param  Y: Pointer to Y position value
 * @retval None.
 */
void ft5336_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y);

/**
 * @brief  Configure the FT5336 device to generate IT on given INT pin
 *         connected to MCU as EXTI.
 * @param  DeviceAddr: Device address on communication Bus (Slave I2C address of FT5336).
 * @retval None
 */
void ft5336_TS_EnableIT(uint16_t DeviceAddr);

/**
 * @brief  Configure the FT5336 device to stop generating IT on the given INT pin
 *         connected to MCU as EXTI.
 * @param  DeviceAddr: Device address on communication Bus (Slave I2C address of FT5336).
 * @retval None
 */
void ft5336_TS_DisableIT(uint16_t DeviceAddr);

/**
 * @brief  Get IT status from FT5336 interrupt status registers
 *         Should be called Following an EXTI coming to the MCU to know the detailed
 *         reason of the interrupt.
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT5336).
 * @retval TS interrupts status
 */
uint8_t ft5336_TS_ITStatus (uint16_t DeviceAddr);

/**
 * @brief  Clear IT status in FT5336 interrupt status clear registers
 *         Should be called Following an EXTI coming to the MCU.
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT5336).
 * @retval TS interrupts status
 */
void ft5336_TS_ClearIT (uint16_t DeviceAddr);

/**** NEW FEATURES enabled when Multi-touch support is enabled ****/

#if (TS_MULTI_TOUCH_SUPPORTED == 1)

/**
 * @brief  Get the last touch gesture identification (zoom, move up/down...).
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT5336).
 * @param  pGestureId : Pointer to get last touch gesture Identification.
 * @retval None.
 */
void ft5336_TS_GetGestureID(uint16_t DeviceAddr, uint32_t * pGestureId);

/**
 * @brief  Get the touch detailed informations on touch number 'touchIdx' (0..1)
 *         This touch detailed information contains :
 *         - weight that was applied to this touch
 *         - sub-area of the touch in the touch panel
 *         - event of linked to the touch (press down, lift up, ...)
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of FT5336).
 * @param  touchIdx : Passed index of the touch (0..1) on which we want to get the
 *                    detailed information.
 * @param  pWeight : Pointer to to get the weight information of 'touchIdx'.
 * @param  pArea   : Pointer to to get the sub-area information of 'touchIdx'.
 * @param  pEvent  : Pointer to to get the event information of 'touchIdx'.

 * @retval None.
 */
void ft5336_TS_GetTouchInfo(uint16_t   DeviceAddr,
                            uint32_t   touchIdx,
                            uint32_t * pWeight,
                            uint32_t * pArea,
                            uint32_t * pEvent);

#endif /* TS_MULTI_TOUCH_SUPPORTED == 1 */

/* Imported TS IO functions --------------------------------------------------------*/

/** @defgroup ft5336_Imported_Functions
 * @{
 */

/* TouchScreen (TS) external IO functions */
extern void     TS_IO_Init(void);
extern void    TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
extern uint8_t TS_IO_Read(uint8_t Addr, uint8_t Reg);
extern void    TS_IO_Delay(uint32_t Delay);

  /**
   * @}
   */

  /* Imported global variables --------------------------------------------------------*/

  /** @defgroup ft5336_Imported_Globals
   * @{
   */


/* Touch screen driver structure */
extern TS_DrvTypeDef ft5336_ts_drv;

  /**
   * @}
   */

#ifdef __cplusplus
}
#endif
#endif /* __FT5336_H */


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
