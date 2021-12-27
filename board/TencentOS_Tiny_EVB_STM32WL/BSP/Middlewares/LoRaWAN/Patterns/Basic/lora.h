/**
  ******************************************************************************
  * @file    lora.h
  * @author  MCD Application Team
  * @brief   lora API to drive the lora state Machine
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

#ifndef __LORA_H__
#define __LORA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "Commissioning.h"
#include "LoRaMac.h"
#include "app_conf.h"
#if defined (LORAMAC_CLASSB_ACTIVE) && ( LORAMAC_CLASSB_ACTIVE == 1 )
#include "LoRaMacClassB.h"
#elif !defined (LORAMAC_CLASSB_ACTIVE)
 #error LORAMAC_CLASSB_ACTIVE not defined
#endif /* LORAMAC_CLASSB_ACTIVE */

#include "region/Region.h"


/* Exported constants --------------------------------------------------------*/
/*!
* LoRaWAN confirmed messages
*/

#define LORAWAN_ADR_ON                              1
#define LORAWAN_ADR_OFF                             0
/* Exported types ------------------------------------------------------------*/

/*!
 * Application Data structure
 */
typedef struct
{
  /*point to the LoRa App data buffer*/
  uint8_t *buff;
  /*LoRa App data buffer size*/
  uint8_t size;
  /*Port on which the LoRa App is data is sent/ received*/
  uint8_t port;
	
  uint8_t ack;
	
  uint8_t fpending;

} lw_app_data_t;


typedef enum
{
  LORA_RESET = 0,
  LORA_SET = !LORA_RESET
} LoraFlagStatus;

typedef enum
{
  LORA_DISABLE = 0,
  LORA_ENABLE = !LORA_DISABLE
} lw_state_t;

typedef enum
{
  LORA_ERROR = -1,
  LORA_SUCCESS = 0
} LoraErrorStatus;

typedef enum
{
  LORAWAN_UNCONFIRMED_MSG = 0,
  LORAWAN_CONFIRMED_MSG = !LORAWAN_UNCONFIRMED_MSG
} LoraConfirm_t;

typedef enum
{
  LORA_TRUE = 0,
  LORA_FALSE = !LORA_TRUE
} LoraBool_t;


typedef enum
{
	APP_RX_EVENT,
	APP_JOINED_EVENT,
	APP_CONFIRM_ClASS_EVENT,
	APP_TX_DONE_EVENT,
}APP_EVENT_t;


/* Lora Main callbacks*/
typedef struct sLoRaMainCallback
{
	 /*!
   * @brief mac event callback
   *
   * @retval null   
   */
	void (*app_events)(APP_EVENT_t evt,const void* param);
	/*!
   *\brief    Will be called each time a Radio IRQ is handled by the MAC
   *          layer.
   *
   *\warning  Runs in a IRQ context. Should only change variables state.
   */
  void (*MacProcessNotify)(void);
  /*!
   * @brief Get the current battery level
   *
   * @retval value  battery level ( 0: very low, 254: fully charged )
   */
  uint8_t (*BoardGetBatteryLevel)(void);
  /*!
   * \brief Get the current temperature
   *
   * \retval value  temperature in degreeCelcius( q7.8 )
   */
  uint16_t (*BoardGetTemperatureLevel)(void);
  /*!
   * @brief Gets the board 64 bits unique ID
   *
   * @param [IN] id Pointer to an array that will contain the Unique ID
   */
  void (*BoardGetUniqueId)(uint8_t *id);
  /*!
  * Returns a pseudo random seed generated using the MCU Unique ID
  *
  * @retval seed Generated pseudo random seed
  */
  uint32_t (*BoardGetRandomSeed)(void);

} LoRaMainCallback_t;



typedef struct
{
    uint16_t mask[6];
}LWChannel_mask_t;


/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/**
 * @brief Lora Initialisation
 * @param [IN] LoRaMainCallback_t
 * @param [IN] application parmaters
 * @retval none
 */
void lw_init(LoRaMainCallback_t *callbacks);

/**
 * @brief run Lora classA state Machine
 * @param [IN] none
 * @retval none
 */
LoRaMacStatus_t lw_send(lw_app_data_t *AppData, LoraConfirm_t IsTxConfirmed);



/**
 * @brief Join a Lora Network in classA
 * @Note if the device is ABP, this is a pass through functon
 * @param [IN] none
 * @retval none
 */
LoRaMacStatus_t lw_join(void);

/**
 * @brief Check whether the Device is joined to the network
 * @param [IN] none
 * @retval returns LORA_SET if joined
 */
LoraFlagStatus lw_join_status(void);

/*=====================================================================
if you want to use the Api below , you must call them after LORA_Init

because it has been made Initialization configuration 

======================================================================*/

/**
 * @brief Enable or Disable ADR
 * @param [IN] lw_state_t
 * @retval returns req state 
 */
LoRaMacStatus_t lw_adr_set(lw_state_t sta);


/**
 * @brief  uplink confirm  retry times setting
 * @param [IN] times
 * @retval returns req state 
 */
void lw_confirm_retry_set(uint8_t retry);

