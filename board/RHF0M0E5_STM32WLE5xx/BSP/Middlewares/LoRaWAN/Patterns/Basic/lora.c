/*
    _/_/_/    _/_/_/    _/_/_/  _/_/_/  _/      _/    _/_/_/  _/    _/  _/_/_/_/
   _/    _/    _/    _/          _/    _/_/    _/  _/        _/    _/  _/
  _/_/_/      _/      _/_/      _/    _/  _/  _/  _/  _/_/  _/_/_/_/  _/_/_/
 _/    _/    _/          _/    _/    _/    _/_/  _/    _/  _/    _/  _/
_/    _/  _/_/_/  _/_/_/    _/_/_/  _/      _/    _/_/_/  _/    _/  _/
    (C)2020/5/20 RisingHF, all rights reserved.
	 Lorawan API
*/

#include "timer.h"
#include "LoRaMac.h"
#include "LoRaMacTest.h"
#include "lora.h"
#include "app_system.h"
#include "lorawan_info.h"
#include "stm32_mem.h"


typedef struct
{
	lw_state_t otaa;        	/*< ENABLE if over the air activation, DISABLE otherwise */
	lw_state_t duty_cycle;  	/*< ENABLE if dutycyle is on, DISABLE otherwise */
	uint8_t DevEui[8];			/*< Device EUI */
	uint8_t JoinEui[8];			/*< Join Eui */
	uint8_t AppKey[16];			/*< Application Key */
	uint8_t NwkKey[16];			/*< Application Key */
	uint8_t NwkSEncKey[16];		/*< Network Session Key */
	uint8_t AppSKey[16];		/*< Application Session Key */
	uint8_t FNwkSIntKey[16];	/*< Application Session Key */
	uint8_t SNwkSIntKey[16];	/*< Application Session Key */
	int16_t Rssi;				/*< Rssi of the received packet */
	int8_t Snr;					/*< Snr of the received packet */
	uint8_t application_port;	/*< Application port we will receive to */
	LoraConfirm_t ReqAck;		/*< ENABLE if acknowledge is requested */
	int8_t TxDatarate;
}lorawan_configuration_t;

/*!
 * LoRa State Machine states
 */
typedef struct slorawan_param
{
	bool adr_enable;  				/*Activation state of adaptativeDatarate*/
	bool enable_public_network;		/*Enable or disable a public network*/
	int8_t tx_datarate;   			/*Uplink datarate, if AdrEnable is off*/
	uint8_t confirm_retry;    		/*the uplink confirm frame retry times*/
	LoRaMacRegion_t active_region; 	/*region to open*/
	uint32_t dev_addr;
} lw_param_t;

static lorawan_configuration_t lw_config =
{
	.otaa 		 = LORA_ENABLE,
	.duty_cycle  = LORA_ENABLE,
	.DevEui 	 = LORAWAN_DEVICE_EUI,
	.JoinEui 	 = LORAWAN_JOIN_EUI,
	.AppKey 	 = LORAWAN_APP_KEY,
	.NwkKey 	 = LORAWAN_NWK_KEY,
	.NwkSEncKey  = LORAWAN_NWK_S_ENC_KEY,
	.AppSKey 	 = LORAWAN_APP_S_KEY,
	.FNwkSIntKey = LORAWAN_F_NWK_S_INT_KEY,
	.SNwkSIntKey = LORAWAN_S_NWK_S_INT_KEY,
	.Rssi 		 = 0,
	.Snr 		 = 0,
	.ReqAck 	 = LORAWAN_UNCONFIRMED_MSG,
	.TxDatarate  = 0
};


static lw_param_t  lw_param_init = 
{
	.adr_enable = 0,
	.tx_datarate =DR_0,
	.confirm_retry = 2,
	.enable_public_network = LORAWAN_PUBLIC_NETWORK,
	.active_region = LORAWAN_DEFAULT_ACTIVE_REGION,
	.dev_addr = LORAWAN_DEVICE_ADDRESS
};

