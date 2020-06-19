#include "proj_config.h"

#include "ln88xx.h"
#include "serial/serial.h"
#include "hal/hal_syscon.h"
#include "hal/hal_uart.h"
//#include "hal/hal_sleep.h"
#include "utils/debug/art_assert.h"

#define UART0_TX_BUF_SIZE  CFG_UART0_TX_BUF_SIZE
#define UART0_RX_BUF_SIZE  CFG_UART0_RX_BUF_SIZE
#define UART1_TX_BUF_SIZE  CFG_UART1_TX_BUF_SIZE
#define UART1_RX_BUF_SIZE  CFG_UART1_RX_BUF_SIZE


/* From the high-level serial driver */
extern Serial_t serial_handles[SER_PORT_NUM];

/* TX and RX buffers */
unsigned char uart0_txbuffer[UART0_TX_BUF_SIZE];
unsigned char uart0_rxbuffer[UART0_RX_BUF_SIZE];
unsigned char uart1_txbuffer[UART1_TX_BUF_SIZE];
unsigned char uart1_rxbuffer[UART1_RX_BUF_SIZE];

/* UART device*/
UART_DevTypeDef g_huart0,g_huart1;

/**
 * Internal state structure
 */
struct ARTSerial
{
    struct SerialHardware  Hardware;
    struct Serial         *serial;
};

struct ARTSerial UARTDescs[SER_PORT_NUM];
static void uart0_SendDataIrqCallback(uint8_t *ch);
static void uart0_RecvDataIrqCallback(uint8_t *ch);
static void uart1_SendDataIrqCallback(uint8_t *ch);
static void uart1_RecvDataIrqCallback(uint8_t *ch);

/*
 * *****************************  Port IO Config  ******************************
 */
static void uart_io_pin_request(struct Serial *serial)
{
    int en = 1;

    ART_ASSERT(serial);

    if(serial->port_id == SER_PORT_UART0){
#if   defined (LN881x)
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART0_RX, GPIO_AF_IO_0, en);
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART0_TX, GPIO_AF_IO_1, en);
#elif defined (LN882x)
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART0_RX, GPIO_AF_IO_18, en); //LN882x: GPIO_A[8], FULL_MUX_18, PAD24 [rom_uart0 RX]
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART0_TX, GPIO_AF_IO_19, en); //LN882x: GPIO_A[9], FULL_MUX_19, PAD25 [rom_uart0 TX]
#else
  #error Add your bord type!!!
#endif
    }else if(serial->port_id == SER_PORT_UART1){
#if   defined (LN881x)
        // EVB V2 引脚更改了
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART1_RX, GPIO_AF_IO_2, en);
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART1_TX, GPIO_AF_IO_4, en);
#elif defined (LN882x)
        //TODO:待最终demo板确认
        //HAL_SYSCON_FuncIOSet(GPIO_AF_UART1_RX, GPIO_AF_IO_15, en);//LN882x FPGA: GPIO_1[5], FULL_MUX_15, PAD21
        //HAL_SYSCON_FuncIOSet(GPIO_AF_UART1_TX, GPIO_AF_IO_16, en);//LN882x FPGA: GPIO_1[6], FULL_MUX_16, PAD22
    #if 0//LN8826CT
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART1_RX, GPIO_AF_IO_8, en);//LN8826CT EVB: GPIOA8 GPIO_0[8], FULL_MUX_08, PAD08
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART1_TX, GPIO_AF_IO_9, en);//LN8826CT EVB: GPIOA9 GPIO_0[9], FULL_MUX_09, PAD09
    #else //LN8820 or LN8825
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART1_RX, GPIO_AF_IO_16, en);//LN8820 (LN8825) &BLE EVB: GPIOB6, FULL_MUX_16, Pin31
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART1_TX, GPIO_AF_IO_17, en);//LN8820 (LN8825) &BLE EVB: GPIOB7, FULL_MUX_17, Pin32
    #endif
#else
  #error Add your bord type!!!
#endif
    }
}

