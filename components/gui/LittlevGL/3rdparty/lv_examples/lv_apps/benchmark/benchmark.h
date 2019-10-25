/**
 * @file benchmark.h
 *
 */

#ifndef BENCHMARK_H
#define BENCHMARK_H

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
#include "../../../lvgl/lvgl.h"
#include "../../../lv_ex_conf.h"
#endif

#if LV_USE_BENCHMARK


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
 * Open a graphics benchmark
 */
void benchmark_create(void);

void benchmark_start(void);

bool benchmark_is_ready(void);

uint32_t benchmark_get_refr_time(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BENCHMARK*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BENCHMARK_H */
