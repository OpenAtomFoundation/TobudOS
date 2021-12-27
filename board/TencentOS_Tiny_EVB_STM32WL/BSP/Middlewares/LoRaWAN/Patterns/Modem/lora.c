/**
  ******************************************************************************
  * @file    lora.c
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

/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "LoRaMac.h"
#include "lora.h"
#include "stm32_tiny_sscanf.h"
#include "app_system.h"
#include "stm32_seq.h"
#include "lorawan_info.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/

/**
  * Lora Configuration
  */
typedef struct
{
  LoraState_t otaa;        /*< ENABLE if over the air activation, DISABLE otherwise */
  LoraState_t duty_cycle;  /*< ENABLE if dutycyle is on, DISABLE otherwise */
  uint8_t DevEui[8];           /*< Device EUI */
  uint8_t JoinEui[8];           /*< Join Eui */
  uint8_t AppKey[16];          /*< Application Key */
  uint8_t NwkKey[16];          /*< Application Key */
  uint8_t NwkSEncKey[16];         /*< Network Session Key */
  uint8_t AppSKey[16];         /*< Application Session Key */
  uint8_t FNwkSIntKey[16];         /*< Application Session Key */
  uint8_t SNwkSIntKey[16];         /*< Application Session Key */
  int16_t Rssi;                /*< Rssi of the received packet */
  int8_t Snr;                 /*< Snr of the received packet */
  uint8_t application_port;    /*< Application port we will receive to */
  LoraConfirm_t ReqAck;      /*< ENABLE if acknowledge is requested */
  int8_t TxDatarate;
} lora_configuration_t;


static lora_configuration_t lora_config =
{
  .otaa = LORA_ENABLE,
  .duty_cycle = LORA_ENABLE,
  .DevEui = LORAWAN_DEVICE_EUI,
  .JoinEui = LORAWAN_JOIN_EUI,
  .AppKey = LORAWAN_APP_KEY,
  .NwkKey = LORAWAN_NWK_KEY,
  .NwkSEncKey = LORAWAN_NWK_S_ENC_KEY,
  .AppSKey = LORAWAN_APP_S_KEY,
  .FNwkSIntKey = LORAWAN_F_NWK_S_INT_KEY,
  .SNwkSIntKey = LORAWAN_S_NWK_S_INT_KEY,
  .Rssi = 0,
  .Snr = 0,
  .ReqAck = LORAWAN_UNCONFIRMED_MSG,
  .TxDatarate = 0
};

/*!
 *  Select either Device_Time_req or Beacon_Time_Req following LoRaWAN version
 *  - Device_Time_req   Available for V1.0.3 or later
 *  - Beacon_time_Req   Available for V1.0.2 and before
 */
#define USE_DEVICE_TIMING

/*!
 * Join requests trials duty cycle.
 */
#define OVER_THE_AIR_ACTIVATION_DUTYCYCLE           10000  // 10 [s] value in ms

#include "LoRaMacTest.h"

#ifndef LORAMAC_CLASSB_ACTIVE
 #error LORAMAC_CLASSB_ACTIVE not defined ( shall be <0 or 1> )
#endif /* LORAMAC_CLASSB_ACTIVE */

#if ( LORAMAC_CLASSB_ACTIVE == 1 )
/*!
 * Default ping slots periodicity
 *
 * \remark periodicity is equal to 2^LORAWAN_DEFAULT_PING_SLOT_PERIODICITY seconds
 *         example: 2^3 = 8 seconds. The end-device will open an Rx slot every 8 seconds.
 */
#define LORAWAN_DEFAULT_PING_SLOT_PERIODICITY       0
static uint8_t  DefaultPingSlotPeriodicity;

#endif /* LORAMAC_CLASSB_ACTIVE */


#define HEX16(X)  X[0],X[1], X[2],X[3], X[4],X[5], X[6],X[7],X[8],X[9], X[10],X[11], X[12],X[13], X[14],X[15]
#define HEX8(X)   X[0],X[1], X[2],X[3], X[4],X[5], X[6],X[7]

