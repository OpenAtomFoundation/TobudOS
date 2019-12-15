/**
 * @file lv_test_canvas.h
 *
 */

#ifndef LV_TEST_CANVAS_H
#define LV_TEST_CANVAS_H

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

#if LV_USE_CANVAS && LV_USE_TESTS

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
 * Create canvas to test its functionalities
 */
void lv_test_canvas_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CANVAS && LV_USE_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_CANVAS_H*/