static void uart_io_pin_release(struct Serial *serial)
{
    int en = 0;

    ART_ASSERT(serial);
    if(serial->port_id == SER_PORT_UART0){
#if   defined (LN881x)
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART0_RX, GPIO_AF_IO_0, en);
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART0_TX, GPIO_AF_IO_1, en);
#elif defined (LN882x)
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART0_RX, GPIO_AF_IO_18, en);//LN882x: GPIO_1[8], FULL_MUX_18, PAD24 [rom_uart0 RX]
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART0_TX, GPIO_AF_IO_19, en);//LN882x: GPIO_1[9], FULL_MUX_19, PAD25 [rom_uart0 TX]
#else
  #error Add your bord type!!!
#endif
    }else if(serial->port_id == SER_PORT_UART1){
#if   defined (LN881x)
        // EVB V2 引脚更改了
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART1_RX, GPIO_AF_IO_2, en);
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART1_TX, GPIO_AF_IO_4, en);
#elif defined (LN882x)
        //TODO:待最终demo板确认
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART1_RX, GPIO_AF_IO_15, en);//LN882x: GPIO_1[5], FULL_MUX_15, PAD21
        HAL_SYSCON_FuncIOSet(GPIO_AF_UART1_TX, GPIO_AF_IO_16, en);//LN882x: GPIO_1[6], FULL_MUX_16, PAD22
#else
  #error Add your bord type!!!
#endif
    }
}

/*
 * *****************************  Device Init  ******************************
 */
static void hw_uart0_init(struct SerialHardware *_hw, struct Serial *serial, uint32_t baudrate)
{
    struct ARTSerial *hw = NULL;

    ART_ASSERT(_hw && serial);
    hw = (struct ARTSerial *)_hw;
    hw->serial = serial;

    g_huart0.Instance           = UART0;
    g_huart0.Config.BaudRate    = baudrate;//115200 921600 2000000
    g_huart0.Config.DataLength  = UART_DATALEN_8BIT;
    g_huart0.Config.Parity      = UART_PARITY_NONE;
    g_huart0.Config.StopBits    = UART_STOP_BIT_1;
    g_huart0.Config.FlowControl = UART_FLOW_CONTROL_SOFTWARE;

    //request pin for uart
    uart_io_pin_request(hw->serial);

    //init uart hardware
    HAL_UART_Init(&g_huart0);

    //config tx/rx trigger
    HAL_UART_FIFOControl(&g_huart0, UART_TX_EMPTY_TRIGGER_FIFO_EMPTY, UART_RCVR_TRIGGER_FIFO_HAS_ONE_CHARACTER, UART_DMA_MODE0);

    //enable RX interrupt if necessary
    HAL_UART_INT_Switch_RecvDataAvailable(&g_huart0, 1);

    //enable TX interrupt if necessary
//    hal_uart_xfer_data_interrupt(_hw->hw_addr, 1);

    //Enable Receiver Line Status Interrupt if necessary
    HAL_UART_INT_Switch_RecvLineStatus(&g_huart0, 1);

    //Enable Modem Status Interrupt if necessary
    //hal_uart_modem_status_interrupt(_hw->hw_addr, 1);

    //Enable Programmable THRE Interrupt if necessary
//    hal_uart_thre_interrupt(_hw->hw_addr, 1);

    HAL_UART_SetIsrRecvCharCallback(g_huart0.Instance, uart0_RecvDataIrqCallback);
    HAL_UART_SetIsrSendCharCallback(g_huart0.Instance, uart0_SendDataIrqCallback);

    //enable uart master switch
    NVIC_EnableIRQ(UART0_IRQn);

    /*
    * Register with the sleep module to ensure that the serial port can be used during Light sleep
    */
//    hal_sleep_register(MOD_UART0, NULL, NULL, NULL);
}

