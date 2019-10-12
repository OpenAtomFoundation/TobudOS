/**
 * @file lv_test_lmeter.h
 *
 */

#ifndef LV_TEST_LMETER_H
#define LV_TEST_LMETER_H

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

#if LV_USE_LMETER && LV_USE_TESTS

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
 * Create line meters to test their functionalities
 */
void lv_test_lmeter_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LMETER && LV_USE_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_LMETER_H*/
