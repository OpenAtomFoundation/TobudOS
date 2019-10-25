/**
 * @file lv_test.h
 *
 */

#ifndef LV_TEST_H
#define LV_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_ex_conf.h"
#else
#include "../../lv_ex_conf.h"
#endif

#if LV_USE_TESTS

#include "../lv_examples.h"

#include "lv_test_obj/lv_test_obj.h"

#include "lv_test_objx/lv_test_arc/lv_test_arc.h"
#include "lv_test_objx/lv_test_bar/lv_test_bar.h"
#include "lv_test_objx/lv_test_btn/lv_test_btn.h"
#include "lv_test_objx/lv_test_btnm/lv_test_btnm.h"
#include "lv_test_objx/lv_test_cb/lv_test_cb.h"
#include "lv_test_objx/lv_test_canvas/lv_test_canvas.h"
#include "lv_test_objx/lv_test_chart/lv_test_chart.h"
#include "lv_test_objx/lv_test_cont/lv_test_cont.h"
#include "lv_test_objx/lv_test_ddlist/lv_test_ddlist.h"
#include "lv_test_objx/lv_test_gauge/lv_test_gauge.h"
#include "lv_test_objx/lv_test_img/lv_test_img.h"
#include "lv_test_objx/lv_test_imgbtn/lv_test_imgbtn.h"
#include "lv_test_objx/lv_test_kb/lv_test_kb.h"
#include "lv_test_objx/lv_test_label/lv_test_label.h"
#include "lv_test_objx/lv_test_led/lv_test_led.h"
#include "lv_test_objx/lv_test_line/lv_test_line.h"
#include "lv_test_objx/lv_test_list/lv_test_list.h"
#include "lv_test_objx/lv_test_lmeter/lv_test_lmeter.h"
#include "lv_test_objx/lv_test_mbox/lv_test_mbox.h"
#include "lv_test_objx/lv_test_page/lv_test_page.h"
#include "lv_test_objx/lv_test_preload/lv_test_preload.h"
#include "lv_test_objx/lv_test_roller/lv_test_roller.h"
#include "lv_test_objx/lv_test_slider/lv_test_slider.h"
#include "lv_test_objx/lv_test_sw/lv_test_sw.h"
#include "lv_test_objx/lv_test_ta/lv_test_ta.h"
#include "lv_test_objx/lv_test_table/lv_test_table.h"
#include "lv_test_objx/lv_test_tabview/lv_test_tabview.h"
#include "lv_test_objx/lv_test_tileview/lv_test_tileview.h"
#include "lv_test_objx/lv_test_win/lv_test_win.h"

#include "lv_test_theme/lv_test_theme_1.h"
#include "lv_test_theme/lv_test_theme_2.h"

#include "lv_test_group/lv_test_group.h"

#include "lv_test_stress/lv_test_stress.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_TESTS */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_H*/
