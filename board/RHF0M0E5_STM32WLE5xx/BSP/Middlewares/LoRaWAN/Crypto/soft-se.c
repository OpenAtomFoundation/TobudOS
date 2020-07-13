/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech
 ___ _____ _   ___ _  _____ ___  ___  ___ ___
/ __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
\__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
|___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
embedded.connectivity.solutions===============

Description: Secure Element software implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis ( Semtech ), Gregory Cristian ( Semtech ),
            Daniel Jaeckle ( STACKFORCE ),  Johannes Bruder ( STACKFORCE )
*/
#include "secure-element.h"

#include <stdlib.h>
#include <stdint.h>

#include "LoRaMacCrypto.h"
#include "utilities.h"
#include "aes.h"
#include "cmac.h"
#include "radio.h"

#define NUM_OF_KEYS      24
#define KEY_SIZE         16

/*!
 * Identifier value pair type for Keys
 */
typedef struct sKey
{
    /*
     * Key identifier
     */
    KeyIdentifier_t KeyID;
    /*
     * Key value
     */
    uint8_t KeyValue[KEY_SIZE];
} Key_t;

/*
 * Secure Element Non Volatile Context structure
 */
typedef struct sSecureElementNvCtx
{
    /*
     * DevEUI storage
     */
    uint8_t DevEui[SE_EUI_SIZE];
    /*
     * Join EUI storage
     */
    uint8_t JoinEui[SE_EUI_SIZE];
    /*
     * AES computation context variable
     */
    aes_context AesContext;
    /*
     * CMAC computation context variable
     */
    AES_CMAC_CTX AesCmacCtx[1];
    /*
     * Key List
     */
    Key_t KeyList[NUM_OF_KEYS];
}SecureElementNvCtx_t;

/*
 * Module context
 */
static SecureElementNvCtx_t SeNvmCtx;

static SecureElementNvmEvent SeNvmCtxChanged;

/*
 * Local functions
 */

/*
 * Gets key item from key list.
 *
 *  cmac = aes128_cmac(keyID, B0 | msg)
 *
 * \param[IN]  keyID          - Key identifier
 * \param[OUT] keyItem        - Key item reference
 * \retval                    - Status of the operation
 */
static SecureElementStatus_t GetKeyByID( KeyIdentifier_t keyID, Key_t** keyItem )
{
    for( uint8_t i = 0; i < NUM_OF_KEYS; i++ )
    {
        if( SeNvmCtx.KeyList[i].KeyID == keyID )
        {
            *keyItem = &( SeNvmCtx.KeyList[i] );
            return SECURE_ELEMENT_SUCCESS;
        }
    }
    return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
}

/*
 * Dummy callback in case if the user provides NULL function pointer
 */
static void DummyCB( void )
{
    return;
}

/*
 * Computes a CMAC of a message using provided initial Bx block
 *
 *  cmac = aes128_cmac(keyID, blocks[i].Buffer)
 *
 * \param[IN]  micBxBuffer    - Buffer containing the initial Bx block
 * \param[IN]  buffer         - Data buffer
 * \param[IN]  size           - Data buffer size
 * \param[IN]  keyID          - Key identifier to determine the AES key to be used
 * \param[OUT] cmac           - Computed cmac
 * \retval                    - Status of the operation
 */
static SecureElementStatus_t ComputeCmac( uint8_t *micBxBuffer, uint8_t *buffer, uint16_t size, KeyIdentifier_t keyID, uint32_t* cmac )
{
    if( ( buffer == NULL ) || ( cmac == NULL ) )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    uint8_t Cmac[16];

    AES_CMAC_Init( SeNvmCtx.AesCmacCtx );

    Key_t* keyItem;
    SecureElementStatus_t retval = GetKeyByID( keyID, &keyItem );

    if( retval == SECURE_ELEMENT_SUCCESS )
    {
        AES_CMAC_SetKey( SeNvmCtx.AesCmacCtx, keyItem->KeyValue );

        if( micBxBuffer != NULL )
        {
            AES_CMAC_Update( SeNvmCtx.AesCmacCtx, micBxBuffer, 16 );
        }

        AES_CMAC_Update( SeNvmCtx.AesCmacCtx, buffer, size );

        AES_CMAC_Final( Cmac, SeNvmCtx.AesCmacCtx );

        // Bring into the required format
        *cmac = ( uint32_t )( ( uint32_t ) Cmac[3] << 24 | ( uint32_t ) Cmac[2] << 16 | ( uint32_t ) Cmac[1] << 8 | ( uint32_t ) Cmac[0] );
    }

    return retval;
}

