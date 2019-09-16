#include "tos.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_RECEIVER      512
#define STK_SIZE_TASK_SENDER        512

#define PRIO_TASK_RECEIVER_HIGHER_PRIO      4
#define PRIO_TASK_RECEIVER_LOWER_PRIO       (PRIO_TASK_RECEIVER_HIGHER_PRIO + 1)

k_stack_t stack_task_receiver_higher_prio[STK_SIZE_TASK_RECEIVER];
k_stack_t stack_task_receiver_lower_prio[STK_SIZE_TASK_RECEIVER];
k_stack_t stack_task_sender[STK_SIZE_TASK_SENDER];

k_task_t task_receiver_higher_prio;
k_task_t task_receiver_lower_prio;
k_task_t task_sender;

k_queue_t queue;

extern void entry_task_receiver_higher_prio(void *arg);
extern void entry_task_receiver_lower_prio(void *arg);
extern void entry_task_sender(void *arg);

void entry_task_receiver_higher_prio(void *arg)
{
    k_err_t err;
    void *msg_received;
    size_t msg_size;

    while (K_TRUE) {
        err = tos_queue_pend(&queue, &msg_received, &msg_size, TOS_TIME_FOREVER);
        if (err == K_ERR_NONE) {
            printf("entry_task_receiver_higher_prio:\n");
            printf("message body: %s\n", (char *)msg_received);
            printf("message size: %d\n", msg_size);
        }
    }
}

void entry_task_receiver_lower_prio(void *arg)
{
    k_err_t err;
    void *msg_received;
    size_t msg_size;

    while (K_TRUE) {
        err = tos_queue_pend(&queue, &msg_received, &msg_size, TOS_TIME_FOREVER);
        if (err == K_ERR_NONE) {
            printf("entry_task_receiver_lower_prio:\n");
            printf("message body: %s\n", (char *)msg_received);
            printf("message size: %d\n", msg_size);
        }
    }
}

void entry_task_sender(void *arg)
{
    int i = 1;
    char *msg_to_one_receiver = "message for one receiver[with highest priority]";
    char *msg_to_all_receiver = "message for all receivers";

    while (K_TRUE) {
        if (i == 2) {
            printf("entry_task_sender:\n");
            printf("send a message to one receiver, and shoud be the highest priority one\n");
            tos_queue_post(&queue, msg_to_one_receiver, strlen(msg_to_one_receiver));
        }
        if (i == 3) {
            printf("entry_task_sender:\n");
            printf("send a message to all recevier\n");
            tos_queue_post_all(&queue, msg_to_all_receiver, strlen(msg_to_all_receiver));
        }
        if (i == 4) {
            printf("entry_task_sender:\n");
            printf("send a message to one receiver, and shoud be the highest priority one\n");
            tos_queue_post(&queue, msg_to_one_receiver, strlen(msg_to_one_receiver));
        }
        if (i == 5) {
            printf("entry_task_sender:\n");
            printf("send a message to all recevier\n");
            tos_queue_post_all(&queue, msg_to_all_receiver, strlen(msg_to_all_receiver));
        }
        tos_task_delay(1000);
        ++i;
    }
}

int main(void)
{
    board_init();
    tos_knl_init();
    tos_queue_create(&queue);
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

