/**
  ******************************************************************************
  * @file    stm8l15x_i2c.h
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   This file contains all the functions prototypes for the I2C firmware 
  *          library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************  
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8L15x_I2C_H
#define __STM8L15x_I2C_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Driver
  * @{
  */

/** @addtogroup I2C
 * @{
 */

/* Exported types ------------------------------------------------------------*/
/** @defgroup I2C_Exported_Types
 * @{
 */
/** @defgroup I2C_mode
  * @{
  */
typedef enum
{
  I2C_Mode_I2C           = (uint8_t)0x00,   /*!< I2C mode */
  I2C_Mode_SMBusDevice   = (uint8_t)0x02,   /*!< SMBus Device mode */
  I2C_Mode_SMBusHost     = (uint8_t)0x0A    /*!< SMBus Host mode */
} I2C_Mode_TypeDef;

/**
  * @}
  */

/** @defgroup I2C_duty_cycle_in_fast_mode
  * @{
  */
typedef enum
{
  I2C_DutyCycle_2    = (uint8_t)0x00,  /*!< Fast mode Tlow/THigh = 2 */
  I2C_DutyCycle_16_9 = (uint8_t)0x40   /*!< Fast mode Tlow/Thigh = 16/9 */
} I2C_DutyCycle_TypeDef;

/**
  * @}
  */

/** @defgroup I2C_acknowledgement
  * @{
  */
typedef enum
{
  I2C_Ack_Disable = (uint8_t)0x00,  /*!< No acknowledge */
  I2C_Ack_Enable  = (uint8_t)0x04   /*!< Acknowledge Enabled */
} I2C_Ack_TypeDef;
/**
  * @}
  */

/** @defgroup  I2C_Position_Acknowledgement
  * @{
  */
typedef enum
{
  I2C_AckPosition_Current = (uint8_t)0x00,   /*!< Acknowledge on the current byte */
  I2C_AckPosition_Next    = (uint8_t)0x08    /*!< Acknowledge on the next byte */
} I2C_AckPosition_TypeDef;

/**
  * @}
  */

/** @defgroup I2C_acknowledged_address
  * @{
  */
typedef enum
{
  I2C_AcknowledgedAddress_7bit  = (uint8_t)0x00,  /*!< 7-bit slave address (10-bit address not acknowledged) */
  I2C_AcknowledgedAddress_10bit = (uint8_t)0x80   /*!< 10-bit slave address (7-bit address not acknowledged) */
} I2C_AcknowledgedAddress_TypeDef;
/**
  * @}
  */

/** @defgroup I2C_transfer_direction
  * @{
  */
  
/**
  * Warning: the values correspond to the ADD0 bit position in the OARL register
  */
typedef enum
{
  I2C_Direction_Transmitter = (uint8_t)0x00,  /*!< Transmission direction */
  I2C_Direction_Receiver    = (uint8_t)0x01   /*!< Reception direction */
} I2C_Direction_TypeDef;

/**
  * @}
  */

/** @defgroup I2C_SMBus_alert_pin_level
  * @{
  */
typedef enum
{
  I2C_SMBusAlert_High = (uint8_t)0x00,  /*!< SMBAlert pin high */
  I2C_SMBusAlert_Low  =  (uint8_t)0x01   /*!< SMBAlert pin Low */
} I2C_SMBusAlert_TypeDef;

/**
  * @}
  */

/** @defgroup I2C_PEC_position
  * @{
  */
typedef enum
{
  I2C_PECPosition_Current = (uint8_t)0x00,  /*!< Current byte in shift register is PEC */
  I2C_PECPosition_Next    = (uint8_t)0x08   /*!< Next  byte in shift register is PEC */
} I2C_PECPosition_TypeDef;
/**
  * @}
  */

/** @defgroup I2C_flags_definition
  * @{
  */
/**
  * @brief Elements values convention: 0xXXYY
  *  X = SRx registers index
  *      X = 1 : SR1
  *      X = 2 : SR2
  *      X = 3 : SR3
  *  Y = Flag mask in the register
  */

