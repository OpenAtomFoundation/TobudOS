/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Garmin Canada Inc. 2015
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *    1) Redistributions of source code must retain the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer.
 *
 *    2) Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 *    3) Neither the name of Garmin nor the names of its
 *       contributors may be used to endorse or promote products
 *       derived from this software without specific prior
 *       written permission.
 *
 * The following actions are prohibited:
 *
 *    1) Redistribution of source code containing the ANT+ Network
 *       Key. The ANT+ Network Key is available to ANT+ Adopters.
 *       Please refer to http://thisisant.com to become an ANT+
 *       Adopter and access the key. 
 *
 *    2) Reverse engineering, decompilation, and/or disassembly of
 *       software provided in binary form under this license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE HEREBY
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; DAMAGE TO ANY DEVICE, LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE. SOME STATES DO NOT ALLOW 
 * THE EXCLUSION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO THE
 * ABOVE LIMITATIONS MAY NOT APPLY TO YOU.
 *
 */
#ifndef __MESSAGE_CACHE_H__
#define __MESSAGE_CACHE_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t dst;
    uint8_t page;
    uint8_t data;
    uint8_t pay0;
    uint8_t pay1;
    uint8_t seq;
    uint8_t ticks;
} sf_message_t;

typedef struct
{
    uint8_t          front;
    uint8_t          back;
    uint8_t          count;
    uint8_t          size;
    sf_message_t * buffer;
} message_cache_t;


/**@brief Empty message cache.
 *
 * @param p_mc          Pointer to message cache struct
 */
void mc_clear(message_cache_t * p_mc);

/**@brief Add message to cache. Old messages are deleted to make room if full.
 *
 * @param p_mc          Pointer to message cache struct
 * @param p_message     Pointer to message to add
 */
void mc_add(message_cache_t * p_mc, uint8_t * p_message);

/**@brief Check if message is in cache.
 *
 * @param p_mc          Pointer to message cache struct
 * @param p_message     Pointer to message to check
 * @return True if message is already in cache
 */
bool mc_is_in_cache(message_cache_t * p_mc, uint8_t * p_message);

/**@brief Purge messages older than given value.
 *
 * @param p_mc          Pointer to message cache struct
 * @param max_ticks     Maximum number of ticks for messages to stay in cache
 */
void mc_cleanup(message_cache_t * p_mc, uint8_t max_ticks);


#ifdef __cplusplus
}
#endif

#endif
