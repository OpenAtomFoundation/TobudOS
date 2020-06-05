/*!
 * \file      FragDecoder.c
 *
 * \brief     Implements the LoRa-Alliance fragmentation decoder
 *            Specification: https://lora-alliance.org/sites/default/files/2018-09/fragmented_data_block_transport_v1.0.0.pdf
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
 * \author    Fabien Holin ( Semtech )
 * \author    Miguel Luis ( Semtech )
 */
#include <stddef.h>
#include <stdbool.h>
#include "utilities.h"
#include "FragDecoder.h"

#define DBG_TRACE                                   0

#if DBG_TRACE == 1
    #include <stdio.h>
    /*!
     * Works in the same way as the printf function does.
     */
    #define DBG( ... )                               \
        do                                           \
        {                                            \
            printf( __VA_ARGS__ );                   \
        }while( 0 )
#else
    #define DBG( fmt, ... )
#endif


/*
 *=============================================================================
 * Fragmentation decoder algorithm utilities
 *=============================================================================
 */

typedef struct
{
#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
    FragDecoderCallbacks_t *Callbacks;
#else
    uint8_t *File;
    uint32_t FileSize;
#endif
    uint16_t FragNb;
    uint8_t FragSize;

    uint32_t M2BLine;
    uint8_t MatrixM2B[( ( FRAG_MAX_REDUNDANCY >> 3 ) + 1 ) * FRAG_MAX_REDUNDANCY];
    uint16_t FragNbMissingIndex[FRAG_MAX_NB];

    uint8_t S[( FRAG_MAX_REDUNDANCY >> 3 ) + 1];

    FragDecoderStatus_t Status;
}FragDecoder_t;

#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
/*!
 * \brief Sets a row from source into file destination
 *
 * \param [IN] src  Source buffer pointer
 * \param [IN] row  Destination index of the row to be copied
 * \param [IN] size Source number of bytes to be copied
 */
static void SetRow( uint8_t *src, uint16_t row, uint16_t size );
#else
/*!
 * \brief Sets a row from source into destination
 *
 * \param [IN] dst  Destination buffer pointer
 * \param [IN] src  Source buffer pointer
 * \param [IN] row  Destination index of the row to be copied
 * \param [IN] size Source number of bytes to be copied
 */
static void SetRow( uint8_t *dst, uint8_t *src, uint16_t row, uint16_t size );
#endif

#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
/*!
 * \brief Gets a row from source and stores it into file destination
 *
 * \param [IN] src  Source buffer pointer
 * \param [IN] row  Source index of the row to be copied
 * \param [IN] size Source number of bytes to be copied
 */
static void GetRow( uint8_t *src, uint16_t row, uint16_t size );
#else
/*!
 * \brief Gets a row from source and stores it into destination
 *
 * \param [IN] dst  Destination buffer pointer
 * \param [IN] src  Source buffer pointer
 * \param [IN] row  Source index of the row to be copied
 * \param [IN] size Source number of bytes to be copied
 */
static void GetRow( uint8_t *dst, uint8_t *src, uint16_t row, uint16_t size );
#endif

/*!
 * \brief Gets the parity value from a given row of the parity matrix
 *
 * \param [IN] index      The index of the row to be computed
 * \param [IN] matrixRow  Pointer to the parity matrix (parity bit array)
 *
 * \retval parity         Parity value at the given index
 */
static uint8_t GetParity( uint16_t index, uint8_t *matrixRow  );

/*!
 * \brief Sets the parity value on the given row of the parity matrix
 *
 * \param [IN]     index     The index of the row to be computed
 * \param [IN/OUT] matrixRow Pointer to the parity matrix.
 * \param [IN]     parity    The parity value to be set in the parity matrix
 */
static void SetParity( uint16_t index, uint8_t *matrixRow, uint8_t parity );

/*!
 * \brief Check if the provided value is a power of 2
 *
 * \param [IN] x  Value to be tested
 *
 * \retval status Return true if frame is a power of two
 */
static bool IsPowerOfTwo( uint32_t x );

/*!
 * \brief XOrs two data lines
 *
 * \param [IN]  line1  1st Data line to be XORed
 * \param [IN]  line2  2nd Data line to be XORed
 * \param [IN]  size   Number of elements in line1
 *
 * \param [OUT] result XOR( line1, line2 ) result stored in line1
 */
