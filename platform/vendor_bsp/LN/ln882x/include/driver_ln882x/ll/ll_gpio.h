#ifndef __LL_GPIO_H_
#define __LL_GPIO_H_

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"
#include "hal/syscon_types.h"

//////////////////////////////  Data type and Macros  /////////////////////////


/**
 * @brief enum of GPIO Direction.
 */
typedef enum
{
    GPIO_INPUT  = 0,
    GPIO_OUTPUT = 1,
} GPIO_Direction;


/**
 * @brief enum of Trigger type
 */
typedef enum
{
	GPIO_TRIG_LOW_LEVEL    = 0,
	GPIO_TRIG_HIGH_LEVEL   = 1,
	GPIO_TRIG_FALLING_EDGE = 2,
	GPIO_TRIG_RISING_EDGE  = 3,
} GPIO_TrigType;


/**
 * @brief enum of gpio debounce. GPIO Debounce is only useful when gpio direction is input.
 */
typedef enum
{
    GPIO_DEBOUNCE_NO  = 0,
    GPIO_DEBOUNCE_YES = 1,
} GPIO_Debounce;


/**
 * @brief enum of gpio value.
 */
typedef enum
{
    GPIO_VALUE_LOW     = 0,
    GPIO_VALUE_HIGH    = 1,
    GPIO_VALUE_DEFAULT = GPIO_VALUE_LOW,
} GPIO_Value;


/**
 * @brief enum of gpio irqlvl. Set whether gpio is level sensitive or edge sensitive.
 */
typedef enum
{
    GPIO_IRQLVL_LEVEL = 0,
    GPIO_IRQLVL_EDGE  = 1,
} GPIO_IrqLvl;


/**
 * @brief enum of gpio polarity.
 */
typedef enum
{
    GPIO_LOW_FALLING = 0,
    GPIO_HIGH_RISING = 1,
} GPIO_Polarity;


//////////////////////////////  Function Declaration  /////////////////////////
/**
 * @brief Controls the type of interrupt that can occur on GPIO ranging from GPIOA_0 to GPIOA_20
 * @param  num: the GPIO_Num to define which GPIO to operate. num can only choose from GPIOA_0 to GPIOA_20
 * @param  irqLvl: it configures the interrupt type to be level-sensitive or edge-sensitive.
 * @return This function has no return.
 */
void LL_GPIO_IrqLevelSet(GPIO_Num num, GPIO_IrqLvl irqLvl);

/**
 * @brief Controls the polarity of edge or level sensitivity that can occur on GPIO ranging from GPIOA_0 to GPIOA_20
 * @param  num: the GPIO_Num to define which GPIO to operate. num can only choose from GPIOA_0 to GPIOA_20
 * @param  ply: choose from enum GPIO_Polarity to configures the interrupt type to falling-edge or rising-edge.
 * @return This function has no return.
 */
void LL_GPIO_PolaritySet(GPIO_Num num, GPIO_Polarity ply);

/**
 * @brief Set the GPIO to trigger a interrupt at both edge, rising and falling.
 * @param  num: the GPIO_Num to define which GPIO to operate.
 * @param  enable: enable or disable whether a gpio trigger an interrupt at both edge.
 * @return This function has no return.
 */
void LL_GPIO_TrigBothEdge(GPIO_Num num, uint8_t enable);

/**
 * @brief Set GPIO Direction, Input or Output
 * @param  num: the GPIO_Num to define which GPIO to operate.
 * @param  dir: choose from GPIO_Direction, GPIO_INPUT or GPIO_OUTPUT
 * @return This function has no return.
 */
void LL_GPIO_SetDir(GPIO_Num num, GPIO_Direction dir);

/**
 * @brief Get GPIO Direction, Input or Output
 * @param  num: the GPIO_Num to define which GPIO to operate.
 * @return return the direction of the specific gpio
 */
GPIO_Direction LL_GPIO_GetDir(GPIO_Num num);

/**
 * @brief Get signals value on the External Port A.
 * 
 * @return uint32_t Only lowest 21 bits are valid, one bit for one gpio.
 */