typedef enum
{
  /* SR1 register flags */
  I2C_FLAG_TXE        = (uint16_t)0x0180,  /*!< Transmit Data Register Empty flag */
  I2C_FLAG_RXNE       = (uint16_t)0x0140,  /*!< Read Data Register Not Empty flag */
  I2C_FLAG_STOPF      = (uint16_t)0x0110,  /*!< Stop detected flag */
  I2C_FLAG_ADD10      = (uint16_t)0x0108,  /*!< 10-bit Header sent flag */
  I2C_FLAG_BTF        = (uint16_t)0x0104,  /*!< Data Byte Transfer Finished flag */
  I2C_FLAG_ADDR       = (uint16_t)0x0102,  /*!< Address Sent/Matched (master/slave) flag */
  I2C_FLAG_SB         = (uint16_t)0x0101,  /*!< Start bit sent flag */

  /* SR2 register flags */
  I2C_FLAG_SMBALERT   = (uint16_t)0x0280,  /*!< SMBUS Alert flag */
  I2C_FLAG_TIMEOUT     = (uint16_t)0x0240,  /*!< Time out flag */
  I2C_FLAG_WUFH       = (uint16_t)0x0220,  /*!< Wake Up From Halt flag */
  I2C_FLAG_PECERR     = (uint16_t)0x0210,  /*!< PEC error flag */
  I2C_FLAG_OVR        = (uint16_t)0x0208,  /*!< Overrun/Underrun flag */
  I2C_FLAG_AF         = (uint16_t)0x0204,  /*!< Acknowledge Failure flag */
  I2C_FLAG_ARLO       = (uint16_t)0x0202,  /*!< Arbitration Loss flag */
  I2C_FLAG_BERR       = (uint16_t)0x0201,  /*!< Misplaced Start or Stop condition */

  /* SR3 register flags */
  I2C_FLAG_DUALF    = (uint16_t)0x0380,  /*!< DUAL Flag */
  I2C_FLAG_SMBHOST    = (uint16_t)0x0340,  /*!< SMBUS host Flag */
  I2C_FLAG_SMBDEFAULT = (uint16_t)0x0320,  /*!< SMBUS default flag */
  I2C_FLAG_GENCALL    = (uint16_t)0x0310,  /*!< General Call header received Flag */
  I2C_FLAG_TRA        = (uint16_t)0x0304,  /*!< Transmitter Receiver flag */
  I2C_FLAG_BUSY       = (uint16_t)0x0302,  /*!< Bus Busy flag */
  I2C_FLAG_MSL        = (uint16_t)0x0301   /*!< Master Slave flag */
} I2C_FLAG_TypeDef;


/**
  * @}
  */

/** @defgroup I2C_interrupts_definition
  * @{
  */

/**
  * @brief I2C Pending bits
  * Elements values convention: 0xXYZZ
  *  X = SRx registers index
  *      X = 0 : ITR
  *      X = 1 : SR1
  *      X = 2 : SR2
  *  Y = Position of the corresponding Interrupt
  *  ZZ = flag mask in the dedicated register(X register)
  */

typedef enum
{
  I2C_IT_ERR             = (uint16_t)0x0001,   /*!< Error Interruption */
  I2C_IT_EVT             = (uint16_t)0x0002,   /*!< Event Interruption */
  I2C_IT_BUF             = (uint16_t)0x0004,   /*!< Buffer Interruption */
  /* SR1 register*/
  I2C_IT_TXE             = (uint16_t)0x1680,   /*!< Transmit Data Register Empty  */
  I2C_IT_RXNE            = (uint16_t)0x1640,   /*!< Read Data Register Not Empty  */
  I2C_IT_STOPF           = (uint16_t)0x1210,   /*!< Stop detected  */
  I2C_IT_ADD10           = (uint16_t)0x1208,   /*!< 10-bit Header sent */
  I2C_IT_BTF             = (uint16_t)0x1204,   /*!< Data Byte Transfer Finished  */
  I2C_IT_ADDR            = (uint16_t)0x1202,   /*!< Address Sent/Matched (master/slave)  */
  I2C_IT_SB              = (uint16_t)0x1201,   /*!< Start bit sent  */

  /* SR2 register*/
  I2C_IT_SMBALERT        = (uint16_t)0x2180,   /*!< SMBUS alert  */
  I2C_IT_TIMEOUT         = (uint16_t)0x2140,   /*!< Time out  */
  I2C_IT_WUFH            = (uint16_t)0x2220,   /*!< PEC error  */
  I2C_IT_PECERR          = (uint16_t)0x2110,   /*!< Wake Up From Halt  */
  I2C_IT_OVR             = (uint16_t)0x2108,   /*!< Overrun/Underrun  */
  I2C_IT_AF              = (uint16_t)0x2104,   /*!< Acknowledge Failure  */
  I2C_IT_ARLO            = (uint16_t)0x2102,   /*!< Arbitration Loss  */
  I2C_IT_BERR            = (uint16_t)0x2101    /*!< Misplaced Start or Stop condition */
} I2C_IT_TypeDef;
/**
  * @}
  */
  
