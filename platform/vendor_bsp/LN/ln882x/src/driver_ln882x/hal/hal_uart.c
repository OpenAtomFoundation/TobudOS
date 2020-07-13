#include "hal/hal_uart.h"

/*********************************  static funciton  *************************/
HAL_StatusTypeDef HAL_UART_Init(UART_DevTypeDef *pUartDev)
{
    assert_param( pUartDev );
    assert_param( IS_UART_INSTANCE(pUartDev->Instance) );

	/* Check the UART handle allocation */
	if(pUartDev == NULL)
	{
		return HAL_ERROR;
	}
    // disable UART
    LL_UART_En(pUartDev->Instance, 0);
    
    HAL_UART_BaudRateConfig(pUartDev, pUartDev->Config.BaudRate);
    // parity
    if (pUartDev->Config.Parity == UART_PARITY_NONE) {
        LL_UART_Parity_Enable(pUartDev->Instance, 0);
    } else {
        LL_UART_Parity_Enable(pUartDev->Instance, 1);

        LL_UART_Parity_Set_EvenOdd( pUartDev->Instance, pUartDev->Config.Parity );
    }

    // stop bits.
    LL_UART_Stopbits_Set( pUartDev->Instance, pUartDev->Config.StopBits );

    // data length.
    LL_UART_DataLenth_Set( pUartDev->Instance, pUartDev->Config.DataLength );

    // flow control.
    LL_UART_FlowControl_Set( pUartDev->Instance, pUartDev->Config.FlowControl );

	// enable UART
	LL_UART_En(pUartDev->Instance, 1);
	return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Deinit(UART_DevTypeDef *pUartDev)
{
    assert_param(pUartDev);
    assert_param(IS_UART_INSTANCE(pUartDev->Instance));

    // FIXME: to be continued.

    // disable all interrupts
    HAL_UART_INT_Switch_All(pUartDev->Instance, false);
    // disable UART module.
    
    // disable clock.
    
    // deinit GPIO

    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_BaudRateConfig(UART_DevTypeDef *pUartDev, uint32_t baudRate)
{
    assert_param( pUartDev );
    assert_param( IS_UART_INSTANCE(pUartDev->Instance) );
    assert_param( baudRate != 0 );
    
    //TODO: APB_CLK0 Could not use this macro definition directly. 
    uint32_t uart_src_clk = APBUS0_CLOCK;
    uint16_t divisor;
    uint8_t frac;

    divisor = uart_divisor_integer( uart_src_clk, baudRate );
    frac = uart_dlf( uart_src_clk, baudRate );

    if (true != LL_UART_DLAB_Set( pUartDev->Instance, 1)){
        return HAL_ERROR;
    }

    LL_UART_DLL_Set( pUartDev->Instance, (uint8_t) (divisor & 0xFF) );
    LL_UART_DLH_Set( pUartDev->Instance, (uint8_t) (divisor >> 8) );
    LL_UART_DLF_Set( pUartDev->Instance, frac );
    
    if( true != LL_UART_DLAB_Set( pUartDev->Instance, 0)){
        return HAL_ERROR;
    }
    
    return HAL_OK;
}


/**
 * Config band rate of a UART interface
 * @param[in]  pUartDev the UART interface
 * @param[in]  ClockSource  clock source select
 */
//void HAL_UART_ClockSourceConfig(UART_DevTypeDef *pUartDev, HAL_UartClockSourceEum ClockSource)
//{
//	
//}

int32_t HAL_UART_ReceiveDataPolling(UART_DevTypeDef *pUartDev, void * pData, uint32_t expectSize, uint32_t timeOut)
{
    assert_param(pUartDev);
    assert_param(pData);
    assert_param( IS_UART_INSTANCE(pUartDev->Instance) );

    uint32_t timeOver = 0;
    uint32_t actualReadSize = 0;
    uint8_t *dst = (uint8_t *)pData;

    while(timeOver <= timeOut) {
        if ( LL_UART_isDataReady(pUartDev->Instance) ) {
            *dst = LL_UART_ReadChar(pUartDev->Instance);
            dst++;
            actualReadSize++;
            if (actualReadSize >= expectSize) {
                break;
            }
        }
        timeOver++;
    }

    if (timeOver > timeOut) {
        return -1;
    }

    return actualReadSize;
}

int32_t HAL_UART_TransmitDataPolling(UART_DevTypeDef *pUartDev, const void * pData, uint32_t dataLen, uint32_t timeOut)
{
    assert_param(pUartDev);
    assert_param(pData);
    assert_param( IS_UART_INSTANCE(pUartDev->Instance) );

    uint32_t timeOver = 0;
    uint32_t actualWriteSize = 0;
    uint8_t *pSrc = (uint8_t *) pData;

    while (timeOver <= timeOut) {
        if ( LL_UART_IsTxFIFONotFull(pUartDev->Instance)) {
            LL_UART_WriteChar(pUartDev->Instance, *pSrc);
            pSrc++;
            actualWriteSize++;
            if (actualWriteSize >= dataLen) {
                break;
            }
        }
        timeOver++;
    }

    if (timeOver > timeOut) {
        return -1;
    }
    return actualWriteSize;
}

void HAL_UART_INT_Switch_All(UART_DevTypeDef *pUartDev, bool enable)
{
    assert_param( IS_UART_INSTANCE(pUartDev->Instance) );

    LL_UART_IT_Enable_ProgramTransmitHoldingRegisterEmpty(pUartDev->Instance, enable);
    LL_UART_IT_Enable_ModemStatus(pUartDev->Instance, enable);
    LL_UART_IT_Enable_ReceiverLineStatus(pUartDev->Instance, enable);
    LL_UART_IT_Enable_TransmitHoldingRegisterEmpty(pUartDev->Instance, enable);
    LL_UART_IT_Enable_ReceivedDataAvailable(pUartDev->Instance, enable);
}

void HAL_UART_FIFOControl(UART_DevTypeDef *pUartDev, Uart_TX_Trigger_Lvl tx_lvl, Uart_RX_Trigger_Lvl rx_lvl, Uart_Dma_Mode dma_mode)
{
    assert_param(IS_UART_INSTANCE(pUartDev->Instance));

    LL_UART_FIFO_Control(pUartDev->Instance, rx_lvl, tx_lvl, dma_mode);
}

void HAL_UART_Flow_Control(UART_DevTypeDef *pUartDev, bool on)
{
    assert_param(IS_UART_INSTANCE(pUartDev->Instance));

    LL_UART_FlowControl_Set(pUartDev->Instance, (UART_FlowControl)on);
}

void HAL_UART_Finish_Transfer(UART_DevTypeDef *pUartDev)
{
    assert_param(IS_UART_INSTANCE(pUartDev->Instance));

    LL_UART_Finish_Transfer(pUartDev->Instance);
}

Uart_Modem_Status HAL_UART_Get_Modem_Status(UART_DevTypeDef *pUartDev)
{
    assert_param(IS_UART_INSTANCE(pUartDev->Instance));

    return (Uart_Modem_Status)LL_UART_Get_Modem_Status(pUartDev->Instance);
}

void HAL_UART_Low_Power_Set(UART_DevTypeDef *pUartDev, uint16_t divisor)
{
    assert_param(IS_UART_INSTANCE(pUartDev->Instance));

    LL_UART_Low_Power_Divisor_Set(pUartDev->Instance, divisor);
}

void HAL_UART_INT_Switch_RecvDataAvailable(UART_DevTypeDef *pUartDev, uint8_t en)
{
    assert_param(IS_UART_INSTANCE(pUartDev->Instance));

    LL_UART_IT_Enable_ReceivedDataAvailable(pUartDev->Instance, en);
}

void HAL_UART_INT_Switch_TransmitHoldingRegEmpty(UART_DevTypeDef *pUartDev, uint8_t en)
{
    assert_param(IS_UART_INSTANCE(pUartDev->Instance));

    LL_UART_IT_Enable_TransmitHoldingRegisterEmpty(pUartDev->Instance, en);
}

void HAL_UART_INT_Switch_RecvLineStatus(UART_DevTypeDef *pUartDev, uint8_t en)
{
    assert_param(IS_UART_INSTANCE(pUartDev->Instance));

    LL_UART_IT_Enable_ReceiverLineStatus(pUartDev->Instance, en);
}

void HAL_UART_INT_Switch_ModemStatus(UART_DevTypeDef *pUartDev, uint8_t en)
{
    assert_param(IS_UART_INSTANCE(pUartDev->Instance));

    LL_UART_IT_Enable_ModemStatus(pUartDev->Instance, en);
}

void HAL_UART_INT_Switch_ProgramTransmitHoldingRegEmpty(UART_DevTypeDef *pUartDev, uint8_t en)
{
    assert_param(IS_UART_INSTANCE(pUartDev->Instance));

    LL_UART_IT_Enable_ProgramTransmitHoldingRegisterEmpty(pUartDev->Instance, en);
}

HAL_StatusTypeDef HAL_UART_ReadOneChar(UartInstance * pUARTx, uint8_t * pData)
{
    assert_param( IS_UART_INSTANCE(pUARTx) );

    // FIXME: Timeout must be a validated default time.
    uint32_t timeout = 40000*2;
    while (!(LL_UART_isDataReady(pUARTx)))
    {
        if ((timeout--) < 1)
        {
            return HAL_TIMEOUT;
        }
    }
    *pData = (uint8_t)LL_UART_ReadChar(pUARTx);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_WriteOneChar(UartInstance * pUARTx, uint8_t * pData)
{
    assert_param( IS_UART_INSTANCE(pUARTx) );
    
    // FIXME: Timeout must be a validated default time.
    uint32_t timeout = 40000*2;
    
    while(!(LL_UART_IsTxFIFONotFull(pUARTx))){
        if ((timeout--) < 1)
        {
            return HAL_TIMEOUT;
        }
    }
    LL_UART_WriteChar(pUARTx, *pData);
    return HAL_OK;
}

static void HAL_UART_DefaultCallback(uint8_t *ch)
{
    ch = ch;//Nothing to do.
}

static pIsrRecvCharCb pUART0_IsrRecvCharCb = HAL_UART_DefaultCallback;
static pIsrSendCharCb pUART0_IsrSendCharCb = HAL_UART_DefaultCallback;
static pIsrRecvCharCb pUART1_IsrRecvCharCb = HAL_UART_DefaultCallback;
static pIsrSendCharCb pUART1_IsrSendCharCb = HAL_UART_DefaultCallback;

void HAL_UART_SetIsrRecvCharCallback(UartInstance * pUARTx, pIsrRecvCharCb callback_fun)
{
    if(pUARTx == UART0){
        pUART0_IsrRecvCharCb = callback_fun;
    } else if(pUARTx == UART1) {
        pUART1_IsrRecvCharCb = callback_fun;
    } else {
        //TODO:ASSERT
    }
}

void HAL_UART_SetIsrSendCharCallback(UartInstance * pUARTx, pIsrSendCharCb callback_fun)
{
    if(pUARTx == UART0){
        pUART0_IsrSendCharCb = callback_fun;
    } else if(pUARTx == UART1) {
        pUART1_IsrSendCharCb = callback_fun;
    } else {
        //TODO:ASSERT
    }
}

//UART0 IT Process
static void HAL_UART0_ModemStatusISR(void)
{
    // FIXME: INT modem status
}
static void HAL_UART0_TransmitHoldingRegEmptyISR(void)
{
    uint8_t tx_char = 0;
    pUART0_IsrSendCharCb(&tx_char);
}
static void HAL_UART0_RecvDataAvailableISR(void)
{
    uint8_t rx_char = 0;
    uint8_t rx_num = LL_UART_GetRxFIFOLevel(UART0);

    while(rx_num > 0){
        HAL_UART_ReadOneChar(UART0, &rx_char);
        pUART0_IsrRecvCharCb(&rx_char);
        rx_num--;
    }
    while(LL_UART_IsRxFIFONotEmpty(UART0)){
        HAL_UART_ReadOneChar(UART0, &rx_char);
        pUART0_IsrRecvCharCb(&rx_char);
    }
}

static void HAL_UART0_RecvCharTimeoutISR(void)
{
    uint8_t rx_char = 0;
    uint8_t rx_num = LL_UART_GetRxFIFOLevel(UART0);

    while(rx_num > 0){
        HAL_UART_ReadOneChar(UART0, &rx_char);
        pUART0_IsrRecvCharCb(&rx_char);
        rx_num--;
    }
    while(LL_UART_IsRxFIFONotEmpty(UART0)){
        HAL_UART_ReadOneChar(UART0, &rx_char);
        pUART0_IsrRecvCharCb(&rx_char);
    }
}

static void HAL_UART0_RecvLineStatusISR(void)
{
    Uart_Line_Status lst;
    
    lst = (Uart_Line_Status)LL_UART_GetLineStatus(UART0);
    if(lst & UART_LSR_OVERRUN_ERR)			//Overrun_Error
    {
        while(LL_UART_GetLineStatus(UART0) & UART_LSR_DATA_READY)
        {
            LL_UART_ReadChar(UART0);
        }
    }
    else if(lst & UART_LSR_FRAMING_ERR)			//Framing_Error
    {
    }
}
static void HAL_UART0_BusyDetectISR(void)
{
    // FIXME: what to do with UART status?
    static uint8_t dumy_read = 0;
    dumy_read = LL_UART_IsUARTBusy(UART0);
    dumy_read = dumy_read;//remove the warning
}

void UART0_IRQHandler(void)
{
    static Uart_Int_Id int_id;
    
    int_id = (Uart_Int_Id)LL_UART_GetInterruptId(UART0);
    
    switch(int_id)
    {   
        case UART_INT_ID_RECV_LINE_STATUS:
            HAL_UART0_RecvLineStatusISR();
            break;
        case UART_INT_ID_RECV_DATA_AVAILABLE:
            HAL_UART0_RecvDataAvailableISR();
            break;
        case UART_INT_ID_THR_EMPTY:
            HAL_UART0_TransmitHoldingRegEmptyISR();
            break;
        case UART_INT_ID_RECV_CHAR_TIMEOUT:
            HAL_UART0_RecvCharTimeoutISR();
            break;
        
        case UART_INT_ID_BUSY_DETECT:
            HAL_UART0_BusyDetectISR();
            break;
            
        case UART_INT_ID_MODEM_STATUS:
            HAL_UART0_ModemStatusISR();
            break;
        
        default:
            break;
    }
}

//UART1 IT Process
static void HAL_UART1_ModemStatusISR(void)
{
    // FIXME: INT modem status
}

static void HAL_UART1_TransmitHoldingRegEmptyISR(void)
{
    uint8_t tx_char = 0;

    pUART1_IsrSendCharCb(&tx_char);
}

static void HAL_UART1_RecvDataAvailableISR(void)
{
    uint8_t rx_char = 0;
    uint8_t rx_num = LL_UART_GetRxFIFOLevel(UART1);

    while(rx_num > 0){
        HAL_UART_ReadOneChar(UART1, &rx_char);
        pUART1_IsrRecvCharCb(&rx_char);
        rx_num--;
    }
    while(LL_UART_IsRxFIFONotEmpty(UART1)){
        HAL_UART_ReadOneChar(UART1, &rx_char);
        pUART1_IsrRecvCharCb(&rx_char);
    }
}

static void HAL_UART1_RecvCharTimeoutISR(void)
{
    uint8_t rx_char = 0;
    uint8_t rx_num = LL_UART_GetRxFIFOLevel(UART1);

    while(rx_num > 0){
        HAL_UART_ReadOneChar(UART1, &rx_char);
        pUART1_IsrRecvCharCb(&rx_char);
        rx_num--;
    }
    while(LL_UART_IsRxFIFONotEmpty(UART1)){
        HAL_UART_ReadOneChar(UART1, &rx_char);
        pUART1_IsrRecvCharCb(&rx_char);
    }
}

static void HAL_UART1_RecvLineStatusISR(void)
{
    Uart_Line_Status lst;
    
    lst = (Uart_Line_Status)LL_UART_GetLineStatus(UART1);
    if(lst & UART_LSR_OVERRUN_ERR)			//Overrun_Error
    {
        while(LL_UART_GetLineStatus(UART1) & UART_LSR_DATA_READY)
        {
            LL_UART_ReadChar(UART1);
        }
    }
    else if(lst & UART_LSR_FRAMING_ERR)			//Framing_Error
    {
    }
}

static void HAL_UART1_BusyDetectISR(void)
{
    // FIXME: what to do with UART status?
    static uint8_t dumy_read = 0;
    dumy_read = LL_UART_IsUARTBusy(UART1);
    dumy_read = dumy_read;//remove the warning
}

void UART1_IRQHandler(void)
{
    static Uart_Int_Id int_id;
    
    int_id = (Uart_Int_Id)LL_UART_GetInterruptId(UART1);
    switch(int_id)
    {   
        case UART_INT_ID_RECV_LINE_STATUS:
            HAL_UART1_RecvLineStatusISR();
            break;
        case UART_INT_ID_RECV_DATA_AVAILABLE:
            HAL_UART1_RecvDataAvailableISR();
            break;
        case UART_INT_ID_THR_EMPTY:
            HAL_UART1_TransmitHoldingRegEmptyISR();
            break;
        case UART_INT_ID_RECV_CHAR_TIMEOUT:
            HAL_UART1_RecvCharTimeoutISR();
            break;
        
        case UART_INT_ID_BUSY_DETECT:
            HAL_UART1_BusyDetectISR();
            break;
            
        case UART_INT_ID_MODEM_STATUS:
            HAL_UART1_ModemStatusISR();
            break;
        
        default:
            break;
    }
}





