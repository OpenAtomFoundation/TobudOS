/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* define the mds(mindswitch) in and out, the trigger in only from trigger out. the connection in only for connection out. */

#ifndef __HAL_MDS_REMAP_H__
#define __HAL_MDS_REMAP_H__

#define MDS_CLU_IN_LEN 4u /* one clu input max length. */

#define CONN_TRIGGER_IN_MASK 19u /* define the connection trigger source mask. */

/* Trigger In. */
#define MDS_TRIGGER_IN_LOGIC_0          0u
#define MDS_TRIGGER_IN_LOGIC_1          1u
#define MDS_TRIGGER_IN_SW               2u
#define MDS_TRIGGER_IN_TIM1_CC1         3u
#define MDS_TRIGGER_IN_TIM1_CC2         4u
#define MDS_TRIGGER_IN_TIM1_CC3         5u
#define MDS_TRIGGER_IN_TIM1_CC4         6u
#define MDS_TRIGGER_IN_TIM1_CC5         7u
#define MDS_TRIGGER_IN_TIM2_CC1         8u
#define MDS_TRIGGER_IN_TIM2_CC2         9u
#define MDS_TRIGGER_IN_TIM2_CC3         10u
#define MDS_TRIGGER_IN_TIM2_CC4         11u
#define MDS_TRIGGER_IN_TIM3_CC1         12u
#define MDS_TRIGGER_IN_TIM3_CC2         13u
#define MDS_TRIGGER_IN_TIM3_CC3         14u
#define MDS_TRIGGER_IN_TIM3_CC4         15u
#define MDS_TRIGGER_IN_TIM4_CC1         16u
#define MDS_TRIGGER_IN_TIM4_CC2         17u
#define MDS_TRIGGER_IN_TIM4_CC3         18u
#define MDS_TRIGGER_IN_TIM4_CC4         19u
#define MDS_TRIGGER_IN_TIM5_CC1         20u
#define MDS_TRIGGER_IN_TIM5_CC2         21u
#define MDS_TRIGGER_IN_TIM5_CC3         22u
#define MDS_TRIGGER_IN_TIM5_CC4         23u
#define MDS_TRIGGER_IN_TIM8_CC1         24u
#define MDS_TRIGGER_IN_TIM8_CC2         25u
#define MDS_TRIGGER_IN_TIM8_CC3         26u
#define MDS_TRIGGER_IN_TIM8_CC4         27u
#define MDS_TRIGGER_IN_TIM8_CC5         28u
#define MDS_TRIGGER_IN_TIM1_OUT         29u
#define MDS_TRIGGER_IN_TIM2_OUT         30u
#define MDS_TRIGGER_IN_TIM3_OUT         31u
#define MDS_TRIGGER_IN_TIM4_OUT         32u
#define MDS_TRIGGER_IN_TIM5_OUT         33u
#define MDS_TRIGGER_IN_TIM8_OUT         36u
#define MDS_TRIGGER_IN_EXTI0            37u
#define MDS_TRIGGER_IN_EXTI1            38u
#define MDS_TRIGGER_IN_EXTI2            39u
#define MDS_TRIGGER_IN_EXTI3            40u
#define MDS_TRIGGER_IN_EXTI4            41u
#define MDS_TRIGGER_IN_EXTI5            42u
#define MDS_TRIGGER_IN_EXTI6            43u
#define MDS_TRIGGER_IN_EXTI7            44u
#define MDS_TRIGGER_IN_EXTI8            45u
#define MDS_TRIGGER_IN_EXTI9            46u
#define MDS_TRIGGER_IN_EXTI10           47u
#define MDS_TRIGGER_IN_EXTI11           48u
#define MDS_TRIGGER_IN_EXTI12           49u
#define MDS_TRIGGER_IN_EXTI13           50u
#define MDS_TRIGGER_IN_EXTI14           51u
#define MDS_TRIGGER_IN_EXTI15           52u
#define MDS_TRIGGER_IN_CLU0             (0u << 17u)
#define MDS_TRIGGER_IN_CLU1             (1u << 17u)
#define MDS_TRIGGER_IN_CLU2             (2u << 17u)
#define MDS_TRIGGER_IN_CLU3             (3u << 17u)

/* Trigger Out. */
#define MDS_TRIGGER_OUT_ADC1_EXT_TRIGGER          0u
#define MDS_TRIGGER_OUT_ADC1_INJECT_EXT_TRIGGER   1u
#define MDS_TRIGGER_OUT_ADC2_EXT_TRIGGER          2u
#define MDS_TRIGGER_OUT_ADC2_INJECT_EXT_TRIGGER   3u
#define MDS_TRIGGER_OUT_DAC1_EXT_TRIGGER          6u
#define MDS_TRIGGER_OUT_DAC2_EXT_TRIGGER          7u

/* Connection In. */
#define MDS_CONN_IN_LOGIC_0           0u
#define MDS_CONN_IN_LOGIC_1           1u
#define MDS_CONN_IN_TIM1_CH1_OUT      2u
#define MDS_CONN_IN_TIM1_CH1N_OUT     3u
#define MDS_CONN_IN_TIM1_CH2_OUT      4u
#define MDS_CONN_IN_TIM1_CH2N_OUT     5u
#define MDS_CONN_IN_TIM1_CH3_OUT      6u
#define MDS_CONN_IN_TIM1_CH3N_OUT     7u
#define MDS_CONN_IN_TIM1_CH4_OUT      8u
#define MDS_CONN_IN_TIM1_CH4N_OUT     9u
#define MDS_CONN_IN_TIM2_CH1_OUT      10u
#define MDS_CONN_IN_TIM2_CH2_OUT      11u
#define MDS_CONN_IN_TIM2_CH3_OUT      12u
#define MDS_CONN_IN_TIM2_CH4_OUT      13u
#define MDS_CONN_IN_TIM3_CH4_OUT      14u
#define MDS_CONN_IN_TIM4_CH4_OUT      15u
#define MDS_CONN_IN_TIM5_CH4_OUT      16u
#define MDS_CONN_IN_TIM8_CH4_OUT      17u
#define MDS_CONN_IN_MDS_IN0           18u
#define MDS_CONN_IN_MDS_IN1           19u
#define MDS_CONN_IN_MDS_IN2           20u
#define MDS_CONN_IN_MDS_IN3           21u
#define MDS_CONN_IN_CLU0              (0u << 17u)
#define MDS_CONN_IN_CLU1              (1u << 17u)
#define MDS_CONN_IN_CLU2              (2u << 17u)
#define MDS_CONN_IN_CLU3              (3u << 17u)

/*Connection Out. */
#define MDS_CONN_OUT_COMP1_BLANK   0u
#define MDS_CONN_OUT_COMP2_BLANK   1u
#define MDS_CONN_OUT_COMP3_BLANK   2u
#define MDS_CONN_OUT_MDS_OUT0     3u
#define MDS_CONN_OUT_MDS_OUT1     4u
#define MDS_CONN_OUT_MDS_OUT2     5u
#define MDS_CONN_OUT_MDS_OUT3     6u

#endif /*__HAL_MDS_REMAP_H__. */