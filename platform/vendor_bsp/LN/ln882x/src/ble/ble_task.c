#include <string.h>
#include "proj_config.h"
#include "ln88xx.h"
#include "utils/debug/art_assert.h"
#include "utils/debug/log.h"
#include "osal/osal.h"
#include "console/console.h"

#if BLE_SUPPORT == ENABLE
#include "driver/ll/ll_syscon.h"
#include "driver/ll/ll_spim.h"
#include "driver/hal/hal_spim.h"
#include "driver/hal/spim.h"
#include "driver/hal/hal_common.h"
#include "driver/hal/hal_gpio.h"
#include "wifi/wifi.h"

#include "ble/ble_task.h"
#include "ble/mg_api.h"

OS_Queue_t g_ble_msg_queue;
OS_Thread_t g_ble_thread;
static __IO uint32_t bleTick;

static bool ble_wifi_cfg=false;

unsigned char *ble_mac_addr;
static unsigned char gConnectedFlag=0;
u16 cur_notifyhandle = 0x16;  //Note: make sure each notify handle by invoking function: set_notifyhandle(hd);
unsigned int StandbyTimeout = 0; 

#define SOFTWARE_INFO "SV1.0.0"
#define MANU_INFO     "LN_Bluetooth"
char DeviceInfo[11] =  "LN_WIFI";  /*max len is 24 bytes*/

#define BLE_TEST DISABLE

#define USE_SHORT_UUID  1

#define ATT_UUID_WIFI	        0xCDE7  //wifi primary service uuid
#define ATT_UUID_WIFI_APINFO    0xCDC7
#define ATT_UUID_WIFI_STATE     0xCDC8
#define ATT_UUID_WIFI_INFO      0xCDC9

#define ATT_APINFO_MAX_SIZE     SSID_MAX_LEN+PASSWORD_MAX_LEN+3 // 2 Bytes header and 1 more Bytes end;
#define ATT_WIFI_STATE_SIZE     20
#define ATT_WIFI_INFO_SIZE      32
#define ATT_WIFI_ADV_SIZE       31

static char wifi_ap_info[ATT_APINFO_MAX_SIZE] = {0};//max ssid 33B, max PW 64B, use the first Byte save length of ssid, second byte save length of passwd 
//static u8 wifi_state[ATT_WIFI_STATE_SIZE] = {0};
//static u8 wifi_info[ATT_WIFI_INFO_SIZE] = {0};
//static u8 ssid_len=0;
//static u8 pwd_len=0;
static u8 info_offset=0;

#if BLE_TEST==ENABLE
static u32 test_count=0;
#endif

u8* getDeviceInfoData(u8* len)
{
    *len = sizeof(DeviceInfo);
    return (u8*)DeviceInfo;
}

void updateDeviceInfoData(u8* name, u8 len)
{
    memcpy(DeviceInfo,name, len);
    
    ble_set_name(name,len);
}