/*!
 *  Select either Device_Time_req or Beacon_Time_Req following LoRaWAN version
 *  - Device_Time_req   Available for V1.0.3 or later
 *  - Beacon_time_Req   Available for V1.0.2 and before
 */
#define USE_DEVICE_TIMING


#include "LoRaMacTest.h"

/*for classB=============================================================*/
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
/*=============================================================*/

#define HEX16(X)  X[0],X[1], X[2],X[3], X[4],X[5], X[6],X[7],X[8],X[9], X[10],X[11], X[12],X[13], X[14],X[15]
#define HEX8(X)   X[0],X[1], X[2],X[3], X[4],X[5], X[6],X[7]

static MlmeReqJoin_t JoinParameters;




/* Dummy data sent periodically to let the tester respond with start test command*/
static TimerEvent_t TxcertifTimer;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/*for classB=============================================================*/
#if ( LORAMAC_CLASSB_ACTIVE == 1 )
    static LoraErrorStatus lw_beacon_req(void);
    static LoraErrorStatus lw_pingslot_req(void);

    #if defined( USE_DEVICE_TIMING )
        static LoraErrorStatus lw_device_time_req(void);
    #else
        static LoraErrorStatus lw_beacon_time_req(void);
    #endif /* USE_DEVICE_TIMING */
#endif /* LORAMAC_CLASSB_ACTIVE */
/*======================================================================*/



static LoRaMacPrimitives_t LoRaMacPrimitives;
static LoRaMacCallback_t LoRaMacCallbacks;
static MibRequestConfirm_t mibReq;
 

static LoRaMainCallback_t *LoRaMainCallbacks;


