/**
 * @file    wm_config.h
 *
 * @brief   w600 chip inferface configure
 *
 * @author  dave
 *
 * @copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef __WM_CONFIG_H__
#define __WM_CONFIG_H__

#define	CFG_ON											1
#define CFG_OFF											0

#define WM_CONFIG_DEBUG_UART1							CFG_OFF   /*PRINTF PORT USE UART1*/

/**Driver Support**/
#define TLS_CONFIG_HS_SPI          						CFG_ON /*High Speed SPI*/
#define TLS_CONFIG_LS_SPI          						CFG_ON /*Low Speed SPI*/
#define TLS_CONFIG_UART									CFG_ON  /*UART*/

/**Host Interface&Command**/
#define TLS_CONFIG_HOSTIF 								CFG_ON
#define TLS_CONFIG_AT_CMD								(CFG_ON && TLS_CONFIG_HOSTIF)
#define TLS_CONFIG_RI_CMD								(CFG_ON && TLS_CONFIG_HOSTIF)
#define TLS_CONFIG_RMMS									CFG_OFF

//LWIP CONFIG
#define TLS_CONFIG_LWIP_VER2_0_3        				CFG_ON 
#define TLS_CONFIG_IPV4                 				CFG_ON      //must ON
#define TLS_CONFIG_IPV6                 				CFG_OFF
#define TLS_CONFIG_DHCP_OPTION60						"Winnermicro:W600_01"

/** SOCKET CONFIG **/
#define TLS_CONFIG_SOCKET_STD							CFG_ON
#define TLS_CONFIG_SOCKET_RAW							CFG_ON
#define TLS_CONFIG_CMD_USE_RAW_SOCKET                   (CFG_ON && TLS_CONFIG_SOCKET_RAW)



#define	TLS_CONFIG_HARD_CRYPTO							CFG_ON

#define TLS_CONFIG_USE_POLARSSL           				CFG_OFF
#define TLS_CONFIG_SERVER_SIDE_SSL                      (CFG_OFF && TLS_CONFIG_HTTP_CLIENT_SECURE)         /*MUST configure TLS_CONFIG_HTTP_CLIENT_SECURE CFG_ON */


/** HTTP CLIENT **/
/*
HTTP Lib
HTTPS Lib
SSL LIB
CRYPTO
*/
#define TLS_CONFIG_HTTP_CLIENT							(CFG_ON)
#define TLS_CONFIG_HTTP_CLIENT_PROXY					CFG_OFF
#define TLS_CONFIG_HTTP_CLIENT_AUTH_BASIC				CFG_OFF
#define TLS_CONFIG_HTTP_CLIENT_AUTH_DIGEST				CFG_OFF
#define TLS_CONFIG_HTTP_CLIENT_AUTH						(TLS_CONFIG_HTTP_CLIENT_AUTH_BASIC || TLS_CONFIG_HTTP_CLIENT_AUTH_DIGEST)
#define TLS_CONFIG_HTTP_CLIENT_SECURE					CFG_OFF
#define TLS_CONFIG_HTTP_CLIENT_TASK						(CFG_ON && TLS_CONFIG_HTTP_CLIENT)



/**IGMP**/
#define TLS_CONFIG_IGMP            				        CFG_ON

/** UPNP AND DLNA**/
#define TLS_CONFIG_UPNP                          		CFG_OFF
#define TLS_CONFIG_DLNA                          		CFG_OFF && TLS_CONFIG_UPNP


#define TLS_CONFIG_NTP 									CFG_ON


#define  VERC_DNS_OPT						            CFG_ON
#define  VERC_LWIP_OPT                                  CFG_ON



#include "wm_ram_config.h"
#endif /*__WM_CONFIG_H__*/

