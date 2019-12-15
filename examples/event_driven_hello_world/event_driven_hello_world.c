#include "tos_evtdrv.h"

typedef struct event_hdr_st {
    uint8_t     event;
} event_hdr_t;

typedef struct consumer_msg_st {
    event_hdr_t     hdr;
    uint16_t        require_len;
} consumer_msg_t;

typedef struct productor_msg_st {
    event_hdr_t     hdr;
    uint16_t        payload_len;
    char           *payload;
} productor_msg_t;

evtdrv_task_id_t productor_id = 0;
evtdrv_task_id_t consumer1_id = 1;
evtdrv_task_id_t consumer2_id = 2;

#define EVENT_CONSUMER1     (1u << 0)
#define EVENT_CONSUMER2     (1u << 1)
#define EVENT_PRODUCTOR     (1u << 2)

#define EVENT_CONSUER1_HUNGRY   (1u << 3)
#define EVENT_CONSUER2_HUNGRY   (1u << 4)

#define CONSUMER1_REQUIRE_LEN       5
#define CONSUMER2_REQUIRE_LEN       9

static void msg_payload_fill(char *payload, uint16_t len)
{
    uint16_t i = 0;

    for (i = 0; i < len - 1; ++i) {
        payload[i] = (i % 10) + 'a';
    }
    payload[len - 1] = '\0';
}

productor_msg_t *msg4consumer_forge(uint16_t consumer_require_len)
{
    productor_msg_t *productor_msg;
    evtdrv_msg_body_t msg_body;

    msg_body = tos_evtdrv_msg_alloc(sizeof(productor_msg_t) + consumer_require_len + 1);
    if (!msg_body) {
        return K_NULL;
    }

    productor_msg               = (productor_msg_t *)msg_body;
    productor_msg->hdr.event    = EVENT_PRODUCTOR;
    productor_msg->payload_len  = consumer_require_len;

    productor_msg->payload      = (char *)(productor_msg + 1);
    msg_payload_fill(productor_msg->payload, consumer_require_len + 1);

    return productor_msg;
}

evtdrv_event_flag_t task_productor(evtdrv_event_flag_t event)
{
    uint16_t consumer_require_len;
    consumer_msg_t *consumer_msg;
    productor_msg_t *productor_msg;

    if (event & TOS_EVTDRV_SYS_EVENT_INIT) {
        printf("init logic for productor\n");

        // return event handled
        return TOS_EVTDRV_SYS_EVENT_INIT;
    }

    if (event & TOS_EVTDRV_SYS_EVENT_MSG) {
        while (consumer_msg = (consumer_msg_t *)tos_evtdrv_msg_recv()) {

            consumer_require_len = consumer_msg->require_len;

            switch (consumer_msg->hdr.event) {
                case EVENT_CONSUMER1:
                    printf("msg from consumer1, %d len required\n", consumer_require_len);

                    productor_msg = msg4consumer_forge(consumer_require_len);
                    if (productor_msg) {
                        tos_evtdrv_msg_send(consumer1_id, (evtdrv_msg_body_t)productor_msg);
                    }

                    break;

                case EVENT_CONSUMER2:
                    printf("msg from consumer2, %d len required\n", consumer_require_len);

                    productor_msg = msg4consumer_forge(consumer_require_len);
                    if (productor_msg) {
                        tos_evtdrv_msg_send(consumer2_id, (evtdrv_msg_body_t)productor_msg);
                    }

                    break;
            }

            tos_evtdrv_msg_free((evtdrv_msg_body_t)consumer_msg);
        }

        // return event handled
        return TOS_EVTDRV_SYS_EVENT_MSG;
    }

    if (event & EVENT_CONSUER1_HUNGRY) {
        productor_msg = msg4consumer_forge(CONSUMER1_REQUIRE_LEN);
        if (productor_msg) {
            tos_evtdrv_msg_send(consumer1_id, (evtdrv_msg_body_t)productor_msg);
        }

        // return event handled
        return EVENT_CONSUER1_HUNGRY;
    }

    if (event & EVENT_CONSUER2_HUNGRY) {
        productor_msg = msg4consumer_forge(CONSUMER2_REQUIRE_LEN);
        if (productor_msg) {
            tos_evtdrv_msg_send(consumer2_id, (evtdrv_msg_body_t)productor_msg);
        }

        // return event handled
        return EVENT_CONSUER2_HUNGRY;
    }

    return TOS_EVTDRV_EVENT_NONE;
}

