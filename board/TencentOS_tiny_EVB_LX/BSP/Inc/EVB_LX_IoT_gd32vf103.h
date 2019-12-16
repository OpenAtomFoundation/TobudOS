#ifndef EVB_LX_IOT_GD32VF103_H
#define EVB_LX_IOT_GD32VF103_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "gd32vf103.h"
     
/* exported types */
typedef enum 
{
    LED = 0,

} led_typedef_enum;

typedef enum 
{
    F1_KEY = 0,
    F2_KEY = 1,
} key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
} keymode_typedef_enum;

/* eval board low layer led */
#define LEDn                             4U

#define LED_PIN                         GPIO_PIN_2
#define LED_GPIO_PORT                   GPIOB
#define LED_GPIO_CLK                    RCU_GPIOB
/*****************E53***********************************/
#define SPI_SCK_PIN                     GPIO_PIN_13
#define SPI_GPIO_PORT                   GPIOB
#define SPI_SCK_CLK                     RCU_GPIOB

#define SPI_NSS_PIN                         GPIO_PIN_12
#define SPI_NSS_PORT                   GPIOB
#define SPI_NSS_CLK                    RCU_GPIOB

#define GPIO1_PIN                         GPIO_PIN_0
#define GPIO1_PORT                   GPIOA
#define GPIO1_CLK                    RCU_GPIOA

#define ADC_PIN                         GPIO_PIN_1
#define ADC_PORT                   GPIOA
#define ADC_CLK                    RCU_GPIOA

#define DAC_PIN                         GPIO_PIN_4
#define DAC_PORT                   GPIOA
#define DAC_CLK                    RCU_GPIOA

#define GPIO2_PIN                         GPIO_PIN_9
#define GPIO2_PORT                   GPIOB
#define GPIO2_CLK                    RCU_GPIOB

#define GPIO3_PIN                         GPIO_PIN_8
#define GPIO3_PORT                   GPIOB
#define GPIO3_CLK                    RCU_GPIOB

#define IIC_SCL_PIN                         GPIO_PIN_6
#define IIC_SCL_PORT                   GPIOB
#define IIC_SCL_CLK                    RCU_GPIOB

#define IIC_SDA_PIN                         GPIO_PIN_7
#define IIC_SDA_PORT                   GPIOB
#define IIC_SDA_CLK                    RCU_GPIOB

#define GPIO4_PIN                         GPIO_PIN_8
#define GPIO4_PORT                   GPIOA
#define GPIO4_CLK                    RCU_GPIOA

#define UART_RX_PIN                         GPIO_PIN_11
#define UART_RX_PORT                   GPIOC
#define UART_RX_CLK                    RCU_GPIOC

#define UART_TX_PIN                         GPIO_PIN_10
#define UART_TX_PORT                   GPIOC
#define UART_TX_CLK                    RCU_GPIOC

#define GPIO5_PIN                         GPIO_PIN_9
#define GPIO5_PORT                   GPIOC
#define GPIO5_CLK                    RCU_GPIOC

#define SPI_MOSI_PIN                         GPIO_PIN_15
#define SPI_MOSI_PORT                   GPIOB
#define SPI_MOSI_CLK                    RCU_GPIOB

#define SPI_MISO_PIN                         GPIO_PIN_14
#define SPI_MISO_PORT                   GPIOB
#define SPI_MISO_CLK                    RCU_GPIOB
/*******************************************************/

/*************************OLED**************************/
#define IIC2_SCL_PIN				GPIO_PIN_10
#define IIC2_SCL_PORT				GPIOB

#define IIC2_SDA_PIN				GPIO_PIN_11
#define IIC2_SDA_PORT				GPIOB
/********************************************************/

#define COMn                             2U

#define EVAL_COM0                        USART2
#define EVAL_COM0_CLK                    RCU_USART2
#define EVAL_COM0_TX_PIN                 GPIO_PIN_2
#define EVAL_COM0_RX_PIN                 GPIO_PIN_3
#define EVAL_COM0_GPIO_PORT              GPIOA
#define EVAL_COM0_GPIO_CLK               RCU_GPIOA

#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCU_USART1
#define EVAL_COM1_TX_PIN                 GPIO_PIN_2
#define EVAL_COM1_RX_PIN                 GPIO_PIN_3
#define EVAL_COM1_GPIO_PORT              GPIOA
#define EVAL_COM1_GPIO_CLK               RCU_GPIOA

#define EVAL_COM3                        UART3
#define EVAL_COM3_CLK                    RCU_UART3
#define EVAL_COM3_TX_PIN                 GPIO_PIN_10
#define EVAL_COM3_RX_PIN                 GPIO_PIN_11
#define EVAL_COM3_GPIO_PORT              GPIOC
#define EVAL_COM3_GPIO_CLK               RCU_GPIOC

#define KEYn                             3U

/* F1 push-button */
#define F1_KEY_PIN                   GPIO_PIN_1
#define F1_KEY_GPIO_PORT             GPIOA
#define F1_KEY_GPIO_CLK              RCU_GPIOA
#define F1_KEY_EXTI_LINE             EXTI_1
#define F1_KEY_EXTI_PORT_SOURCE      GPIO_PORT_SOURCE_GPIOA
#define F1_KEY_EXTI_PIN_SOURCE       GPIO_PIN_SOURCE_1
#define F1_KEY_EXTI_IRQn             EXTI1_IRQn

/* F2 push-button */
#define F2_KEY_PIN                   GPIO_PIN_0
#define F2_KEY_GPIO_PORT             GPIOA
#define F2_KEY_GPIO_CLK              RCU_GPIOA
#define F2_KEY_EXTI_LINE             EXTI_0
#define F2_KEY_EXTI_PORT_SOURCE      GPIO_PORT_SOURCE_GPIOA
#define F2_KEY_EXTI_PIN_SOURCE       GPIO_PIN_SOURCE_0
#define F2_KEY_EXTI_IRQn             EXTI0_IRQn  





/* function declarations */
/* configure led GPIO */
void gd_eval_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_eval_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_eval_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_eval_led_toggle(led_typedef_enum lednum);
/* configure key */
void gd_eval_key_init(key_typedef_enum key_num, keymode_typedef_enum key_mode);
/* return the selected key state */
uint8_t gd_eval_key_state_get(key_typedef_enum key);
/* configure COM port */
void gd_eval_com_init(uint32_t com);

#ifdef __cplusplus
}
#endif

#endif /* BEARPI_IOT_GD32F303_H */

