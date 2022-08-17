/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_DMA_REQUESET_H__
#define __HAL_DMA_REQUESET_H__

/* DMA1 Requests Remap. */
/* ADC1. */
#define DMA_REQ_DMA1_ADC1        0u

/* ADC2. */
#define DMA_REQ_DMA1_ADC2        1u

/* SPI1. */
#define DMA_REQ_DMA1_SPI1_RX     1u
#define DMA_REQ_DMA1_SPI1_TX     2u

/* SPI2. */
#define DMA_REQ_DMA1_SPI2_RX     3u
#define DMA_REQ_DMA1_SPI2_TX     4u

/* UART1. */
#define DMA_REQ_DMA1_UART1_TX    3u
#define DMA_REQ_DMA1_UART1_RX    4u

/* UART2. */
#define DMA_REQ_DMA1_UART2_TX    5u
#define DMA_REQ_DMA1_UART2_RX    6u

/* UART3. */
#define DMA_REQ_DMA1_UART3_TX    1u
#define DMA_REQ_DMA1_UART3_RX    2u

/* UART4. */

/* UART5. */

/* UART6. */
#define DMA_REQ_DMA1_UART6_RX    0u
#define DMA_REQ_DMA1_UART6_TX    7u

/* UART7. */

/* I2C1. */
#define DMA_REQ_DMA1_I2C1_TX     5u
#define DMA_REQ_DMA1_I2C1_RX     6u

/* I2C2. */
#define DMA_REQ_DMA1_I2C2_TX     3u
#define DMA_REQ_DMA1_I2C2_RX     4u

/* TIM1. */
#define DMA_REQ_DMA1_TIM1_CC1    1u
#define DMA_REQ_DMA1_TIM1_CC2    2u
#define DMA_REQ_DMA1_TIM1_CC4    3u
#define DMA_REQ_DMA1_TIM1_UP     4u
#define DMA_REQ_DMA1_TIM1_CC3    5u
#define DMA_REQ_DMA1_TIM1_TRIG_2 6u
#define DMA_REQ_DMA1_TIM1_COM_2  7u

#define DMA_REQ_DMA1_TIM1_TRIG_1 3u

#define DMA_REQ_DMA1_TIM1_COM_1  3u

/* TIM2. */
#define DMA_REQ_DMA1_TIM2_CC3_1  0u
#define DMA_REQ_DMA1_TIM2_UP_1   1u
#define DMA_REQ_DMA1_TIM2_UP_2   2u
#define DMA_REQ_DMA1_TIM2_CC1    4u
#define DMA_REQ_DMA1_TIM2_CC2    6u
#define DMA_REQ_DMA1_TIM2_UP_3   7u

#define DMA_REQ_DMA1_TIM2_CC3_2  2u
#define DMA_REQ_DMA1_TIM2_CC4_1  6u
#define DMA_REQ_DMA1_TIM2_CC4_2  7u

/* TIM3. */
#define DMA_REQ_DMA1_TIM3_CC3    1u
#define DMA_REQ_DMA1_TIM3_CC4    2u
#define DMA_REQ_DMA1_TIM3_CC1    5u
#define DMA_REQ_DMA1_TIM3_CC2    7u

#define DMA_REQ_DMA1_TIM3_UP     2u
#define DMA_REQ_DMA1_TIM3_TRIG   5u

/* TIM4. */
#define DMA_REQ_DMA1_TIM4_CC1    0u
#define DMA_REQ_DMA1_TIM4_CC2    3u
#define DMA_REQ_DMA1_TIM4_CC3    4u
#define DMA_REQ_DMA1_TIM4_UP     6u
#define DMA_REQ_DMA1_TIM4_CC4    7u

/* TIM5. */

/* TIM6. */

/* TIM7. */

/* TIM8. */

/* FELXCAN1. */
#define DMA_REQ_DMA1_FLEXCAN1_RX 7u

/* QSPI. */

/* DMA2 Requests Remap. */
/* ADC. */

/* DAC. */
#define DMA_REQ_DMA2_DAC_CH1_1   2u
#define DMA_REQ_DMA2_DAC_CH2_1   3u
#define DMA_REQ_DMA2_DAC_CH1_2   6u

#define DMA_REQ_DMA2_DAC_CH2_2   6u

/* SPI3. */
#define DMA_REQ_DMA2_SPI3_RX     0u
#define DMA_REQ_DMA2_SPI3_TX     1u

/* UART4. */
#define DMA_REQ_DMA2_UART4_RX_1  2u
#define DMA_REQ_DMA2_UART4_TX_1  4u
#define DMA_REQ_DMA2_UART4_RX_2  6u
#define DMA_REQ_DMA2_UART4_TX_2  7u

/* UART5. */
#define DMA_REQ_DMA2_UART5_RX    0u
#define DMA_REQ_DMA2_UART5_TX    1u

/* UART6. */
#define DMA_REQ_DMA2_UART6_TX    3u
#define DMA_REQ_DMA2_UART6_RX    5u

/* UART7. */
#define DMA_REQ_DMA2_UART7_RX_1    0u
#define DMA_REQ_DMA2_UART7_TX_1    1u
#define DMA_REQ_DMA2_UART7_RX_2    6u
#define DMA_REQ_DMA2_UART7_TX_2    7u

/* LPUART. */
#define DMA_REQ_DMA2_LPUART_TX   3u
#define DMA_REQ_DMA2_LPUART_RX   5u

/* I2C. */

/* TIM1. */

/* TIM2. */

/* TIM3. */

/* TIM4. */

/* TIM5. */
#define DMA_REQ_DMA2_TIM5_CC4    0u
#define DMA_REQ_DMA2_TIM5_CC3    1u
#define DMA_REQ_DMA2_TIM5_CC2    3u
#define DMA_REQ_DMA2_TIM5_CC1    4u
#define DMA_REQ_DMA2_TIM5_UP_2   5u

#define DMA_REQ_DMA2_TIM5_TRIG   0u
#define DMA_REQ_DMA2_TIM5_UP_1   1u

/* TIM6. */
#define DMA_REQ_DMA2_TIM6_UP_1   2u
#define DMA_REQ_DMA2_TIM6_UP_2   5u

/* TIM7. */
#define DMA_REQ_DMA2_TIM7_UP_1   3u
#define DMA_REQ_DMA2_TIM7_UP_2   5u

/* TIM8. */
#define DMA_REQ_DMA2_TIM8_CC3    0u
#define DMA_REQ_DMA2_TIM8_CC4    1u
#define DMA_REQ_DMA2_TIM8_CC1    2u
#define DMA_REQ_DMA2_TIM8_CC2    4u
#define DMA_REQ_DMA2_TIM8_UP_2   5u
#define DMA_REQ_DMA2_TIM8_TRIG_2 6u
#define DMA_REQ_DMA2_TIM8_COM_2  7u

#define DMA_REQ_DMA2_TIM8_UP_1   0u
#define DMA_REQ_DMA2_TIM8_TRIG_1 1u

#define DMA_REQ_DMA2_TIM8_COM_1  1u

/* FELXCAN2. */
#define DMA_REQ_DMA2_FLEXCAN2_RX 5u

/* QSPI. */
#define DMA_REQ_DMA2_QSPI        7u

#endif /* __HAL_DMA_REQUESET_H__ */
