/*!
 * \file      NvmCtxMgmt.c
 *
 * \brief     NVM context management implementation
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

#include <stdio.h>
#include "NvmCtxMgmt.h"
#include "utilities.h"
#include "eeprom.h"
#include "nvmm.h"

/*!
 * Enables/Disables the context storage management storage at all. Must be enabled for LoRaWAN 1.1.x.
 * WARNING: Still under development and not tested yet.
 */
#define CONTEXT_MANAGEMENT_ENABLED         0

/*!
 * Enables/Disables maximum persistent context storage management. All module contexts will be saved on a non-volatile memory.
 * WARNING: Still under development and not tested yet.
 */
#define MAX_PERSISTENT_CTX_MGMT_ENABLED    0

#if ( MAX_PERSISTENT_CTX_MGMT_ENABLED == 1 )
#define NVM_CTX_STORAGE_MASK               0xFF
#else
#define NVM_CTX_STORAGE_MASK               0x8C
#endif

#if ( CONTEXT_MANAGEMENT_ENABLED == 1 )
/*!
 * LoRaMAC Structure holding contexts changed status
 * in case of a \ref MLME_NVM_CTXS_UPDATE indication.
 */
typedef union uLoRaMacCtxsUpdateInfo
{
    /*!
     * Byte-access to the bits
     */
    uint8_t Value;
    /*!
     * The according context bit will be set to one
     * if the context changed or 0 otherwise.
     */
    struct sElements
    {
        /*!
         * Mac core nvm context
         */
        uint8_t Mac : 1;
        /*!
         * Region module nvm contexts
         */
        uint8_t Region : 1;
        /*!
         * Cryto module context
         */
        uint8_t Crypto : 1;
        /*!
         * Secure Element driver context
         */
        uint8_t SecureElement : 1;
        /*!
         * MAC commands module context
         */
        uint8_t Commands : 1;
        /*!
         * Class B module context
         */
        uint8_t ClassB : 1;
        /*!
         * Confirm queue module context
         */
        uint8_t ConfirmQueue : 1;
        /*!
         * FCnt Handler module context
         */
        uint8_t FCntHandlerNvmCtx : 1;
    }Elements;
}LoRaMacCtxUpdateStatus_t;

LoRaMacCtxUpdateStatus_t CtxUpdateStatus = { .Value = 0 };

/*
 * Nvmm handles
 */
static NvmmDataBlock_t FCntHandlerNvmCtxDataBlock;
static NvmmDataBlock_t SecureElementNvmCtxDataBlock;
static NvmmDataBlock_t CryptoNvmCtxDataBlock;
#if ( MAX_PERSISTENT_CTX_MGMT_ENABLED == 1 )
static NvmmDataBlock_t MacNvmCtxDataBlock;
static NvmmDataBlock_t RegionNvmCtxDataBlock;
static NvmmDataBlock_t CommandsNvmCtxDataBlock;
static NvmmDataBlock_t ConfirmQueueNvmCtxDataBlock;
static NvmmDataBlock_t ClassBNvmCtxDataBlock;
#endif
#endif

