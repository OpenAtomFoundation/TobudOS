#ifndef _FREERTOS_CPUUSAGE_H__
#define _FREERTOS_CPUUSAGE_H__

#include "./FreeRTOS_Adapter/freertos_common.h"

#ifdef __cplusplus
 extern "C" {
#endif


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define CALCULATION_PERIOD    1000

/* Exported functions ------------------------------------------------------- */
uint16_t osGetCPUUsage (void);

#ifdef __cplusplus
}
#endif

#endif /* _FREERTOS_CPUUSAGE_H__ */