static MlmeReqJoin_t JoinParameters;


static uint32_t DevAddr = LORAWAN_DEVICE_ADDRESS;

/* Dummy data sent periodically to let the tester respond with start test command*/
static TimerEvent_t TxcertifTimer;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

#if ( LORAMAC_CLASSB_ACTIVE == 1 )
    static LoraErrorStatus LORA_BeaconReq(void);
    static LoraErrorStatus LORA_PingSlotReq(void);

    #if defined( USE_DEVICE_TIMING )
        static LoraErrorStatus LORA_DeviceTimeReq(void);
    #else
        static LoraErrorStatus LORA_BeaconTimeReq(void);
    #endif /* USE_DEVICE_TIMING */
#endif /* LORAMAC_CLASSB_ACTIVE */

static void OnCertifTimer(void *);

static void CertifSend(void);
/*!
 * Defines the LoRa parameters at Init
 */
static LoRaParam_t *LoRaParamInit;
static LoRaMacPrimitives_t LoRaMacPrimitives;
static LoRaMacCallback_t LoRaMacCallbacks;
static MibRequestConfirm_t mibReq;
static LoRaMacRegion_t LoRaRegion;

static LoRaMainCallback_t *LoRaMainCallbacks;

static LoRaCertifCallback_t *certif;

/* Private user code ---------------------------------------------------------*/
/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] McpsConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void McpsConfirm(McpsConfirm_t *mcpsConfirm)
{
  if (mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
  {
    switch (mcpsConfirm->McpsRequest)
    {
      case MCPS_UNCONFIRMED:
      {
        // Check Datarate
        // Check TxPower
        break;
      }
      case MCPS_CONFIRMED:
      {
        // Check Datarate
        // Check TxPower
        // Check AckReceived
        if (mcpsConfirm->AckReceived)
        {
          LoRaMainCallbacks->LORA_McpsDataConfirm();
        }
        // Check NbTrials
        break;
      }
      case MCPS_PROPRIETARY:
      {
        break;
      }
      default:
        break;
    }
  }
}

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void McpsIndication(McpsIndication_t *mcpsIndication)
{
  lora_AppData_t AppData;

  if (mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK)
  {
    return;
  }

  switch (mcpsIndication->McpsIndication)
  {
    case MCPS_UNCONFIRMED:
    {
      break;
    }
    case MCPS_CONFIRMED:
    {
      break;
    }
    case MCPS_PROPRIETARY:
    {
      break;
    }
    case MCPS_MULTICAST:
    {
      break;
    }
    default:
      break;
  }

  // Check Multicast
  // Check Port
  // Check Datarate
  // Check FramePending
  if (mcpsIndication->FramePending == true)
  {
    // The server signals that it has pending data to be sent.
    // We schedule an uplink as soon as possible to flush the server.
    LoRaMainCallbacks->LORA_TxNeeded();
  }
  // Check Buffer
  // Check BufferSize
  // Check Rssi
  // Check Snr
  // Check RxSlot
  /*if (certif->running() == true)
  {
    certif->DownLinkIncrement();
  }*/

  if (mcpsIndication->RxData == true)
  {
    switch (mcpsIndication->Port)
    {
      /*case CERTIF_PORT:
        certif->rx(mcpsIndication, &JoinParameters);
        break;
        */
      default:

        AppData.Port = mcpsIndication->Port;
        AppData.BuffSize = mcpsIndication->BufferSize;
        AppData.Buff = mcpsIndication->Buffer;
        lora_config.Rssi = mcpsIndication->Rssi;
        lora_config.Snr  = mcpsIndication->Snr;
        LoRaMainCallbacks->LORA_RxData(&AppData);
        break;
    }
  }
}

/*!
 * \brief   MLME-Confirm event function
 *
 * \param   [IN] MlmeConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void MlmeConfirm(MlmeConfirm_t *mlmeConfirm)
{
#if ( LORAMAC_CLASSB_ACTIVE == 1 )
  MibRequestConfirm_t mibReq;
#endif /* LORAMAC_CLASSB_ACTIVE */

  switch (mlmeConfirm->MlmeRequest)
  {
    case MLME_JOIN:
    {
      if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
      {
        // Status is OK, node has joined the network
        LoRaMainCallbacks->LORA_HasJoined();
#if ( LORAMAC_CLASSB_ACTIVE == 1 )
#if defined( USE_DEVICE_TIMING )
        LORA_DeviceTimeReq();
#else
        LORA_BeaconTimeReq();
#endif /* USE_DEVICE_TIMING */

#endif /* LORAMAC_CLASSB_ACTIVE */
      }
      else
      {
        // Join was not successful. Try to join again
        LORA_Join();
      }
      break;
    }
    case MLME_LINK_CHECK:
    {
      if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
      {
        // Check DemodMargin
        // Check NbGateways
        /*if (certif->running() == true)
        {
          certif->linkCheck(mlmeConfirm);
        }*/
      }
      break;
    }
#if ( LORAMAC_CLASSB_ACTIVE == 1 )
    case MLME_BEACON_ACQUISITION:
    {
      if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
      {
        /* Beacon has been acquired */
        /* REquest Server for Ping Slot */
        LORA_PingSlotReq();
      }
      else
      {
        /* Beacon not acquired */
        /* Search again */
        /* we can check if the MAC has received a time reference for the beacon*/
        /* in this case do either a Device_Time_Req  or a Beacon_Timing_req*/
        LORA_BeaconReq();
      }
      break;
    }
    case MLME_PING_SLOT_INFO:
    {
      if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
      {
        /* class B is now ativated*/
        mibReq.Type = MIB_DEVICE_CLASS;
        mibReq.Param.Class = CLASS_B;
        LoRaMacMibSetRequestConfirm(&mibReq);

        if ((LoRaParamInit->ActiveRegion == LORAMAC_REGION_AU915) || (LoRaParamInit->ActiveRegion == LORAMAC_REGION_US915))
        {
          mibReq.Type = MIB_PING_SLOT_DATARATE;
          mibReq.Param.PingSlotDatarate = DR_8;
          LoRaMacMibSetRequestConfirm(&mibReq);
        } 

        /*notify upper layer*/
        LoRaMainCallbacks->LORA_ConfirmClass(CLASS_B);
      }
      else
      {
        LORA_PingSlotReq();
      }
      break;
    }
#if defined( USE_DEVICE_TIMING )
    case MLME_DEVICE_TIME:
    {
      if (mlmeConfirm->Status != LORAMAC_EVENT_INFO_STATUS_OK)
      {
        LORA_DeviceTimeReq();
      }
    }
#endif /* USE_DEVICE_TIMING */
#endif /* LORAMAC_CLASSB_ACTIVE */
    default:
      break;
  }
}

