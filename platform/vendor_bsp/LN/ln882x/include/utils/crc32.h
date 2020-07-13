
#ifndef ___CRC32_H__
#define ___CRC32_H__

#include <stdint.h>

typedef struct {
    uint32_t    crc;
} crc32_ctx_t;


void        ln_crc32_init(crc32_ctx_t *ctx);
void        ln_crc32_update(crc32_ctx_t *ctx, uint8_t *data, uint32_t len);
uint32_t    ln_crc32_final(crc32_ctx_t *ctx);
uint32_t    ln_crc32_signle_cal(uint8_t *ptr, int len);


#endif /* ___CRC32_H__ */


