/*!
 * \file      gps-board.h
 *
 * \brief     Target board GPS driver implementation
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
#ifndef __GPS_BOARD_H__
#define __GPS_BOARD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "uart.h"

/*!
 * Select the edge of the PPS signal which is used to start the
 * reception of data on the UART. Depending of the GPS, the PPS
 * signal may go low or high to indicate the presence of data
 */
typedef enum PpsTrigger_s
{
    PpsTriggerIsRising = 0,
    PpsTriggerIsFalling,
}PpsTrigger_t;

/*!
 * \brief Low level handling of the PPS signal from the GPS receiver
 */
void GpsMcuOnPpsSignal( void* context );

/*!
 * \brief Invert the IRQ trigger edge on the PPS signal
 */
void GpsMcuInvertPpsTrigger( void );

/*!
 * \brief Low level Initialization of the UART and IRQ for the GPS
 */
void GpsMcuInit( void );

/*!
 * \brief Switch ON the GPS
 */
void GpsMcuStart( void );

/*!
 * \brief Switch OFF the GPS
 */
void GpsMcuStop( void );

/*!
 * Updates the GPS status
 */
void GpsMcuProcess( void );

/*!
 * \brief IRQ handler for the UART receiver
 */
void GpsMcuIrqNotify( UartNotifyId_t id );

#ifdef __cplusplus
}
#endif

#endif // __GPS_BOARD_H__
