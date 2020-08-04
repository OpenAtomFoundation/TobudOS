#include "cmsis_os.h"
#include "mb.h"
#include "tim.h"
#include "stdlib.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START     0x0001U 
#define REG_INPUT_NREGS 4

#define REG_HOLDING_START               ( 1 )
#define REG_HOLDING_NREGS               ( 32 )
/* ----------------------- Static variables ---------------------------------*/
static uint16_t   usRegInputStart = REG_INPUT_START;
static uint16_t   usRegInputBuf[REG_INPUT_NREGS] = {0x05,0x06,0x07,0x08};

static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS] = {0x00};

#define TASK1_STK_SIZE          512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);


#define MB_REG_ADDR_PM2D5               0x00 // int    0-250
#define MB_REG_ADDR_SMOKE               0x01 // int    0-4096
#define MB_REG_ADDR_HUMI                0x03 // float  0-100
#define MB_REG_ADDR_REPORT_PERIOD       0x05 // int    0-4000
#define MB_REG_ADDR_SWITCH              0x07 // bool   0/1
#define MB_REG_ADDR_LIGHT               0x08 // float  0-4000
#define MB_REG_ADDR_CO2VALUE            0x0a // int    0-10000
#define MB_REG_ADDR_TVOC                0x0c // int    0-10000
#define MB_REG_ADDR_TEMP                0x0e // float  0-100

uint16_t val_pm2d5 = 200;
uint16_t val_smoke = 1000;
uint16_t val_humi = 66;
uint16_t val_report_period = 5;
uint16_t val_switch = 0;
uint16_t val_light = 888;
uint16_t val_co2_value = 2666;
uint16_t val_tvoc = 1555;
uint16_t val_temp = 25;

void randomValues() {
    val_pm2d5 = 200 + rand()%20;
    val_smoke = 1000 + rand()%50;
    val_humi =  66 + rand()%10;
    val_light = 888 + rand()%50;
    val_co2_value = 2666 + rand()%100;
    val_tvoc = 1555 + rand()%50;
    val_temp = 25 + rand()%5;
#if 0
    printf("random pm2d5: %d\r\n", val_pm2d5);
    printf("random val_smoke: %d\r\n", val_smoke);
    printf("random val_humi: %d\r\n", val_humi);
    printf("random val_light: %d\r\n", val_light);
    printf("random val_co2_value: %d\r\n", val_co2_value);
    printf("random val_tvoc: %d\r\n", val_tvoc);
    printf("random val_temp: %d\r\n", val_temp);
#endif
}


void initRegHoldingBuf() {
    usRegHoldingBuf[MB_REG_ADDR_PM2D5] = val_pm2d5;
    usRegHoldingBuf[MB_REG_ADDR_SMOKE] = val_smoke >> 8;
    usRegHoldingBuf[MB_REG_ADDR_SMOKE + 1] = val_smoke & 0x00ff;
    usRegHoldingBuf[MB_REG_ADDR_HUMI] = val_humi >> 8;
    usRegHoldingBuf[MB_REG_ADDR_HUMI + 1] = val_humi & 0x00ff;
    usRegHoldingBuf[MB_REG_ADDR_REPORT_PERIOD] = val_report_period >> 8;
    usRegHoldingBuf[MB_REG_ADDR_REPORT_PERIOD + 1] = val_report_period & 0x00ff;
    usRegHoldingBuf[MB_REG_ADDR_SWITCH] = val_switch;
    usRegHoldingBuf[MB_REG_ADDR_LIGHT] = val_light >> 8;
    usRegHoldingBuf[MB_REG_ADDR_LIGHT + 1] = val_light & 0x00ff;
    usRegHoldingBuf[MB_REG_ADDR_CO2VALUE] = val_co2_value >> 8;
    usRegHoldingBuf[MB_REG_ADDR_CO2VALUE + 1] = val_co2_value & 0x00ff;
    usRegHoldingBuf[MB_REG_ADDR_TVOC] = val_tvoc >> 8;
    usRegHoldingBuf[MB_REG_ADDR_TVOC + 1] = val_tvoc & 0x00ff;
    usRegHoldingBuf[MB_REG_ADDR_TEMP] = val_temp;
}


void updateRegHoldingBuf() {
    randomValues();

    usRegHoldingBuf[MB_REG_ADDR_PM2D5] = val_pm2d5;
    usRegHoldingBuf[MB_REG_ADDR_SMOKE] = val_smoke >> 8;
    usRegHoldingBuf[MB_REG_ADDR_SMOKE + 1] = val_smoke & 0x00ff;
    usRegHoldingBuf[MB_REG_ADDR_HUMI] = val_humi >> 8;
    usRegHoldingBuf[MB_REG_ADDR_HUMI + 1] = val_humi & 0x00ff;
    //usRegHoldingBuf[MB_REG_ADDR_REPORT_PERIOD] = val_report_period >> 8;
    //usRegHoldingBuf[MB_REG_ADDR_REPORT_PERIOD + 1] = val_report_period & 0x00ff;
    //usRegHoldingBuf[MB_REG_ADDR_SWITCH] = val_switch;
    usRegHoldingBuf[MB_REG_ADDR_LIGHT] = val_light >> 8;
    usRegHoldingBuf[MB_REG_ADDR_LIGHT + 1] = val_light & 0x00ff;
    usRegHoldingBuf[MB_REG_ADDR_CO2VALUE] = val_co2_value >> 8;
    usRegHoldingBuf[MB_REG_ADDR_CO2VALUE + 1] = val_co2_value & 0x00ff;
    usRegHoldingBuf[MB_REG_ADDR_TVOC] = val_tvoc >> 8;
    usRegHoldingBuf[MB_REG_ADDR_TVOC + 1] = val_tvoc & 0x00ff;
    usRegHoldingBuf[MB_REG_ADDR_TEMP] = val_temp;
}

void WriteRegHoldingCallback(int iRegIndex) {
    if (iRegIndex == MB_REG_ADDR_SWITCH) {
        uint16_t switch_val = usRegHoldingBuf[MB_REG_ADDR_SWITCH];
        printf("write switch value %d\r\n", switch_val);
        GPIO_PinState state = GPIO_PIN_SET;
        if (switch_val == 0) {
            state = GPIO_PIN_RESET;
        }
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, state);
    }
}

void task1(void *arg)
{
	__HAL_TIM_CLEAR_FLAG(&htim6,TIM_FLAG_UPDATE);
	eMBInit(MB_RTU, 0x01, 1, 9600, MB_PAR_ODD);
	eMBEnable();

    initRegHoldingBuf();
    while (1) {
        eMBPoll();
        // printf("FreeModbus eMBPoll function running\r\n");
        osDelay(200);
        updateRegHoldingBuf();
    }
}

void application_entry(void *arg)
{
    osThreadCreate(osThread(task1), NULL); // Create task1
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

        case MB_REG_WRITE:
            printf("holding write usAddress[%d] usNRegs[%d] eMode[%d]\r\n", usAddress, usNRegs, eMode);
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                WriteRegHoldingCallback(iRegIndex);
                iRegIndex++;
                usNRegs--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNCoils;
    ( void )eMode;
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNDiscrete;
    return MB_ENOREG;
}
