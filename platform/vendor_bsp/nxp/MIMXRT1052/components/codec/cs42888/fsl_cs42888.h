/*
 * Copyright  2017-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CS42888_H_
#define _FSL_CS42888_H_

#include "fsl_codec_i2c.h"

/*!
 * @addtogroup cs42888
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @name Driver version */
/*@{*/
/*! @brief cs42888 driver version 2.1.1. */
#define FSL_CS42888_DRIVER_VERSION (MAKE_VERSION(2, 1, 1))
/*@}*/

/*! @brief CS42888 handle size */
#ifndef CS42888_I2C_HANDLER_SIZE
#define CS42888_I2C_HANDLER_SIZE CODEC_I2C_MASTER_HANDLER_SIZE
#endif

/*! @brief Define the register address of CS42888. */
#define CS42888_ID 0x01
#define CS42888_POWER_CONTROL 0x02
#define CS42888_FUNCTIONAL_MODE 0x03
#define CS42888_INTERFACE_FORMATS 0x04
#define CS42888_ADC_CONTROL 0x05
#define CS42888_TRANSITION_CONTROL 0x06
#define CS42888_CHANNEL_MUTE 0x07
#define CS42888_VOL_CONTROL_AOUT1 0x08
#define CS42888_VOL_CONTROL_AOUT2 0x09
#define CS42888_VOL_CONTROL_AOUT3 0x0A
#define CS42888_VOL_CONTROL_AOUT4 0x0B
#define CS42888_VOL_CONTROL_AOUT5 0x0C
#define CS42888_VOL_CONTROL_AOUT6 0x0D
#define CS42888_VOL_CONTROL_AOUT7 0x0E
#define CS42888_VOL_CONTROL_AOUT8 0x0F
#define CS42888_DAC_CHANNEL_INVERT 0x10
#define CS42888_VOL_CONTROL_AIN1 0x11
#define CS42888_VOL_CONTROL_AIN2 0x12
#define CS42888_VOL_CONTROL_AIN3 0x13
#define CS42888_VOL_CONTROL_AIN4 0x14
#define CS42888_ADC_CHANNEL_INVERT 0x17
#define CS42888_STATUS_CONTROL 0x18
#define CS42888_STATUS 0x19
#define CS42888_STATUS_MASK 0x1A
#define CS42888_MUTEC_PIN_CONTROL 0x1B

#define CS42888_POWER_CONTROL_PDN_MASK 0x1
#define CS42888_POWER_CONTROL_PDN_SHIFT 0
#define CS42888_POWER_CONTROL_PDN(x) \
    (((uint8_t)((uint8_t)(x) << CS42888_POWER_CONTROL_PDN_SHIFT)) & CS42888_POWER_CONTROL_PDN_MASK)
#define CS42888_POWER_CONTROL_PDN_DAC1_MASK 0x2
#define CS42888_POWER_CONTROL_PDN_DAC1_SHIFT 2
#define CS42888_POWER_CONTROL_PDN_DAC1(x) \
    (((uint8_t)((uint8_t)(x) << CS42888_POWER_CONTROL_PDN_DAC1_SHIFT)) & CS42888_POWER_CONTROL_PDN_DAC1_MASK)
#define CS42888_POWER_CONTROL_PDN_DAC2_MASK 0x4
#define CS42888_POWER_CONTROL_PDN_DAC2_SHIFT 3
#define CS42888_POWER_CONTROL_PDN_DAC2(x) \
    (((uint8_t)((uint8_t)(x) << CS42888_POWER_CONTROL_PDN_DAC2_SHIFT)) & CS42888_POWER_CONTROL_PDN_DAC2_MASK)
#define CS42888_POWER_CONTROL_PDN_DAC3_MASK 0x8
#define CS42888_POWER_CONTROL_PDN_DAC3_SHIFT 4
#define CS42888_POWER_CONTROL_PDN_DAC3(x) \
    (((uint8_t)((uint8_t)(x) << CS42888_POWER_CONTROL_PDN_DAC3_SHIFT)) & CS42888_POWER_CONTROL_PDN_DAC3_MASK)
#define CS42888_POWER_CONTROL_PDN_DAC4_MASK 0x10
#define CS42888_POWER_CONTROL_PDN_DAC4_SHIFT 5
#define CS42888_POWER_CONTROL_PDN_DAC4(x) \
    (((uint8_t)((uint8_t)(x) << CS42888_POWER_CONTROL_PDN_DAC4_SHIFT)) & CS42888_POWER_CONTROL_PDN_DAC4_MASK)
