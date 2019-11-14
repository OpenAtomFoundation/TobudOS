#include "tos.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_RECEIVER      512
#define STK_SIZE_TASK_SENDER        512

#define PRIO_TASK_RECEIVER_HIGHER_PRIO      4
#define PRIO_TASK_RECEIVER_LOWER_PRIO       (PRIO_TASK_RECEIVER_HIGHER_PRIO + 1)

#define MESSAGE_MAX     10

k_stack_t stack_task_receiver_higher_prio[STK_SIZE_TASK_RECEIVER];
k_stack_t stack_task_receiver_lower_prio[STK_SIZE_TASK_RECEIVER];
k_stack_t stack_task_sender[STK_SIZE_TASK_SENDER];

uint8_t msg_pool[MESSAGE_MAX * sizeof(void *)];

k_task_t task_receiver_higher_prio;
k_task_t task_receiver_lower_prio;
k_task_t task_sender;

k_msg_q_t msg_q;

extern void entry_task_receiver_higher_prio(void *arg);
extern void entry_task_receiver_lower_prio(void *arg);
extern void entry_task_sender(void *arg);

void entry_task_receiver_higher_prio(void *arg)
{
    k_err_t err;
    void *msg_received;

    while (K_TRUE) {
        err = tos_msg_q_pend(&msg_q, &msg_received, TOS_TIME_FOREVER);
        if (err == K_ERR_NONE) {
            printf("higher: msg incoming[%s]\n", (char *)msg_received);
        }
    }
}

void entry_task_receiver_lower_prio(void *arg)
{
    k_err_t err;
    void *msg_received;

    while (K_TRUE) {
        err = tos_msg_q_pend(&msg_q, &msg_received, TOS_TIME_FOREVER);
        if (err == K_ERR_NONE) {
            printf("lower: msg incoming[%s]\n", (char *)msg_received);
        }
    }
}

void entry_task_sender(void *arg)
{
    int i = 1;
    char *msg_to_one_receiver = "message for one receiver(with highest priority)";
    char *msg_to_all_receiver = "message for all receivers";

    while (K_TRUE) {
        if (i == 2) {
            printf("sender: send a message to one receiver, and shoud be the highest priority one\n");
            tos_msg_q_post(&msg_q, msg_to_one_receiver);
        }
        if (i == 3) {
            printf("sender: send a message to all recevier\n");
            tos_msg_q_post_all(&msg_q, msg_to_all_receiver);
        }
        if (i == 4) {
            printf("sender: send a message to one receiver, and shoud be the highest priority one\n");
            tos_msg_q_post(&msg_q, msg_to_one_receiver);
        }
        if (i == 5) {
            printf("sender: send a message to all recevier\n");
            tos_msg_q_post_all(&msg_q, msg_to_all_receiver);
        }
        tos_task_delay(1000);
        ++i;
    }
}

int main(void)
{
    board_init();
    tos_knl_init();
    tos_msg_q_create(&msg_q, msg_pool, MESSAGE_MAX);
    (void)tos_task_create(&task_receiver_higher_prio, "receiver_higher_prio",
                            entry_task_receiver_higher_prio, NULL, PRIO_TASK_RECEIVER_HIGHER_PRIO,
                            stack_task_receiver_higher_prio, STK_SIZE_TASK_RECEIVER, 0);
    (void)tos_task_create(&task_receiver_lower_prio, "receiver_lower_prio",
                            entry_task_receiver_lower_prio, NULL, PRIO_TASK_RECEIVER_LOWER_PRIO,
                            stack_task_receiver_lower_prio, STK_SIZE_TASK_RECEIVER, 0);
    (void)tos_task_create(&task_sender, "sender", entry_task_sender, NULL,
                            4, stack_task_sender, STK_SIZE_TASK_SENDER, 0);
    tos_knl_start();
}
