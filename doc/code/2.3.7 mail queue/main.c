#include "tos_k.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_RECEIVER      512
#define STK_SIZE_TASK_SENDER        512

#define PRIO_TASK_RECEIVER_HIGHER_PRIO      4
#define PRIO_TASK_RECEIVER_LOWER_PRIO       (PRIO_TASK_RECEIVER_HIGHER_PRIO + 1)

#define MAIL_MAX    10

k_stack_t stack_task_receiver_higher_prio[STK_SIZE_TASK_RECEIVER];
k_stack_t stack_task_receiver_lower_prio[STK_SIZE_TASK_RECEIVER];
k_stack_t stack_task_sender[STK_SIZE_TASK_SENDER];

typedef struct mail_st {
    char   *message;
    int     payload;
} mail_t;

uint8_t mail_pool[MAIL_MAX * sizeof(mail_t)];

k_task_t task_receiver_higher_prio;
k_task_t task_receiver_lower_prio;
k_task_t task_sender;

k_mail_q_t mail_q;

extern void entry_task_receiver_higher_prio(void *arg);
extern void entry_task_receiver_lower_prio(void *arg);
extern void entry_task_sender(void *arg);

void entry_task_receiver_higher_prio(void *arg)
{
    k_err_t err;
    mail_t mail;
    size_t mail_size;

    while (K_TRUE) {
        err = tos_mail_q_pend(&mail_q, &mail, &mail_size, TOS_TIME_FOREVER);
        if (err == K_ERR_NONE) {
            TOS_ASSERT(mail_size == sizeof(mail_t));
            printf("higher: msg incoming[%s], payload[%d]\n", mail.message, mail.payload);
        }
    }
}

void entry_task_receiver_lower_prio(void *arg)
{
    k_err_t err;
    mail_t mail;
    size_t mail_size;

    while (K_TRUE) {
        err = tos_mail_q_pend(&mail_q, &mail, &mail_size, TOS_TIME_FOREVER);
        if (err == K_ERR_NONE) {
            TOS_ASSERT(mail_size == sizeof(mail_t));
            printf("lower: msg incoming[%s], payload[%d]\n", mail.message, mail.payload);
        }
    }
}

void entry_task_sender(void *arg)
{
    int i = 1;
    mail_t mail;

    while (K_TRUE) {
        if (i == 2) {
            printf("sender: send a mail to one receiver, and shoud be the highest priority one\n");
            mail.message = "1st time post";
            mail.payload = 1;
            tos_mail_q_post(&mail_q, &mail, sizeof(mail_t));
        }
        if (i == 3) {
            printf("sender: send a message to all recevier\n");
            mail.message = "2nd time post";
            mail.payload = 2;
            tos_mail_q_post_all(&mail_q, &mail, sizeof(mail_t));
        }
        if (i == 4) {
            printf("sender: send a message to one receiver, and shoud be the highest priority one\n");
            mail.message = "3rd time post";
            mail.payload = 3;
            tos_mail_q_post(&mail_q, &mail, sizeof(mail_t));
        }
        if (i == 5) {
            printf("sender: send a message to all recevier\n");
            mail.message = "4th time post";
            mail.payload = 4;
            tos_mail_q_post_all(&mail_q, &mail, sizeof(mail_t));
        }
        tos_task_delay(1000);
        ++i;
    }
}

int main(void)
{
    board_init();
    tos_knl_init();
    tos_mail_q_create(&mail_q, mail_pool, MAIL_MAX, sizeof(mail_t));
    (void)tos_task_create(&task_receiver_higher_prio, "receiver_higher_prio",
                            entry_task_receiver_higher_prio, NULL, PRIO_TASK_RECEIVER_HIGHER_PRIO,
                            stack_task_receiver_higher_prio, STK_SIZE_TASK_RECEIVER, 0);
    (void)tos_task_create(&task_receiver_lower_prio, "receiver_lower_prio",
                            entry_task_receiver_lower_prio, NULL, PRIO_TASK_RECEIVER_LOWER_PRIO,
                            stack_task_receiver_lower_prio, STK_SIZE_TASK_RECEIVER, 0);
    (void)tos_task_create(&task_sender, "sender", entry_task_sender, NULL,
                            5, stack_task_sender, STK_SIZE_TASK_SENDER, 0);
    tos_knl_start();
}

