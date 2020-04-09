#include "ll/ll_gpio.h"
#include "hal/hal_gpio.h"
#include "hal/hal_common.h"

/**
 * @brief Set the GPIO trigger type.
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @param  type: the GPIO_TrigType, choosing from enum GPIO_TrigType
 * @return This function has no return.
 */
void HAL_GPIO_TrigType(GPIO_Num n, GPIO_TrigType type)
{
    // FIXME: portB doesn't support Trigger
    // assert_param(IS_PORTA_GPIO(n));
    if (!IS_PORTA_GPIO(n)) {
        return;
    }
	switch(type)
	{
		case GPIO_TRIG_LOW_LEVEL:
			HAL_GPIO_IrqLevelSet(n, GPIO_IRQLVL_LEVEL);
			HAL_GPIO_PolaritySet(n, GPIO_LOW_FALLING);
			break;
		case GPIO_TRIG_HIGH_LEVEL:
			HAL_GPIO_IrqLevelSet(n, GPIO_IRQLVL_LEVEL);
			HAL_GPIO_PolaritySet(n, GPIO_HIGH_RISING);
			break;
		case GPIO_TRIG_FALLING_EDGE:
			HAL_GPIO_IrqLevelSet(n, GPIO_IRQLVL_EDGE);
			HAL_GPIO_PolaritySet(n, GPIO_LOW_FALLING);
			break;
		case GPIO_TRIG_RISING_EDGE:
			HAL_GPIO_IrqLevelSet(n, GPIO_IRQLVL_EDGE);
			HAL_GPIO_PolaritySet(n, GPIO_HIGH_RISING);
			break;
        default:
            break;
	}
}


/**
 * @brief Set the GPIO to trigger a interrupt at both edge, rising and falling.
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @param  enable: enable or disable whether a gpio trigger an interrupt at both edge.
 * @return This function has no return.
 */
void HAL_GPIO_TrigBothEdge(GPIO_Num n, uint8_t enable)
{
    assert_param(IS_PORTA_GPIO(n));

    LL_GPIO_TrigBothEdge(n, enable);
}


/**
 * @brief GPIO Initialize.
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @param  config: a struct to initialize a GPIO.
 * @return This function has no return.
 */
void HAL_GPIO_Init(GPIO_Num n, GPIO_InitTypeDef config)
{
    HAL_GPIO_SetDir(n, config.dir);
    if(GPIO_OUTPUT == config.dir) {
        // FIXME: 不应该在初始化的时候写入某个值，应该让用户自己写。
        // HAL_GPIO_WritePin(n, config.value);
    } else {
		HAL_GPIO_SetDebounce(n, config.debounce);
		HAL_GPIO_TrigType(n,config.trig_type);
    }
}

/**
 * @brief Set GPIO Direction, Input or Output
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @param  dir: choose from GPIO_Direction, GPIO_INPUT or GPIO_OUTPUT
 * @return This function has no return.
 */
void HAL_GPIO_SetDir(GPIO_Num n, GPIO_Direction dir)
{
    LL_GPIO_SetDir(n, dir);
}

/**
 * @brief Get GPIO Direction, Input or Output
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @return return the direction of the specific gpio
 */
GPIO_Direction HAL_GPIO_GetDir(GPIO_Num n)
{
    return LL_GPIO_GetDir(n);
}

/**
 * @brief Enable the interrupt of specific GPIO
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void HAL_GPIO_IntEnable(GPIO_Num n)
{
    assert_param(IS_PORTA_GPIO(n));

    LL_GPIO_IntEnable(n);
}

/**
 * @brief Disable the interrupt of specific GPIO
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void HAL_GPIO_IntDisable(GPIO_Num n)
{
    assert_param(IS_PORTA_GPIO(n));

    LL_GPIO_IntDisable(n);
}

/**
 * @brief When GPIO direction is output, write value to set gpio level.
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @param  value: value can be choose from GPIO_Value, GPIO_VALUE_LOW or GPIO_VALUE_HIGH
 * @return This function has no return.
 */
void HAL_GPIO_WritePin(GPIO_Num n, GPIO_Value value)
{
    LL_GPIO_WritePin(n, value);
}

/**
 * @brief When GPIO direction is input, read current gpio level.
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @return return the result of current gpio level.
 */
