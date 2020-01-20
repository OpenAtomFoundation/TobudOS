#include "tos_k.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_DEMO      512

k_stack_t stack_task_demo[STK_SIZE_TASK_DEMO];

k_task_t task_demo;

extern void entry_task_demo(void *arg);

void entry_task_demo(void *arg)
{
    void *p = K_NULL, *p_aligned = K_NULL;
    int i = 0;

    while (K_TRUE) {
        if (i == 1) {
            p = tos_mmheap_alloc(0x30);
            if (p) {
                printf("alloc: %x\n", (cpu_addr_t)p);
            }
        } else if (i == 2) {
            if (p) {
                printf("free: %x\n", p);
                tos_mmheap_free(p);
            }
        } else if (i == 3) {
            p = tos_mmheap_alloc(0x30);
            if (p) {
                printf("alloc: %x\n", (cpu_addr_t)p);
            }
        } else if (i == 4) {
            p_aligned = tos_mmheap_aligned_alloc(0x50, 16);
            if (p_aligned) {
                printf("aligned alloc: %x\n", (cpu_addr_t)p_aligned);
                if ((cpu_addr_t)p_aligned % 16 == 0) {
                    printf("%x is 16 aligned\n", (cpu_addr_t)p_aligned);
                } else {
                    printf("should not happen\n");
                }
            }
        } else if (i == 5) {
            p = tos_mmheap_realloc(p, 0x40);
            if (p) {
                printf("realloc: %x\n", (cpu_addr_t)p);
            }
        } else if (i == 6) {
            if (p) {
                tos_mmheap_free(p);
            }
            if (p_aligned) {
                tos_mmheap_free(p_aligned);
            }
        }

        tos_task_delay(1000);
        ++i;
    }
}

int main(void)
{
    board_init();
    tos_knl_init();
    (void)tos_task_create(&task_demo, "receiver_higher_prio", entry_task_demo, NULL,
                            4, stack_task_demo, STK_SIZE_TASK_DEMO, 0);
    tos_knl_start();
}