static void XorDataLine( uint8_t *line1, uint8_t *line2, int32_t size );

/*!
 * \brief XORs two parity lines
 *
 * \param [IN]  line1  1st Parity line to be XORed
 * \param [IN]  line2  2nd Parity line to be XORed
 * \param [IN]  size   Number of elements in line1
 *
 * \param [OUT] result XOR( line1, line2 ) result stored in line1
 */
static void XorParityLine( uint8_t* line1, uint8_t* line2, int32_t size );

/*!
 * \brief Generates a pseudo random number : PRBS23
 *
 * \param [IN] value The input of the PRBS23 generator
 *
 * \retval nextValue Returns the next pseudo random number
 */
static int32_t FragPrbs23( int32_t value );

/*!
 * \brief Gets and fills the parity matrix
 *
 * \param [IN]  n         Fragment N
 * \param [IN]  m         Fragment number
 * \param [OUT] matrixRow Parity matrix
 */
static void FragGetParityMatrixRow( int32_t n, int32_t m, uint8_t *matrixRow );

/*!
 * \brief Finds the index of the first one in a bit array
 *
 * \param [IN] bitArray Pointer to the bit array
 * \param [IN] size     Bit array size
 * \retval index        The index of the first 1 in the bit array
 */
static uint16_t BitArrayFindFirstOne( uint8_t *bitArray, uint16_t size );

/*!
 * \brief Checks if the provided bit array only contains zeros
 *
 * \param [IN] bitArray Pointer to the bit array
 * \param [IN] size     Bit array size
 * \retval isAllZeros   [0: Contains ones, 1: Contains all zeros]
 */
static uint8_t BitArrayIsAllZeros( uint8_t *bitArray, uint16_t  size );

/*!
 * \brief Finds & marks missing fragments
 *
 * \param [IN]  counter Current fragment counter
 * \param [OUT] FragDecoder.FragNbMissingIndex[] array is updated in place
 */
static void FragFindMissingFrags( uint16_t counter );

/*!
 * \brief Finds the index (frag counter) of the x th missing frag
 *
 * \param [IN] x   x th missing frag
 *
 * \retval counter The counter value associated to the x th missing frag
 */
static uint16_t FragFindMissingIndex( uint16_t x );

/*!
 * \brief Extacts a row from the binary matrix and expands it to a bitArray
 *
 * \param [IN] bitArray  Pointer to the bit array
 * \param [IN] rowIndex  Matrix row index
 * \param [IN] bitsInRow Number of bits in one row
 */
static void FragExtractLineFromBinaryMatrix( uint8_t* bitArray, uint16_t rowIndex, uint16_t bitsInRow );

/*!
 * \brief Collapses and Pushs a row of a bit array to the matrix
 *
 * \param [IN] bitArray  Pointer to the bit array
 * \param [IN] rowIndex  Matrix row index
 * \param [IN] bitsInRow Number of bits in one row
 */
static void FragPushLineToBinaryMatrix( uint8_t *bitArray, uint16_t rowIndex, uint16_t bitsInRow );

/*
 *=============================================================================
 * Fragmentation decoder algorithm
 *=============================================================================
 */

static FragDecoder_t FragDecoder;

#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
void FragDecoderInit( uint16_t fragNb, uint8_t fragSize, FragDecoderCallbacks_t *callbacks )
#else
void FragDecoderInit( uint16_t fragNb, uint8_t fragSize, uint8_t *file, uint32_t fileSize )
#endif
{
#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
    FragDecoder.Callbacks = callbacks;
#else
    FragDecoder.File = file;
    FragDecoder.FileSize = fileSize;
#endif
    FragDecoder.FragNb = fragNb;                                // FragNb = FRAG_MAX_SIZE
    FragDecoder.FragSize = fragSize;                            // number of byte on a row
    FragDecoder.Status.FragNbLastRx = 0;
    FragDecoder.Status.FragNbLost = 0;
    FragDecoder.M2BLine = 0;

    // Initialize missing fragments index array
    for( uint16_t i = 0; i < FRAG_MAX_NB; i++ )
    {
        FragDecoder.FragNbMissingIndex[i] = 1;
    }

    // Initialize parity matrix
    for( uint32_t i = 0; i < ( ( FRAG_MAX_REDUNDANCY >> 3 ) + 1 ); i++ )
    {
        FragDecoder.S[i] = 0;
    }

    for( uint32_t i = 0; i < ( ( ( FRAG_MAX_REDUNDANCY >> 3 ) + 1 ) * FRAG_MAX_REDUNDANCY ); i++ )
    {
       FragDecoder.MatrixM2B[i] = 0xFF;
    }
    
    // Initialize final uncoded data buffer ( FRAG_MAX_NB * FRAG_MAX_SIZE )
    for( uint32_t i = 0; i < ( fragNb * fragSize ); i++ )
    {
#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
        if( ( FragDecoder.Callbacks != NULL ) && ( FragDecoder.Callbacks->FragDecoderWrite != NULL ) )
        {
            FragDecoder.Callbacks->FragDecoderWrite( i, ( uint8_t[] ){ 0xFF }, 1 );
        }
#else
        FragDecoder.File[i] = 0xFF;
#endif
    }
    FragDecoder.Status.FragNbLost = 0;
    FragDecoder.Status.FragNbLastRx = 0;
}