/** @defgroup I2C_Events
  * @{
  */

/**
  * @brief I2C possible events
  * Values convention: 0xXXYY
  * XX = Event SR3 corresponding value
  * YY = Event SR1 corresponding value
  * @note if Event = EV3_2 the rule above does not apply
  * YY = Event SR2 corresponding value
  */

typedef enum
{
  /**
 ===============================================================================
               I2C Master Events (Events grouped in order of communication)                   
 ===============================================================================  
 */
/**
  * @brief  Communication start
  *
  * After sending the START condition (I2C_GenerateSTART() function) the master
  * has to wait for this event. It means that the Start condition has been correctly
  * released on the I2C bus (the bus is free, no other devices is communicating).
  *
  */
  /* --EV5 */
  I2C_EVENT_MASTER_MODE_SELECT               = (uint16_t)0x0301,  /*!< BUSY, MSL and SB flag */

/**
  * @brief  Address Acknowledge
  *
  * After checking on EV5 (start condition correctly released on the bus), the
  * master sends the address of the slave(s) with which it will communicate
  * (I2C_Send7bitAddress() function, it also determines the direction of the communication:
  * Master transmitter or Receiver).
  * Then the master has to wait that a slave acknowledges his address.
  * If an acknowledge is sent on the bus, one of the following events will
  * be set:
  *
  *  1) In case of Master Receiver (7-bit addressing):
  *  the I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED event is set.
  *
  *  2) In case of Master Transmitter (7-bit addressing):
  *  the I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED is set
  *
  *  3) In case of 10-Bit addressing mode, the master (just after generating the START
  *  and checking on EV5) has to send the header of 10-bit addressing mode (I2C_SendData()
  *  function).
  *  Then master should wait on EV9. It means that the 10-bit addressing
  *  header has been correctly sent on the bus.
  *  Then master should send the second part of the 10-bit address (LSB) using
  *  the function I2C_Send7bitAddress(). Then master should wait for event EV6.
  *
  */
  /* --EV6 */
  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED = (uint16_t)0x0782,  /*!< BUSY, MSL, ADDR, TXE and TRA flags */
  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED    = (uint16_t)0x0302,  /*!< BUSY, MSL and ADDR flags */
  /* --EV9 */
  I2C_EVENT_MASTER_MODE_ADDRESS10            = (uint16_t)0x0308,  /*!< BUSY, MSL and ADD10 flags */

/**
  * @brief Communication events
  *
  * If a communication is established (START condition generated and slave address
  * acknowledged) then the master has to check on one of the following events for
  * communication procedures:
  *
  * 1) Master Receiver mode: The master has to wait on the event EV7 then to read
  *    the data received from the slave (I2C_ReceiveData() function).
  *
  * 2) Master Transmitter mode: The master has to send data (I2C_SendData()
  *    function) then to wait on event EV8 or EV8_2.
  *    These two events are similar:
  *     - EV8 means that the data has been written in the data register and is
  *       being shifted out.
  *     - EV8_2 means that the data has been physically shifted out and output
  *       on the bus.
  *     In most cases, using EV8 is sufficient for the application.
  *     Using EV8_2 leads to a slower communication but ensure more reliable test.
  *     EV8_2 is also more suitable than EV8 for testing on the last data transmission
  *     (before Stop condition generation).
  *
  *  @note In case the user software does not guarantee that this event EV7 is
  *  managed before the current byte end of transfer, then user may check on EV7
  *  and BTF flag at the same time (ie. (I2C_EVENT_MASTER_BYTE_RECEIVED | I2C_FLAG_BTF)).
  *  In this case the communication may be slower.
  *
  */
  /* Master RECEIVER mode -----------------------------*/
  /* --EV7 */
  I2C_EVENT_MASTER_BYTE_RECEIVED             = (uint16_t)0x0340,  /*!< BUSY, MSL and RXNE flags */

  /* Master TRANSMITTER mode --------------------------*/
  /* --EV8 */
  I2C_EVENT_MASTER_BYTE_TRANSMITTING         = (uint16_t)0x0780,  /*!< TRA, BUSY, MSL, TXE flags */
  /* --EV8_2 */

  I2C_EVENT_MASTER_BYTE_TRANSMITTED          = (uint16_t)0x0784,  /*!< EV8_2: TRA, BUSY, MSL, TXE and BTF flags */

/**
 ===============================================================================
               I2C Slave Events (Events grouped in order of communication)                  
 ===============================================================================  
 */
 
/**
  * @brief  Communication start events
  *
  * Wait on one of these events at the start of the communication. It means that
  * the I2C peripheral detected a Start condition on the bus (generated by master
  * device) followed by the peripheral address.
  * The peripheral generates an ACK condition on the bus (if the acknowledge
  * feature is enabled through function I2C_AcknowledgeConfig()) and the events
  * listed above are set :
  *
  * 1) In normal case (only one address managed by the slave), when the address
  *   sent by the master matches the own address of the peripheral (configured by
  *   I2C_OwnAddress1 field) the I2C_EVENT_SLAVE_XXX_ADDRESS_MATCHED event is set
  *   (where XXX could be TRANSMITTER or RECEIVER).
  *
  * 2) In case the address sent by the master matches the second address of the 
  *   peripheral (configured by the function I2C_OwnAddress2Config() and enabled 
  *   by the function I2C_DualAddressCmd()) the events I2C_EVENT_SLAVE_XXX_SECONDADDRESS_MATCHED 
  *   (where XXX could be TRANSMITTER or RECEIVER) are set.
  *  
  * 3) In case the address sent by the master is General Call (address 0x00) and 
  *   if the General Call is enabled for the peripheral (using function I2C_GeneralCallCmd()) 
  *   the following event is set I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED.  
  * 
  */

  /* --EV1  (all the events below are variants of EV1) */
  /* 1) Case of One Single Address managed by the slave */
  I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED    = (uint16_t)0x0202,  /*!< BUSY and ADDR flags */
  I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED = (uint16_t)0x0682,  /*!< TRA, BUSY, TXE and ADDR flags */

  /* 2) Case of Dual address managed by the slave */
  I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED    = (uint16_t)0x8200,  /*! DUALF and BUSY flags */
  I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED = (uint16_t)0x8680,  /*! DUALF, TRA, BUSY and TXE flags */

  /* 3) Case of General Call enabled for the slave */
  I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED  = (uint16_t)0x1200,  /*!< EV2: GENCALL and BUSY flags */

  /**
    * @brief  Communication events
    *
    * Wait on one of these events when EV1 has already been checked :
    *
    * - Slave RECEIVER mode:
    *     - EV2: When the application is expecting a data byte to be received.
    *     - EV4: When the application is expecting the end of the communication:
    *       master sends a stop condition and data transmission is stopped.
    *
    * - Slave Transmitter mode:
    *    - EV3: When a byte has been transmitted by the slave and the application
    *      is expecting the end of the byte transmission.
    *      The two events I2C_EVENT_SLAVE_BYTE_TRANSMITTED and I2C_EVENT_SLAVE_BYTE_TRANSMITTING
    *      are similar. The second one can optionally be used when the user software
    *      doesn't guarantee the EV3 is managed before the current byte end of transfer.
    *    - EV3_2: When the master sends a NACK in order to tell slave that data transmission
    *      shall end (before sending the STOP condition).
    *      In this case slave has to stop sending data bytes and expect a Stop
    *      condition on the bus.
    *
    *  @note In case the  user software does not guarantee that the event EV2 is
    *  managed before the current byte end of transfer, then user may check on EV2
    *  and BTF flag at the same time (ie. (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_BTF)).
    *  In this case the communication may be slower.
    *
    */
  /* Slave RECEIVER mode --------------------------*/
  /* --EV2 */
  I2C_EVENT_SLAVE_BYTE_RECEIVED              = (uint16_t)0x0240,  /*!< BUSY and RXNE flags */
  /* --EV4  */
  I2C_EVENT_SLAVE_STOP_DETECTED              = (uint16_t)0x0010,  /*!< STOPF flag */

  /* Slave TRANSMITTER mode -----------------------*/
  /* --EV3 */
  I2C_EVENT_SLAVE_BYTE_TRANSMITTED           = (uint16_t)0x0684,  /*!< TRA, BUSY, TXE and BTF flags */
  I2C_EVENT_SLAVE_BYTE_TRANSMITTING          = (uint16_t)0x0680,  /*!< TRA, BUSY and TXE flags */
  /* --EV3_2 */
  I2C_EVENT_SLAVE_ACK_FAILURE                = (uint16_t)0x0004  /*!< AF flag */
} I2C_Event_TypeDef;

