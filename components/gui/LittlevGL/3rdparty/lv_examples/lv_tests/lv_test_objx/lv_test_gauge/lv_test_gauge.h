/**
 * @file lv_test_gauge.h
 *
 */

#ifndef LV_TEST_GAUGE_H
#define LV_TEST_GAUGE_H

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

#if LV_USE_GAUGE && LV_USE_TESTS

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
 * Create gauges to test their functionalities
 */
void lv_test_gauge_1(void);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GAUGE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_USE_GAUGE && LV_USE_TESTS*/