#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
uint32_t FragDecoderGetMaxFileSize( void )
{
    return FRAG_MAX_NB * FRAG_MAX_SIZE;
}
#endif

int32_t FragDecoderProcess( uint16_t fragCounter, uint8_t *rawData )
{
    uint16_t firstOneInRow = 0;
    int32_t first = 0;
    int32_t noInfo = 0;

    uint8_t matrixRow[(FRAG_MAX_NB >> 3 ) + 1];
    uint8_t matrixDataTemp[FRAG_MAX_SIZE];
    uint8_t dataTempVector[( FRAG_MAX_REDUNDANCY >> 3 ) + 1];
    uint8_t dataTempVector2[( FRAG_MAX_REDUNDANCY >> 3 ) + 1];

    memset1( matrixRow, 0, ( FRAG_MAX_NB >> 3 ) + 1 );
    memset1( matrixDataTemp, 0, FRAG_MAX_SIZE );
    memset1( dataTempVector, 0, ( FRAG_MAX_REDUNDANCY >> 3 ) + 1 );
    memset1( dataTempVector2, 0, ( FRAG_MAX_REDUNDANCY >> 3 ) + 1 );

    FragDecoder.Status.FragNbRx = fragCounter;

    if( fragCounter < FragDecoder.Status.FragNbLastRx )
    {
        return FRAG_SESSION_ONGOING;  // Drop frame out of order
    }

    // The M (FragNb) first packets aren't encoded or in other words they are
    // encoded with the unitary matrix
    if( fragCounter < ( FragDecoder.FragNb + 1 ) )
    {
        // The M first frame are not encoded store them
#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
        SetRow( rawData, fragCounter - 1, FragDecoder.FragSize );
#else
        SetRow( FragDecoder.File, rawData, fragCounter - 1, FragDecoder.FragSize );
#endif

        FragDecoder.FragNbMissingIndex[fragCounter - 1] = 0;

        // Update the FragDecoder.FragNbMissingIndex with the loosing frame
        FragFindMissingFrags( fragCounter );
    }
    else
    {
        if( FragDecoder.Status.FragNbLost > FRAG_MAX_REDUNDANCY )
        {
           FragDecoder.Status.MatrixError = 1;
           return FRAG_SESSION_FINISHED;
        }
        // At this point we receive encoded frames and the number of loosing frames
        // is well known: FragDecoder.FragNbLost - 1;

        // In case of the end of true data is missing
        FragFindMissingFrags( fragCounter );

        if( FragDecoder.Status.FragNbLost == 0 )
        { 
            // the case : all the M(FragNb) first rows have been transmitted with no error
            return FragDecoder.Status.FragNbLost;
        }

        // fragCounter - FragDecoder.FragNb
        FragGetParityMatrixRow( fragCounter - FragDecoder.FragNb, FragDecoder.FragNb, matrixRow );

        for( int32_t i = 0; i < FragDecoder.FragNb; i++ )
        {
            if( GetParity( i , matrixRow ) == 1 )
            {
                if( FragDecoder.FragNbMissingIndex[i] == 0 )
                {
                    // XOR with already receive frag
                    SetParity( i, matrixRow, 0 );
#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
                    GetRow( matrixDataTemp, i, FragDecoder.FragSize );
#else
                    GetRow( matrixDataTemp, FragDecoder.File, i, FragDecoder.FragSize );
#endif
                    XorDataLine( rawData, matrixDataTemp, FragDecoder.FragSize );
                }
                else
                {
                    // Fill the "little" boolean matrix m2b
                    SetParity( FragDecoder.FragNbMissingIndex[i] - 1, dataTempVector, 1 );
                    if( first == 0 )
                    {
                        first = 1;
                    }
                }
            }
        }

        firstOneInRow = BitArrayFindFirstOne( dataTempVector, FragDecoder.Status.FragNbLost );

        if( first > 0 )
        {
            int32_t li;
            int32_t lj;

            // Manage a new line in MatrixM2B
            while( GetParity( firstOneInRow, FragDecoder.S ) == 1 )
            { 
                // Row already diagonalized exist & ( FragDecoder.MatrixM2B[firstOneInRow][0] )
                FragExtractLineFromBinaryMatrix( dataTempVector2, firstOneInRow, FragDecoder.Status.FragNbLost );
                XorParityLine( dataTempVector, dataTempVector2, FragDecoder.Status.FragNbLost );
                // Have to store it in the mi th position of the missing frag
                li = FragFindMissingIndex( firstOneInRow );
#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
                GetRow( matrixDataTemp, li, FragDecoder.FragSize );
#else
                GetRow( matrixDataTemp, FragDecoder.File, li, FragDecoder.FragSize );
#endif
                XorDataLine( rawData, matrixDataTemp, FragDecoder.FragSize );
                if( BitArrayIsAllZeros( dataTempVector, FragDecoder.Status.FragNbLost ) )
                {
                    noInfo = 1;
                    break;
                }
                firstOneInRow = BitArrayFindFirstOne( dataTempVector, FragDecoder.Status.FragNbLost );
            }

            if( noInfo == 0 )
            {
                FragPushLineToBinaryMatrix( dataTempVector, firstOneInRow, FragDecoder.Status.FragNbLost );
                li = FragFindMissingIndex( firstOneInRow );
#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
                SetRow( rawData, li, FragDecoder.FragSize );
#else
                SetRow( FragDecoder.File, rawData, li, FragDecoder.FragSize );
#endif
                SetParity( firstOneInRow, FragDecoder.S, 1 );
                FragDecoder.M2BLine++;
            }

            if( FragDecoder.M2BLine == FragDecoder.Status.FragNbLost )
            { 
                // Then last step diagonalized
                if( FragDecoder.Status.FragNbLost > 1 )
                {
                    int32_t i, j;

                    for( i = ( FragDecoder.Status.FragNbLost - 2 ); i >= 0 ; i-- )
                    {
                        li = FragFindMissingIndex( i );
#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
                        GetRow( matrixDataTemp, li, FragDecoder.FragSize );
#else
                        GetRow( matrixDataTemp, FragDecoder.File, li, FragDecoder.FragSize );
#endif
                        for( j = ( FragDecoder.Status.FragNbLost - 1 ); j > i; j--)
                        {
                            FragExtractLineFromBinaryMatrix( dataTempVector2, i, FragDecoder.Status.FragNbLost );
                            FragExtractLineFromBinaryMatrix( dataTempVector, j, FragDecoder.Status.FragNbLost );
                            if( GetParity( j, dataTempVector2 ) == 1 )
                            {
                                XorParityLine( dataTempVector2, dataTempVector, FragDecoder.Status.FragNbLost );

                                lj = FragFindMissingIndex( j );

#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
                                GetRow( rawData, lj, FragDecoder.FragSize );
#else
                                GetRow( rawData, FragDecoder.File, lj, FragDecoder.FragSize );
#endif
                                XorDataLine( matrixDataTemp , rawData , FragDecoder.FragSize );
                            }
                        }
#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
                        SetRow( matrixDataTemp, li, FragDecoder.FragSize );
#else
                        SetRow( FragDecoder.File, matrixDataTemp, li, FragDecoder.FragSize );
#endif
                    }
                    return FragDecoder.Status.FragNbLost;
                }
                else
                { 
                    //If not ( FragDecoder.FragNbLost > 1 )
                    return FragDecoder.Status.FragNbLost;
                }
            }
        }
    }
    return FRAG_SESSION_ONGOING;
}

