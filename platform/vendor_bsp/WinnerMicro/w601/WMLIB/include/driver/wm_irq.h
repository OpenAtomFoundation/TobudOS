/**
 * @file    wm_irq.h
 *
 * @brief   interupt driver module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */

#ifndef WM_IRQ_H
#define WM_IRQ_H

#include "wm_type_def.h"

/* Number of interrupts. */

#define INTR_CNT 41

#define SDIO_UP_DATA_INT            (0UL)
#define SDIO_DOWN_DATA_INT          (1UL)
#define SDIO_UP_CMD_INT             (2UL)
#define SDIO_DOWN_CMD_INT           (3UL)
#define MAC_INT                     (4UL)

#define RX_INT                      (6UL)
#define TX_MGMT_COMPLETE_INT        (7UL)
#define TX_DATA_COMPLETE_INT        (8UL)
#define PMU_TIMER1_INT              (9UL)
#define PMU_TIMER0_INT   			(10UL)
#define PMU_GPIO_WAKEUP_INT         (11UL)
#define PMU_SDIO_WAKEUP_INT         (12UL)
#define DMA0_INT                    (13UL)
#define DMA1_INT                    (14UL)
#define DMA2_INT                    (15UL)
#define DMA3_INT                    (16UL)
#define DMA4_INT                    (17UL)
#define DMA_BURST_INT               (18UL)
#define I2C_INT                     (19UL)
#define ADC_INT                     (20UL)
#define SPI0_INT                    (21UL)
#define SPI1_INT                    (22UL)
#define UART0_INT                   (23UL)
#define UART1_INT                   (24UL)
#define GPIO_INT                    (25UL)
#define TIMER0_INT                  (26UL)
#define TIMER1_INT                  (27UL)
#define TIMER2_INT                  (28UL)
#define   TIMER3_INT                (29UL)
#define   TIMER4_INT                (30UL)
#define   TIMER5_INT                (31UL)
#define   WATCHDOG_INT              (32UL)
#define   PMU_BOD_INT              	(33UL)
#define   FLASH_INT                  (34UL)
#define   PWM_INT                 	  (35UL)
#define   I2S_INT              	  (36UL)
#define	  PMU_RTC_INT				(37UL)
#define	  RSA_INT					(38UL)
#define   CRYPTION_INT              (39UL)
#define   GPIOB_INT                 (40UL)
#define   UART2_INT					(41UL)	

typedef void (*intr_handler_func) (void *);

/**
 * @typedef struct tls_irq_handler
 */
typedef struct tls_irq_handler
{
    void (*handler) (void *);
    void *data;
//  const char  *name;
    u32 counter;
} tls_irq_handler_t;

/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup IRQ_Driver_APIs IRQ Driver APIs
 * @brief IRQ driver APIs
 */

/**
 * @addtogroup IRQ_Driver_APIs
 * @{
 */

/**
 * @brief          This function is used to initial system interrupt.
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void tls_irq_init(void);

/**
 * @brief          This function is used to register interrupt handler function.
 *
 * @param[in]      vec_no           interrupt NO
 * @param[in]      handler
 * @param[in]      *data
 *
 * @return         None
 *
 * @note           None
 */
void tls_irq_register_handler(u8 vec_no, intr_handler_func handler, void *data);


/**
 * @brief          This function is used to enable interrupt.
 *
 * @param[in]      vec_no       interrupt NO
 *
 * @return         None
 *
 * @note           None
 */
void tls_irq_enable(u8 vec_no);

/**
 * @brief          This function is used to disable interrupt.
 *
 * @param[in]      vec_no       interrupt NO
 *
 * @return         None
 *
 * @note           None
 */
void tls_irq_disable(u8 vec_no);


/**
 * @brief          This function is used to get the isr count.
 *
 * @param[in]      None
 *
 * @retval         count
 *
 * @note           None
 */
u8 tls_get_isr_count(void);

/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_IRQ_H */
