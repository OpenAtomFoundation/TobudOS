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
#include <string.h>
#include "message_cache.h"
#include "nordic_common.h"

static bool mc_pop(message_cache_t * p_mc);
static bool mc_compare(uint8_t * p_msg, sf_message_t * p_opt_msg);

void mc_clear(message_cache_t * p_mc)
{
    if (p_mc == NULL)
    {
        return;
    }

    p_mc->back  = 0;
    p_mc->front = 0;
    p_mc->count = 0;
}

void mc_add(message_cache_t * p_mc, uint8_t * p_message)
{
    // If the cache is full, delete an old element to make room for the new one
    if (p_mc->count >= p_mc->size)
    {
        UNUSED_PARAMETER(mc_pop(p_mc));
    }

    // Copy new element into the queue
    // Convert from ANT message format into optimized struct
    p_mc->buffer[p_mc->back].page  = p_message[0];
    p_mc->buffer[p_mc->back].dst   = p_message[1];
    p_mc->buffer[p_mc->back].seq   = p_message[6];
    p_mc->buffer[p_mc->back].data  = p_message[7];
    p_mc->buffer[p_mc->back].pay0  = p_message[5];
    p_mc->buffer[p_mc->back].pay1  = p_message[4];
    p_mc->buffer[p_mc->back].ticks = 0;

    p_mc->back = (p_mc->back == (p_mc->size - 1)) ? 0 : p_mc->back + 1;
    p_mc->count++;
}


bool mc_is_in_cache(message_cache_t * p_mc, uint8_t * p_message)
{
    if (p_mc->count == 0)
    {
        return false;
    }

    uint8_t iter = p_mc->front;

    for (int i = 0; i < p_mc->count; i++)
    {
        if (mc_compare(p_message, &p_mc->buffer[iter]))
        {
            return true;
        }

        iter = (iter == (p_mc->size - 1)) ? 0 : iter + 1;
    }
    return false;
}


void mc_cleanup(message_cache_t * p_mc, uint8_t max_ticks)
{
    if (p_mc->count == 0)
    {
        return;
    }

    uint8_t size        = p_mc->count;
    uint8_t iter        = p_mc->front;
    bool    msg_expired = false;

    for (int i = 0; i < size; i++)
    {
        p_mc->buffer[iter].ticks++;

        if (p_mc->buffer[iter].ticks >= max_ticks)
        {
            msg_expired = true;
        }

        iter = (iter == (p_mc->size - 1)) ? 0 : iter + 1;

        if (msg_expired)
        {
            p_mc->front = iter;
            p_mc->count--;
        }
    }
}


/**@brief Pop message from cache
 *
 * @param p_mc          Pointer to message cache struct
 *
 * @return  true if able to pop item
 */
static bool mc_pop(message_cache_t * p_mc)
{
    if (p_mc->count == 0)
    {
        return false;
    }

    p_mc->front = (p_mc->front == (p_mc->size - 1)) ? 0 : p_mc->front + 1;
    p_mc->count--;
    return true;
}


/**@brief Compare a raw ANT message with optimized messagestruct
 * @param p_msg         Pointer to raw ANT message
 * @param p_opt_msg     Pointer to optimized message struct
 * @return true if the values match, false otherwise
 */
static bool mc_compare(uint8_t * p_msg, sf_message_t * p_opt_msg)
{
    bool result = true;

    if (p_opt_msg->page != p_msg[0])
    {
        result = false;
    }

    if (p_opt_msg->dst != p_msg[1])
    {
        result = false;
    }

    if (p_opt_msg->seq != p_msg[6])
    {
        result = false;
    }

    if (p_opt_msg->data != p_msg[7])
    {
        result = false;
    }

    if (p_opt_msg->pay0 != p_msg[5])
    {
        result = false;
    }

    if (p_opt_msg->pay1 != p_msg[4])
    {
        result = false;
    }

    return result;
}