/**********************************************************************************
                 *****DataBase****
01 - 06  GAP (Primary service) 0x1800
03:04  name
07 - 0f  Device Info (Primary service) 0x180a
0a:0b  firmware version
0e:0f  software version
10 - 19  LED service (Primary service) 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
  11:12  6E400003-B5A3-F393-E0A9-E50E24DCCA9E(0x04)  RxNotify
  13     cfg
  14:15  6E400002-B5A3-F393-E0A9-E50E24DCCA9E(0x0C)  Tx
  16     cfg
  17:18  6E400004-B5A3-F393-E0A9-E50E24DCCA9E(0x0A)  BaudRate
  19     0x2901  info
************************************************************************************/
//
///STEP0:Character declare
//
const BLE_CHAR AttCharList[] = 
{
    // ======  gatt =====  Do NOT Change!!
    {TYPE_CHAR, 0x03, {ATT_CHAR_PROP_RD, 0x04,0,0x00,0x2a},UUID16_FORMAT},//name
    //05-06 reserved
    
    // ======  device info =====    Do NOT Change if using the default!!!  
    {TYPE_CHAR, 0x08, {ATT_CHAR_PROP_RD, 0x09,0,0x29,0x2a},UUID16_FORMAT},//manufacture
    {TYPE_CHAR, 0x0a, {ATT_CHAR_PROP_RD, 0x0b,0,0x26,0x2a},UUID16_FORMAT},//firmware version
    {TYPE_CHAR, 0x0e, {ATT_CHAR_PROP_RD, 0x0f,0,0x28,0x2a},UUID16_FORMAT},//sw version
    
    // ======  User service or other services added here =====  User defined  
#if USE_SHORT_UUID==1
    {TYPE_CHAR, 0x11, {ATT_CHAR_PROP_W|ATT_CHAR_PROP_RD, 0x12,0, (ATT_UUID_WIFI_APINFO&0xFF), (ATT_UUID_WIFI_APINFO>>8)&0xFF}, UUID16_FORMAT},//wifi ap info
    {TYPE_CHAR, 0x13, {ATT_CHAR_PROP_RD, 0x14,0, (ATT_UUID_WIFI_INFO&0xFF), (ATT_UUID_WIFI_INFO>>8)&0xFF}, UUID16_FORMAT},   //wifi info
    
    {TYPE_CHAR, 0x15, {ATT_CHAR_PROP_RD|ATT_CHAR_PROP_NTF, 0x16,0, (ATT_UUID_WIFI_STATE&0xFF),(ATT_UUID_WIFI_STATE>>8)&0xFF}, UUID16_FORMAT},  //wifi state
    {TYPE_CFG,  0x17,  {ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W}},//client char configuration,  bit 0: notification enable/disable, bit1: indications enable/disable
#else
    {TYPE_CHAR, 0x11, {ATT_CHAR_PROP_W|ATT_CHAR_PROP_RD, 0x12,0, 0,0},                                       1},//wifi ap info  //
    {TYPE_CHAR, 0x13, {ATT_CHAR_PROP_RD, 0x14,0, 0, 0},                                     2},   //wifi info
    {TYPE_CHAR, 0x15, {ATT_CHAR_PROP_RD|ATT_CHAR_PROP_NTF, 0x16, 0, 0, 0}, 3},  //wifi state
    {TYPE_CFG,  0x17,  {ATT_CHAR_PROP_RD|ATT_CHAR_PROP_W}},//cfg   
#endif    
};

#if USE_SHORT_UUID==1
 const BLE_UUID128 AttUuid128List[] = 
{
    {0}, //empty
};   
#else
const BLE_UUID128 AttUuid128List[] = 
{
    {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,(ATT_UUID_WIFI&0xFF), (ATT_UUID_WIFI>>8)&0xFF, 0x00,0x00},
    {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00, (ATT_UUID_WIFI_APINFO&0xFF), (ATT_UUID_WIFI_APINFO>>8)&0xFF, 0x00, 0x00}, 
    {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,(ATT_UUID_WIFI_INFO&0xFF), (ATT_UUID_WIFI_INFO>>8)&0xFF, 0x00,0x00},
    {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,(ATT_UUID_WIFI_STATE&0xFF),(ATT_UUID_WIFI_STATE>>8)&0xFF,0x00,0x00},
};
#endif

u8 GetCharListDim(void)
{
    return sizeof(AttCharList)/sizeof(AttCharList[0]);
}

void stringTohex(u8* destValue, const u8* src, int stringLen)
{
    int i = 0;
    int j = 0;

	for(; i<stringLen; i+=2, j++)
	{
		if(src[i] >= '0' && src[i] <= '9')
			destValue[j] = (src[i] - 0x30) << 4;
		else if(src[i] >= 'A' && src[i] <= 'F')
			destValue[j] = (src[i] - 'A' + 10) << 4;
		else if(src[i] >= 'a' && src[i] <= 'f')
			destValue[j] = (src[i] - 'a' + 10) << 4;

		if(src[1+i] >= '0' && src[1+i] <= '9')
			destValue[j] |= (src[1+i]-0x30);
		else if(src[1+i] >= 'A' && src[1+i] <= 'F')
			destValue[j] |= (src[1+i]- 'A' + 10);
		else if(src[1+i] >= 'a' && src[1+i] <= 'f')
			destValue[j] |= (src[1+i]- 'a' + 10);
	}
}

