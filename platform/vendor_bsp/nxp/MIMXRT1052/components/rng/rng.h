/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _HAL_RNG_H_
#define _HAL_RNG_H_

/*!
 * @addtogroup RNG_Adapter
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

/*! @brief Hal rand status. */
typedef enum _hal_rng_status
{
    kStatus_HAL_RngSuccess        = kStatus_Success,                      /*!< Success */
    KStatus_HAL_RngNotSupport     = MAKE_STATUS(kStatusGroup_HAL_RNG, 1), /*!<Not support*/
    kStatus_HAL_RngInternalError  = MAKE_STATUS(kStatusGroup_HAL_RNG, 2), /*!<Internal Error*/
    kStatus_HAL_RngNullPointer    = MAKE_STATUS(kStatusGroup_HAL_RNG, 3), /*!<Null pointer*/
    kStatus_HAL_RngMaxRequests    = MAKE_STATUS(kStatusGroup_HAL_RNG, 4), /*!<Max request*/
    kStatus_HAL_RngInvalidArgumen = MAKE_STATUS(kStatusGroup_HAL_RNG, 5), /*!<Invalid Argumen*/
} hal_rng_status_t;
/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
/*!
 * @brief Initializes the random adapter module for a random data generator basic operation.
 *
 * @note This API should be called at the beginning of the application using the random adapter driver.
 *
 * @retval kStatus_HAL_RngSuccess Rand module initialize succeed
 */
hal_rng_status_t HAL_RngInit(void);

/*!
 * @brief DeInitilizate the random adapter module.
 *
 * @note This API should be called when not using the rand adapter driver anymore.
 *
 */
void HAL_RngDeinit(void);

/*!
 * @brief Get random value from random hardware
 *
 * @note This API should be called to get random data.
 *
 * @param pRandomNo             Pointer to random data
 * @param dataSize            The random data size
 * @retval kStatus_HAL_RngSuccess Rand get data succeed
 */
hal_rng_status_t HAL_RngHwGetData(void *pRandomNo, uint32_t dataSize);

/*!
 * @brief Get a pseudo random number
 *
 * @note This API should be called to get random data.
 *
 * @param pRandomNo             Pointer to random data
 * @param dataSize            The random data size
 * @retval kStatus_HAL_RngSuccess Rand get data succeed
 */
hal_rng_status_t HAL_RngGetData(void *pRandomNo, uint32_t dataSize);

/*!
 * @brief Set random seed of random generator hardware
 *
 * @note This API should be called to set seed before get random data.
 *
 * @param seed             Seed of the random hardware generator
 * @retval kStatus_HAL_RngSuccess Rand set rand seed succeed
 * @retval KStatus_HAL_RngNotSupport Rand set rand seed not support
 */
hal_rng_status_t HAL_RngSetSeed(uint32_t seed);
/*! @}*/
#endif /* _HAL_RNG_H_ */
