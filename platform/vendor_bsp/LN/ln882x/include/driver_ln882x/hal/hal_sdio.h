#ifndef __HAL_SDIO_H__
#define __HAL_SDIO_H__

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"
#include "osal/osal.h"
#include "ll/ll_sdio.h"

typedef int (*receive_from_host_cb_t)(uint8_t *addr, int len, bool mmcopy);
typedef uint8_t *(*get_buffer_for_receive_from_host_cb_t)(void);
typedef struct
{
    receive_from_host_cb_t                      receive_from_host_cb;
    get_buffer_for_receive_from_host_cb_t       get_buffer_for_receive;
}host_ops_t;

typedef struct
{
    OS_Mutex_t          lock;
    OS_Semaphore_t      sdio_sem;
    bool                fn1_en;
    host_ops_t          host_ops;
    uint8_t             sdio_cis_fn0[128];
    uint8_t             sdio_cis_fn1[128];
    uint8_t             rx_buffer[1600];
}hal_sdio_ctrl_t;

void hal_sdio_xfer_to_host(uint8_t *addr, int len);
void hal_sdio_init(sdio_config_t *config, host_ops_t *sdio_ops);
void hal_sdio_reset(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __HAL_SDIO_H__