//////////////////////////////////////////////////////////////////////////
///STEP1:Service declare
// read by type request handle, primary service declare implementation
void att_server_rdByGrType( u8 pdu_type, u8 attOpcode, u16 st_hd, u16 end_hd, u16 att_type )
{
 //!!!!!!!!  hard code for gap and gatt, make sure here is 100% matched with database:[AttCharList] !!!!!!!!!
    if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd == 1))//hard code for device info service
    {
        //att_server_rdByGrTypeRspDeviceInfo(pdu_type);//using the default device info service
        //GAP Device Name
        u8 t[] = {0x00,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x1,0x6,(u8*)(t),2);
        return;
    }
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x07))//hard code for device info service
    {
        //apply user defined (device info)service example
        u8 t[] = {0xa,0x18};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x7,0xf,(u8*)(t),2);
        return;
    }
    else if((att_type == GATT_PRIMARY_SERVICE_UUID) && (st_hd <= 0x10)) //usr's service
    {
        LOG(LOG_LVL_INFO,"att_server_rdByGrType pdu_type=0x%x, attOpcode=0x%x st_hd=0x%x, end_hd=0x%x, att_type=0x%x\r\n", 
                          pdu_type,attOpcode, st_hd, end_hd, att_type);
#if USE_SHORT_UUID==1
        u8 wifi[2] = {ATT_UUID_WIFI&0xFF, (ATT_UUID_WIFI>>8)&0xFF};
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x10,0x17,(u8*)(wifi),2);
#else
        att_server_rdByGrTypeRspPrimaryService(pdu_type,0x10,0x19,(u8*)(AttUuid128List[0].uuid128),16);
#endif
        unsigned short interval= sconn_GetConnInterval();
        LOG(LOG_LVL_INFO,"att_server_rdByGrType interval=%d\r\n", interval);

        return;
    }

    ///error handle
    att_notFd( pdu_type, attOpcode, st_hd );
}

///STEP2:data coming
///write response, data coming....
void ser_write_rsp(u8 pdu_type/*reserved*/, u8 attOpcode/*reserved*/, 
                   u16 att_hd, u8* attValue/*app data pointer*/, u8 valueLen_w/*app data size*/)
{   
    //LOG(LOG_LVL_INFO," ser_write_rsp attOpcode=0x%x, att_hd=0x%x pdu_type=0x%x, valueLen_w=%d\r\n", attOpcode, att_hd, pdu_type,valueLen_w);
     
    switch(att_hd)
    {        
        case 0x12:  //wifi ap info
        {
            u8 ssid_len=0;
            u8 pwd_len=0;
            
            //console_printf("\r\nser_write_rsp ap info len=%d \r\n", valueLen_w);
            //console_printf("attvalue[0-9]=0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x \r\n", 
            //attValue[0],attValue[1],attValue[2],attValue[3],attValue[4],attValue[5],attValue[6],attValue[7],attValue[8],attValue[9] );

            ser_write_rsp_pkt(pdu_type);
            
            if(valueLen_w>ATT_APINFO_MAX_SIZE)
                valueLen_w = ATT_APINFO_MAX_SIZE;

            ssid_len=0;
            pwd_len=0;
            memset(wifi_ap_info, 0, ATT_APINFO_MAX_SIZE);

            stringTohex(&ssid_len, attValue, 2);
            stringTohex(&pwd_len, &attValue[2], 2);

#if BLE_TEST==ENABLE            
            test_count++;
            LOG(LOG_LVL_INFO,"ssid_len=%d, pwd_len=%d,count=%d\r\n",ssid_len,pwd_len,test_count);
#endif
            wifi_ap_info[0]=ssid_len;
            wifi_ap_info[1]=pwd_len;
            memcpy(&wifi_ap_info[2], &attValue[4], ssid_len+pwd_len);       
            ble_station_connect(wifi_ap_info);
        }
        break;
        
        case 0x11:
        case 0x17://cfg
        {
            LOG(LOG_LVL_INFO,"attvalue[0-4]=0x%x,0x%x, 0x%x,0x%x", attValue[0],attValue[1],attValue[2],attValue[3]);
            ser_write_rsp_pkt(pdu_type); /*if the related character has the property of WRITE(with response) or TYPE_CFG, one MUST invoke this func*/
            if(att_hd==0x17)  //char client configuration
            {
                //u8 ret=0;
                //u8 t[1] = {1};
                //if wifi connected, notify 1, else 0
                //ret = sconn_notifydata(t, 1);
                LOG(LOG_LVL_INFO,"ser_write_rsp att_hd=0x17\r\n");
            }
        }
        break;
        
        default:
            att_notFd( pdu_type, attOpcode, att_hd );/*the default response, also for the purpose of error robust */
            break;
    }
 }
 ///STEP2.1:Queued Writes data if any
