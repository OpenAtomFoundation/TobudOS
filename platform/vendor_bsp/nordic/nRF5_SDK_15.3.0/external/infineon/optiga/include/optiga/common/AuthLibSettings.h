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
* \brief    This file is defines the compilation switches to build code with required features.
*
* \addtogroup  grMutualAuth
* @{
*/


#ifndef _AUTH_LIB_SETTINGS_H_
#define _AUTH_LIB_SETTINGS_H_
/*
To enable a user in choosing required feature set, compilation switches are provided to build the code.
1.FEATURE_ONEWAY_AUTH       : One Way authentication feature is enabled.
2.FEATURE_DTLS_MUTUAL_AUTH  : Mutual authentication feature is enabled.
3.FEATURE_TOOLBOX           : Tool box feature is enabled
4.By default all features are enabled.

A user can define more switches in the following way and add it before the default #else directive

//Sample definition for new compile time switch
#ifdef NEW_REQUIRED_FEATURES
  #define MODULE_X
  #define MODULE_Y
  #define MODULE_Z
#endif
*/

//If only One Way Authentication feature required
#ifdef FEATURE_ONEWAY_AUTH
    #define MODULE_ENABLE_READ_WRITE
    #define MODULE_ENABLE_ONE_WAY_AUTH
    #define FEATURE_ENABLED
#endif

//If only Mutual Authentication feature required
#ifdef FEATURE_DTLS_MUTUAL_AUTH
    #define MODULE_ENABLE_READ_WRITE
    #define MODULE_ENABLE_DTLS_MUTUAL_AUTH
    #define FEATURE_ENABLED
#endif

#ifdef FEATURE_TOOLBOX
    #define MODULE_ENABLE_TOOLBOX
    #define FEATURE_ENABLED
#endif

//By Default all are enabled
#ifndef FEATURE_ENABLED
    #define MODULE_ENABLE_READ_WRITE
    #define MODULE_ENABLE_ONE_WAY_AUTH
//  #define MODULE_ENABLE_DTLS_MUTUAL_AUTH
    #define MODULE_ENABLE_TOOLBOX
#endif

#ifdef OPTIMIZE_CODE
    #define OPTIMIZE_CODE(x) #ifndef x
    #define OPTIMIZE_CODE_ENDS(x) #endif
#else
    #define OPTIMIZE_CODE(x)
    #define OPTIMIZE_CODE_ENDS(x)
#endif
#endif /* _AUTH_LIB_SETTINGS_H_*/

/**
* @}
*/