/**
 * @brief  return uplink confirm  retry times  
 * @param [IN] null
 * @retval times
 */
uint8_t lw_confirm_retry_get(void);


/**
 * @brief  set the currnt chmask
 * @param [IN] LWChannel_mask_t
 * @retval status
 */
LoRaMacStatus_t lw_current_chmask_set(const LWChannel_mask_t* ch);

/**
 * @brief  return the currnt chmask
 * @param [IN] LWChannel_mask_t
 * @retval status
 */
LoRaMacStatus_t lw_current_chmask_get(LWChannel_mask_t* ch);

/**
 * @brief  set Rx win2  datarate and freq , there are default configuration in region parameter
 * @param [IN] RxChannelParams_t
 * @retval status
 */
LoRaMacStatus_t lw_rxwin2_ch_set(const RxChannelParams_t* rx2_ch);

/**
 * @brief  get Rx win2  datarate and freq
 * @param [IN] RxChannelParams_t
 * @retval status
 */

LoRaMacStatus_t lw_rxwin2_ch_get(RxChannelParams_t* rx2_ch);

/**
 * @brief change Lora Class
 * @Note callback LORA_ConfirmClass informs upper layer that the change has occured
 * @Note Only switch from class A to class B/C OR from  class B/C to class A is allowed
 * @Attention can be calld only in LORA_ClassSwitchSlot or LORA_RxData callbacks
 * @param [IN] DeviceClass_t NewClass
 * @retval LoraErrorStatus
 */
LoRaMacStatus_t lw_request_class(DeviceClass_t newClass);

/**
 * @brief get the current Lora Class
 * @param [IN] DeviceClass_t NewClass
 * @retval None
 */
DeviceClass_t lw_get_current_class(void);

/**
  * @brief  Set join activation process: OTAA vs ABP
  * @param  Over The Air Activation status to set: enable or disable
  * @retval None
  */
void lw_config_otaa_set(lw_state_t otaa);

/**
  * @brief  Get join activation process: OTAA vs ABP
  * @param  None
  * @retval ENABLE if OTAA is used, DISABLE if ABP is used
  */
lw_state_t lw_config_otaa_get(void);

/**
  * @brief  Set duty cycle: ENABLE or DISABLE
  * @param  Duty cycle to set: enable or disable
  * @retval None
  */
void lw_config_duty_cycle_set(lw_state_t duty_cycle);

/**
  * @brief  Get Duty cycle: OTAA vs ABP
  * @param  None
  * @retval ENABLE / DISABLE
  */
lw_state_t lw_config_duty_cycle_get(void);

/**
  * @brief  Get Device EUI
  * @param  None
  * @retval DevEUI
  */
uint8_t *lw_config_deveui_get(void);

/**
  * @brief  Get Join Eui
  * @param  None
  * @retval JoinEUI
  */
uint8_t *lw_config_joineui_get(void);

/**
  * @brief  Set Join Eui
  * @param  JoinEUI
  * @retval Nonoe
  */
void lw_config_joineui_set(uint8_t joineui[8]);

/**
  * @brief  Get Application Key
  * @param  None
  * @retval AppKey
  */
uint8_t *lw_config_appkey_get(void);

/**
  * @brief  Set Application Key
  * @param  AppKey
  * @retval None
  */
void lw_config_appkey_set(uint8_t appkey[16]);

/**
 * @brief  Set whether or not acknowledgement is required
 * @param  ENABLE or DISABLE
 * @retval None
 */
void lw_config_reqack_set(LoraConfirm_t reqack);

/**
 * @brief  Get whether or not acknowledgement is required
 * @param  None
 * @retval ENABLE or DISABLE
 */
LoraConfirm_t lw_config_reqack_get(void);

/**
 * @brief  Get the SNR of the last received data
 * @param  None
 * @retval SNR
 */
int8_t lw_config_snr_get(void);

/**
 * @brief  Get the RSSI of the last received data
 * @param  None
 * @retval RSSI
 */
int16_t lw_config_rssi_get(void);

/**
 * @brief  set tx datarate
 * @param  None
 * @retval The application port
 */
void lw_config_tx_datarate_set(int8_t TxDataRate);

/**
 * @brief  get tx datarate
 * @param  None
 * @retval tx datarate
 */
int8_t lw_config_tx_datarate_get(void);

/**
 * @brief  get LoRaMac region
 * @param  None
 * @retval LoRaMac region
 */
LoRaMacRegion_t lw_region_get(void);

#if defined (LORAMAC_CLASSB_ACTIVE) && ( LORAMAC_CLASSB_ACTIVE == 1 )
    
    LoRaMacClassBNvmCtx_t* LORA_Get_ClassBCtx(void);
    uint8_t LORA_Get_PingSlotPeriodicity(void);
    void LORA_Set_PingSlotPeriodicity(uint8_t PingSlotPeriodicity);
    
#elif !defined (LORAMAC_CLASSB_ACTIVE)
 #error LORAMAC_CLASSB_ACTIVE not defined
#endif
    
#ifdef __cplusplus
    }
#endif




#ifdef __cplusplus
}
#endif

#endif /*__LORA_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
