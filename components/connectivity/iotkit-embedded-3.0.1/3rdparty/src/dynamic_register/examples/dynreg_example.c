#include <stdio.h>
#include <string.h>
#include "infra_types.h"
#include "infra_defs.h"
#include "dynreg_api.h"

void HAL_Printf(const char *fmt, ...);
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN]);
int HAL_GetProductSecret(char *product_secret);
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN]);

int main(int argc, char *argv[])
{
    int32_t res = 0;
    iotx_dev_meta_info_t meta;
    iotx_http_region_types_t region = IOTX_HTTP_REGION_SHANGHAI;
    HAL_Printf("dynreg example\n");

    memset(&meta,0,sizeof(iotx_dev_meta_info_t));
    HAL_GetProductKey(meta.product_key);
    HAL_GetProductSecret(meta.product_secret);
    HAL_GetDeviceName(meta.device_name);

    res = IOT_Dynamic_Register(region, &meta);
    if (res < 0) {
        HAL_Printf("IOT_Dynamic_Register failed\n");
        return -1;
    }

    HAL_Printf("\nDevice Secret: %s\n\n",meta.device_secret);

    return 0;
}