#define CS42888_POWER_CONTROL_PDN_ADC1_MASK 0x20
#define CS42888_POWER_CONTROL_PDN_ADC1_SHIFT 5
#define CS42888_POWER_CONTROL_PDN_ADC1(x) \
    (((uint8_t)((uint8_t)(x) << CS42888_POWER_CONTROL_PDN_ADC1_SHIFT)) & CS42888_POWER_CONTROL_PDN_ADC1_MASK)
#define CS42888_POWER_CONTROL_PDN_ADC2_MASK 0x40
#define CS42888_POWER_CONTROL_PDN_ADC2_SHIFT 6
#define CS42888_POWER_CONTROL_PDN_ADC2(x) \
    (((uint8_t)((uint8_t)(x) << CS42888_POWER_CONTROL_PDN_ADC2_SHIFT)) & CS42888_POWER_CONTROL_PDN_ADC2_MASK)

#define CS42888_FUNCTIONAL_MODE_ADC_FM_MASK 0x30
#define CS42888_FUNCTIONAL_MODE_ADC_FM_SHIFT 4
#define CS42888_FUNCTIONAL_MODE_ADC_FM(x) \
    (((uint8_t)((uint8_t)(x) << CS42888_FUNCTIONAL_MODE_ADC_FM_SHIFT)) & CS42888_FUNCTIONAL_MODE_ADC_FM_MASK)
#define CS42888_FUNCTIONAL_MODE_DAC_FM_MASK 0xC0
#define CS42888_FUNCTIONAL_MODE_DAC_FM_SHIFT 6
#define CS42888_FUNCTIONAL_MODE_DAC_FM(x) \
    (((uint8_t)((uint8_t)(x) << CS42888_FUNCTIONAL_MODE_DAC_FM_SHIFT)) & CS42888_FUNCTIONAL_MODE_DAC_FM_MASK)

#define CS42888_INTERFACE_FORMATS_DAC_DIF_MASK 0x38
#define CS42888_INTERFACE_FORMATS_DAC_DIF_SHIFT 3
#define CS42888_INTERFACE_FORMATS_DAC_DIF(x) \
    (((uint8_t)((uint8_t)(x) << CS42888_INTERFACE_FORMATS_DAC_DIF_SHIFT)) & CS42888_INTERFACE_FORMATS_DAC_DIF_MASK)

/*! @brief Cache register number */
#define CS42888_CACHEREGNUM 28

/*! @brief CS42888 I2C address. */
#define CS42888_I2C_ADDR 0x48

/*! @brief CS42888 I2C baudrate */
#define CS42888_I2C_BITRATE (100000U)

/*! @brief cs42888 reset function pointer */
typedef void (*cs42888_reset)(bool state);

/*! @brief CS42888 support modes. */
typedef enum _CS42888_func_mode
{
    kCS42888_ModeMasterSSM = 0x0, /*!< master single speed mode */
    kCS42888_ModeMasterDSM = 0x1, /*!< master dual speed mode */
    kCS42888_ModeMasterQSM = 0x2, /*!< master quad speed mode */
    kCS42888_ModeSlave     = 0x3, /*!< master single speed mode */
} cs42888_func_mode;

/*! @brief Modules in CS42888 board. */
typedef enum _CS42888_module
{
    kCS42888_ModuleDACPair1 = 0x2,  /*!< DAC pair1 (AOUT1 and AOUT2) module in CS42888 */
    kCS42888_ModuleDACPair2 = 0x4,  /*!< DAC pair2 (AOUT3 and AOUT4) module in CS42888 */
    kCS42888_ModuleDACPair3 = 0x8,  /*!< DAC pair3 (AOUT5 and AOUT6) module in CS42888 */
    kCS42888_ModuleDACPair4 = 0x10, /*!< DAC pair4 (AOUT7 and AOUT8) module in CS42888 */
    kCS42888_ModuleADCPair1 = 0x20, /*!< ADC pair1 (AIN1 and AIN2) module in CS42888 */
    kCS42888_ModuleADCPair2 = 0x40, /*!< ADC pair2 (AIN3 and AIN4) module in CS42888 */
} cs42888_module_t;

/*! @brief CS42888 supported audio bus type. */
typedef enum _CS42888_bus
{
    kCS42888_BusLeftJustified  = 0x0, /*!< Left justified format, up to 24 bits.*/
    kCS42888_BusI2S            = 0x1, /*!< I2S format, up to 24 bits */
    kCS42888_BusRightJustified = 0x2, /*!< Right justified, can support 16bits and 24 bits */
    kCS42888_BusOL1            = 0x4, /*!< One-Line #1 mode */
    kCS42888_BusOL2            = 0x5, /*!< One-Line #2 mode */
    kCS42888_BusTDM            = 0x6  /*!< TDM mode */
} cs42888_bus_t;

