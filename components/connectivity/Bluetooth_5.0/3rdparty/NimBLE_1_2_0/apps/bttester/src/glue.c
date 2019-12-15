/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "syscfg/syscfg.h"

#if !MYNEWT_VAL(BLE_MESH)
#include <assert.h>
#include <string.h>
#include "os/os.h"
#include "os/os_mbuf.h"
#include "glue.h"


#define ASSERT_NOT_CHAIN(om) assert(SLIST_NEXT(om, om_next) == NULL)

const char *bt_hex(const void *buf, size_t len)
{
	static const char hex[] = "0123456789abcdef";
	static char hexbufs[4][137];
	static u8_t curbuf;
	const u8_t *b = buf;
	char *str;
	int i;

	str = hexbufs[curbuf++];
	curbuf %= ARRAY_SIZE(hexbufs);

	len = min(len, (sizeof(hexbufs[0]) - 1) / 2);

	for (i = 0; i < len; i++) {
		str[i * 2] = hex[b[i] >> 4];
		str[i * 2 + 1] = hex[b[i] & 0xf];
	}

	str[i * 2] = '\0';

	return str;
}

struct os_mbuf * NET_BUF_SIMPLE(uint16_t size)
{
	struct os_mbuf *buf;

	buf = os_msys_get(size, 0);
	assert(buf);

	return buf;
}

/* This is by purpose */
void net_buf_simple_init(struct os_mbuf *buf,
			 size_t reserve_head)
{
	/* This is called in Zephyr after init.
	 * Note in Mynewt case we don't care abour reserved head*/
	buf->om_data = &buf->om_databuf[buf->om_pkthdr_len] + reserve_head;
	buf->om_len = 0;
}

void
net_buf_simple_add_le16(struct os_mbuf *om, uint16_t val)
{
	val = htole16(val);
	os_mbuf_append(om, &val, sizeof(val));
	ASSERT_NOT_CHAIN(om);
}

void
net_buf_simple_add_be16(struct os_mbuf *om, uint16_t val)
{
	val = htobe16(val);
	os_mbuf_append(om, &val, sizeof(val));
	ASSERT_NOT_CHAIN(om);
}

void
net_buf_simple_add_be32(struct os_mbuf *om, uint32_t val)
{
	val = htobe32(val);
	os_mbuf_append(om, &val, sizeof(val));
	ASSERT_NOT_CHAIN(om);
}

void
net_buf_simple_add_u8(struct os_mbuf *om, uint8_t val)
{
	os_mbuf_append(om, &val, 1);
	ASSERT_NOT_CHAIN(om);
}

void*
net_buf_simple_add(struct os_mbuf *om, uint8_t len)
{
	void * tmp;

	tmp = os_mbuf_extend(om, len);
	ASSERT_NOT_CHAIN(om);

	return tmp;
}

uint8_t *
net_buf_simple_push(struct os_mbuf *om, uint8_t len)
{
	uint8_t headroom = om->om_data - &om->om_databuf[om->om_pkthdr_len];

	assert(headroom >= len);
	om->om_data -= len;
	om->om_len += len;

	return om->om_data;
}
#endif
