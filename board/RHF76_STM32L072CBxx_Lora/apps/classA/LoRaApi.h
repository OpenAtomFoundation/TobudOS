#ifndef __LORAAPI_H__
#define __LORAAPI_H__

#include "stdio.h"
#include "string.h"
#include "utilities.h"
#include "board.h"
#include "gpio.h"
#include "LoRaMac.h"
#include "NvmCtxMgmt.h"

#define OVER_THE_AIR_ACTIVATION                            0

#define ABP_ACTIVATION_LRWAN_VERSION_V10x                  0x01000300 // 1.0.3.0

#define ABP_ACTIVATION_LRWAN_VERSION                       ABP_ACTIVATION_LRWAN_VERSION_V10x

#define LORAWAN_PUBLIC_NETWORK                             true

#define LORAWAN_NETWORK_ID                                 ( uint32_t )0

#define ACTIVE_REGION 										LORAMAC_REGION_CN470

#define LORAWAN_APP_DATA_MAX_SIZE                           64

#define LORAWAN_DEFAULT_DATARATE                    DR_0


#define LORAWAN_ADR_ON                              1

#define LORAWAN_DUTYCYCLE_ON                        false


extern bool NextTx;
extern TimerEvent_t TxNextPacketTimer;

void LoRa_Init(void);
void LoRa_JoinNetwork(uint8_t jointype);
uint8_t Lora_Send(uint8_t type,uint8_t port, uint8_t * psrc, uint16_t len);
void printf_device_info(void);

#endif
