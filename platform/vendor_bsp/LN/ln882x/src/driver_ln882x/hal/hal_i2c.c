#include "ll/ll_i2c.h"
#include "hal/hal_i2c.h"
#include "hal/hal_common.h"

// FIXME: add assert for sanity check.

void HAL_I2C_Enable(I2CInstance *i2cX, uint8_t en)
{
    LL_I2C_Enable(i2cX, en);
}

void HAL_I2C_Abort(I2CInstance *i2cX, uint8_t en)
{
    LL_I2C_Abort(i2cX, en);
}

void HAL_I2C_SetMode(I2CInstance *i2cX, I2C_Mode_Enum_t mode)
{
    if (I2C_SLAVE == mode) {
        LL_I2C_SlaveDisable_Set(i2cX, 0);
        LL_I2C_MasterMode_Set(i2cX, 0);
    } else {
        // i2c_master
        LL_I2C_SlaveDisable_Set(i2cX, 1);
        LL_I2C_MasterMode_Set(i2cX, 1);
    }
}

I2C_Mode_Enum_t HAL_I2C_GetMode(I2CInstance *i2cX)
{
    if (1 == LL_I2C_MasterMode_Get(i2cX)) {
        return I2C_MASTER;
    }
    return I2C_SLAVE;
}

void HAL_I2C_SetSpecialBit(I2CInstance *i2cX, I2C_Special_Enum_t special)
{
    LL_I2C_SetSpecialBit(i2cX, special);
}

void HAL_I2C_GeneralCall_OR_Start(I2CInstance *i2cX, uint8_t start)
{
    LL_I2C_GeneralCall_Or_Start(i2cX, start);
}

void HAL_I2C_RestartMode_Enable(I2CInstance *i2cX, uint8_t en)
{
    LL_I2C_RestartMode_Enable(i2cX, en);
}

void HAL_I2C_Set7_10Addressing_Master(I2CInstance *i2cX, I2C_7_10Addressing_Enum_t addr)
{
    LL_I2C_Set7_10Addressing_Master(i2cX, addr);
}

void HAL_I2C_Set7_10Addressing_Slave(I2CInstance *i2cX, I2C_7_10Addressing_Enum_t addr)
{
    LL_I2C_Set7_10Addressing_Slave(i2cX, addr);
}

void HAL_I2C_SetTarAddr(I2CInstance *i2cX, uint16_t addr)
{
    LL_I2C_TarAddr_Set(i2cX, addr);
}

void HAL_I2C_SlaveAddr_Set(I2CInstance *i2cX, uint16_t addr)
{
    LL_I2C_SlaveAddr_Set(i2cX, addr);
}

void HAL_I2C_Int_Threshold_Set(I2CInstance *i2cX, uint8_t tx_tl, uint8_t rx_tl)
{
    LL_I2C_Tx_Threshold_Set(i2cX, tx_tl);
    LL_I2C_Rx_Threshold_Set(i2cX, rx_tl);
}

void HAL_I2C_Read_FIFO(I2CInstance *i2cX, uint8_t *buf, uint32_t length)
{
    uint32_t i;

    for (i = 0; i < length; i++) {
        while( 0 == LL_I2C_Rx_FIFO_Level(i2cX));
        buf[i] = LL_I2C_Data_Get(i2cX);
    }
}

void HAL_I2C_Data_Cmd_Pack(I2CInstance *i2cX, uint8_t restart, uint8_t stop, uint8_t cmd, uint8_t dat)
{
    LL_I2C_Data_Cmd_Pack(i2cX, restart, stop, cmd, dat);
}

void HAL_I2C_ReadData_Polling(I2CInstance *i2cX, uint8_t *buf, uint32_t length)
{
    uint32_t i;
    uint32_t times = 10000;

    for (i = 0; i < length; i++) {
        while( (times > 1) && ( HAL_I2C_Tx_FIFO_Level(i2cX) >= I2C_RX_TX_FIFO_DEPTH ) ) {
            times--;
        }

        if (length - 1 == i) {
            LL_I2C_Data_Cmd_Pack(i2cX, 0, 1, 1, 0); // with stop bit
        } else {
            LL_I2C_Data_Cmd_Pack(i2cX, 0, 0, 1, 0);
        }

        while(0 == LL_I2C_Rx_FIFO_Level(i2cX));

        *(buf + i) = LL_I2C_Data_Get(i2cX);
    }
}

