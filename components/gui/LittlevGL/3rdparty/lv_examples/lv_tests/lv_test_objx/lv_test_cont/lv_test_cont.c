/**
 * @file lv_test_cont.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_cont.h"

#if LV_USE_CONT && LV_USE_TESTS

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
 * Create containers to test their  basic functionalities
 */
void lv_test_cont_1(void)
{
    /* Create a default object*/
    lv_obj_t * cont1 = lv_cont_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(cont1, 10, 10);
    lv_cont_set_style(cont1, LV_CONT_STYLE_MAIN, &lv_style_pretty);

    /*Test fit wit adding two labels*/
    lv_obj_t * cont2 = lv_cont_create(lv_disp_get_scr_act(NULL), cont1);
    lv_cont_set_fit(cont2, LV_FIT_TIGHT);

    lv_obj_t * obj2_1 = lv_label_create(cont2, NULL);
    lv_label_set_text(obj2_1, "Short");

    lv_obj_t * obj2_2 = lv_label_create(cont2, NULL);
    lv_label_set_text(obj2_2, "A longer text");
    lv_obj_set_pos(obj2_2, 80, 30);

    lv_obj_align(cont2, cont1, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);

    /*Test layout and fit togother*/
    lv_obj_t * cont3 = lv_cont_create(lv_disp_get_scr_act(NULL), cont2);
    lv_label_create(cont3, obj2_1);
    lv_label_create(cont3, obj2_2);
    lv_cont_set_layout(cont3, LV_LAYOUT_COL_L);

    lv_obj_align(cont3, cont2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);


    /*Set a new style with new padding*/
    static lv_style_t cont4_style;
    lv_style_copy(&cont4_style, &lv_style_pretty_color);
    cont4_style.body.padding.left = 20;
    cont4_style.body.padding.right = 20;
    cont4_style.body.padding.top = 40;
    cont4_style.body.padding.bottom = 40;
    cont4_style.body.padding.inner = 1;

    lv_obj_t * cont4 = lv_cont_create(lv_disp_get_scr_act(NULL), cont3);
    lv_label_create(cont4, obj2_1);
    lv_label_create(cont4, obj2_2);
    lv_cont_set_style(cont4, LV_CONT_STYLE_MAIN, &cont4_style);

    lv_obj_align(cont4, cont3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

}

/**
 * Test nested style inheritance on padding update
 */
void lv_test_cont_2(void)
{

    /*Create a new style with big paddings*/
    static lv_style_t cont4_style;
    lv_style_copy(&cont4_style, &lv_style_pretty_color);
    cont4_style.body.padding.left = 10;
    cont4_style.body.padding.right = 10;
    cont4_style.body.padding.top = 20;
    cont4_style.body.padding.bottom = 20;
    cont4_style.body.padding.inner = 1;

    /* Create a main container*/
    lv_obj_t * cont_main = lv_cont_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(cont_main, 50, 50);
    lv_cont_set_style(cont_main, LV_CONT_STYLE_MAIN, &lv_style_pretty);
    lv_cont_set_fit(cont_main, LV_FIT_TIGHT);
    lv_cont_set_layout(cont_main, LV_LAYOUT_ROW_M);

    /*Create two containers on the main* with two-two labels*/
    lv_obj_t * cont_sub1 = lv_cont_create(cont_main, NULL);
    lv_cont_set_style(cont_sub1, LV_CONT_STYLE_MAIN, NULL);      /*Inherit style from parent*/
    lv_cont_set_fit(cont_sub1, LV_FIT_TIGHT);
    lv_cont_set_layout(cont_sub1, LV_LAYOUT_COL_M);

    lv_obj_t * obj1_1 = lv_label_create(cont_sub1, NULL);
    lv_label_set_text(obj1_1, "Short");

    lv_obj_t * obj1_2 = lv_label_create(cont_sub1, NULL);
    lv_label_set_text(obj1_2, "A long text");

    lv_obj_t * cont_sub2 = lv_cont_create(cont_main, cont_sub1);
    lv_label_create(cont_sub2, obj1_1);
    lv_label_create(cont_sub2, obj1_2);

    /*Set the new style*/
    lv_cont_set_style(cont_main, LV_CONT_STYLE_MAIN, &cont4_style);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_CONT && LV_USE_TESTS*/
