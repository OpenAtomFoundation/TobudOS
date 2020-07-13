/*!
 * \file      mpl3115.h
 *
 * \brief     MPL3115 Temperature, pressure and altitude sensor driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#ifndef __MPL3115_H__
#define __MPL3115_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*
 * MPL3115A2 I2C address
 */
#define MPL3115A_I2C_ADDRESS                          0x60

/*
 * MPL3115A2 Registers
 */
#define STATUS_REG            0x00 // STATUS Register

#define OUT_P_MSB_REG         0x01 // Bits 12-19 of 20-bit real-time Pressure sample
#define OUT_P_CSB_REG         0x02 // Bits 4-11 of 20-bit real-time Pressure sample
#define OUT_P_LSB_REG         0x03 // Bits 0-3 of 20-bit real-time Pressure sample
#define OUT_T_MSB_REG         0x04 // Bits 4-11 of 12-bit real-time Temperature sample
#define OUT_T_LSB_REG         0x05 // Bits 0-3 of 12-bit real-time Temperature sample

#define DR_STATUS             0x06 // Data Ready status information

#define OUT_P_DELTA_MSB_REG   0x07 // Bits 12-19 of 20-bit Pressure change data
#define OUT_P_DELTA_CSB_REG   0x08 // Bits 4-11 of 20-bit Pressure change data
#define OUT_P_DELTA_LSB_REG   0x09 // Bits 0-3 of 20-bit Pressure change data
#define OUT_T_DELTA_MSB_REG   0x0A // Bits 4-11 of 12-bit Temperature change data
#define OUT_T_DELTA_LSB_REG   0x0B // Bits 0-3 of 12-bit Temperature change data

#define MPL3115_ID            0x0C // Fixed Device ID Number = 0xC4

#define F_STATUS_REG          0x0D // FIFO Status
#define F_DATA_REG            0x0E // FIFO 8-bit data access
#define F_SETUP_REG           0x0F // FIFO setup
#define TIME_DLY_REG          0x10 // Time since FIFO overflow

#define SYSMOD_REG            0x11 // Current system mode
#define INT_SOURCE_REG        0x12 // Interrupt status
#define PT_DATA_CFG_REG       0x13 // Data event flag configuration

#define BAR_IN_MSB_REG        0x14 // Barometric input for Altitude calculation bits 8-15
#define BAR_IN_LSB_REG        0x15 // Barometric input for Altitude calculation bits 0-7

#define P_TGT_MSB_REG         0x16 // Pressure/Altitude target value bits 8-15
#define P_TGT_LSB_REG         0x17 // Pressure/Altitude target value bits 0-7
#define T_TGT_REG             0x18 // Temperature target value

#define P_WND_MSB_REG         0x19 // Pressure/Altitude window value bits 8-15
#define P_WND_LSB_REG         0x1A // Pressure/Altitude window value bits 0-7
#define T_WND_REG             0x1B // Temperature window value

#define P_MIN_MSB_REG         0x1C // Minimum Pressure/Altitude bits 12-19
#define P_MIN_CSB_REG         0x1D // Minimum Pressure/Altitude bits 4-11
#define P_MIN_LSB_REG         0x1E // Minimum Pressure/Altitude bits 0-3
#define T_MIN_MSB_REG         0x1F // Minimum Temperature bits 8-15
#define T_MIN_LSB_REG         0x20 // Minimum Temperature bits 0-7

#define P_MAX_MSB_REG         0x21 // Maximum Pressure/Altitude bits 12-19
#define P_MAX_CSB_REG         0x22 // Maximum Pressure/Altitude bits 4-11
#define P_MAX_LSB_REG         0x23 // Maximum Pressure/Altitude bits 0-3
#define T_MAX_MSB_REG         0x24 // Maximum Pressure/Altitude bits 8-15
#define T_MAX_LSB_REG         0x25 // Maximum Pressure/Altitude bits 0-7

#define CTRL_REG1             0x26 // CTRL_REG1 System Control 1 Register
#define CTRL_REG2             0x27 // CTRL_REG2 System Control 2 Register
#define CTRL_REG3             0x28 // CTRL_REG3 Interrupt Configuration Register
#define CTRL_REG4             0x29 // CTRL_REG4 Interrupt Enable Register
#define CTRL_REG5             0x2A // CTRL_REG5 Interrupt Output Pin Assignment Register

#define OFF_P_REG             0x2B // Pressure data offset
#define OFF_T_REG             0x2C // Temperature data offset
#define OFF_H_REG             0x2D // Altitude data offset


/*
 * MPL3115A2 Bit Field
 */
#define PTDR                  0x08
#define PDR                   0x04
#define TDR                   0x02
#define ALT                   0x80
#define RAW                   0x40
#define OS2                   0x20
#define OS1                   0x10
#define OS0                   0x08
#define RST                   0x04
#define OST                   0x02
#define SBYB                  0x01
#define OS_1                  0x00
#define OS_2                  OS0
#define OS_4                  OS1
#define OS_8                  OS1 | OS0
#define OS_16                 OS2
#define OS_32                 OS2 | OS0
#define OS_64                 OS2 | OS1
#define OS_128                OS2 | OS1 | OS0
#define DREM                  0x04
#define PDEFE                 0x02
#define TDEFE                 0x01

/*!
 * \brief Initializes the device
 *
 * \retval status [SUCCESS, FAIL]
 */
uint8_t MPL3115Init( void );

/*!
 * \brief Resets the device
 *
 * \retval status [SUCCESS, FAIL]
 */
uint8_t MPL3115Reset( void );

/*!
 * \brief Reads the altitude from the MPL3115
 *
 * \retval altitude Measured altitude
 */
float MPL3115ReadAltitude( void );

/*!
 * \brief Reads the Pressure from the MPL3115
 *
 * \retval pressure Measured pressure
 */
float MPL3115ReadPressure( void );

/*!
 * \brief Reads the Temperature from the MPL3115
 *
 * \retval temperature Measured temperature
 */
float MPL3115ReadTemperature( void );

#ifdef __cplusplus
}
#endif

#endif // __MPL3115_H__
