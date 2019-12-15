/**
 * @file lv_test_page.h
 *
 */

#ifndef LV_TEST_PAGE_H
#define LV_TEST_PAGE_H

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

#if LV_USE_PAGE && LV_USE_TESTS

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
 * Create pages to test their basic functionalities
 */
void lv_test_page_1(void);

/**
 * Test styling, scrollbar modes, layout and action
 */
void lv_test_page_2(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_PAGE && LV_USE_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_PAGE_H*/