/*! @brief CS428888 play channel */
enum _cs42888_play_channel
{
    kCS42888_AOUT1 = 1U, /*!< aout1 */
    kCS42888_AOUT2 = 2U, /*!< aout2 */
    kCS42888_AOUT3 = 3U, /*!< aout3 */
    kCS42888_AOUT4 = 4U, /*!< aout4 */
    kCS42888_AOUT5 = 5U, /*!< aout5 */
    kCS42888_AOUT6 = 6U, /*!< aout6 */
    kCS42888_AOUT7 = 7U, /*!< aout7 */
    kCS42888_AOUT8 = 8U, /*!< aout8 */
};

/*! @brief cs42888 audio format */
typedef struct _cs42888_audio_format
{
    uint32_t mclk_HZ;    /*!< master clock frequency */
    uint32_t sampleRate; /*!< sample rate */
    uint32_t bitWidth;   /*!< bit width */
} cs42888_audio_format_t;

/*! @brief Initialize structure of CS42888 */
typedef struct cs42888_config
{
    cs42888_bus_t bus;             /*!< Audio transfer protocol */
    cs42888_audio_format_t format; /*!< cs42888 audio format */
    cs42888_func_mode ADCMode;     /*!< CS42888 ADC function mode. */
    cs42888_func_mode DACMode;     /*!< CS42888 DAC function mode. */
    bool master;                   /*!< true is master, false is slave */
    codec_i2c_config_t i2cConfig;  /*!< i2c bus configuration */
    uint8_t slaveAddress;          /*!< slave address */
    cs42888_reset reset;           /*!< reset function pointer */
} cs42888_config_t;

/*! @brief cs42888 handler */
typedef struct _cs42888_handle
{
    cs42888_config_t *config;                    /*!< cs42888 config pointer */
    uint8_t i2cHandle[CS42888_I2C_HANDLER_SIZE]; /*!< i2c handle pointer */
} cs42888_handle_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief CS42888 initialize function.
 *
 * The second parameter is NULL to CS42888 in this version. If users want
 * to change the settings, they have to use cs42888_write_reg() or cs42888_modify_reg()
 * to set the register value of CS42888.
 * Note: If the codec_config is NULL, it would initialize CS42888 using default settings.
 * The default setting:
 * codec_config->bus = kCS42888_BusI2S
 * codec_config->ADCmode = kCS42888_ModeSlave
 * codec_config->DACmode = kCS42888_ModeSlave
 *
 * @param handle CS42888 handle structure.
 * @param codec_config CS42888 configuration structure.
 */
status_t CS42888_Init(cs42888_handle_t *handle, cs42888_config_t *config);

/*!
 * @brief Deinit the CS42888 codec.
 *
 * This function close all modules in CS42888 to save power.
 *
 * @param handle CS42888 handle structure pointer.
 */
status_t CS42888_Deinit(cs42888_handle_t *handle);

/*!
 * @brief Set the audio transfer protocol.
 *
 * CS42888 only supports I2S, left justified, right justified, PCM A, PCM B format.
 *
 * @param handle CS42888 handle structure.
 * @param bus Audio data transfer protocol.
 */
status_t CS42888_SetProtocol(cs42888_handle_t *handle, cs42888_bus_t protocol, uint32_t bitWidth);

/*!
 * @brief Set CS42888 to differernt working mode.
 *
 * @Deprecated api, Do not use it anymore. It has been superceded by @ref CS42888_SelectFunctionalMode.
 *
 * @param handle CS42888 handle structure.
 * @param mode differenht working mode of CS42888.
 */
void CS42888_SetFuncMode(cs42888_handle_t *handle, cs42888_func_mode mode);

/*!
 * @brief Set CS42888 to differernt functional mode.
 *
 * @param handle CS42888 handle structure.
 * @param adcMode differenht working mode of CS42888.
 * @param dacMode differenht working mode of CS42888.
 */
status_t CS42888_SelectFunctionalMode(cs42888_handle_t *handle, cs42888_func_mode adcMode, cs42888_func_mode dacMode);

/*!
 * @brief Set the volume of different modules in CS42888.
 *
 * This function would set the volume of CS42888 modules. Uses need to appoint the module.
 * The function assume that left channel and right channel has the same volume.
 *
 * @param handle CS42888 handle structure.
 * @param channel AOUT channel, it shall be 1~8.
 * @param volume Volume value need to be set.
 */
