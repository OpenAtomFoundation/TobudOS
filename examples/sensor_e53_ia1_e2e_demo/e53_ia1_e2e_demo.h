#ifndef _E53_IA1_E2E_DEMO_H_
#define _E53_IA1_E2E_DEMO_H_

#include "mcu_init.h"
#include "tos_at.h"
#include "string.h"
#include "cmsis_os.h"
#include "sal_module_wrapper.h"

#include "mqtt_wrapper.h"

extern void application_entry(void *arg);
extern void motor_control(char mode);
extern void light_control(char mode);

#endif /* _E53_IA1_E2E_DEMO_H_ */
