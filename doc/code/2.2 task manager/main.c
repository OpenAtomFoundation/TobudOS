#include "tos_k.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_PRIO4      512
#define STK_SIZE_TASK_PRIO5      1024


k_stack_t stack_task_prio4[STK_SIZE_TASK_PRIO4];
k_stack_t stack_task_prio5[STK_SIZE_TASK_PRIO5];

k_task_t task_prio4;
k_task_t task_prio5;

extern void entry_task_prio4(void *arg);
extern void entry_task_prio5(void *arg);

uint32_t arg_task_prio4_array[3] = {
    1, 2, 3,
};

char *arg_task_prio5_string = "arg for task_prio5";

void dump_uint32_array(uint32_t *array, size_t len)
{
    size_t i = 0;

    for (i = 0; i < len; ++i) {
        printf("%d\t", array[i]);
    }
    printf("\n\n");

}

void entry_task_prio4(void *arg)
{
    uint32_t *array_from_main = (uint32_t *)arg;

    printf("array from main:\n");
    dump_uint32_array(array_from_main, 3);

    while (K_TRUE) {
        printf("task_prio4 body\n");
        tos_task_delay(1000);
    }
}

void entry_task_prio5(void *arg)
{
    int i = 0;
    char *string_from_main = (char *)arg;
    printf("string from main:\n");
    printf("%s\n\n", string_from_main);

    while (K_TRUE) {
        if (i == 2) {
            printf("i = %d\n", i);
            tos_task_suspend(&task_prio4);
        } else if (i == 4) {
            printf("i = %d\n", i);
            tos_task_resume(&task_prio4);
        } else if (i == 6) {
            printf("i = %d\n", i);
            tos_task_destroy(&task_prio4);
        }
        printf("task_prio5 body\n");
        tos_task_delay(1000);
        ++i;
    }
}

int main(void)
{
    board_init();
    tos_knl_init();
    (void)tos_task_create(&task_prio4, "task_prio4", entry_task_prio4, (void *)(&arg_task_prio4_array[0]),
                            4, stack_task_prio4, STK_SIZE_TASK_PRIO4, 0);
    (void)tos_task_create(&task_prio5, "task_prio5", entry_task_prio5, (void *)arg_task_prio5_string, 5,
                            stack_task_prio5, STK_SIZE_TASK_PRIO5, 0);
    tos_knl_start();
}

