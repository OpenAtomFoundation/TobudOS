#include "tos_k.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_PRODUCER      512
#define STK_SIZE_TASK_CONSUMER      512

k_stack_t stack_task_producer[STK_SIZE_TASK_PRODUCER];
k_stack_t stack_task_consumer[STK_SIZE_TASK_CONSUMER];

k_task_t task_producer;
k_task_t task_consumer;

extern void entry_task_producer(void *arg);
extern void entry_task_consumer(void *arg);

k_mutex_t buffer_locker;
k_sem_t full;
k_sem_t empty;

#define RESOURCE_COUNT_MAX      3

struct resource_st {
    int cursor;
    uint32_t buffer[RESOURCE_COUNT_MAX];
} resource = { 0, {0} };

static void produce_item(int salt)
{
    printf("produce item:\n");

    printf("%d", salt);
    resource.buffer[resource.cursor++] = salt;
    printf("\n");
}

void entry_task_producer(void *arg)
{
    size_t salt = 0;
    k_err_t err;

    while (K_TRUE) {
        err = tos_sem_pend(&empty, TOS_TIME_FOREVER);
        if (err != K_ERR_NONE) {
            continue;
        }
        err = tos_mutex_pend(&buffer_locker);
        if (err != K_ERR_NONE) {
            continue;
        }

        produce_item(salt);

        tos_mutex_post(&buffer_locker);
        tos_sem_post(&full);
        tos_task_delay(1000);
        ++salt;
    }
}

static void consume_item(void)
{
    printf("cosume item:\n");
    printf("%d\t", resource.buffer[--resource.cursor]);
    printf("\n");
}

void entry_task_consumer(void *arg)
{
    k_err_t err;

    while (K_TRUE) {
        err = tos_sem_pend(&full, TOS_TIME_FOREVER);
        if (err != K_ERR_NONE) {
            continue;
        }
        tos_mutex_pend(&buffer_locker);
        if (err != K_ERR_NONE) {
            continue;
        }

        consume_item();

        tos_mutex_post(&buffer_locker);
        tos_sem_post(&empty);
        tos_task_delay(2000);
    }
}

int main(void)
{
    board_init();
    tos_knl_init();
    tos_mutex_create(&buffer_locker);
    tos_sem_create(&full, 0);
    tos_sem_create(&empty, RESOURCE_COUNT_MAX);
    (void)tos_task_create(&task_producer, "producer", entry_task_producer, NULL,
                            4, stack_task_producer, STK_SIZE_TASK_PRODUCER, 0);
    (void)tos_task_create(&task_consumer, "consumer", entry_task_consumer, NULL,
                            4, stack_task_consumer, STK_SIZE_TASK_CONSUMER, 0);
    tos_knl_start();
}

