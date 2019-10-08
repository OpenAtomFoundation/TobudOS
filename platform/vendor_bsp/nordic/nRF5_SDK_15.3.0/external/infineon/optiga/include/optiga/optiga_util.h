/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
* \file
*
* \brief   This file defines APIs, types and data structures used in the OPTIGA UTIL module.
*
* \defgroup  grOptigaUtil OPTIGA UTIL module
* \ingroup infineon_api
* @{
*/

#ifndef _H_OPTIGA_UTIL_H_
#define _H_OPTIGA_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "optiga/cmd/CommandLib.h"
#include "optiga/common/AuthLibSettings.h"

/// Option to only write the data object
#define OPTIGA_UTIL_WRITE_ONLY      (0x00)
/// Option to erase and write the data object
#define OPTIGA_UTIL_ERASE_AND_WRITE (0x40)

/**
 * \brief  Typedef for OIDs
 */
typedef enum eOID_d
{
    /// Global Life Cycle State
    eLCS_G = 0xE0C0,
    /// Global Security Status
    eSECURITY_STATUS_G = 0xE0C1,
    /// Coprocessor UID
    eCOPROCESSOR_UID = 0xE0C2,
    /// Global Life Cycle State
    eSLEEP_MODE_ACTIVATION_DELAY = 0xE0C3,
    /// Current limitation
    eCURRENT_LIMITATION = 0xE0C4,
    /// Security Event Counter
    eSECURITY_EVENT_COUNTER = 0xE0C5,
    /// Device Public Key Certificate issued by IFX
    eDEVICE_PUBKEY_CERT_IFX = 0xE0E0,
    /// Project-Specific device Public Key Certificate
    eDEVICE_PUBKEY_CERT_PRJSPC_1 = 0xE0E1,
    /// Project-Specific device Public Key Certificate
    eDEVICE_PUBKEY_CERT_PRJSPC_2 = 0xE0E2,
    /// Project-Specific device Public Key Certificate
    eDEVICE_PUBKEY_CERT_PRJSPC_3 = 0xE0E3,
    /// First Device Private Key
    eFIRST_DEVICE_PRIKEY_1 = 0xE0F0,
    /// First Device Private Key
    eFIRST_DEVICE_PRIKEY_2 = 0xE0F1,
    /// First Device Private Key
    eFIRST_DEVICE_PRIKEY_3 = 0xE0F2,
    /// First Device Private Key
    eFIRST_DEVICE_PRIKEY_4 = 0xE0F3,
    /// Application Life Cycle Status
    eLCS_A = 0xF1C0,
    /// Application Security Status
    eSECURITY_STATUS_A = 0xF1C1,
    /// Error codes
    eERROR_CODES = 0xF1C2
} eOID_d;

/**
 * @brief Initializes the communication with optiga.
 *
 * Initializes the communication with OPTIGA for the given instance. <br>
 *
 *<b>Pre Conditions:</b>
 *
 *<b>API Details:</b>
 * - Initiate open application command to optiga.<br>
 *<br>
 *
 * \param[in]      p_comms       Pointer to the communication parameters initialised before
 *
 * \retval  #OPTIGA_LIB_SUCCESS   Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR     Error during function execution
 */
optiga_lib_status_t optiga_util_open_application(optiga_comms_t* p_comms);

/**
 * @brief Reads data from optiga.
 *
 * Retrieves the requested data that is stored from the user provided data object.<br>
 *
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 *<b>API Details:</b>
 * - Invokes CmdLib_GetDataObject API, based on the input arguments to read the data from the data object.<br>
 *<br>
 *
 *<b>Notes:</b>
 * - The maximum value of the <b>*bytes_to_read</b> parameter is size of buffer <b>p_buffer</b>. In case the value is greater than buffer size, memory corruption can occur.<br>
 * - <b>*bytes_to_read</b> parameter is set to 0, in case any errors occurs while retrieving the data. initially contains the value of the user provided data length to be read. When the data is successfully retrieved, this value is updated with actual data length retrieved.<br>
 *      In case of any errors, the value is set to 0.<br>
 *
 * \param[in]      optiga_oid       OID of data object
 *                                  - It should be a valid data object, otherwise OPTIGA returns an error.<br>
 * \param[in]      offset           Offset from within data object
 *                                  - It must be valid offset from within data object, otherwise OPTIGA returns an error.<br>
 * \param[in,out]  buffer           Valid pointer to the buffer to which data is read
 * \param[in,out]  bytes_to_read    Valid pointer to the length of data to be read from data object
 *                                  - When the data is successfully retrieved, it is updated with actual data length retrieved
 *
 * \retval  #OPTIGA_LIB_SUCCESS     Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR       Wrong Input arguments provided or error occured
 */
optiga_lib_status_t optiga_util_read_data(uint16_t optiga_oid,
                                          uint16_t offset,
                                          uint8_t * buffer,
                                          uint16_t * bytes_to_read);

/**
 * @brief Reads metadata of a data object from optiga.
 *
 * Reads the metadata of the user provided data object.<br>
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 *<b>API Details:</b>
 * - Invokes CmdLib_GetDataObject API, based on the input arguments to read the metadata from the data object.<br>
 *<br>
 *
 *<b>Notes:</b>
 * - The metadata returned will be in TLV format.<br>
 * - The maximum value of the <b>*bytes_to_read</b> parameter is size of buffer <b>p_buffer</b>. In case the value is greater than buffer size, memory corruption can occur.<br>
 *
 * \param[in]      optiga_oid      OID of data object
 *                                - It should be a valid data object, otherwise OPTIGA returns an error.<br>
 * \param[in,out]  buffer          Valid pointer to the buffer to which metadata is read
 * \param[in,out]  bytes_to_read   Valid pointer to the length of metadata to be read from data object
 *                                 - When the metadata is successfully retrieved, it is updated with actual metadata length retrieved
 *
 * \retval  #OPTIGA_LIB_SUCCESS    Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR      Wrong Input arguments provided or error occured
 */
optiga_lib_status_t optiga_util_read_metadata(uint16_t optiga_oid,
                                              uint8_t * buffer,
                                              uint16_t * bytes_to_read);

/**
 * @brief Writes data to optiga.
 *
 * Writes the data provided by the user into the specified data object.<br>
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 *<b>API Details:</b>
 * - Invokes CmdLib_SetDataObject API, based on the input arguments to write the data to the data object.<br>
 *<br>
 *
 *<b>Notes:</b>
 * - The maximum value of the <b>bytes_to_write parameter</b> is size of buffer <b>p_buffer</b>. In case the value is greater than buffer size, incorrect values can get written into the data object in OPTIGA.<br>
 * - In case the write_type provided is other than <b>erase and write(0x00)</b> or <b>write only(0x40)</b>, the function returns OPTIGA_UTIL_ERROR.<br>
 *
 * \param[in]      optiga_oid     OID of data object
 *                                - It should be a valid data object, otherwise OPTIGA returns an error.<br>
 * \param[in]      write_type     Type of the write operation. Can be OPTIGA_UTIL_ERASE_AND_WRITE or OPTIGA_UTIL_WRITE_ONLY
 * \param[in]      offset         Offset from within data object
 *                                - It must be valid offset from within data object, otherwise OPTIGA returns an error.<br>
 * \param[in,out]  buffer         Valid pointer to the buffer with user data to write
 * \param[in]      bytes_to_write Length of data to be written
 *
 * \retval  #OPTIGA_LIB_SUCCESS   Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR     Wrong Input arguments provided or error occured
 */
optiga_lib_status_t optiga_util_write_data(uint16_t optiga_oid,
                                           uint8_t write_type,
                                           uint16_t offset,
                                           uint8_t * buffer,
                                           uint16_t bytes_to_write);

/**
 * @brief Writes metadata for the user provided data object.
 *
 * Writes metadata for the user provided data object.
 *
 *<b>Pre Conditions:</b>
 * - The application on OPTIGA must be opened using #optiga_util_open_application before using this API.<br>
 *
 *<b>API Details:</b>
 * - Invokes CmdLib_SetDataObject API, based on the input arguments to write metadata to the data object.<br>
 *<br>
 *
 *<b>Notes:</b>
 * - The maximum value of the <b>bytes_to_write</b> parameter is size of buffer <b>p_buffer</b>.
 *   In case the value is greater than buffer size, incorrect values can get written into the meta data of the data object in OPTIGA.<br>
 * - The metadata to be written must be in TLV format <br>
 *
 * \param[in]      optiga_oid     OID of data object
 *                                - It should be a valid data object, otherwise OPTIGA returns an error.<br>
 * \param[in,out]  buffer         Valid pointer to the buffer with metadata to write
 * \param[in]      bytes_to_write Length of metadata to be written
 *
 * \retval  #OPTIGA_LIB_SUCCESS   Successful invocation of optiga cmd module
 * \retval  #OPTIGA_LIB_ERROR     Wrong Input arguments provided or error occured
 */
optiga_lib_status_t optiga_util_write_metadata(uint16_t optiga_oid,
                                               uint8_t * buffer,
                                               uint8_t bytes_to_write);
#ifdef __cplusplus
}
#endif

#endif //_H_OPTIGA_UTIL_H_

/**
* @}
*/
