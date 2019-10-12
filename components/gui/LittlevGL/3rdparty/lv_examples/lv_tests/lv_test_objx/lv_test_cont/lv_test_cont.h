/**
 * @file lv_test_cont.h
 *
 */

#ifndef LV_TEST_CONT_H
#define LV_TEST_CONT_H

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

#if LV_USE_CONT && LV_USE_TESTS

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
 * Create containers to test their  basic functionalities
 */
void lv_test_cont_1(void);

/**
 * Test nested style inheritance on padding update
 */
void lv_test_cont_2(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CONT && LV_USE_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_CONT_H*/
