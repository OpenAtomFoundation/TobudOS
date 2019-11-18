#include "tos.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_WAIT      512
#define STK_SIZE_TASK_TRIGGER   512

k_stack_t stack_task_wait[STK_SIZE_TASK_WAIT];
k_stack_t stack_task_trigger[STK_SIZE_TASK_TRIGGER];

k_task_t task_wait;
k_task_t task_trigger;

k_completion_t completion;

void entry_task_wait(void *arg)
{
    printf("wait: I won't go further until someone do the trigger(make it 'complete')\n");
    tos_completion_pend(&completion);
    printf("wait: someone has made it complete, so I'm here\n");
}

void entry_task_trigger(void *arg)
{
    printf("trigger: I'm the one who make complete, anyone waitting for the complete won't go further until I do the trigger\n");
    tos_completion_post(&completion);
    printf("trigger: I have done the completion\n");
}

int main(void)
{
    board_init();
    tos_knl_init();
    tos_completion_create(&completion);
    (void)tos_task_create(&task_wait, "wait", entry_task_wait, NULL,
                            3, stack_task_wait, STK_SIZE_TASK_WAIT, 0);
    (void)tos_task_create(&task_trigger, "trigger", entry_task_trigger, NULL,
                            4, stack_task_trigger, STK_SIZE_TASK_TRIGGER, 0);
    tos_knl_start();
}