FragDecoderStatus_t FragDecoderGetStatus( void )
{ 
    return FragDecoder.Status;
}

/*
 *=============================================================================
 * Fragmentation decoder algorithm utilities
 *=============================================================================
 */

#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
static void SetRow( uint8_t *src, uint16_t row, uint16_t size )
{
    if( ( FragDecoder.Callbacks != NULL ) && ( FragDecoder.Callbacks->FragDecoderWrite != NULL ) )
    {
        FragDecoder.Callbacks->FragDecoderWrite( row * size, src, size );
    }
}

static void GetRow( uint8_t *dst, uint16_t row, uint16_t size )
{
    if( ( FragDecoder.Callbacks != NULL ) && ( FragDecoder.Callbacks->FragDecoderRead != NULL ) )
    {
        FragDecoder.Callbacks->FragDecoderRead( row * size, dst, size );
    }
}
#else
static void SetRow( uint8_t *dst, uint8_t *src, uint16_t row, uint16_t size )
{
    memcpy1( &dst[row * size], src, size );
}

static void GetRow( uint8_t *dst, uint8_t *src, uint16_t row, uint16_t size )
{
    memcpy1( dst, &src[row * size], size );
}
#endif

static uint8_t GetParity( uint16_t index, uint8_t *matrixRow  )
{
    uint8_t parity;
    parity = matrixRow[index >> 3];
    parity = ( parity >> ( 7 - ( index % 8 ) ) ) & 0x01;
    return parity;
}

