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
*
* \file
*
* \brief   This file implements the Configuration functions for OCP Library.
*
* \addtogroup  grMutualAuth
* @{
*/
#include "optiga/dtls/DtlsTransportLayer.h"
#include "optiga/dtls/DtlsHandshakeProtocol.h" //To be put under ifdef
#include "optiga/dtls/DtlsRecordLayer.h"
#include "optiga/dtls/HardwareCrypto.h"
#include "optiga/optiga_dtls.h"

#ifdef MODULE_ENABLE_DTLS_MUTUAL_AUTH

/// @cond hidden
//lint --e{714} suppress "Functions are extern and not reference in header file as
//          these function not to be used for external interfaces. Hence suppressed"
Void ConfigHL(fPerformHandshake_d* PpfPerformHandshake,eConfiguration_d PeConfiguration)
{
    //Based on input mode assign pointers to PpsAppOCPCntx
    switch(PeConfiguration)
    {
        case eDTLS_12_UDP_HWCRYPTO:
            //Assign the Handshake layer function pointer to context data
            *PpfPerformHandshake = DtlsHS_Handshake;
            break;

        case eTLS_12_TCP_HWCRYPTO:
            break;
    }
}

//lint --e{714} suppress "Functions are extern and not reference in header file as
//          these function not to be used for external interfaces. Hence suppressed"
Void ConfigRL(sConfigRL_d* PpsConfigRL,eConfiguration_d PeConfiguration)
{
    //Based on input mode assign pointers to psConfigRL
    switch(PeConfiguration)
    {
        case eDTLS_12_UDP_HWCRYPTO:

            PpsConfigRL->pfInit = DtlsRL_Init;
            PpsConfigRL->pfSend = DtlsRL_Send;
            PpsConfigRL->pfRecv = DtlsRL_Recv;
            PpsConfigRL->pfClose = DtlsRL_Close;
            break;

        case eTLS_12_TCP_HWCRYPTO:
            break;
    }
}

//lint --e{714} suppress "Functions are extern and not reference in header file as
//          these function not to be used for external interfaces. Hence suppressed"
Void ConfigTL(sConfigTL_d* PpsConfigTL,eConfiguration_d PeConfiguration)
{
    //Based on input mode assign pointers to psConfigTL
    switch(PeConfiguration)
    {
        case eTLS_12_TCP_HWCRYPTO:
            break;
        case eDTLS_12_UDP_HWCRYPTO:
            //Assign function to function pointers
            PpsConfigTL->pfInit = DtlsTL_Init;
            PpsConfigTL->pfConnect = DtlsTL_Connect;
            PpsConfigTL->pfDisconnect = DtlsTL_Disconnect;
            PpsConfigTL->pfRecv = DtlsTL_Recv;
            PpsConfigTL->pfSend = DtlsTL_Send;
            break;
    }
}

//lint --e{714} suppress "Functions are extern and not reference in header file as
//         these function not to be used for external interfaces. Hence suppressed"
Void ConfigCL(sConfigCL_d* PpsConfigCL,eConfiguration_d PeConfiguration)
{
    //Based on input mode assign pointers to psConfigCL
    switch(PeConfiguration)
    {
        case eDTLS_12_UDP_HWCRYPTO:
        case eTLS_12_TCP_HWCRYPTO:
            PpsConfigCL->pfInit = HWCL_Init;
            PpsConfigCL->pfEncrypt = HWCL_Encrypt;
            PpsConfigCL->pfDecrypt = HWCL_Decrypt;
            PpsConfigCL->pfClose = HWCL_Close;
            break;
    }
}

/// @endcond




/**
* @}
*/
#endif /*MODULE_ENABLE_DTLS_MUTUAL_AUTH*/
