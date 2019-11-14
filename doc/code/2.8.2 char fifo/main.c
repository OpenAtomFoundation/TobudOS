#include "tos.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_DEMO      512

#define PRIO_TASK_DEMO          4

k_stack_t stack_task_demo[STK_SIZE_TASK_DEMO];

k_task_t task_demo;

#define FIFO_BUFFER_SIZE        5
uint8_t fifo_buffer[FIFO_BUFFER_SIZE];

k_chr_fifo_t fifo;

extern void entry_task_demo(void *arg);

void char_push(void)
{
    k_err_t err;
    int i = 0;
    uint8_t data;

    for (i = 0; i < FIFO_BUFFER_SIZE; ++i) {
        printf("char pushed: %c\n", 'a' + i);
        err = tos_chr_fifo_push(&fifo, 'a' + i);
        if (err != K_ERR_NONE) {
            printf("should never happen\n");
        }
    }

    err = tos_chr_fifo_push(&fifo, 'z');
    if (err == K_ERR_RING_Q_FULL) {
        printf("fifo is full: %s\n", tos_chr_fifo_is_full(&fifo) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }

    for (i = 0; i < FIFO_BUFFER_SIZE; ++i) {
        err = tos_chr_fifo_pop(&fifo, &data);
        if (err == K_ERR_NONE) {
            printf("%d pop: %c\n", i, data);
        } else {
            printf("should never happen\n");
        }
    }
    err = tos_chr_fifo_pop(&fifo, &data);
    if (err == K_ERR_RING_Q_EMPTY) {
        printf("fifo is empty: %s\n", tos_chr_fifo_is_empty(&fifo) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }
}

void stream_push(void)
{
    int count = 0, i = 0;
    uint8_t stream[FIFO_BUFFER_SIZE] = { 'a', 'b', 'c', 'd', 'e' };
    uint8_t stream_dummy[1] = { 'z' };
    uint8_t stream_pop[FIFO_BUFFER_SIZE];

    count = tos_chr_fifo_push_stream(&fifo, &stream[0], FIFO_BUFFER_SIZE);
    if (count != FIFO_BUFFER_SIZE) {
        printf("should never happen\n");
    }

    count = tos_chr_fifo_push_stream(&fifo, &stream_dummy[0], 1);
    if (count == 0) {
        printf("fifo is full: %s\n", tos_chr_fifo_is_full(&fifo) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }

    count = tos_chr_fifo_pop_stream(&fifo, &stream_pop[0], FIFO_BUFFER_SIZE);
    if (count == FIFO_BUFFER_SIZE) {
        printf("stream popped:\n");
        for (i = 0; i < FIFO_BUFFER_SIZE; ++i) {
            printf("%c", stream_pop[i]);
        }
        printf("\n");
    } else {
        printf("should never happen\n");
    }

    count = tos_chr_fifo_pop_stream(&fifo, &stream_pop[0], 1);
    if (count == 0) {
        printf("fifo is empty: %s\n", tos_chr_fifo_is_empty(&fifo) ? "TRUE" : "FALSE");
    } else {
        printf("should never happen\n");
    }
}

void entry_task_demo(void *arg)
{
    tos_chr_fifo_create(&fifo, &fifo_buffer[0], FIFO_BUFFER_SIZE);

    printf("fifo, dealing with char\n");
    char_push();

    printf("fifo, dealing with stream\n");
    stream_push();
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

