#include "LoRaApi.h"

#if( OVER_THE_AIR_ACTIVATION == 0 )
uint8_t DevEui[] = { 0x38, 0x37, 0x37, 0x37, 0x71, 0x37, 0x68, 0x07 };
uint8_t JoinEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t AppKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
uint8_t NwkKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
uint32_t DevAddr = 0x008B6CCB;
uint8_t FNwkSIntKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
uint8_t SNwkSIntKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
uint8_t NwkSEncKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
uint8_t AppSKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
#else
uint8_t DevEui[] = { 0x47, 0x9B, 0x41, 0xF2, 0x00, 0x45, 0x00, 0x28 };
uint8_t JoinEui[] = { 0x52, 0x69, 0x73, 0x69, 0x6E, 0x67, 0x48, 0x46 };
uint8_t AppKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
uint8_t NwkKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
uint32_t DevAddr = 0x00ADE921;
uint8_t FNwkSIntKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
uint8_t SNwkSIntKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
uint8_t NwkSEncKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
uint8_t AppSKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
#endif

bool NextTx = true;
uint8_t IsTxConfirmed = false;
TimerEvent_t TxNextPacketTimer;
uint8_t AppPort = 2;
uint8_t AppDataSize = 13;
uint8_t AppDataBuffer[LORAWAN_APP_DATA_MAX_SIZE];
LoRaMacPrimitives_t LoRaMacPrimitives;
LoRaMacCallback_t LoRaMacCallbacks;
MibRequestConfirm_t mibReq;
LoRaMacStatus_t status;

LoRaMacStatus_t SendFrame( void )
{
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;
    
    if( LoRaMacQueryTxPossible( AppDataSize, &txInfo ) != LORAMAC_STATUS_OK )
    {
        // Send empty frame in order to flush MAC commands
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
        mcpsReq.Req.Unconfirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
    }
    else
    {
			if( IsTxConfirmed == false )
        {
            mcpsReq.Type = MCPS_UNCONFIRMED;
            mcpsReq.Req.Unconfirmed.fPort = AppPort;
            mcpsReq.Req.Unconfirmed.fBuffer = AppDataBuffer;
            mcpsReq.Req.Unconfirmed.fBufferSize = AppDataSize;
            mcpsReq.Req.Unconfirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
        }
        else
        {
            mcpsReq.Type = MCPS_CONFIRMED;
            mcpsReq.Req.Confirmed.fPort = AppPort;
            mcpsReq.Req.Confirmed.fBuffer = AppDataBuffer;
            mcpsReq.Req.Confirmed.fBufferSize = AppDataSize;
            mcpsReq.Req.Confirmed.NbTrials = 8;
            mcpsReq.Req.Confirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
        }
    }

    /*发送数据*/
    return LoRaMacMcpsRequest( &mcpsReq);
}


static void McpsConfirm( McpsConfirm_t *mcpsConfirm )
{
    if( mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
    {
        switch( mcpsConfirm->McpsRequest )
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
            default:
                break;
        }
    }
}
static void OnTxNextPacketTimerEvent( void* context)
{
	MibRequestConfirm_t mibReq;
	LoRaMacStatus_t status;
	TimerStop( &TxNextPacketTimer );
	mibReq.Type = MIB_NETWORK_ACTIVATION;
	status = LoRaMacMibGetRequestConfirm( &mibReq );

	if( status == LORAMAC_STATUS_OK )
	{
			if( mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE )
			{
					// Network not joined yet. Try to join again
					LoRa_JoinNetwork(1);
			}
			else
			{
					NextTx = true;
			}
	}
	NextTx = true;
}

static void McpsIndication( McpsIndication_t *mcpsIndication )
{
    if( mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
        return;
    }

    switch( mcpsIndication->McpsIndication )
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
    if( mcpsIndication->FramePending == true )
    {
        // The server signals that it has pending data to be sent.
        // We schedule an uplink as soon as possible to flush the server.
        OnTxNextPacketTimerEvent( NULL );
    }
    if( mcpsIndication->RxData == true )
    {
    }
}

