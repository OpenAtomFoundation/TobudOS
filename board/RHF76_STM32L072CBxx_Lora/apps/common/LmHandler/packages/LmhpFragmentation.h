/*!
 * \file      LmhpFragmentation.h
 *
 * \brief     Implements the LoRa-Alliance fragmented data block transport package
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
 * \author    Miguel Luis ( Semtech )
 */
#ifndef __LMHP_FRAGMENTATION_H__
#define __LMHP_FRAGMENTATION_H__

#include "LoRaMac.h"
#include "LmHandlerTypes.h"
#include "LmhPackage.h"
#include "FragDecoder.h"

/*!
 * Fragmentation data block transport package identifier.
 *
 * \remark This value must be unique amongst the packages
 */
#define PACKAGE_ID_FRAGMENTATION                    3

/*!
 * Fragmentation package parameters
 */
typedef struct LmhpFragmentationParams_s
{
#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
    /*!
     * FragDecoder Write/Read function callbacks
     */
    FragDecoderCallbacks_t DecoderCallbacks;
#else
    /*!
     * Pointer to the un-fragmented received buffer.
     */
    uint8_t *Buffer;
    /*!
     * Size of the un-fragmented received buffer.
     */
    uint32_t BufferSize;
#endif
    /*!
     * Notifies the progress of the current fragmentation session
     *
     * \param [IN] fragCounter Fragment counter
     * \param [IN] fragNb      Number of fragments
     * \param [IN] fragSize    Size of fragments
     * \param [IN] fragNbLost  Number of lost fragments
     */
    void ( *OnProgress )( uint16_t fragCounter, uint16_t fragNb, uint8_t fragSize, uint16_t fragNbLost );
#if( FRAG_DECODER_FILE_HANDLING_NEW_API == 1 )
    /*!
     * Notifies that the fragmentation session is finished
     *
     * \param [IN] status Fragmentation session status [FRAG_SESSION_ONGOING,
     *                                                  FRAG_SESSION_FINISHED or
     *                                                  FragDecoder.Status.FragNbLost]
     * \param [IN] size   Received file size
     */
    void ( *OnDone )( int32_t status, uint32_t size );
#else
    /*!
     * Notifies that the fragmentation session is finished
     *
     * \param [IN] status Fragmentation session status [FRAG_SESSION_ONGOING,
     *                                                  FRAG_SESSION_FINISHED or
     *                                                  FragDecoder.Status.FragNbLost]
     * \param [IN] file   Pointer to the reception file buffer
     * \param [IN] size   Received file size
     */
    void ( *OnDone )( int32_t status, uint8_t *file, uint32_t size );
#endif
}LmhpFragmentationParams_t;

LmhPackage_t *LmhpFragmentationPackageFactory( void );

#endif // __LMHP_FRAGMENTATION_H__
