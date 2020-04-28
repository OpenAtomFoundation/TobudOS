#ifndef __HAL_I2C_H__
#define __HAL_I2C_H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"

#define I2C_RX_TX_FIFO_DEPTH 16

/**
 * @brief enum of I2C mode. I2C module can work in master mode or slave mode.
 */
typedef enum
{
    I2C_SLAVE = 0,
    I2C_MASTER
} I2C_Mode_Enum_t;

/**
 * @brief enum of I2C Speed. I2C module can work at different speed.
          Normally Standard Mode work in less than 100kbps
          Fast Mode work in less than 400kbps
          HighSpeed Mode work in 3.4Mbps
 */
typedef enum
{
    I2C_STANDARD_MODE = 1,
    I2C_FAST_MODE,
    I2C_HIGHSPEED_MODE
} I2C_SpeedMode_Enum_t;

/**
 * @brief enum of Special bit.
           indicates whether software performs a Device-ID, General Call or START BYTE command.
 */
typedef enum
{
    I2C_NORMALLY = 0,
    I2C_GENERALCALL_STARTBYTE
} I2C_Special_Enum_t;

/**
 * @brief enum of I2C Addressing. Can be select to use 7bit addressing or 10bit addressing
 */
typedef enum
{
    I2C_7_ADDRESSING = 0,
    I2C_10_ADDRESSING
} I2C_7_10Addressing_Enum_t;

/**
 * @brief enum of int status.
 */
typedef enum
{
    I2C_INTR_RX_UNDER = 0,
    I2C_INTR_RX_OVER, // 1
    I2C_INTR_RX_FULL,
    I2C_INTR_TX_OVER,
    I2C_INTR_TX_EMPTY,
    I2C_INTR_RD_REQ, // 5
    I2C_INTR_TX_ABORT,
    I2C_INTR_RX_DONE,
    I2C_INTR_ACTIVITY,
    I2C_INTR_STOP_DET,
    I2C_INTR_START_DET, // 10
    I2C_INTR_GEN_CALL,
    I2C_INTR_RESTART_DET,
    I2C_INTR_MST_ON_HOLD,
    I2C_INTR_SCL_STUCK_LOW,	//14
    I2C_INTR_MAX
} I2C_Int_Status_Enum_t;

/**
 * @brief enum of tx abort src.
 */
typedef enum
{
    I2C_ABRT_7BADDR_NOACK = 0,
    I2C_ABRT_10BADDR1_NOACK,
    I2C_ABRT_10BADDR2_NOACK,
    I2C_ABRT_TXDATA_NOACK,
    I2C_ABRT_GCALL_NOACK,
    I2C_ABRT_GCALL_READ,        //5
    I2C_ABRT_HS_ACKDET,
    I2C_ABRT_SBYTE_ACKDET,
    I2C_ABRT_HS_NORSTRT,
    I2C_ABRT_SBYTE_NORSTRT,
    I2C_ABRT_10B_RD_NORSTRT,    //10
    I2C_ABRT_MASTER_DIS,
    I2C_ABRT_ARB_LOST,
    I2C_ABRT_SLVFLUSH_TXFIFO,
    I2C_ABRT_SLV_ARBLOST,
    I2C_ABRT_SLVRD_INTX,        //15
    I2C_ABRT_USER_ABRT,
    I2C_ABRT_SDA_STUCK_AT_LOW,
    I2C_ABRT_DEVICE_NOACK,
    I2C_ABRT_DEVICE_SLVADDR_NOACK,
    I2C_ABRT_DEVICE_WRITE       //20
} I2C_TX_Abort_Src_Enum_t;

/**
 * @brief enum of int mask.
 */
typedef enum
{
    I2C_MASK_RX_UNDER = 0,
    I2C_MASK_RX_OVER,
    I2C_MASK_RX_FULL,
    I2C_MASK_TX_OVER,
    I2C_MASK_TX_EMPTY,
    I2C_MASK_RD_REQ,        //5
    I2C_MASK_TX_ABRT,
    I2C_MASK_RX_DONE,
    I2C_MASK_ACTIVITY,
    I2C_MASK_STOP_DET,
    I2C_MASK_START_DET,     //10
    I2C_MASK_GEN_CALL,
    I2C_MASK_RESTART_DET,
    I2C_MASK_MST_ON_HOLD,
    I2C_MASK_SCL_STUCK_AT_LOW   //14
} I2C_Int_Mask_Enum_t;


/**
 * @brief using the default i2c timing parameter or define your own parameter.
 */
typedef enum
{
    I2C_TIMING_DEFAULT = 0,
    I2C_TIMING_USER_DEFINE
} I2C_Timing_Enum_t;


/**
 * @brief This is the struct to initialize the i2c module.
 */
typedef struct
{
	I2C_Mode_Enum_t mode;                  /**< mode: I2C mode. slave mode or master mode. */
	uint16_t dev_addr;              /**< dev_addr: I2C slave address indicate */
	I2C_7_10Addressing_Enum_t addr_type;   /**< addr_type: 7bit or 10bit addressing select */
	I2C_SpeedMode_Enum_t speed;            /**< speed: select from standard mode, fast mode or high speed mode */
	I2C_Timing_Enum_t timing;          /**< timing: use default timing or define your own i2c timing */
	uint8_t restart_en;             /**< restart_en: Determines whether RESTART conditions may be sent when acting as a master */
} I2C_InitTypeDef;


/**
 * @brief This is the struct to define the i2c timing, only define this when you define your own i2c timing(I2C_TIMING_USER_DEFINE)
 */
