#ifndef __SENSOR_DEMO_H__
#define __SENSOR_DEMO_H__

#include "mcu_init.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct
{
    char data[32];
}ReportData_TypeDef;

void application_entry(void *arg);
#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_DEMO_H__ */

