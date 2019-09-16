/*!
 * \file      delay-board.h
 *
 * \brief     Target board delay implementation
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
 *
 * \author    Johannes Bruder ( STACKFORCE )
 */
#ifndef __DELAY_BOARD_H__
#define __DELAY_BOARD_H__

#include <stdint.h>

/*!
 * \brief Blocking delay of "ms" milliseconds
 *
 * \param [IN] ms    delay in milliseconds
 */
void DelayMsMcu( uint32_t ms );

#endif // __DELAY_BOARD_H__
