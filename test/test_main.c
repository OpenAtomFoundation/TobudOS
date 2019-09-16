#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "test/test.h"
#include "greatest/greatest.h"

SUITE_EXTERN(suit_task);
SUITE_EXTERN(suit_mutex);
SUITE_EXTERN(suit_sem);
SUITE_EXTERN(suit_queue);
SUITE_EXTERN(suit_event);
SUITE_EXTERN(suit_msg_queue);
SUITE_EXTERN(suit_mmblk);
SUITE_EXTERN(suit_fifo);
SUITE_EXTERN(suit_robin);
SUITE_EXTERN(suit_timer);


GREATEST_MAIN_DEFS(); 

int application_entry(void *arg)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suit_task);
    RUN_SUITE(suit_mutex);
    RUN_SUITE(suit_sem);
    RUN_SUITE(suit_queue);
    RUN_SUITE(suit_event);
    RUN_SUITE(suit_msg_queue);
    RUN_SUITE(suit_mmblk);
    RUN_SUITE(suit_fifo);
    RUN_SUITE(suit_robin);
    RUN_SUITE(suit_timer);

    GREATEST_MAIN_END();
}

