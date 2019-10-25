/**
 * @file lv_test_task.h
 *
 */

#ifndef LV_TEST_TASK_H
#define LV_TEST_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "../../../lvgl/lvgl.h"
#include "../../../lv_ex_conf.h"
#endif

#if LV_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Test the scheduling with various periods and priorities.
 */
void lv_test_task_1(void);

/**
 * Create a lot of short task and see their order. They should be executed according to their priority
 */
void lv_test_task_2(void);

/**
 * Change the priority later
 */
void lv_test_task_3(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_TASK_H*/
