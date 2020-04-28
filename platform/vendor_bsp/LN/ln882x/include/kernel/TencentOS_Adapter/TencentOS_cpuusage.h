#ifndef _FREERTOS_CPUUSAGE_H__
#define _FREERTOS_CPUUSAGE_H__

#include "./TencentOS_Adapter/tos_common.h"

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
