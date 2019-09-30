/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdint.h>
#ifdef DX_LINUX_PLATFORM
#include <stdio.h>//for printf
#endif
#include <string.h>//for memcpy
#ifdef DX_LINUX_PLATFORM
#include <unistd.h>//for usleep
#endif
#include "ssi_pal_types.h"
#include "integration_test_ssi_data.h"
#include "crys_hash.h"

const hashDataStuct hashVectors[] = {
	{
		/*Test parameters*/
		{"HASH-SHA1/Input_data_16"},
		/*Hash operation mode*/
		CRYS_HASH_SHA1_mode,
		/*input_data*/
		{0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a},
		/*input_data*/
		16*sizeof(uint8_t),
		/*SHA1 Expected output data*/
		{0x3fb53721,0xd3d034ef,0xc388c31e,0x21de6e96,0x7bd0505f}, 
		/*SHA1 expected dataSize*/
		CRYS_HASH_SHA1_DIGEST_SIZE_IN_BYTES*sizeof(uint8_t), 
	},

	{
		/*Test parameters*/
		{"HASH-SHA256/Input_data_32"},
		/*Hash operation mode*/
		CRYS_HASH_SHA256_mode,
		/*input_data*/
		{0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a,
		 0x65,0xa2,0x32,0xd6,0xbc,0xd0,0xf9,0x39,0xed,0x1f,0xe1,0x28,0xc1,0x3b,0x0e,0x1b},
		/*input_data*/
		32*sizeof(uint8_t),
		/*SHA256 Expected output data*/
		{0x9bb3cf75,0x9274c462,0x97ad2a1e,0x8b0f219c,0x9d0a1869,0x96f2e958,0x4a90b9a4,0x40aae7e6}, 
		/*SHA1 expected dataSize*/
		CRYS_HASH_SHA256_DIGEST_SIZE_IN_BYTES*sizeof(uint8_t), 
	},
	{
		/*Test parameters*/
		{"HASH-SHA512/Input_data_65"},
		/*Hash operation mode*/
		CRYS_HASH_SHA512_mode,
		/*input_data*/
		{0x99,0xfd,0x18,0xa3,0x5d,0x50,0x81,0x84,0xa6,0xf3,0x61,0xc6,0x7c,0xd9,0xb1,0x0b,
		0x4c,0xd1,0xd8,0xb2,0x46,0x57,0x2a,0x4d,0x03,0xb0,0xae,0x55,0x6b,0x36,0x24,0x1d,
		0xd6,0xf0,0x46,0x05,0x71,0x65,0x4f,0xf0,0xe4,0xb2,0xba,0xf8,0x31,0xdb,0x4c,0x60,
		0xdf,0x5f,0x54,0xc9,0x59,0x0f,0x32,0xa9,0x91,0x1f,0x16,0xfa,0xe8,0x7e,0x0a,0x2f,
		0x52},
		/*input_data*/
		65*sizeof(uint8_t),
		/*SHA512 Expected output data*/
		{0x2BCF565C,0xD8C28D9F,0xDDEB3EA9,0x60853E68,
         0x68EF95D3,0x2FCEC94A,0xB23134FA,0x1B7DE628,
         0x97CDA362,0xF34DDB9C,0x0ED22CF5,0x8410253C,
         0x2B2D0FC8,0xBA7A09A6,0x105D2E0D,0xDEA30C98},
		/*SHA512 expected dataSize*/
		CRYS_HASH_SHA512_DIGEST_SIZE_IN_BYTES*sizeof(uint8_t),
	},
};




