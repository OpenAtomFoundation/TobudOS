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

const hmacDataStuct hmacVectors[] = {
	{
		/*Test parameters*/
		{"HMAC-SHA1/Input_data_32/Key_32/"},
		/*Hash operation mode*/
		CRYS_HASH_SHA1_mode,
		/*key*/
		{0xf6,0x4a,0xb2,0x3f,0x7a,0x95,0x92,0x9e,0x9e,0xa6,0x24,0xd9,0x5a,0x68,0x36,0xd5,
		0x12,0x83,0x65,0x0b,0x67,0xdc,0x90,0x2e,0x53,0x5a,0x8d,0x0d,0x75,0xf0,0x1a,0xbc},
		/*keySize */
		32*sizeof(uint8_t),
		/*input_data*/
		{0x57,0x22,0xd9,0xe2,0xed,0x48,0xf4,0x25,0xa8,0xce,0x7f,0x70,0x3d,0xe6,0xef,0x37,
		 0xe7,0x32,0x71,0xc1,0x59,0xe4,0x7d,0x74,0xf8,0x24,0xae,0x53,0x47,0xb2,0xd1,0xc5},
		/*input_dataSize*/
		32*sizeof(uint8_t),
		/*TST_SHA1_Expected_output_data*/
		{0x6568524f,0x6783ca5f,0x8f7a8e8e,0x139d0804,0x167b05f2}, 
		/*TST_SHA1_Expected_output_dataSize*/
		CRYS_HASH_SHA1_DIGEST_SIZE_IN_BYTES*sizeof(uint8_t), 
	},
	{
		/*Test parameters*/
		{"HMAC-SHA256/Input_data_65/Key_65/"},
		/*Hash operation mode*/
		CRYS_HASH_SHA256_mode,
		/*key*/
		{0x15,0xb2,0x9a,0xd8,0xae,0x2a,0xad,0x73,0xa7,0x26,0x43,0x50,0x70,0xe8,0xe9,0xda,
		 0x9b,0x47,0x69,0xc3,0xe3,0xa4,0xee,0x99,0x6e,0x20,0x6a,0x9b,0x4f,0x0c,0x35,0xca,
		 0x4f,0xa2,0xf7,0x43,0xed,0xf2,0xc7,0xcb,0xa3,0x1e,0x94,0xac,0x6b,0xca,0xc4,0xc0,
		 0x82,0xcf,0x1c,0xcb,0x6c,0x2f,0xe0,0x0d,0x38,0x4e,0x3b,0x18,0x05,0x5f,0xe0,0xe0},
		/*keySize */
		64*sizeof(uint8_t),
		/*input_data*/
		{0x99,0xfd,0x18,0xa3,0x5d,0x50,0x81,0x84,0xa6,0xf3,0x61,0xc6,0x7c,0xd9,0xb1,0x0b,
		 0x4c,0xd1,0xd8,0xb2,0x46,0x57,0x2a,0x4d,0x03,0xb0,0xae,0x55,0x6b,0x36,0x24,0x1d,
		 0xd6,0xf0,0x46,0x05,0x71,0x65,0x4f,0xf0,0xe4,0xb2,0xba,0xf8,0x31,0xdb,0x4c,0x60,
		 0xdf,0x5f,0x54,0xc9,0x59,0x0f,0x32,0xa9,0x91,0x1f,0x16,0xfa,0xe8,0x7e,0x0a,0x2f,
		 0x52},
		/*input_dataSize*/
		65*sizeof(uint8_t),
		/*TST_SHA256_Expected_output_data*/
		{0xE0903CC8,0x24C89469,0x71B12528,0x6DEFD88C,
		 0xF662C7FC,0x971C4DD1,0x5755CB85,0x8E72FD6F}, 
		/*TST_SAH256_expected_dataSize*/
		CRYS_HASH_SHA256_DIGEST_SIZE_IN_BYTES*sizeof(uint8_t),  


	}


};
