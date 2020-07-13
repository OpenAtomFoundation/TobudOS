/*!
 * \file      FragDecoder.h
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
#ifndef __FRAG_DECODER_H__
#define __FRAG_DECODER_H__

#include <stdint.h>

/*!
 * If set to 1 the new API defining \ref FragDecoderWrite and
 * \ref FragDecoderReadfunction callbacks is used.
 */
#define FRAG_DECODER_FILE_HANDLING_NEW_API          1

/*!
 * Maximum number of fragment that can be handled.
 *
 * \remark This parameter has an impact on the memory footprint.
 */
#define FRAG_MAX_NB                                 21

/*!
 * Maximum fragment size that can be handled.
 *
 * \remark This parameter has an impact on the memory footprint.
 */
#define FRAG_MAX_SIZE                               50

/*!
 * Maximum number of extra frames that can be handled.
 *
 * \remark This parameter has an impact on the memory footprint.
 */
#define FRAG_MAX_REDUNDANCY                         5

#define FRAG_SESSION_FINISHED                       ( int32_t )0
#define FRAG_SESSION_NOT_STARTED                    ( int32_t )-2
#define FRAG_SESSION_ONGOING                        ( int32_t )-1

typedef struct sFragDecoderStatus
{
    uint16_t FragNbRx;
    uint16_t FragNbLost;
    uint16_t FragNbLastRx;
    uint8_t MatrixError;
}FragDecoderStatus_t;

#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
typedef struct sFragDecoderCallbacks
{
    /*!
     * Writes `data` buffer of `size` starting at address `addr`
     *
     * \param [IN] addr Address start index to write to.
     * \param [IN] data Data buffer to be written.
     * \param [IN] size Size of data buffer to be written.
     * 
     * \retval status Write operation status [0: Success, -1 Fail]
     */
    uint8_t ( *FragDecoderWrite )( uint32_t addr, uint8_t *data, uint32_t size );
    /*!
     * Reads `data` buffer of `size` starting at address `addr`
     *
     * \param [IN] addr Address start index to read from.
     * \param [IN] data Data buffer to be read.
     * \param [IN] size Size of data buffer to be read.
     * 
     * \retval status Read operation status [0: Success, -1 Fail]
     */
    uint8_t ( *FragDecoderRead )( uint32_t addr, uint8_t *data, uint32_t size );
}FragDecoderCallbacks_t;
#endif

#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
/*!
 * \brief Initializes the fragmentation decoder
 *
 * \param [IN] fragNb     Number of expected fragments (without redundancy packets)
 * \param [IN] fragSize   Size of a fragment
 * \param [IN] callbacks  Pointer to the Write/Read functions.
 */
void FragDecoderInit( uint16_t fragNb, uint8_t fragSize, FragDecoderCallbacks_t *callbacks );
#else
/*!
 * \brief Initializes the fragmentation decoder
 *
 * \param [IN] fragNb     Number of expected fragments (without redundancy packets)
 * \param [IN] fragSize   Size of a fragment
 * \param [IN] file       Pointer to file buffer size
 * \param [IN] fileSize   File buffer size
 */
void FragDecoderInit( uint16_t fragNb, uint8_t fragSize, uint8_t *file, uint32_t fileSize );
#endif

#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
/*!
 * \brief Gets the maximum file size that can be received
 * 
 * \retval size FileSize
 */
uint32_t FragDecoderGetMaxFileSize( void );
#endif

/*!
 * \brief Function to decode and reconstruct the binary file
 *        Called for each receive frame
 * 
 * \param [IN] fragCounter Fragment counter [1..(FragDecoder.FragNb + FragDecoder.Redundancy)]
 * \param [IN] rawData     Pointer to the fragment to be processed (length = FragDecoder.FragSize)
 *
 * \retval status          Process status. [FRAG_SESSION_ONGOING,
 *                                          FRAG_SESSION_FINISHED or
 *                                          FragDecoder.Status.FragNbLost]
 */
int32_t FragDecoderProcess( uint16_t fragCounter, uint8_t *rawData );

/*!
 * \brief Gets the current fragmentation status
 * 
 * \retval status Fragmentation decoder status
 */
FragDecoderStatus_t FragDecoderGetStatus( void );

#endif // __FRAG_DECODER_H__
