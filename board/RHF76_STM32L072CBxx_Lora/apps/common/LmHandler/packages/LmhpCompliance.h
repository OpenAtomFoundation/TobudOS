/*!
 * \file      LmhpCompliance.h
 *
 * \brief     Implements the LoRa-Alliance certification protocol handling
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
#ifndef __LMHP_COMPLIANCE__
#define __LMHP_COMPLIANCE__

#include "LoRaMac.h"
#include "LmHandlerTypes.h"
#include "LmhPackage.h"

/*!
 * Compliance package identifier.
 * 
 * \remark This value must be unique amongst the packages
 */
#define PACKAGE_ID_COMPLIANCE                       0

/*!
 * Compliance test protocol handler parameters
 */
typedef struct LmhpComplianceParams_s
{
    /*!
     * Holds the ADR state
     */
    bool AdrEnabled;
    /*!
    * LoRaWAN ETSI duty cycle control enable/disable
    *
    * \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
    */
    bool DutyCycleEnabled;
    /*!
     * Stops unnecessary peripherals.
     *
     * \remark Use for the compliance tests protocol handling in order to
     *         reduce the power consumption.
     */
    void ( *StopPeripherals )( void );
    /*!
     * Starts previously stopped peripherals.
     *
     * \remark Use for the compliance tests protocol handling in order to
     *         reduce the power consumption.
     */
    void ( *StartPeripherals )( void );
}LmhpComplianceParams_t;

LmhPackage_t *LmphCompliancePackageFactory( void );

#endif // __LMHP_COMPLIANCE__
