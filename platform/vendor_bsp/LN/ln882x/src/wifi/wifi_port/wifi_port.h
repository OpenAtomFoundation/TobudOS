#ifndef  __WIFI_PORT_H__
#define  __WIFI_PORT_H__

#include "ln_types.h"

int    kv_has_psk_info_list(void);
int    kv_set_psk_info_list(uint8_t *v_buf, uint32_t buf_len);
int    kv_get_psk_info_list(uint8_t *v_buf, uint32_t buf_len);

void  *mac_malloc(uint32_t size);
void   mac_free(void *ptr);


#endif /* __WIFI_PORT_H__ */