/*!
 * \brief   MLME-Indication event function
 *
 * \param   [IN] MlmeIndication - Pointer to the indication structure.
 */
static void MlmeIndication(MlmeIndication_t *MlmeIndication)
{
#if ( LORAMAC_CLASSB_ACTIVE == 1 )
  MibRequestConfirm_t mibReq;
#endif /* LORAMAC_CLASSB_ACTIVE */


  switch (MlmeIndication->MlmeIndication)
  {
    case MLME_SCHEDULE_UPLINK:
    {
      // The MAC signals that we shall provide an uplink as soon as possible
      LoRaMainCallbacks->LORA_TxNeeded();
      break;
    }
#if ( LORAMAC_CLASSB_ACTIVE == 1 )
    case MLME_BEACON_LOST:
    {
      // Switch to class A again
      mibReq.Type = MIB_DEVICE_CLASS;
      mibReq.Param.Class = CLASS_A;
      LoRaMacMibSetRequestConfirm(&mibReq);

      LORA_BeaconReq();
      break;
    }
    case MLME_BEACON:
    {
      if (MlmeIndication->Status == LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED)
      {
        APP_LOG(TS_ON, VLEVEL_H, "BEACON RECEIVED\n\r");
      }
      else
      {
        APP_LOG(TS_ON, VLEVEL_H, "BEACON NOT RECEIVED\n\r");
      }
      break;

    }
#endif /* LORAMAC_CLASSB_ACTIVE */
    default:
      break;
  }
}
/**
 *  lora Init
 */
