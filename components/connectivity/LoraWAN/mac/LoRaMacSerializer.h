/*!
 * \file      LoRaMacSerializer.h
 *
 * \brief     LoRa MAC layer message serializer functionality implementation
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
 * \author    Johannes Bruder ( STACKFORCE )
 *
 * addtogroup LORAMAC
 * \{
 *
 */
#ifndef __LORAMAC_SERIALIZER_H__
#define __LORAMAC_SERIALIZER_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "LoRaMacMessageTypes.h"


/*!
 * LoRaMac Serializer Status
 */
typedef enum eLoRaMacSerializerStatus
{
    /*!
     * No error occurred
     */
    LORAMAC_SERIALIZER_SUCCESS = 0,
    /*!
     * Null pointer exception
     */
    LORAMAC_SERIALIZER_ERROR_NPE,
    /*!
     * Incompatible buffer size
     */
    LORAMAC_SERIALIZER_ERROR_BUF_SIZE,
    /*!
     * Undefined Error occurred
     */
    LORAMAC_SERIALIZER_ERROR,
}LoRaMacSerializerStatus_t;

/*!
 * Creates serialized MAC message of structured object.
 *
 * \param[IN/OUT] macMsg        - Join-request message object
 * \retval                      - Status of the operation
 */
LoRaMacSerializerStatus_t LoRaMacSerializerJoinRequest( LoRaMacMessageJoinRequest_t* macMsg );

/*!
 * Creates serialized MAC message of structured object.
 *
 * \param[IN/OUT] macMsg        - Join-request message object
 * \retval                      - Status of the operation
 */
LoRaMacSerializerStatus_t LoRaMacSerializerReJoinType1( LoRaMacMessageReJoinType1_t* macMsg );

/*!
 * Creates serialized MAC message of structured object.
 *
 * \param[IN/OUT] macMsg        - Join-request message object
 * \retval                      - Status of the operation
 */
LoRaMacSerializerStatus_t LoRaMacSerializerReJoinType0or2( LoRaMacMessageReJoinType0or2_t* macMsg );

/*!
 * Creates serialized MAC message of structured object.
 *
 * \param[IN/OUT] macMsg        - Data message object
 * \retval                      - Status of the operation
 */
LoRaMacSerializerStatus_t LoRaMacSerializerData( LoRaMacMessageData_t* macMsg );

/*! \} addtogroup LORAMAC */

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_SERIALIZER_H__

