#ifndef  __LN_KV_API_H__
#define  __LN_KV_API_H__

#include "ln_types.h"
#include "ln_kv_err.h"
#include "ln_kv_api.h"

kv_err_t    ln_kv_port_init(uint32_t flash_start, uint32_t flash_end);
kv_err_t    ln_kv_deinit(void);
kv_err_t    ln_kv_set(const char *key, const void *value, size_t v_len);
kv_err_t    ln_kv_get(const char *key, void *value_buf, size_t value_buf_size, size_t *v_len);
int         ln_kv_has_key(const char *key);
kv_err_t    ln_kv_del(const char *key);

#endif /* __LN_KV_API_H__ */

