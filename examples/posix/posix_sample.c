#include "tos_posix.h"
#include "pthread.h"
#include "mqueue.h"
#include "semaphore.h"
#include "time.h"

typedef struct pthread_arg_st {
    char       *msg;
    uint32_t    payload;
} pthread_arg_t;

#define FIRSTBORN_PTHREAD_STACK_SIZE    1024
uint8_t firstborn_pthread_stack[FIRSTBORN_PTHREAD_STACK_SIZE];

pthread_t firstborn_pthread;
pthread_arg_t the_arg;

pthread_t secondborn_pthread;
char *secondborn_last_words = "2nd, dead now";

pthread_t thirdborn_pthread;

pthread_t fourthborn_pthread;
char *fourthborn_last_words = "4th, dead now";

void *fourthborn_routine(void *arg)
{
    int i = 0;

    while (1) {
        printf("I am the 4th-born\n");
        tos_task_delay(2000);
        if (++i == 4) {
            printf("4th-born: I kill myself!\n");
            pthread_exit(fourthborn_last_words);
        }
    }
}

void *thirdborn_routine(void *arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while (1) {
        printf("I am the 3rd-born\n");
        tos_task_delay(2000);
    }
}

void *secondborn_routine(void *arg)
{
    printf("I am the 2nd-born, hello!\n");
    tos_task_delay(2000);
    printf("I am the 2nd-born, goodbye!\n");
    return (void *)secondborn_last_words;
}

void *firstborn_routine(void *arg)
{
    int i = 0;
    int rc = 0;
    pthread_arg_t *the_arg;
    pthread_attr_t attr;
    void *value;

    the_arg = (pthread_arg_t *)arg;
    printf("msg: %s\n", the_arg->msg);
    printf("payload:0x%x\n", the_arg->payload);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    rc = pthread_create(&secondborn_pthread, &attr, secondborn_routine, NULL);
    if (rc != 0) {
        printf("2nd-born create failed!\n");
    }

    pthread_join(secondborn_pthread, &value);
    printf("2nd-born is dead now\n");
    printf("his last words: %s\n", (char *)value);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    rc = pthread_create(&thirdborn_pthread, &attr, thirdborn_routine, NULL);
    if (rc != 0) {
        printf("3rd-born pthread create failed!\n");
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    rc = pthread_create(&fourthborn_pthread, &attr, fourthborn_routine, NULL);
    if (rc != 0) {
        printf("4th-born create failed!\n");
    }

    while (1) {
        ++i;
        printf("I am the 1st-born\n");

        if (i == 3) {
            printf("do the canceling to 3rd-born!\n");
            pthread_cancel(thirdborn_pthread);
        } else if (i == 4) {
            /* the fourththread must be dead already, check his last words */
            pthread_join(fourthborn_pthread, &value);
            printf("4th-born's last words: %s\n", (char *)value);
        }
        tos_task_delay(3000);
    }
}

void application_entry(void *arg)
{
    int rc = 0;
    pthread_attr_t attr;
    struct sched_param param;

    /* if use posix, must have this function invoked first */
    rc = tos_posix_init();
    if (rc != 0) {
        return;
    }

    the_arg.msg         = "hello, 1st-born posix thread!";
    the_arg.payload     = 0xDEADBEEF;

    pthread_attr_init(&attr);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    param.sched_priority = 4;
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setstack(&attr, firstborn_pthread_stack, sizeof(firstborn_pthread_stack));

    rc = pthread_create(&firstborn_pthread, &attr, firstborn_routine, &the_arg);
    if (rc != 0) {
        return;
    }
}