static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
		switch( mlmeConfirm->MlmeRequest )
		{
				case MLME_JOIN:
				{
					if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
					{
							MibRequestConfirm_t mibGet;
							printf( "###### ===== JOINED ==== ######\r\n" );
							printf( "\r\nOTAA\r\n\r\n" );

							mibGet.Type = MIB_DEV_ADDR;
							LoRaMacMibGetRequestConfirm( &mibGet );
							printf( "DevAddr     : %08X\r\n", mibGet.Param.DevAddr );

							printf( "\n\r\n" );
							mibGet.Type = MIB_CHANNELS_DATARATE;
							LoRaMacMibGetRequestConfirm( &mibGet );
							printf( "DATA RATE   : DR_%d\r\n", mibGet.Param.ChannelsDatarate );
							printf( "\r\n" );
							// Status is OK, node has joined the network
							NextTx = true;
					}
					else
					{
							// Join was not successful. Try to join again
							LoRa_JoinNetwork(1);
					}
					break;
				}
				case MLME_LINK_CHECK:
				{
						break;
				}
				default:
						break;
		}

}


/*!
 * \brief   MLME-Indication event function
 *
 * \param   [IN] mlmeIndication - Pointer to the indication structure.
 */
static void MlmeIndication( MlmeIndication_t *mlmeIndication )
{
    if( mlmeIndication->Status != LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED )
    {
        printf( "\r\n###### ===== MLME-Indication ==== ######\r\n" );
    }
    if( mlmeIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
    }
    switch( mlmeIndication->MlmeIndication )
    {
        case MLME_SCHEDULE_UPLINK:
        {// The MAC signals that we shall provide an uplink as soon as possible
            OnTxNextPacketTimerEvent( NULL );
            break;
        }
        default:
            break;
    }
}
void OnMacProcessNotify( void )
{
	
}

void LoRa_Init(void)
{
		LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
		LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
		LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
		LoRaMacPrimitives.MacMlmeIndication = MlmeIndication;	

		LoRaMacCallbacks.GetBatteryLevel = BoardGetBatteryLevel;
		LoRaMacCallbacks.GetTemperatureLevel = NULL;
		LoRaMacCallbacks.NvmContextChange = NvmCtxMgmtEvent;
		LoRaMacCallbacks.MacProcessNotify = OnMacProcessNotify;
		LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, ACTIVE_REGION );

		mibReq.Type = MIB_APP_KEY;
		mibReq.Param.AppKey = AppKey;
		LoRaMacMibSetRequestConfirm( &mibReq );
		mibReq.Type = MIB_NWK_KEY;
		mibReq.Param.NwkKey = NwkKey;
		LoRaMacMibSetRequestConfirm( &mibReq );

#if( OVER_THE_AIR_ACTIVATION == 0 )
		mibReq.Type = MIB_NET_ID;
		mibReq.Param.NetID = LORAWAN_NETWORK_ID;
		LoRaMacMibSetRequestConfirm( &mibReq );

		mibReq.Type = MIB_DEV_ADDR;
		mibReq.Param.DevAddr = DevAddr;
		LoRaMacMibSetRequestConfirm( &mibReq );

		mibReq.Type = MIB_F_NWK_S_INT_KEY;
		mibReq.Param.FNwkSIntKey = FNwkSIntKey;
		LoRaMacMibSetRequestConfirm( &mibReq );

		mibReq.Type = MIB_S_NWK_S_INT_KEY;
		mibReq.Param.SNwkSIntKey = SNwkSIntKey;
		LoRaMacMibSetRequestConfirm( &mibReq );

		mibReq.Type = MIB_NWK_S_ENC_KEY;
		mibReq.Param.NwkSEncKey = NwkSEncKey;
		LoRaMacMibSetRequestConfirm( &mibReq );

		mibReq.Type = MIB_APP_S_KEY;
		mibReq.Param.AppSKey = AppSKey;
		LoRaMacMibSetRequestConfirm( &mibReq );
#endif

		LoRaMacStart( );
		mibReq.Type = MIB_NETWORK_ACTIVATION;
		status = LoRaMacMibGetRequestConfirm( &mibReq );
		if( status == LORAMAC_STATUS_OK )
		{
			if( mibReq.Param.NetworkActivation != ACTIVATION_TYPE_NONE )
			{
					NextTx = true;
			}
		}
#if 0
#if( OVER_THE_AIR_ACTIVATION == 0 )
		LoRa_JoinNetwork(0);
#else
		LoRa_JoinNetwork(1);