void ser_prepare_write(u16 handle, u8* attValue, u16 attValueLen, u16 att_offset)//user's call back api 
{
    //LOG(LOG_LVL_INFO,"ser_prepare_write  handle=0x%x, attValueLen=%d, offset=%d \r\n", handle, attValueLen, att_offset);
    //queued data:offset + data(size)
    //when ser_execute_write() is invoked, means end of queue write.

    if(att_offset==0) //if 0, then it is the first packet
    {
        u8 ssid_len=0;
        u8 pwd_len=0;
        
        info_offset=0;
        memset(wifi_ap_info, 0, ATT_APINFO_MAX_SIZE);
                                
        stringTohex(&ssid_len, attValue, 2);
        stringTohex(&pwd_len, &attValue[2], 2);
        
        wifi_ap_info[0]=ssid_len;
        wifi_ap_info[1]=pwd_len;
        info_offset = (attValueLen-4);
        
        //console_printf("ssid_len=%d, pwd_len=%d, info_offser=%d \r\n",ssid_len,pwd_len, info_offset);
        memcpy(&wifi_ap_info[2], &attValue[4], info_offset);
        info_offset+=2;
    }
    else
    {
    
        //console_printf("info_offser=%d attValueLen=%d\r\n",info_offset, attValueLen);
        memcpy(&wifi_ap_info[info_offset], attValue, attValueLen);
        info_offset += attValueLen;
    }
    
}
 
void ser_execute_write(void)//user's call back api 
{
#if BLE_TEST==ENABLE  
    test_count++;
    LOG(LOG_LVL_INFO,"ser_execute_write, %d packets!! ssid_len=%d, pwd_len=%d\r\n", test_count, wifi_ap_info[0], wifi_ap_info[1]);
#endif

    //start to connect the wifi ap with the ssid and pwd
    ble_station_connect(wifi_ap_info);
}



///STEP3:Read data
//// read response
void server_rd_rsp(u8 attOpcode, u16 attHandle, u8 pdu_type)
{
    //u8 tab[3];
    u8  d_len;
    u8* ble_name = getDeviceInfoData(&d_len);

    LOG(LOG_LVL_INFO,"server_rd_rsp attOpcode=0x%x, attHandle=0x%x pdu_type=0x%x\r\n", attOpcode, attHandle, pdu_type);
    
    switch(attHandle) //hard code
    {
        case 0x04: //GAP name
            att_server_rd( pdu_type, attOpcode, attHandle, ble_name, d_len);
            break;
                
        case 0x09: //MANU_INFO
        {
            //att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(MANU_INFO), sizeof(MANU_INFO)-1);
            att_server_rd( pdu_type, attOpcode, attHandle, get_ble_version(), strlen((const char*)get_ble_version())); //ble lib build version
        }
        break;
        
        case 0x0b: //FIRMWARE_INFO
        {            
            //do NOT modify this code!!!
            att_server_rd( pdu_type, attOpcode, attHandle, GetFirmwareInfo(),strlen((const char*)GetFirmwareInfo()));
        }
        break;
        
        case 0x0f://SOFTWARE_INFO
            att_server_rd( pdu_type, attOpcode, attHandle, (u8*)(SOFTWARE_INFO), sizeof(SOFTWARE_INFO)-1);
            break;

       case 0x12: //read ap info of SSID
       {
             unsigned char info[ATT_WIFI_INFO_SIZE]="Disconnected";
              //if wifi connected, return the wifi ssid, or return disconnected.

             if(strlen(wifi_ap_info))
                att_server_rd( pdu_type, attOpcode, attHandle, (unsigned char*)wifi_ap_info, strlen(wifi_ap_info));
             else
                att_server_rd( pdu_type, attOpcode, attHandle, info, strlen(info));
       }
       break;
        
        case 0x14:  //WIFI Info
        {
            unsigned char info[]="LN_WIFI";
            att_server_rd( pdu_type, attOpcode, attHandle, info, strlen(info));
        }
        break;
        
        case 0x17://cfg
        {
            u8 t[2] = {0,0};
            att_server_rd( pdu_type, attOpcode, attHandle, t, 2);
        }
        break;
        
        default:
            att_notFd( pdu_type, attOpcode, attHandle );/*the default response, also for the purpose of error robust */
            break;
    }               
}