consumer_msg_t *msg4productor_forge(uint16_t consumer_event, uint16_t consumer_require_len)
{
    consumer_msg_t *consumer_msg;
    evtdrv_msg_body_t msg_body;

    msg_body = tos_evtdrv_msg_alloc(sizeof(consumer_msg_t));
    if (!msg_body) {
        return K_NULL;
    }

    consumer_msg                = (consumer_msg_t *)msg_body;
    consumer_msg->hdr.event     = consumer_event;
    consumer_msg->require_len   = consumer_require_len;

    return consumer_msg;
}

evtdrv_timer_t consumer1_timer;
evtdrv_timer_t consumer2_timer;

void consumer1_timer_callback(void *arg)
{
    printf("consumer1_timer_callback\n");
    tos_evtdrv_event_set(productor_id, EVENT_CONSUER1_HUNGRY);
}

void consumer2_timer_callback(void *arg)
{
    printf("consumer2_timer_callback\n");
    tos_evtdrv_event_set(productor_id, EVENT_CONSUER2_HUNGRY);
}

evtdrv_event_flag_t task_consumer1(evtdrv_event_flag_t event)
{
    consumer_msg_t *consumer_msg;
    productor_msg_t *productor_msg;

    if (event & TOS_EVTDRV_SYS_EVENT_INIT) {
        printf("init logic for consumer1\n");

        tos_evtdrv_timer_create(&consumer1_timer,
                                    consumer1_timer_callback, NULL,
                                    EVTDRV_TIMER_OPT_PERIODIC);
        tos_evtdrv_timer_start(&consumer1_timer, 4000);

        consumer_msg = msg4productor_forge(EVENT_CONSUMER1, CONSUMER1_REQUIRE_LEN);
        if (consumer_msg) {
            tos_evtdrv_msg_send(productor_id, (evtdrv_msg_body_t)consumer_msg);
        }

        // return event handled
        return TOS_EVTDRV_SYS_EVENT_INIT;
    }

    if (event & TOS_EVTDRV_SYS_EVENT_MSG) {
        while (productor_msg = (productor_msg_t *)tos_evtdrv_msg_recv()) {

            switch (productor_msg->hdr.event) {
                case EVENT_PRODUCTOR:
                    printf("\nconsumer1:\nmsg from productor, payload_len: %d\npayload: %s\n", productor_msg->payload_len, productor_msg->payload);

                    break;
            }

            tos_evtdrv_msg_free((evtdrv_msg_body_t)productor_msg);
        }

        // return event handled
        return TOS_EVTDRV_SYS_EVENT_MSG;
    }

    return TOS_EVTDRV_EVENT_NONE;
}

evtdrv_event_flag_t task_consumer2(evtdrv_event_flag_t event)
{
    consumer_msg_t *consumer_msg;
    productor_msg_t *productor_msg;

    if (event & TOS_EVTDRV_SYS_EVENT_INIT) {
        printf("init logic for consumer2\n");

        tos_evtdrv_timer_create(&consumer2_timer,
                                    consumer2_timer_callback, NULL,
                                    EVTDRV_TIMER_OPT_PERIODIC);
        tos_evtdrv_timer_start(&consumer2_timer, 6000);

        consumer_msg = msg4productor_forge(EVENT_CONSUMER2, CONSUMER2_REQUIRE_LEN);
        if (consumer_msg) {
            tos_evtdrv_msg_send(productor_id, (evtdrv_msg_body_t)consumer_msg);
        }

        // return event handled
        return TOS_EVTDRV_SYS_EVENT_INIT;
    }

    if (event & TOS_EVTDRV_SYS_EVENT_MSG) {
        while (productor_msg = (productor_msg_t *)tos_evtdrv_msg_recv()) {

            switch (productor_msg->hdr.event) {
                case EVENT_PRODUCTOR:
                    printf("\nconsumer2:\nmsg from productor, payload_len: %d\npayload: %s\n", productor_msg->payload_len, productor_msg->payload);

                    break;
            }

            tos_evtdrv_msg_free((evtdrv_msg_body_t)productor_msg);
        }

        // return event handled
        return TOS_EVTDRV_SYS_EVENT_MSG;
    }

    return TOS_EVTDRV_EVENT_NONE;
}

evtdrv_task_entry_t tasks[] = {
    task_productor,
    task_consumer1,
    task_consumer2
};

int main(void)
{
    board_init();

    tos_evtdrv_sys_init(tasks, sizeof(tasks) / sizeof(tasks[0]), K_NULL);
    tos_evtdrv_sys_start();
}

