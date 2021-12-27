/******************************************************************************
  * @file    lora-test.c
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
#include "stddef.h"
//#include "lorawan_conf.h"    /* Don't remove: needed for LORAMAC_CLASSB_ENABLED */
//#include "mw_log_conf.h"
#include "LoRaMac.h"
#include "LoRaMacTest.h"
#include "lora.h"

/* Private typedef -----------------------------------------------------------*/

typedef struct ComplianceTest_s
{
  bool Running;
  uint8_t State;
  LoraConfirm_t IsTxConfirmed;
  uint8_t DataBufferSize;
  uint8_t DataBuffer[242]; /* max possible payload */
  uint16_t DownLinkCounter;
  bool LinkCheck;
  uint8_t DemodMargin;
  uint8_t NbGateways;
} ComplianceTest_t;

typedef enum
{
  CERTIF_TX_PROCESS = 0,
  CERTIF_RX_PROCESS
} requestProcess_t;

/* Private define ------------------------------------------------------------*/
#define TEST_TX_DUTYCYCLE 5000
/* Private variables ---------------------------------------------------------*/


static requestProcess_t requested_process_id;

/*!
 * Timer to handle the application data transmission duty cycle
 */
static TimerEvent_t CertifTxNextPacketTimer;

static ComplianceTest_t certifParam;
static LoraConfirm_t IsTxConfirmed;
static bool AdrEnableInit;
static McpsIndication_t *CertifMcpsIndication;
static MlmeReqJoin_t *CertifJoinParameters;

/* Private functions ---------------------------------------------------------*/
static bool CERTIF_running(void);

static void CERTIF_DownLinkIncrement(void);

static void CERTIF_linkCheck(MlmeConfirm_t *mlmeConfirm);

static void CERTIF_rx_request(McpsIndication_t *mcpsIndication, MlmeReqJoin_t *JoinParameters);

static void CERTIF_rx(McpsIndication_t *mcpsIndication, MlmeReqJoin_t *JoinParameters);

static bool CERTIF_tx(void);

static void OnCertifTxNextPacketTimerEvent(void *context);

static void (*CertifProcessNotify)(void);

static LoRaCertifCallback_t certifCb =
{
  CERTIF_running,
  CERTIF_DownLinkIncrement,
  CERTIF_linkCheck,
  CERTIF_rx_request,
};

/* Exported functions definition---------------------------------------------------------*/

void CERTIF_Init(void (*CERTIF_ProcessNotify)(void))
{
  /*initialises Callback into the Lora.c layer*/
  LORA_TestInit(&certifCb);
  /*Register TestProcessNotify callback*/
  CertifProcessNotify = CERTIF_ProcessNotify;
}

void CERTIF_Process(void)
{
  switch (requested_process_id)
  {
    case CERTIF_TX_PROCESS:
      CERTIF_tx();
      break;
    case CERTIF_RX_PROCESS:
      CERTIF_rx(CertifMcpsIndication, CertifJoinParameters);
      break;
    default:
      break;
  }
}
static bool CERTIF_running(void)
{
  return certifParam.Running;
}

static void CERTIF_DownLinkIncrement(void)
{
  certifParam.DownLinkCounter++;
}

static void CERTIF_linkCheck(MlmeConfirm_t *mlmeConfirm)
{
  certifParam.LinkCheck = true;
  certifParam.DemodMargin = mlmeConfirm->DemodMargin;
  certifParam.NbGateways = mlmeConfirm->NbGateways;
}