void server_blob_rd_rsp(u8 attOpcode, u16 attHandle, u8 dataHdrP,u16 offset)
{
}

//return 1 means found
int GetPrimaryServiceHandle(unsigned short hd_start, unsigned short hd_end,
                            unsigned short uuid16,   
                            unsigned short* hd_start_r,unsigned short* hd_end_r)
{
    if((uuid16 == ATT_UUID_WIFI) && (hd_start <= 0x10))// MUST keep match with the information save in function  att_server_rdByGrType(...)
    {
        *hd_start_r = 0x10;
        *hd_end_r = 0x17;
        return 1;
    }
    
    return 0;
}

char GetConnectedStatus(void)
{
    LOG(LOG_LVL_INFO,"GetConnectedStatus gConnectedFlag=%d\r\n", gConnectedFlag);
    return gConnectedFlag;
}

void ConnectStausUpdate(unsigned char IsConnectedFlag) //porting api
{
    LOG(LOG_LVL_INFO,"ConnectStausUpdate gConnectedFlag=%d, IsConnectedFlag=%d\r\n", gConnectedFlag,IsConnectedFlag);
    if(IsConnectedFlag != gConnectedFlag)
        gConnectedFlag = IsConnectedFlag;

    if(gConnectedFlag)  //ble connected
    {
#if BLE_TEST==ENABLE //new connection, clean the count           
        test_count=0;
#endif            
    }
    else  //ble disconnected
    {
        ble_wifi_cfg=false;    
        memset(wifi_ap_info, 0, ATT_APINFO_MAX_SIZE);
    }
}


void UsrProcCallback(void) //porting api
{
    StandbyTimeout ++;
    
    if(gConnectedFlag)
    {//connected
        StandbyTimeout = 0;
    }
}

void  UpdateLEDValueAll(void)
{

}



//本回调函数可用于蓝牙模块端主动发送数据之用，协议栈会在系统允许的时候（异步）回调本函数，不得阻塞！！
void gatt_user_send_notify_data_callback(void)
{

}

void Notify_Wifi_connected(void)
{
    u8 t[1] = {1};
    sconn_notifydata(t, 1);
}

/**********************************************************************************************/
/********************************* porting functions*******************************************/
unsigned char aes_encrypt_HW(unsigned char *painText128bitBE,unsigned char *key128bitBE)
{
    return 0; //not supported
}

void ble_gpio_Init(void)
{
    GPIO_InitTypeDef cs_config;
    GPIO_InitTypeDef irq_config;

    cs_config.dir = GPIO_OUTPUT;
    cs_config.debounce = GPIO_DEBOUNCE_YES;
    cs_config.value = GPIO_VALUE_HIGH;
    //cs_config.trig_type= GPIO_TRIG_FALLING_EDGE;//GPIO_TRIG_LOW_LEVEL;  
    HAL_GPIO_Init(BLE_CS_PIN, cs_config);

    irq_config.dir = GPIO_INPUT;
    irq_config.debounce = GPIO_DEBOUNCE_YES;
    //irq_config.value = GPIO_VALUE_LOW;
    irq_config.trig_type= GPIO_TRIG_FALLING_EDGE;
    HAL_GPIO_Init(BLE_IRQ_PIN, irq_config);
    
    HAL_GPIO_MaskIrq(BLE_IRQ_PIN);
}

void ble_spim_init(uint32_t baud)
{
    LL_SPIM_Enable(SSI_Disabled);
    LL_SPIM_Slave_Select(0);
    LL_SPIM_CtrlR0(DFS_32_8_bits, Control_Word_1_bit, 0, 0, Transmit_and_Receive, Inactive_Low, SCLK_Toggle_In_Middle, Motorola_SPI);
    LL_SPIM_CtrlR1(0);

    LL_SPIM_CLK_Div(APBUS0_CLOCK / baud);
    LL_SPIM_TX_FIFO_Threshold_Set(1);
    LL_SPIM_RX_FIFO_Threshold_Set(1);
    LL_SPIM_Slave_Select(1);
    LL_SPIM_Enable(SSI_Enabled);
}