void NvmCtxMgmtEvent( LoRaMacNvmCtxModule_t module )
{
#if ( CONTEXT_MANAGEMENT_ENABLED == 1 )
    switch( module )
    {
        case LORAMAC_NVMCTXMODULE_MAC:
        {
            CtxUpdateStatus.Elements.Mac = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_REGION:
        {
            CtxUpdateStatus.Elements.Region = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_CRYPTO:
        {
            CtxUpdateStatus.Elements.Crypto = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_SECURE_ELEMENT:
        {
            CtxUpdateStatus.Elements.SecureElement = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_COMMANDS:
        {
            CtxUpdateStatus.Elements.Commands = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_CLASS_B:
        {
            CtxUpdateStatus.Elements.ClassB = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_CONFIRM_QUEUE:
        {
            CtxUpdateStatus.Elements.ConfirmQueue = 1;
            break;
        }
        case LORAMAC_NVMCTXMODULE_FCNT_HANDLER:
        {
            CtxUpdateStatus.Elements.FCntHandlerNvmCtx = 1;
            break;
        }
        default:
        {
            break;
        }
    }
#endif
}

NvmCtxMgmtStatus_t NvmCtxMgmtStore( void )
{
#if ( CONTEXT_MANAGEMENT_ENABLED == 1 )
    // Read out the contexts lengths and pointers
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NVM_CTXS;
    LoRaMacMibGetRequestConfirm( &mibReq );
    LoRaMacCtxs_t* MacContexts = mibReq.Param.Contexts;

    // Input checks
    if( ( CtxUpdateStatus.Value & NVM_CTX_STORAGE_MASK ) == 0 )
    {
        return NVMCTXMGMT_STATUS_FAIL;
    }
    if( LoRaMacStop( ) != LORAMAC_STATUS_OK )
    {
        return NVMCTXMGMT_STATUS_FAIL;
    }

    // Write
    if( CtxUpdateStatus.Elements.Crypto == 1 )
    {
        if( NvmmWrite( &CryptoNvmCtxDataBlock, MacContexts->CryptoNvmCtx, MacContexts->CryptoNvmCtxSize ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL;
        }
    }

    if( CtxUpdateStatus.Elements.SecureElement == 1 )
    {
        if( NvmmWrite( &SecureElementNvmCtxDataBlock, MacContexts->SecureElementNvmCtx, MacContexts->SecureElementNvmCtxSize ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL;
        }
    }

    if( CtxUpdateStatus.Elements.FCntHandlerNvmCtx == 1 )
    {
        if( NvmmWrite( &FCntHandlerNvmCtxDataBlock, MacContexts->FCntHandlerNvmCtx, MacContexts->FCntHandlerNvmCtxSize ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL;
        }
    }

#if ( MAX_PERSISTENT_CTX_MGMT_ENABLED == 1 )
    if( CtxUpdateStatus.Elements.Mac == 1 )
    {
        if( NvmmWrite( &MacNvmCtxDataBlock, MacContexts->MacNvmCtx, MacContexts->MacNvmCtxSize ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL;
        }
    }

    if( CtxUpdateStatus.Elements.Region == 1 )
    {
        if( NvmmWrite( &RegionNvmCtxDataBlock, MacContexts->RegionNvmCtx, MacContexts->RegionNvmCtxSize ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL;
        }
    }

    if( CtxUpdateStatus.Elements.Commands == 1 )
    {
        if( NvmmWrite( &CommandsNvmCtxDataBlock, MacContexts->CommandsNvmCtx, MacContexts->CommandsNvmCtxSize ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL;
        }
    }

    if( CtxUpdateStatus.Elements.ClassB == 1 )
    {
        if( NvmmWrite( &ClassBNvmCtxDataBlock, MacContexts->ClassBNvmCtx, MacContexts->ClassBNvmCtxSize ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL;
        }
    }

    if( CtxUpdateStatus.Elements.ConfirmQueue == 1 )
    {
        if( NvmmWrite( &ConfirmQueueNvmCtxDataBlock, MacContexts->ConfirmQueueNvmCtx, MacContexts->ConfirmQueueNvmCtxSize ) != NVMM_SUCCESS )
        {
            return NVMCTXMGMT_STATUS_FAIL;
        }
    }
#endif

    CtxUpdateStatus.Value = 0x00;

    // Resume LoRaMac
    LoRaMacStart( );

    return NVMCTXMGMT_STATUS_SUCCESS;
#else
    return NVMCTXMGMT_STATUS_FAIL;
#endif
}

NvmCtxMgmtStatus_t NvmCtxMgmtRestore( void )
{
#if ( CONTEXT_MANAGEMENT_ENABLED == 1 )
    MibRequestConfirm_t mibReq;
    LoRaMacCtxs_t contexts = { 0 };
    NvmCtxMgmtStatus_t status = NVMCTXMGMT_STATUS_SUCCESS;

    // Read out the contexts lengths
    mibReq.Type = MIB_NVM_CTXS;
    LoRaMacMibGetRequestConfirm( &mibReq );


    uint8_t NvmCryptoCtxRestore[mibReq.Param.Contexts->CryptoNvmCtxSize];
    uint8_t NvmSecureElementCtxRestore[mibReq.Param.Contexts->SecureElementNvmCtxSize];
    uint8_t NvmFCntHandlerCtxRestore[mibReq.Param.Contexts->FCntHandlerNvmCtxSize];
#if ( MAX_PERSISTENT_CTX_MGMT_ENABLED == 1 )
    uint8_t NvmMacCtxRestore[mibReq.Param.Contexts->MacNvmCtxSize];
    uint8_t NvmRegionCtxRestore[mibReq.Param.Contexts->RegionNvmCtxSize];
    uint8_t NvmCommandsCtxRestore[mibReq.Param.Contexts->CommandsNvmCtxSize];
    uint8_t NvmClassBCtxRestore[mibReq.Param.Contexts->ClassBNvmCtxSize];
    uint8_t NvmConfirmQueueCtxRestore[mibReq.Param.Contexts->ConfirmQueueNvmCtxSize];
#endif

    if ( NvmmDeclare( &CryptoNvmCtxDataBlock, mibReq.Param.Contexts->CryptoNvmCtxSize ) == NVMM_SUCCESS )
    {
        NvmmRead( &CryptoNvmCtxDataBlock, NvmCryptoCtxRestore, mibReq.Param.Contexts->CryptoNvmCtxSize );
        contexts.CryptoNvmCtx = &NvmCryptoCtxRestore;
        contexts.CryptoNvmCtxSize = mibReq.Param.Contexts->CryptoNvmCtxSize;
    }
    else
    {
        status = NVMCTXMGMT_STATUS_FAIL;
    }

    if ( NvmmDeclare( &SecureElementNvmCtxDataBlock, mibReq.Param.Contexts->SecureElementNvmCtxSize ) == NVMM_SUCCESS )
    {
        NvmmRead( &SecureElementNvmCtxDataBlock, NvmSecureElementCtxRestore, mibReq.Param.Contexts->SecureElementNvmCtxSize );
        contexts.SecureElementNvmCtx = &NvmSecureElementCtxRestore;
        contexts.SecureElementNvmCtxSize = mibReq.Param.Contexts->SecureElementNvmCtxSize;
    }
    else
    {
        status = NVMCTXMGMT_STATUS_FAIL;
    }

    if ( NvmmDeclare( &FCntHandlerNvmCtxDataBlock, mibReq.Param.Contexts->FCntHandlerNvmCtxSize ) == NVMM_SUCCESS )
    {
        NvmmRead( &FCntHandlerNvmCtxDataBlock, NvmFCntHandlerCtxRestore, mibReq.Param.Contexts->FCntHandlerNvmCtxSize );
        contexts.FCntHandlerNvmCtx = &NvmFCntHandlerCtxRestore;
        contexts.FCntHandlerNvmCtxSize = mibReq.Param.Contexts->FCntHandlerNvmCtxSize;
    }
    else
    {
        status = NVMCTXMGMT_STATUS_FAIL;
    }

#if ( MAX_PERSISTENT_CTX_MGMT_ENABLED == 1 )
    if( NvmmDeclare( &MacNvmCtxDataBlock, mibReq.Param.Contexts->MacNvmCtxSize ) == NVMM_SUCCESS )
    {
        NvmmRead( &MacNvmCtxDataBlock, NvmMacCtxRestore, mibReq.Param.Contexts->MacNvmCtxSize );
        contexts.MacNvmCtx = &NvmMacCtxRestore;
        contexts.MacNvmCtxSize = mibReq.Param.Contexts->MacNvmCtxSize;
    }
    else
    {
        status = NVMCTXMGMT_STATUS_FAIL;
    }

    if ( NvmmDeclare( &RegionNvmCtxDataBlock, mibReq.Param.Contexts->RegionNvmCtxSize ) == NVMM_SUCCESS )
    {
        NvmmRead( &RegionNvmCtxDataBlock, NvmRegionCtxRestore, mibReq.Param.Contexts->RegionNvmCtxSize );
        contexts.RegionNvmCtx = &NvmRegionCtxRestore;
        contexts.RegionNvmCtxSize = mibReq.Param.Contexts->RegionNvmCtxSize;
    }
    else
    {
        status = NVMCTXMGMT_STATUS_FAIL;
    }

    if ( NvmmDeclare( &CommandsNvmCtxDataBlock, mibReq.Param.Contexts->CommandsNvmCtxSize ) == NVMM_SUCCESS )
    {
        NvmmRead( &CommandsNvmCtxDataBlock, NvmCommandsCtxRestore, mibReq.Param.Contexts->CommandsNvmCtxSize );
        contexts.CommandsNvmCtx = &NvmCommandsCtxRestore;
        contexts.CommandsNvmCtxSize = mibReq.Param.Contexts->CommandsNvmCtxSize;
    }
    else
    {
        status = NVMCTXMGMT_STATUS_FAIL;
    }

    if ( NvmmDeclare( &ClassBNvmCtxDataBlock, mibReq.Param.Contexts->ClassBNvmCtxSize ) == NVMM_SUCCESS )
    {
        NvmmRead( &ClassBNvmCtxDataBlock, NvmClassBCtxRestore, mibReq.Param.Contexts->ClassBNvmCtxSize );
        contexts.ClassBNvmCtx = &NvmClassBCtxRestore;
        contexts.ClassBNvmCtxSize = mibReq.Param.Contexts->ClassBNvmCtxSize;
    }
    else
    {
        status = NVMCTXMGMT_STATUS_FAIL;
    }

    if ( NvmmDeclare( &ConfirmQueueNvmCtxDataBlock, mibReq.Param.Contexts->ConfirmQueueNvmCtxSize ) == NVMM_SUCCESS )
    {
        NvmmRead( &ConfirmQueueNvmCtxDataBlock, NvmConfirmQueueCtxRestore, mibReq.Param.Contexts->ConfirmQueueNvmCtxSize );
        contexts.ConfirmQueueNvmCtx = &NvmConfirmQueueCtxRestore;
        contexts.ConfirmQueueNvmCtxSize = mibReq.Param.Contexts->ConfirmQueueNvmCtxSize;
    }
    else
    {
        status = NVMCTXMGMT_STATUS_FAIL;
    }
#endif

    // Enforce storing all contexts
    if( status == NVMCTXMGMT_STATUS_FAIL )
    {
        CtxUpdateStatus.Value = 0xFF;
        NvmCtxMgmtStore( );
    }
    else
    {  // If successful query the mac to restore contexts
        mibReq.Type = MIB_NVM_CTXS;
        mibReq.Param.Contexts = &contexts;
        LoRaMacMibSetRequestConfirm( &mibReq );
    }

    return status;
#else
    return NVMCTXMGMT_STATUS_FAIL;
#endif
}