uint32_t LL_GPIO_ExtPortAValue(void);

/**
 * @brief Get signals value on the External Port B.
 * 
 * @return uint32_t Only lowest 14 bits are valid, one bit for one gpio.
 */
uint32_t LL_GPIO_ExtPortBValue(void);

/**
 * @brief Values written to this register are output on the I/O signals for
 * Port A if the corresponding data direction bits for Port A are set
 * to Output mode and the corresponding control bit for Port A is
 * set to Software mode. The value read back is equal to the last
 * value written to this register.
 * 
 * @param value 
 */
void LL_GPIO_PORTADataRegWrite(uint32_t value);

/**
 * @brief Values written to this register are output on the I/O signals for
 * Port A if the corresponding data direction bits for Port A are set
 * to Output mode and the corresponding control bit for Port A is
 * set to Software mode. The value read back is equal to the last
 * value written to this register.
 * 
 * @return uint32_t 
 */
uint32_t LL_GPIO_PORTADataRegRead(void);

/**
 * @brief Values written to this register are output on the I/O signals for
 * Port B if the corresponding data direction bits for Port B are set
 * to Output mode and the corresponding control bit for Port B is
 * set to Software mode. The value read back is equal to the last
 * value written to this register.
 * 
 * @param value 
 */
void LL_GPIO_PORTBDataRegWrite(uint32_t value);

uint32_t LL_GPIO_PORTBDataRegRead(void);

/**
 * @brief When GPIO direction is input, read current gpio level.
 * @param  num: the GPIO_Num to define which GPIO to operate.
 * @return return the result of current gpio level.
 */
GPIO_Value LL_GPIO_ReadPin(GPIO_Num num);

/**
 * @brief When GPIO direction is output, write value to set gpio level.
 * @param  num: the GPIO_Num to define which GPIO to operate.
 * @param  value: value can be choose from GPIO_Value, GPIO_VALUE_LOW or GPIO_VALUE_HIGH
 * @return This function has no return.
 */
void LL_GPIO_WritePin(GPIO_Num num, GPIO_Value value);

/**
 * @brief Toggle a gpio pin
 * @param  num: the GPIO_Num to define which GPIO to operate.
 * @return This function has no return.
 */
void LL_GPIO_TogglePin(GPIO_Num num);

/**
 * @brief Controls whether en external signal that is the source of an interrupt needs to be debounced to remove any spurious glitches.
 * 
 * @param num  The GPIO_Num to define which GPIO to operate.
 * @param debounce enable/disable debounce.
 */
void LL_GPIO_SetDebounce(GPIO_Num num, GPIO_Debounce debounce);

/**
 * @brief Enable the interrupt of specific GPIO
 * @param  num: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void LL_GPIO_IntEnable(GPIO_Num num);

/**
 * @brief Disable the interrupt of specific GPIO
 * @param  num: the GPIO_Num to define which GPIO to operate. num can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void LL_GPIO_IntDisable(GPIO_Num num);

/**
 * @brief Mask the interrupt of specific GPIO, when the interrupt is masked, no interrupt will trigger to CPU
 * @param  num: the GPIO_Num to define which GPIO to operate. num can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void LL_GPIO_MaskIrq(GPIO_Num num);

/**
 * @brief Unmask the interrupt of specific GPIO, when the interrupt is unmasked, the interrupt will trigger to CPU
 * @param  num: the GPIO_Num to define which GPIO to operate. num can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void LL_GPIO_UnmaskIrq(GPIO_Num num);

/**
 * @brief Interrupt status of PORT A. 
 * Only 21 bits are valid, one bit for one GPIO.
 * 
 * @return uint32_t 
 */
uint32_t LL_GPIO_IntStatus(void);

/**
 * @brief Clear the interrupt of specific gpio
 * @param  num: the GPIO_Num to define which GPIO to operate. num can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void LL_GPIO_IrqClear(GPIO_Num num);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __LL_GPIO_H_
