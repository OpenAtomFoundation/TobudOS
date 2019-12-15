#include "test/test.h"

void (*test_task_hook)(void) = K_NULL;

volatile uint32_t test_count = 0;

k_err_t test_err = K_ERR_NONE;

test_context_t test_context = TEST_CONTEXT_NONE;

void *test_msg = K_NULL;

test_mail_t test_mail;
size_t test_mail_size = 0;

k_event_flag_t test_event_match = 0;

void test_event_reset(void)
{
    test_event_match = 0;
}

void test_event_set(k_event_flag_t event_match)
{
    test_event_match = event_match;
}

void test_msg_reset(void)
{
    test_msg = K_NULL;
}

void test_msg_set(void *msg)
{
    test_msg = msg;
}

void test_mail_reset(void)
{
    test_mail.a = 0;
    test_mail.b = K_NULL;
    test_mail.c = 'X';
}

void test_mail_set(void *mail, size_t mail_size)
{
    memcpy(&test_mail, mail, sizeof(test_mail_t));
    test_mail_size = mail_size;
}

void test_err_reset(void)
{
    test_err = K_ERR_NONE;
}

void test_err_set(k_err_t err)
{
    test_err = err;
}

void test_context_reset(void)
{
    test_context = TEST_CONTEXT_NONE;
}

void test_context_set(test_context_t context)
{
    test_context = context;
}

void test_count_reset(void)
{
    test_count = 0x0;
}

void test_count_inc(void)
{
    ++test_count;
}

void test_task_hook_reset(void)
{
    test_task_hook = K_NULL;
}

void test_task_hook_set(void (*hook)(void))
{
    test_task_hook = hook;
}

