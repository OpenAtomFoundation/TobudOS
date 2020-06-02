/*!
 * \file      LmhpClockSync.h
 *
 * \brief     Implements the LoRa-Alliance clock synchronization package
 *            Specification: https://lora-alliance.org/sites/default/files/2018-09/application_layer_clock_synchronization_v1.0.0.pdf
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
 *              (C)2013-2018 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 */
#ifndef __LMHP_CLOCK_SYNC_H__
#define __LMHP_CLOCK_SYNC_H__

#include "LoRaMac.h"
#include "LmHandlerTypes.h"
#include "LmhPackage.h"

/*!
 * Clock sync package identifier.
 *
 * \remark This value must be unique amongst the packages
 */
#define PACKAGE_ID_CLOCK_SYNC                       1

/*!
 * Clock sync package parameters
 *
 * This package doesn't require parameters
 */
//typedef struct LmphClockSyncParams_s
//{
//}LmphClockSyncParams_t;

LmhPackage_t *LmphClockSyncPackageFactory( void );

LmHandlerErrorStatus_t LmhpClockSyncAppTimeReq( void );

#endif // __LMHP_CLOCK_SYNC_H__
