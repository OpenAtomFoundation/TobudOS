/**
 * @file lv_test_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_line.h"

#if LV_USE_LINE && LV_USE_TESTS

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
 * Create lines to test their functionalities
 */
void lv_test_line_1(void)
{
    static const lv_point_t p[] = {{5, 5}, {60, 5}, {120, 65}};

    /* Create a default object*/
    lv_obj_t * line1 = lv_line_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(line1, 10, 10);
    lv_line_set_points(line1, p, 3);

    /*Test y invert*/
    lv_obj_t * line2 = lv_line_create(lv_disp_get_scr_act(NULL), line1);
    lv_line_set_y_invert(line2, true);
    lv_obj_align(line2, line1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    /*Test styling*/
    static lv_style_t style1;
    lv_style_copy(&style1, &lv_style_plain);

    style1.line.color = LV_COLOR_RED;
    style1.line.width = 20;

    lv_obj_t * line3 = lv_line_create(lv_disp_get_scr_act(NULL), line2);
    lv_line_set_style(line3, LV_LINE_STYLE_MAIN, &style1);
    lv_obj_align(line3, line1, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);
    lv_obj_set_hidden(line3, false);

    /*Test rounding*/
    static lv_style_t style2;
    lv_style_copy(&style2, &style1);
    style2.line.rounded = 1;

    lv_obj_t * line4 = lv_line_create(lv_disp_get_scr_act(NULL), line3);
    lv_line_set_style(line4, LV_LINE_STYLE_MAIN, &style2);
    lv_obj_align(line4, line3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_set_hidden(line4, false);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_LINE && LV_USE_TESTS*/
