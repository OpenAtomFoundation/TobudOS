/**
  ******************************************************************************
  * @file   app_conf_template.h
  * @author MCD Application Team
  * @brief  applic configuration, e.g. : debug, trace, low power, sensors
  * @note   this is a template: the app_conf.h shall be defined in the Project/...
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
 ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_CONF_H__
#define __APP_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* LoraWAN application configuration (Mw is configured by lorawan_conf.h) */
#define ACTIVE_REGION        LORAMAC_REGION_EU868
#define HYBRID_ENABLED       0

#define LORAMAC_CLASSB_ACTIVE 0  /* 0 if not active, 1 if active */

/* Enable trace logs. Note verbose level can be selected in app_system.h */
#define APP_LOG_ENABLED  1

#define DEBUGGER_ON  0   /* if ON (=1) it enables the debbugger plus 4 dgb pins */
                         /* if OFF (=0) the debugger is OFF (lower consumption) */

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __APP_CONF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