void LORA_Init(LoRaMainCallback_t *callbacks, LoRaParam_t *LoRaParam)
{
  LoRaMacInfo_t *LoRaMacRegionInfo;

  LoraMacInfo_Init();
  LoRaMacRegionInfo = LoraMacInfo_GetPtr();
  if (LoRaMacRegionInfo->Region == 0)
  {
    APP_PRINTF("error: At leats one region shall be defined in the MW: check lorawan_conf.h \n\r");
    while(1) {} /* At leats one region shall be defined in MW */
  }

#if ( LORAMAC_CLASSB_ACTIVE == 1 )
  if (LoRaMacRegionInfo->ClassB != 1)
  {
    APP_PRINTF("error: ClassB is not defined in the MW: set lorawan_conf.h accordingly\n\r");
    while(1) {}   /* error: ClassB is not defined in the MW*/
  }
#elif ( LORAMAC_CLASSB_ACTIVE == 0 )
  if (LoRaMacRegionInfo->ClassB != 0)
  {
    APP_PRINTF("error: missmatch between applic and MW CLASSB defines: set lorawan_conf.h according to app_conf.h\n\r");
    while(1) {}   /* error: ClassB should not be defined in the MW*/
  }
#endif /* LORAMAC_CLASSB_ACTIVE */

  /* init the Tx Duty Cycle*/
  LoRaParamInit = LoRaParam;

  /* init the main call backs*/
  LoRaMainCallbacks = callbacks;

#if (STATIC_DEVICE_EUI != 1)
  LoRaMainCallbacks->BoardGetUniqueId(lora_config.DevEui);
#endif

#if (STATIC_DEVICE_ADDRESS != 1)
  // Choose a "pseudo-random" device address
  DevAddr =  LoRaMainCallbacks->BoardGetRandomSeed();
#endif

  LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
  LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
  LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
  LoRaMacPrimitives.MacMlmeIndication = MlmeIndication;
  LoRaMacCallbacks.GetBatteryLevel = LoRaMainCallbacks->BoardGetBatteryLevel;
  LoRaMacCallbacks.GetTemperatureLevel = LoRaMainCallbacks->BoardGetTemperatureLevel;
  LoRaMacCallbacks.MacProcessNotify = LoRaMainCallbacks->MacProcessNotify;

  if( 0U != ((1<<(LoRaParam->ActiveRegion))& (LoRaMacRegionInfo->Region)) )
  {
    LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, LoRaParam->ActiveRegion);
  }
  else
  {
    APP_PRINTF("error: Region is not defined in the MW: set lorawan_conf.h accordingly\n\r");
    while(1) {}   /* error: Region is not defined in the MW */
  }

#if ( HYBRID_ENABLED == 1 )
    if ((LoRaParamInit->ActiveRegion == LORAMAC_REGION_AU915) || (LoRaParamInit->ActiveRegion == LORAMAC_REGION_US915))
    { 
      uint16_t channelMask[] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0001, 0x0000};
      mibReq.Type = MIB_CHANNELS_MASK;
      mibReq.Param.ChannelsMask = channelMask;
      LoRaMacMibSetRequestConfirm(&mibReq);
      mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
      mibReq.Param.ChannelsDefaultMask = channelMask;
      LoRaMacMibSetRequestConfirm(&mibReq);
    }
    if (LoRaParamInit->ActiveRegion == LORAMAC_REGION_CN470) 
    {
      uint16_t channelMask[] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
      mibReq.Type = MIB_CHANNELS_MASK;
      mibReq.Param.ChannelsMask = channelMask;
      LoRaMacMibSetRequestConfirm(&mibReq);
      mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
      mibReq.Param.ChannelsDefaultMask = channelMask;
      LoRaMacMibSetRequestConfirm(&mibReq);
    }