/**
  * @}
  */

/** @defgroup  I2C_Registers
  * @{
  */
typedef enum
{
  I2C_Register_CR1    = (uint8_t)0x00,   /*!< Control register 1 */
  I2C_Register_CR2    = (uint8_t)0x01,   /*!< Control register 2 */
  I2C_Register_FREQR  = (uint8_t)0x02,   /*!< Frequency register */
  I2C_Register_OARL   = (uint8_t)0x03,   /*!< Own address register LSB */
  I2C_Register_OARH   = (uint8_t)0x04,   /*!< Own address register MSB */
  I2C_Register_DR     = (uint8_t)0x06,   /*!< Data register */
  I2C_Register_SR1    = (uint8_t)0x07,   /*!< Status register 1 */
  I2C_Register_SR2    = (uint8_t)0x08,   /*!< Status register 2 */
  I2C_Register_SR3    = (uint8_t)0x09,   /*!< Status register 3 */
  I2C_Register_ITR    = (uint8_t)0x0A,   /*!< Interrupt and DMA register */
  I2C_Register_CCRL   = (uint8_t)0x0B,   /*!< Clock control register low */
  I2C_Register_CCRH   = (uint8_t)0x0C,   /*!< Clock control register high */
  I2C_Register_TRISER = (uint8_t)0x0D,   /*!< TRISE register */
  I2C_Register_PECR   = (uint8_t)0x0E    /*!< PEC register */
} I2C_Register_TypeDef;
/**
  * @}
  */
  
