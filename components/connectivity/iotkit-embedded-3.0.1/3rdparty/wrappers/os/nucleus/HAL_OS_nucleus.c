#include "infra_types.h"
#include "infra_defs.h"

#include "kal_public_defs.h"
#include "kal_public_api.h"
#include "kal_general_types.h"
#include "soc_api.h"

#define HAL_MEM_SIZE             (30 * 1024)
static KAL_ADM_ID hal_heap_adm_id =NULL;

#if defined(WIN32)
static kal_uint8 my_heap[HAL_MEM_SIZE];
#else
static __align(32) kal_uint8 hal_heap[HAL_MEM_SIZE];
#endif

void hal_mem_init(void)
{
	hal_heap_adm_id = kal_adm_create2(hal_heap, HAL_MEM_SIZE, NULL, KAL_FALSE, 0);
}

uint64_t HAL_UptimeMs(void)
{
    unsigned int tick, cur_time;
    kal_get_time(&tick);
    cur_time = kal_ticks_to_milli_secs(tick);
    return (uint64_t)cur_time;
}

int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

void HAL_Printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

void *HAL_Malloc(uint32_t size)
{
    if(hal_heap_adm_id == NULL)
    {
        hal_mem_init();
        if(hal_heap_adm_id ==NULL){
            return NULL;
        }
    }
	return (void *)kal_adm_alloc(hal_heap_adm_id, size);
}

void HAL_Free(void *ptr)
{
    if(hal_heap_adm_id ==NULL || ptr ==NULL){
        return;
    }    
	kal_adm_free(hal_heap_adm_id, ptr);
}

int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN])
{
	return (int)1;
}

int HAL_GetFirmwareVersion(char *version)
{
	return (int)1;
}

int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN])
{
	return (int)1;
}

int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN])
{
	return (int)1;
}