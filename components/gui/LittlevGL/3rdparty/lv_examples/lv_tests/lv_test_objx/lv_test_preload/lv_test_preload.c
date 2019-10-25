/**
 * @file lv_test_preload.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_preload.h"
#if LV_USE_PRELOAD && LV_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create preloads to test their functionalities
 */
void lv_test_preload_1(void)
{
    /* Create a default object. It will look strange with the default style*/
    lv_obj_t * preload1 = lv_preload_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(preload1, 10, 10);

    /* Create and apply a style*/
    static lv_style_t style1;
    lv_style_copy(&style1, &lv_style_plain);
    style1.line.color = LV_COLOR_RED;
    style1.line.width = 8;
    style1.line.rounded = 1;
    style1.body.border.width = 2;
    style1.body.border.color = LV_COLOR_MAROON;
    style1.body.padding.left = 3;
    style1.body.padding.right = 3;
    style1.body.padding.top = 3;
    style1.body.padding.bottom = 3;
    lv_obj_t * preload2 = lv_preload_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(preload2, 60, 60);
    lv_preload_set_style(preload2, LV_PRELOAD_STYLE_MAIN, &style1);
    lv_obj_align(preload2, preload1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);


    lv_obj_t * preload3 = lv_preload_create(lv_disp_get_scr_act(NULL), preload2);
    lv_preload_set_arc_length(preload3, 90);
    lv_preload_set_spin_time(preload3, 5000);
    lv_obj_align(preload3, preload2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
//
//    /* Copy 'preload2' and set a new style for it */
//    static lv_style_t style1;
//    lv_style_copy(&style1, &lv_style_plain);
//    style1.line.color = LV_COLOR_RED;
//    style1.line.width = 8;
//    lv_obj_t * preload3 = lv_preload_create(lv_scr_act(NULL), preload2);
//    lv_obj_set_style(preload3, &style1);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_PRELOAD && LV_USE_TESTS*/