status_t CS42888_SetAOUTVolume(cs42888_handle_t *handle, uint8_t channel, uint8_t volume);

/*!
 * @brief Set the volume of different modules in CS42888.
 *
 * This function would set the volume of CS42888 modules. Uses need to appoint the module.
 * The function assume that left channel and right channel has the same volume.
 *
 * @param handle CS42888 handle structure.
 * @param channel AIN channel, it shall be 1~4.
 * @param volume Volume value need to be set.
 */
status_t CS42888_SetAINVolume(cs42888_handle_t *handle, uint8_t channel, uint8_t volume);

/*!
 * @brief Get the volume of different AOUT channel in CS42888.
 *
 * This function gets the volume of CS42888 modules. Uses need to appoint the module.
 * The function assume that left channel and right channel has the same volume.
 *
 * @param handle CS42888 handle structure.
 * @param channel AOUT channel, it shall be 1~8.
 * @return Volume value of the module.
 */
uint8_t CS42888_GetAOUTVolume(cs42888_handle_t *handle, uint8_t channel);

/*!
 * @brief Get the volume of different AIN channel in CS42888.
 *
 * This function gets the volume of CS42888 modules. Uses need to appoint the module.
 * The function assume that left channel and right channel has the same volume.
 *
 * @param handle CS42888 handle structure.
 * @param channel AIN channel, it shall be 1~4.
 * @return Volume value of the module.
 */
uint8_t CS42888_GetAINVolume(cs42888_handle_t *handle, uint8_t channel);

/*!
 * @brief Mute modules in CS42888.
 *
 * @param handle CS42888 handle structure.
 * @param channelMask Channel mask for mute. Mute channel 0, it shall be 0x1, while mute channel 0 and 1, it shall
 * be 0x3. Mute all channel, it shall be 0xFF. Each bit represent one channel, 1 means mute, 0 means unmute.
 */
status_t CS42888_SetMute(cs42888_handle_t *handle, uint8_t channelMask);

/*!
 * @brief Mute channel modules in CS42888.
 *
 * @param handle CS42888 handle structure.
 * @param channel reference _cs42888_play_channel.
 * @param isMute true is mute, falase is unmute.
 */
status_t CS42888_SetChannelMute(cs42888_handle_t *handle, uint8_t channel, bool isMute);

/*!
 * @brief Enable/disable expected devices.
 *
 * @param handle CS42888 handle structure.
 * @param module Module expected to enable.
 * @param isEnabled Enable or disable moudles.
 */
status_t CS42888_SetModule(cs42888_handle_t *handle, cs42888_module_t module, bool isEnabled);

/*!
 * @brief Configure the data format of audio data.
 *
 * This function would configure the registers about the sample rate, bit depths.
 *
 * @param handle CS42888 handle structure pointer.
 * @param mclk Master clock frequency of I2S.
 * @param sample_rate Sample rate of audio file running in CS42888. CS42888 now
 * supports 8k, 11.025k, 12k, 16k, 22.05k, 24k, 32k, 44.1k, 48k and 96k sample rate.
 * @param bits Bit depth of audio file (CS42888 only supports 16bit, 20bit, 24bit
 * and 32 bit in HW).
 */
status_t CS42888_ConfigDataFormat(cs42888_handle_t *handle, uint32_t mclk, uint32_t sample_rate, uint32_t bits);

/*!
 * @brief Write register to CS42888 using I2C.
 *
 * @param handle CS42888 handle structure.
 * @param reg The register address in CS42888.
 * @param val Value needs to write into the register.
 */
status_t CS42888_WriteReg(cs42888_handle_t *handle, uint8_t reg, uint8_t val);

/*!
 * @brief Read register from CS42888 using I2C.
 * @param handle CS42888 handle structure.
 * @param reg The register address in CS42888.
 * @param val Value written to.
 */
status_t CS42888_ReadReg(cs42888_handle_t *handle, uint8_t reg, uint8_t *val);

/*!
 * @brief Modify some bits in the register using I2C.
 * @param handle CS42888 handle structure.
 * @param reg The register address in CS42888.
 * @param mask The mask code for the bits want to write. The bit you want to write should be 0.
 * @param val Value needs to write into the register.
 */
status_t CS42888_ModifyReg(cs42888_handle_t *handle, uint8_t reg, uint8_t mask, uint8_t val);

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_CS42888_H_ */
