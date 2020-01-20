#include "tos_k.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_WRITER        512
#define STK_SIZE_TASK_READER        512

k_stack_t stack_task_writer[STK_SIZE_TASK_WRITER];
k_stack_t stack_task_reader[STK_SIZE_TASK_READER];

k_task_t task_writer;
k_task_t task_reader;

extern void entry_task_writer(void *arg);
extern void entry_task_reader(void *arg);

k_mutex_t critical_resource_locker;

static uint32_t critical_resource[3];

static void write_critical_resource(int salt)
{
    size_t i = 0;

    printf("writting critical resource:\n");
    for (i = 0; i < 3; ++i) {
        printf("%d\t", salt + i);
        critical_resource[i] = salt + i;
    }
    printf("\n");
}

void entry_task_writer(void *arg)
{
    size_t salt = 0;
    k_err_t err;

    while (K_TRUE) {
        err = tos_mutex_pend(&critical_resource_locker);
        if (err == K_ERR_NONE) {
            write_critical_resource(salt);
            tos_mutex_post(&critical_resource_locker);
        }
        tos_task_delay(1000);
        ++salt;
    }
}

static void read_critical_resource(void)
{
    size_t i = 0;

    printf("reading critical resource:\n");
    for (i = 0; i < 3; ++i) {
        printf("%d\t", critical_resource[i]);
    }
    printf("\n");
}

void entry_task_reader(void *arg)
{
    k_err_t err;

    while (K_TRUE) {
        err = tos_mutex_pend(&critical_resource_locker);
        if (err == K_ERR_NONE) {
            read_critical_resource();
            tos_mutex_post(&critical_resource_locker);
        }
        tos_task_delay(1000);
    }
}

int main(void)
{
    board_init();
    tos_knl_init();
    tos_mutex_create(&critical_resource_locker);
    (void)tos_task_create(&task_writer, "writer", entry_task_writer, NULL,
                            4, stack_task_writer, STK_SIZE_TASK_WRITER, 0);
    (void)tos_task_create(&task_reader, "reader", entry_task_reader, NULL,
                            4, stack_task_reader, STK_SIZE_TASK_READER, 0);
    tos_knl_start();
}

