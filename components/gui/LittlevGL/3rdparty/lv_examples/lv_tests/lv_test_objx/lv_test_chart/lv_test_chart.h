/**
 * @file lv_test_chart.h
 *
 */

#ifndef LV_TEST_CHART_H
#define LV_TEST_CHART_H

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

#if LV_USE_BTN && LV_USE_TESTS

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
 * Create charts to test their functionalities
 */
void lv_test_chart_1(void);
void lv_test_chart_2(uint8_t chart);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BTN && LV_USE_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_CHART_H*/
