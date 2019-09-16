/**
  ******************************************************************************
  * @file    chlib_k.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   include all header files
  ******************************************************************************
  */
#ifndef __CH_LIB_K_H__
#define __CH_LIB_K_H__

#ifdef __cplusplus
 extern "C" {
#endif
     
/* libray driver */
#include "common.h"
#include "gpio.h"
#include "adc.h"
#include "can.h"
#include "uart.h"
#include "systick.h"
#include "pit.h"
#include "dac.h"
#include "i2c.h"
#include "spi.h"
#include "rtc.h"
#include "cpuidy.h"
#include "dma.h"
#include "enet.h"
#include "ftm.h"
#include "lptmr.h"
#include "flexbus.h"
#include "sd.h"
#include "tsi.h"
#include "vref.h"
#include "crc.h"
#include "wdog.h"

/* T90FS Flash */
#include "flash.h"

/* DSP lib */
#define ARM_MATH_CM4

#include "arm_const_structs.h"
#include "arm_math.h"
#include "arm_common_tables.h"


#ifdef __cplusplus
}
#endif

#endif