/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/
/** @defgroup I2C_Exported_Constants
  * @{
  */
#define I2C_MAX_STANDARD_FREQ ((uint32_t)100000)
#define I2C_MAX_FAST_FREQ     ((uint32_t)400000)

/**
  *@}
  */

/* Exported macro -----------------------------------------------------------*/
/** @defgroup I2C_Exported_Macros
  * @{
  */

/**
  * @brief Macro used by the assert function to check the different functions parameters.
  */

/**
  * @brief  Macro used by the assert function to check the different I2C modes.
  */
#define IS_I2C_MODE(MODE)(((MODE) == I2C_Mode_I2C) || \
                          ((MODE) == I2C_Mode_SMBusDevice) || \
                          ((MODE) == I2C_Mode_SMBusHost))

/**
  * @brief  Macro used by the assert function to check the different I2C duty cycles.
  */
#define IS_I2C_DUTY_CYCLE(CYCLE)(((CYCLE) == I2C_DutyCycle_2) || \
                                 ((CYCLE) == I2C_DutyCycle_16_9))

/**
  * @brief  Macro used by the assert function to check the different acknowledgement configuration
  */
#define IS_I2C_ACK_STATE(STATE) (((STATE) == I2C_Ack_Disable) || \
                                 ((STATE) == I2C_Ack_Enable))
/**
  * @brief  Macro used by the assert function to check the different acknowledgement position
  */
#define IS_I2C_ACK_POSITION(POSITION) (((POSITION) == I2C_AckPosition_Next) || \
                                       ((POSITION) == I2C_AckPosition_Current))

/**
  * @brief  Macro used by the assert function to check the different I2C PEC positions.
  */
#define IS_I2C_PEC_POSITION(POSITION) (((POSITION) == I2C_PECPosition_Current) || \
                                       ((POSITION) == I2C_PECPosition_Next))

/**
  * @brief  Macro used by the assert function to check the different I2C addressing modes.
  */
#define IS_I2C_ACKNOWLEDGE_ADDRESS(ADDMODE) (((ADDMODE) == I2C_AcknowledgedAddress_7bit) || \
    ((ADDMODE) == I2C_AcknowledgedAddress_10bit))

/**
  * @brief  Macro used by the assert function to check the different I2C SMBus Alert pin configuration.
  */
#define IS_I2C_SMBUS_ALERT(ALERT) (((ALERT) == I2C_SMBusAlert_High) || \
                                   ((ALERT) == I2C_SMBusAlert_Low))

/**
  * @brief  Macro used by the assert function to check the different I2C communication direction.
  */
#define IS_I2C_DIRECTION(DIR)(((DIR) == I2C_Direction_Transmitter) || \
                              ((DIR) == I2C_Direction_Receiver ))

/**
  * @brief  Macro used by the assert function to check the different I2C flags.
  */
