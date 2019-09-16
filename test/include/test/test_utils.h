#ifndef _TEST_UTILS_H_
#define  _TEST_UTILS_H_

typedef enum test_context_en {
    TEST_CONTEXT_NONE,
    TEST_CONTEXT_00,
    TEST_CONTEXT_01,
    TEST_CONTEXT_02,
    TEST_CONTEXT_03,
    TEST_CONTEXT_04,
    TEST_CONTEXT_05,
    TEST_CONTEXT_06,
    TEST_CONTEXT_07,
} test_context_t;

extern void (*test_task_hook)(void);

extern volatile uint32_t test_count;

extern k_err_t test_err;

extern test_context_t test_context;

extern void *test_msg_addr;

extern size_t test_msg_size;

extern k_event_flag_t test_event_match;

void test_event_reset(void);

void test_event_set(k_event_flag_t event_match);

void test_msg_reset(void);

void test_msg_set(void *msg_addr, size_t msg_size);

void test_err_reset(void);

void test_err_set(k_err_t err);

void test_context_reset(void);

void test_context_set(test_context_t context);

void test_count_reset(void);

void test_count_inc(void);

void test_task_hook_reset(void);

void test_task_hook_set(void (*hook)(void));

#endif

