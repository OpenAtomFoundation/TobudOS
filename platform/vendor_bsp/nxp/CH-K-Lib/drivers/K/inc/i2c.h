/**
  ******************************************************************************
  * @file    i2c.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片IIC模块的底层功能函数
  ******************************************************************************
  */
#ifndef __CH_LIB_I2C_H__
#define __CH_LIB_I2C_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include <stdint.h>

/**
 * \struct I2C_InitTypeDef
 * \brief I2C初始化结构
 */     
typedef struct
{
    uint32_t instance;    ///< I2C pin select
    uint32_t baudrate;    ///< baudrate some common baudrate: 48000Hz 76000Hz 96000Hz 376000Hz
}I2C_InitTypeDef;

/** 
 * \struct i2c_gpio
 * \brief i2c sda and scl
 */
typedef struct
{
    uint32_t instace;		///< I2C 模块号
    uint32_t sda_pin;		///< I2C 数据线
    uint32_t scl_pin;		///< I2C 时钟线
}i2c_gpio;

/* I2C模块号 */
#define HW_I2C0         (0x00U)   /* I2C模块0，以下依次类推 */
#define HW_I2C1         (0x01U)
#define HW_I2C2         (0x02U)


/* I2C QuickInit macro */
#define I2C1_SCL_PE01_SDA_PE00  (0X000081A1U)
#define I2C0_SCL_PE19_SDA_PE18  (0X0000A520U)
#define I2C0_SCL_PF22_SDA_PF23  (0X0000ACA8U)
#define I2C0_SCL_PB00_SDA_PB01  (0X00008088U)
#define I2C0_SCL_PB02_SDA_PB03  (0X00008488U)
#define I2C1_SCL_PC10_SDA_PC11  (0X00009491U)
#define I2C0_SCL_PD08_SDA_PD09  (0X00009098U)
#define I2C0_SCL_PE24_SDA_PE25  (0X0000B160U)
#define I2C1_SCL_PC01_SDA_PC02  (0X00008291U)
#define I2Cx_SCL_PC14_SDA_PC15  (0X00009C50U)

/**
 * \enum I2C_Direction_Type
 * \brief I2C 读写设置
 */
typedef enum
{
    kI2C_Read,                  /**< I2C Master Read Data */
    kI2C_Write,                 /**< I2C Master Write Data */
    kI2C_DirectionNameCount,
}I2C_Direction_Type; 

/**
 * \enum I2C_ITDMAConfig_Type
 * \brief I2C 中断DMA配置
 */
typedef enum
{
    kI2C_IT_Disable,        /**< Disable Interrupt */
    kI2C_DMA_Disable,       /**< Disable DMA */
    kI2C_IT_BTC,            /**< Byte Transfer Complete Interrupt */
    kI2C_DMA_BTC,           /**< DMA Trigger On Byte Transfer Complete */
}I2C_ITDMAConfig_Type;


/* I2C CallBack Type */
typedef void (*I2C_CallBackType)(void);


uint8_t I2C_QuickInit(uint32_t MAP, uint32_t baudrate);
int I2C_BurstWrite(uint32_t instance ,uint8_t chipAddr, uint32_t addr, uint32_t addrLen, uint8_t *buf, uint32_t len);
int I2C_WriteSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t data);
int I2C_BurstRead(uint32_t instance ,uint8_t chipAddr, uint32_t addr, uint32_t addrLen, uint8_t *buf, uint32_t len);
int I2C_ReadSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t *data);
int SCCB_ReadSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t* data);
int SCCB_WriteSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t data);

/* test function */
int I2C_Probe(uint32_t instance, uint8_t chipAddr);
void I2C_Scan(uint32_t MAP);

#ifdef __cplusplus
}
#endif


#endif


