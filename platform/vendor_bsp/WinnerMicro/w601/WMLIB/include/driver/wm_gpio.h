/**
 * @file wm_gpio.h
 *
 * @brief GPIO Driver Module
 *
 * @author dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_GPIO_H
#define WM_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wm_type_def.h"
#include "wm_io.h"

/** gpio interrupte callback function */
typedef void (*tls_gpio_irq_callback)(void *arg);

/** Indicating gpio direction */
enum tls_gpio_dir {
    WM_GPIO_DIR_OUTPUT,    /**< output */
    WM_GPIO_DIR_INPUT      /**< input */
};

/** Indicating gpio attribute */
enum tls_gpio_attr {
    WM_GPIO_ATTR_FLOATING,    /**< floating status */
    WM_GPIO_ATTR_PULLHIGH,    /**< pull high */
    WM_GPIO_ATTR_PULLLOW      /**< pull low */
};

/** Indicating gpio interrupt trigger type */
enum tls_gpio_irq_trig {
    WM_GPIO_IRQ_TRIG_RISING_EDGE,    /**< rising edge arises the interrupt */
    WM_GPIO_IRQ_TRIG_FALLING_EDGE,   /**< falling edge arises the interrupt */
    WM_GPIO_IRQ_TRIG_DOUBLE_EDGE,    /**< both rising edge and falling edge arise the interrupt */
    WM_GPIO_IRQ_TRIG_HIGH_LEVEL,     /**< high power level arises the interrupt */
    WM_GPIO_IRQ_TRIG_LOW_LEVEL       /**< low power level arises the interrupt */
};

/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup GPIO_Driver_APIs GPIO Driver APIs
 * @brief GPIO driver APIs
 */

/**
 * @addtogroup GPIO_Driver_APIs
 * @{
 */


/**
 * @brief          	This function is used to config gpio function
 *
 * @param[in]      	gpio_pin    	gpio pin num
 * @param[in]      	dir         		gpio direction
 * @param[in]      	attr        		gpio attribute
 *
 * @return         None
 *
 * @note			None	
 */
void tls_gpio_cfg(enum tls_io_name gpio_pin, enum tls_gpio_dir dir, enum tls_gpio_attr attr);


/**
 * @brief          This function is used to read gpio status
 *
 * @param[in]      gpio_pin    gpio pin num
 *
 * @retval         0     power level is low
 * @retval         1     power level is high
 *
 * @note           None
 */
u8 tls_gpio_read(enum tls_io_name gpio_pin);


/**
 * @brief          	This function is used to modify gpio status
 *
 * @param[in]      	gpio_pin    	gpio pin num
 * @param[in]      	value       	power level
 *                        	0: 			low  power level
 * 				1: 			high power level
 *
 * @return         	None
 *
 * @note           None
 */
void tls_gpio_write(enum tls_io_name gpio_pin, u8 value);


/**
 * @brief          This function is used to config gpio interrupt
 *
 * @param[in]      gpio_pin    gpio pin num
 * @param[in]      mode        interrupt trigger type
 *
 * @return         None
 *
 * @note           None
 */
void tls_gpio_irq_enable(enum tls_io_name gpio_pin, enum tls_gpio_irq_trig mode);


/**
 * @brief          This function is used to disable gpio interrupt
 *
 * @param[in]      gpio_pin    gpio pin num
 *
 * @return         None
 *
 * @note           None
 */
void tls_gpio_irq_disable(enum tls_io_name gpio_pin);


/**
 * @brief          This function is used to get gpio interrupt status
 *
 * @param[in]      gpio_pin    gpio pin num
 *
 * @retval         0     no interrupt happened
 * @retval         1     interrupt happened
 *
 * @note           None
 */
u8 tls_get_gpio_irq_status(enum tls_io_name gpio_pin);


/**
 * @brief          This function is used to clear gpio interrupt flag
 *
 * @param[in]      gpio_pin    gpio pin num
 *
 * @return         None
 *
 * @note           None
 */
void tls_clr_gpio_irq_status(enum tls_io_name gpio_pin);


/**
 * @brief          This function is used to register gpio interrupt
 *
 * @param[in]      gpio_pin    gpio pin num
 * @param[in]      callback    the gpio interrupt call back function
 * @param[in]      arg         parammeter for the callback
 *
 * @return         None
 *
 * @note
 * gpio callback function is called in interrupt,
 * so can not operate the critical data in the callback fuuction,
 * recommendation to send messages to other tasks to operate it.
 */
void tls_gpio_isr_register(enum tls_io_name gpio_pin,
                           tls_gpio_irq_callback callback,
                           void *arg);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* end of WM_GPIO_H */

/*** (C) COPYRIGHT 2014 Winner Microelectronics Co., Ltd. ***/