static void hw_uart1_init(struct SerialHardware *_hw, struct Serial *serial, uint32_t baudrate)
{
    struct ARTSerial *hw = NULL;

    ART_ASSERT(_hw && serial);
    hw = (struct ARTSerial *)_hw;
    hw->serial = serial;

    g_huart1.Instance           = UART1;
    g_huart1.Config.BaudRate    = baudrate;//115200, 921600
    g_huart1.Config.DataLength  = UART_DATALEN_8BIT;
    g_huart1.Config.Parity      = UART_PARITY_NONE;
    g_huart1.Config.StopBits    = UART_STOP_BIT_1;
    g_huart1.Config.FlowControl = UART_FLOW_CONTROL_SOFTWARE;

    //request pin for uart
    uart_io_pin_request(hw->serial);

    //init uart hardware
    HAL_UART_Init(&g_huart1);

    //config tx/rx trigger
    HAL_UART_FIFOControl(&g_huart1, UART_TX_EMPTY_TRIGGER_FIFO_EMPTY, UART_RCVR_TRIGGER_FIFO_HAS_ONE_CHARACTER, UART_DMA_MODE0);

    //enable RX interrupt if necessary
    HAL_UART_INT_Switch_RecvDataAvailable(&g_huart1, 1);

    HAL_UART_SetIsrRecvCharCallback(g_huart1.Instance, uart1_RecvDataIrqCallback);
    HAL_UART_SetIsrSendCharCallback(g_huart1.Instance, uart1_SendDataIrqCallback);

    //enable uart master switch
    NVIC_EnableIRQ(UART1_IRQn);

    /*
    * Register with the sleep module to ensure that the serial port can be used during Light sleep
    */
//    hal_sleep_register(MOD_UART1, NULL, NULL, NULL);
}

/*
 * *****************************  Device Cleanup  ******************************
 */
static void hw_uart_cleanup(struct SerialHardware *_hw)
{
    struct ARTSerial *hw = NULL;

    ART_ASSERT(_hw);
    hw = (struct ARTSerial *)_hw;

    HAL_UART_Deinit(hw->Hardware.hw_device);
    uart_io_pin_release(hw->serial);
    hw->serial = NULL;
}

/*
 * *****************************  Device txStart  ******************************
 */
//txStart polling mode.
static void hw_uart_txStartPolling(struct SerialHardware * _hw)
{
    uint8_t ch;
    struct ARTSerial *hw = NULL;
    UART_DevTypeDef * pUartDev;

    ART_ASSERT(_hw);
    hw = (struct ARTSerial *)_hw;
    while(!fifo_isempty(&hw->serial->txfifo))
    {
        ch = fifo_pop(&hw->serial->txfifo);
        pUartDev = (UART_DevTypeDef *)hw->Hardware.hw_device;
        HAL_UART_WriteOneChar(pUartDev->Instance, &ch);
    }
}

//txStart ISR mode.
static void hw_uart_txStartIsr(struct SerialHardware * _hw)
{
    struct ARTSerial *hw = NULL;
    ART_ASSERT(_hw);
    hw = (struct ARTSerial *)_hw;

    if (hw->Hardware.isSending){
        return;
    }

    if(!fifo_isempty(&hw->serial->txfifo))
    {
        hw->Hardware.isSending = true;
        /* Enable TX empty interrupts. */
        HAL_UART_INT_Switch_TransmitHoldingRegEmpty(hw->Hardware.hw_device, 1);
    }
}

/*
 * *****************************  Device ixSending  ******************************
 */
static bool hw_uart_txIsSending(struct SerialHardware * _hw)
{
    struct ARTSerial *hw = NULL;

    ART_ASSERT(_hw);
    hw = (struct ARTSerial *)_hw;
    return hw->Hardware.isSending;
}

/*
 * *****************************  Device setBaudrate  ******************************
 */
static bool hw_uart_setBaudrate(struct SerialHardware * _hw, uint32_t baudrate)
{
    struct ARTSerial *hw = NULL;
    UART_DevTypeDef * pUartDev;

    ART_ASSERT(_hw);
    hw = (struct ARTSerial *)_hw;

    pUartDev = (UART_DevTypeDef *)hw->Hardware.hw_device;

    if(HAL_OK == HAL_UART_BaudRateConfig(pUartDev, baudrate)){
        return  true;
    } else {
        return  false;
    }
}

/*
 * *****************************  Register ISR Callback  *****************************
 */
