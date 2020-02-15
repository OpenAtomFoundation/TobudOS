#include "tos_kv.h"

#include "cmsis_os.h"

#define TASK_STK_SIZE          2048
void task(void *arg);
osThreadDef(task, osPriorityNormal, 1, TASK_STK_SIZE);

// #define USING_ONCHIP_FLASH

#ifdef USING_ONCHIP_FLASH
extern kv_flash_drv_t stm32l4_norflash_onchip_drv;
extern kv_flash_prop_t stm32l4_norflash_onchip_prop;
#else
extern kv_flash_drv_t stm32l4_qspiflash_drv;
extern kv_flash_prop_t stm32l4_qspiflash_prop;
#endif

void disp_value(uint8_t *value, uint32_t value_len)
{
    int i = 0;

    printf("value_len: %d\n", value_len);

    printf("\"");
    for (i = 0; i < value_len; ++i) {
        printf("%c", value[i]);
    }
    printf("\"\n\n");
}

void task(void *arg)
{
    int has_key;
    kv_err_t err;
    uint8_t value_buf[40];
    uint32_t value_len;

#ifndef USING_ONCHIP_FLASH  // use qspiflash
    extern void MX_QUADSPI_Init(void);
    MX_QUADSPI_Init();
#endif

#ifdef USING_ONCHIP_FLASH
    err = tos_kv_init(&stm32l4_norflash_onchip_drv, &stm32l4_norflash_onchip_prop);
    printf("kv init, rc: %d\n", err);
#else
    err = tos_kv_init(&stm32l4_qspiflash_drv, &stm32l4_qspiflash_prop);
    printf("kv init, rc: %d\n", err);
#endif

    has_key = tos_kv_has_key("key00");
    printf("has key[%s] ? %s\n\n", "key00", has_key ? "True" : "False");

    err = tos_kv_set("key00", "value_00", strlen("value_00"));
    printf("kv set(%s), rc: %d\n\n", "key00", err);

    has_key = tos_kv_has_key("key00");
    printf("has key[%s] ? %s\n\n", "key00", has_key ? "True" : "False");

    if (err == KV_ERR_NONE) {
        err = tos_kv_get("key00", value_buf, sizeof(value_buf), &value_len);
        printf("kv get(%s), rc: %d\n\n", "key00", err);

        if (err == KV_ERR_NONE) {
            disp_value(value_buf, value_len);
        }

        tos_kv_walkthru();
    }

    err = tos_kv_set("key00", "value_xx", strlen("value_xx"));
    printf("kv set(%s), rc: %d\n\n", "key00", err);

    if (err == KV_ERR_NONE) {
        err = tos_kv_get("key00", value_buf, sizeof(value_buf), &value_len);
        printf("kv get(%s), rc: %d\n\n", "key00", err);

        if (err == KV_ERR_NONE) {
            disp_value(value_buf, value_len);
        }

        tos_kv_walkthru();
    }

    err = tos_kv_del("key00");
    printf("kv del(%s), rc: %d\n\n", "key00", err);

    has_key = tos_kv_has_key("key00");
    printf("has key[%s] ? %s\n\n", "key00", has_key ? "True" : "False");

    tos_kv_walkthru();
}

void application_entry(void *arg)
{
    osThreadCreate(osThread(task), NULL); // Create task1
}

