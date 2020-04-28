/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-02-16 00:02:40
 * @LastEditTime : 2020-02-16 00:23:28
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "bsp_motor.h"   
#include <stdio.h>
#include <stdlib.h>

TIM_HandleTypeDef   motor_tim;

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
    MOTOR_TIM_RCC_CLK_ENABLE();
}

static void motor_control(uint32_t m1, uint32_t m2)
{
    if (m1 >= MOTOR_PERIOD)
        m1 = MOTOR_PERIOD;
    
    if (m2 > MOTOR_PERIOD)
        m2 = MOTOR_PERIOD;
    
    __HAL_TIM_SET_COMPARE(&motor_tim, MOTOR_FORWARD_CHANNEL, m1);
    __HAL_TIM_SET_COMPARE(&motor_tim, MOTOR_REVERSE_CHANNEL, m2);
}

void motor_set_speed(motor_mode_t s, uint8_t pwm)
{
    uint32_t pulse;
    
    if (pwm > MOTOR_MAX_DUTY)
        pwm = MOTOR_MAX_DUTY;
    
    pulse = (pwm * MOTOR_PERIOD) / MOTOR_MAX_DUTY;
    
    if (MOTOR_STOP == s)
        motor_control(0, 0);
    else if (MOTOR_FORWARD == s)
        motor_control(pulse, 0);
    else
        motor_control(0, pulse);
}

static void Error_Handler(void)
{
    while(1);
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef gpio_init;
	
    if(htim->Instance == MOTOR_TIM) {

        MOTOR_TIM_GPIO_RCC_CLK_ENABLE();
        MOTOR_TIM_AFIO_REMAP();
        
        gpio_init.Pin = MOTOR_FORWARD_PIN | MOTOR_REVERSE_PIN;
        gpio_init.Mode = GPIO_MODE_AF_PP;
        gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &gpio_init);
	}
}

void motor_init(void)
{
    TIM_OC_InitTypeDef oc_init;

    motor_tim.Instance = MOTOR_TIM;
    motor_tim.Init.Prescaler = MOTOR_PRESCALER;
    motor_tim.Init.CounterMode = TIM_COUNTERMODE_UP;
    motor_tim.Init.Period = MOTOR_PERIOD;
    motor_tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    if (HAL_TIM_Base_Init(&motor_tim) != HAL_OK)
        Error_Handler();
    
    HAL_TIM_Base_Start(&motor_tim);
    
    if (HAL_TIM_PWM_Init(&motor_tim) != HAL_OK)
        Error_Handler();
    
    HAL_TIM_MspPostInit(&motor_tim);
    
    oc_init.OCMode = TIM_OCMODE_PWM1;
    oc_init.Pulse = MOTOR_PERIOD;
    oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
    
    if (HAL_TIM_PWM_ConfigChannel(&motor_tim, &oc_init, MOTOR_FORWARD_CHANNEL) != HAL_OK)
        Error_Handler();
    
    if (HAL_TIM_PWM_ConfigChannel(&motor_tim, &oc_init, MOTOR_REVERSE_CHANNEL) != HAL_OK)
        Error_Handler();
    
    HAL_TIM_PWM_Start(&motor_tim, MOTOR_FORWARD_CHANNEL);//开启PWM通道1
    HAL_TIM_PWM_Start(&motor_tim, MOTOR_REVERSE_CHANNEL);//开启PWM通道2
    
    motor_set_speed(MOTOR_STOP, 0);
}