uint8_t BLE_spim_readwrite_onebyte(uint8_t data)
{
    uint8_t value = 0x00;
    uint32_t retry =0;

    while( LL_SPIM_Is_TxFIFONotFull() == Transmit_FIFO_Full)
    {
        retry++;
        if(retry > 200) return 0;
    }
    LL_SPIM_DataReg_Set(data);

    retry=0;
    while( LL_SPIM_Is_RxFIFONotEmpty() == Receive_FIFO_Empty)
    {
        retry++;
        if(retry > 200) return 0;
    }
    value = LL_SPIM_DataReg_Get();
    
    return value;
}


void McuGotoSleepAndWakeup(void) // auto goto sleep AND wakeup, porting api
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval : None
  */
void SysTick_BLEHandler(void)	 //一毫秒系统中断
{
    bleTick ++;
    ble_nMsRoutine();
}

unsigned int GetSysTickCount(void)
{
    return bleTick;
}

void GPIO_IRQHandler()
{
    uint32_t status;

    status = HAL_GPIO_IntStatus();
    if ( status & _BIT(BLE_IRQ_PIN)) 
    {
        HAL_GPIO_IrqClear(BLE_IRQ_PIN);
        ble_run(0);
    }
    
}

void SysClk8to48(void)
{
}
void SysClk48to8(void)
{
}

void DisableEnvINT(void)
{ 

}
void EnableEnvINT(void)
{

}
void SPI_CS_Enable_(void)
{	
    //LL_SPIM_Slave_Select(1);
    HAL_GPIO_WritePin(BLE_CS_PIN,GPIO_VALUE_LOW);
}

void SPI_CS_Disable_(void)
{
    //LL_SPIM_Slave_Select(0);
    HAL_GPIO_WritePin(BLE_CS_PIN, GPIO_VALUE_HIGH);
}

char IsIrqEnabled(void)
{
    if(HAL_GPIO_ReadPin(BLE_IRQ_PIN) == GPIO_VALUE_LOW)
        return 1;

    return 0;
}

unsigned char SPI_WriteRead(unsigned char SendData, unsigned char WriteFlag)
{
    unsigned char rcvData = 0;

    rcvData = BLE_spim_readwrite_onebyte(SendData);

    return rcvData;
}

/*************************************************************************************/
/*****************************BLE WIFI function***************************************/ 
/*************************************************************************************/
static bool ble_station_connect_general(char *str, bool save_to_flash)
{
    u8 ssid_len=0;
    u8 pwd_len=0;
    wifi_config_t wifi_config = {0,};
    wifi_scan_config_t scan_config = {0,};

    /*
     * string format: <ssid_len>,<pwd_len>,<ssid>,<pwd>
     */
     
    //ssid
    ssid_len =wifi_ap_info[0];
    pwd_len = wifi_ap_info[1];
    
	if(ssid_len)
	{
        memset(wifi_config.sta.ssid, 0, SSID_MAX_LEN);
        memcpy(wifi_config.sta.ssid, &wifi_ap_info[2], ssid_len);
	}

    //password
    if(pwd_len)
    {
        memset(wifi_config.sta.password, 0, PASSWORD_MAX_LEN);
        memcpy(wifi_config.sta.password,  &wifi_ap_info[2+ssid_len], pwd_len);
	}

    if(save_to_flash)
    {
        if(!wifi_set_config(STATION_IF, &wifi_config))
        {
            goto err;
        }
    }
    else
    {
        if(!wifi_set_config_current(STATION_IF, &wifi_config))
        {
            goto err;
        }
    }

    if(strlen((const char *)wifi_config.sta.ssid) > 0)
    {
        memcpy(scan_config.ssid, wifi_config.sta.ssid, strlen((const char *)wifi_config.sta.ssid));
    }


    scan_config.channel = 0;
    scan_config.show_hidden = false;
    scan_config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
    scan_config.scan_time.active.min = 20;
    scan_config.scan_time.active.max = 40;
    
    if(!wifi_station_scan(&scan_config))
    {
        goto err;
    }
    return true;
    
err:
    return false;
}
/*************************************************************************************
Input para: str[0]:ssid length in the str
            str[1]:pwd length in the str
            str[2-n]:ssid+pwd
output para: connect successful, return 0, else return 1;
**************************************************************************************/
void ble_station_connect(char *str)
{
    ble_msg_send(BLE_MSG_ID_CONNECT_WIFI,strlen(str),str);
}

