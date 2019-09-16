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

Description: LoRa MAC layer cryptographic functionality implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis ( Semtech ), Gregory Cristian ( Semtech ),
            Daniel Jaeckle ( STACKFORCE ),  Johannes Bruder ( STACKFORCE )
*/

#include <stdint.h>

#include "utilities.h"
#include "eeprom.h"
#include "nvmm.h"

#define NVMM_MAGIC_NUMBER                   0xA23

typedef struct sDataBlockHeader
{
    /*
     * Checksum
     */
    size_t CSum;
    /*
     * Size of current data block
     */
    size_t Num;
} DataBlockHeader_t;

static uint16_t DataBlockAdrCnt = sizeof( DataBlockHeader_t );

static uint32_t ComputeChecksum( uint8_t* data, uint16_t size )
{
    uint32_t checksum = NVMM_MAGIC_NUMBER; // Start with a magic number

    for( uint16_t i = 0; i < size; i++ )
    {
        checksum += data[i];
    }
    return checksum;
}

static uint32_t ComputeChecksumNvm( uint16_t addr, uint16_t size )
{
    uint32_t checksum = NVMM_MAGIC_NUMBER; // Start with a magic number
    uint8_t data = 0;

    for( uint16_t i = 0; i < size; i++ )
    {
        EepromReadBuffer( addr + i, &data, 1 );
        checksum += data;
    }
    return checksum;
}

/*
 * API functions
 */

NvmmStatus_t NvmmDeclare( NvmmDataBlock_t* dataB, size_t num )
{
    NvmmStatus_t retval = NVMM_ERROR;

    // Increment the internal data block address
    dataB->virtualAddr = DataBlockAdrCnt;

    if( NvmmVerify( dataB, num ) == NVMM_SUCCESS )
    {
        retval = NVMM_SUCCESS;
    }

    // If it is the first time or memory was corrupted
    else
    {
        DataBlockHeader_t dataBHdr;
        dataBHdr.CSum = 0;
        dataBHdr.Num = num;
        EepromWriteBuffer( ( dataB->virtualAddr - sizeof( DataBlockHeader_t ) ), ( uint8_t* ) &dataBHdr, sizeof( DataBlockHeader_t ) );

        retval = NVMM_FAIL_CHECKSUM;
    }

    DataBlockAdrCnt = DataBlockAdrCnt + num + sizeof( DataBlockHeader_t );

    return retval;
}

NvmmStatus_t NvmmVerify( NvmmDataBlock_t* dataB, size_t num )
{
    DataBlockHeader_t dataBHdr;

    // Read the data block header to obtain the size of data block
    EepromReadBuffer( ( dataB->virtualAddr - sizeof( DataBlockHeader_t ) ), ( uint8_t* ) &dataBHdr, sizeof( DataBlockHeader_t ) );

    // Catch already a mismatch of sizes
    if( num != dataBHdr.Num )
    {
        return NVMM_FAIL_CHECKSUM;
    }

    if( ComputeChecksumNvm( dataB->virtualAddr, dataBHdr.Num ) == dataBHdr.CSum )
    {
        return NVMM_SUCCESS;
    }
    else
    {
        return NVMM_FAIL_CHECKSUM;
    }
}


NvmmStatus_t NvmmWrite( NvmmDataBlock_t* dataB, void* src, size_t num )
{
    CRITICAL_SECTION_BEGIN( );

    DataBlockHeader_t dataBHdr;

    // Read the data block header to obtain the maximum allowed size to write
    EepromReadBuffer( ( dataB->virtualAddr - sizeof( DataBlockHeader_t ) ), ( uint8_t* ) &dataBHdr, sizeof( dataBHdr ) );

    if( num > dataBHdr.Num )
    {
        CRITICAL_SECTION_END( );
        return NVMM_ERROR_SIZE;
    }

    dataBHdr.CSum = ComputeChecksum( ( uint8_t* ) src, num );

    // Update data block header
    EepromWriteBuffer( ( dataB->virtualAddr - sizeof( DataBlockHeader_t ) ), ( uint8_t* ) &dataBHdr, sizeof( DataBlockHeader_t ) );

    // Write data block
    EepromWriteBuffer( dataB->virtualAddr, ( uint8_t* ) src, num );

    CRITICAL_SECTION_END( );

    return NVMM_SUCCESS;
}

NvmmStatus_t NvmmRead( NvmmDataBlock_t* dataB, void* dst, size_t num )
{
    CRITICAL_SECTION_BEGIN( );

    DataBlockHeader_t dataBHdr;

    // Read the data block header to obtain the maximum allowed size to write
    EepromReadBuffer( (dataB->virtualAddr - sizeof( DataBlockHeader_t ) ), ( uint8_t * ) &dataBHdr, sizeof( dataBHdr ) );

    if( num > dataBHdr.Num )
    {
        CRITICAL_SECTION_END( );
        return NVMM_ERROR_SIZE;
    }

    //  data block
    EepromReadBuffer( dataB->virtualAddr, ( uint8_t* ) dst, num );

    CRITICAL_SECTION_END( );

    return NVMM_SUCCESS;
}
