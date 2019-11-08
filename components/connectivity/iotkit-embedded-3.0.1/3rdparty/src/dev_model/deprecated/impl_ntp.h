/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifndef NTP_H
#define NTP_H

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif


#define TOPIC_NTP                       "/ext/ntp/%s/%s/request"
#define TOPIC_NTP_REPLY                 "/ext/ntp/%s/%s/response"

#define NTP_TIME_STR_MAX_LEN            (20)

int linkkit_ntp_time_request(void (*)(const char *ntp_offset_time_ms));

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