static bool CERTIF_tx(void)
{
  McpsReq_t mcpsReq;
  LoRaMacTxInfo_t txInfo;

  if (certifParam.LinkCheck == true)
  {
    certifParam.LinkCheck = false;
    certifParam.DataBufferSize = 3;
    certifParam.DataBuffer[0] = 5;
    certifParam.DataBuffer[1] = certifParam.DemodMargin;
    certifParam.DataBuffer[2] = certifParam.NbGateways;
    certifParam.State = 1;
  }
  else
  {
    switch (certifParam.State)
    {
      case 4:
        certifParam.State = 1;
        break;
      case 1:
        certifParam.DataBufferSize = 2;
        certifParam.DataBuffer[0] = certifParam.DownLinkCounter >> 8;
        certifParam.DataBuffer[1] = certifParam.DownLinkCounter;
        break;
    }
  }

  if (LoRaMacQueryTxPossible(certifParam.DataBufferSize, &txInfo) != LORAMAC_STATUS_OK)
  {
    // Send empty frame in order to flush MAC commands
    mcpsReq.Type = MCPS_UNCONFIRMED;
    mcpsReq.Req.Unconfirmed.fBuffer = NULL;
    mcpsReq.Req.Unconfirmed.fBufferSize = 0;
    mcpsReq.Req.Unconfirmed.Datarate = DR_0;
  }
  else
  {
    if (IsTxConfirmed == LORAWAN_UNCONFIRMED_MSG)
    {
      mcpsReq.Type = MCPS_UNCONFIRMED;
      mcpsReq.Req.Unconfirmed.fPort = CERTIF_PORT;
      mcpsReq.Req.Unconfirmed.fBufferSize = certifParam.DataBufferSize;
      mcpsReq.Req.Unconfirmed.fBuffer = &(certifParam.DataBuffer);
      mcpsReq.Req.Unconfirmed.Datarate = DR_0;
    }
    else
    {
      mcpsReq.Type = MCPS_CONFIRMED;
      mcpsReq.Req.Confirmed.fPort = CERTIF_PORT;
      mcpsReq.Req.Confirmed.fBufferSize = certifParam.DataBufferSize;
      mcpsReq.Req.Confirmed.fBuffer = &(certifParam.DataBuffer);
      mcpsReq.Req.Confirmed.NbTrials = 8;
      mcpsReq.Req.Confirmed.Datarate = DR_0;
    }
  }

  /*cerification test on-going*/
  TimerStart(&CertifTxNextPacketTimer);

  if (LoRaMacMcpsRequest(&mcpsReq) == LORAMAC_STATUS_OK)
  {
    return false;
  }
  return true;
}

static void CERTIF_rx_request(McpsIndication_t *mcpsIndication, MlmeReqJoin_t *JoinParameters)
{
  CertifMcpsIndication = mcpsIndication;
  CertifJoinParameters = JoinParameters;

  requested_process_id = CERTIF_RX_PROCESS;

  CertifProcessNotify();
}

