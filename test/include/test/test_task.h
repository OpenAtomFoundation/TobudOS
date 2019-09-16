#ifndef _TEST_TASK_H_
#define  _TEST_TASK_H_

#define TEST_TASK_STACK_SIZE_00     512
#define TEST_TASK_STACK_SIZE_01     512
#define TEST_TASK_STACK_SIZE_02     512

#define TEST_TASK_PRIO_00           3
#define TEST_TASK_PRIO_01           4
#define TEST_TASK_PRIO_02           5

extern k_stack_t test_task_stack_00[TEST_TASK_STACK_SIZE_00];
extern k_stack_t test_task_stack_01[TEST_TASK_STACK_SIZE_01];
extern k_stack_t test_task_stack_02[TEST_TASK_STACK_SIZE_02];

extern k_task_t test_task_00;
extern k_task_t test_task_01;
extern k_task_t test_task_02;

#endif