//send data callback in ISR mode.
static void uart0_SendDataIrqCallback(uint8_t *ch)
{
    struct ARTSerial *hw = (struct ARTSerial *)&UARTDescs[SER_PORT_UART0];
    uint8_t tx_char = 0;

    if (fifo_isempty(&hw->serial->txfifo))
    {
        /* Disable TX empty interrupts if there're no more characters to transmit. */
        HAL_UART_INT_Switch_TransmitHoldingRegEmpty(&g_huart0, 0);
        hw->Hardware.isSending = false;
    }
    else
    {
        tx_char = fifo_pop(&hw->serial->txfifo);
        HAL_UART_WriteOneChar(g_huart0.Instance, &tx_char);
    }
}

//recieve data callback in ISR mode.
static void uart0_RecvDataIrqCallback(uint8_t *ch)
{
    struct ARTSerial *hw = (struct ARTSerial *)&UARTDescs[SER_PORT_UART0];

    while (fifo_isfull_locked(&hw->serial->rxfifo)){
        serial_purgeRx(hw->serial);
    }
    fifo_push(&hw->serial->rxfifo, (unsigned char)*ch);
    hw->serial->callback();
}

//send data callback in ISR mode.
static void uart1_SendDataIrqCallback(uint8_t *ch)
{
    struct ARTSerial *hw = (struct ARTSerial *)&UARTDescs[SER_PORT_UART1];
    uint8_t tx_char = 0;

    if (fifo_isempty(&hw->serial->txfifo))
    {
        /* Disable TX empty interrupts if there're no more characters to transmit. */
        HAL_UART_INT_Switch_TransmitHoldingRegEmpty(&g_huart1, 0);
        hw->Hardware.isSending = false;
    }
    else
    {
        *ch = fifo_pop(&hw->serial->txfifo);
        HAL_UART_WriteOneChar(g_huart1.Instance, &tx_char);
    }
}

//recieve data callback in ISR mode.
static void uart1_RecvDataIrqCallback(uint8_t *ch)
{
    struct ARTSerial *hw = (struct ARTSerial *)&UARTDescs[SER_PORT_UART1];

    if (fifo_isfull_locked(&hw->serial->rxfifo)){
        serial_purgeRx(hw->serial);
    }
    fifo_push(&hw->serial->rxfifo, (unsigned char)*ch);
    if(*ch == '\n'){
        hw->serial->callback();
    }
}


/*
 * High-level interface data structures.
 */
static const struct SerialHardwareVT uart0_vtable =
{
    .init        = hw_uart0_init,
    .cleanup     = hw_uart_cleanup,
    .txStart     = hw_uart_txStartPolling,//hw_uart_txStartPolling,hw_uart_txStartIsr
    .txSending   = hw_uart_txIsSending,
    .setBaudrate = hw_uart_setBaudrate,
};
static const struct SerialHardwareVT uart1_vtable =
{
    .init        = hw_uart1_init,
    .cleanup     = hw_uart_cleanup,
    .txStart     = hw_uart_txStartPolling,//hw_uart_txStartPolling,hw_uart_txStartIsr
    .txSending   = hw_uart_txIsSending,
    .setBaudrate = hw_uart_setBaudrate,
};

struct ARTSerial UARTDescs[SER_PORT_NUM] =
{
    {
        .Hardware =
        {
          .table         = &uart0_vtable,
          .txbuffer      = uart0_txbuffer,
          .rxbuffer      = uart0_rxbuffer,
          .txbuffer_size = sizeof(uart0_txbuffer),
          .rxbuffer_size = sizeof(uart0_rxbuffer),
          .hw_device     = (void *)&g_huart0,
          .isSending     = false,
        },
        .serial = NULL,
    },
    {
        .Hardware =
        {
            .table         = &uart1_vtable,
            .txbuffer      = uart1_txbuffer,
            .rxbuffer      = uart1_rxbuffer,
            .txbuffer_size = sizeof(uart1_txbuffer),
            .rxbuffer_size = sizeof(uart1_rxbuffer),
            .hw_device     = (void *)&g_huart1,
            .isSending     = false,
        },
        .serial = NULL,
    },
};

struct SerialHardware *serial_hw_getdesc(SerialPortID port_id)
{
    ART_ASSERT(port_id < SER_PORT_NUM);
    return  (struct SerialHardware *)&UARTDescs[port_id].Hardware;
}