// FIXME: 函数什么意思？不合理
void HAL_I2C_ReadData(I2CInstance *i2cX, uint32_t length)
{
    uint32_t i;
    uint32_t times = 10000;

    for( i = 0; i < length; i++) {
        while ( (times > 1) && ( HAL_I2C_Tx_FIFO_Level(i2cX) >= I2C_RX_TX_FIFO_DEPTH ) ) {
            times--;
        }

        if (length - 1 == i) {
            LL_I2C_Data_Cmd_Pack(i2cX, 0, 1, 1, 0); // with stop bit
        } else {
            LL_I2C_Data_Cmd_Pack(i2cX, 0, 0, 1, 0);
        }
    }
}

// FIXME: 函数什么意思？不合理
void HAL_I2C_Read_LastByte(I2CInstance *i2cX)
{
    LL_I2C_Data_Cmd_Pack(i2cX, 0, 1, 1, 0);
}

/**
 * @brief Send I2C data only.
 *
 * @param i2cX
 * @param buf
 * @param length
 * @return uint32_t return 0 on tx data ACK captured;
 *                  return other value on tx abort failure.
 */
uint32_t HAL_I2C_WriteData(I2CInstance *i2cX, uint8_t *buf, uint32_t length)
{
    uint32_t i;
    uint32_t times = 10000;

    for (i = 0; i < length; i++) {
        while ( (times > 1) && ( HAL_I2C_Tx_FIFO_Level(i2cX) >= I2C_RX_TX_FIFO_DEPTH ) ) {
            times--;
        }

        if (times <= 1) { // 超时了，出现了问题，需要 tx abrt
            HAL_I2C_Abort(i2cX, 1);
            return 1;
        }

        if (length - 1 == i) {
            LL_I2C_Data_Cmd_Pack(i2cX, 0, 1, 0, *(buf+i)); // with stop bit
        } else {
            LL_I2C_Data_Cmd_Pack(i2cX, 0, 0, 0, *(buf+i));
        }
    }

    return 0;
}

void HAL_I2C_Write_LastByte(I2CInstance *i2cX, uint8_t buf)
{
    LL_I2C_Data_Cmd_Pack(i2cX, 0, 1, 0, buf);
}

/**
 * @brief Send device and command.
 *
 * @param i2cX
 * @param regAddr
 * @param len
 * @return uint32_t return 0 on device address ACK and command ACK captured;
 *                  return other value on tx abort failure.
 */
uint32_t HAL_I2C_SendAddrCmd(I2CInstance *i2cX, uint8_t *regAddr, uint8_t len)
{
    uint32_t i;
    uint32_t times = 10000;

    for (i = 0; i < len; i++) {
        while ( (times > 1) && ( HAL_I2C_Tx_FIFO_Level(i2cX) >= I2C_RX_TX_FIFO_DEPTH ) ) {
            times--;
        }
        if (times <= 1) { // 超时了，出现了问题，需要 tx abrt
            HAL_I2C_Abort(i2cX, 1);
            return 1;
        }

        LL_I2C_Data_Cmd_Pack(i2cX, 0, 0, 0, *(regAddr + i));
    }

    return 0;
}

uint8_t HAL_I2C_Tx_FIFO_Level(I2CInstance *i2cX)
{
    uint8_t txlvl = 0;
    txlvl = LL_I2C_Tx_FIFO_Level(i2cX);
    return txlvl;
}

uint32_t HAL_I2C_TxAbortSource_Get(I2CInstance *i2cX)
{
    return LL_I2C_TxAbortSource_Get(i2cX);
}

uint8_t HAL_I2C_Rx_FIFO_Level(I2CInstance *i2cX)
{
    return LL_I2C_Rx_FIFO_Level(i2cX);
}

