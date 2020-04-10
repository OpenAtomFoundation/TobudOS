#ifndef __SYSTEM_LN88XX_H__
#define __SYSTEM_LN88XX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
extern uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock) */


/**
  \brief Setup the microcontroller system.

   Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);
extern void SetSysClock(void);


/**
  \brief  Update SystemCoreClock variable.

   Updates the SystemCoreClock with current core Clock retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);
extern uint32_t get_SystemCoreClock(void);
#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_LN88XX_H__ */