/*
 * API functions
 */

SecureElementStatus_t SecureElementInit( SecureElementNvmEvent seNvmCtxChanged )
{
    uint8_t itr = 0;
    uint8_t zeroKey[16] = { 0 };

    // Initialize with defaults
    SeNvmCtx.KeyList[itr++].KeyID = APP_KEY;
    SeNvmCtx.KeyList[itr++].KeyID = GEN_APP_KEY;
    SeNvmCtx.KeyList[itr++].KeyID = NWK_KEY;
    SeNvmCtx.KeyList[itr++].KeyID = J_S_INT_KEY;
    SeNvmCtx.KeyList[itr++].KeyID = J_S_ENC_KEY;
    SeNvmCtx.KeyList[itr++].KeyID = F_NWK_S_INT_KEY;
    SeNvmCtx.KeyList[itr++].KeyID = S_NWK_S_INT_KEY;
    SeNvmCtx.KeyList[itr++].KeyID = NWK_S_ENC_KEY;
    SeNvmCtx.KeyList[itr++].KeyID = APP_S_KEY;
    SeNvmCtx.KeyList[itr++].KeyID = MC_ROOT_KEY;
    SeNvmCtx.KeyList[itr++].KeyID = MC_KE_KEY;
    SeNvmCtx.KeyList[itr++].KeyID = MC_KEY_0;
    SeNvmCtx.KeyList[itr++].KeyID = MC_APP_S_KEY_0;
    SeNvmCtx.KeyList[itr++].KeyID = MC_NWK_S_KEY_0;
    SeNvmCtx.KeyList[itr++].KeyID = MC_KEY_1;
    SeNvmCtx.KeyList[itr++].KeyID = MC_APP_S_KEY_1;
    SeNvmCtx.KeyList[itr++].KeyID = MC_NWK_S_KEY_1;
    SeNvmCtx.KeyList[itr++].KeyID = MC_KEY_2;
    SeNvmCtx.KeyList[itr++].KeyID = MC_APP_S_KEY_2;
    SeNvmCtx.KeyList[itr++].KeyID = MC_NWK_S_KEY_2;
    SeNvmCtx.KeyList[itr++].KeyID = MC_KEY_3;
    SeNvmCtx.KeyList[itr++].KeyID = MC_APP_S_KEY_3;
    SeNvmCtx.KeyList[itr++].KeyID = MC_NWK_S_KEY_3;
    SeNvmCtx.KeyList[itr].KeyID = SLOT_RAND_ZERO_KEY;

    // Set standard keys
    memcpy1( SeNvmCtx.KeyList[itr].KeyValue, zeroKey, KEY_SIZE );

    memset1( SeNvmCtx.DevEui, 0, SE_EUI_SIZE );
    memset1( SeNvmCtx.JoinEui, 0, SE_EUI_SIZE );

    // Assign callback
    if( seNvmCtxChanged != 0 )
    {
        SeNvmCtxChanged = seNvmCtxChanged;
    }
    else
    {
        SeNvmCtxChanged = DummyCB;
    }

    return SECURE_ELEMENT_SUCCESS;
}

SecureElementStatus_t SecureElementRestoreNvmCtx( void* seNvmCtx )
{
    // Restore nvm context
    if( seNvmCtx != 0 )
    {
        memcpy1( ( uint8_t* ) &SeNvmCtx, ( uint8_t* ) seNvmCtx, sizeof( SeNvmCtx ) );
        return SECURE_ELEMENT_SUCCESS;
    }
    else
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }
}

void* SecureElementGetNvmCtx( size_t* seNvmCtxSize )
{
    *seNvmCtxSize = sizeof( SeNvmCtx );
    return &SeNvmCtx;
}

