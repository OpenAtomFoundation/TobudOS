/*!
 * \file      LoRaMacTest.h
 *
 * \brief     LoRa MAC layer test function implementation
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
 * \defgroup  LORAMACTEST LoRa MAC layer test function implementation
 *            This module specifies the API implementation of test function of the LoRaMAC layer.
 *            The functions in this file are only for testing purposes only.
 * \{
 */
#ifndef __LORAMACTEST_H__
#define __LORAMACTEST_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * \brief   Enabled or disables the duty cycle
 *
 * \details This is a test function. It shall be used for testing purposes only.
 *          Changing this attribute may lead to a non-conformance LoRaMac operation.
 *
 * \param   [IN] enable - Enabled or disables the duty cycle
 */
void LoRaMacTestSetDutyCycleOn( bool enable );

/*! \} defgroup LORAMACTEST */

#ifdef __cplusplus
}
#endif

#endif // __LORAMACTEST_H__
