/**
  ******************************************************************************
  * @file   app_conf.h
  * @author MCD Application Team
  * @brief  applic configuration, e.g. : debug, trace, low power, sensors
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_CONF_H__
#define __APP_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 * lw APP configuration
 ******************************************************************************/

/* lw application configuration (Mw is configured by lorawan_conf.h) */
#define LORAWAN_DEFAULT_ACTIVE_REGION        		LORAMAC_REGION_CN470
 
 
	
/*enable Low power*/
#define LOW_POWER_ENBLE  0

/* LPM_STOP_MODE_DISABLE = 0 : LowPowerMode enabled : MCU enters stop2 mode*/
/* LPM_STOP_MODE_DISABLE = 1 : LowPowerMode disabled : MCU enters sleep mode only */
#define LPM_STOP_MODE_DISABLE  0
	
	
/******************************************************************************
 * classB configuration
 ******************************************************************************/
/*using class b must define LORAMAC_CLASSB_ENABLED and active ClassB*/
/* Class B ------------------------------------*/
//#define LORAMAC_CLASSB_ENABLED
#define LORAMAC_CLASSB_ACTIVE 0 /* 0 if not active, 1 if active */


/******************************************************************************
 * DEBUGGING  configuration
 ******************************************************************************/

/* Enable trace logs. Note verbose level can be selected in app_system.h */
#define APP_LOG_ENABLED  1
#define MW_LOG_ENABLED   1
/*
0 only print the informtion
1.print the function and line
2 print the file , line ,function
*/
#define LOG_LEVEL     0



/******************************************************************************
 * Sequencer
 ******************************************************************************/
#define CFG_SEQ_Prio_0 0

/**
* This is the list of task id required by the application
* Each Id shall be in the range 0..1<31
*/
#define CFG_SEQ_TASK_LORAMAC_PROCESS       (1 << 1)
#define CFG_SEQ_TASK_LORA_SEND_ON_TX_TIMER (1 << 2)
#define CFG_SEQ_TASK_LORA_CERTIF_PROCESS   (1 << 3)

/******************************************************************************
 * LOW POWER MANAGER
 ******************************************************************************/
/**
 * Supported requester to the MCU Low Power Manager - can be increased up  to 32
 * It lits a bit mapping of all user of the Low Power Manager
 */
typedef enum
{
  LPM_APPLI_Id = (1 << 0),
  LPM_LIB_Id = (1 << 1),
  LPM_RTC_Id = (1 << 2),
  LPM_GPS_Id = (1 << 3),
  LPM_UART_RX_Id = (1 << 4),
  LPM_UART_TX_Id = (1 << 5),
  LPM_TCXO_WA_Id = (1 << 6),
} LPM_Id_t;

#ifdef __cplusplus
}
#endif

#endif /* __APP_CONF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
