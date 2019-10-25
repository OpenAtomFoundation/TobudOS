/**
 * @file lv_test_kb.h
 *
 */

#ifndef LV_TEST_KB_H
#define LV_TEST_KB_H

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
#include "../../../../lvgl/lvgl.h"
#include "../../../../lv_ex_conf.h"
#endif

#if LV_USE_KB && LV_USE_TESTS

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
 * Create a default object and test the basic functions
 */
void lv_test_kb_1(void);

/**
 * Create a styles keyboard
 */
void lv_test_kb_2(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_KB*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_USE_KB && LV_USE_TESTS*/
