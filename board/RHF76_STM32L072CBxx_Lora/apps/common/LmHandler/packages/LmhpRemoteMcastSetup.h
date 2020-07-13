/*!
 * \file      LmhpRemoteMcastSetup.h
 *
 * \brief     Implements the LoRa-Alliance remote multicast setup package
 *            Specification: https://lora-alliance.org/sites/default/files/2018-09/remote_multicast_setup_v1.0.0.pdf
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
#ifndef __LMHP_REMOTE_MCAST_SETUP_H__
#define __LMHP_REMOTE_MCAST_SETUP_H__

#include "LoRaMac.h"
#include "LmHandlerTypes.h"
#include "LmhPackage.h"

/*!
 * Remote multicast setup package identifier.
 *
 * \remark This value must be unique amongst the packages
 */
#define PACKAGE_ID_REMOTE_MCAST_SETUP               2

/*!
 * Remote multicast setup package parameters
 *
 * This package doesn't require parameters
 */
//typedef struct LmhpRemoteMcastSetupParams_s
//{
//}LmhpRemoteMcastSetupParams_t;

LmhPackage_t *LmhpRemoteMcastSetupPackageFactory( void );

#endif // __LMHP_REMOTE_MCAST_SETUP_H__
