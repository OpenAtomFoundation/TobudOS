/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <string.h>
#include "utils_ringbuff.h"

int ring_buff_init(sRingbuff* ring_buff, char* buff, uint32_t size )
{
	ring_buff->buffer     = buff;
    ring_buff->size       = size;
    ring_buff->readpoint  = 0;
    ring_buff->writepoint = 0;
	memset(ring_buff->buffer, 0, ring_buff->size);
	ring_buff->full = false;
	
    return RINGBUFF_OK;
}

int ring_buff_flush(sRingbuff* ring_buff)
{
    ring_buff->readpoint  = 0;
    ring_buff->writepoint = 0;
	memset(ring_buff->buffer, 0, ring_buff->size);
	ring_buff->full = false;
	
    return RINGBUFF_OK;
}

int ring_buff_push_data(sRingbuff* ring_buff, uint8_t *pData, int len)
{
	int i;
	
	if(len > ring_buff->size)
	{
		return RINGBUFF_TOO_SHORT;
	}

	for(i = 0; i < len; i++)
	{
		if(((ring_buff->writepoint + 1) % ring_buff->size) == ring_buff->readpoint)
	    {
	    	ring_buff->full = true;
	        return RINGBUFF_FULL;
	    }
	    else
	    {
	        if(ring_buff->writepoint < (ring_buff->size - 1)) 
	        {
				ring_buff->writepoint ++;
			}         
	        else
	        {
				ring_buff->writepoint = 0;
			}	            
			ring_buff->buffer[ring_buff->writepoint] = pData[i];
	    } 		
	}

	return RINGBUFF_OK;
}

int ring_buff_pop_data(sRingbuff* ring_buff, uint8_t *pData, int len)
{
	int i;

	if(len > ring_buff->size)
	{
		 return RINGBUFF_TOO_SHORT;
	}

	for(i = 0; i < len; i++)
	{
		if(ring_buff->writepoint == ring_buff->readpoint)
		{
			break;
		}
		else
		{
			if(ring_buff->readpoint == (ring_buff->size - 1))
			{
				ring_buff->readpoint = 0;				
			}

			else
			{
				ring_buff->readpoint++;
			}
			pData[i] = ring_buff->buffer[ring_buff->readpoint];
		}
	}
	
	return i;
}
