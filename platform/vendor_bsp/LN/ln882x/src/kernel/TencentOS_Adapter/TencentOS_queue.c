/**
 * @file freertos_queue.c
 * @author LightningSemi WLAN Team
 */
#include "./TencentOS_Adapter/TencentOS_queue.h"
#include "./TencentOS_Adapter/TencentOS_time.h"
#include "./TencentOS_Adapter/TencentOS_debug.h"

/*----------------------------------Queue--------------------------------------------*/
OS_Status OS_QueueCreate(OS_Queue_t *queue, uint32_t queueLen, uint32_t itemSize)
{
    //TODO:check handle is null!
    //TODO:check queueLen > 0,itemSize > 0!

    size_t q_size_in_bytes = (size_t)(queueLen * itemSize);

    if(NULL == (queue->handle = OS_Malloc(sizeof(k_mail_q_t)))){
        OS_ERR("err:os queue handle malloc failed.\r\n");
        return OS_FAIL;
    }

    k_err_t err = tos_mail_q_create_dyn(queue->handle, queueLen, itemSize);

    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {        
        OS_Free(queue->handle); 
        queue->handle = NULL;

        OS_WARN("err:%d,OS_QueueCreate filed.\r\n", err);
        return OS_FAIL;
    }

}

OS_Status OS_QueueDelete(OS_Queue_t *queue)
{
    k_err_t err = tos_mail_q_destroy_dyn(queue->handle);
    
    if (err == K_ERR_NONE) {
        OS_Free(queue->handle); 
        queue->handle = NULL;

        return OS_OK;
	} else {
        OS_WARN("err:%d,OS_QueueDelete filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_QueueSend(OS_Queue_t *queue, const void *item, OS_Time_t waitMS)
{
    uint32_t size = queue->handle->ring_q.item_size;
    
    k_err_t err = tos_mail_q_post(queue->handle, (void *)item, size);

    if (err == K_ERR_NONE) {
        return OS_OK;
	} 
    else if(err == K_ERR_OBJ_PTR_NULL)
    {
        OS_WARN("err:%d,OS_QueueSend K_ERR_OBJ_PTR_NULL.\r\n", err);
        return OS_OK;
    }
    else if(err == K_ERR_RING_Q_FULL)
    {
        OS_WARN("err:%d,OS_QueueSend K_ERR_RING_Q_FULL.\r\n", err);
        return OS_FAIL;
    }
    else   
    {
        OS_WARN("err:%d,OS_QueueSend filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_QueueReceive(OS_Queue_t *queue, void *item, OS_Time_t waitMS)
{
    static uint32_t size = 0;
    size = queue->handle->ring_q.item_size;

    k_err_t err = tos_mail_q_pend(queue->handle, item, &size, OS_CalcWaitTicks(waitMS));
    
    if (err == K_ERR_NONE) {
        return OS_OK;
	} else if (err == K_ERR_PEND_TIMEOUT) {
        OS_WARN("err:%d,OS_QueueReceive timeout.\r\n", err);
        return OS_FAIL;    
    } else {
        OS_WARN("err:%d,OS_QueueReceive filed.\r\n", err);
        return OS_FAIL;
    }
}


/*----------------------------------MsgQueue--------------------------------------------*/
OS_Status OS_MsgQueueCreate(OS_MsgQueue_t *queue, uint32_t queueLen)
{
    //TODO:check handle is null!

    if(NULL == (queue->handle = OS_Malloc(sizeof(k_msg_q_t)))){
        OS_ERR("err:os msg queue handle malloc failed.\r\n");
        return OS_FAIL;
    }

    k_err_t err = tos_msg_q_create_dyn(queue->handle, queueLen);

    if (err == K_ERR_NONE) {
        return OS_OK;
	} else {
        OS_Free(queue->handle); 
        queue->handle = NULL;

        OS_WARN("err:%d,OS_MsgQueueCreate filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_MsgQueueDelete(OS_MsgQueue_t *queue)
{
    k_err_t err = tos_msg_q_destroy_dyn(queue->handle);

    if (err == K_ERR_NONE) {
        OS_Free(queue->handle); 
        queue->handle = NULL;

        return OS_OK;
	} else {
        OS_WARN("err:%d,OS_MsgQueueDelete filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_MsgQueueSend(OS_MsgQueue_t *queue, void *msg, OS_Time_t waitMS)
{
    k_err_t err = tos_msg_q_post(queue->handle, msg);

    if (err == K_ERR_NONE) {
        return OS_OK;
	} 
    else if(err == K_ERR_OBJ_PTR_NULL)
    {
//        OS_WARN("err:%d,OS_MsgQueueSend K_ERR_OBJ_PTR_NULL.\r\n", err);
        return OS_OK;
    }
    else if(err == K_ERR_RING_Q_FULL)
    {
//        OS_WARN("err:%d,OS_MsgQueueSend K_ERR_RING_Q_FULL.\r\n", err);
        return OS_FAIL;
    }
    else   
    {
        OS_WARN("err:%d,OS_MsgQueueSend filed.\r\n", err);
        return OS_FAIL;
    }
}

OS_Status OS_MsgQueueReceive(OS_MsgQueue_t *queue, void **msg, OS_Time_t waitMS)
{
    k_err_t err = tos_msg_q_pend(queue->handle, msg, OS_CalcWaitTicks(waitMS));
    
    if (err == K_ERR_NONE) {
        return OS_OK;
	} else if (err == K_ERR_PEND_TIMEOUT) {
//        OS_WARN("err:%d,OS_MsgQueueReceive timeout.\r\n", err);
        return OS_FAIL;    
    } else {
//        OS_WARN("err:%d,OS_MsgQueueReceive filed.\r\n", err);
        return OS_FAIL;
    }
}





