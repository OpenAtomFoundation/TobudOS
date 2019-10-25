/**
 * @file lv_test_lmeter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>

#include "lv_test_lmeter.h"

#if LV_USE_LMETER && LV_USE_TESTS

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
 * Create line meters to test their functionalities
 */
void lv_test_lmeter_1(void)
{
    /* Create a default object*/
    lv_obj_t * lmeter1 = lv_lmeter_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(lmeter1, 10, 10);
    lv_lmeter_set_value(lmeter1, 60);

    /*Copy the previous line meter and set smaller size for it*/
    lv_obj_t * lmeter2 = lv_lmeter_create(lv_disp_get_scr_act(NULL), lmeter1);
    lv_obj_set_size(lmeter2, LV_DPI / 2,  LV_DPI / 2);
    lv_obj_align(lmeter2, lmeter1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    /*Create a styled line meter*/
    static lv_style_t style3;
    lv_style_copy(&style3, &lv_style_pretty);
    style3.body.main_color = LV_COLOR_GREEN;
    style3.body.grad_color = LV_COLOR_RED;
    style3.body.padding.left = 4;
    style3.body.border.color = LV_COLOR_GRAY;      /*Means the needle middle*/
    style3.line.width = 2;
    style3.line.color = LV_COLOR_SILVER;

    lv_obj_t * lmeter3 = lv_lmeter_create(lv_disp_get_scr_act(NULL), lmeter1);
    lv_obj_align(lmeter3, lmeter1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style(lmeter3, &style3);
    lv_lmeter_set_scale(lmeter3, 270, 41);
    lv_lmeter_set_range(lmeter3, -100, 100);
    lv_lmeter_set_value(lmeter3, 50);

    /*Copy the modified 'lmeter3' and set a smaller size for it*/
    lv_obj_t * lmeter4 = lv_lmeter_create(lv_disp_get_scr_act(NULL), lmeter3);
    lv_obj_set_size(lmeter4, 60, 60);
    lv_obj_align(lmeter4, lmeter3, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_LMETER && LV_USE_TESTS*/
