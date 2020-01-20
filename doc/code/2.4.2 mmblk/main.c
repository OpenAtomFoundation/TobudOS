#include "tos_k.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_DEMO      512

k_stack_t stack_task_demo[STK_SIZE_TASK_DEMO];

k_task_t task_demo;


#define MMBLK_BLK_NUM       5
#define MMBLK_BLK_SIZE      0x20

k_mmblk_pool_t mmblk_pool;

uint8_t mmblk_pool_buffer[MMBLK_BLK_NUM * MMBLK_BLK_SIZE];

void *p[MMBLK_BLK_NUM] = { K_NULL };

extern void entry_task_demo(void *arg);

void entry_task_demo(void *arg)
{
    void *p_dummy = K_NULL;
    k_err_t err;
    int i = 0;

    printf("mmblk_pool has %d blocks, size of each block is 0x%x\n", 5, 0x20);
    for (; i < MMBLK_BLK_NUM; ++i) {
        err = tos_mmblk_alloc(&mmblk_pool, &p[i]);
        if (err == K_ERR_NONE) {
            printf("%d block alloced: 0x%x\n", i, p[i]);
        } else {
            printf("should not happen\n");
        }
    }

    err = tos_mmblk_alloc(&mmblk_pool, &p_dummy);
    if (err == K_ERR_MMBLK_POOL_EMPTY) {
        printf("blocks exhausted, all blocks is alloced\n");
    } else {
        printf("should not happen\n");
    }

    for (i = 0; i < MMBLK_BLK_NUM; ++i) {
        err = tos_mmblk_free(&mmblk_pool, p[i]);
        if (err != K_ERR_NONE) {
            printf("should not happen\n");
        }
    }
    err = tos_mmblk_free(&mmblk_pool, p[0]);
    if (err == K_ERR_MMBLK_POOL_FULL) {
        printf("pool is full\n");
    } else {
        printf("should not happen\n");
    }
}

int main(void)
{
    board_init();
    tos_knl_init();
    tos_mmblk_pool_create(&mmblk_pool, mmblk_pool_buffer, MMBLK_BLK_NUM, MMBLK_BLK_SIZE);
    (void)tos_task_create(&task_demo, "receiver_higher_prio", entry_task_demo, NULL,
                            4, stack_task_demo, STK_SIZE_TASK_DEMO, 0);
    tos_knl_start();
}

