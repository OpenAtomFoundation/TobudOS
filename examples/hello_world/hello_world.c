#include "cmsis_os.h"

#define TASK1_STK_SIZE		512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

#define TASK2_STK_SIZE		512
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);


k_ring_q_t ring_q;
k_chr_fifo_t chr_fifo;
k_msg_q_t msg_q;

k_mail_q_t mail_q;

k_bin_heap_t bin_heap;

k_prio_q_t prio_q;

typedef struct fuck {
    int     a;
    char   *name;
} fuck_t;

char ring_q_pool[sizeof(fuck_t) * 4];

uint8_t chr_fifo_pool[sizeof(uint8_t) * 4];

uint8_t msg_q_pool[sizeof(void *) * 4];

uint8_t mail_q_pool[sizeof(fuck_t) * 4];

int int_pool[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int int_seq[] = { 0, 1, 2, 3, 4, 8, 9, 3, 5 };

fuck_t fuck_pool[9];

fuck_t fuck_seq[9] = {
        { 0, "000" },
        { 1, "111" },
        { 2, "222" },
        { 3, "333" },
        { 4, "444" },
        { 8, "888" },
        { 9, "999" },
        { 3, "333" },
        { 5, "555" },
};

uint8_t mgr_entry_array[TOS_PRIO_Q_ARRAY_SIZE(3)];

int int_cmp(void *first, void *second)
{
    int f = *(int *)first;
    int s = *(int *)second;

    if (f > s) {
        return 1;
    }
    return 0;
}

int fuck_cmp(void *first, void *second)
{
    fuck_t *f = (fuck_t *)first;
    fuck_t *s = (fuck_t *)second;

    if (f->a > s->a) {
        return 1;
    }
    return 0;
}


int fuck_int(void)
{
    int i;
    for (i = 0; i < bin_heap.total; ++i) {
        printf("%d\n", int_pool[i]);
    }
}

int fuck_fuck(void)
{
    int i;
    for (i = 0; i < bin_heap.total; ++i) {
        printf("%d  %s\n", fuck_pool[i].a, fuck_pool[i].name);
    }

}

int fuck(void)
{
    fuck_fuck();
}

void task1(void *arg)
{
    size_t item_size;
    int fuckx = 19;
    k_err_t err;
    int i = 0;

    fuck_t dummy = {
        10, "dummy"
    };

#if 0
    for (i = 0; i < sizeof(int_seq) / sizeof(int); ++i) {
        err = tos_bin_heap_push(&bin_heap, &int_seq[i]);
        printf("%d  %d\n", int_seq[i], err);
    }

    err = tos_bin_heap_push(&bin_heap, &fuckx);
    printf("%d  %d\n", fuckx, err);

    printf("=============\n\n");
    fuck();

    while (!tos_bin_heap_is_empty(&bin_heap)) {
        err = tos_bin_heap_pop(&bin_heap, &fuckx, &item_size);
        printf("!!!!!!  %d  %d  %d\n", err, fuckx, item_size);
    }
#elif 0
    for (i = 0; i < 9; ++i) {
        err = tos_bin_heap_push(&bin_heap, &fuck_seq[i]);
        printf("%d  %s  %d\n", fuck_seq[i].a, fuck_seq[i].name, err);
    }

    err = tos_bin_heap_push(&bin_heap, &dummy);
    printf("%d  %d\n", dummy.a, err);

    printf("=============\n\n");
    fuck();

    while (!tos_bin_heap_is_empty(&bin_heap)) {
        err = tos_bin_heap_pop(&bin_heap, &dummy, &item_size);
        printf("!!!!!!  %d  %d   %s  %d\n", err, dummy.a, dummy.name, item_size);
    }
#else

    for (i = 0; i < 9; ++i) {
        err = tos_prio_q_enqueue(&prio_q, &fuck_seq[i], sizeof(fuck_t), fuck_seq[i].a);
        printf("%d  %s  %d\n", fuck_seq[i].a, fuck_seq[i].name, err);
    }

    err = tos_prio_q_enqueue(&prio_q, &dummy, sizeof(fuck_t), dummy.a);
    printf("%d  %d\n", dummy.a, err);

    printf("=============\n\n");
    // fuck();

    while (!tos_prio_q_is_empty(&prio_q)) {
        err = tos_prio_q_dequeue(&prio_q, &dummy, &item_size);
        printf("!!!!!!  %d  %d   %s  %d\n", err, dummy.a, dummy.name, item_size);
    }


#endif

    return;


    fuck_t fuck1 = { 1, "111" };
    fuck_t fuck2 = { 2, "222" };
    fuck_t fuck3 = { 3, "333" };
    fuck_t fuck4 = { 4, "444" };
    fuck_t fuck5 = { 5, "555" };

    int count = 1;
    while (1) {
        printf("###This is task1, %d\r\n", count++);

        osDelay(1000);

        if (count == 2) {
            err = tos_mail_q_post(&mail_q, &fuck1, sizeof(fuck1));
            printf("task1:  %d\n", err);
        } else if (count == 3) {
            err = tos_mail_q_post(&mail_q, &fuck2, sizeof(fuck2));
            printf("task1:  %d\n", err);
        } else if (count == 4) {
            err = tos_mail_q_post(&mail_q, &fuck3, sizeof(fuck3));
            printf("task1:  %d\n", err);
        } else if (count == 5) {
            err = tos_mail_q_post(&mail_q, &fuck4, sizeof(fuck4));
            printf("task1:  %d\n", err);
        }
    }
}

void task2(void *arg)
{
    return;
    k_err_t err;
    void *msg;
    size_t msg_size;

    int count = 1;
    while (1) {
        printf("***This is task2, %d\r\n", count++);

        err = tos_mail_q_pend(&mail_q, &msg, &msg_size, TOS_TIME_FOREVER);
        printf("task2:  %d  %d  %x  %d  %s\n", err,  msg_size, msg, ((fuck_t *)msg)->a, ((fuck_t *)msg)->name);
    }
}

void application_entry(void *arg)
{
    tos_prio_q_create(&prio_q, mgr_entry_array, fuck_pool, sizeof(fuck_t), sizeof(fuck_pool) / sizeof(fuck_t));
    // tos_bin_heap_create(&bin_heap, int_pool, sizeof(int), sizeof(int_pool) / sizeof(int), int_cmp);
    // tos_bin_heap_create(&bin_heap, fuck_pool, sizeof(fuck_t), sizeof(fuck_pool) / sizeof(fuck_t), fuck_cmp);

#if 0
    tos_ring_q_create(&ring_q, ring_q_pool, sizeof(fuck_t), 4);

    tos_chr_fifo_create(&chr_fifo, chr_fifo_pool, sizeof(chr_fifo_pool));

    tos_msg_q_create(&msg_q, msg_q_pool, 4);

    tos_mail_q_create(&mail_q, mail_q_pool, sizeof(fuck_t), 4);
#endif

    osThreadCreate(osThread(task1), NULL); // Create task1
    osThreadCreate(osThread(task2), NULL); // Create task2
}