#define IS_I2C_GET_FLAG(FLAG) (((FLAG) == I2C_FLAG_TXE)        || \
                               ((FLAG) == I2C_FLAG_RXNE)       || \
                               ((FLAG) == I2C_FLAG_STOPF)      || \
                               ((FLAG) == I2C_FLAG_ADD10)      || \
                               ((FLAG) == I2C_FLAG_BTF)        || \
                               ((FLAG) == I2C_FLAG_ADDR)       || \
                               ((FLAG) == I2C_FLAG_SB)         || \
                               ((FLAG) == I2C_FLAG_SMBALERT)   || \
                               ((FLAG) == I2C_FLAG_TIMEOUT)    || \
                               ((FLAG) == I2C_FLAG_WUFH)       || \
                               ((FLAG) == I2C_FLAG_PECERR)     || \
                               ((FLAG) == I2C_FLAG_OVR)        || \
                               ((FLAG) == I2C_FLAG_AF)         || \
                               ((FLAG) == I2C_FLAG_ARLO)       || \
                               ((FLAG) == I2C_FLAG_BERR)       || \
                               ((FLAG) == I2C_FLAG_DUALF)      || \
                               ((FLAG) == I2C_FLAG_SMBHOST)    || \
                               ((FLAG) == I2C_FLAG_SMBDEFAULT) || \
                               ((FLAG) == I2C_FLAG_GENCALL)    || \
                               ((FLAG) == I2C_FLAG_TRA)        || \
                               ((FLAG) == I2C_FLAG_BUSY)       || \
                               ((FLAG) == I2C_FLAG_MSL))

/**
  * @brief  Macro used by the assert function to check the I2C flags to clear.
  */
#define IS_I2C_CLEAR_FLAG(FLAG) ((((uint16_t)(FLAG) & (uint16_t)0xFD00) == 0x00) && ((uint16_t)(FLAG) != 0x00))

/**
  * @brief Macro used by the assert_param function in order to check the different
  *  sensitivity values for the Interrupts
  */
#define IS_I2C_CONFIG_IT(IT) ((((uint16_t)(IT) & (uint16_t)0xFFF8) == 0x00) && ((uint16_t)(IT) != 0x00))

/**
  * @brief  Macro used by the assert function to check the different I2C possible
  *   pending bits to clear by writing 0.
  */
#define IS_I2C_CLEAR_IT(IT) ((((uint16_t)(IT) & (uint16_t)0xDC00) == 0x00) && ((uint16_t)(IT) != 0x00))

/**
  * @brief  Macro used by the assert function to check the different I2C possible pending bits.
  */
#define IS_I2C_GET_IT(IT) (((IT) == I2C_IT_OVR)      ||\
                           ((IT) == I2C_IT_AF)       ||\
                           ((IT) == I2C_IT_ARLO)     ||\
                           ((IT) == I2C_IT_BERR)     ||\
                           ((IT) == I2C_IT_TXE)      ||\
                           ((IT) == I2C_IT_RXNE)     ||\
                           ((IT) == I2C_IT_STOPF)    ||\
                           ((IT) == I2C_IT_ADD10)    ||\
                           ((IT) == I2C_IT_BTF)      ||\
                           ((IT) == I2C_IT_ADDR)     ||\
                           ((IT) == I2C_IT_PECERR)   ||\
                           ((IT) == I2C_IT_TIMEOUT)  ||\
                           ((IT) == I2C_IT_SMBALERT) ||\
                           ((IT) == I2C_IT_WUFH)     ||\
                           ((IT) == I2C_IT_SB))
/**
  * @brief  Macro used by the assert function to check the different I2C possible events.
  */
#define IS_I2C_EVENT(EVENT) (((EVENT) == I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED) || \
                             ((EVENT) == I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED) || \
                             ((EVENT) == I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED) || \
                             ((EVENT) == I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED) || \
                             ((EVENT) == I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED) || \
                             ((EVENT) == I2C_EVENT_SLAVE_BYTE_RECEIVED) || \
                             ((EVENT) == (I2C_EVENT_SLAVE_BYTE_RECEIVED | (uint16_t)I2C_FLAG_DUALF)) || \
                             ((EVENT) == (I2C_EVENT_SLAVE_BYTE_RECEIVED | (uint16_t)I2C_FLAG_GENCALL)) || \
                             ((EVENT) == I2C_EVENT_SLAVE_BYTE_TRANSMITTED) || \
                             ((EVENT) == (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | (uint16_t)I2C_FLAG_DUALF)) || \
                             ((EVENT) == (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | (uint16_t)I2C_FLAG_GENCALL)) || \
                             ((EVENT) == I2C_EVENT_SLAVE_ACK_FAILURE) || \
                             ((EVENT) == I2C_EVENT_SLAVE_STOP_DETECTED) || \
                             ((EVENT) == I2C_EVENT_MASTER_MODE_SELECT) || \
                             ((EVENT) == I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) || \
                             ((EVENT) == I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) || \
                             ((EVENT) == I2C_EVENT_MASTER_BYTE_RECEIVED) || \
                             ((EVENT) == I2C_EVENT_MASTER_BYTE_TRANSMITTED) || \
                             ((EVENT) == I2C_EVENT_MASTER_BYTE_TRANSMITTING) || \
                             ((EVENT) == I2C_EVENT_MASTER_MODE_ADDRESS10))
