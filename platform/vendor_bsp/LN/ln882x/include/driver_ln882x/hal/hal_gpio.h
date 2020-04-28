/**
****************************************************************************************
*
* @file hal_gpio.h
*
* @brief gpio driver interface.
*
* Copyright (C) LightningSemi electronics 2016-2017
*
*
****************************************************************************************
*/



/**
****************************************************************************************
* @defgroup GPIO
* @ingroup DRIVERS
* @brief GPIO Drivers.
****************************************************************************************
*/

#ifndef __HAL_GPIO_H_
#define __HAL_GPIO_H_
#include "types.h"

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus


//////////////////////////////  Data type and Macros  /////////////////////////

#include "ll/ll_gpio.h"
#include "hal/syscon_types.h"

/**
 * @brief This is the struct to initialize the gpio module.
 */
typedef struct
{
    GPIO_Direction  dir;        /**< dir: select GPIO direction */
    GPIO_Debounce   debounce;   /**< debounce: enable debounce */
    GPIO_Value      value;      /**< value: set gpio pin value */
    // Irq releated data
    GPIO_TrigType   trig_type;  /**< trig_type: set gpio trigger type */
} GPIO_InitTypeDef;


//////////////////////////////  Function Declaration  /////////////////////////

/**
 * @brief Set the GPIO trigger type.
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @param  type: the GPIO_TrigType, choosing from enum GPIO_TrigType
 * @return This function has no return.
 */
void HAL_GPIO_TrigType(GPIO_Num n, GPIO_TrigType type);

/**
 * @brief Set the GPIO to trigger a interrupt at both edge, rising and falling.
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @param  enable: enable or disable whether a gpio trigger an interrupt at both edge.
 * @return This function has no return.
 */
void HAL_GPIO_TrigBothEdge(GPIO_Num n, uint8_t enable);

/**
 * @brief GPIO Initialize.
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @param  config: a struct to initialize a GPIO.
 * @return This function has no return.
 */
void HAL_GPIO_Init(GPIO_Num n, GPIO_InitTypeDef config);

/**
 * @brief Set GPIO Direction, Input or Output
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @param  dir: choose from GPIO_Direction, GPIO_INPUT or GPIO_OUTPUT
 * @return This function has no return.
 */
void HAL_GPIO_SetDir(GPIO_Num n, GPIO_Direction dir);

/**
 * @brief Get GPIO Direction, Input or Output
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @return return the direction of the specific gpio
 */
GPIO_Direction HAL_GPIO_GetDir(GPIO_Num n);

/**
 * @brief Enable the interrupt of specific GPIO
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void HAL_GPIO_IntEnable(GPIO_Num n);

/**
 * @brief Disable the interrupt of specific GPIO
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void HAL_GPIO_IntDisable(GPIO_Num n);

/**
 * @brief When GPIO direction is output, write value to set gpio level.
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @param  value: value can be choose from GPIO_Value, GPIO_VALUE_LOW or GPIO_VALUE_HIGH
 * @return This function has no return.
 */
void HAL_GPIO_WritePin(GPIO_Num n, GPIO_Value value);

/**
 * @brief When GPIO direction is input, read current gpio level.
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @return return the result of current gpio level.
 */
GPIO_Value HAL_GPIO_ReadPin(GPIO_Num n);

/**
 * @brief Toggle a gpio pin
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @return This function has no return.
 */
void HAL_GPIO_TogglePin(GPIO_Num n);

/**
 * @brief Mask the interrupt of specific GPIO, when the interrupt is masked, no interrupt will trigger to CPU
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void HAL_GPIO_MaskIrq(GPIO_Num n);

/**
 * @brief Unmask the interrupt of specific GPIO, when the interrupt is unmasked, the interrupt will trigger to CPU
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void HAL_GPIO_UnmaskIrq(GPIO_Num n);

/**
 * @brief Get all the interrupt status of gpio
 * @return return all the interrupt status of gpio ranging from GPIOA_0 to GPIOA_20
 */
uint32_t HAL_GPIO_IntStatus(void);

/**
 * @brief Clear the interrupt of specific gpio
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void HAL_GPIO_IrqClear(GPIO_Num n);

/**
 * @brief Controls the polarity of edge or level sensitivity that can occur on GPIO ranging from GPIOA_0 to GPIOA_20
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @param  ply: choose from enum GPIO_Polarity to configures the interrupt type to falling-edge or rising-edge.
 * @return This function has no return.
 */
void HAL_GPIO_PolaritySet(GPIO_Num n, GPIO_Polarity ply);

/**
 * @brief Controls the type of interrupt that can occur on GPIO ranging from GPIOA_0 to GPIOA_20
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @param  irqLvl: it configures the interrupt type to be level-sensitive or edge-sensitive.
 * @return This function has no return.
 */
void HAL_GPIO_IrqLevelSet(GPIO_Num n,GPIO_IrqLvl irqLvl);

/**
 * @brief Controls whether an external signal that is the source of an interrupt needs to be debounced to remove any spurious glitches.
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @return This function has no return.
 */
void HAL_GPIO_SetDebounce(GPIO_Num n, GPIO_Debounce debounce);

SYSTEM_EXT_INT_Wakeup_Index HAL_GPIO_Mapping_To_Ext_Int(GPIO_Num gpio);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __HAL_GPIO_H_

/// @} GPIO

