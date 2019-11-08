/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _ALCS_LOCALSETUP_H_
#define _ALCS_LOCALSETUP_H_

void alcs_localsetup_init(void *adapter_handle, void* coap_handler, char *product_key,char *device_name);
void alcs_localsetup_add_sub_device (void *adapter_handle,char *product_key,char *device_name);
void alcs_localsetup_deinit(void *handle);

#endif