/**
  * @brief  Macro used by the assert function to check the different I2C registers.
  */
#define IS_I2C_REGISTER(REGISTER) (((REGISTER) == I2C_Register_CR1) || \
                                   ((REGISTER) == I2C_Register_CR2) || \
                                   ((REGISTER) == I2C_Register_FREQR) || \
                                   ((REGISTER) == I2C_Register_OARL) || \
                                   ((REGISTER) == I2C_Register_OARH) || \
                                   ((REGISTER) == I2C_Register_DR) || \
                                   ((REGISTER) == I2C_Register_SR1) || \
                                   ((REGISTER) == I2C_Register_SR2) || \
                                   ((REGISTER) == I2C_Register_SR3) || \
                                   ((REGISTER) == I2C_Register_ITR) || \
                                   ((REGISTER) == I2C_Register_CCRL) || \
                                   ((REGISTER) == I2C_Register_CCRH) || \
                                   ((REGISTER) == I2C_Register_TRISER) || \
                                   ((REGISTER) == I2C_Register_PECR))
/**
  * @brief  Macro used by the assert function to check the different I2C possible own address.
  */
#define IS_I2C_OWN_ADDRESS(ADDRESS) ((ADDRESS) <= (uint16_t)0x03FF)

/**
  * @brief  Macro used by the assert function to check the different I2C address
  * The address must be even
  */
#define IS_I2C_ADDRESS(ADD) (((ADD) & (uint8_t)0x01) == (uint8_t)0x00)

/**
  * @brief  Macro used by the assert function to check that I2C Output clock frequency must be between 1Hz and 400kHz.
  */
#define IS_I2C_OUTPUT_CLOCK_FREQ(FREQ) (((FREQ) >= (uint8_t)1) && ((FREQ) <= I2C_MAX_FAST_FREQ))

/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */

/*  Function used to set the I2C configuration to the default reset state *****/
void I2C_DeInit(I2C_TypeDef* I2Cx);

/* Initialization and Configuration functions *********************************/
void I2C_Init(I2C_TypeDef* I2Cx, uint32_t OutputClockFrequency, uint16_t OwnAddress,
              I2C_Mode_TypeDef I2C_Mode, I2C_DutyCycle_TypeDef I2C_DutyCycle,
              I2C_Ack_TypeDef I2C_Ack, I2C_AcknowledgedAddress_TypeDef I2C_AcknowledgedAddress);
void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GeneralCallCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_SoftwareResetCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_StretchClockCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_ARPCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_OwnAddress2Config(I2C_TypeDef* I2Cx, uint8_t Address);
void I2C_DualAddressCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_AckPositionConfig(I2C_TypeDef* I2Cx, I2C_AckPosition_TypeDef I2C_AckPosition);
void I2C_FastModeDutyCycleConfig(I2C_TypeDef* I2Cx, I2C_DutyCycle_TypeDef I2C_DutyCycle);
void I2C_SMBusAlertConfig(I2C_TypeDef* I2Cx, I2C_SMBusAlert_TypeDef I2C_SMBusAlert);
void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, I2C_Direction_TypeDef I2C_Direction);

/* Data transfers functions ***************************************************/ 
void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);
uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);