/* Private user code ---------------------------------------------------------*/
/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] McpsConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void McpsConfirm(McpsConfirm_t *mcpsConfirm)
{
	lw_app_data_t app_data = {0}; 
	
	if (mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
	{
		switch (mcpsConfirm->McpsRequest)
		{
			case MCPS_UNCONFIRMED:
			case MCPS_CONFIRMED:
			{
				// Check Datarate
				// Check TxPower
				// Check AckReceived
			
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
	app_data.ack =  mcpsConfirm->AckReceived;			 
	LoRaMainCallbacks->app_events(APP_TX_DONE_EVENT,&app_data);
			
}

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void McpsIndication(McpsIndication_t *mcpsIndication)
{
	lw_app_data_t app_data ={0};

	if(mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK)
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

	// Check Buffer
	// Check BufferSize
	// Check Rssi
	// Check Snr
	// Check RxSlot
	if(mcpsIndication->RxData == true)
	{
		app_data.port = mcpsIndication->Port;
		app_data.size = mcpsIndication->BufferSize;
		app_data.buff = mcpsIndication->Buffer;
		app_data.fpending = mcpsIndication->FramePending;
		
		lw_config.Rssi = mcpsIndication->Rssi;
		lw_config.Snr  = mcpsIndication->Snr;
		LoRaMainCallbacks->app_events(APP_RX_EVENT,&app_data);
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
#if (LORAMAC_CLASSB_ACTIVE == 1)
	MibRequestConfirm_t mibReq;
#endif /* LORAMAC_CLASSB_ACTIVE */

	switch (mlmeConfirm->MlmeRequest)
	{
		case MLME_JOIN:
		{
			if(mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
			{
				// Status is OK, node has joined the network
				LoRaMainCallbacks->app_events(APP_JOINED_EVENT,NULL);
				#if ( LORAMAC_CLASSB_ACTIVE == 1 )
					#if defined( USE_DEVICE_TIMING )
						lw_device_time_req();
					#else
						lw_beacon_time_req();
					#endif /* USE_DEVICE_TIMING */
				#endif /* LORAMAC_CLASSB_ACTIVE */
			}
			else
			{
				// Join was not successful. Try to join again
				lw_join();
			}
			break;
		}
	case MLME_LINK_CHECK:
	{
		if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
		{
 
		}
		break;
	}
#if ( LORAMAC_CLASSB_ACTIVE == 1 )
	case MLME_BEACON_ACQUISITION:
	{
		if(mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
		{
			/* Beacon has been acquired */
			/* REquest Server for Ping Slot */
			lw_pingslot_req();
		}
		else
		{
			/* Beacon not acquired */
			/* Search again */
			/* we can check if the MAC has received a time reference for the beacon*/
			/* in this case do either a Device_Time_Req  or a Beacon_Timing_req*/
			lw_beacon_req();
		}
		break;
	}
	case MLME_PING_SLOT_INFO:
	{
		if(mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
		{
			/* class B is now ativated*/
			mibReq.Type = MIB_DEVICE_CLASS;
			mibReq.Param.Class = CLASS_B;
			LoRaMacMibSetRequestConfirm(&mibReq);

			if((lw_param_init.active_region == LORAMAC_REGION_AU915) || (lw_param_init.active_region == LORAMAC_REGION_US915))
			{
				mibReq.Type = MIB_PING_SLOT_DATARATE;
				mibReq.Param.PingSlotDatarate = DR_8;
				LoRaMacMibSetRequestConfirm(&mibReq);
			} 
			/*notify upper layer*/
			/*ClassB*/
			DeviceClass_t  cur_class =CLASS_B;
			LoRaMainCallbacks->app_events(APP_CONFIRM_ClASS_EVENT,&cur_class);
		}
		else
		{
			lw_pingslot_req();
		}
		break;
    }
#if defined( USE_DEVICE_TIMING )
	case MLME_DEVICE_TIME:
    {
		if (mlmeConfirm->Status != LORAMAC_EVENT_INFO_STATUS_OK)
		{
			lw_device_time_req();
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
	lw_app_data_t app_data ={0};
	
#if ( LORAMAC_CLASSB_ACTIVE == 1 )
	MibRequestConfirm_t mibReq;
#endif /* LORAMAC_CLASSB_ACTIVE */

	switch (MlmeIndication->MlmeIndication)
	{
		case MLME_SCHEDULE_UPLINK:
		{
			app_data.fpending = 1;
			// The MAC signals that we shall provide an uplink as soon as possible
			LoRaMainCallbacks->app_events(APP_RX_EVENT,&app_data);
			break;
		}
	#if ( LORAMAC_CLASSB_ACTIVE == 1 )
		case MLME_BEACON_LOST:
		{
			// Switch to class A again
			mibReq.Type = MIB_DEVICE_CLASS;
			mibReq.Param.Class = CLASS_A;
			LoRaMacMibSetRequestConfirm(&mibReq);
			lw_beacon_req();
			break;
		}
		case MLME_BEACON:
		{
			if(MlmeIndication->Status == LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED)
			{
				APP_LOG("BEACON RECEIVED\n\r");
			}
			else
			{
				APP_LOG("BEACON NOT RECEIVED\n\r");
			}
			break;
		}
	#endif /* LORAMAC_CLASSB_ACTIVE */
		default:
		break;
	}
}
/**
 *  lorawan init
 */
void lw_init(LoRaMainCallback_t *callbacks)
{
	LoRaMacInfo_t *LoRaMacRegionInfo;

	LoraMacInfo_Init();
	LoRaMacRegionInfo = LoraMacInfo_GetPtr();
	if(LoRaMacRegionInfo->Region == 0)
	{
		APP_PRINTF("error: At leats one region shall be defined in the MW: check lorawan_conf.h \n\r");
		while(1) {} /* At leats one region shall be defined in MW */
	}

	#if ( LORAMAC_CLASSB_ACTIVE == 1 )
	if(LoRaMacRegionInfo->ClassB != 1)
	{
		APP_PRINTF("error: ClassB is not defined in the MW: set lorawan_conf.h accordingly\n\r");
		while(1) {}   /* error: ClassB is not defined in the MW*/
	}
	#elif ( LORAMAC_CLASSB_ACTIVE == 0 )
	if(LoRaMacRegionInfo->ClassB != 0)
	{
		APP_PRINTF("error: missmatch between applic and MW CLASSB defines: set lorawan_conf.h according to app_conf.h\n\r");
		while(1) {}   /* error: ClassB should not be defined in the MW*/
	}
	#endif /* LORAMAC_CLASSB_ACTIVE */

	/* init the main call backs*/
	LoRaMainCallbacks = callbacks;

	#if (STATIC_DEVICE_EUI != 1)
		LoRaMainCallbacks->BoardGetUniqueId(lw_config.DevEui);
	#endif

	#if (STATIC_DEVICE_ADDRESS != 1)
		// Choose a "pseudo-random" device address
		lw_param_init.dev_addr =  LoRaMainCallbacks->BoardGetRandomSeed();
	#endif

	LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
	LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
	LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
	LoRaMacPrimitives.MacMlmeIndication = MlmeIndication;
	LoRaMacCallbacks.GetBatteryLevel = LoRaMainCallbacks->BoardGetBatteryLevel;
	LoRaMacCallbacks.GetTemperatureLevel = LoRaMainCallbacks->BoardGetTemperatureLevel;
	LoRaMacCallbacks.MacProcessNotify = LoRaMainCallbacks->MacProcessNotify;

	if(0U != ((1<<(lw_param_init.active_region))& (LoRaMacRegionInfo->Region)))
	{
		LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, lw_param_init.active_region);
	}
	else
	{
		APP_PRINTF("error: Region is not defined in the MW: set lorawan_conf.h accordingly\n\r");
		while(1) {}   /* error: Region is not defined in the MW */
	}
	if((lw_param_init.active_region == LORAMAC_REGION_CN470)||(lw_param_init.active_region == LORAMAC_REGION_CN470ALI)) 
	{   
		uint16_t channelMask[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
		if(lw_param_init.active_region == LORAMAC_REGION_CN470){
			channelMask[0] = 0x00FF;
		}
		else{
			channelMask[0] = 0xFF00;
		}
		mibReq.Type = MIB_CHANNELS_MASK;
		mibReq.Param.ChannelsMask = channelMask;
		LoRaMacMibSetRequestConfirm(&mibReq);
		mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
		mibReq.Param.ChannelsDefaultMask = channelMask;
		LoRaMacMibSetRequestConfirm(&mibReq);
	}

	lw_config_otaa_set(lw_config.otaa);

	mibReq.Type = MIB_DEV_EUI;
	mibReq.Param.DevEui = lw_config.DevEui;
	LoRaMacMibSetRequestConfirm(&mibReq);

	mibReq.Type = MIB_JOIN_EUI;
	mibReq.Param.JoinEui = lw_config.JoinEui;
	LoRaMacMibSetRequestConfirm(&mibReq);

	mibReq.Type = MIB_ADR;
	mibReq.Param.AdrEnable = lw_param_init.adr_enable;
	LoRaMacMibSetRequestConfirm(&mibReq);

	mibReq.Type = MIB_PUBLIC_NETWORK;
	mibReq.Param.EnablePublicNetwork = lw_param_init.enable_public_network;
	LoRaMacMibSetRequestConfirm(&mibReq);

	mibReq.Type = MIB_APP_KEY;
	mibReq.Param.AppKey = lw_config.AppKey;
	LoRaMacMibSetRequestConfirm(&mibReq);

	mibReq.Type = MIB_NWK_KEY;
	mibReq.Param.NwkKey = lw_config.NwkKey;
	LoRaMacMibSetRequestConfirm(&mibReq);

	mibReq.Type = MIB_DEVICE_CLASS;
	mibReq.Param.Class = CLASS_A;
	LoRaMacMibSetRequestConfirm(&mibReq);

	if((lw_param_init.active_region == LORAMAC_REGION_EU868) 
	|| (lw_param_init.active_region == LORAMAC_REGION_RU864)
	|| (lw_param_init.active_region == LORAMAC_REGION_CN779)
	|| (lw_param_init.active_region == LORAMAC_REGION_EU433))
	{
		LoRaMacTestSetDutyCycleOn(true);
		lw_config.duty_cycle = LORA_ENABLE;
	}
	else
	{
		lw_config.duty_cycle = LORA_DISABLE;
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

LoRaMacStatus_t lw_join(void)
{
	LoRaMacStatus_t status;
    
	if (lw_config.otaa == LORA_ENABLE)
	{
		MlmeReq_t mlmeReq;

		mlmeReq.Type = MLME_JOIN;
		mlmeReq.Req.Join.Datarate = lw_param_init.tx_datarate;

		JoinParameters = mlmeReq.Req.Join;
		status = LoRaMacMlmeRequest(&mlmeReq);
	}
	else
	{
		/*no Join in abp*/
		status = LORAMAC_STATUS_BUSY;
	}

	return status;
}

LoraFlagStatus lw_join_status(void)
{
	MibRequestConfirm_t mibReq;

	mibReq.Type = MIB_NETWORK_ACTIVATION;

	LoRaMacMibGetRequestConfirm(&mibReq);

	if(mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE)
	{
		return LORA_RESET;
	}
	else
	{
		return LORA_SET;
	}
}

LoRaMacStatus_t lw_current_chmask_get(LWChannel_mask_t* ch)
{
	mibReq.Type = MIB_CHANNELS_MASK;
	mibReq.Param.ChannelsMask = (uint16_t*)ch;
	LoRaMacStatus_t sta = LoRaMacMibGetRequestConfirm(&mibReq);

	if(LORAMAC_STATUS_OK == sta)
	{
		for(uint8_t i=0;i<6;i++)
		{
			ch->mask[i] = mibReq.Param.ChannelsMask[i];
		}
	}
	return sta;
}
LoRaMacStatus_t lw_current_chmask_set(const LWChannel_mask_t* ch)
{
	static bool only;
	
	if(only == false)
	{
		only = true;
		mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
		mibReq.Param.ChannelsDefaultMask = (uint16_t*)ch;
		LoRaMacMibSetRequestConfirm(&mibReq);
	}
	
    mibReq.Type = MIB_CHANNELS_MASK;
    mibReq.Param.ChannelsMask = (uint16_t*)ch;
    return  LoRaMacMibSetRequestConfirm(&mibReq);
}
LoRaMacStatus_t lw_rxwin2_ch_set(const RxChannelParams_t* rx2_ch)
{
	LoRaMacStatus_t sta;
	
	mibReq.Type = MIB_RX2_CHANNEL;
	mibReq.Param.Rx2Channel.Frequency = rx2_ch->Frequency;
	mibReq.Param.Rx2Channel.Datarate  = rx2_ch->Datarate;
	
	sta = LoRaMacMibSetRequestConfirm(&mibReq);
	return sta;
}

LoRaMacStatus_t lw_rxwin2_ch_get(RxChannelParams_t* rx2_ch)
{
	LoRaMacStatus_t sta;
	mibReq.Type = MIB_RX2_CHANNEL;
	
	sta = LoRaMacMibGetRequestConfirm(&mibReq);
	
	if(LORAMAC_STATUS_OK == sta)
	{
		*rx2_ch = mibReq.Param.Rx2Channel;
	}
	return sta;
 }

 
LoRaMacStatus_t lw_adr_set(lw_state_t sta)
{
	LoRaMacStatus_t n_sta=0;
	
	mibReq.Type = MIB_ADR;
	mibReq.Param.AdrEnable = sta ;
	n_sta = LoRaMacMibSetRequestConfirm(&mibReq); 
	if(LORAMAC_STATUS_OK == n_sta){
		lw_param_init.adr_enable = sta;
	}
	return n_sta;
}


LoRaMacStatus_t lw_send(lw_app_data_t *app_data, LoraConfirm_t IsTxConfirmed)
{
	McpsReq_t mcpsReq;
	LoRaMacTxInfo_t txInfo;

	if(LoRaMacQueryTxPossible(app_data->size, &txInfo) != LORAMAC_STATUS_OK)
	{
		// Send empty frame in order to flush MAC commands
		mcpsReq.Type = MCPS_UNCONFIRMED;
		mcpsReq.Req.Unconfirmed.fBuffer = NULL;
		mcpsReq.Req.Unconfirmed.fBufferSize = 0;
		mcpsReq.Req.Unconfirmed.Datarate = lw_config_tx_datarate_get() ;
	}
	else
	{
		if (IsTxConfirmed == LORAWAN_UNCONFIRMED_MSG)
		{
			mcpsReq.Type = MCPS_UNCONFIRMED;
			mcpsReq.Req.Unconfirmed.fPort = app_data->port;
			mcpsReq.Req.Unconfirmed.fBufferSize = app_data->size;
			mcpsReq.Req.Unconfirmed.fBuffer = app_data->buff;
			mcpsReq.Req.Unconfirmed.Datarate = lw_config_tx_datarate_get() ;
		}
		else
		{
			mcpsReq.Type = MCPS_CONFIRMED;
			mcpsReq.Req.Confirmed.fPort = app_data->port;
			mcpsReq.Req.Confirmed.fBufferSize = app_data->size;
			mcpsReq.Req.Confirmed.fBuffer = app_data->buff;
			mcpsReq.Req.Confirmed.NbTrials = lw_confirm_retry_get();
			mcpsReq.Req.Confirmed.Datarate = lw_config_tx_datarate_get() ;
		}
	}
	return LoRaMacMcpsRequest(&mcpsReq);
 
}

#if ( LORAMAC_CLASSB_ACTIVE == 1 )
	#if defined( USE_DEVICE_TIMING )
		static LoRaMacStatus_t lw_device_time_req(void)
		{
			MlmeReq_t mlmeReq;

			mlmeReq.Type = MLME_DEVICE_TIME;

			return LoRaMacMlmeRequest(&mlmeReq);
		}
	#else
		static LoRaMacStatus_t lw_beacon_time_req(void)
		{
			MlmeReq_t mlmeReq;

			mlmeReq.Type = MLME_BEACON_TIMING;

			return LoRaMacMlmeRequest(&mlmeReq);
		}
	#endif /* USE_DEVICE_TIMING */

	static LoRaMacStatus_t lw_beacon_req(void)
	{
		MlmeReq_t mlmeReq;

		mlmeReq.Type = MLME_BEACON_ACQUISITION;

		return LoRaMacMlmeRequest(&mlmeReq);
	}

	static LoRaMacStatus_t lw_pingslot_req(void)
	{
		MlmeReq_t mlmeReq;

		mlmeReq.Type = MLME_LINK_CHECK;
		LoRaMacMlmeRequest(&mlmeReq);

		mlmeReq.Type = MLME_PING_SLOT_INFO;
		mlmeReq.Req.PingSlotInfo.PingSlot.Fields.Periodicity = DefaultPingSlotPeriodicity;
		mlmeReq.Req.PingSlotInfo.PingSlot.Fields.RFU = 0;

		return LoRaMacMlmeRequest(&mlmeReq);
	}
#endif /* LORAMAC_CLASSB_ACTIVE */

LoRaMacStatus_t lw_request_class(DeviceClass_t newClass)
{
	LoRaMacStatus_t Errorstatus = LORAMAC_STATUS_OK;
    
	MibRequestConfirm_t mibReq;
	DeviceClass_t currentClass;

	mibReq.Type = MIB_DEVICE_CLASS;
	LoRaMacMibGetRequestConfirm(&mibReq);

	currentClass = mibReq.Param.Class;
	/*attempt to swicth only if class update*/
	if(currentClass != newClass)
	{
		switch(newClass)
		{
			case CLASS_A:
			{
				mibReq.Param.Class = CLASS_A;

                Errorstatus = LoRaMacMibSetRequestConfirm(&mibReq);
				if(Errorstatus == LORAMAC_STATUS_OK)
				{
					DeviceClass_t  cur_class =CLASS_A;
					LoRaMainCallbacks->app_events(APP_CONFIRM_ClASS_EVENT,&cur_class);
				}
			break;
			}
			case CLASS_B:
			{
				#if (LORAMAC_CLASSB_ACTIVE == 1)
					if(currentClass != CLASS_A)
					{
						Errorstatus = LORAMAC_STATUS_ERROR;
					}
					/*switch is not instantanuous*/
					Errorstatus = lw_beacon_req();
				#else
					APP_PRINTF("warning: LORAMAC_CLASSB_ACTIVE has not been defined at compilation\n\r");
				#endif /* LORAMAC_CLASSB_ACTIVE */
				break;
			}
			case CLASS_C:
			{
				if (currentClass != CLASS_A)
				{
					Errorstatus = LORAMAC_STATUS_ERROR;
				}
				/*switch is instantanuous*/
				mibReq.Param.Class = CLASS_C;

                Errorstatus = LoRaMacMibSetRequestConfirm(&mibReq);
                
				if(Errorstatus == LORAMAC_STATUS_OK)
				{
					DeviceClass_t  cur_class =CLASS_C;
					LoRaMainCallbacks->app_events(APP_CONFIRM_ClASS_EVENT,&cur_class);
				}
				break;
			}
			default:
			break;
		}
	}
	return Errorstatus;
}

DeviceClass_t lw_get_current_class(void)
{
	MibRequestConfirm_t mibReq;

	mibReq.Type = MIB_DEVICE_CLASS;
	LoRaMacMibGetRequestConfirm(&mibReq);

	return mibReq.Param.Class;
}

void lw_config_otaa_set(lw_state_t otaa)
{
	lw_config.otaa = otaa;

	if (lw_config.otaa == LORA_ENABLE)
	{
		APP_PPRINTF("OTAA Mode enabled\n\r");
		APP_PPRINTF("DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n\r", HEX8(lw_config.DevEui));
		APP_PPRINTF("JoinEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n\r", HEX8(lw_config.JoinEui));
		APP_PPRINTF("AppKey= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n\r", HEX16(lw_config.AppKey));

		mibReq.Type = MIB_NETWORK_ACTIVATION;
		mibReq.Param.NetworkActivation = ACTIVATION_TYPE_NONE;
		LoRaMacMibSetRequestConfirm(&mibReq);
	}
	else
	{
		APP_PPRINTF("ABP Mode enabled\n\r");
		APP_PPRINTF("DevEui= %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n\r", HEX8(lw_config.DevEui));
		APP_PPRINTF("DevAdd=  %08X\n\r", lw_param_init.dev_addr) ;
		APP_PPRINTF("NwkSKey= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n\r", HEX16(lw_config.NwkSEncKey));
		APP_PPRINTF("AppSKey= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n\r", HEX16(lw_config.AppSKey));

		mibReq.Type = MIB_NET_ID;
		mibReq.Param.NetID = LORAWAN_NETWORK_ID;
		LoRaMacMibSetRequestConfirm(&mibReq);

		mibReq.Type = MIB_DEV_ADDR;
		mibReq.Param.DevAddr = lw_param_init.dev_addr;
		LoRaMacMibSetRequestConfirm(&mibReq);

		mibReq.Type = MIB_F_NWK_S_INT_KEY;
		mibReq.Param.FNwkSIntKey = lw_config.FNwkSIntKey;
		LoRaMacMibSetRequestConfirm(&mibReq);

		mibReq.Type = MIB_S_NWK_S_INT_KEY;
		mibReq.Param.SNwkSIntKey = lw_config.SNwkSIntKey;
		LoRaMacMibSetRequestConfirm(&mibReq);

		mibReq.Type = MIB_NWK_S_ENC_KEY;
		mibReq.Param.NwkSEncKey = lw_config.NwkSEncKey;
		LoRaMacMibSetRequestConfirm(&mibReq);

		mibReq.Type = MIB_APP_S_KEY;
		mibReq.Param.AppSKey = lw_config.AppSKey;
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

lw_state_t lw_config_otaa_get(void)
{
	return lw_config.otaa;
}


void lw_confirm_retry_set(uint8_t retry)
{
	lw_param_init.confirm_retry = retry;
}
    

uint8_t lw_confirm_retry_get(void)
{
	return lw_param_init.confirm_retry;
}


void lw_config_duty_cycle_set(lw_state_t duty_cycle)
{
	lw_config.duty_cycle = duty_cycle;
	LoRaMacTestSetDutyCycleOn((duty_cycle == LORA_ENABLE) ? 1 : 0);
}

lw_state_t lw_config_duty_cycle_get(void)
{
	return lw_config.duty_cycle;
}

uint8_t *lw_config_deveui_get(void)
{
	return lw_config.DevEui;
}

uint8_t *lw_config_joineui_get(void)
{
	return lw_config.JoinEui;
}

void lw_config_joineui_set(uint8_t joineui[8])
{
	UTIL_MEM_cpy_8(lw_config.JoinEui, joineui, sizeof(lw_config.JoinEui));

	mibReq.Type = MIB_JOIN_EUI;
	mibReq.Param.JoinEui = lw_config.JoinEui;
	LoRaMacMibSetRequestConfirm(&mibReq);
}

uint8_t *lw_config_appkey_get(void)
{
	return lw_config.AppKey;
}

void lw_config_appkey_set(uint8_t appkey[16])
{
	UTIL_MEM_cpy_8(lw_config.AppKey, appkey, sizeof(lw_config.AppKey));
	UTIL_MEM_cpy_8(lw_config.NwkKey, appkey, sizeof(lw_config.NwkKey));

	mibReq.Type = MIB_APP_KEY;
	mibReq.Param.AppKey = lw_config.AppKey;
	LoRaMacMibSetRequestConfirm(&mibReq);

	mibReq.Type = MIB_NWK_KEY;
	mibReq.Param.NwkKey = lw_config.NwkKey;
	LoRaMacMibSetRequestConfirm(&mibReq);
}

void lw_config_reqack_set(LoraConfirm_t reqack)
{
	lw_config.ReqAck = reqack;
}

LoraConfirm_t lw_config_reqack_get(void)
{
	return lw_config.ReqAck;
}

int8_t lw_config_snr_get(void)
{
	return lw_config.Snr;
}

int16_t lw_config_rssi_get(void)
{
	return lw_config.Rssi;
}

void lw_config_tx_datarate_set(int8_t TxDataRate)
{
	lw_config.TxDatarate = TxDataRate;
}

int8_t lw_config_tx_datarate_get(void)
{
	return lw_config.TxDatarate;
}


LoRaMacRegion_t lora_region_get(void)
{
	return lw_param_init.active_region;
}
#if ( LORAMAC_CLASSB_ACTIVE == 1 )
	LoRaMacClassBNvmCtx_t* lw_get_classBCtx(void)
	{
		mibReq.Type = MIB_NVM_CTXS;
		LoRaMacMibGetRequestConfirm( &mibReq );
		return ( LoRaMacClassBNvmCtx_t*) mibReq.Param.Contexts->ClassBNvmCtx;
	}

	uint8_t lw_get_pingslot_periodicity(void)
	{
		return DefaultPingSlotPeriodicity;
	}

	void lw_set_pingslot_periodicity(uint8_t PingSlotPeriodicity)
	{
		DefaultPingSlotPeriodicity= PingSlotPeriodicity;
	}
#endif  /* LORAMAC_CLASSB_ACTIVE */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

