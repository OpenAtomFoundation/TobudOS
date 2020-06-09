/*!
 * \file      LoRaMacCrypto.c
 *
 * \brief     LoRa MAC layer cryptography implementation
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
 * \author    Johannes Bruder ( STACKFORCE )
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "utilities.h"
#include "secure-element.h"

#include "LoRaMacParser.h"
#include "LoRaMacSerializer.h"
#include "LoRaMacCrypto.h"

/*!
 * Indicates if LoRaWAN 1.1.x crypto scheme is enabled
 */
#define USE_LRWAN_1_1_X_CRYPTO                      0

/*!
 * Indicates if a random devnonce must be used or not
 */
#define USE_RANDOM_DEV_NONCE                        1

/*
 * Frame direction definition for uplink communications
 */
#define UPLINK                          0

/*
 * Frame direction definition for downlink communications
 */
#define DOWNLINK                        1

/*
 * CMAC/AES Message Integrity Code (MIC) Block B0 size
 */
#define MIC_BLOCK_BX_SIZE               16

/*
 * Size of JoinReqType is field for integrity check
 */
#define JOIN_REQ_TYPE_SIZE              1

/*
 * Size of DevNonce is field for integrity check
 */
#define DEV_NONCE_SIZE                  2

/*
 * Number of security context entries
 */
#define NUM_OF_SEC_CTX                  5

/*
 * Size of the module context
 */
#define CRYPTO_CTX_SIZE                 sizeof( LoRaMacCryptoCtx_t )

/*
 * Size of the module non volatile context
 */
#define CRYPTO_NVM_CTX_SIZE             sizeof( LoRaMacCryptoNvmCtx_t )

/*
 * Maximum size of the message that can be handled by the crypto operations
 */
#define CRYPTO_MAXMESSAGE_SIZE          256

/*
 * Maximum size of the buffer for crypto operations
 */
#define CRYPTO_BUFFER_SIZE              CRYPTO_MAXMESSAGE_SIZE + MIC_BLOCK_BX_SIZE

/*
 * MIC computaion offset
 */
#define CRYPTO_MIC_COMPUTATION_OFFSET   JOIN_REQ_TYPE_SIZE + LORAMAC_JOIN_EUI_FIELD_SIZE + DEV_NONCE_SIZE + LORAMAC_MHDR_FIELD_SIZE

/*!
 * LoRaWAN Frame counter list.
 */
typedef struct sFCntList
{
    /*!
     * Uplink frame counter which is incremented with each uplink.
     */
    uint32_t FCntUp;
    /*!
     * Network downlink frame counter which is incremented with each downlink on FPort 0
     * or when the FPort field is missing.
     */
    uint32_t NFCntDown;
    /*!
     * Application downlink frame counter which is incremented with each downlink
     * on a port different than 0.
     */
    uint32_t AFCntDown;
    /*!
     * In case if the device is connected to a LoRaWAN 1.0 Server,
     * this counter is used for every kind of downlink frame.
     */
    uint32_t FCntDown;
    /*!
     * Multicast downlink counter for index 0
     */
    uint32_t McFCntDown0;
    /*!
     * Multicast downlink counter for index 1
     */
    uint32_t McFCntDown1;
    /*!
     * Multicast downlink counter for index 2
     */
    uint32_t McFCntDown2;
    /*!
     * Multicast downlink counter for index 3
     */
    uint32_t McFCntDown3;
}FCntList_t;

/*
 * LoRaMac Crypto Non Volatile Context structure
 */
typedef struct sLoRaMacCryptoNvmCtx
{
    /*
     * Stores the information if the device is connected to a LoRaWAN network
     * server with prior to 1.1.0 implementation.
     */
    Version_t LrWanVersion;
    /*
     * Device nonce is a counter starting at 0 when the device is initially
     * powered up and incremented with every JoinRequest.
     */
    uint16_t DevNonce;
    /*
     * JoinNonce is a device specific counter value (that never repeats itself)
     * provided by the join server and incremented with every JoinAccept message.
     */
    uint32_t JoinNonce;
    /*
     * Frame counter list
     */
    FCntList_t FCntList;
    /*
     * RJcount1 is a counter incremented with every Rejoin request Type 1 frame transmitted.
     */
    uint16_t RJcount1;
    /*
     * LastDownFCnt stores the information which frame counter was used to unsecure the last frame.
     * This information is needed to compute ConfFCnt in B1 block for the MIC.
     */
    uint32_t* LastDownFCnt;
}LoRaMacCryptoNvmCtx_t;

/*
 * LoRaMac Crypto Context structure
 */
typedef struct sLoRaMacCryptoCtx
{
    /*
     * RJcount0 is a counter incremented with every Type 0 or 2 Rejoin frame transmitted.
     */
    uint16_t RJcount0;
    /*
     * Non volatile module context structure
     */
    LoRaMacCryptoNvmCtx_t* NvmCtx;
    /*
     * Callback function to notify the upper layer about context change
     */
    LoRaMacCryptoNvmEvent EventCryptoNvmCtxChanged;
}LoRaMacCryptoCtx_t;

/*
 * Key-Address item
 */
typedef struct sKeyAddr
{
    /*
     * Address identifier
     */
    AddressIdentifier_t AddrID;
    /*
     * Application session key
     */
    KeyIdentifier_t AppSkey;
    /*
     * Network session key
     */
    KeyIdentifier_t NwkSkey;
    /*
     * Rootkey (Multicast only)
     */
    KeyIdentifier_t RootKey;
}KeyAddr_t;

/*
 *Crypto module context.
 */
static LoRaMacCryptoCtx_t CryptoCtx;

/*
 * Non volatile module context.
 */
static LoRaMacCryptoNvmCtx_t NvmCryptoCtx;

/*
 * Key-Address list
 */
static KeyAddr_t KeyAddrList[NUM_OF_SEC_CTX] =
    {
        { MULTICAST_0_ADDR, MC_APP_S_KEY_0, MC_NWK_S_KEY_0, MC_KEY_0 },
        { MULTICAST_1_ADDR, MC_APP_S_KEY_1, MC_NWK_S_KEY_1, MC_KEY_1 },
        { MULTICAST_2_ADDR, MC_APP_S_KEY_2, MC_NWK_S_KEY_2, MC_KEY_2 },
        { MULTICAST_3_ADDR, MC_APP_S_KEY_3, MC_NWK_S_KEY_3, MC_KEY_3 },
        { UNICAST_DEV_ADDR, APP_S_KEY, S_NWK_S_INT_KEY, NO_KEY }
    };

/*
 * Local functions
 */

/*
 * Encrypts the payload
 *
 * \param[IN]  keyID            - Key identifier
 * \param[IN]  address          - Address
 * \param[IN]  dir              - Frame direction ( Uplink or Downlink )
 * \param[IN]  frameCounter     - Frame counter
 * \param[IN]  size             - Size of data
 * \param[IN/OUT]  buffer       - Data buffer
 * \retval                      - Status of the operation
 */