/* PEC management functions ***************************************************/ 
void I2C_PECPositionConfig(I2C_TypeDef* I2Cx, I2C_PECPosition_TypeDef I2C_PECPosition);
uint8_t I2C_GetPEC(I2C_TypeDef* I2Cx);
void I2C_TransmitPEC(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_CalculatePEC(I2C_TypeDef* I2Cx, FunctionalState NewState);

/* DMA transfers management functions *****************************************/
void I2C_DMACmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_DMALastTransferCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/* Interrupts, events and flags management functions **************************/

void I2C_ITConfig(I2C_TypeDef* I2Cx, I2C_IT_TypeDef I2C_IT, FunctionalState NewState);
uint8_t I2C_ReadRegister(I2C_TypeDef* I2Cx, I2C_Register_TypeDef I2C_Register);

/**
 * @brief
 *  
@verbatim 
================================================================================
                       I2C State Monitoring Functions
================================================================================
 This I2C driver provides three different ways for I2C state monitoring
  depending on the application requirements and constraints:


 1) Basic state monitoring:
    Using I2C_CheckEvent() function:
    It compares the status registers (SR1, SR2 and SR3) content to a given event
    (can be the combination of one or more flags).
    It returns SUCCESS if the current status includes the given flags
    and returns ERROR if one or more flags are missing in the current status.
    - When to use:
      - This function is suitable for most applications as well as for startup
      activity since the events are fully described in the product reference manual
      (RM0031).
      - It is also suitable for users who need to define their own events.
    - Limitations:
      - If an error occurs (ie. error flags are set besides to the monitored flags),
        the I2C_CheckEvent() function may return SUCCESS despite the communication
        hold or corrupted real state.
        In this case, it is advised to use error interrupts to monitor the error
        events and handle them in the interrupt IRQ handler.

        @note
        For error management, it is advised to use the following functions:
          - I2C_ITConfig() to configure and enable the error interrupts (I2C_IT_ERR).
          - I2Cx_IRQHandler() which is called when the I2C interrupts occur.
            Where x is the peripheral instance (I2C1,...)
          - I2C_GetFlagStatus() or I2C_GetITStatus() to be called into the
           I2Cx_IRQHandler() function in order to determine which error occurred.
          - I2C_ClearFlag() or I2C_ClearITPendingBit() and/or I2C_SoftwareResetCmd()
            and/or I2C_GenerateStop() in order to clear the error flag and
            source and return to correct communication status.


  2) Advanced state monitoring:
     Using the function I2C_GetLastEvent() which returns the image of both SR1
     & SR3 status registers in a single word (uint16_t) (Status Register 3 value
     is shifted left by 8 bits and concatenated to Status Register 1).
     - When to use:
       - This function is suitable for the same applications above but it allows to
         overcome the limitations of I2C_GetFlagStatus() function (see below).
         The returned value could be compared to events already defined in the
         library (stm8l15x_i2c.h) or to custom values defined by user.
       - This function is suitable when multiple flags are monitored at the same time.
       - At the opposite of I2C_CheckEvent() function, this function allows user to
         choose when an event is accepted (when all events flags are set and no
         other flags are set or just when the needed flags are set like
         I2C_CheckEvent() function).
     - Limitations:
       - User may need to define his own events.
       - Same remark concerning the error management is applicable for this
         function if user decides to check only regular communication flags (and
         ignores error flags).


 3) Flag-based state monitoring:
     Using the function I2C_GetFlagStatus() which simply returns the status of
     one single flag (ie. I2C_FLAG_RXNE ...).
     - When to use:
        - This function could be used for specific applications or in debug phase.
        - It is suitable when only one flag checking is needed (most I2C events
          are monitored through multiple flags).
     - Limitations:
        - When calling this function, the Status register is accessed. Some flags are
          cleared when the status register is accessed. So checking the status
          of one Flag, may clear other ones.
        - Function may need to be called twice or more in order to monitor one
          single event.
          
          
@endverbatim
 *            
 */

/**
 ===============================================================================
                          1. Basic state monitoring                    
 ===============================================================================
*/  
ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, I2C_Event_TypeDef I2C_Event);
/**
 ===============================================================================
                          2. Advanced state monitoring                   
 ===============================================================================  
 */
I2C_Event_TypeDef I2C_GetLastEvent(I2C_TypeDef* I2Cx);
/**
 ===============================================================================
                          3. Flag-based state monitoring                   
 ===============================================================================  
 */
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, I2C_FLAG_TypeDef I2C_FLAG);

void I2C_ClearFlag(I2C_TypeDef* I2Cx, I2C_FLAG_TypeDef I2C_FLAG);
ITStatus I2C_GetITStatus(I2C_TypeDef* I2Cx, I2C_IT_TypeDef I2C_IT);
void I2C_ClearITPendingBit(I2C_TypeDef* I2Cx, I2C_IT_TypeDef I2C_IT);

#endif /* __STM8L15x_I2C_H */

/**
  * @}
  */
  
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
