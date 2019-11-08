#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE_EXTERN(suit_task);
SUITE_EXTERN(suit_mutex);
SUITE_EXTERN(suit_sem);
SUITE_EXTERN(suit_event);
SUITE_EXTERN(suit_ring_queue);
SUITE_EXTERN(suit_char_fifo);
SUITE_EXTERN(suit_message_queue);
SUITE_EXTERN(suit_mail_queue);
SUITE_EXTERN(suit_binary_heap);
SUITE_EXTERN(suit_priority_queue);
SUITE_EXTERN(suit_priority_message_queue);
SUITE_EXTERN(suit_priority_mail_queue);
SUITE_EXTERN(suit_mmblk);
SUITE_EXTERN(suit_robin);
SUITE_EXTERN(suit_timer);


GREATEST_MAIN_DEFS();

int application_entry(void *arg)
{
    GREATEST_MAIN_BEGIN();

#if TOS_CFG_ROUND_ROBIN_EN > 0u
    RUN_SUITE(suit_robin);
#else
    RUN_SUITE(suit_task);
    RUN_SUITE(suit_mmblk);
    RUN_SUITE(suit_timer);
    RUN_SUITE(suit_mutex);
    RUN_SUITE(suit_sem);
    RUN_SUITE(suit_event);
    RUN_SUITE(suit_ring_queue);
    RUN_SUITE(suit_char_fifo);
    RUN_SUITE(suit_message_queue);
    RUN_SUITE(suit_mail_queue);
    RUN_SUITE(suit_binary_heap);
    RUN_SUITE(suit_priority_queue);
    RUN_SUITE(suit_priority_message_queue);
    RUN_SUITE(suit_priority_mail_queue);
#endif

    GREATEST_MAIN_END();
}

