#include "string.h"
#include "ln88xx.h"
#include "hal/hal_sdio.h"
#include "hal/hal_syscon.h"
#include "utils/debug/log.h"

static hal_sdio_ctrl_t g_hal_sdio_ctrl __attribute__((section("wlan_shared_mem"))) = {0,};
static hal_sdio_ctrl_t *hal_sdio_ctrl_get_handle(void)
{
    return &g_hal_sdio_ctrl;
}

/*
 * ruturn the value of powers 2:
 * eg:  1 --> 4
 *      3 --> 4
 *      33 --> 64
 *      511 --> 512
 *      513 --> 1024
 *      1025 --> 1536
 * max value is 1536
 */
static int power_of_2(int v)
{
    int res = 1;
    if(v < SDIO_FUNC_BLOCK_SIZE){
	    while (res < v)
	        res <<= 1;
	    if(res < 4)
	        res = 4;
    }else if(v <= 3*SDIO_FUNC_BLOCK_SIZE){
    	if(v % SDIO_FUNC_BLOCK_SIZE)
            res = (v / SDIO_FUNC_BLOCK_SIZE + 1) * SDIO_FUNC_BLOCK_SIZE;
        else
    		res = v;
    }else{
        LOG(LOG_LVL_ERROR, "[%s, %d]SDIO XFER data length is overfllow(len=%d), and truncated.\r\n", __func__, __LINE__, v);
        res = 3*SDIO_FUNC_BLOCK_SIZE;
    }
    
    return res;
}

static void hal_sdio_receive_from_host(void)
{
    uint8_t *next_rcv = NULL;
    hal_sdio_ctrl_t *hal_sdio_ctrl = hal_sdio_ctrl_get_handle();
    host_ops_t      *host_ops = &(hal_sdio_ctrl->host_ops);
    uint8_t *src = (uint8_t *)ll_sdio_receive_from_host_buffer_get();
    uint16_t count = ll_sdio_receive_from_host_buffer_size_get();

    if(src){
        //LOG(LOG_LVL_ERROR, "[%s, %d]count=%d, real_count=%d, src=%p\r\n", __func__, __LINE__, count, src[0] | (src[1]&0x0F)<<8, src);
    	if((src[0] | (src[1]&0x0F)<<8) <= count && count <= 3*SDIO_FUNC_BLOCK_SIZE){
            if(host_ops->receive_from_host_cb && (src == hal_sdio_ctrl->rx_buffer)){
                host_ops->receive_from_host_cb(src, count, true);
            }else if(host_ops->receive_from_host_cb){
                host_ops->receive_from_host_cb(src, count, false);
            }
            
        }else{
        	LOG(LOG_LVL_ERROR, "[%s, %d]SDIO receive fail. count=%d, real_count=%d(free addr=%p)\r\n", __func__, __LINE__, count, src[0] | (src[1]&0x0F)<<8, src);
            //Continue to use this buffer
            ll_sdio_receive_from_host_buffer_set(src);
            return;
        }
    }
    
    //Malloc a buffer for next host rx
    if(host_ops->get_buffer_for_receive){
        next_rcv = host_ops->get_buffer_for_receive();
    }
    if(next_rcv){
        ll_sdio_receive_from_host_buffer_set(next_rcv);
    }else{
        next_rcv = hal_sdio_ctrl->rx_buffer;
        ll_sdio_receive_from_host_buffer_set(next_rcv);
        
    }
}
void hal_sdio_xfer_to_host(uint8_t *addr, int len)
{
    uint32_t size = 0;
    hal_sdio_ctrl_t *hal_sdio_ctrl = hal_sdio_ctrl_get_handle();

    OS_MutexLock(&hal_sdio_ctrl->lock, OS_WAIT_FOREVER);
    if(hal_sdio_ctrl->fn1_en){
        //hexdump(LOG_LVL_ERROR, "To HOST", addr, len);
        size = power_of_2(len);
        ll_sdio_xfer_to_host_buffer_set(addr);
        ll_sdio_xfer_to_host_buffer_size_set(size);
        ll_sdio_triggle_data1_interrupt_to_host();
        OS_SemaphoreWait(&hal_sdio_ctrl->sdio_sem, OS_WAIT_FOREVER);
    }
    OS_MutexUnlock(&hal_sdio_ctrl->lock);
}

