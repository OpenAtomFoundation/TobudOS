#ifndef __BSP_ETH_H__
#define __BSP_ETH_H__

#include "stm32f4xx_hal.h"


/**ETH GPIO Configuration    
PC1     ------> ETH_MDC
PA1     ------> ETH_REF_CLK
PA2     ------> ETH_MDIO
PA7     ------> ETH_CRS_DV
PC4     ------> ETH_RXD0
PC5     ------> ETH_RXD1
PB11     ------> ETH_TX_EN
PG13     ------> ETH_TXD0
PG14     ------> ETH_TXD1 
*/

/* Private defines --------------------------------------*/
#define ETH_MDC_Pin                 GPIO_PIN_1
#define ETH_MDC_GPIO_Port           GPIOC
#define ETH_REF_CLK_Pin             GPIO_PIN_1
#define ETH_REF_CLK_GPIO_Port       GPIOA
#define ETH_MDIO_Pin                GPIO_PIN_2
#define ETH_MDIO_GPIO_Port          GPIOA
#define ETH_CRS_DV_Pin              GPIO_PIN_7
#define ETH_CRS_DV_GPIO_Port        GPIOA
#define ETH_RXD0_Pin                GPIO_PIN_4
#define ETH_RXD0_GPIO_Port          GPIOC
#define ETH_RXD1_Pin                GPIO_PIN_5
#define ETH_RXD1_GPIO_Port          GPIOC
#define ETH_TX_EN_Pin               GPIO_PIN_11
#define ETH_TX_EN_GPIO_Port         GPIOB
#define USART1_TX_Pin               GPIO_PIN_9
#define USART1_TX_GPIO_Port         GPIOA
#define USART1_TXA10_Pin            GPIO_PIN_10
#define USART1_TXA10_GPIO_Port      GPIOA
#define ETH_TXD0_Pin                GPIO_PIN_13
#define ETH_TXD0_GPIO_Port          GPIOG
#define ETH_TXD1_Pin                GPIO_PIN_14
#define ETH_TXD1_GPIO_Port          GPIOG




HAL_StatusTypeDef Bsp_Eth_Init(void);


#endif
