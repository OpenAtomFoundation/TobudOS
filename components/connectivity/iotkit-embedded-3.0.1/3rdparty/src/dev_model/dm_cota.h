/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_COTA_H_
#define _DM_COTA_H_

typedef struct {
    int is_report_new_config;
} dm_cota_ctx_t;

int dm_cota_init(void);
int dm_cota_deinit(void);
int dm_cota_perform_sync(_OU_ char *output, _IN_ int output_len);
int dm_cota_get_config(const char *config_scope, const char *get_type, const char *attribute_keys);
int dm_cota_status_check(void);

#endif