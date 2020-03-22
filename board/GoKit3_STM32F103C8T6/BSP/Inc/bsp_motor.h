/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-02-16 00:02:40
 * @LastEditTime : 2020-02-16 00:31:10
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _BSP_MOTOR_H
#define	_BSP_MOTOR_H

#include "stm32f1xx_hal.h"

#define MOTOR_FORWARD_PORT                  GPIOB
#define MOTOR_FORWARD_PIN                   GPIO_PIN_4
#define MOTOR_FORWARD_CHANNEL               TIM_CHANNEL_1

         
#define MOTOR_REVERSE_PORT                  GPIOB
#define MOTOR_REVERSE_PIN                   GPIO_PIN_5
#define MOTOR_REVERSE_CHANNEL               TIM_CHANNEL_2

#define MOTOR_TIM                           TIM3
#define MOTOR_TIM_RCC_CLK_ENABLE()          __HAL_RCC_TIM3_CLK_ENABLE()
#define MOTOR_TIM_AFIO_REMAP()              __HAL_AFIO_REMAP_TIM3_PARTIAL()
#define MOTOR_TIM_RCC_CLK_DISABLE()         __HAL_RCC_TIM3_CLK_DISABLE()
#define MOTOR_TIM_GPIO_RCC_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE();

//72M/72=1M的计数频率，自动重装载为500，那么PWM频率为1M/250=4kHZ
#define MOTOR_PRESCALER     71      //定时器预分频
#define MOTOR_PERIOD        249     // 定时器重装载值

#define MOTOR_MAX_DUTY      100     // 最大占空比

typedef enum motor_mode {
    MOTOR_STOP = 0,
    MOTOR_FORWARD = 1, 
    MOTOR_REVERSE = 2
} motor_mode_t;

void motor_init(void);
void motor_set_speed(motor_mode_t s, uint8_t pwm);
    
#endif /* _BSP_MOTOR_H */