GPIO_Value HAL_GPIO_ReadPin(GPIO_Num n)
{
    return LL_GPIO_ReadPin(n);
}

/**
 * @brief Toggle a gpio pin
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @return This function has no return.
 */
void HAL_GPIO_TogglePin(GPIO_Num n)
{
    LL_GPIO_TogglePin(n);
}

/**
 * @brief Mask the interrupt of specific GPIO, when the interrupt is masked, no interrupt will trigger to CPU
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void HAL_GPIO_MaskIrq(GPIO_Num n)
{
    assert_param(IS_PORTA_GPIO(n));

    LL_GPIO_MaskIrq(n);
}

/**
 * @brief Unmask the interrupt of specific GPIO, when the interrupt is unmasked, the interrupt will trigger to CPU
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void HAL_GPIO_UnmaskIrq(GPIO_Num n)
{
    assert_param(IS_PORTA_GPIO(n));

    LL_GPIO_UnmaskIrq(n);
}

/**
 * @brief Get all the interrupt status of gpio
 * @return return all the interrupt status of gpio ranging from GPIOA_0 to GPIOA_20
 */
uint32_t HAL_GPIO_IntStatus(void)
{
    return LL_GPIO_IntStatus();
}

/**
 * @brief Clear the interrupt of specific gpio
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @return This function has no return.
 */
void HAL_GPIO_IrqClear(GPIO_Num n)
{
    assert_param(IS_PORTA_GPIO(n));

    LL_GPIO_IrqClear(n);
}

/**
 * @brief Controls the polarity of edge or level sensitivity that can occur on GPIO ranging from GPIOA_0 to GPIOA_20
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @param  ply: choose from enum GPIO_Polarity to configures the interrupt type to falling-edge or rising-edge.
 * @return This function has no return.
 */
void HAL_GPIO_PolaritySet(GPIO_Num n, GPIO_Polarity ply)
{
    assert_param(IS_PORTA_GPIO(n));

    LL_GPIO_PolaritySet(n, ply);
}

/**
 * @brief Controls the type of interrupt that can occur on GPIO ranging from GPIOA_0 to GPIOA_20
 * @param  n: the GPIO_Num to define which GPIO to operate. n can only choose from GPIOA_0 to GPIOA_20
 * @param  irqLvl: it configures the interrupt type to be level-sensitive or edge-sensitive.
 * @return This function has no return.
 */
void HAL_GPIO_IrqLevelSet(GPIO_Num n,GPIO_IrqLvl irqLvl)
{
    assert_param(IS_PORTA_GPIO(n));

    LL_GPIO_IrqLevelSet(n, irqLvl);
}


/**
 * @brief Controls whether an external signal that is the source of an interrupt needs to be debounced to remove any spurious glitches.
 * @param  n: the GPIO_Num to define which GPIO to operate.
 * @return This function has no return.
 */
void HAL_GPIO_SetDebounce(GPIO_Num n, GPIO_Debounce debounce)
{
    // FIXME: portb doesn't support DEBOUNCE.
    // assert_param(IS_PORTA_GPIO(n));
    if (IS_PORTA_GPIO(n)) {
        LL_GPIO_SetDebounce(n, debounce);
    }
}

SYSTEM_EXT_INT_Wakeup_Index HAL_GPIO_Mapping_To_Ext_Int(GPIO_Num gpio)
{
    SYSTEM_EXT_INT_Wakeup_Index int_idx = SYSTEM_EXT_INT_ERROR;

    switch(gpio)
    {
        case GPIOA_0:
            int_idx = SYSTEM_EXT_INT0;
            break;
        case GPIOA_1:
            int_idx = SYSTEM_EXT_INT1;
            break;
        case GPIOA_2:
            int_idx = SYSTEM_EXT_INT2;
            break;
        case GPIOA_3:
            int_idx = SYSTEM_EXT_INT3;
            break;
        case GPIOA_5:
            int_idx = SYSTEM_EXT_INT4;
            break;
        case GPIOA_6:
            int_idx = SYSTEM_EXT_INT5;
            break;
        case GPIOA_7:
            int_idx = SYSTEM_EXT_INT6;
            break;
        case GPIOB_9:
            int_idx = SYSTEM_EXT_INT7;
            break;
        default:
            break;
    }
    return int_idx;
}
/// @} GPIO
