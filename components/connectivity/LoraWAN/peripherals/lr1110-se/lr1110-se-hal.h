/*!
 * \file      lr1110-se-hal.h
 *
 * \brief     Secure Element hardware abstraction layer
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
 *              (C)2020 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 */
#ifndef __LR1110_SE_HAL_H__
#define __LR1110_SE_HAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*!
 * \brief Get a 64 bits unique ID
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void LR1110SeHalGetUniqueId( uint8_t *id );

/*!
 * \brief Get a random number
 *
 * \remark The number SHALL NOT be generated using a pseudo random number
 *         generator
 * \retval number 32 bit random value
 */
uint32_t LR1110SeHalGetRandomNumber( void );

#ifdef __cplusplus
}
#endif

#endif  //  __LR1110_SE_HAL_H__