void ble_wifi_connected_ind(void)
{
    LOG(LOG_LVL_INFO,"ble_wifi_connected_ind ble_wifi_cfg=%d\r\n", ble_wifi_cfg);
    if(ble_wifi_cfg)
        ble_msg_send(BLE_MSG_ID_WIFI_CONNECTED_IND,0, NULL);
}

/*************************************************************************************/
/*************************************************************************************/
void ble_msg_send(BLE_MSG_ID_ENUM id, unsigned short msg_len, void *msg)
{
    BLE_MSG_T ble_msg = {BLE_MSG_ID_INVALID,0,};

    ble_msg.id = id;
    ble_msg.len = msg_len;
    ble_msg.msg= msg;
    OS_QueueSend(&g_ble_msg_queue, &ble_msg, OS_WAIT_FOREVER);
}

void ble_task(void *params)
{
    BLE_MSG_T ble_msg;
    unsigned char addr[6]={0x01, 0x88, 0x10, 0x11, 0x22, 0x33};

    //SPI Init firstly
    LL_SYSCON_SPIMEnable(SPIM_IO_EN0);
    ble_spim_init(8000000);

    ble_gpio_Init();

    	
    SetBleIntRunningMode();

    //radio_setBleAddr(addr); //can set wanted BLE address.
		
    radio_initBle(TXPWR_0DBM, &ble_mac_addr); 
    
    LOG(LOG_LVL_INFO,"Local MAC = %02X:%02X:%02X:%02X:%02X:%02X\r\n",ble_mac_addr[5],ble_mac_addr[4],ble_mac_addr[3],ble_mac_addr[2],ble_mac_addr[1],ble_mac_addr[0]);

    ble_run_interrupt_start(160*2); //320*0.625=200ms adv interval
    HAL_GPIO_UnmaskIrq(BLE_IRQ_PIN);
    HAL_GPIO_IntEnable(BLE_IRQ_PIN);
    NVIC_EnableIRQ(GPIO_IRQn);

    while(1)
    {
        if(OS_OK == OS_QueueReceive(&g_ble_msg_queue, &ble_msg, OS_WAIT_FOREVER))
        {
            switch(ble_msg.id)
            {
                case BLE_MSG_ID_CONNECT_WIFI:
                {
                    ble_wifi_cfg = true;
                    ble_station_connect_general(ble_msg.msg, true);
                }
                break;

                case BLE_MSG_ID_WIFI_CONNECTED_IND:
                {
                    ble_wifi_cfg=false;
                    LOG(LOG_LVL_INFO,"BLE_MSG_ID_WIFI_CONNECTED_IND\r\n");
                    //ble_disconnect();
                }
                break;

                default:
                    LOG(LOG_LVL_INFO,"ble_task unknow msg_id=%d\r\n", ble_msg.id);
                break;
            }
        }
        else
            LOG(LOG_LVL_WARN,"ble_task Receive msg Failed!!\r\n");
    }
}


void ble_init(void)
{
    if(OS_QueueCreate(&g_ble_msg_queue, BLE_MSG_QUEUE_SIZE, sizeof(BLE_MSG_T)) != OS_OK) 
    {
        LOG(LOG_LVL_ERROR, "OS_MsgQueueCreate g_ble_msg_queue fail.\n");
        return;
    }
    
    if(OS_ThreadCreate(&g_ble_thread, "ble_task", ble_task, NULL, OS_PRIORITY_NORMAL, BLE_TASK_STACK_SIZE) != OS_OK) 
    {
        LOG(LOG_LVL_ERROR, "OS_ThreadCreate g_ble_thread fail.\n");
        return;
    }
}

#endif
