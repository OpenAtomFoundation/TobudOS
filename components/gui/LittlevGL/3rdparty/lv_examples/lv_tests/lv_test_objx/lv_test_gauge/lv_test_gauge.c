/**
 * @file lv_test_gauge.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_gauge.h"

#if LV_USE_GAUGE && LV_USE_TESTS

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
 * Create gauges to test their functionalities
 */
void lv_test_gauge_1(void)
{
    /* Create a default object*/
    lv_obj_t * gauge1 = lv_gauge_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(gauge1, 10, 10);
    lv_gauge_set_value(gauge1, 0, 75);

    /*Copy the previous gauge and set smaller size for it*/
    lv_obj_t * gauge2 = lv_gauge_create(lv_disp_get_scr_act(NULL), gauge1);
    lv_obj_set_size(gauge2, 2 * lv_obj_get_width(gauge1) / 3,  2 * lv_obj_get_height(gauge1) / 3);
    lv_obj_align(gauge2, gauge1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    /*Copy the first gauge add more needles and set new style*/
#if LV_COMPILER_NON_CONST_INIT_SUPPORTED
    static lv_color_t needle_colors[3] = {LV_COLOR_BLUE, LV_COLOR_PURPLE, LV_COLOR_TEAL};
#else
    static lv_color_t needle_colors[3] = { {{0}}, {{0}}, {{0}} };
#endif
    /*Create a styled gauge*/
    static lv_style_t style3;
    lv_style_copy(&style3, &lv_style_pretty);
    style3.body.main_color = LV_COLOR_GREEN;
    style3.body.grad_color = LV_COLOR_RED;
    style3.body.padding.left = 6;
    style3.body.padding.inner = 10;
    style3.body.padding.top = 8;
    style3.body.border.color = LV_COLOR_GRAY;
    style3.line.width = 2;

    lv_obj_t * gauge3 = lv_gauge_create(lv_disp_get_scr_act(NULL), gauge1);
    lv_obj_align(gauge3, gauge1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style(gauge3, &style3);
    lv_gauge_set_scale(gauge3, 270, 41, 5);
    lv_gauge_set_needle_count(gauge3, 3, needle_colors);
    lv_gauge_set_value(gauge3, 0, 20);
    lv_gauge_set_value(gauge3, 1, 40);
    lv_gauge_set_value(gauge3, 2, 60);

    /*Copy the modified 'gauge3' and set a smaller size for it*/
    lv_obj_t * gauge4 = lv_gauge_create(lv_disp_get_scr_act(NULL), gauge3);
    lv_obj_set_size(gauge4, 100, 100);
    lv_obj_align(gauge4, gauge3, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GAUGE && LV_USE_TESTS*/

