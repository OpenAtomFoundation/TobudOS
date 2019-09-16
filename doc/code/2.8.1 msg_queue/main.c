#include "tos.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_DEMO      512 

#define PRIO_TASK_DEMO          4
 
k_stack_t stack_task_demo[STK_SIZE_TASK_DEMO]; 

k_task_t task_demo;

k_msg_queue_t msg_queue;

struct msg_st {
    char *msg;
    size_t size;
} msgs[TOS_CFG_MSG_POOL_SIZE] = {
    { "msg 0", 6 },
    { "msg 1", 6 },
    { "msg 2", 6 },
};

struct msg_st dummy_msg = { "dummy msg", 10 };

extern void entry_task_demo(void *arg);

void fifo_opt(void) {
    k_err_t err;
    int i = 0;
    char *msg_received = K_NULL;
    size_t msg_size = 0;

    for (; i < TOS_CFG_MSG_POOL_SIZE; ++i) {
        printf("msg put: %s\n", msgs[i].msg);
        err = tos_msg_queue_put(&msg_queue, (void *)msgs[i].msg, msgs[i].size, TOS_OPT_MSG_PUT_FIFO);
        if (err != K_ERR_NONE) {
            printf("should never happen\n");
        }
    }

    err = tos_msg_queue_put(&msg_queue, (void *)dummy_msg.msg, dummy_msg.size, TOS_OPT_MSG_PUT_FIFO);
    if (err == K_ERR_MSG_QUEUE_FULL) {
        printf("msg queue is full\n");
    } else {
        printf("should never happen\n");
    }

    for (i = 0; i < TOS_CFG_MSG_POOL_SIZE; ++i) {
        err = tos_msg_queue_get(&msg_queue, (void **)&msg_received, &msg_size);
        if (err == K_ERR_NONE) {
            printf("msg received: %s\n", msg_received);
            printf("msg size: %d\n", msg_size);
        } else {
            printf("should never happen\n");
        }
    }
    err = tos_msg_queue_get(&msg_queue, (void **)&msg_received, &msg_size);
    if (err == K_ERR_MSG_QUEUE_EMPTY) {
        printf("msg queue is empty\n");
    } else {
        printf("should never happen\n");
    }
}

void lifo_opt(void) {
    k_err_t err;
    int i = 0;
    char *msg_received = K_NULL;
    size_t msg_size = 0;

    for (; i < TOS_CFG_MSG_POOL_SIZE; ++i) {
        printf("msg put: %s\n", msgs[i].msg);
        err = tos_msg_queue_put(&msg_queue, (void *)msgs[i].msg, msgs[i].size, TOS_OPT_MSG_PUT_LIFO);
        if (err != K_ERR_NONE) {
            printf("should never happen\n");
        }
    }
    err = tos_msg_queue_put(&msg_queue, (void *)dummy_msg.msg, dummy_msg.size, TOS_OPT_MSG_PUT_LIFO);
    if (err == K_ERR_MSG_QUEUE_FULL) {
        printf("msg queue is full\n");
    } else {
        printf("should never happen\n");
    }

    for (i = 0; i < TOS_CFG_MSG_POOL_SIZE; ++i) {
        err = tos_msg_queue_get(&msg_queue, (void **)&msg_received, &msg_size);
        if (err == K_ERR_NONE) {
            printf("msg received: %s\n", msg_received);
            printf("msg size: %d\n", msg_size);
        } else {
            printf("should never happen\n");
        }
    }
    err = tos_msg_queue_get(&msg_queue, (void **)&msg_received, &msg_size);
    if (err == K_ERR_MSG_QUEUE_EMPTY) {
        printf("msg queue is empty\n");
    } else {
        printf("should never happen\n");
    }
}

void entry_task_demo(void *arg)
{ 
    tos_msg_queue_create(&msg_queue);

    printf("max msg in pool: %d\n", TOS_CFG_MSG_POOL_SIZE);
    printf("msg queue using TOS_OPT_MSG_PUT_FIFO\n");
    fifo_opt();

    printf("msg queue using TOS_OPT_MSG_PUT_LIFO\n");
    lifo_opt();
}

int main(void)
{
    board_init();
    tos_knl_init(); 
    (void)tos_task_create(&task_demo, "demo1", entry_task_demo, NULL,
                            PRIO_TASK_DEMO, stack_task_demo, STK_SIZE_TASK_DEMO,
                            0); 
    tos_knl_start();
}

