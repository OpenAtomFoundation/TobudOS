#include "tos.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_DEMO      512

#define PRIO_TASK_DEMO          4

k_stack_t stack_task_demo[STK_SIZE_TASK_DEMO];

k_task_t task_demo;

typedef struct item_st {
    int a;
    int b;
    int c;
} item_t;

#define RING_QUEUE_ITEM_MAX        5
uint8_t ring_q_buffer[RING_QUEUE_ITEM_MAX * sizeof(item_t)];

k_ring_q_t rinq_q;

void entry_task_demo(void *arg)
{
    k_err_t err;
    int i = 0;
    item_t item;
    size_t item_size;

    tos_ring_q_create(&rinq_q, ring_q_buffer, RING_QUEUE_ITEM_MAX, sizeof(item_t));
    for (i = 0; i < RING_QUEUE_ITEM_MAX; ++i) {
        printf("enqueue: %d  %d  %d\n", i, i, i);
        item.a = i;
        item.b = i;
        item.c = i;
        err = tos_ring_q_enqueue(&rinq_q, &item, sizeof(item_t));
        if (err != K_ERR_NONE) {
            printf("should never happen\n");
        }
    }

    err = tos_ring_q_enqueue(&rinq_q, &item, sizeof(item_t));
    if (err == K_ERR_RING_Q_FULL) {
        printf("ring queue is full: %s\n", tos_ring_q_is_full(&rinq_q) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }

    for (i = 0; i < RING_QUEUE_ITEM_MAX; ++i) {
        err = tos_ring_q_dequeue(&rinq_q, &item, &item_size);
        if (err == K_ERR_NONE) {
            printf("dequeue: %d %d %d\n", item.a, item.b, item.c);
        } else {
            printf("should never happen\n");
        }
    }

    err = tos_ring_q_dequeue(&rinq_q, &item, &item_size);
    if (err == K_ERR_RING_Q_EMPTY) {
        printf("ring queue is empty: %s\n", tos_ring_q_is_empty(&rinq_q) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }

}

int main(void)
{
    board_init();
    tos_knl_init();
    (void)tos_task_create(&task_demo, "demo", entry_task_demo, NULL,
                            PRIO_TASK_DEMO, stack_task_demo, STK_SIZE_TASK_DEMO,
                            0);
    tos_knl_start();
}

