/******************************************************************************
 * Include files
 ******************************************************************************/
#include "usart.h"

#define SLAVEADDR   0xc0
uint8_t u8TxData[2] = {0xaa,0x55};
uint8_t u8RxData[2] = {0x00};
uint8_t u8TxCnt=0,u8RxCnt=0;

void TxIntCallback(void)
{
    
}
void RxIntCallback(void)
{
    Uart_DisableIrq(UARTCH1,UartRxIrq);
}
void ErrIntCallback(void)
{
  
}
void PErrIntCallBack(void)
{
}
void CtsIntCallBack(void)
{
}
void Uart_PortInit(void)
{
    stc_gpio_config_t stcGpioCfg;
    DDL_ZERO_STRUCT(stcGpioCfg);
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin2,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin2,GpioAf1);//TX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortA,GpioPin3,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin3,GpioAf1);//RX
}
void usart_init(void)
{  
    uint16_t u16Scnt = 0;
    stc_uart_config_t  stcConfig;
    stc_uart_irq_cb_t stcUartIrqCb;
    stc_uart_multimode_t stcMulti;
    stc_uart_baud_t stcBaud;
    
    en_uart_mmdorck_t enTb8;

    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcUartIrqCb);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralDma,TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart1,TRUE);
    Uart_PortInit();

    stcUartIrqCb.pfnRxIrqCb   = RxIntCallback;
    stcUartIrqCb.pfnTxIrqCb   = TxIntCallback;
    stcUartIrqCb.pfnRxFEIrqCb = ErrIntCallback;
    stcUartIrqCb.pfnPEIrqCb   = PErrIntCallBack;
    stcUartIrqCb.pfnCtsIrqCb  = CtsIntCallBack;
    stcConfig.pstcIrqCb = &stcUartIrqCb;
    stcConfig.bTouchNvic = TRUE;
  

    stcConfig.enRunMode = UartMode3;
    stcConfig.enStopBit = Uart1bit;  
 
    Uart_Init(UARTCH1, &stcConfig);
    
    Uart_SetClkDiv(UARTCH1,Uart8Or16Div);
    stcBaud.u32Pclk = Sysctrl_GetPClkFreq();
    stcBaud.enRunMode = UartMode3;
    stcBaud.u32Baud = 9600;
    u16Scnt = Uart_CalScnt(UARTCH1,&stcBaud);
    Uart_SetBaud(UARTCH1,u16Scnt);
    
    Uart_ClrIsr(UARTCH1);
    //Uart_EnableIrq(UARTCH1,UartTxIrq);
    //Uart_EnableIrq(UARTCH1,UartRxIrq);
    Uart_EnableFunc(UARTCH1,UartTx); 
    Uart_DisableIrq(UARTCH1,UartRxIrq);
    enTb8 = UartDataOrAddr;
    Uart_SetMMDOrCk(UARTCH1,enTb8);
    //Uart_ClrStatus(UARTCH1,UartTC);  
}

void Debug_UartWrite(uint8_t u8Data)
{
    Uart_SendData(UARTCH1,u8Data);
}

int fputc(int ch, FILE *f)
{

    if (((uint8_t)ch) == '\n')
    {
        Debug_UartWrite('\r');
    }
    Debug_UartWrite(ch);

    return ch;
}


/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


