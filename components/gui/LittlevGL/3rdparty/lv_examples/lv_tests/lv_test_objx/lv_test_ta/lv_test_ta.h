/**
 * @file lv_test_ta.h
 *
 */

#ifndef LV_TEST_TA_H
#define LV_TEST_TA_H

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

#if LV_USE_TA && LV_USE_TESTS

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
 * Create text areas to test their basic functionalities
 */
void lv_test_ta_1(void);

/**
 * Test cursor modes
 */
void lv_test_ta_2(void);

/**********************
 *      MACROS
**********************/

#endif /*LV_USE_TA && LV_USE_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_TA_H*/
