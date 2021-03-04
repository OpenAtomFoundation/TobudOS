#ifndef _TENCENTOS_CPUUSAGE_H__
#define _TENCENTOS_CPUUSAGE_H__

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

#endif /* _TENCENTOS_CPUUSAGE_H__ */
