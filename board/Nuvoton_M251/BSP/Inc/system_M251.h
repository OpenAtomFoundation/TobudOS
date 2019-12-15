/**************************************************************************//**
* @file     system_M251.h
* @version  V0.10
* @brief    System Setting Header File
*
* @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
****************************************************************************/

#ifndef __SYSTEM_M251_H__
#define __SYSTEM_M251_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define __HXT       (12000000UL)              /*!< External Crystal Clock Frequency */
#define __LXT       (32768UL)                 /*!< External Crystal Clock Frequency 32.768KHz */
#define __LIRC      (38400UL)                 /*!< Internal 38.4KHz RC Oscillator Frequency */
#define __MIRC      (4032000UL)               /*!< Internal 4.032M RC Oscillator Frequency */
#define __HIRC      (48000000UL)              /*!< Internal 48M RC Oscillator Frequency */
#define __HSI       (48000000UL)              /*!< PLL default output is 48MHz */
#define __SYS_OSC_CLK     (    ___HSI)        /*!< Main oscillator frequency */

#define __SYSTEM_CLOCK    (1UL*__HXT)

extern uint32_t SystemCoreClock;    /*!< System Clock Frequency (Core Clock)  */
extern uint32_t CyclesPerUs;        /*!< Cycles per micro second              */
extern uint32_t PllClock;           /*!< PLL Output Clock Frequency           */


/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the micro controller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit(void);

/**
 * Update SystemCoreClock variable
 *
 * @param  none
 * @return none
 *
 * @brief  Updates the SystemCoreClock with current core Clock
 *         retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate(void);

/**
 * Set UART0 default multi function pin
 *
 * @param  none
 * @return none
 *
 * @brief  The initialization of uart0 default multiple-function pin.
 */
extern void Uart0DefaultMPF(void);


#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_M251_H__ */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
