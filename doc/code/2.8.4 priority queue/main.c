#include "tos_k.h"
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

#define PRIO_QUEUE_ITEM_MAX     5
uint8_t ring_q_buffer[PRIO_QUEUE_ITEM_MAX * sizeof(item_t)];
uint8_t mgr_pool[TOS_PRIO_Q_MGR_ARRAY_SIZE(PRIO_QUEUE_ITEM_MAX)];

k_prio_q_t prio_q;

void entry_task_demo(void *arg)
{
    k_err_t err;
    int i = 0;
    item_t item;
    k_prio_t prio;
    size_t item_size;

    tos_prio_q_create(&prio_q, mgr_pool, ring_q_buffer, PRIO_QUEUE_ITEM_MAX, sizeof(item_t));

    for (i = PRIO_QUEUE_ITEM_MAX; i > 0; --i) {
        printf("enqueue: %d  %d  %d\n", i, i, i);
        item.a = i;
        item.b = i;
        item.c = i;
        err = tos_prio_q_enqueue(&prio_q, &item, sizeof(item_t), i);
        if (err != K_ERR_NONE) {
            printf("should never happen\n");
        }
    }

    err = tos_prio_q_enqueue(&prio_q, &item, sizeof(item_t), i);
    if (err == K_ERR_PRIO_Q_FULL) {
        printf("priority queue is full: %s\n", tos_prio_q_is_full(&prio_q) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }

    for (i = 0; i < PRIO_QUEUE_ITEM_MAX; ++i) {
        err = tos_prio_q_dequeue(&prio_q, &item, &item_size, &prio);
        if (err == K_ERR_NONE) {
            printf("dequeue: %d %d %d, prio: %d\n", item.a, item.b, item.c, prio);
        } else {
            printf("should never happen\n");
        }
    }

    err = tos_prio_q_dequeue(&prio_q, &item, &item_size, &prio);
    if (err == K_ERR_PRIO_Q_EMPTY) {
        printf("priority queue is empty: %s\n", tos_prio_q_is_empty(&prio_q) ? "TRUE" : "FALSE");
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