static void SetParity( uint16_t index, uint8_t *matrixRow, uint8_t parity )
{
    uint8_t mask = 0xFF - ( 1 << ( 7 - ( index % 8 ) ) );
    parity = parity << ( 7 - ( index % 8 ) );
    matrixRow[index >> 3] = ( matrixRow[index >> 3] & mask ) + parity;
}

static bool IsPowerOfTwo( uint32_t x )
{
    uint8_t sumBit = 0;

    for( uint8_t i = 0; i < 32; i++ )
    {
        sumBit += ( x & ( 1 << i ) ) >> i;
    }
    if( sumBit == 1 )
    {
        return true;
    }
    return false;
}

static void XorDataLine( uint8_t *line1, uint8_t *line2, int32_t size )
{
    for( int32_t i = 0; i < size; i++ )
    {
        line1[i] = line1[i] ^ line2[i];
    }
}

static void XorParityLine( uint8_t* line1, uint8_t* line2, int32_t size )
{
    for( int32_t i = 0; i < size; i++ )
    {
        SetParity( i, line1, ( GetParity( i, line1 ) ^ GetParity( i, line2 ) ) );
    }
}

static int32_t FragPrbs23( int32_t value )
{
    int32_t b0 = value & 0x01;
    int32_t b1 = ( value & 0x20 ) >> 5;
    return ( value >> 1 ) + ( ( b0 ^ b1 ) << 22 );;
}

static void FragGetParityMatrixRow( int32_t n, int32_t m, uint8_t *matrixRow )
{
    int32_t mTemp;
    int32_t x;
    int32_t nbCoeff = 0;
    int32_t r;

    if( IsPowerOfTwo( m ) != false )
    {
        mTemp = 1;
    }
    else 
    {
        mTemp = 0;
    }

    x = 1 + ( 1001 * n );
    for( uint8_t i = 0; i < ( ( m >> 3 ) + 1 ); i++ )
    {
        matrixRow[i] = 0;
    }
    while( nbCoeff < ( m >> 1 ) )
    {
        r = 1 << 16;
        while( r >= m )
        {
            x = FragPrbs23( x );
            r = x % ( m + mTemp );
        }
        SetParity( r, matrixRow, 1 );
        nbCoeff += 1;
    }
}

static uint16_t BitArrayFindFirstOne( uint8_t *bitArray, uint16_t size )
{
    for( uint16_t i = 0; i < size; i++)
    {
        if ( GetParity( i, bitArray ) == 1 )
        {
            return i;
        }
    }
    return 0;
}

static uint8_t BitArrayIsAllZeros( uint8_t *bitArray, uint16_t  size )
{
    for( uint16_t i = 0; i < size; i++ )
    {
        if( GetParity( i, bitArray ) == 1 )
        {
            return 0;
        }
    }
    return 1;
}

/*!
 * \brief Finds & marks missing fragments
 *
 * \param [IN]  counter Current fragment counter
 * \param [OUT] FragDecoder.FragNbMissingIndex[] array is updated in place
 */
