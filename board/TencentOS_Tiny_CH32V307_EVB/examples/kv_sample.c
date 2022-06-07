#include "tos_kv.h"

extern kv_flash_drv_t ch32v30x_spiflash_drv;
extern kv_flash_prop_t ch32v30x_spiflash_prop;

//#define KV_SAMPLE_DEBUG

void disp_value(uint8_t *value, uint32_t value_len)
{
    int i = 0;

    printf("value_len: %d\r\n", value_len);

    printf("\"");
    for (i = 0; i < value_len; ++i) {
        printf("%c", value[i]);
    }
    printf("\"\r\n\n");
}

void kv_test(void)
{
    int has_key;
    kv_err_t err;
    uint8_t value_buf[40];
    uint32_t value_len;

    err = tos_kv_init(0, 2 * 4096, &ch32v30x_spiflash_drv, &ch32v30x_spiflash_prop);
    printf("kv init, rc: %d\r\n", err);

    has_key = tos_kv_has_key("key00");
    printf("has key[%s] ? %s\r\n\n", "key00", has_key ? "True" : "False");

    err = tos_kv_set("key00", "value_00", strlen("value_00"));
    printf("kv set(%s), rc: %d\r\n\n", "key00", err);

    has_key = tos_kv_has_key("key00");
    printf("has key[%s] ? %s\r\n\n", "key00", has_key ? "True" : "False");

    if (err == KV_ERR_NONE) {
        err = tos_kv_get("key00", value_buf, sizeof(value_buf), &value_len);
        printf("kv get(%s), rc: %d\r\n\n", "key00", err);

        if (err == KV_ERR_NONE) {
            disp_value(value_buf, value_len);
        }

#ifdef KV_SAMPLE_DEBUG
        tos_kv_walkthru();
#endif
    }

    err = tos_kv_set("key00", "value_xx", strlen("value_xx"));
    printf("kv set(%s), rc: %d\r\n\n", "key00", err);

    if (err == KV_ERR_NONE) {
        err = tos_kv_get("key00", value_buf, sizeof(value_buf), &value_len);
        printf("kv get(%s), rc: %d\r\n\n", "key00", err);

        if (err == KV_ERR_NONE) {
            disp_value(value_buf, value_len);
        }
    }

#ifdef KV_SAMPLE_DEBUG
    tos_kv_walkthru();
#endif

    err = tos_kv_del("key00");
    printf("kv del(%s), rc: %d\r\n\n", "key00", err);

    has_key = tos_kv_has_key("key00");
    printf("has key[%s] ? %s\r\n\n", "key00", has_key ? "True" : "False");

#ifdef KV_SAMPLE_DEBUG
    tos_kv_walkthru();
#endif
}

void application_entry(void *arg)
{
    kv_test();
}