// NOTE: 根据时钟源计算CLK，然后配置寄存器。
void HAL_I2C_SetSpeed(I2CInstance *i2cX, uint32_t src_clock, I2C_SpeedMode_Enum_t speedMode, I2C_Timing_Enum_t timing, I2C_Timing_Setting_t *setting)
{
    LL_I2C_SetSpeed(i2cX, speedMode);

    if(speedMode == I2C_STANDARD_MODE)			//100kbit
    {
        if(timing == I2C_TIMING_DEFAULT)
        {
            // 经过计算，src_clock = 80M 时，ss_scl_hcnt = 393, ss_scl_lcnt = 391
            LL_I2C_StandardSpeed_SCL_HighCount_Set(i2cX, src_clock/2/100000 - LL_I2C_FastSpeed_SPKLEN_Get(i2cX) - 7);
            LL_I2C_StandardSpeed_SCL_LowCount_Set(i2cX,  src_clock/2/100000 - 1);

            LL_I2C_SDA_RxHold_Set(i2cX, (LL_I2C_StandardSpeed_SCL_HighCount_Get(i2cX) - LL_I2C_FastSpeed_SPKLEN_Get(i2cX) - 3) / 2);
            LL_I2C_SDA_TxHold_Set(i2cX, (LL_I2C_StandardSpeed_SCL_HighCount_Get(i2cX) - LL_I2C_FastSpeed_SPKLEN_Get(i2cX) - 3) / 2);
        }
        else
        {
            LL_I2C_StandardSpeed_SCL_HighCount_Set(i2cX, setting->ss_scl_hcnt);
            LL_I2C_StandardSpeed_SCL_LowCount_Set(i2cX, setting->ss_scl_lcnt);

            LL_I2C_SDA_RxHold_Set(i2cX, setting->ic_sda_rx_hold);
            LL_I2C_SDA_TxHold_Set(i2cX, setting->ic_sda_tx_hold);
        }
    }
    else if(speedMode == I2C_FAST_MODE)
    {
        if(timing == I2C_TIMING_DEFAULT)
		{
            LL_I2C_FastSpeed_SCL_HighCount_Set(i2cX, src_clock/2/400000 - LL_I2C_FastSpeed_SPKLEN_Get(i2cX) - 7);
            LL_I2C_FastSpeed_SCL_LowCount_Set(i2cX,  src_clock/2/400000 - 1);

            LL_I2C_SDA_RxHold_Set(i2cX, (LL_I2C_FastSpeed_SCL_HighCount_Get(i2cX) - LL_I2C_FastSpeed_SPKLEN_Get(i2cX) - 3) / 2);
            LL_I2C_SDA_TxHold_Set(i2cX, (LL_I2C_FastSpeed_SCL_HighCount_Get(i2cX) - LL_I2C_FastSpeed_SPKLEN_Get(i2cX) - 3) / 2);
        }
        else
        {
            LL_I2C_FastSpeed_SCL_HighCount_Set(i2cX, setting->fs_scl_hcnt);
            LL_I2C_FastSpeed_SCL_LowCount_Set(i2cX, setting->fs_scl_lcnt);

            LL_I2C_SDA_RxHold_Set(i2cX, setting->ic_sda_rx_hold);
            LL_I2C_SDA_TxHold_Set(i2cX, setting->ic_sda_tx_hold);
        }
    }
    else if(speedMode == I2C_HIGHSPEED_MODE)
    {
        if(timing == I2C_TIMING_DEFAULT)
        {
            LL_I2C_FastSpeed_SCL_HighCount_Set(i2cX, src_clock/2/400000 - LL_I2C_FastSpeed_SPKLEN_Get(i2cX) - 7);
            LL_I2C_FastSpeed_SCL_LowCount_Set(i2cX,  src_clock/2/400000 -1);

            LL_I2C_HighSpeed_SCL_HighCount_Set(i2cX, src_clock/2/3400000 - LL_I2C_HighSpeed_SPKLEN_Get(i2cX) - 1);
            LL_I2C_HighSpeed_SCL_LowCount_Set(i2cX,  src_clock/2/3400000 - 1);

            LL_I2C_SDA_RxHold_Set(i2cX, (LL_I2C_HighSpeed_SCL_HighCount_Get(i2cX) - LL_I2C_HighSpeed_SPKLEN_Get(i2cX) -3 ) / 2);
            LL_I2C_SDA_TxHold_Set(i2cX, (LL_I2C_HighSpeed_SCL_HighCount_Get(i2cX) - LL_I2C_HighSpeed_SPKLEN_Get(i2cX) - 3) / 2);
        }
        else
        {
            LL_I2C_FastSpeed_SCL_HighCount_Set(i2cX, setting->fs_scl_hcnt);
            LL_I2C_FastSpeed_SCL_LowCount_Set(i2cX, setting->fs_scl_lcnt);

            LL_I2C_HighSpeed_SCL_HighCount_Set(i2cX, setting->hs_scl_hcnt);
            LL_I2C_HighSpeed_SCL_LowCount_Set(i2cX, setting->hs_scl_lcnt);

            LL_I2C_SDA_RxHold_Set(i2cX, setting->ic_sda_rx_hold);
            LL_I2C_SDA_TxHold_Set(i2cX, setting->ic_sda_tx_hold);
        }
	}
}

// TODO: rename
void HAL_I2C_HighSpeed_MasterAddr_Set(I2CInstance *i2cX, uint8_t maddr)
{
    LL_I2C_HighSpeed_MasterAddr_Set(i2cX, maddr);
}

