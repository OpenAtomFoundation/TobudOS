/**
 * @file      atecc608a-tnglora-se-hal.h
 *
 * @brief     Secure Element hardware abstraction layer
 *
 * @remark    Current implementation only supports LoRaWAN 1.0.x version
 * 
 * @copyright Copyright (c) 2020 The Things Industries B.V.
 * 
 * Revised BSD License
 * Copyright The Things Industries B.V 2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Things Industries B.V nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE THINGS INDUSTRIES B.V BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __ATECC608A_TNGLORA_SE_HAL_H__
#define __ATECC608A_TNGLORA_SE_HAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*!
 * \brief Get a random number
 *
 * \remark The number SHALL NOT be generated using a pseudo random number
 *         generator
 * \retval number 32 bit random value
 */
uint32_t ATECC608ASeHalGetRandomNumber( void );

#ifdef __cplusplus
}
#endif

#endif  //  __ATECC608A_TNGLORA_SE_HAL_H__
