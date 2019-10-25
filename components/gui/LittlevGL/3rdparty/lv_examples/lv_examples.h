/**
 * @file lv_examples.h
 *
 */

#ifndef LV_EXAMPLES_H
#define LV_EXAMPLES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/
/*Test  lvgl version*/
#define LV_EXAMPLES_LVGL_REQ_MAJOR  6
#define LV_EXAMPLES_LVGL_REQ_MINOR  0
#define LV_EXAMPLES_LVGL_REQ_PATCH  0

#if LV_EXAMPLES_LVGL_REQ_MAJOR != LVGL_VERSION_MAJOR
#error "lv_examples: Wrong lvgl major version"
#endif

#if LV_EXAMPLES_LVGL_REQ_MINOR > LVGL_VERSION_MINOR
#error "lv_examples: Wrong lvgl minor version"
#endif

#if LV_EXAMPLES_LVGL_REQ_PATCH > LVGL_VERSION_PATCH
#error "lv_examples: Wrong lvgl bug fix version"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_EXAMPLES_H*/