void hal_sdio_init(sdio_config_t *config, host_ops_t *sdio_ops)
{
    hal_sdio_ctrl_t *hal_sdio_ctrl = hal_sdio_ctrl_get_handle();
    if (OS_OK != OS_SemaphoreCreateBinary(&hal_sdio_ctrl->sdio_sem)){
		LOG(LOG_LVL_ERROR, "[%s, %d]OS_SemaphoreCreateBinary hal_sdio_ctrl->lock fail.\r\n", __func__, __LINE__);
        return;
    }
    OS_MutexCreate(&hal_sdio_ctrl->lock);
    config->cis_fn0_base = hal_sdio_ctrl->sdio_cis_fn0;
    config->cis_fn1_base = hal_sdio_ctrl->sdio_cis_fn1;
    ll_sdio_init(config);
    hal_sdio_ctrl->host_ops.receive_from_host_cb = sdio_ops->receive_from_host_cb;
    hal_sdio_ctrl->host_ops.get_buffer_for_receive = sdio_ops->get_buffer_for_receive;
}

void hal_sdio_reset(void){
    hal_sdio_ctrl_t *hal_sdio_ctrl = hal_sdio_ctrl_get_handle();
    host_ops_t      *host_ops = &(hal_sdio_ctrl->host_ops);
    uint8_t *buffer = NULL;

    //Malloc a buffer for next host rx
    if(host_ops->get_buffer_for_receive){
        buffer = host_ops->get_buffer_for_receive();
    }
    ll_sdio_receive_from_host_buffer_set(buffer);
    
}
void SDIO_FUN1_IRQHandler(void)
{
    hal_sdio_ctrl_t *hal_sdio_ctrl = hal_sdio_ctrl_get_handle();
    uint32_t int_status = ll_sdio_get_interrupt_status() & 0xfff;
    
    //LOG(LOG_LVL_ERROR, "[%s, %d]int_status=0x%08X\r\n", __func__, __LINE__, int_status);
    if(int_status & FN1_WRITE_OVER_INTERRPT) { //HOST write over interrupt
        hal_sdio_receive_from_host();
        //clear this IRQ bit
        ll_sdio_set_interrupt_status(int_status&(~FN1_WRITE_OVER_INTERRPT));
        //clear busy SD
        ll_sdio_clear_busy_sd();
    }
    if(int_status & FN1_READ_OVER_INTERRPT) {
        //clear this IRQ bit
        ll_sdio_set_interrupt_status(int_status&(~FN1_READ_OVER_INTERRPT));
        if(hal_sdio_ctrl->fn1_en){
            OS_SemaphoreRelease(&hal_sdio_ctrl->sdio_sem);
        }
    }
    if(int_status & READ_ERROR_FN1_INTERRPT) {
        ll_sdio_set_interrupt_status(int_status&(~READ_ERROR_FN1_INTERRPT));
    }
    if(int_status & WRITE_ERROR_FN1_INTERRPT) {
        ll_sdio_set_interrupt_status(int_status&(~WRITE_ERROR_FN1_INTERRPT));
    }
    if(int_status & WRITE_ABORT_FN1_INTERRPT) {
        ll_sdio_set_interrupt_status(int_status&(~WRITE_ABORT_FN1_INTERRPT));
    }
    if(int_status & RESET_FN1_INTERRPT) {
    	hal_sdio_ctrl->fn1_en = false;
		ll_sdio_set_interrupt_status(int_status&(~RESET_FN1_INTERRPT));
    }
    if(int_status & FN1_ENABLE_INTERRPT) {
        hal_sdio_ctrl->fn1_en = true;
        ll_sdio_set_interrupt_status(int_status&(~FN1_ENABLE_INTERRPT));
    }
    if(int_status & FN1_STATUS_PCRRT_INTERRPT) {
        ll_sdio_set_interrupt_status(int_status&(~FN1_STATUS_PCRRT_INTERRPT));
    }
    if(int_status & FN1_STATUS_PCWRT_INTERRPT) {
        ll_sdio_set_interrupt_status(int_status&(~FN1_STATUS_PCWRT_INTERRPT));
    }
    if(int_status & FN1_RTC_SET_INTERRPT) {
        ll_sdio_set_interrupt_status(int_status&(~FN1_RTC_SET_INTERRPT));
    }
    if(int_status & FN1_CLINTRD_INTERRPT) {
        ll_sdio_set_interrupt_status(int_status&(~FN1_CLINTRD_INTERRPT));
    }
    if(int_status & FN1_INT_EN_UP_INTERRPT) {
        ll_sdio_set_interrupt_status(int_status&(~FN1_INT_EN_UP_INTERRPT));
    }
    if(int_status & FN1_M2S_INT_INTERRPT) {
        ll_sdio_set_interrupt_status(int_status&(~FN1_M2S_INT_INTERRPT));
    }
}