#elif !defined (HYBRID_ENABLED)
    APP_PRINTF("error: HYBRID_ENABLED has not been defined at compilation\n\r");
    #error HYBRID_ENABLED not defined ( shall be <0 or 1> )
#endif /* HYBRID_ENABLED */

  lora_config_otaa_set(LORA_ENABLE);

  mibReq.Type = MIB_DEV_EUI;
  mibReq.Param.DevEui = lora_config.DevEui;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_JOIN_EUI;
  mibReq.Param.JoinEui = lora_config.JoinEui;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_ADR;
  mibReq.Param.AdrEnable = LoRaParamInit->AdrEnable;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_PUBLIC_NETWORK;
  mibReq.Param.EnablePublicNetwork = LoRaParamInit->EnablePublicNetwork;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_APP_KEY;
  mibReq.Param.AppKey = lora_config.AppKey;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_NWK_KEY;
  mibReq.Param.NwkKey = lora_config.NwkKey;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_DEVICE_CLASS;
  mibReq.Param.Class = CLASS_A;
  LoRaMacMibSetRequestConfirm(&mibReq);

  if (   (LoRaParamInit->ActiveRegion == LORAMAC_REGION_EU868) 
      || (LoRaParamInit->ActiveRegion == LORAMAC_REGION_RU864)
      || (LoRaParamInit->ActiveRegion == LORAMAC_REGION_CN779)
      || (LoRaParamInit->ActiveRegion == LORAMAC_REGION_EU433))
  {
    LoRaMacTestSetDutyCycleOn(true);
    lora_config.duty_cycle = LORA_ENABLE;
  }
  else
  {
    lora_config.duty_cycle = LORA_DISABLE;
  }

  mibReq.Type = MIB_SYSTEM_MAX_RX_ERROR;
  mibReq.Param.SystemMaxRxError = 20;
  LoRaMacMibSetRequestConfirm(&mibReq);

#if ( LORAMAC_CLASSB_ACTIVE == 1 )
  if (LoRaMacRegionInfo->ClassB == 1)
  {
    DefaultPingSlotPeriodicity =  LORAWAN_DEFAULT_PING_SLOT_PERIODICITY;
  }
#endif /* LORAMAC_CLASSB_ACTIVE */

  /*set Mac statein Idle*/
  LoRaMacStart();
}

LoraErrorStatus LORA_Join(void)
{
  LoraErrorStatus status;
  if (lora_config.otaa == LORA_ENABLE)
  {
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_JOIN;
    mlmeReq.Req.Join.Datarate = LoRaParamInit->TxDatarate;

    JoinParameters = mlmeReq.Req.Join;

    LoRaMacMlmeRequest(&mlmeReq);

    status = LORA_SUCCESS;
  }
  else
  {
    /*no Join in abp*/
    status = LORA_ERROR;
  }

  return status;
}

LoraFlagStatus LORA_JoinStatus(void)
{
  MibRequestConfirm_t mibReq;

  mibReq.Type = MIB_NETWORK_ACTIVATION;

  LoRaMacMibGetRequestConfirm(&mibReq);

  if (mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE)
  {
    return LORA_RESET;
  }
  else
  {
    return LORA_SET;
  }
}