typedef struct
{
    uint16_t ss_scl_hcnt;
    uint16_t ss_scl_lcnt;

    uint16_t fs_scl_hcnt;
    uint16_t fs_scl_lcnt;

    uint16_t hs_scl_hcnt;
    uint16_t hs_scl_lcnt;

    uint16_t ic_sda_rx_hold;
    uint16_t ic_sda_tx_hold;
} I2C_Timing_Setting_t;


void HAL_I2C_Enable(I2CInstance *i2cX, uint8_t en);

void HAL_I2C_Abort(I2CInstance *i2cX, uint8_t en);

void HAL_I2C_SetMode(I2CInstance *i2cX, I2C_Mode_Enum_t mode);

I2C_Mode_Enum_t HAL_I2C_GetMode(I2CInstance *i2cX);

void HAL_I2C_SetSpecialBit(I2CInstance *i2cX, I2C_Special_Enum_t special);

void HAL_I2C_GeneralCall_OR_Start(I2CInstance *i2cX, uint8_t start);

void HAL_I2C_RestartMode_Enable(I2CInstance *i2cX, uint8_t en);

void HAL_I2C_Set7_10Addressing_Master(I2CInstance *i2cX, I2C_7_10Addressing_Enum_t addr);

void HAL_I2C_Set7_10Addressing_Slave(I2CInstance *i2cX, I2C_7_10Addressing_Enum_t addr);

void HAL_I2C_SetTarAddr(I2CInstance *i2cX, uint16_t addr);

void HAL_I2C_SlaveAddr_Set(I2CInstance *i2cX, uint16_t addr);

void HAL_I2C_Int_Threshold_Set(I2CInstance *i2cX, uint8_t tx_tl, uint8_t rx_tl);

void HAL_I2C_Read_FIFO(I2CInstance *i2cX, uint8_t *buf, uint32_t length);

void HAL_I2C_Data_Cmd_Pack(I2CInstance *i2cX, uint8_t restart, uint8_t stop, uint8_t cmd, uint8_t dat);

void HAL_I2C_ReadData_Polling(I2CInstance *i2cX, uint8_t *buf, uint32_t length);

void HAL_I2C_ReadData(I2CInstance *i2cX, uint32_t length);

void HAL_I2C_Read_LastByte(I2CInstance *i2cX);

uint32_t HAL_I2C_WriteData(I2CInstance *i2cX, uint8_t *buf, uint32_t length);

void HAL_I2C_Write_LastByte(I2CInstance *i2cX, uint8_t buf);

uint32_t HAL_I2C_SendAddrCmd(I2CInstance *i2cX, uint8_t *regAddr, uint8_t len);

uint8_t HAL_I2C_Tx_FIFO_Level(I2CInstance *i2cX);

uint32_t HAL_I2C_TxAbortSource_Get(I2CInstance *i2cX);

uint8_t HAL_I2C_Rx_FIFO_Level(I2CInstance *i2cX);

void HAL_I2C_SetSpeed(I2CInstance *i2cX, uint32_t src_clock, I2C_SpeedMode_Enum_t speedMode, I2C_Timing_Enum_t timing, I2C_Timing_Setting_t *setting);

void HAL_I2C_HighSpeed_MasterAddr_Set(I2CInstance *i2cX, uint8_t maddr);

void HAL_I2C_DMA_Ctrl(I2CInstance *i2cX, uint8_t tx_dma_en, uint8_t rx_dma_en, uint8_t tx_data_level, uint8_t rx_data_level);

void HAL_I2C_HighSpeed_SPKLEN_Set(I2CInstance *i2cX, uint8_t spklen);

void HAL_I2C_FastSpeed_SPKLEN_Set(I2CInstance *i2cX, uint8_t spklen);

uint8_t HAL_I2C_IntClr_RestartDet(I2CInstance *i2cX);

uint8_t HAL_I2C_IntClr_GenCall(I2CInstance *i2cX);

uint8_t HAL_I2C_IntClr_StartDet(I2CInstance *i2cX);

uint8_t HAL_I2C_IntClr_StopDet(I2CInstance *i2cX);

uint8_t HAL_I2C_IntCrl_Activity(I2CInstance *i2cX);

uint8_t HAL_I2C_IntClr_RxDone(I2CInstance *i2cX);

uint8_t HAL_I2C_IntClr_TxAbort(I2CInstance *i2cX);

uint8_t HAL_I2C_IntClr_RdReq(I2CInstance *i2cX);

uint8_t HAL_I2C_IntClr_RxOver(I2CInstance *i2cX);

uint8_t HAL_I2C_IntClr_TxOver(I2CInstance *i2cX);

uint8_t HAL_I2C_IntClr_RxUnder(I2CInstance *i2cX);

uint8_t HAL_I2C_IntClr_All(I2CInstance *i2cX);

uint32_t HAL_I2C_RawIntStatus(I2CInstance *i2cX);

void HAL_I2C_IntMask_Set(I2CInstance *i2cX, uint16_t mask);

uint16_t HAL_I2C_IntMask_Get(I2CInstance *i2cX);

uint16_t HAL_I2C_IntStatus(I2CInstance *i2cX);

uint8_t HAL_I2C_IC_Enable_Status(I2CInstance *i2cX);

uint32_t HAL_I2C_IC_Status(I2CInstance *i2cX);

void HAL_I2C_Init(I2CInstance *i2cX, uint32_t src_clk, I2C_InitTypeDef config, I2C_Timing_Setting_t *setting);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __HAL_I2C_H__
