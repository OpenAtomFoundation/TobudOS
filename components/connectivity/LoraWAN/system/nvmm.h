/*!
 * \file      nvmm.h
 *
 * \brief     None volatile memory management module
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
#ifndef __NVMM_H__
#define __NVMM_H__

#include <stdlib.h>
#include <stdint.h>

/*!
 * Nvmm Status
 */
typedef enum eNvmmStatus
{
    /*!
     * No error occurred
     */
    NVMM_SUCCESS = 0,
    /*!
     * Checksum of the data block does not match.
     */
    NVMM_FAIL_CHECKSUM,
    /*!
     * Size does not fit
     */
    NVMM_ERROR_SIZE,
    /*!
     * Null pointer exception
     */
    NVMM_ERROR_NPE,
    /*!
     * Undefined Error occurred
     */
    NVMM_ERROR,
}NvmmStatus_t;

/*!
 * Nvmm Data Block handle
 */
typedef struct
{
  /*
   * Unique internal used virtual address for the data block.
   */
  uint16_t virtualAddr;
}NvmmDataBlock_t;

/*!
 * Declares a data block
 *
 * \param[IN] num Size as number of bytes.
 * \retval           Status of the operation
 */
 NvmmStatus_t NvmmDeclare( NvmmDataBlock_t* dataB, size_t num );

/*!
 * Reads the data block header and verifies the checksum to determine
 * if it ever has been written or the data is corrupted.
 *
 * \param[IN] dataB  Pointer to the data block.
 * \param[IN] num    Size as number of bytes.
 * \retval           Status of the operation
 */
NvmmStatus_t NvmmVerify( NvmmDataBlock_t* dataB, size_t num );

/*!
 *  Writes data to given data block.
 *
 * \param[IN] dataB  Pointer to the data block.
 * \param[IN] src    Pointer to the source of data to be copied.
 * \param[IN] num    Number of bytes to copy.
 * \retval           Status of the operation
 */
NvmmStatus_t NvmmWrite( NvmmDataBlock_t* dataB, void* src, size_t num );

/*!
 * Reads from data block to destination pointer.
 *
 * \param[IN] dataB  Pointer to the data block.
 * \param[IN] dst    Pointer to the destination array where the content is to be copied.
 * \param[IN] num    Number of bytes to copy.
 * \retval           Status of the operation
 */
NvmmStatus_t NvmmRead( NvmmDataBlock_t* dataB, void* dst, size_t num );

#endif // __NVMM_H__
