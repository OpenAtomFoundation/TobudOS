/*
 * Copyright 2022 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_CRC_H__
#define __HAL_CRC_H__

#include "hal_common.h"

/*!
 * @addtogroup CRC
 * @{
 */

/*!
 * @brief CRC polynomail width type.
 */
typedef enum
{
    CRC_PolynomialWidth_32b = 0,  /*!< Polynomial Width 32 bits. */
    CRC_PolynomialWidth_16b = 1,  /*!< Polynomial Width 16 bits. */
    CRC_PolynomialWidth_8b = 2,   /*!< Polynomial Width 8 bits. */
} CRC_PolynomialWidth_Type;

/*!
 * @brief Whether the data in reverse order.
 */
typedef enum
{
    CRC_Rev_Normal = 0, /*!< Transfer data in normal order. */
    CRC_Rev_Rev = 1,    /*!< Transfer data in reverse order. */
} CRC_Rev_Type;

/*!
 * @brief The data endian is little-endian or big-endian.
 */
typedef enum
{
    CRC_DataEndian_LittleEndian = 0,  /*!< Specofy the data endian is little-endian. */
    CRC_DataEndian_BigEndian = 1,     /*!< Specofy the data endian is big-endian. */
} CRC_DataEndian_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref CRC_Init() to initialize the CRC module.
 */
typedef struct
{
    uint32_t                  Polynomial;       /*!< Configure arbitrary polynomial. */
    CRC_PolynomialWidth_Type  PolynomialWidth;  /*!< Specify the CRC polynomial width. */
    CRC_DataEndian_Type       InEndian;         /*!< Specify input endian is little-endian or big-endian. */
    CRC_DataEndian_Type       OutEndian;        /*!< Specify output endian is little-endian or big-ndian. */
    CRC_Rev_Type              InRev;            /*!< Specify whether the input data in reverse order. */
    CRC_Rev_Type              OutRev;           /*!< Specify whether the output data in reverse order. */
} CRC_Init_Type;

/*!
 * @brief Initialize the CRC module.
 *
 * Initialize polynomial width, polynomial and input/output configuration
 * The highest power of polynomial is determined by polynomial width.
 *
 * @param CRCx CRC instance.
 * @param init Pointer to the initialization structure. See to @ref CRC_Init_Type.
 * @return None.
 */
void CRC_Init(CRC_Type * CRCx, CRC_Init_Type * init);

/*!
 * @brief Set the first data for CRC calculation.
 *
 * @param CRCx CRC instance.
 * @param val The first data of CRC calculation.
 * @return None.
 */
void CRC_SetSeed(CRC_Type * CRCx, uint32_t val);

/*!
 * @brief Set data for CRC calculation.
 *
 * @param CRCx CRC instance.
 * @param val The value used for CRC calculation.
 * @return None.
 */
void CRC_SetData(CRC_Type * CRCx, uint32_t val);

/*!
 * @brief Get the most recent result of CRC calculation.
 *
 * @param CRCx CRC instance.
 * @return Return the most recent result of CRC calculation.
 */
uint32_t CRC_GetResult(CRC_Type * CRCx);

/*!
 *@}
 */
 
#endif /*__HAL_CRC_H__. */

