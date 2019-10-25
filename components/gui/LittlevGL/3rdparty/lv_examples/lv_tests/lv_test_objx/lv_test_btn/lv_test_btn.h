/**
 * @file lv_test_btn.h
 *
 */

#ifndef LV_TEST_BTN_H
#define LV_TEST_BTN_H

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
 * Create buttons to test their functionalities
 */
void lv_test_btn_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BTN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_USE_BTN && LV_USE_TESTS*/
