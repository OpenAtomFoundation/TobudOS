#ifndef __MCU_INIT_H__
#define __MCU_INIT_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "board.h"
#include "usart.h"
#include "pin_mux.h"
#include "clock_config.h"

/* Defines ------------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Typedefs -----------------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Functions API ------------------------------------------------------------*/

void SystemClock_Config(void);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SYS_INIT_H__ */
