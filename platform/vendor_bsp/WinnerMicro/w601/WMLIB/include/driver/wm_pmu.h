/**
 * @file    wm_pmu.h
 *
 * @brief   pmu driver module
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_PMU_H
#define WM_PMU_H
#ifdef __cplusplus
extern "C" {
#endif

#include "wm_type_def.h"

typedef void (*tls_pmu_irq_callback)(void *arg);

/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup PMU_Driver_APIs PMU Driver APIs
 * @brief PMU driver APIs
 */

/**
 * @addtogroup PMU_Driver_APIs
 * @{
 */

/**
 * @brief          	This function is used to register pmu timer1 interrupt
 *
 * @param[in]      	callback    	the pmu timer1 interrupt call back function
 * @param[in]      	arg         		parameter of call back function
 *
 * @return         	None
 *
 * @note
 * user not need clear interrupt flag.
 * pmu timer1 callback function is called in interrupt,
 * so can not operate the critical data in the callback fuuction,
 * recommendation to send messages to other tasks to operate it.
 */
void tls_pmu_timer1_isr_register(tls_pmu_irq_callback callback, void *arg);


/**
 * @brief          	This function is used to register pmu timer0 interrupt
 *
 * @param[in]      	callback    	the pmu timer0 interrupt call back function
 * @param[in]      	arg         		parameter of call back function
 *
 * @return         	None
 *
 * @note
 * user not need clear interrupt flag.
 * pmu timer0 callback function is called in interrupt,
 * so can not operate the critical data in the callback fuuction,
 * recommendation to send messages to other tasks to operate it.
 */
void tls_pmu_timer0_isr_register(tls_pmu_irq_callback callback, void *arg);


/**
 * @brief          	This function is used to register pmu gpio interrupt
 *
 * @param[in]      	callback    	the pmu gpio interrupt call back function
 * @param[in]      	arg         		parameter of call back function
 *
 * @return         	None
 *
 * @note
 * user not need clear interrupt flag.
 * pmu gpio callback function is called in interrupt,
 * so can not operate the critical data in the callback fuuction,
 * recommendation to send messages to other tasks to operate it.
 */
void tls_pmu_gpio_isr_register(tls_pmu_irq_callback callback, void *arg);


/**
 * @brief          	This function is used to register pmu sdio interrupt
 *
 * @param[in]      	callback    	the pmu sdio interrupt call back function
 * @param[in]      	arg         		parameter of call back function
 *
 * @return         	None
 *
 * @note
 * user not need clear interrupt flag.
 * pmu sdio callback function is called in interrupt,
 * so can not operate the critical data in the callback fuuction,
 * recommendation to send messages to other tasks to operate it.
 */
void tls_pmu_sdio_isr_register(tls_pmu_irq_callback callback, void *arg);


/**
 * @brief          	This function is used to select pmu clk
 *
 * @param[in]      	bypass    pmu clk whether or not use bypass mode
 *				1   pmu clk use 32K by 40MHZ
 *				other pmu clk 32K by calibration circuit
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_pmu_clk_select(u8 bypass);


/**
 * @brief          	This function is used to start pmu timer0
 *
 * @param[in]      	second  	vlaue of timer0 count[s]
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_pmu_timer0_start(u16 second);


/**
 * @brief          	This function is used to stop pmu timer0
 *
 * @param		None
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_pmu_timer0_stop(void);



/**
 * @brief          	This function is used to start pmu timer1
 *
 * @param[in]      	second  	vlaue of timer1 count[ms]
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_pmu_timer1_start(u16 msec);


/**
 * @brief          	This function is used to stop pmu timer1
 *
 * @param		None
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_pmu_timer1_stop(void);



/**
 * @brief          	This function is used to start pmu goto standby 
 *
 * @param		None
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_pmu_standby_start(void);


typedef enum {
    TLS_PERIPHERAL_TYPE_I2C   = (1 << 0),
    TLS_PERIPHERAL_TYPE_UART0 = (1 << 1),
    TLS_PERIPHERAL_TYPE_UART1 = (1 << 2),
    TLS_PERIPHERAL_TYPE_LSPI  = (1 << 3),
    TLS_PERIPHERAL_TYPE_DMA   = (1 << 4),
    TLS_PERIPHERAL_TYPE_RFCFG = (1 << 5),
    TLS_PERIPHERAL_TYPE_TIMER = (1 << 6),
    TLS_PERIPHERAL_TYPE_GPIO  = (1 << 7),
    TLS_PERIPHERAL_TYPE_SDADC = (1 << 8),
    TLS_PERIPHERAL_TYPE_PWM   = (1 << 9),
    TLS_PERIPHERAL_TYPE_LCD   = (1 << 10),
    TLS_PERIPHERAL_TYPE_I2S   = (1 << 11),
    TLS_PERIPHERAL_TYPE_RSA   = (1 << 12),
    TLS_PERIPHERAL_TYPE_GPSEC = (1 << 13),
    TLS_PERIPHERAL_TYPE_7816  = (1 << 14)
}tls_peripheral_type_s;

/**
 * @brief          	This function is used to close peripheral's clock
 *
 * @param[in]      	devices  	peripherals
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_close_peripheral_clock(tls_peripheral_type_s devices);

/**
 * @brief          	This function is used to open peripheral's clock
 *
 * @param[in]      	devices  	peripherals
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_open_peripheral_clock(tls_peripheral_type_s devices);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* end of WM_PMU_H */

/*** (C) COPYRIGHT 2014 Winner Microelectronics Co., Ltd. ***/


