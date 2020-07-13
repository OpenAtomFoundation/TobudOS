#ifndef __LL_H__
#define __LL_H__

#include <stdint.h>
#include "ln88xx.h"

/** @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 */

#define GLOBAL_INT_START()  __enable_irq()

/** @brief Disable interrupts globally in the system.
 * This macro must be used when the system wants to disable all the interrupt
 * it could handle.
 */

#define GLOBAL_INT_STOP()  __disable_irq()


/** @brief Disable interrupts globally in the system.
 * This macro must be used in conjunction with the @ref GLOBAL_INT_RESTORE macro since this
 * last one will close the brace that the current macro opens.  This means that both
 * macros must be located at the same scope level.
 */

#define GLOBAL_INT_DISABLE()					\
	do{											\
		uint32_t __irq_mask = __get_PRIMASK();	\
		__disable_irq();



/** @brief Restore interrupts from the previous global disable.
 * @sa GLOBAL_INT_DISABLE
 */

#define GLOBAL_INT_RESTORE()					\
		if(__irq_mask==0)	__enable_irq();		\
	}while(0);

/** @brief Invoke the wait for interrupt procedure of the processor.
 *
 * @warning It is suggested that this macro is called while the interrupts are disabled
 * to have performed the checks necessary to decide to move to sleep mode.
 *
 */
#define WFI()    			__WFI()
#endif // __LL_H__