LoraErrorStatus LORA_send(lora_AppData_t *AppData, LoraConfirm_t IsTxConfirmed)
{
  McpsReq_t mcpsReq;
  LoRaMacTxInfo_t txInfo;

  /*if certification test are on going, application data is not sent*/
    /*
  if (certif->running() == true)
  {
    return LORA_ERROR;
  }*/

  if (LoRaMacQueryTxPossible(AppData->BuffSize, &txInfo) != LORAMAC_STATUS_OK)
  {
    // Send empty frame in order to flush MAC commands
    mcpsReq.Type = MCPS_UNCONFIRMED;
    mcpsReq.Req.Unconfirmed.fBuffer = NULL;
    mcpsReq.Req.Unconfirmed.fBufferSize = 0;
    mcpsReq.Req.Unconfirmed.Datarate = lora_config_tx_datarate_get() ;
  }
  else
  {
    if (IsTxConfirmed == LORAWAN_UNCONFIRMED_MSG)
    {
      mcpsReq.Type = MCPS_UNCONFIRMED;
      mcpsReq.Req.Unconfirmed.fPort = AppData->Port;
      mcpsReq.Req.Unconfirmed.fBufferSize = AppData->BuffSize;
      mcpsReq.Req.Unconfirmed.fBuffer = AppData->Buff;
      mcpsReq.Req.Unconfirmed.Datarate = lora_config_tx_datarate_get() ;
    }
    else
    {
      mcpsReq.Type = MCPS_CONFIRMED;
      mcpsReq.Req.Confirmed.fPort = AppData->Port;
      mcpsReq.Req.Confirmed.fBufferSize = AppData->BuffSize;
      mcpsReq.Req.Confirmed.fBuffer = AppData->Buff;
      mcpsReq.Req.Confirmed.NbTrials = 8;
      mcpsReq.Req.Confirmed.Datarate = lora_config_tx_datarate_get() ;
    }
  }
  if (LoRaMacMcpsRequest(&mcpsReq) == LORAMAC_STATUS_OK)
  {
    return LORA_SUCCESS;
  }
  return LORA_ERROR;
}

#if ( LORAMAC_CLASSB_ACTIVE == 1 )
#if defined( USE_DEVICE_TIMING )
static LoraErrorStatus LORA_DeviceTimeReq(void)
{
  MlmeReq_t mlmeReq;

  mlmeReq.Type = MLME_DEVICE_TIME;

  if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK)
  {
    return LORA_SUCCESS;
  }
  else
  {
    return LORA_ERROR;
  }
}
#else
static LoraErrorStatus LORA_BeaconTimeReq(void)
{
  MlmeReq_t mlmeReq;

  mlmeReq.Type = MLME_BEACON_TIMING;

  if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK)
  {
    return LORA_SUCCESS;
  }
  else
  {
    return LORA_ERROR;
  }
}
#endif /* USE_DEVICE_TIMING */

static LoraErrorStatus LORA_BeaconReq(void)
{
  MlmeReq_t mlmeReq;

  mlmeReq.Type = MLME_BEACON_ACQUISITION;

  if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK)
  {
    return LORA_SUCCESS;
  }
  else
  {
    return LORA_ERROR;
  }
}

static LoraErrorStatus LORA_PingSlotReq(void)
{
  MlmeReq_t mlmeReq;

  mlmeReq.Type = MLME_LINK_CHECK;
  LoRaMacMlmeRequest(&mlmeReq);

  mlmeReq.Type = MLME_PING_SLOT_INFO;
  mlmeReq.Req.PingSlotInfo.PingSlot.Fields.Periodicity = DefaultPingSlotPeriodicity;
  mlmeReq.Req.PingSlotInfo.PingSlot.Fields.RFU = 0;

  if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK)
  {
    return LORA_SUCCESS;
  }
  else
  {
    return LORA_ERROR;
  }
}
#endif /* LORAMAC_CLASSB_ACTIVE */

LoraErrorStatus LORA_RequestClass(DeviceClass_t newClass)
{
  LoraErrorStatus Errorstatus = LORA_SUCCESS;
  MibRequestConfirm_t mibReq;
  DeviceClass_t currentClass;

  mibReq.Type = MIB_DEVICE_CLASS;
  LoRaMacMibGetRequestConfirm(&mibReq);

  currentClass = mibReq.Param.Class;
  /*attempt to swicth only if class update*/
  if (currentClass != newClass)
  {
    switch (newClass)
    {
      case CLASS_A:
      {
        mibReq.Param.Class = CLASS_A;
        if (LoRaMacMibSetRequestConfirm(&mibReq) == LORAMAC_STATUS_OK)
        {
          /*switch is instantanuous*/
          LoRaMainCallbacks->LORA_ConfirmClass(CLASS_A);
        }
        else
        {
          Errorstatus = LORA_ERROR;
        }
        break;
      }
      case CLASS_B:
      {
#if ( LORAMAC_CLASSB_ACTIVE == 1 )
        if (currentClass != CLASS_A)
        {
          Errorstatus = LORA_ERROR;
        }
        /*switch is not instantanuous*/
        Errorstatus = LORA_BeaconReq();
#else
        APP_PRINTF("warning: LORAMAC_CLASSB_ACTIVE has not been defined at compilation\n\r");
#endif /* LORAMAC_CLASSB_ACTIVE */
        break;
      }
      case CLASS_C:
      {
        if (currentClass != CLASS_A)
        {
          Errorstatus = LORA_ERROR;
        }
        /*switch is instantanuous*/
        mibReq.Param.Class = CLASS_C;
        if (LoRaMacMibSetRequestConfirm(&mibReq) == LORAMAC_STATUS_OK)
        {
          LoRaMainCallbacks->LORA_ConfirmClass(CLASS_C);
        }
        else
        {
          Errorstatus = LORA_ERROR;
        }
        break;
      }
      default:
        break;
    }
  }
  return Errorstatus;
}

