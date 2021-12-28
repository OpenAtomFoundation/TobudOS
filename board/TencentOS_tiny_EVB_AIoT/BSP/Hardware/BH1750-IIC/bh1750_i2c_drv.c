#include "bh1750_i2c_drv.h"

#if defined(USE_ST_HAL)
static void bh1750_i2c_init(void)
{
    // it will be called in main.
    // MX_I2C1_Init();
}

static int bh1750_send_cmd(uint8_t cmd)
{
    return HAL_I2C_Master_Transmit(&BH1750_I2C_HANDLE, BH1750_ADDR_WRITE, &cmd, 1, 0xFFFF);
}

static int bh1750_read_dat(uint8_t* dat)
{
    return HAL_I2C_Master_Receive(&BH1750_I2C_HANDLE, BH1750_ADDR_READ, dat, 2, 0xFFFF);
}

#elif defined(USE_NXP_FSL)
static void bh1750_i2c_init(void)
{
    lpi2c_master_config_t masterConfig;
    
    /*Clock setting for LPI2C */
    CLOCK_SetMux(kCLOCK_Lpi2cMux, LPI2C_CLOCK_SOURCE_SELECT);
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, LPI2C_CLOCK_SOURCE_DIVIDER);
    
    /*
     * masterConfig.debugEnable = false;
     * masterConfig.ignoreAck = false;
     * masterConfig.pinConfig = kLPI2C_2PinOpenDrain;
     * masterConfig.baudRate_Hz = 100000U;
     * masterConfig.busIdleTimeout_ns = 0;
     * masterConfig.pinLowTimeout_ns = 0;
     * masterConfig.sdaGlitchFilterWidth_ns = 0;
     * masterConfig.sclGlitchFilterWidth_ns = 0;
     */
    LPI2C_MasterGetDefaultConfig(&masterConfig);

    /* Change the default baudrate configuration */
    masterConfig.baudRate_Hz = LPI2C_BAUDRATE;

    /* Initialize the LPI2C master peripheral */
    LPI2C_MasterInit(BH1750_I2C_MASTER, &masterConfig, LPI2C_MASTER_CLOCK_FREQUENCY);
}

static int bh1750_send_cmd(uint8_t cmd)
{
    status_t reVal = kStatus_Fail;
    
    /* Send master blocking data to slave */
    if (kStatus_Success != LPI2C_MasterStart(BH1750_I2C_MASTER, BH1750_DEVICE_ADDR_7BIT, kLPI2C_Write))
    {
        return -1;
    }
    
    /* Check communicate with slave successful or not */
    if (LPI2C_MasterGetStatusFlags(BH1750_I2C_MASTER) & kLPI2C_MasterNackDetectFlag)
    {
        return kStatus_LPI2C_Nak;
    }

    reVal = LPI2C_MasterSend(BH1750_I2C_MASTER, (uint8_t *)&cmd, 1);
    if (reVal != kStatus_Success)
    {
        if (reVal == kStatus_LPI2C_Nak)
        {
            LPI2C_MasterStop(BH1750_I2C_MASTER);
        }
        return -1;
    }

    reVal = LPI2C_MasterStop(BH1750_I2C_MASTER);
    if (reVal != kStatus_Success)
    {
        return -1;
    }
    
    return 0;
}

static int bh1750_read_dat(uint8_t* dat)
{
    status_t reVal = kStatus_Fail;
    
    if (kStatus_Success != LPI2C_MasterStart(BH1750_I2C_MASTER, BH1750_DEVICE_ADDR_7BIT, kLPI2C_Read)) {
        return -1;
    }
    
    reVal = LPI2C_MasterReceive(BH1750_I2C_MASTER, dat, 2);
    if (reVal != kStatus_Success)
    {
        if (reVal == kStatus_LPI2C_Nak)
        {
            LPI2C_MasterStop(BH1750_I2C_MASTER);
        }
        return -1;
    }

    reVal = LPI2C_MasterStop(BH1750_I2C_MASTER);
    if (reVal != kStatus_Success)
    {
        return -1;
    }
    
    return 0;
}
#endif /* USE_ST_HAL or USE_NXP_FSL */

static uint16_t bh1750_dat2lux(uint8_t* dat)
{
    uint16_t lux = 0;
    
    lux = dat[0];
    lux <<= 8;
    lux += dat[1];
    lux = (int)(lux / 1.2);
	
    return lux;
}

int bh1750_init(void)
{
    bh1750_i2c_init();
    return 0;
}

int bh1750_start(bh1750_mode_t mode)
{
    return bh1750_send_cmd(mode);
}

int bh1750_read_lux(uint16_t *lux)
{
    uint8_t dat[2] = {0};
    
    if (bh1750_read_dat(dat) != 0) {
        return -1;
    }
    
    *lux = bh1750_dat2lux(dat);
    
    return 0;
}
