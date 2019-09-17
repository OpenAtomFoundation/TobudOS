#ifndef __MQTT_DEMO_H__
#define __MQTT_DEMO_H__

#include "mcu_init.h"
#include "tos_at.h"
#include "string.h"
#include "cmsis_os.h"
#include "sal_module_wrapper.h"

#include "mqtt_wrapper.h"

extern void application_entry(void *arg);
extern void motor_control(char mode);
extern void light_control(char mode);
#endif /* __APP_DEMO_H__ */

