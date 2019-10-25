/**
 * @file lv_test_label.h
 *
 */

#ifndef LV_TEST_LABEL_H
#define LV_TEST_LABEL_H

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

#if LV_USE_LABEL && LV_USE_TESTS

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
 * Create labels with dynamic, static and array texts
 */
void lv_test_label_1(void);

/**
 * Test label long modes
 */
void lv_test_label_2(void);

/**
 * Test text insert and cut
 */
void lv_test_label_3(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LABEL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_USE_LABEL && LV_USE_TESTS*/
