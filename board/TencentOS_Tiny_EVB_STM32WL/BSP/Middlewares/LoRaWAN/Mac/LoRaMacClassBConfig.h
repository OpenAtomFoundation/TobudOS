/*!
 * \file      LoRaMacClassBConfig.h
 *
 * \brief     LoRa MAC Class B configuration
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
 *              (C)2013 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \defgroup  LORAMACCLASSB LoRa MAC Class B configuration
 *            This header file contains parameters to configure the class b operation.
 *            By default, all parameters are set according to the specification.
 * \{
 */
#ifndef __LORAMACCLASSBCONFIG_H__
#define __LORAMACCLASSBCONFIG_H__

/*!
 * Defines the beacon interval in ms
 */
#define CLASSB_BEACON_INTERVAL                      128000

/*!
 * Beacon reserved time in ms
 */
#define CLASSB_BEACON_RESERVED                      2120

/*!
 * Beacon guard time in ms
 */
#define CLASSB_BEACON_GUARD                         3000

/*!
 * Beacon window time in ms
 */
#define CLASSB_BEACON_WINDOW                        122880

/*!
 * Beacon window time in numer of slots
 */
#define CLASSB_BEACON_WINDOW_SLOTS                  4096

/*!
 * Ping slot length time in ms
 */
#define CLASSB_PING_SLOT_WINDOW                     30

/*!
 * Maximum allowed beacon less time in ms
 */
#define CLASSB_MAX_BEACON_LESS_PERIOD               7200000

/*!
 * Delay time for the BeaconTimingAns in ms
 */
#define CLASSB_BEACON_DELAY_BEACON_TIMING_ANS       30

/*!
 * Default symbol timeout for beacons and ping slot windows
 */
#define CLASSB_BEACON_SYMBOL_TO_DEFAULT             8

/*!
 * Maximum symbol timeout for beacons
 */
#define CLASSB_BEACON_SYMBOL_TO_EXPANSION_MAX       255

/*!
 * Maximum symbol timeout for ping slots
 */
#define CLASSB_PING_SLOT_SYMBOL_TO_EXPANSION_MAX    30

/*!
 * Symbol expansion value for beacon windows in case of beacon
 * loss in symbols
 */
#define CLASSB_BEACON_SYMBOL_TO_EXPANSION_FACTOR    2

/*!
 * Defines the default window movement time
 */
#define CLASSB_WINDOW_MOVE_DEFAULT                  2

/*!
 * Defines the maximum time for the beacon movement
 */
#define CLASSB_WINDOW_MOVE_EXPANSION_MAX            256

/*!
 * Defines the expansion factor for the beacon movement
 */
#define CLASSB_WINDOW_MOVE_EXPANSION_FACTOR         2

#endif // __LORAMACCLASSBCONFIG_H__