SecureElementStatus_t SecureElementSetKey( KeyIdentifier_t keyID, uint8_t* key )
{
    if( key == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    for( uint8_t i = 0; i < NUM_OF_KEYS; i++ )
    {
        if( SeNvmCtx.KeyList[i].KeyID == keyID )
        {
            if( ( keyID == MC_KEY_0 ) || ( keyID == MC_KEY_1 ) || ( keyID == MC_KEY_2 ) || ( keyID == MC_KEY_3 ) )
            {  // Decrypt the key if its a Mckey
                SecureElementStatus_t retval = SECURE_ELEMENT_ERROR;
                uint8_t decryptedKey[16] = { 0 };

                retval = SecureElementAesEncrypt( key, 16, MC_KE_KEY, decryptedKey );

                memcpy1( SeNvmCtx.KeyList[i].KeyValue, decryptedKey, KEY_SIZE );
                SeNvmCtxChanged( );

                return retval;
            }
            else
            {
                memcpy1( SeNvmCtx.KeyList[i].KeyValue, key, KEY_SIZE );
                SeNvmCtxChanged( );
                return SECURE_ELEMENT_SUCCESS;
            }
        }
    }

    return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
}

SecureElementStatus_t SecureElementComputeAesCmac( uint8_t *micBxBuffer, uint8_t *buffer, uint16_t size, KeyIdentifier_t keyID, uint32_t* cmac )
{
    if( keyID >= LORAMAC_CRYPTO_MULTICAST_KEYS )
    {
        //Never accept multicast key identifier for cmac computation
        return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
    }

    return ComputeCmac( micBxBuffer, buffer, size, keyID, cmac );
}

SecureElementStatus_t SecureElementVerifyAesCmac( uint8_t* buffer, uint16_t size, uint32_t expectedCmac, KeyIdentifier_t keyID )
{
    if( buffer == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    SecureElementStatus_t retval = SECURE_ELEMENT_ERROR;
    uint32_t compCmac = 0;
    retval = ComputeCmac( NULL, buffer, size, keyID, &compCmac );
    if( retval != SECURE_ELEMENT_SUCCESS )
    {
        return retval;
    }

    if( expectedCmac != compCmac )
    {
        retval = SECURE_ELEMENT_FAIL_CMAC;
    }

    return retval;
}

SecureElementStatus_t SecureElementAesEncrypt( uint8_t* buffer, uint16_t size, KeyIdentifier_t keyID, uint8_t* encBuffer )
{
    if( buffer == NULL || encBuffer == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    // Check if the size is divisible by 16,
    if( ( size % 16 ) != 0 )
    {
        return SECURE_ELEMENT_ERROR_BUF_SIZE;
    }

    memset1( SeNvmCtx.AesContext.ksch, '\0', 240 );

    Key_t* pItem;
    SecureElementStatus_t retval = GetKeyByID( keyID, &pItem );

    if( retval == SECURE_ELEMENT_SUCCESS )
    {
        aes_set_key( pItem->KeyValue, 16, &SeNvmCtx.AesContext );

        uint8_t block = 0;

        while( size != 0 )
        {
            aes_encrypt( &buffer[block], &encBuffer[block], &SeNvmCtx.AesContext );
            block = block + 16;
            size = size - 16;
        }
    }
    return retval;
}

SecureElementStatus_t SecureElementDeriveAndStoreKey( Version_t version, uint8_t* input, KeyIdentifier_t rootKeyID, KeyIdentifier_t targetKeyID )
{
    if( input == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    SecureElementStatus_t retval = SECURE_ELEMENT_ERROR;
    uint8_t key[16] = { 0 };

    // In case of MC_KE_KEY, prevent other keys than NwkKey or AppKey for LoRaWAN 1.1 or later
    if( targetKeyID == MC_KE_KEY )
    {
        if( ( ( rootKeyID == APP_KEY ) && ( version.Fields.Minor == 0 ) ) || ( rootKeyID == NWK_KEY ) )
        {
            return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
        }
    }

    // Derive key
    retval = SecureElementAesEncrypt( input, 16, rootKeyID, key );
    if( retval != SECURE_ELEMENT_SUCCESS )
    {
        return retval;
    }

    // Store key
    retval = SecureElementSetKey( targetKeyID, key );
    if( retval != SECURE_ELEMENT_SUCCESS )
    {
        return retval;
    }

    return SECURE_ELEMENT_SUCCESS;
}

SecureElementStatus_t SecureElementRandomNumber( uint32_t* randomNum )
{
    if( randomNum == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }
    *randomNum = Radio.Random( );
    return SECURE_ELEMENT_SUCCESS;
}

SecureElementStatus_t SecureElementSetDevEui( uint8_t* devEui )
{
    if( devEui == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }
    memcpy1( SeNvmCtx.DevEui, devEui, SE_EUI_SIZE );
    SeNvmCtxChanged( );
    return SECURE_ELEMENT_SUCCESS;
}

uint8_t* SecureElementGetDevEui( void )
{
    return SeNvmCtx.DevEui;
}

SecureElementStatus_t SecureElementSetJoinEui( uint8_t* joinEui )
{
    if( joinEui == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }
    memcpy1( SeNvmCtx.JoinEui, joinEui, SE_EUI_SIZE );
    SeNvmCtxChanged( );
    return SECURE_ELEMENT_SUCCESS;
}

uint8_t* SecureElementGetJoinEui( void )
{
    return SeNvmCtx.JoinEui;
}
