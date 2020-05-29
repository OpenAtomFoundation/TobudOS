/*!
 * \file      sx1509.h
 *
 * \brief     SX1509 IO expander driver implementation
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
#ifndef __SX1509_H__
#define __SX1509_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define SX1509_I2C_ADDRESS                          0x3E

/*!
 * SX1509 registers addresses
 */
#define RegReset                                    0x7D

#define RegInputDisableB                            0x00
#define RegInputDisableA                            0x01
#define RegLongSlewB                                0x02
#define RegLongSlewA                                0x03
#define RegLowDriveB                                0x04
#define RegLowDriveA                                0x05
#define RegPullUpB                                  0x06
#define RegPullUpA                                  0x07
#define RegPullDownB                                0x08
#define RegPullDownA                                0x09
#define RegOpenDrainB                               0x0A
#define RegOpenDrainA                               0x0B
#define RegPolarityB                                0x0C
#define RegPolarityA                                0x0D
#define RegDirB                                     0x0E
#define RegDirA                                     0x0F
#define RegDataB                                    0x10
#define RegDataA                                    0x11
#define RegInterruptMaskB                           0x12
#define RegInterruptMaskA                           0x13
#define RegSenseHighB                               0x14
#define RegSenseLowB                                0x15
#define RegSenseHighA                               0x16
#define RegSenseLowA                                0x17
#define RegInterruptSourceB                         0x18
#define RegInterruptSourceA                         0x19
#define RegEventStatusB                             0x1A
#define RegEventStatusA                             0x1B
#define RegLevelShifter1                            0x1C
#define RegLevelShifter2                            0x1D
#define RegClock                                    0x1E
#define RegMisc                                     0x1F
#define RegLEDDriverEnableB                         0x20
#define RegLEDDriverEnableA                         0x21
#define RegDebounceConfig                           0x22
#define RegDebounceEnableB                          0x23
#define RegDebounceEnableA                          0x24
#define RegKeyConfig1                               0x25
#define RegKeyConfig2                               0x26
#define RegKeyData1                                 0x27
#define RegKeyData2                                 0x28
#define RegTOn0                                     0x29
#define RegIOn0                                     0x2A
#define RegOff0                                     0x2B
#define RegTOn1                                     0x2C
#define RegIOn1                                     0x2D
#define RegOff1                                     0x2E
#define RegTOn2                                     0x2F
#define RegIOn2                                     0x30
#define RegOff2                                     0x31
#define RegTOn3                                     0x32
#define RegIOn3                                     0x33
#define RegOff3                                     0x34
#define RegTOn4                                     0x35
#define RegIOn4                                     0x36
#define RegOff4                                     0x37
#define RegTRise4                                   0x38
#define RegTFall4                                   0x39
#define RegTOn5                                     0x3A
#define RegIOn5                                     0x3B
#define RegOff5                                     0x3C
#define RegTRise5                                   0x3D
#define RegTFall5                                   0x3E
#define RegTOn6                                     0x3F
#define RegIOn6                                     0x40
#define RegOff6                                     0x41
#define RegTRise6                                   0x42
#define RegTFall6                                   0x43
#define RegTOn7                                     0x44
#define RegIOn7                                     0x45
#define RegOff7                                     0x46
#define RegTRise7                                   0x47
#define RegTFall7                                   0x48
#define RegTOn8                                     0x49
#define RegIOn8                                     0x4A
#define RegOff8                                     0x4B
#define RegTOn9                                     0x4C
#define RegIOn9                                     0x4D
#define RegOff9                                     0x4E
#define RegTOn10                                    0x4F
#define RegIOn10                                    0x50
#define RegOff10                                    0x51
#define RegTOn11                                    0x52
#define RegIOn11                                    0x53
#define RegOff11                                    0x54
#define RegTOn12                                    0x55
#define RegIOn12                                    0x56
#define RegOff12                                    0x57
#define RegTRise12                                  0x58
#define RegTFall12                                  0x59
#define RegTOn13                                    0x5A
#define RegIOn13                                    0x5B
#define RegOff13                                    0x5C
#define RegTRise13                                  0x5D
#define RegTFall13                                  0x5E
#define RegTOn14                                    0x5F
#define RegIOn14                                    0x60
#define RegOff14                                    0x61
#define RegTRise14                                  0x62
#define RegTFall14                                  0x63
#define RegTOn15                                    0x64
#define RegIOn15                                    0x65
#define RegOff15                                    0x66
#define RegTRise15                                  0x67
#define RegTFall15                                  0x68
#define RegHighInputB                               0x69
#define RegHighInputA                               0x6A

/*!
 * \brief Mask to select correct data from the registers
 */
#define REG_SENSE_PIN_MASK_1                        0xFC
#define REG_SENSE_PIN_MASK_2                        0xF3
#define REG_SENSE_PIN_MASK_3                        0xCF
#define REG_SENSE_PIN_MASK_4                        0x3F

/*!
 * \brief Initializes the device
 */
void SX1509Init( void );

/*!
 * \brief Resets the device
 *
 * \retval status [SUCCESS, FAIL]
 */
uint8_t SX1509Reset( void );

/*!
 * \brief Writes a byte at specified address in the device
 *
 * \param [IN]: addr
 * \param [IN]: data
 * \retval status [SUCCESS, FAIL]
 */
uint8_t SX1509Write( uint8_t addr, uint8_t data );

/*!
 * \brief Writes a buffer at specified address in the device
 *
 * \param [IN]: addr
 * \param [IN]: data
 * \param [IN]: size
 * \retval status [SUCCESS, FAIL]
 */
uint8_t SX1509WriteBuffer( uint8_t addr, uint8_t *data, uint8_t size );

/*!
 * \brief Reads a byte at specified address in the device
 *
 * \param [IN]: addr
 * \param [OUT]: data
 * \retval status [SUCCESS, FAIL]
 */
uint8_t SX1509Read( uint8_t addr, uint8_t *data );

/*!
 * \brief Reads a buffer at specified address in the device
 *
 * \param [IN]: addr
 * \param [OUT]: data
 * \param [IN]: size
 * \retval status [SUCCESS, FAIL]
 */
uint8_t SX1509ReadBuffer( uint8_t addr, uint8_t *data, uint8_t size );

/*!
 * \brief Sets the I2C device slave address
 *
 * \param [IN]: addr
 */
void SX1509SetDeviceAddr( uint8_t addr );

/*!
 * \brief Gets the I2C device slave address
 *
 * \retval: addr Current device slave address
 */
uint8_t SX1509GetDeviceAddr( void );

#ifdef __cplusplus
}
#endif

#endif // __SX1509_H__
