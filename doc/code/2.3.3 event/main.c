#include "tos_k.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_LISTENER      512
#define STK_SIZE_TASK_TRIGGER       512

k_stack_t stack_task_listener1[STK_SIZE_TASK_LISTENER];
k_stack_t stack_task_listener2[STK_SIZE_TASK_LISTENER];
k_stack_t stack_task_trigger[STK_SIZE_TASK_TRIGGER];

k_task_t task_listener1;
k_task_t task_listener2;
k_task_t task_trigger;

extern void entry_task_listener1(void *arg);
extern void entry_task_listener2(void *arg);
extern void entry_task_trigger(void *arg);

const k_event_flag_t event_eeny     = (k_event_flag_t)(1 << 0);
const k_event_flag_t event_meeny    = (k_event_flag_t)(1 << 1);
const k_event_flag_t event_miny     = (k_event_flag_t)(1 << 2);
const k_event_flag_t event_moe      = (k_event_flag_t)(1 << 3);

k_event_t event;

void entry_task_listener1(void *arg)
{
    k_event_flag_t flag_match;
    k_err_t err;

    while (K_TRUE) {
        err = tos_event_pend(&event, event_eeny | event_meeny | event_miny | event_moe,
                                &flag_match, TOS_TIME_FOREVER, TOS_OPT_EVENT_PEND_ALL | TOS_OPT_EVENT_PEND_CLR);
        if (err == K_ERR_NONE) {
            printf("entry_task_listener1:\n");
            printf("eeny, meeny, miny, moe, they all come\n");
        }
    }
}

void entry_task_listener2(void *arg)
{
    k_event_flag_t flag_match;
    k_err_t err;

    while (K_TRUE) {
        err = tos_event_pend(&event, event_eeny | event_meeny | event_miny | event_moe,
                                &flag_match, TOS_TIME_FOREVER, TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_CLR);
        if (err == K_ERR_NONE) {
            printf("entry_task_listener2:\n");
            if (flag_match == event_eeny) {
                printf("eeny comes\n");
            }
            if (flag_match == event_meeny) {
                printf("meeny comes\n");
            }
            if (flag_match == event_miny) {
                printf("miny comes\n");
            }
            if (flag_match == event_moe) {
                printf("moe comes\n");
            }
            if (flag_match == (event_eeny | event_meeny | event_miny | event_moe)) {
                printf("all come\n");
            }
        }
    }
}

void entry_task_trigger(void *arg)
{
    int i = 1;

    while (K_TRUE) {
        if (i == 2) {
            printf("entry_task_trigger:\n");
            printf("eeny will come\n");
            tos_event_post(&event, event_eeny);
        }
        if (i == 3) {
            printf("entry_task_trigger:\n");
            printf("meeny will come\n");
            tos_event_post(&event, event_meeny);
        }
        if (i == 4) {
            printf("entry_task_trigger:\n");
            printf("miny will come\n");
            tos_event_post(&event, event_miny);
        }
        if (i == 5) {
            printf("entry_task_trigger:\n");
            printf("moe will come\n");
            tos_event_post(&event, event_moe);
        }
        if (i == 6) {
            printf("entry_task_trigger:\n");
            printf("all will come\n");
            tos_event_post(&event, event_eeny | event_meeny | event_miny | event_moe);
        }
        tos_task_delay(1000);
        ++i;
    }
}

int main(void)
{
    board_init();
    tos_knl_init();
    tos_event_create(&event, (k_event_flag_t)0u);
    (void)tos_task_create(&task_listener1, "listener1", entry_task_listener1, NULL,
                            3, stack_task_listener1, STK_SIZE_TASK_LISTENER, 0);
    (void)tos_task_create(&task_listener2, "listener2", entry_task_listener2, NULL,
                            4, stack_task_listener2, STK_SIZE_TASK_LISTENER, 0);
    (void)tos_task_create(&task_trigger, "trigger", entry_task_trigger, NULL,
                            4, stack_task_trigger, STK_SIZE_TASK_TRIGGER, 0);
    tos_knl_start();
}

