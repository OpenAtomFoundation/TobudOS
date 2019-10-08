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
* \brief   This file implements
*
* \addtogroup  grOptigaUtil
* @{
*/

#include "optiga/optiga_util.h"
#include "optiga/comms/optiga_comms.h"
#include "optiga/cmd/CommandLib.h"
#include "optiga/pal/pal_os_timer.h"

/**
 * OPTIGA util module return values
 */
///OPTIGA util API execution is successful
#define OPTIGA_UTIL_SUCCESS                         (0x0000)
///OPTIGA util module in busy state
#define OPTIGA_UTIL_BUSY                            (0x0001)
///OPTIGA util API failed
#define OPTIGA_UTIL_ERROR                           (0x0302)
///OPTIGA util API called with invalid inputs
#define OPTIGA_UTIL_ERROR_INVALID_INPUT             (0x0303)
///OPTIGA util API called with insufficient memory buffer
#define OPTIGA_UTIL_ERROR_MEMORY_INSUFFICIENT       (0x0304)
///OPTIGA util API called when, a request of same instance is already in service
#define OPTIGA_UTIL_ERROR_INSTANCE_IN_USE           (0x0305)

///Requested operation completed without any error
#define INT_LIB_OK                          0x75AB1C02

///Null parameter(s)
#define INT_LIB_NULL_PARAM                  0x80002001

///Invalid data in response
#define INT_LIB_INVALID_RESPONSE            (INT_LIB_NULL_PARAM + 1)

///Invalid access condition
#define INT_LIB_INVALID_AC                  (INT_LIB_NULL_PARAM + 2)

///Length of input is zero
#define INT_LIB_ZEROLEN_ERROR               (INT_LIB_NULL_PARAM + 3)

///Invalid or unsupported parameter(s)
#define INT_LIB_INVALID_PARAM               (INT_LIB_NULL_PARAM + 4)

///Invalid Length
#define INT_LIB_INVALID_LENGTH              (INT_LIB_NULL_PARAM + 5)

///Malloc Failures
#define INT_LIB_MALLOC_FAILURE              (INT_LIB_NULL_PARAM + 6)

///General error
#define INT_LIB_ERROR                       0xFE5A5502

///Length of metadata
#define LENGTH_METADATA             0x1C

///Length of certificate
#define LENGTH_CERTIFICATE          1728

///Length of R and S vector
#define LENGTH_RS_VECTOR            0x40

///Length of maximum additional bytes to encode sign in DER
#define MAXLENGTH_SIGN_ENCODE       0x08

///Length of Signature
#define LENGTH_ASN1_SIGNATURE       (LENGTH_RS_VECTOR + MAXLENGTH_SIGN_ENCODE)

///size of public key for NIST-P256
#define LENGTH_PUB_KEY_NISTP256     0x41

///Position of data field in metadata
#define METADATA_MAX_LEN            0x001C

///Offset of Length in the metadata
#define OFFSET_TLV_LENGTH           0x01

///Size of TLV Format Header
#define TLV_HEADER_SIZE             0x02

///Length of metadata header
#define METADATA_HEADER_SIZE        TLV_HEADER_SIZE

///Value for parsig failure
#define PARSE_FAILURE               0xFF

///TLV position for Length
#define POS_LEN                     0x01

///TLV position for Value
#define POS_VAL                     0x02

///Already Found
#define VALUE_TAG_FOUND             0x9B

///Length of R and S vector
#define VALUE_TAG_NOTFOUND          0x31

///ASN Tag for sequence
#define ASN_TAG_SEQUENCE            0x30

///ASN Tag for integer
#define ASN_TAG_INTEGER             0x02

///msb bit mask
#define MASK_MSB                    0x80

///TLS Identity Tag
#define TLS_TAG                     0xC0

/**
 * \brief Enumeration to object ids.
 */
typedef enum eObjectId_d
{
    ///Life Cycle State Global
    eLCSG = 0xE0C0,
    ///Life Cycle State Application
    eLCSA = 0xF1C0
}eObjectId_d;

/**
 * \brief Structure to specify general purpose data object parameters for read
 */
typedef struct sACVector_d
{
    ///OID of data object
    uint8_t bLcsA;

    uint8_t bLcsG;

    uint8_t bLcsO;

    sbBlob_d *psMetaData;

}sACVector_d;

/**
 * \brief Operators available in simple and complex Access Conditions
 */

typedef enum eOperator_d {
    /// Equal
    eOP_EQUAL = 0xFA,
    /// Greater than
    eOP_GREATER_THAN = 0xFB,
    /// Less than
    eOP_LESS_THAN = 0xFC,
    /// AND
    eOP_AND = 0xFD,
    /// OR
    eOP_OR = 0xFE
} eOperator_d;

/**
 * \brief Meta data tags
 */
typedef enum eMetaDataTag_d
{
    /// Object Life Cycle
    eLCSO =     0xC0,
    /// Change AC
    eCHANGE_AC = 0xD0,
    /// Read AC
    eREAD_AC =  0xD1
} eMetaDataTag_d;

/**
 * \brief IDs associated with the metadata access condition (simple and complex)
 */
typedef enum eAccessConditionID_d
{
    /// Always
    eACID_ALW = 0x00,
    /// Global life cycle status
    eACID_LCSG = 0x70,
    /// Application specific life cycle status
    eACID_LCSA = 0xE0,
    /// Data object specific life cycle status
    eACID_LCSO = 0xE1,
    /// Never
    eACID_NEV = 0xFF
} eAccessConditionID_d;

volatile static host_lib_status_t optiga_comms_status;

#ifdef MODULE_ENABLE_READ_WRITE
/**
 *
 * Implementation to get the metadata tag position.<br>
 * Returns error if metadata is not correct<br>
 * (if duplication of tag, metadata length / Tag length more than max length of metadata).<br> <br>
 *
 * PpbPos contains the actual position of the tag if found.<br>
 * PpbPos contains 0xFF if tag not found.This is considering that metatdata length is 28 bytes.
 * The return value in this case is #INT_LIB_OK.<br>
 *
 * \param[in]  Pprgbmetadata   Pointer to the buffer that contains metadata
 * \param[in]  PbTag           Tag type.
 * \param[in]  PpbPos          Pointer to tag position in metadata
 *
 * \retval    #INT_LIB_OK      Successful execution
 * \retval    #INT_LIB_ERROR   Failure in execution
 *
 */
static int32_t __optiga_util_get_tag_position (const uint8_t* Pprgbmetadata, uint8_t PbTag, puint8_t PpbPos)
{
    int32_t i4Status = (int32_t)INT_LIB_ERROR;
    uint8_t bMetadataSize, bAcLen;
    uint8_t bTempPos, bAlreadyFound=VALUE_TAG_NOTFOUND;

    do
    {
        if (NULL == Pprgbmetadata || NULL == PpbPos)
        {
            break;
        }

        bMetadataSize = Pprgbmetadata[OFFSET_TLV_LENGTH];

        *PpbPos = 0xFF;

        if (METADATA_MAX_LEN < (bMetadataSize + METADATA_HEADER_SIZE))
        {
            //Metadata Corrupted [Length field in Metadata is more than METADATA_MAX_SIZE]
            break;
        }

        bTempPos = METADATA_HEADER_SIZE;

        for(;;)
        {
            if (Pprgbmetadata[bTempPos] == PbTag)
            {
                if (VALUE_TAG_FOUND == bAlreadyFound)
                {
                    i4Status = (int32_t)INT_LIB_ERROR;
                    break;
                }
                i4Status = INT_LIB_OK;
                *PpbPos = bTempPos;
                bAlreadyFound = VALUE_TAG_FOUND;
            }

            if(0xFF == Pprgbmetadata[bTempPos])
            {
                //Length field in Metadata is not correct
                break;
            }
            bAcLen = Pprgbmetadata[bTempPos+1];

            //Is metadata Corrupted?
            if (METADATA_MAX_LEN <= (bAcLen+bTempPos+1))
            {
                //Metadata Corrupted
                break;
            }

            bTempPos += (bAcLen+2);

            if(bMetadataSize <= (bTempPos-METADATA_HEADER_SIZE))
            {
                i4Status = INT_LIB_OK;
                break;
            }
        }
    } while(0);

    return i4Status;
}

/**
 *
 * Implementation to verify expressions related to LCSA, LCSG, LCSO.<br>
 *
 * \param[in]     PpsACVal             Pointer to the access condition vector
 * \param[in]     PprgbAC              Pointer to the AC expression
 * \param[in,out] PpwVerifyOver        Pointer to verification status
 *
 * \retval    #INT_LIB_OK       Successful execution
 * \retval    #INT_LIB_ERROR    Failure in execution
 *
 */
static int32_t __optiga_util_verify_lcsa_lcsg_lcso(const sACVector_d* PpsACVal, const uint8_t* PprgbAC,puint16_t PpwVerifyOver)
{
    int32_t i4Status = (int32_t)INT_LIB_ERROR;
    uint8_t bVal, bLcs = 0x00;
    eOperator_d eOp ;

    // Life cycle state of object
    #define LCS_O_VAL           (PpsACVal->bLcsO)
    // Life cycle state of application
    #define LCS_A_VAL           (PpsACVal->bLcsA)
    // Life cycle state of Global
    #define LCS_G_VAL           (PpsACVal->bLcsG)

    do
    {
        if((NULL == PpwVerifyOver)
            || (NULL == PpsACVal) || (NULL == PprgbAC))
        {
            break;
        }

        bLcs = LCS_A_VAL;
        if ((uint8_t)eACID_LCSG == *PprgbAC)
        {
            bLcs = LCS_G_VAL;
        }
        else if((uint8_t)eACID_LCSO == *PprgbAC)
        {
            bLcs = LCS_O_VAL;
        }

        eOp = (eOperator_d)(*(PprgbAC+1));
        bVal = *(PprgbAC+2);

        if(eOp == eOP_GREATER_THAN)
        {
            if(bLcs > bVal)
            {
                i4Status = INT_LIB_OK;
            }
        }
        else if(eOp == eOP_LESS_THAN)
        {
            if(bLcs < bVal)
            {
                i4Status = INT_LIB_OK;
            }
        }
        else if(eOp == eOP_EQUAL)
        {
            if(bLcs == bVal)
            {
                i4Status = INT_LIB_OK;
            }
        }
        else
        {
            i4Status = (int32_t)INT_LIB_ERROR;
            *PpwVerifyOver = TRUE;
            break;
        }
    } while(0);
    return i4Status;
#undef LCS_O_VAL
#undef LCS_A_VAL
#undef LCS_G_VAL
}

/**
 *
 * Implementation to validate the access conditions.<br>
 *
 * \param[in]  PpsACVal   Pointer to the buffer that contains metadata
 *
 * \retval    #INT_LIB_OK       Successful execution
 * \retval    #INT_LIB_ERROR    Failure in execution
 *
 */
static int32_t __optiga_util_check_ac(const sACVector_d *PpsACVal)
{
    int32_t i4Status = (int32_t)INT_LIB_ERROR;
    int32_t i4StatusCurr = (int32_t)INT_LIB_ERROR;
    int32_t i4StatusPrev = (int32_t)INT_LIB_OK;
    uint16_t wIndex = 0, wLen, wIDCount = 0;
    uint16_t wVerificationOver = 0;
    uint8_t bComplexAcOP = 0x00;
    puint8_t prgbAccessCode;

    // Remaining bytes of the access condition
    #define REMAINING_BYTES (wLen - wIndex)

    do
    {
        if((NULL == PpsACVal) || (NULL == PpsACVal->psMetaData)
                    || (NULL == PpsACVal->psMetaData->prgbStream))
        {
            break;
        }

        wLen = PpsACVal->psMetaData->wLen;
        prgbAccessCode = PpsACVal->psMetaData->prgbStream;

        while(wIndex < wLen)
        {
            switch((eAccessConditionID_d)*(prgbAccessCode+wIndex))
            {
                case  eACID_ALW:
                case  eACID_NEV:
                    if((REMAINING_BYTES > 1) || (wIDCount > 0))
                    {
                        wVerificationOver = TRUE;
                        break;
                    }
                    i4StatusCurr = INT_LIB_OK;
                    if((uint8_t)eACID_NEV == *(prgbAccessCode+wIndex))
                    {
                        i4StatusCurr = (int32_t)INT_LIB_ERROR;
                    }
                    wIndex++;
                    wIDCount++;
                    break;

                case  eACID_LCSO:
                case  eACID_LCSA:
                case  eACID_LCSG:
                    //including access id
                    if(REMAINING_BYTES < 3)
                    {
                        //because of invalid access coding
                        wVerificationOver = TRUE;
                        break;
                    }

                    i4StatusCurr = __optiga_util_verify_lcsa_lcsg_lcso(PpsACVal, prgbAccessCode+wIndex, &wVerificationOver);
                    if(TRUE == wVerificationOver)
                    {
                        //because of invalid access coding
                        break;
                    }

                    wIndex+=3;
                    wIDCount++;
                    break;

                default:
                    //because of invalid access coding
                    i4StatusPrev = (int32_t)INT_LIB_ERROR;
                    i4StatusCurr = (int32_t)INT_LIB_ERROR;
                    wVerificationOver = TRUE;
                    break;
            }//switch

            if(wVerificationOver)
            {
                break;
            }

            if(bComplexAcOP == (uint8_t)eOP_AND)
            {
                if(i4StatusCurr != i4StatusPrev)
                {
                    i4StatusCurr = (int32_t)INT_LIB_ERROR;
                }
            }

            if(wIndex == wLen)
            {
                break;
            }

            // for operator
            if(REMAINING_BYTES < 3)
            {
                i4StatusPrev = (int32_t)INT_LIB_ERROR;
                i4StatusCurr = (int32_t)INT_LIB_ERROR;
                //wVerificationOver = TRUE;
                break;
            }

            bComplexAcOP = *(prgbAccessCode+wIndex);
            switch(bComplexAcOP)
            {
                case  eOP_AND:
                    i4StatusPrev = i4StatusCurr;
                    i4StatusCurr = (int32_t)INT_LIB_ERROR;
                    break;

                case  eOP_OR:
                    i4StatusPrev = INT_LIB_OK;
                    if(i4StatusCurr == INT_LIB_OK)
                    {
                        //Note: further validation not required
                        //because the metadata itself is protected by checksum
                        wVerificationOver = TRUE;
                    }
                    break;

                default:
                    //because of invalid access coding
                    i4StatusPrev = (int32_t)INT_LIB_ERROR;
                    i4StatusCurr = (int32_t)INT_LIB_ERROR;
                    wVerificationOver = TRUE;
                    break;
            }//switch

            if(wVerificationOver)
            {
                break;
            }

            wIndex++;
            wIDCount++; //indication of complex AC
        }//while

        if((i4StatusPrev == INT_LIB_OK) &&
        (i4StatusCurr == INT_LIB_OK))
        {
            i4Status = INT_LIB_OK;
        }

    }while(0);

    return i4Status;
#undef REMAINING_BYTES
}



/**
 *
 * Reads either LcsA or LcsG based on request.<br>
 *
 * \param[in]  PeLcsType   ObjectId of LcsA or LcsG
 * \param[in,out]  PpbValue    Pointer for returning life cycle state.
 *
 * \retval    #INT_LIB_OK       Successful execution
 * \retval    #INT_LIB_ERROR    Failure in execution
 *
 */
static int32_t __optiga_util_read_lcsa_lcsg(eObjectId_d PeLcsType,uint8_t *PpbValue)
{
    int32_t i4Status  = (int32_t)INT_LIB_ERROR;
    sGetData_d sGDVector;
    sCmdResponse_d sResponse;
    do
    {
        if(NULL == PpbValue)
        {
            i4Status = (int32_t)INT_LIB_NULL_PARAM;
            break;
        }
        sGDVector.wOID = (uint16_t)PeLcsType;
        sGDVector.wLength = 1;
        sGDVector.wOffset = 0;
        sGDVector.eDataOrMdata = eDATA;

        sResponse.prgbBuffer = PpbValue;
        sResponse.wBufferLength = 1;
        sResponse.wRespLength = 0;

        i4Status = CmdLib_GetDataObject(&sGDVector,&sResponse);
        if(CMD_LIB_OK != i4Status)
        {
            break;
        }
        //check if the length is 1
        if((0x01 != sResponse.wRespLength))
        {
            i4Status = (int32_t)INT_LIB_INVALID_RESPONSE;
            *PpbValue = 0x00;
            break;
        }
        i4Status = INT_LIB_OK;
    }while(FALSE);
   return i4Status;
}

/**
 *
 * Verifies the requested access condition in the metadata.<br>
 *
 * \param[in]  PeMetaDataTag     Type of access condition
 * \param[in]  PpsACVal         Pointer to metadata.
 *
 * \retval    #INT_LIB_OK       Successful execution
 * \retval    #INT_LIB_ERROR    Failure in execution
 *
 */
static int32_t __optiga_util_verify_ac(eMetaDataTag_d PeMetaDataTag, sACVector_d *PpsACVal)
{
    int32_t i4Status  = (int32_t)INT_LIB_ERROR;
    uint8_t bTagLocation = 0;
    do
    {
        if((NULL == PpsACVal)||(NULL == PpsACVal->psMetaData)||
            (NULL == PpsACVal->psMetaData->prgbStream))
        {
            i4Status = (int32_t)INT_LIB_NULL_PARAM;
            break;
        }

        //get tag position of lcsO and read lcsO.
        //LCO may not be found for all object.It is not an error
        i4Status = __optiga_util_get_tag_position(PpsACVal->psMetaData->prgbStream,(uint8_t)eLCSO,&bTagLocation);
        if(INT_LIB_OK == i4Status && bTagLocation != PARSE_FAILURE)
        {
            //get the LcsO value from TLV
            PpsACVal->bLcsO = (PpsACVal->psMetaData->prgbStream)[bTagLocation+2];
        }
        else
        {
            //LcsO not present
            PpsACVal->bLcsO = 0x00;
        }
        //reset tag location
        bTagLocation = 0;
        //get tag position
        i4Status = __optiga_util_get_tag_position(PpsACVal->psMetaData->prgbStream,(uint8_t)PeMetaDataTag,&bTagLocation);
        if((INT_LIB_OK != i4Status) || (PARSE_FAILURE == bTagLocation))
        {
            i4Status = (int32_t)INT_LIB_ERROR;
            break;
        }
        //check access condition
        PpsACVal->psMetaData->wLen = *(PpsACVal->psMetaData->prgbStream + (bTagLocation+POS_LEN));
        PpsACVal->psMetaData->prgbStream += (bTagLocation+POS_VAL);

        i4Status = __optiga_util_check_ac(PpsACVal);
    }while(FALSE);

    return i4Status;
}

static void __optiga_util_comms_event_handler(void* upper_layer_ctx, host_lib_status_t event)
{
    optiga_comms_status = event;
}

optiga_lib_status_t optiga_util_open_application(optiga_comms_t* p_comms)
{
    optiga_lib_status_t status = OPTIGA_LIB_ERROR;
    sOpenApp_d sOpenApp;

    do {
        // OPTIGA Initialisation phase
        //Invoke optiga_comms_open to initialize the IFX I2C Protocol and security chip
        optiga_comms_status = OPTIGA_COMMS_BUSY;
        p_comms->upper_layer_handler = __optiga_util_comms_event_handler;
        status = optiga_comms_open(p_comms);
        if(E_COMMS_SUCCESS != status)
        {
            status = OPTIGA_LIB_ERROR;
            break;
        }

        //Wait until IFX I2C initialization is complete
        while(optiga_comms_status == OPTIGA_COMMS_BUSY)
        {
            pal_os_timer_delay_in_milliseconds(1);
        }

        if((OPTIGA_COMMS_SUCCESS != status) || (optiga_comms_status == OPTIGA_COMMS_ERROR))
        {
            status = OPTIGA_LIB_ERROR;
            break;
        }

        //Set OPTIGA comms context in Command library before invoking the use case APIs or command library APIs
        //This context will be used by command libary to communicate with OPTIGA using IFX I2C Protocol.
        CmdLib_SetOptigaCommsContext(p_comms);

        //Open the application in Security Chip
        sOpenApp.eOpenType = eInit;
        status = CmdLib_OpenApplication(&sOpenApp);
        if(CMD_LIB_OK != status)
        {
            status = OPTIGA_LIB_ERROR;
        }

        // OPTIGA ready
        status = OPTIGA_LIB_SUCCESS;
    } while(FALSE);

    return status;
}

optiga_lib_status_t optiga_util_read_data(uint16_t optiga_oid, uint16_t offset,
                                          uint8_t * p_buffer, uint16_t* buffer_size)
{
    //lint --e{818} suppress "PpsGPData is out parameter"
    int32_t status  = (int32_t)OPTIGA_LIB_ERROR;
    sGetData_d cmd_params;
    sCmdResponse_d cmd_resp;

    do
    {
        if((NULL == p_buffer) || (NULL == buffer_size) || (0 == *buffer_size))
        {
            status = (int32_t)OPTIGA_LIB_ERROR;
            break;
        }

        //If access condition satisfied, get the data
        cmd_params.wOID = optiga_oid;
        cmd_params.wLength = *buffer_size;
        cmd_params.wOffset = offset;
        cmd_params.eDataOrMdata = eDATA;

        cmd_resp.prgbBuffer = p_buffer;
        cmd_resp.wBufferLength = *buffer_size;
        cmd_resp.wRespLength = 0;

        status = CmdLib_GetDataObject(&cmd_params,&cmd_resp);

        if(CMD_LIB_OK != status)
        {
            status = (int32_t)OPTIGA_LIB_ERROR;
            break;
        }
        *buffer_size = cmd_resp.wRespLength;
        status = OPTIGA_LIB_SUCCESS;
    }while(FALSE);

    return status;
}

optiga_lib_status_t optiga_util_read_metadata(uint16_t optiga_oid, uint8_t * p_buffer, uint16_t* buffer_size)
{
    //lint --e{818} suppress "PpsGPData is out parameter"
    int32_t status  = (int32_t)OPTIGA_LIB_ERROR;
    sGetData_d cmd_params;
    sCmdResponse_d cmd_resp;
    uint16_t buffer_limit = *buffer_size;
    sbBlob_d meta_data = {LENGTH_METADATA, p_buffer};
    sACVector_d ac_vector;

    do
    {
        if((NULL == p_buffer) || (NULL == buffer_size) || (0 == *buffer_size))
        {
            status = (int32_t)OPTIGA_LIB_ERROR;
            break;
        }

        //Read lcsA
        status = __optiga_util_read_lcsa_lcsg(eLCSA,&(ac_vector.bLcsA));
        if(INT_LIB_OK != status)
        {
           status = (int32_t)OPTIGA_LIB_ERROR;
           break;
        }

        //Read lcsG
        status = __optiga_util_read_lcsa_lcsg(eLCSG,&(ac_vector.bLcsG));
        if(INT_LIB_OK != status)
        {
            status = (int32_t)OPTIGA_LIB_ERROR;
            break;
        }

        //check if OID is for lcsA or lcaG
        if((uint16_t)eLCSA == optiga_oid)
        {
            //return the read value
            *p_buffer = ac_vector.bLcsA;
            *buffer_size = 0x01;
            break;
        }
        if((uint16_t)eLCSG == optiga_oid)
        {
            //return the read value
            *p_buffer = ac_vector.bLcsG;
            *buffer_size = 0x01;
            break;
        }

        //Get metadata of oid
        cmd_params.wOID = optiga_oid;
        cmd_params.wLength = LENGTH_METADATA;
        cmd_params.wOffset = 0;
        cmd_params.eDataOrMdata = eMETA_DATA;

        cmd_resp.prgbBuffer = p_buffer;
        cmd_resp.wBufferLength = buffer_limit;
        cmd_resp.wRespLength = 0;

        status = CmdLib_GetDataObject(&cmd_params,&cmd_resp);
        if(CMD_LIB_OK != status)
        {
            status = (int32_t)OPTIGA_LIB_ERROR;
            break;
        }
        //Check the length
        *buffer_size = *(cmd_resp.prgbBuffer + POS_LEN );
        if((*buffer_size != (cmd_resp.wRespLength-POS_VAL)))
        {
            status = (int32_t)INT_LIB_INVALID_RESPONSE;
            break;
        }
        //Check read access condition
        ac_vector.psMetaData = &meta_data;
        status = __optiga_util_verify_ac(eREAD_AC,&ac_vector);
        if(INT_LIB_OK != status)
        {
            status = (int32_t)INT_LIB_INVALID_AC;
            break;
        }
        status = OPTIGA_LIB_SUCCESS;

    }while(FALSE);

    if(status != OPTIGA_LIB_SUCCESS)
    {
        status = OPTIGA_LIB_ERROR;
    }

    return status;

}

optiga_lib_status_t optiga_util_write_data(uint16_t optiga_oid, uint8_t write_type, uint16_t offset, uint8_t * p_buffer, uint16_t buffer_size)
{
    int32_t status  = (int32_t)OPTIGA_LIB_ERROR;

    sSetData_d sd_params;

    do
    {
        if((NULL == p_buffer) || (0x00 == buffer_size))
        {
            break;
        }

        if ((OPTIGA_UTIL_WRITE_ONLY != write_type) && (OPTIGA_UTIL_ERASE_AND_WRITE != write_type))
        {
            status = OPTIGA_UTIL_ERROR_INVALID_INPUT;
            break;
        }

        //If access condition satisfied, set the data
        sd_params.wOID = optiga_oid;
        sd_params.wOffset = offset;
        sd_params.eDataOrMdata = eDATA;
        if (OPTIGA_UTIL_ERASE_AND_WRITE == write_type)
            sd_params.eWriteOption = eERASE_AND_WRITE;
        else
            sd_params.eWriteOption = eWRITE;
        sd_params.prgbData = p_buffer;
        sd_params.wLength = buffer_size;

        status = CmdLib_SetDataObject(&sd_params);
        if(CMD_LIB_OK != status)
        {
            break;
        }
        status = OPTIGA_LIB_SUCCESS;
    }while(FALSE);

    if(status != OPTIGA_LIB_SUCCESS)
    {
        status = OPTIGA_LIB_ERROR;
    }

    return status;
}

optiga_lib_status_t optiga_util_write_metadata(uint16_t optiga_oid, uint8_t * p_buffer, uint8_t buffer_size)
{

    int32_t status  = (int32_t)OPTIGA_LIB_ERROR;

    sSetData_d sd_params;

    //If access condition satisfied, set the data
    sd_params.wOID = optiga_oid;
    sd_params.wOffset = 0;
    sd_params.eDataOrMdata = eMETA_DATA;
    sd_params.eWriteOption = eWRITE;
    sd_params.prgbData = p_buffer;
    sd_params.wLength = buffer_size;

    status = CmdLib_SetDataObject(&sd_params);
    if(CMD_LIB_OK != status)
    {
        return  OPTIGA_LIB_ERROR;
    }

    return OPTIGA_LIB_SUCCESS;
}

#endif // MODULE_ENABLE_READ_WRITE
