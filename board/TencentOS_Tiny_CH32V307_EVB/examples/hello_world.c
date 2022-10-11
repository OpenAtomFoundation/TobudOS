#include "tos_k.h"
#include "tos_js.h"

#define TASK1_STK_SIZE       1024
k_task_t task1;
uint8_t task1_stk[TASK1_STK_SIZE];


#define TASK2_STK_SIZE       1024
k_task_t task2;
uint8_t task2_stk[TASK2_STK_SIZE];

#define TASK3_STK_SIZE       4096
k_task_t task3;
uint8_t task3_stk[TASK3_STK_SIZE];

void task1_entry(void *arg)
{
    while (1) {
        printf("###I am task1\r\n");
        tos_task_delay(2000);
    }
}

void task2_entry(void *arg)
{
    while (1) {
        printf("***I am task2\r\n");
        tos_task_delay(1000);
    }
}

char js_mem[6000];
void task3_entry(void *arg) {
    printf("JS Beegin Running!\r\n");
    struct js *js =  tos_js_vm_create(js_mem, sizeof(js_mem));
    js_run(js,
            "let a = 1;"
            "LCD_ShowString(0, 20, 'Greet from JS!');"
            "let b = 1, c = 1, tmp = 1;"
            "for (let i = 1; i <= 10; i++) {"
            "    a *= 2;"
            "    print('res:', a, 'c-fib:', fib(i), 'js-fib:', c);"
            "    tmp = b + c;"
            "    b = c;"
            "    c = tmp;"
            "    sleep(1000);"
            "}"
       );
}

void application_entry(void *arg)
{
    tos_task_create(&task1, "task1", task1_entry, NULL, 3, task1_stk, TASK1_STK_SIZE, 0); // Create task1
    tos_task_create(&task2, "task2", task2_entry, NULL, 3, task2_stk, TASK2_STK_SIZE, 0);// Create task2
    tos_task_create(&task3, "task3", task3_entry, NULL, 4, task3_stk, TASK3_STK_SIZE, 0);// Create task3
}