void LORA_GetCurrentClass(DeviceClass_t *currentClass)
{
  MibRequestConfirm_t mibReq;

  mibReq.Type = MIB_DEVICE_CLASS;
  LoRaMacMibGetRequestConfirm(&mibReq);

  *currentClass = mibReq.Param.Class;
}


void lora_config_otaa_set(LoraState_t otaa)
{
  lora_config.otaa = otaa;

  if (lora_config.otaa == LORA_ENABLE)
  {
    APP_PPRINTF("OTAA Mode enabled\n\r");
    APP_PPRINTF("DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n\r", HEX8(lora_config.DevEui));
    APP_PPRINTF("JoinEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n\r", HEX8(lora_config.JoinEui));
    APP_PPRINTF("AppKey= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n\r", HEX16(lora_config.AppKey));

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    mibReq.Param.NetworkActivation = ACTIVATION_TYPE_NONE;
    LoRaMacMibSetRequestConfirm(&mibReq);
  }
  else
  {
    APP_PPRINTF("ABP Mode enabled\n\r");
    APP_PPRINTF("DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n\r", HEX8(lora_config.DevEui));
    APP_PPRINTF("DevAdd=  %08X\n\r", DevAddr) ;
    APP_PPRINTF("NwkSKey= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n\r", HEX16(lora_config.NwkSEncKey));
    APP_PPRINTF("AppSKey= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n\r", HEX16(lora_config.AppSKey));

    mibReq.Type = MIB_NET_ID;
    mibReq.Param.NetID = LORAWAN_NETWORK_ID;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_DEV_ADDR;
    mibReq.Param.DevAddr = DevAddr;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_F_NWK_S_INT_KEY;
    mibReq.Param.FNwkSIntKey = lora_config.FNwkSIntKey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_S_NWK_S_INT_KEY;
    mibReq.Param.SNwkSIntKey = lora_config.SNwkSIntKey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_NWK_S_ENC_KEY;
    mibReq.Param.NwkSEncKey = lora_config.NwkSEncKey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_APP_S_KEY;
    mibReq.Param.AppSKey = lora_config.AppSKey;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    mibReq.Param.NetworkActivation = ACTIVATION_TYPE_ABP;
    LoRaMacMibSetRequestConfirm(&mibReq);

    // Enable legacy mode to operate according to LoRaWAN Spec. 1.0.3
    Version_t abpLrWanVersion;

    abpLrWanVersion.Fields.Major    = 1;
    abpLrWanVersion.Fields.Minor    = 0;
    abpLrWanVersion.Fields.Revision = 3;
    abpLrWanVersion.Fields.Rfu      = 0;

    mibReq.Type = MIB_ABP_LORAWAN_VERSION;
    mibReq.Param.AbpLrWanVersion = abpLrWanVersion;
    LoRaMacMibSetRequestConfirm(&mibReq);
  }
}


LoraState_t lora_config_otaa_get(void)
{
  return lora_config.otaa;
}

