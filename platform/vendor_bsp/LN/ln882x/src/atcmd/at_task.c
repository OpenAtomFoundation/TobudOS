#include <string.h>
#include "proj_config.h"
#include "ln88xx.h"
#include "utils/debug/art_assert.h"
#include "utils/debug/log.h"

#include "console/console.h"
#include "atcmd/at_task.h"
#include "atcmd/at_parser.h"
#include "atcmd/at_cmd_wifi.h"


#define AT_TASK_STACK_SIZE          5*256
#define AT_MSG_QUEUE_SIZE           5

OS_Queue_t g_at_msg_queue;
OS_Thread_t g_at_thread;
#if (TCP_TEST_DATA_FROM_CONSOLE==1)
uint8_t   data_bufer[1024];
uint16_t   data_bufer_len=1023;
uint32_t   AT_COMMAN_CNT = 0;
#endif

void at_task(void *params)
{
    AT_MSG_T at_msg;
    AT_CMD_TYPE atcmd_type;

    while(1)
    {
        if(OS_QueueReceive(&g_at_msg_queue, &at_msg, OS_WAIT_FOREVER) == OS_OK){
			atcmd_type = at_check_cmdtype(at_msg.pBuffer);
#if (TCP_TEST_DATA_FROM_CONSOLE==1)		
            data_bufer_len = (at_strlen(at_msg.pBuffer)) & 0x3ff;
            memcpy(data_bufer, at_msg.pBuffer, data_bufer_len );
#endif
            //Remove CRLF in at_cmd at the beginning
            str_remove_cr_lf(at_msg.pBuffer);
            if(atcmd_type == AT_SHORT_CMD){
                at_parse_short_cmd(at_msg.pBuffer);
            }else if(atcmd_type == AT_PLUS_CMD){
                at_parse_plus_cmd(at_msg.pBuffer);
            }else{
#if (TCP_TEST_DATA_FROM_CONSOLE==1)
                if((at_strlen(at_msg.pBuffer)) < 1024)
                	at_printf("\r\nInvalid AT command count (%d)\r\n", AT_COMMAN_CNT++);
#else
				at_printf("\r\nInvalid AT command(%s)\r\n", at_msg.pBuffer);
#endif
            }
            OS_SemaphoreRelease(at_msg.sem);
        }
    }
}


void at_init(void)
{
    console_init();
    at_cmd_init();


    if(OS_QueueCreate(&g_at_msg_queue, AT_MSG_QUEUE_SIZE, sizeof(AT_MSG_T)) != OS_OK) {
        LOG(LOG_LVL_ERROR, "OS_MsgQueueCreate g_at_msg_queue fail.\n");
        return;
    }
    if(OS_ThreadCreate(&g_at_thread, "at_task", at_task, NULL, OS_PRIORITY_NORMAL, AT_TASK_STACK_SIZE) != OS_OK) {
        LOG(LOG_LVL_ERROR, "OS_ThreadCreate at_task fail.\n");
        return;
    }
}

