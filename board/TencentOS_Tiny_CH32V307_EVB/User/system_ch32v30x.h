/********************************** (C) COPYRIGHT *******************************
* File Name          : system_ch32v30x.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : CH32V30x Device Peripheral Access Layer System Header File.
*******************************************************************************/
#ifndef __SYSTEM_CH32V30x_H 
#define __SYSTEM_CH32V30x_H

#ifdef __cplusplus
 extern "C" {
#endif 

extern uint32_t SystemCoreClock;          /* System Clock Frequency (Core Clock) */

/* System_Exported_Functions */  
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /*__CH32V30x_SYSTEM_H */