static LoRaMacCryptoStatus_t PayloadEncrypt( uint8_t* buffer, int16_t size, KeyIdentifier_t keyID, uint32_t address, uint8_t dir, uint32_t frameCounter )
{
    if( buffer == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    uint8_t bufferIndex = 0;
    uint16_t ctr = 1;
    uint8_t sBlock[16] = { 0 };
    uint8_t aBlock[16] = { 0 };

    aBlock[0] = 0x01;

    aBlock[5] = dir;

    aBlock[6] = address & 0xFF;
    aBlock[7] = ( address >> 8 ) & 0xFF;
    aBlock[8] = ( address >> 16 ) & 0xFF;
    aBlock[9] = ( address >> 24 ) & 0xFF;

    aBlock[10] = frameCounter & 0xFF;
    aBlock[11] = ( frameCounter >> 8 ) & 0xFF;
    aBlock[12] = ( frameCounter >> 16 ) & 0xFF;
    aBlock[13] = ( frameCounter >> 24 ) & 0xFF;

    while( size > 0 )
    {
        aBlock[15] = ctr & 0xFF;
        ctr++;
        if( SecureElementAesEncrypt( aBlock, 16, keyID, sBlock ) != SECURE_ELEMENT_SUCCESS )
        {
            return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
        }

        for( uint8_t i = 0; i < ( ( size > 16 ) ? 16 : size ); i++ )
        {
            buffer[bufferIndex + i] = buffer[bufferIndex + i] ^ sBlock[i];
        }
        size -= 16;
        bufferIndex += 16;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

#if( USE_LRWAN_1_1_X_CRYPTO == 1 )
/*
 * Encrypts the FOpts
 *
 * \param[IN]  address          - Address
 * \param[IN]  dir              - Frame direction ( Uplink or Downlink )
 * \param[IN]  fCntID           - Frame counter identifier
 * \param[IN]  frameCounter     - Frame counter
 * \param[IN]  size             - Size of data
 * \param[IN/OUT]  buffer       - Data buffer
 * \retval                      - Status of the operation
 */
static LoRaMacCryptoStatus_t FOptsEncrypt( uint16_t size, uint32_t address, uint8_t dir, FCntIdentifier_t fCntID, uint32_t frameCounter, uint8_t* buffer )
{
    if( buffer == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    uint8_t bufferIndex = 0;
    uint8_t sBlock[16] = { 0 };
    uint8_t aBlock[16] = { 0 };

    aBlock[0] = 0x01;

    if( CryptoCtx.NvmCtx->LrWanVersion.Value > 0x01010000 )
    {
        // Introduced in LoRaWAN 1.1.1 specification
        switch( fCntID )
        {
            case FCNT_UP:
            {
                aBlock[4] = 0x01;
                break;
            }
            case N_FCNT_DOWN:
            {
                aBlock[4] = 0x01;
                break;
            }
            case A_FCNT_DOWN:
            {
                aBlock[4] = 0x02;
                break;
            }
            default:
                return LORAMAC_CRYPTO_FAIL_PARAM;
        }
    }

    aBlock[5] = dir;

    aBlock[6] = address & 0xFF;
    aBlock[7] = ( address >> 8 ) & 0xFF;
    aBlock[8] = ( address >> 16 ) & 0xFF;
    aBlock[9] = ( address >> 24 ) & 0xFF;

    aBlock[10] = frameCounter & 0xFF;
    aBlock[11] = ( frameCounter >> 8 ) & 0xFF;
    aBlock[12] = ( frameCounter >> 16 ) & 0xFF;
    aBlock[13] = ( frameCounter >> 24 ) & 0xFF;

    if( CryptoCtx.NvmCtx->LrWanVersion.Value > 0x01010000 )
    {
        // Introduced in LoRaWAN 1.1.1 specification
        aBlock[15] = 0x01;
    }

    if( size > 0 )
    {
        if( SecureElementAesEncrypt( aBlock, 16, NWK_S_ENC_KEY, sBlock ) != SECURE_ELEMENT_SUCCESS )
        {
            return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
        }
        for( uint8_t i = 0; i < size; i++ )
        {
            buffer[bufferIndex + i] = buffer[bufferIndex + i] ^ sBlock[i];
        }
    }

    return LORAMAC_CRYPTO_SUCCESS;
}
#endif

/*
 * Prepares B0 block for cmac computation.
 *
 * \param[IN]  msgLen         - Length of message
 * \param[IN]  keyID          - Key identifier
 * \param[IN]  isAck          - True if it is a acknowledge frame ( Sets ConfFCnt in B0 block )
 * \param[IN]  devAddr        - Device address
 * \param[IN]  dir            - Frame direction ( Uplink:0, Downlink:1 )
 * \param[IN]  fCnt           - Frame counter
 * \param[IN/OUT]  b0         - B0 block
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t PrepareB0( uint16_t msgLen, KeyIdentifier_t keyID, bool isAck, uint8_t dir, uint32_t devAddr, uint32_t fCnt, uint8_t* b0 )
{
    if( b0 == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    b0[0] = 0x49;

    if( ( isAck == true ) && ( dir == DOWNLINK ) )
    {
        // confFCnt contains the frame counter value modulo 2^16 of the "confirmed" uplink or downlink frame that is being acknowledged
        uint16_t confFCnt = 0;

        confFCnt = ( uint16_t )( CryptoCtx.NvmCtx->FCntList.FCntUp % 65536 );

        b0[1] = confFCnt & 0xFF;
        b0[2] = ( confFCnt >> 8 ) & 0xFF;
    }
    else
    {
        b0[1] = 0x00;
        b0[2] = 0x00;
    }

    b0[3] = 0x00;
    b0[4] = 0x00;

    b0[5] = dir;

    b0[6] = devAddr & 0xFF;
    b0[7] = ( devAddr >> 8 ) & 0xFF;
    b0[8] = ( devAddr >> 16 ) & 0xFF;
    b0[9] = ( devAddr >> 24 ) & 0xFF;

    b0[10] = fCnt & 0xFF;
    b0[11] = ( fCnt >> 8 ) & 0xFF;
    b0[12] = ( fCnt >> 16 ) & 0xFF;
    b0[13] = ( fCnt >> 24 ) & 0xFF;

    b0[14] = 0x00;

    b0[15] = msgLen & 0xFF;

    return LORAMAC_CRYPTO_SUCCESS;
}

/*
 * Computes cmac with adding B0 block in front.
 *
 *  cmac = aes128_cmac(keyID, B0 | msg)
 *
 * \param[IN]  msg            - Message to compute the integrity code
 * \param[IN]  len            - Length of message
 * \param[IN]  keyID          - Key identifier
 * \param[IN]  isAck          - True if it is a acknowledge frame ( Sets ConfFCnt in B0 block )
 * \param[IN]  devAddr        - Device address
 * \param[IN]  dir            - Frame direction ( Uplink:0, Downlink:1 )
 * \param[IN]  fCnt           - Frame counter
 * \param[OUT] cmac           - Computed cmac
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t ComputeCmacB0( uint8_t* msg, uint16_t len, KeyIdentifier_t keyID, bool isAck, uint8_t dir, uint32_t devAddr, uint32_t fCnt, uint32_t* cmac )
{
    if( ( msg == 0 ) || ( cmac == 0 ) )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }
    if( len > CRYPTO_MAXMESSAGE_SIZE )
    {
        return LORAMAC_CRYPTO_ERROR_BUF_SIZE;
    }

    uint8_t micBuff[MIC_BLOCK_BX_SIZE];

    // Initialize the first Block
    PrepareB0( len, keyID, isAck, dir, devAddr, fCnt, micBuff );

    if( SecureElementComputeAesCmac( micBuff, msg, len, keyID, cmac ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }
    return LORAMAC_CRYPTO_SUCCESS;
}

/*!
 * Verifies cmac with adding B0 block in front.
 *
 * \param[IN]  msg            - Message to compute the integrity code
 * \param[IN]  len            - Length of message
 * \param[IN]  keyID          - Key identifier
 * \param[IN]  isAck          - True if it is a acknowledge frame ( Sets ConfFCnt in B0 block )
 * \param[IN]  devAddr        - Device address
 * \param[IN]  dir            - Frame direction ( Uplink:0, Downlink:1 )
 * \param[IN]  fCnt           - Frame counter
 * \param[in]  expectedCmac   - Expected cmac
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t VerifyCmacB0( uint8_t* msg, uint16_t len, KeyIdentifier_t keyID, bool isAck, uint8_t dir, uint32_t devAddr, uint32_t fCnt, uint32_t expectedCmac )
{
    if( msg == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }
    if( len > CRYPTO_MAXMESSAGE_SIZE )
    {
        return LORAMAC_CRYPTO_ERROR_BUF_SIZE;
    }

    uint8_t micBuff[CRYPTO_BUFFER_SIZE];
    memset1( micBuff, 0, CRYPTO_BUFFER_SIZE );

    // Initialize the first Block
    PrepareB0( len, keyID, isAck, dir, devAddr, fCnt, micBuff );

    // Copy the given data to the mic computation buffer
    memcpy1( ( micBuff + MIC_BLOCK_BX_SIZE ), msg, len );

    SecureElementStatus_t retval = SECURE_ELEMENT_ERROR;
    retval = SecureElementVerifyAesCmac( micBuff, ( len + MIC_BLOCK_BX_SIZE ), expectedCmac, keyID );

    if( retval == SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_SUCCESS;
    }
    else if( retval == SECURE_ELEMENT_FAIL_CMAC )
    {
        return LORAMAC_CRYPTO_FAIL_MIC;
    }

    return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
}

#if( USE_LRWAN_1_1_X_CRYPTO == 1 )
/*
 * Prpares B1 block for cmac computation.
 *
 * \param[IN]  msgLen         - Length of message
 * \param[IN]  keyID          - Key identifier
 * \param[IN]  isAck          - True if it is a acknowledge frame ( Sets ConfFCnt in B0 block )
 * \param[IN]  txDr           - Data rate used for the transmission
 * \param[IN]  txCh           - Index of the channel used for the transmission
 * \param[IN]  devAddr        - Device address
 * \param[IN]  fCntUp         - Frame counter
 * \param[IN/OUT]  b0         - B0 block
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t PrepareB1( uint16_t msgLen, KeyIdentifier_t keyID, bool isAck, uint8_t txDr, uint8_t txCh, uint32_t devAddr, uint32_t fCntUp, uint8_t* b1 )
{
    if( b1 == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    b1[0] = 0x49;

    if( isAck == true )
    {
        // confFCnt contains the frame counter value modulo 2^16 of the "confirmed" uplink frame that is being acknowledged
        uint16_t confFCnt = ( uint16_t )( *CryptoCtx.NvmCtx->LastDownFCnt % 65536 );
        b1[1] = confFCnt & 0xFF;
        b1[2] = ( confFCnt >> 8 ) & 0xFF;
    }
    else
    {
        b1[1] = 0x00;
        b1[2] = 0x00;
    }

    b1[3] = txDr;
    b1[4] = txCh;
    b1[5] = UPLINK;  // dir = Uplink

    b1[6] = devAddr & 0xFF;
    b1[7] = ( devAddr >> 8 ) & 0xFF;
    b1[8] = ( devAddr >> 16 ) & 0xFF;
    b1[9] = ( devAddr >> 24 ) & 0xFF;

    b1[10] = fCntUp & 0xFF;
    b1[11] = ( fCntUp >> 8 ) & 0xFF;
    b1[12] = ( fCntUp >> 16 ) & 0xFF;
    b1[13] = ( fCntUp >> 24 ) & 0xFF;

    b1[14] = 0x00;

    b1[15] = msgLen & 0xFF;

    return LORAMAC_CRYPTO_SUCCESS;
}

/*
 * Computes cmac with adding B1 block in front ( only for Uplink frames LoRaWAN 1.1 )
 *
 *  cmac = aes128_cmac(keyID, B1 | msg)
 *
 * \param[IN]  msg            - Message to calculate the Integrity code
 * \param[IN]  len            - Length of message
 * \param[IN]  keyID          - Key identifier
 * \param[IN]  isAck          - True if it is a acknowledge frame ( Sets ConfFCnt in B0 block )
 * \param[IN]  txDr           - Data rate used for the transmission
 * \param[IN]  txCh           - Index of the channel used for the transmission
 * \param[IN]  devAddr        - Device address
 * \param[IN]  fCntUp         - Uplink Frame counter
 * \param[OUT] cmac           - Computed cmac
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t ComputeCmacB1( uint8_t* msg, uint16_t len, KeyIdentifier_t keyID, bool isAck, uint8_t txDr, uint8_t txCh, uint32_t devAddr, uint32_t fCntUp, uint32_t* cmac )
{
    if( ( msg == 0 ) || ( cmac == 0 ) )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }
    if( len > CRYPTO_MAXMESSAGE_SIZE )
    {
        return LORAMAC_CRYPTO_ERROR_BUF_SIZE;
    }

    uint8_t micBuff[MIC_BLOCK_BX_SIZE];

    // Initialize the first Block
    PrepareB1( len, keyID, isAck, txDr, txCh, devAddr, fCntUp, micBuff );

    if( SecureElementComputeAesCmac( micBuff, msg, len, keyID, cmac ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }
    return LORAMAC_CRYPTO_SUCCESS;
}
#endif

/*
 * Gets security item from list.
 *
 * \param[IN]  addrID          - Address identifier
 * \param[OUT] keyItem        - Key item reference
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t GetKeyAddrItem( AddressIdentifier_t addrID, KeyAddr_t** item )
{
    for( uint8_t i = 0; i < NUM_OF_SEC_CTX; i++ )
    {
        if( KeyAddrList[i].AddrID == addrID )
        {
            *item = &( KeyAddrList[i] );
            return LORAMAC_CRYPTO_SUCCESS;
        }
    }
    return LORAMAC_CRYPTO_ERROR_INVALID_ADDR_ID;
}

/*
 * Derives a session key as of LoRaWAN versions prior to 1.1.0
 *
 * \param[IN]  keyID          - Key Identifier for the key to be calculated
 * \param[IN]  joinNonce      - Sever nonce
 * \param[IN]  netID          - Network Identifier
 * \param[IN]  deviceNonce    - Device nonce
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t DeriveSessionKey10x( KeyIdentifier_t keyID, uint8_t* joinNonce, uint8_t* netID, uint8_t* devNonce )
{
    if( ( joinNonce == 0 ) || ( netID == 0 ) || ( devNonce == 0 ) )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    uint8_t compBase[16] = { 0 };

    switch( keyID )
    {
        case F_NWK_S_INT_KEY:
        case S_NWK_S_INT_KEY:
        case NWK_S_ENC_KEY:
            compBase[0] = 0x01;
            break;
        case APP_S_KEY:
            compBase[0] = 0x02;
            break;
        default:
            return LORAMAC_CRYPTO_ERROR_INVALID_KEY_ID;
    }

    memcpy1( compBase + 1, joinNonce, 3 );
    memcpy1( compBase + 4, netID, 3 );
    memcpy1( compBase + 7, devNonce, 2 );

    if( SecureElementDeriveAndStoreKey( CryptoCtx.NvmCtx->LrWanVersion, compBase, NWK_KEY, keyID ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

#if( USE_LRWAN_1_1_X_CRYPTO == 1 )
/*
 * Derives a session key as of LoRaWAN 1.1.0
 *
 * \param[IN]  keyID          - Key Identifier for the key to be calculated
 * \param[IN]  joinNonce      - Sever nonce
 * \param[IN]  joinEUI        - Join Server EUI
 * \param[IN]  deviceNonce    - Device nonce
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t DeriveSessionKey11x( KeyIdentifier_t keyID, uint8_t* joinNonce, uint8_t* joinEUI, uint8_t* devNonce )
{
    if( ( joinNonce == 0 ) || ( joinEUI == 0 ) || ( devNonce == 0 ) )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    uint8_t compBase[16] = { 0 };
    KeyIdentifier_t rootKeyId = NWK_KEY;

    switch( keyID )
    {
        case F_NWK_S_INT_KEY:
            compBase[0] = 0x01;
            break;
        case S_NWK_S_INT_KEY:
            compBase[0] = 0x03;
            break;
        case NWK_S_ENC_KEY:
            compBase[0] = 0x04;
            break;
        case APP_S_KEY:
            rootKeyId = APP_KEY;
            compBase[0] = 0x02;
            break;
        default:
            return LORAMAC_CRYPTO_ERROR_INVALID_KEY_ID;
    }

    memcpy1( compBase + 1, joinNonce, 3 );
    memcpyr( compBase + 4, joinEUI, 8 );
    memcpy1( compBase + 12, devNonce, 2 );

    if( SecureElementDeriveAndStoreKey( CryptoCtx.NvmCtx->LrWanVersion, compBase, rootKeyId, keyID ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

/*
 * Derives a life time session key (JSIntKey or JSEncKey)  as of LoRaWAN 1.1.0
 *
 * \param[IN]  keyID          - Key Identifier for the key to be calculated
 * \param[IN]  devEUI         - Device EUI
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t DeriveLifeTimeSessionKey( KeyIdentifier_t keyID, uint8_t* devEUI )
{
    if( devEUI == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    uint8_t compBase[16] = { 0 };

    switch( keyID )
    {
        case J_S_INT_KEY:
            compBase[0] = 0x06;
            break;
        case J_S_ENC_KEY:
            compBase[0] = 0x05;
            break;
        default:
            return LORAMAC_CRYPTO_ERROR_INVALID_KEY_ID;
    }

    memcpyr( compBase + 1, devEUI, 8 );

    if( SecureElementDeriveAndStoreKey( CryptoCtx.NvmCtx->LrWanVersion, compBase, NWK_KEY, keyID ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}
#endif

/*
 * Gets the last received frame counter
 *
 * \param[IN]     fCntID       - Frame counter identifier
 * \param[IN]     lastDown     - Last downlink counter value
 *
 * \retval                     - Status of the operation
 */
static LoRaMacCryptoStatus_t GetLastFcntDown( FCntIdentifier_t fCntID, uint32_t* lastDown )
{
    if( lastDown == NULL )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }
    switch( fCntID )
    {
        case N_FCNT_DOWN:
            *lastDown = CryptoCtx.NvmCtx->FCntList.NFCntDown;
            CryptoCtx.NvmCtx->LastDownFCnt = &CryptoCtx.NvmCtx->FCntList.NFCntDown;
            break;
        case A_FCNT_DOWN:
            *lastDown = CryptoCtx.NvmCtx->FCntList.AFCntDown;
            CryptoCtx.NvmCtx->LastDownFCnt = &CryptoCtx.NvmCtx->FCntList.AFCntDown;
            break;
        case FCNT_DOWN:
            *lastDown = CryptoCtx.NvmCtx->FCntList.FCntDown;
            CryptoCtx.NvmCtx->LastDownFCnt = &CryptoCtx.NvmCtx->FCntList.FCntDown;
            break;
        case MC_FCNT_DOWN_0:
            *lastDown = CryptoCtx.NvmCtx->FCntList.McFCntDown0;
            break;
        case MC_FCNT_DOWN_1:
            *lastDown = CryptoCtx.NvmCtx->FCntList.McFCntDown1;
            break;
        case MC_FCNT_DOWN_2:
            *lastDown = CryptoCtx.NvmCtx->FCntList.McFCntDown2;
            break;
        case MC_FCNT_DOWN_3:
            *lastDown = CryptoCtx.NvmCtx->FCntList.McFCntDown3;
            break;
        default:
            return LORAMAC_CRYPTO_FAIL_FCNT_ID;
    }
    return LORAMAC_CRYPTO_SUCCESS;
}

/*
 * Checks the downlink counter value
 *
 * \param[IN]     fCntID       - Frame counter identifier
 * \param[IN]     currentDown  - Current downlink counter value
 *
 * \retval                     - Status of the operation
 */
static bool CheckFCntDown( FCntIdentifier_t fCntID, uint32_t currentDown )
{
    uint32_t lastDown = 0;
    if( GetLastFcntDown( fCntID, &lastDown ) != LORAMAC_CRYPTO_SUCCESS )
    {
        return false;
    }
    if( ( currentDown > lastDown ) ||
        // For LoRaWAN 1.0.X only. Allow downlink frames of 0
        ( lastDown == FCNT_DOWN_INITAL_VALUE ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*!
 * Updates the reference downlink counter
 *
 * \param[IN]     fCntID        - Frame counter identifier
 * \param[IN]     currentDown   - Current downlink counter value
 *
 * \retval                     - Status of the operation
 */
static void UpdateFCntDown( FCntIdentifier_t fCntID, uint32_t currentDown )
{
    switch( fCntID )
    {
        case N_FCNT_DOWN:
            CryptoCtx.NvmCtx->FCntList.NFCntDown = currentDown;
            break;
        case A_FCNT_DOWN:
            CryptoCtx.NvmCtx->FCntList.AFCntDown = currentDown;
            break;
        case FCNT_DOWN:
            CryptoCtx.NvmCtx->FCntList.FCntDown = currentDown;
            break;
        case MC_FCNT_DOWN_0:
            CryptoCtx.NvmCtx->FCntList.McFCntDown0 = currentDown;
            break;
        case MC_FCNT_DOWN_1:
            CryptoCtx.NvmCtx->FCntList.McFCntDown1 = currentDown;
            break;
        case MC_FCNT_DOWN_2:
            CryptoCtx.NvmCtx->FCntList.McFCntDown2 = currentDown;
            break;
        case MC_FCNT_DOWN_3:
            CryptoCtx.NvmCtx->FCntList.McFCntDown3 = currentDown;
            break;
        default:
            break;
    }
    CryptoCtx.EventCryptoNvmCtxChanged( );
}

/*!
 * Resets the frame counters
 */
static void ResetFCnts( void )
{

    CryptoCtx.NvmCtx->FCntList.FCntUp = 0;
    CryptoCtx.NvmCtx->FCntList.NFCntDown = FCNT_DOWN_INITAL_VALUE;
    CryptoCtx.NvmCtx->FCntList.AFCntDown = FCNT_DOWN_INITAL_VALUE;
    CryptoCtx.NvmCtx->FCntList.FCntDown = FCNT_DOWN_INITAL_VALUE;
    CryptoCtx.NvmCtx->LastDownFCnt = &CryptoCtx.NvmCtx->FCntList.FCntDown;

    CryptoCtx.NvmCtx->FCntList.McFCntDown0 = FCNT_DOWN_INITAL_VALUE;
    CryptoCtx.NvmCtx->FCntList.McFCntDown1 = FCNT_DOWN_INITAL_VALUE;
    CryptoCtx.NvmCtx->FCntList.McFCntDown2 = FCNT_DOWN_INITAL_VALUE;
    CryptoCtx.NvmCtx->FCntList.McFCntDown3 = FCNT_DOWN_INITAL_VALUE;

    CryptoCtx.EventCryptoNvmCtxChanged( );
}

/*
 * Dummy callback in case if the user provides NULL function pointer
 */
static void DummyCB( void )
{
    return;
}

/*
 *  API functions
 */

LoRaMacCryptoStatus_t LoRaMacCryptoInit( LoRaMacCryptoNvmEvent cryptoNvmCtxChanged )
{
    // Assign non volatile context
    CryptoCtx.NvmCtx = &NvmCryptoCtx;

    // Assign callback
    if( cryptoNvmCtxChanged != 0 )
    {
        CryptoCtx.EventCryptoNvmCtxChanged = cryptoNvmCtxChanged;
    }
    else
    {
        CryptoCtx.EventCryptoNvmCtxChanged = DummyCB;
    }

    // Initialize with default
    memset1( (uint8_t*) CryptoCtx.NvmCtx, 0, sizeof( LoRaMacCryptoNvmCtx_t ) );

    // Set default LoRaWAN version
    CryptoCtx.NvmCtx->LrWanVersion.Fields.Major = 1;
    CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor = 1;
    CryptoCtx.NvmCtx->LrWanVersion.Fields.Revision = 1;
    CryptoCtx.NvmCtx->LrWanVersion.Fields.Rfu = 0;

    // Reset frame counters
    ResetFCnts( );

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoSetLrWanVersion( Version_t version )
{
    CryptoCtx.NvmCtx->LrWanVersion = version;
    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoRestoreNvmCtx( void* cryptoNvmCtx )
{
    // Restore module context
    if( cryptoNvmCtx != 0 )
    {
        memcpy1( ( uint8_t* ) &NvmCryptoCtx, ( uint8_t* ) cryptoNvmCtx, CRYPTO_NVM_CTX_SIZE );
        return LORAMAC_CRYPTO_SUCCESS;
    }
    else
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }
}

void* LoRaMacCryptoGetNvmCtx( size_t* cryptoNvmCtxSize )
{
    *cryptoNvmCtxSize = CRYPTO_NVM_CTX_SIZE;
    return &NvmCryptoCtx;
}

LoRaMacCryptoStatus_t LoRaMacCryptoGetFCntUp( uint32_t* currentUp )
{
    if( currentUp == NULL )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    *currentUp = CryptoCtx.NvmCtx->FCntList.FCntUp + 1;

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoGetFCntDown( FCntIdentifier_t fCntID, uint16_t maxFCntGap, uint32_t frameFcnt, uint32_t* currentDown )
{
    uint32_t lastDown = 0;
    int32_t fCntDiff = 0;
    LoRaMacCryptoStatus_t cryptoStatus = LORAMAC_CRYPTO_ERROR;

    if( currentDown == NULL )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    cryptoStatus = GetLastFcntDown( fCntID, &lastDown );
    if( cryptoStatus != LORAMAC_CRYPTO_SUCCESS )
    {
        return cryptoStatus;
    }

    // For LoRaWAN 1.0.X only, allow downlink frames of 0
    if( lastDown == FCNT_DOWN_INITAL_VALUE )
    {
         *currentDown = frameFcnt;
    }
    else
    {
        // Add difference, consider roll-over
        fCntDiff = ( int32_t )( ( int64_t )frameFcnt - ( int64_t )( lastDown & 0x0000FFFF ) );

        if( fCntDiff > 0 )
        {  // Positive difference
            *currentDown = lastDown + fCntDiff;
        }
        else if( fCntDiff == 0 )
        {  // Duplicate FCnt value, keep the current value.
            *currentDown = lastDown;
            return LORAMAC_CRYPTO_FAIL_FCNT_DUPLICATED;
        }
        else
        {  // Negative difference, assume a roll-over of one uint16_t
            *currentDown = ( lastDown & 0xFFFF0000 ) + 0x10000 + frameFcnt;
        }
    }


    // For LoRaWAN 1.0.X only, check maxFCntGap
    if( CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor == 0 )
    {
        if( ( ( int64_t )*currentDown - ( int64_t )lastDown ) >= maxFCntGap )
        {
            return LORAMAC_CRYPTO_FAIL_MAX_GAP_FCNT;
        }
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoSetMulticastReference( MulticastCtx_t* multicastList )
{
    if( multicastList == NULL )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    multicastList[0].DownLinkCounter = &CryptoCtx.NvmCtx->FCntList.McFCntDown0;
    multicastList[1].DownLinkCounter = &CryptoCtx.NvmCtx->FCntList.McFCntDown1;
    multicastList[2].DownLinkCounter = &CryptoCtx.NvmCtx->FCntList.McFCntDown2;
    multicastList[3].DownLinkCounter = &CryptoCtx.NvmCtx->FCntList.McFCntDown3;

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoSetKey( KeyIdentifier_t keyID, uint8_t* key )
{
    if( SecureElementSetKey( keyID, key ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }
    if( ( keyID == GEN_APP_KEY ) || ( keyID == APP_KEY ) )
    {
        // Derive lifetime keys
        if( LoRaMacCryptoDeriveMcRootKey( keyID ) != LORAMAC_CRYPTO_SUCCESS )
        {
            return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
        }
        if( LoRaMacCryptoDeriveMcKEKey( MC_ROOT_KEY ) != LORAMAC_CRYPTO_SUCCESS )
        {
            return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
        }
    }
    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoPrepareJoinRequest( LoRaMacMessageJoinRequest_t* macMsg )
{
    if( macMsg == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }
    KeyIdentifier_t micComputationKeyID = NWK_KEY;

    // Add device nonce
#if ( USE_RANDOM_DEV_NONCE == 1 )
    uint32_t devNonce = 0;
    SecureElementRandomNumber( &devNonce );
    CryptoCtx.NvmCtx->DevNonce = devNonce;
#else
    CryptoCtx.NvmCtx->DevNonce++;
#endif
    CryptoCtx.EventCryptoNvmCtxChanged( );
    macMsg->DevNonce = CryptoCtx.NvmCtx->DevNonce;

#if( USE_LRWAN_1_1_X_CRYPTO == 1 )
    // Derive lifetime session keys
    if( DeriveLifeTimeSessionKey( J_S_INT_KEY, macMsg->DevEUI ) != LORAMAC_CRYPTO_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR;
    }
    if( DeriveLifeTimeSessionKey( J_S_ENC_KEY, macMsg->DevEUI ) != LORAMAC_CRYPTO_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR;
    }
#endif

    // Serialize message
    if( LoRaMacSerializerJoinRequest( macMsg ) != LORAMAC_SERIALIZER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SERIALIZER;
    }

    // Compute mic
    if( SecureElementComputeAesCmac( NULL, macMsg->Buffer, ( LORAMAC_JOIN_REQ_MSG_SIZE - LORAMAC_MIC_FIELD_SIZE ), micComputationKeyID, &macMsg->MIC ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    // Reserialize message to add the MIC
    if( LoRaMacSerializerJoinRequest( macMsg ) != LORAMAC_SERIALIZER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SERIALIZER;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoPrepareReJoinType1( LoRaMacMessageReJoinType1_t* macMsg )
{
    if( macMsg == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    // Check for RJcount1 overflow
    if( CryptoCtx.NvmCtx->RJcount1 == 65535 )
    {
        return LORAMAC_CRYPTO_ERROR_RJCOUNT1_OVERFLOW;
    }

    // Serialize message
    if( LoRaMacSerializerReJoinType1( macMsg ) != LORAMAC_SERIALIZER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SERIALIZER;
    }

    // Compute mic
    // cmac = aes128_cmac(JSIntKey, MHDR | RejoinType | JoinEUI| DevEUI | RJcount1)
    if( SecureElementComputeAesCmac( NULL, macMsg->Buffer, ( LORAMAC_RE_JOIN_1_MSG_SIZE - LORAMAC_MIC_FIELD_SIZE ), J_S_INT_KEY, &macMsg->MIC ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    // Reserialize message to add the MIC
    if( LoRaMacSerializerReJoinType1( macMsg ) != LORAMAC_SERIALIZER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SERIALIZER;
    }

    // Increment RJcount1
    CryptoCtx.NvmCtx->RJcount1++;
    CryptoCtx.EventCryptoNvmCtxChanged( );

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoPrepareReJoinType0or2( LoRaMacMessageReJoinType0or2_t* macMsg )
{
    if( macMsg == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    // Check for RJcount0 overflow
    if( CryptoCtx.RJcount0 == 65535 )
    {
        return LORAMAC_CRYPTO_FAIL_RJCOUNT0_OVERFLOW;
    }

    // Serialize message
    if( LoRaMacSerializerReJoinType0or2( macMsg ) != LORAMAC_SERIALIZER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SERIALIZER;
    }

    // Compute mic
    // cmac = aes128_cmac(SNwkSIntKey, MHDR | Rejoin Type | NetID | DevEUI | RJcount0)
    if( SecureElementComputeAesCmac( NULL, macMsg->Buffer, ( LORAMAC_RE_JOIN_0_2_MSG_SIZE - LORAMAC_MIC_FIELD_SIZE ), S_NWK_S_INT_KEY, &macMsg->MIC ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    // Reserialize message to add the MIC
    if( LoRaMacSerializerReJoinType0or2( macMsg ) != LORAMAC_SERIALIZER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SERIALIZER;
    }

    // Increment RJcount0
    CryptoCtx.RJcount0++;

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoHandleJoinAccept( JoinReqIdentifier_t joinReqType, uint8_t* joinEUI, LoRaMacMessageJoinAccept_t* macMsg )
{
    if( ( macMsg == 0 ) || ( joinEUI == 0 ) )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    LoRaMacCryptoStatus_t retval = LORAMAC_CRYPTO_ERROR;
    KeyIdentifier_t micComputationKeyID;
    KeyIdentifier_t encryptionKeyID;
    uint8_t micComputationOffset = 0;
#if( USE_LRWAN_1_1_X_CRYPTO == 1 )
    uint8_t* devNonceForKeyDerivation = ( uint8_t* ) &CryptoCtx.NvmCtx->DevNonce;
#endif

    // Determine decryption key and DevNonce for key derivation
    if( joinReqType == JOIN_REQ )
    {
        encryptionKeyID = NWK_KEY;
        micComputationOffset = CRYPTO_MIC_COMPUTATION_OFFSET;
    }
#if( USE_LRWAN_1_1_X_CRYPTO == 1 )
    else
    {
        encryptionKeyID = J_S_ENC_KEY;

        // If Join-accept is a reply to a rejoin, the RJcount(0 or 1) replaces DevNonce in the key derivation process.
        if( ( joinReqType == REJOIN_REQ_0 ) || ( joinReqType == REJOIN_REQ_2 ) )
        {
            devNonceForKeyDerivation = ( uint8_t* ) &CryptoCtx.RJcount0;
        }
        else
        {
            devNonceForKeyDerivation = ( uint8_t* ) &CryptoCtx.NvmCtx->RJcount1;
        }
    }
#endif
    // Decrypt header, skip MHDR
    uint8_t procBuffer[CRYPTO_MAXMESSAGE_SIZE + CRYPTO_MIC_COMPUTATION_OFFSET];
    memset1( procBuffer, 0, ( macMsg->BufSize + micComputationOffset ) );

    if( SecureElementAesEncrypt( macMsg->Buffer + LORAMAC_MHDR_FIELD_SIZE, ( macMsg->BufSize - LORAMAC_MHDR_FIELD_SIZE ), encryptionKeyID, ( procBuffer + micComputationOffset ) ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }
    // Copy the result to an offset location to keep space for additional information which have to be added in case of 1.1 and later
    memcpy1( macMsg->Buffer + LORAMAC_MHDR_FIELD_SIZE, ( procBuffer + micComputationOffset ), ( macMsg->BufSize - LORAMAC_MHDR_FIELD_SIZE ) );

    // Parse the message
    if( LoRaMacParserJoinAccept( macMsg ) != LORAMAC_PARSER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_PARSER;
    }

    // Is it a LoRaWAN 1.1.0 or later ?
    if( macMsg->DLSettings.Bits.OptNeg == 1 )
    {
        CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor = 1;
        micComputationKeyID = J_S_INT_KEY;
    }
    else
    {
        CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor = 0;
        micComputationKeyID = NWK_KEY;
    }

    // Verify mic
    if( CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor == 0 )
    {
        // For legacy mode :
        //   cmac = aes128_cmac(NwkKey, MHDR |  JoinNonce | NetID | DevAddr | DLSettings | RxDelay | CFList | CFListType)
        if( SecureElementVerifyAesCmac( macMsg->Buffer, ( macMsg->BufSize - LORAMAC_MIC_FIELD_SIZE ), macMsg->MIC, micComputationKeyID ) != SECURE_ELEMENT_SUCCESS )
        {
            return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
        }
    }
    else
    {
        // For 1.1 and later:
        //   cmac = aes128_cmac(JSIntKey, JoinReqType | JoinEUI | DevNonce | MHDR | JoinNonce | NetID | DevAddr | DLSettings | RxDelay | CFList | CFListType)

        // Prepare the msg for integrity check (adding JoinReqType, JoinEUI and DevNonce)
        uint16_t bufItr = 0;
        procBuffer[bufItr++] = ( uint8_t ) joinReqType;

        memcpyr( &procBuffer[bufItr], joinEUI, LORAMAC_JOIN_EUI_FIELD_SIZE );
        bufItr += LORAMAC_JOIN_EUI_FIELD_SIZE;

        procBuffer[bufItr++] = CryptoCtx.NvmCtx->DevNonce & 0xFF;
        procBuffer[bufItr++] = ( CryptoCtx.NvmCtx->DevNonce >> 8 ) & 0xFF;

        procBuffer[bufItr++] = macMsg->MHDR.Value;

        if( SecureElementVerifyAesCmac( procBuffer,  ( macMsg->BufSize + micComputationOffset - LORAMAC_MHDR_FIELD_SIZE - LORAMAC_MIC_FIELD_SIZE ), macMsg->MIC, micComputationKeyID ) != SECURE_ELEMENT_SUCCESS )
        {
            return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
        }

        // Check if the JoinNonce is greater as the previous one
        uint32_t currentJoinNonce = 0;
        currentJoinNonce = ( uint32_t ) macMsg->JoinNonce[0];
        currentJoinNonce |= ( ( uint32_t ) macMsg->JoinNonce[1] << 8 );
        currentJoinNonce |= ( ( uint32_t ) macMsg->JoinNonce[2] << 16 );

        if( currentJoinNonce > CryptoCtx.NvmCtx->JoinNonce )
        {
            CryptoCtx.NvmCtx->JoinNonce = currentJoinNonce;
            CryptoCtx.EventCryptoNvmCtxChanged( );
        }
        else
        {
            return LORAMAC_CRYPTO_FAIL_JOIN_NONCE;
        }
    }

    // Derive session keys
#if( USE_LRWAN_1_1_X_CRYPTO == 1 )
    if( CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor == 1 )
    {
        // Derive lifetime keys
        retval = LoRaMacCryptoDeriveMcRootKey( APP_KEY );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = LoRaMacCryptoDeriveMcKEKey( MC_ROOT_KEY );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = DeriveSessionKey11x( F_NWK_S_INT_KEY, macMsg->JoinNonce, joinEUI, devNonceForKeyDerivation );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = DeriveSessionKey11x( S_NWK_S_INT_KEY, macMsg->JoinNonce, joinEUI, devNonceForKeyDerivation );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = DeriveSessionKey11x( NWK_S_ENC_KEY, macMsg->JoinNonce, joinEUI, devNonceForKeyDerivation );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = DeriveSessionKey11x( APP_S_KEY, macMsg->JoinNonce, joinEUI, devNonceForKeyDerivation );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }
    }
    else
#endif
    {
        // prior LoRaWAN 1.1.0
        retval = LoRaMacCryptoDeriveMcRootKey( GEN_APP_KEY );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = LoRaMacCryptoDeriveMcKEKey( MC_ROOT_KEY );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = DeriveSessionKey10x( APP_S_KEY, macMsg->JoinNonce, macMsg->NetID, ( uint8_t* ) &CryptoCtx.NvmCtx->DevNonce );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = DeriveSessionKey10x( NWK_S_ENC_KEY, macMsg->JoinNonce, macMsg->NetID, ( uint8_t* ) &CryptoCtx.NvmCtx->DevNonce );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = DeriveSessionKey10x( F_NWK_S_INT_KEY, macMsg->JoinNonce, macMsg->NetID, ( uint8_t* ) &CryptoCtx.NvmCtx->DevNonce );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = DeriveSessionKey10x( S_NWK_S_INT_KEY, macMsg->JoinNonce, macMsg->NetID, ( uint8_t* ) &CryptoCtx.NvmCtx->DevNonce );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }
    }

    // Join-Accept is successfully processed, reset frame counters
    CryptoCtx.RJcount0 = 0;
    CryptoCtx.NvmCtx->FCntList.FCntUp = 0;
    CryptoCtx.NvmCtx->FCntList.FCntDown = FCNT_DOWN_INITAL_VALUE;
    CryptoCtx.NvmCtx->FCntList.NFCntDown = FCNT_DOWN_INITAL_VALUE;
    CryptoCtx.NvmCtx->FCntList.AFCntDown = FCNT_DOWN_INITAL_VALUE;
    CryptoCtx.EventCryptoNvmCtxChanged( );

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoSecureMessage( uint32_t fCntUp, uint8_t txDr, uint8_t txCh, LoRaMacMessageData_t* macMsg )
{
    LoRaMacCryptoStatus_t retval = LORAMAC_CRYPTO_ERROR;
    KeyIdentifier_t payloadDecryptionKeyID = APP_S_KEY;

    if( macMsg == NULL )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    if( fCntUp < CryptoCtx.NvmCtx->FCntList.FCntUp )
    {
        return LORAMAC_CRYPTO_FAIL_FCNT_SMALLER;
    }

    // Encrypt payload
    if( macMsg->FPort == 0 )
    {
        // Use network session key
        payloadDecryptionKeyID = NWK_S_ENC_KEY;
    }

    if( fCntUp > CryptoCtx.NvmCtx->FCntList.FCntUp )
    {
        retval = PayloadEncrypt( macMsg->FRMPayload, macMsg->FRMPayloadSize, payloadDecryptionKeyID, macMsg->FHDR.DevAddr, UPLINK, fCntUp );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

#if( USE_LRWAN_1_1_X_CRYPTO == 1 )
        if( CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor == 1 )
        {
            // Encrypt FOpts
            retval = FOptsEncrypt( macMsg->FHDR.FCtrl.Bits.FOptsLen, macMsg->FHDR.DevAddr, UPLINK, FCNT_UP, fCntUp, macMsg->FHDR.FOpts );
            if( retval != LORAMAC_CRYPTO_SUCCESS )
            {
                return retval;
            }
        }
#endif
    }
    CryptoCtx.NvmCtx->FCntList.FCntUp = fCntUp;
    CryptoCtx.EventCryptoNvmCtxChanged( );

    // Serialize message
    if( LoRaMacSerializerData( macMsg ) != LORAMAC_SERIALIZER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SERIALIZER;
    }

    // Compute mic
#if( USE_LRWAN_1_1_X_CRYPTO == 1 )
    if( CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor == 1 )
    {
        uint32_t cmacS = 0;
        uint32_t cmacF = 0;

        // cmacS  = aes128_cmac(SNwkSIntKey, B1 | msg)
        retval = ComputeCmacB1( macMsg->Buffer, ( macMsg->BufSize - LORAMAC_MIC_FIELD_SIZE ), S_NWK_S_INT_KEY, macMsg->FHDR.FCtrl.Bits.Ack, txDr, txCh, macMsg->FHDR.DevAddr, fCntUp, &cmacS );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }
        //cmacF = aes128_cmac(FNwkSIntKey, B0 | msg)
        retval = ComputeCmacB0( macMsg->Buffer, ( macMsg->BufSize - LORAMAC_MIC_FIELD_SIZE ), F_NWK_S_INT_KEY, macMsg->FHDR.FCtrl.Bits.Ack, UPLINK, macMsg->FHDR.DevAddr, fCntUp, &cmacF );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }
        // MIC = cmacS[0..1] | cmacF[0..1]
        macMsg->MIC = ( ( cmacF << 16 ) & 0xFFFF0000 ) | ( cmacS & 0x0000FFFF );
    }
    else
#endif
    {
        // MIC = cmacF[0..3]
        // The IsAck parameter is every time false since the ConfFCnt field is not used in legacy mode.
        retval = ComputeCmacB0( macMsg->Buffer, ( macMsg->BufSize - LORAMAC_MIC_FIELD_SIZE ), NWK_S_ENC_KEY, false, UPLINK, macMsg->FHDR.DevAddr, fCntUp, &macMsg->MIC );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }
    }

    // Re-serialize message to add the MIC
    if( LoRaMacSerializerData( macMsg ) != LORAMAC_SERIALIZER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SERIALIZER;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoUnsecureMessage( AddressIdentifier_t addrID, uint32_t address, FCntIdentifier_t fCntID, uint32_t fCntDown, LoRaMacMessageData_t* macMsg )
{
    if( macMsg == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    if( CheckFCntDown( fCntID, fCntDown ) == false )
    {
        return LORAMAC_CRYPTO_FAIL_FCNT_SMALLER;
    }

    LoRaMacCryptoStatus_t retval = LORAMAC_CRYPTO_ERROR;
    KeyIdentifier_t payloadDecryptionKeyID = APP_S_KEY;
    KeyIdentifier_t micComputationKeyID = S_NWK_S_INT_KEY;
    KeyAddr_t* curItem;

    // Parse the message
    if( LoRaMacParserData( macMsg ) != LORAMAC_PARSER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_PARSER;
    }

    // Determine current security context
    retval = GetKeyAddrItem( addrID, &curItem );
    if( retval != LORAMAC_CRYPTO_SUCCESS )
    {
        return retval;
    }

    payloadDecryptionKeyID = curItem->AppSkey;
    micComputationKeyID = curItem->NwkSkey;

    // Check if it is our address
    if( address != macMsg->FHDR.DevAddr )
    {
        return LORAMAC_CRYPTO_FAIL_ADDRESS;
    }

    // Compute mic
    bool isAck = macMsg->FHDR.FCtrl.Bits.Ack;
    if( CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor == 0 )
    {
        // In legacy mode the IsAck parameter is forced to be false since the ConfFCnt field is not used.
        isAck = false;
    }

    // Verify mic
    retval = VerifyCmacB0( macMsg->Buffer, ( macMsg->BufSize - LORAMAC_MIC_FIELD_SIZE ), micComputationKeyID, isAck, DOWNLINK, address, fCntDown, macMsg->MIC );
    if( retval != LORAMAC_CRYPTO_SUCCESS )
    {
        return retval;
    }

    // Decrypt payload
    if( macMsg->FPort == 0 )
    {
        // Use network session encryption key
        payloadDecryptionKeyID = NWK_S_ENC_KEY;
    }
    retval = PayloadEncrypt( macMsg->FRMPayload, macMsg->FRMPayloadSize, payloadDecryptionKeyID, address, DOWNLINK, fCntDown );
    if( retval != LORAMAC_CRYPTO_SUCCESS )
    {
        return retval;
    }

#if( USE_LRWAN_1_1_X_CRYPTO == 1 )
    if( CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor == 1 )
    {
        // Decrypt FOpts
        retval = FOptsEncrypt( macMsg->FHDR.FCtrl.Bits.FOptsLen, address, DOWNLINK, fCntID, fCntDown, macMsg->FHDR.FOpts );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }
    }
#endif

    UpdateFCntDown( fCntID, fCntDown );

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoDeriveMcRootKey( KeyIdentifier_t keyID )
{
    // Prevent other keys than GenAppKey for LoRaWAN 1.0.x or AppKey for LoRaWAN 1.1 or later
    if( ( ( keyID == APP_KEY ) && ( CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor == 0 ) ) ||
        ( ( keyID == GEN_APP_KEY ) && ( CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor == 1 ) ) )
    {
        return LORAMAC_CRYPTO_ERROR_INVALID_KEY_ID;
    }
    uint8_t compBase[16] = { 0 };

    if( CryptoCtx.NvmCtx->LrWanVersion.Fields.Minor == 1 )
    {
        compBase[0] = 0x20;
    }
    if( SecureElementDeriveAndStoreKey( CryptoCtx.NvmCtx->LrWanVersion, compBase, keyID, MC_ROOT_KEY ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoDeriveMcKEKey( KeyIdentifier_t keyID )
{
    // Prevent other keys than McRootKey
    if( keyID != MC_ROOT_KEY )
    {
        return LORAMAC_CRYPTO_ERROR_INVALID_KEY_ID;
    }
    uint8_t compBase[16] = { 0 };

    if( SecureElementDeriveAndStoreKey( CryptoCtx.NvmCtx->LrWanVersion, compBase, keyID, MC_KE_KEY ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoDeriveMcSessionKeyPair( AddressIdentifier_t addrID, uint32_t mcAddr )
{
    if( mcAddr == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    LoRaMacCryptoStatus_t retval = LORAMAC_CRYPTO_ERROR;

    // Determine current security context
    KeyAddr_t* curItem;
    retval = GetKeyAddrItem( addrID, &curItem );
    if( retval != LORAMAC_CRYPTO_SUCCESS )
    {
        return retval;
    }

    //McAppSKey = aes128_encrypt(McKey, 0x01 | McAddr | pad16)
    //McNwkSKey = aes128_encrypt(McKey, 0x02 | McAddr | pad16)

    uint8_t compBaseAppS[16] = { 0 };
    uint8_t compBaseNwkS[16] = { 0 };

    compBaseAppS[0] = 0x01;
    compBaseAppS[1] = mcAddr & 0xFF;
    compBaseAppS[2] = ( mcAddr >> 8 ) & 0xFF;
    compBaseAppS[3] = ( mcAddr >> 16 ) & 0xFF;
    compBaseAppS[4] = ( mcAddr >> 24 ) & 0xFF;

    compBaseNwkS[0] = 0x02;
    compBaseNwkS[1] = mcAddr & 0xFF;
    compBaseNwkS[2] = ( mcAddr >> 8 ) & 0xFF;
    compBaseNwkS[3] = ( mcAddr >> 16 ) & 0xFF;
    compBaseNwkS[4] = ( mcAddr >> 24 ) & 0xFF;

    if( SecureElementDeriveAndStoreKey( CryptoCtx.NvmCtx->LrWanVersion, compBaseAppS, curItem->RootKey, curItem->AppSkey ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    if( SecureElementDeriveAndStoreKey( CryptoCtx.NvmCtx->LrWanVersion, compBaseNwkS, curItem->RootKey, curItem->NwkSkey ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}
