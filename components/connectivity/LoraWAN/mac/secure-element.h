/*!
 * \file      secure-element.h
 *
 * \brief     Secure Element driver API
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
 * \defgroup  SECUREELEMENT Secure Element API Definition
 *
 * \{
 *
 */
#ifndef __SECURE_ELEMENT_H__
#define __SECURE_ELEMENT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "LoRaMacCrypto.h"

/*!
 * Secure-element keys size in bytes
 */
#define SE_KEY_SIZE             16

/*!
 * Secure-element EUI size in bytes
 */
#define SE_EUI_SIZE             8

/*!
 * Secure-element pin size in bytes
 */
#define SE_PIN_SIZE             4

/*!
 * Return values.
 */
typedef enum eSecureElementStatus
{
    /*!
     * No error occurred
     */
    SECURE_ELEMENT_SUCCESS = 0,
    /*!
     * CMAC does not match
     */
    SECURE_ELEMENT_FAIL_CMAC,
    /*!
     * Null pointer exception
     */
    SECURE_ELEMENT_ERROR_NPE,
    /*!
     * Invalid key identifier exception
     */
    SECURE_ELEMENT_ERROR_INVALID_KEY_ID,
    /*!
     * Invalid LoRaWAN specification version
     */
    SECURE_ELEMENT_ERROR_INVALID_LORAWAM_SPEC_VERSION,
    /*!
     * Incompatible buffer size
     */
    SECURE_ELEMENT_ERROR_BUF_SIZE,
    /*!
     * Undefined Error occurred
     */
    SECURE_ELEMENT_ERROR,
    /*!
     * Failed to encrypt
     */
    SECURE_ELEMENT_FAIL_ENCRYPT,
}SecureElementStatus_t;

/*!
 * Signature of callback function to be called by the Secure Element driver when the
 * non volatile context have to be stored.
 *
 */
typedef void ( *SecureElementNvmEvent )( void );

/*!
 * Initialization of Secure Element driver
 *
 * \param[IN]     seNvmCtxChanged           - Callback function which will be called  when the
 *                                            non-volatile context have to be stored.
 * \retval                                  - Status of the operation
 */
SecureElementStatus_t SecureElementInit( SecureElementNvmEvent seNvmCtxChanged );

/*!
 * Restores the internal nvm context from passed pointer.
 *
 * \param[IN]     seNvmCtx         - Pointer to non-volatile module context to be restored.
 * \retval                         - Status of the operation
 */
SecureElementStatus_t SecureElementRestoreNvmCtx( void* seNvmCtx );

/*!
 * Returns a pointer to the internal non-volatile context.
 *
 * \param[IN]     seNvmCtxSize    - Size of the module non volatile context
 * \retval                        - Points to a structure where the module store its non volatile context
 */
void* SecureElementGetNvmCtx( size_t* seNvmCtxSize );

/*!
 * Sets a key
 *
 * \param[IN]  keyID          - Key identifier
 * \param[IN]  key            - Key value
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementSetKey( KeyIdentifier_t keyID, uint8_t* key );

/*!
 * Computes a CMAC of a message using provided initial Bx block
 *
 * \param[IN]  micBxBuffer    - Buffer containing the initial Bx block
 * \param[IN]  buffer         - Data buffer
 * \param[IN]  size           - Data buffer size
 * \param[IN]  keyID          - Key identifier to determine the AES key to be used
 * \param[OUT] cmac           - Computed cmac
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementComputeAesCmac( uint8_t* micBxBuffer, uint8_t* buffer, uint16_t size, KeyIdentifier_t keyID, uint32_t* cmac );

/*!
 * Verifies a CMAC (computes and compare with expected cmac)
 *
 * \param[IN]  buffer         - Data buffer
 * \param[IN]  size           - Data buffer size
 * \param[in]  expectedCmac   - Expected cmac
 * \param[IN]  keyID          - Key identifier to determine the AES key to be used
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementVerifyAesCmac( uint8_t* buffer, uint16_t size, uint32_t expectedCmac, KeyIdentifier_t keyID );

/*!
 * Encrypt a buffer
 *
 * \param[IN]  buffer         - Data buffer
 * \param[IN]  size           - Data buffer size
 * \param[IN]  keyID          - Key identifier to determine the AES key to be used
 * \param[OUT] encBuffer      - Encrypted buffer
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementAesEncrypt( uint8_t* buffer, uint16_t size, KeyIdentifier_t keyID, uint8_t* encBuffer );

/*!
 * Derives and store a key
 *
 * \param[IN]  version        - LoRaWAN specification version currently in use.
 * \param[IN]  input          - Input data from which the key is derived ( 16 byte )
 * \param[IN]  rootKeyID      - Key identifier of the root key to use to perform the derivation
 * \param[IN]  targetKeyID    - Key identifier of the key which will be derived
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementDeriveAndStoreKey( Version_t version, uint8_t* input, KeyIdentifier_t rootKeyID, KeyIdentifier_t targetKeyID );

/*!
 * Process JoinAccept message.
 *
 * \param[IN]  encJoinAccept     - Received encrypted JoinAccept message
 * \param[IN]  encJoinAcceptSize - Received encrypted JoinAccept message Size
 * \param[OUT] decJoinAccept     - Decrypted and validated JoinAccept message
 * \param[OUT] versionMinor      - Detected LoRaWAN specification version minor field.
 *                                     - 0 -> LoRaWAN 1.0.x
 *                                     - 1 -> LoRaWAN 1.1.x
 * \retval                       - Status of the operation
 */
SecureElementStatus_t SecureElementProcessJoinAccept( JoinReqIdentifier_t joinReqType, uint8_t* joinEui,
                                                      uint16_t devNonce, uint8_t* encJoinAccept,
                                                      uint8_t encJoinAcceptSize, uint8_t* decJoinAccept,
                                                      uint8_t* versionMinor );

/*!
 * Generates a random number
 *
 * \param[OUT] randomNum      - 32 bit random number
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementRandomNumber( uint32_t* randomNum );

/*!
 * Sets the DevEUI
 *
 * \param[IN] devEui          - Pointer to the 8-byte devEUI
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementSetDevEui( uint8_t* devEui );

/*!
 * Gets the DevEUI
 *
 * \retval                    - Pointer to the 8-byte devEUI
 */
uint8_t* SecureElementGetDevEui( void );

/*!
 * Sets the JoinEUI
 *
 * \param[IN] joinEui         - Pointer to the 8-byte joinEui
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementSetJoinEui( uint8_t* joinEui );

/*!
 * Gets the DevEUI
 *
 * \retval                    - Pointer to the 8-byte joinEui
 */
uint8_t* SecureElementGetJoinEui( void );

/*!
 * Sets the pin
 *
 * \param[IN] pin             - Pointer to the 4-byte pin
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementSetPin( uint8_t* pin );

/*!
 * Gets the Pin
 *
 * \retval                    - Pointer to the 4-byte pin
 */
uint8_t* SecureElementGetPin( void );

/*! \} defgroup SECUREELEMENT */

#ifdef __cplusplus
}
#endif

#endif //  __SECURE_ELEMENT_H__