static void FragFindMissingFrags( uint16_t counter )
{
    int32_t i;
    for( i = FragDecoder.Status.FragNbLastRx; i < ( counter - 1 ); i++ )
    {
        if( i < FragDecoder.FragNb )
        {
            FragDecoder.Status.FragNbLost++;
            FragDecoder.FragNbMissingIndex[i] = FragDecoder.Status.FragNbLost;
        }
    }
    if( i < FragDecoder.FragNb )
    {
        FragDecoder.Status.FragNbLastRx = counter;
    }
    else
    {
        FragDecoder.Status.FragNbLastRx = FragDecoder.FragNb + 1;
    }
    DBG( "RECEIVED    : %5d / %5d Fragments\n", FragDecoder.Status.FragNbRx, FragDecoder.FragNb );
    DBG( "              %5d / %5d Bytes\n", FragDecoder.Status.FragNbRx * FragDecoder.FragSize, FragDecoder.FragNb * FragDecoder.FragSize );
    DBG( "LOST        :       %7d Fragments\n\n", FragDecoder.Status.FragNbLost );
}

/*!
 * \brief Finds the index (frag counter) of the x th missing frag
 *
 * \param [IN] x   x th missing frag
 *
 * \retval counter The counter value associated to the x th missing frag
 */
static uint16_t FragFindMissingIndex( uint16_t x )
{
    for( uint16_t i = 0; i < FragDecoder.FragNb; i++ )
    {
        if( FragDecoder.FragNbMissingIndex[i] == ( x + 1 ) )
        {
            return i;
        }
    }
    return 0;
}

/*!
 * \brief Extacts a row from the binary matrix and expands it to a bitArray
 *
 * \param [IN] bitArray  Pointer to the bit array
 * \param [IN] rowIndex  Matrix row index
 * \param [IN] bitsInRow Number of bits in one row
 */
static void FragExtractLineFromBinaryMatrix( uint8_t* bitArray, uint16_t rowIndex, uint16_t bitsInRow )
{
    uint32_t findByte = 0;
    uint32_t findBitInByte = 0;

    if( rowIndex > 0 )
    {
        findByte      = ( rowIndex * bitsInRow - ( ( rowIndex * ( rowIndex - 1 ) ) >> 1 ) ) >> 3;
        findBitInByte = ( rowIndex * bitsInRow - ( ( rowIndex * ( rowIndex - 1 ) ) >> 1 ) ) % 8;
    }
    if( rowIndex > 0 )
    {
        for( uint16_t i = 0; i < rowIndex; i++ )
        {
            SetParity( i, bitArray, 0 );
        }
    }
    for( uint16_t i = rowIndex; i < bitsInRow; i++ )
    {
        SetParity( i,
                   bitArray, 
                   ( FragDecoder.MatrixM2B[findByte] >> ( 7 - findBitInByte ) ) & 0x01 );

        findBitInByte++;
        if( findBitInByte == 8 )
        {
            findBitInByte = 0;
            findByte++;
        }
    }
}

/*!
 * \brief Collapses and Pushs a row of a bit array to the matrix
 *
 * \param [IN] bitArray  Pointer to the bit array
 * \param [IN] rowIndex  Matrix row index
 * \param [IN] bitsInRow Number of bits in one row
 */
static void FragPushLineToBinaryMatrix( uint8_t *bitArray, uint16_t rowIndex, uint16_t bitsInRow )
{
    uint32_t findByte = 0;
    uint32_t findBitInByte = 0;

    if ( rowIndex > 0) {
        findByte      = ( rowIndex * bitsInRow - ( ( rowIndex * ( rowIndex - 1 ) ) >> 1 ) ) >> 3;
        findBitInByte = ( rowIndex * bitsInRow - ( ( rowIndex * ( rowIndex - 1 ) ) >> 1 ) ) % 8;

    }
    for( uint16_t i = rowIndex; i < bitsInRow; i++ )
    {
        if( GetParity( i, bitArray ) == 0 )
        {
            FragDecoder.MatrixM2B[findByte] = FragDecoder.MatrixM2B[findByte] & ( 0xFF - ( 1 << ( 7 - findBitInByte ) ) );
        }
        findBitInByte++;
        if( findBitInByte == 8 )
        {
            findBitInByte = 0;
            findByte++;
        }
    }
}
