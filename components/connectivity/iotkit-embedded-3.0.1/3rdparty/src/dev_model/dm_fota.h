/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_FOTA_H_
#define _DM_FOTA_H_

typedef struct {
    int is_report_new_config;
} dm_fota_ctx_t;

int dm_fota_init(void);
int dm_fota_deinit(void);
int dm_fota_perform_sync(_OU_ char *output, _IN_ int output_len);
int dm_fota_status_check(void);
int dm_fota_request_image(_IN_ const char *version, _IN_ int buffer_len);

#endif