#endif
		TimerInit( &TxNextPacketTimer, OnTxNextPacketTimerEvent );	
		printf_device_info();
		while( 1 )
    {
			char *data = "Welcome to Tos Tiny!";
			LoRaMacProcess( );
			
			Lora_Send(0,2,(uint8_t *)data,strlen(data));
			//PrepareTxFrame();
			//SendFrame();
			NextTx = false;
			TimerSetValue( &TxNextPacketTimer, 15000 );
			TimerStart( &TxNextPacketTimer );
			while(!NextTx);
			//tos_sleep_hmsm(0,0,1,400); //延时1.4s
    }
#endif 
}

/*lorawan入网操作*/
void LoRa_JoinNetwork(uint8_t jointype)
{
    /*OTAA模式*/
    if(jointype > 0)
    {
		MlmeReq_t mlmeReq;
		mlmeReq.Type = MLME_JOIN;
		mlmeReq.Req.Join.DevEui = DevEui;
		mlmeReq.Req.Join.JoinEui = JoinEui;
		mlmeReq.Req.Join.Datarate = LORAWAN_DEFAULT_DATARATE;
		LoRaMacMlmeRequest( &mlmeReq );
    }
    else
    {
        /*设置入网状态*/
			MibRequestConfirm_t mibReq;
			mibReq.Type = MIB_ABP_LORAWAN_VERSION;
			mibReq.Param.AbpLrWanVersion.Value = ABP_ACTIVATION_LRWAN_VERSION;
			LoRaMacMibSetRequestConfirm( &mibReq );

			mibReq.Type = MIB_NETWORK_ACTIVATION;
			mibReq.Param.NetworkActivation = ACTIVATION_TYPE_ABP;
			LoRaMacMibSetRequestConfirm( &mibReq );
    }
}

/*lorawan发送*/
uint8_t Lora_Send(uint8_t type,uint8_t port, uint8_t * psrc, uint16_t len)
{
    /*提取发送报文类型*/
    if(type == 1)
    {
        IsTxConfirmed = true;
    }
    else
    {
        IsTxConfirmed = false;
    }
    /*提取端口*/
    if(port <1 && port > 233)
    {
        return 1;
    }
    else
    {
        /*更新端口*/
        AppPort = port;
    }
    /*提取数据*/
    if(len > LORAWAN_APP_DATA_MAX_SIZE)
    {
        return 1;
    }
    memcpy1(AppDataBuffer, psrc, len);
    AppDataSize = len;

    /*发送数据*/
		//printf("begin to send data\r\n");
    LoRaMacStatus_t res = SendFrame( );
    if(res == LORAMAC_STATUS_OK)
    {
				printf("SEND OK\r\n");
        return 0;
    }
    else if(res == LORAMAC_STATUS_NO_NETWORK_JOINED)
    {
				printf("NO NETWORK JOINED\r\n");
        return 2;
    }
    else if(res == LORAMAC_STATUS_BUSY)
    {
				printf("LORAMAC STATUS BUSY\r\n");
        return 3;
    }
    else
    {
        return 1;
    }
}

void printf_device_info(void)
{
                printf( "DevEui      : %02X", DevEui[0] );
                for( int i = 1; i < 8; i++ )
                {
                    printf( "-%02X", DevEui[i] );
                }
                printf( "\r\n" );
                printf( "AppEui      : %02X", JoinEui[0] );
                for( int i = 1; i < 8; i++ )
                {
                    printf( "-%02X", JoinEui[i] );
                }
                printf( "\r\n" );
                printf( "AppKey      : %02X", NwkKey[0] );
                for( int i = 1; i < 16; i++ )
                {
                    printf( " %02X", NwkKey[i] );
                }
                printf( "\n\r\n" );
#if( OVER_THE_AIR_ACTIVATION == 0 )
                printf( "###### ===== JOINED ==== ######\r\n" );
                printf( "\r\nABP\r\n\r\n" );
                printf( "DevAddr     : %08X\r\n", DevAddr );
                printf( "NwkSKey     : %02X", FNwkSIntKey[0] );
                for( int i = 1; i < 16; i++ )
                {
                    printf( " %02X", FNwkSIntKey[i] );
                }
                printf( "\r\n" );
                printf( "AppSKey     : %02X", AppSKey[0] );
                for( int i = 1; i < 16; i++ )
                {
                    printf( " %02X", AppSKey[i] );
                }
                printf( "\n\r\n" );
#endif								
}
