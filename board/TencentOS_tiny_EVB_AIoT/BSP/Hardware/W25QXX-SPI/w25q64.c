#include "w25q64.h"

#if defined(USE_ST_HAL)
static void spi_init(void)
{
    // it will be called in main.
    // MX_SPIx_Init();
}

static void spi_cs_select(void)
{
    HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_RESET);
    
    /* tCHSH: /CS Active Hold Time relative to CLK, min is 5 ns. */
    for (int i = 0; i < 50; i++) {
        __NOP();
    }
}

static void spi_cs_deselect(void)
{
    HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET);
    
    /* tSHSL: /CS Deselect Time (for Array Read -> Array Read ->
        -> Erase or Program -> Read Status Registers), min is 10 ns. */
    for (int i = 0; i < 100; i++) {
        __NOP();
    }
}

static int spi_transmit(uint8_t *buf, uint16_t size)
{
   HAL_StatusTypeDef status;

   status = HAL_SPI_Transmit(&SPI_Handle, buf, size, 100);

   return status == HAL_OK ? 0 : -1;
}

static int spi_receive(uint8_t *buf, uint16_t size)
{
   HAL_StatusTypeDef status;

   status = HAL_SPI_Receive(&SPI_Handle, buf, size, 100);

   return status == HAL_OK ? 0 : -1;
}

#elif defined(USE_NXP_FSL)
static void spi_init(void)
{
    lpspi_master_config_t masterConfig;
    uint32_t srcClock_Hz;
    
    /* Set clock source for LPSPI */
    CLOCK_SetMux(kCLOCK_LpspiMux, LPSPI_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_LpspiDiv, LPSPI_CLOCK_SOURCE_DIVIDER);
    
    /* Master config */
    LPSPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate = TRANSFER_BAUDRATE;

    srcClock_Hz = LPSPI_MASTER_CLK_FREQ;
    LPSPI_MasterInit(SPI_Handle, &masterConfig, srcClock_Hz);
}

static void spi_cs_select(void)
{
    GPIO_PinWrite(SPI_CS_PORT, SPI_CS_PIN, 0);
    
    /* tCHSH: /CS Active Hold Time relative to CLK, min is 5 ns. */
    for (int i = 0; i < 500; i++) {
        __NOP();
    }
}

static void spi_cs_deselect(void)
{
    GPIO_PinWrite(SPI_CS_PORT, SPI_CS_PIN, 1);
    
    /* tSHSL: /CS Deselect Time (for Array Read -> Array Read ->
        -> Erase or Program -> Read Status Registers), min is 10 ns. */
    for (int i = 0; i < 1000; i++) {
        __NOP();
    }
}

static int spi_transmit(uint8_t *buf, uint16_t size)
{
    status_t status;
    lpspi_transfer_t masterXfer;
    
    /* Start master transfer, transfer data to slave. */
    masterXfer.txData   = buf;
    masterXfer.rxData   = NULL;
    masterXfer.dataSize = size;
    masterXfer.configFlags = kLPSPI_MasterByteSwap;

    status = LPSPI_MasterTransferBlocking(SPI_Handle, &masterXfer);
    
    return status == kStatus_Success ? 0 : -1;
}

static int spi_receive(uint8_t *buf, uint16_t size)
{
    status_t status;
    lpspi_transfer_t masterXfer;
    
    /* Start master transfer, receive data from slave */
    masterXfer.txData   = NULL;
    masterXfer.rxData   = buf;
    masterXfer.dataSize = size;
    masterXfer.configFlags = kLPSPI_MasterByteSwap;

    status = LPSPI_MasterTransferBlocking(SPI_Handle, &masterXfer);
    
    return status == kStatus_Success ? 0 : -1;
}
#endif /* USE_ST_HAL or USE_NXP_FSL */

int w25qxx_init(void)
{
    spi_init();
    
    return 0;
}

uint16_t w25qxx_read_deviceid(void)
{
    uint8_t recv_buf[2] = {0};
    uint16_t device_id = 0;
    uint8_t send_data[4] = {ManufactDeviceID_CMD, 0x00, 0x00, 0x00};

    spi_cs_select();
    if (spi_transmit(send_data, 4) == 0) {
        if (spi_receive(recv_buf, 2) == 0) {
            device_id = (recv_buf[0] << 8) | recv_buf[1];
        }
    }
    spi_cs_deselect();
    
    return device_id;
}

static void w25qxx_wait_busy(void)
{
    uint8_t cmd;
    uint8_t result;
   
    cmd = READ_STATU_REGISTER_1;
  
    spi_cs_select();
    spi_transmit(&cmd, 1);
    while (1) {
        spi_receive(&result, 1);
        if ((result & 0x01) != 0x01) {
            break;
        }
    }
    spi_cs_deselect();
    
    return;
}

int w25qxx_read(uint8_t* buffer, uint32_t start_addr, uint16_t nbytes)
{
    uint8_t cmd[4];

    cmd[0] = READ_DATA_CMD;
    cmd[1] = (uint8_t)(start_addr >> 16);
    cmd[2] = (uint8_t)(start_addr >> 8);
    cmd[3] = (uint8_t)(start_addr);
    
    spi_cs_select();
    if (spi_transmit(cmd, 4) == 0) {
        if (spi_receive(buffer, nbytes) == 0) {
            spi_cs_deselect();
            return 0;
        }
    }
    spi_cs_deselect();
    
    return -1;
}

void w25qxx_write_enable(void)
{
    uint8_t cmd = WRITE_ENABLE_CMD;
    
    spi_cs_select();
    spi_transmit(&cmd, 1);
    spi_cs_deselect();
}

void w25qxx_write_disable(void)
{
    uint8_t cmd = WRITE_DISABLE_CMD;
    
    spi_cs_select();
    spi_transmit(&cmd, 1);
    spi_cs_deselect();
}

int w25qxx_erase_sector(uint32_t sector_addr)
{
    uint8_t cmd[4];

    cmd[0] = SECTOR_ERASE_CMD;
    cmd[1] = (uint8_t)(sector_addr>>16);
    cmd[2] = (uint8_t)(sector_addr>>8);
    cmd[3] = (uint8_t)(sector_addr);
    
    w25qxx_wait_busy();
    
    w25qxx_write_enable();
   
    spi_cs_select();
    if (spi_transmit(cmd, 4) != 0) {
        spi_cs_deselect();
        return -1;
    }
    spi_cs_deselect();
    
    w25qxx_wait_busy();
    
    return 0;
}

int w25qxx_page_program(uint8_t* dat, uint32_t write_addr, uint16_t nbytes)
{
    uint8_t cmd[4];
    
    cmd[0] = PAGE_PROGRAM_CMD;
    cmd[1] = (uint8_t)(write_addr >> 16);
    cmd[2] = (uint8_t)(write_addr >> 8);
    cmd[3] = (uint8_t)(write_addr);
    
    w25qxx_wait_busy();
    
    w25qxx_write_enable();

    spi_cs_select();
    if (spi_transmit(cmd, 4) != 0) {
        spi_cs_deselect();
        return -1;
    }
    if (spi_transmit(dat, nbytes) != 0) {
        spi_cs_deselect();
        return -1;
    }
    spi_cs_deselect();
    
    w25qxx_wait_busy();

    return 0;
}
