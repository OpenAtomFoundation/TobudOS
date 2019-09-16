/**
  ******************************************************************************
  * @file    i2c.h
  * @author  YANDLD
  * @version V2.6
  * @date    2015.06.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_I2C_H__
#define __CH_LIB_I2C_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include <stdint.h>


#define HW_I2C0         (0x00U)
#define HW_I2C1         (0x01U)
#define HW_I2C2         (0x02U)

//!< I2C QuickInit macro
#define I2C1_SCL_PE01_SDA_PE00  (0X000081A1U)
#define I2C0_SCL_PE19_SDA_PE18  (0X0000A520U)
#define I2C0_SCL_PF22_SDA_PF23  (0X0000ACA8U)
#define I2C0_SCL_PB00_SDA_PB01  (0X00008088U)
#define I2C0_SCL_PB02_SDA_PB03  (0X00008488U)
#define I2C1_SCL_PC10_SDA_PC11  (0X00009491U)
#define I2C0_SCL_PD08_SDA_PD09  (0X00009098U)
#define I2C0_SCL_PE24_SDA_PE25  (0X0000B160U)
#define I2C1_SCL_PC01_SDA_PC02  (0X00008291U)


uint8_t I2C_Init(uint32_t MAP, uint32_t baudrate);
int32_t I2C_BurstRead(uint32_t instance, uint8_t chipAddr, uint32_t addr, uint32_t addrLen, uint8_t* buf, uint32_t len);
uint8_t I2C_BurstWrite(uint32_t instance ,uint8_t chipAddr, uint32_t addr, uint32_t addrLen, uint8_t *buf, uint32_t len);
uint8_t I2C_ReadSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t* buf);
uint8_t I2C_WriteSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t buf);
int SCCB_ReadSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t* buf);
int SCCB_WriteSingleRegister(uint32_t instance, uint8_t chipAddr, uint8_t addr, uint8_t buf);
/* test function */
uint8_t I2C_Probe(uint32_t instance, uint8_t chipAddr);
void I2C_Scan(uint32_t MAP);

#ifdef __cplusplus
}
#endif


#endif


