/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _HTTP2_CONFIG_H
#define _HTTP2_CONFIG_H

/* maximum packet len in one http2 frame */
#ifndef FS_UPLOAD_PACKET_LEN
#define FS_UPLOAD_PACKET_LEN        10240                   /* must < 16384 */
#endif

/* maximum content len of the http2 request */
#ifndef FS_UPLOAD_PART_LEN
#define FS_UPLOAD_PART_LEN          (1024 * 1024 * 4)       /* 100KB ~ 100MB */
#endif


#endif /* #ifdef _HTTP2_CONFIG_H */