// TODO: rename.
void HAL_I2C_DMA_Ctrl(I2CInstance *i2cX, uint8_t tx_dma_en, uint8_t rx_dma_en, uint8_t tx_data_level, uint8_t rx_data_level)
{
    LL_I2C_DMACtrl(i2cX, tx_dma_en, rx_dma_en);

    LL_I2C_DMA_DataLevel_Set(i2cX, tx_data_level, rx_data_level);
}

void HAL_I2C_HighSpeed_SPKLEN_Set(I2CInstance *i2cX, uint8_t spklen)
{
    LL_I2C_HighSpeed_SPKLEN_Set(i2cX, spklen);
}

void HAL_I2C_FastSpeed_SPKLEN_Set(I2CInstance *i2cX, uint8_t spklen)
{
    LL_I2C_FastSpeed_SPKLEN_Set(i2cX, spklen);
}

uint8_t HAL_I2C_IntClr_RestartDet(I2CInstance *i2cX)
{
    return LL_I2C_IntClr_RestartDet(i2cX);
}

uint8_t HAL_I2C_IntClr_GenCall(I2CInstance *i2cX)
{
    return LL_I2C_IntClr_GenCall(i2cX);
}

uint8_t HAL_I2C_IntClr_StartDet(I2CInstance *i2cX)
{
    return LL_I2C_IntClr_StartDet(i2cX);
}

uint8_t HAL_I2C_IntClr_StopDet(I2CInstance *i2cX)
{
    return LL_I2C_IntClr_StopDet(i2cX);
}

uint8_t HAL_I2C_IntCrl_Activity(I2CInstance *i2cX)
{
    return LL_I2C_IntClr_Activity(i2cX);
}

uint8_t HAL_I2C_IntClr_RxDone(I2CInstance *i2cX)
{
    return LL_I2C_IntClr_RxDone(i2cX);
}

uint8_t HAL_I2C_IntClr_TxAbort(I2CInstance *i2cX)
{
    return LL_I2C_IntClr_TxAbort(i2cX);
}

uint8_t HAL_I2C_IntClr_RdReq(I2CInstance *i2cX)
{
    return LL_I2C_IntClr_RdReq(i2cX);
}

uint8_t HAL_I2C_IntClr_RxOver(I2CInstance *i2cX)
{
    return LL_I2C_IntClr_RxOver(i2cX);
}

uint8_t HAL_I2C_IntClr_TxOver(I2CInstance *i2cX)
{
    return LL_I2C_IntClr_TxOver(i2cX);
}

uint8_t HAL_I2C_IntClr_RxUnder(I2CInstance *i2cX)
{
    return LL_I2C_IntClr_RxUnder(i2cX);
}

uint8_t HAL_I2C_IntClr_All(I2CInstance *i2cX)
{
    return LL_I2C_IntClr_All(i2cX);
}

uint32_t HAL_I2C_RawIntStatus(I2CInstance *i2cX)
{
    return LL_I2C_RawIntStatus(i2cX);
}

void HAL_I2C_IntMask_Set(I2CInstance *i2cX, uint16_t mask)
{
    LL_I2C_IntMask_Set(i2cX, mask);
}

uint16_t HAL_I2C_IntMask_Get(I2CInstance *i2cX)
{
    return LL_I2C_IntMask_Get(i2cX);
}

uint16_t HAL_I2C_IntStatus(I2CInstance *i2cX)
{
    return LL_I2C_IntStatus(i2cX);
}

uint8_t HAL_I2C_IC_Enable_Status(I2CInstance *i2cX)
{
    return LL_I2C_IC_Enable_Status(i2cX);
}

uint32_t HAL_I2C_IC_Status(I2CInstance *i2cX)
{
    return LL_I2C_IC_Status(i2cX);
}

void HAL_I2C_Init(I2CInstance *i2cX, uint32_t src_clk, I2C_InitTypeDef config, I2C_Timing_Setting_t *setting)
{
    HAL_I2C_Enable(i2cX, 0);

    HAL_I2C_SetMode(i2cX, config.mode);
    HAL_I2C_Set7_10Addressing_Master(i2cX, config.addr_type);
    HAL_I2C_Set7_10Addressing_Slave(i2cX, config.addr_type);
    HAL_I2C_SetTarAddr(i2cX, config.dev_addr >> 1);
    HAL_I2C_SetSpeed(i2cX, src_clk, config.speed, config.timing, setting);
    HAL_I2C_RestartMode_Enable(i2cX, config.restart_en);

    HAL_I2C_Enable(i2cX, 1);
}
