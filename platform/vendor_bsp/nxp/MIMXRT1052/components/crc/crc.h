/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CRC_H_
#define _CRC_H_

#include "fsl_common.h"

/*!
 * @addtogroup CRC_Adapter
 * @{
 */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
***********************************************************************************/

/************************************************************************************
*************************************************************************************
* Public types
*************************************************************************************
************************************************************************************/
/*! @brief crcRefIn definitions. */
typedef enum _hal_crc_cfg_refin
{
    KHAL_CrcInputNoRef = 0U, /*!< Do not manipulate input data stream. */
    KHAL_CrcRefInput   = 1U  /*!< Reflect each byte in the input stream bitwise. */
} hal_crc_cfg_refin_t;

/*! @brief crcRefOut definitions. */
typedef enum _hal_crc_cfg_refout
{
    KHAL_CrcOutputNoRef = 0U, /*!< Do not manipulate CRC result. */
    KHAL_CrcRefOutput   = 1U  /*!< CRC result is to be reflected bitwise (operated on entire word). */
} hal_crc_cfg_refout_t;

/*! @brief crcByteOrder definitions. */
typedef enum _hal_crc_cfg_byteord
{
    KHAL_CrcLSByteFirst = 0U, /*!< Byte order of the CRC LS Byte first. */
    KHAL_CrcMSByteFirst = 1U  /*!< Bit order of the CRC  MS Byte first. */
} hal_crc_cfg_byteord_t;

/*! @brief CRC polynomials to use. */
typedef enum _hal_crc_polynomial
{
    KHAL_CrcPolynomial_CRC_8_CCITT = 0x103,      /*!< x^8+x^2+x^1+1 */
    KHAL_CrcPolynomial_CRC_16      = 0x1021,     /*!< x^16+x^12+x^5+1 */
    KHAL_CrcPolynomial_CRC_32      = 0x4C11DB7U, /*!< x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1 */
} hal_crc_polynomial_t;

/*! @brief CRC configuration structure. */
typedef struct _hal_crc_config
{
    hal_crc_cfg_refin_t crcRefIn;       /*!< CRC reflect input. See "hal_crc_cfg_refin_t". */
    hal_crc_cfg_refout_t crcRefOut;     /*!< CRC reflect output. See "hal_crc_cfg_refout_t". */
    hal_crc_cfg_byteord_t crcByteOrder; /*!< CRC byte order. See "hal_crc_cfg_byteord_t". */
    uint32_t crcSeed;                   /*!< CRC Seed value. Initial value for CRC LFSR. */
    uint32_t crcPoly;                   /*!< CRC Polynomial value. */
    uint32_t crcXorOut;                 /*!< XOR mask for CRC result (for no mask, should be 0). */
    uint8_t complementChecksum;         /*!< wether output the complement checksum. */
    uint8_t crcSize;                    /*!< Number of CRC octets, 2 mean use CRC16, 4 mean use CRC32. */
    uint8_t crcStartByte; /*!< Start CRC with this byte position. Byte #0 is the first byte of Sync Address. */
} hal_crc_config_t;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name CRC
 * @{
 */

/*!
 * @brief Compute CRC function.
 *
 * The function computes the CRC.
 *
 *  @code
 * config = (hal_crc_config_t) {
 *     .crcSize       = 4,
 *     .crcStartByte = 0,
 *     .crcRefIn       = KHAL_CrcInputNoRef,
 *     .crcRefOut    = KHAL_CrcOutputNoRef,
 *     .crcByteOrder = KHAL_CrcMSByteFirst,
 *     .complementChecksum = true,
 *     .crcSeed       = 0xFFFFFFFF,
 *     .crcPoly       = KHAL_CrcPolynomial_CRC_32,
 *     .crcXorOut    = 0xFFFFFFFF,
 * };
 *
 * res = HAL_CrcCompute(&config, (uint8_t *) pattern, strlen(pattern));
 *  @endcode
 *
 * @note The settings for compute CRC are taken from the passed CRC_config_t structure.
 *
 * @param crcConfig    configuration structure.
 * @param dataIn input data buffer.
 * @param length input data buffer size.
 *
 * @retval Computed CRC value.
 */
uint32_t HAL_CrcCompute(hal_crc_config_t *crcConfig, uint8_t *dataIn, uint32_t length);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _CRC_H_ */