static void CERTIF_rx(McpsIndication_t *mcpsIndication, MlmeReqJoin_t *JoinParameters)
{
  if (certifParam.Running == false)
  {
    // Check compliance test enable command (i)
    if ((mcpsIndication->BufferSize == 4) &&
        (mcpsIndication->Buffer[0] == 0x01) &&
        (mcpsIndication->Buffer[1] == 0x01) &&
        (mcpsIndication->Buffer[2] == 0x01) &&
        (mcpsIndication->Buffer[3] == 0x01))
    {
      MibRequestConfirm_t mibReq;
      IsTxConfirmed = LORAWAN_UNCONFIRMED_MSG;
      certifParam.DataBufferSize = 2;
      certifParam.DownLinkCounter = 0;
      certifParam.LinkCheck = false;
      certifParam.DemodMargin = 0;
      certifParam.NbGateways = 0;
      certifParam.Running = true;
      certifParam.State = 1;

      mibReq.Type = MIB_ADR;

      LoRaMacMibGetRequestConfirm(&mibReq);
      AdrEnableInit = mibReq.Param.AdrEnable;

      mibReq.Type = MIB_ADR;
      mibReq.Param.AdrEnable = true;
      LoRaMacMibSetRequestConfirm(&mibReq);

      if (   (ACTIVE_REGION == LORAMAC_REGION_EU868) 
          || (ACTIVE_REGION == LORAMAC_REGION_RU864)
          || (ACTIVE_REGION == LORAMAC_REGION_CN779)
          || (ACTIVE_REGION == LORAMAC_REGION_EU433))
      {
        LoRaMacTestSetDutyCycleOn(false);
      }

      TimerInit(&CertifTxNextPacketTimer, OnCertifTxNextPacketTimerEvent);
      TimerSetValue(&CertifTxNextPacketTimer,  TEST_TX_DUTYCYCLE);

      /*confirm test mode activation */
      CERTIF_tx();
    }
  }

  else
  {
    certifParam.State = mcpsIndication->Buffer[0];
    switch (certifParam.State)
    {
      case 0: // Check compliance test disable command (ii)
      {

        certifParam.DownLinkCounter = 0;
        certifParam.Running = false;

        MibRequestConfirm_t mibReq;
        mibReq.Type = MIB_ADR;
        mibReq.Param.AdrEnable = AdrEnableInit;
        LoRaMacMibSetRequestConfirm(&mibReq);

        if (   (ACTIVE_REGION == LORAMAC_REGION_EU868) 
            || (ACTIVE_REGION == LORAMAC_REGION_RU864)
            || (ACTIVE_REGION == LORAMAC_REGION_CN779)
            || (ACTIVE_REGION == LORAMAC_REGION_EU433))
        {
          LoRaMacTestSetDutyCycleOn(true);
        }

        break;
      }
      case 1: // (iii, iv)
        certifParam.DataBufferSize = 2;
        break;
      case 2: // Enable confirmed messages (v)
        IsTxConfirmed = LORAWAN_CONFIRMED_MSG;
        certifParam.State = 1;
        break;
      case 3:  // Disable confirmed messages (vi)
        IsTxConfirmed = LORAWAN_UNCONFIRMED_MSG;
        certifParam.State = 1;
        break;
      case 4: // (vii)
        certifParam.DataBufferSize = mcpsIndication->BufferSize;

        certifParam.DataBuffer[0] = 4;
        for (uint8_t i = 1; i < certifParam.DataBufferSize; i++)
        {
          certifParam.DataBuffer[i] = mcpsIndication->Buffer[i] + 1;
        }
        break;
      case 5: // (viii)
      {
        MlmeReq_t mlmeReq;
        mlmeReq.Type = MLME_LINK_CHECK;
        LoRaMacMlmeRequest(&mlmeReq);
        break;
      }
      case 6: // (ix)
      {
        MlmeReq_t mlmeReq;

        // Disable TestMode and revert back to normal operation

        certifParam.DownLinkCounter = 0;
        certifParam.Running = false;

        MibRequestConfirm_t mibReq;
        mibReq.Type = MIB_ADR;
        mibReq.Param.AdrEnable = AdrEnableInit;
        LoRaMacMibSetRequestConfirm(&mibReq);

        mlmeReq.Type = MLME_JOIN;
        mlmeReq.Req.Join = *JoinParameters;

        LoRaMacMlmeRequest(&mlmeReq);
        break;
      }

      case 7: // (x)
      {
        if (mcpsIndication->BufferSize == 3)
        {
          MlmeReq_t mlmeReq;
          mlmeReq.Type = MLME_TXCW;
          mlmeReq.Req.TxCw.Timeout = (uint16_t)((mcpsIndication->Buffer[1] << 8) | mcpsIndication->Buffer[2]);
          LoRaMacMlmeRequest(&mlmeReq);
        }
        else if (mcpsIndication->BufferSize == 7)
        {
          MlmeReq_t mlmeReq;
          mlmeReq.Type = MLME_TXCW_1;
          mlmeReq.Req.TxCw.Timeout = (uint16_t)((mcpsIndication->Buffer[1] << 8) | mcpsIndication->Buffer[2]);
          mlmeReq.Req.TxCw.Frequency = (uint32_t)((mcpsIndication->Buffer[3] << 16) | (mcpsIndication->Buffer[4] << 8) | mcpsIndication->Buffer[5]) * 100;
          mlmeReq.Req.TxCw.Power = mcpsIndication->Buffer[6];
          LoRaMacMlmeRequest(&mlmeReq);
        }
        certifParam.State = 1;
        break;
      }

      case 8: // Send DeviceTimeReq
      {
        MlmeReq_t mlmeReq;

        mlmeReq.Type = MLME_DEVICE_TIME;

        LoRaMacMlmeRequest(&mlmeReq);
        break;
      }
#if defined (LORAMAC_CLASSB_ACTIVE) && ( LORAMAC_CLASSB_ACTIVE == 1 )
      case 9: // Switch end device Class
      {
        MibRequestConfirm_t mibReq;

        mibReq.Type = MIB_DEVICE_CLASS;
        // CLASS_A = 0, CLASS_B = 1, CLASS_C = 2
        mibReq.Param.Class = (DeviceClass_t)mcpsIndication->Buffer[1];
        LoRaMacMibSetRequestConfirm(&mibReq);
        break;
      }
      case 10: // Send PingSlotInfoReq
      {
        MlmeReq_t mlmeReq;

        mlmeReq.Type = MLME_PING_SLOT_INFO;

        mlmeReq.Req.PingSlotInfo.PingSlot.Value = mcpsIndication->Buffer[1];

        LoRaMacMlmeRequest(&mlmeReq);
        break;
      }
      case 11: // Send BeaconTimingReq
      {
        MlmeReq_t mlmeReq;

        mlmeReq.Type = MLME_BEACON_TIMING;

        LoRaMacMlmeRequest(&mlmeReq);
        break;
      }
#elif !defined (LORAMAC_CLASSB_ACTIVE)
 #error LORAMAC_CLASSB_ACTIVE not defined
#endif
      default:
        break;
    }
  }

  if (certifParam.Running == false)
  {
    /*certification test stops*/
    TimerStop(&CertifTxNextPacketTimer);
  }
}

/*!
 * \brief Function executed on TxNextPacket Timeout event
 */
static void OnCertifTxNextPacketTimerEvent(void *context)
{
  requested_process_id = CERTIF_TX_PROCESS;
  CertifProcessNotify();
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