void lora_config_duty_cycle_set(LoraState_t duty_cycle)
{
  lora_config.duty_cycle = duty_cycle;
  LoRaMacTestSetDutyCycleOn((duty_cycle == LORA_ENABLE) ? 1 : 0);
}

LoraState_t lora_config_duty_cycle_get(void)
{
  return lora_config.duty_cycle;
}

uint8_t *lora_config_deveui_get(void)
{
  return lora_config.DevEui;
}

uint8_t *lora_config_joineui_get(void)
{
  return lora_config.JoinEui;
}

void lora_config_joineui_set(uint8_t joineui[8])
{
  UTIL_MEM_cpy_8(lora_config.JoinEui, joineui, sizeof(lora_config.JoinEui));
}

uint8_t *lora_config_appkey_get(void)
{
  return lora_config.AppKey;
}

void lora_config_appkey_set(uint8_t appkey[16])
{
  UTIL_MEM_cpy_8(lora_config.AppKey, appkey, sizeof(lora_config.AppKey));
  UTIL_MEM_cpy_8(lora_config.NwkKey, appkey, sizeof(lora_config.NwkKey));

  mibReq.Type = MIB_APP_KEY;
  mibReq.Param.AppKey = lora_config.AppKey;
  LoRaMacMibSetRequestConfirm(&mibReq);

  mibReq.Type = MIB_NWK_KEY;
  mibReq.Param.NwkKey = lora_config.NwkKey;
  LoRaMacMibSetRequestConfirm(&mibReq);
}

void lora_config_reqack_set(LoraConfirm_t reqack)
{
  lora_config.ReqAck = reqack;
}

LoraConfirm_t lora_config_reqack_get(void)
{
  return lora_config.ReqAck;
}

int8_t lora_config_snr_get(void)
{
  return lora_config.Snr;
}

int16_t lora_config_rssi_get(void)
{
  return lora_config.Rssi;
}

void lora_config_tx_datarate_set(int8_t TxDataRate)
{
  lora_config.TxDatarate = TxDataRate;
}

int8_t lora_config_tx_datarate_get(void)
{
  return lora_config.TxDatarate;
}

/*
static void OnCertifTimer(void *context)
{
  UTIL_SEQ_SetTask(CFG_SEQ_TASK_LORA_CERTIF_TX, CFG_SEQ_Prio_0);
}

static void CertifSend(void)
{
  uint8_t Dummy[1] = {1};
  lora_AppData_t AppData;
  AppData.Buff = Dummy;
  AppData.BuffSize = sizeof(Dummy);
  AppData.Port = 99;

  LORA_send(&AppData, LORAWAN_UNCONFIRMED_MSG);
}

void lora_wan_certif(void)
{
  LoRaMacTestSetDutyCycleOn(false);
  LORA_Join();
  TimerInit(&TxcertifTimer,  OnCertifTimer);  // 8s 
  TimerSetValue(&TxcertifTimer,  8000);  // 8s 
  TimerStart(&TxcertifTimer);
  UTIL_SEQ_RegTask(CFG_SEQ_TASK_LORA_CERTIF_TX, UTIL_SEQ_RFU, CertifSend);

}
*/

LoRaMacRegion_t lora_region_get(void)
{
  return LoRaRegion;
}
/*
void LORA_TestInit(LoRaCertifCallback_t *callbacks)
{
  certif = callbacks;
}
*/
#if ( LORAMAC_CLASSB_ACTIVE == 1 )
LoRaMacClassBNvmCtx_t* LORA_Get_ClassBCtx(void)
{
  mibReq.Type = MIB_NVM_CTXS;
  LoRaMacMibGetRequestConfirm( &mibReq );
  return ( LoRaMacClassBNvmCtx_t*) mibReq.Param.Contexts->ClassBNvmCtx;
}

uint8_t LORA_Get_PingSlotPeriodicity(void)
{
  return DefaultPingSlotPeriodicity;
}

void LORA_Set_PingSlotPeriodicity(uint8_t PingSlotPeriodicity)
{
  DefaultPingSlotPeriodicity= PingSlotPeriodicity;
}
#endif  /* LORAMAC_CLASSB_ACTIVE */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
