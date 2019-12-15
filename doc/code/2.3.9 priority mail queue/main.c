#include "tos.h"
#include "mcu_init.h"

#define STK_SIZE_TASK_RECEIVER      512
#define STK_SIZE_TASK_SENDER        512

#define MAIL_MAX    10

k_stack_t stack_task_receiver[STK_SIZE_TASK_RECEIVER];
k_stack_t stack_task_sender[STK_SIZE_TASK_SENDER];

typedef struct mail_st {
    char   *message;
    int     payload;
} mail_t;

uint8_t mail_pool[MAIL_MAX * sizeof(mail_t)];

k_task_t task_receiver;
k_task_t task_sender;

k_prio_mail_q_t prio_mail_q;

extern void entry_task_receiver(void *arg);
extern void entry_task_sender(void *arg);

void entry_task_receiver(void *arg)
{
    k_err_t err;
    mail_t mail;
    size_t mail_size;

    while (K_TRUE) {
        err = tos_prio_mail_q_pend(&prio_mail_q, &mail, &mail_size, TOS_TIME_FOREVER);
        if (err == K_ERR_NONE) {
            TOS_ASSERT(mail_size == sizeof(mail_t));
            printf("receiver: msg incoming[%s], payload[%d]\n", mail.message, mail.payload);
        }
    }
}

void entry_task_sender(void *arg)
{
    mail_t mail_0, mail_1, mail_2;

    printf("sender: post a mail with priority 2\n");
    mail_2.message = "priority 2";
    mail_2.payload = 2;
    tos_prio_mail_q_post(&prio_mail_q, &mail_2, sizeof(mail_t), 2);

    printf("sender: post a mail with priority 1\n");
    mail_1.message = "priority 1";
    mail_1.payload = 1;
    tos_prio_mail_q_post_all(&prio_mail_q, &mail_1, sizeof(mail_t), 1);

    printf("sender: post a mail with priority 0\n");
    mail_0.message = "priority 0";
    mail_0.payload = 0;
    tos_prio_mail_q_post(&prio_mail_q, &mail_0, sizeof(mail_t), 0);
}

int main(void)
{
    board_init();
    tos_knl_init();
    tos_prio_mail_q_create(&prio_mail_q, mail_pool, MAIL_MAX, sizeof(mail_t));
    (void)tos_task_create(&task_receiver, "receiver", entry_task_receiver, NULL,
                            6, stack_task_receiver, STK_SIZE_TASK_RECEIVER, 0);
    (void)tos_task_create(&task_sender, "sender", entry_task_sender, NULL,
                            5, stack_task_sender, STK_SIZE_TASK_SENDER, 0);
    tos_knl_start();
}

