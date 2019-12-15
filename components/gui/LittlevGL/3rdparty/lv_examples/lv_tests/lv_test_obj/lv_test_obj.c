/**
 * @file lv_test_object.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_obj.h"
#include "../../lv_examples.h"  /*Just to include somewhere to test 'lv_example' version*/
#if LV_USE_TESTS

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
 * Create base objects to test their functionalities
 */
void lv_test_object_1(void)
{
    /* Create a default object and set LV_STYLE_PRETTY_COLOR style */
    lv_obj_t * obj1 = lv_obj_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_style(obj1, &lv_style_plain_color);


    /*Create an object and set a user create style for it*/
    static lv_style_t style_obj2;
    lv_style_copy(&style_obj2, &lv_style_pretty);
    style_obj2.body.main_color = LV_COLOR_RED;
    style_obj2.body.grad_color = LV_COLOR_BLACK;
    style_obj2.body.radius = 0;
    style_obj2.body.border.color = LV_COLOR_WHITE;
    style_obj2.body.border.width = 4;
    style_obj2.body.opa = LV_OPA_50;
    style_obj2.body.shadow.width = 10;

    lv_obj_t * obj2 = lv_obj_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(obj2, 30, 30);
    lv_obj_align(obj2, obj1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style(obj2, &style_obj2);

    /*Test drag, drag_parent, drag throw and copy*/
    lv_obj_t * obj3_parent = lv_obj_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_align(obj3_parent, obj2, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style(obj3_parent, &lv_style_pretty);
    lv_obj_set_drag(obj3_parent, true);
    lv_obj_set_drag_throw(obj3_parent, true);

    lv_obj_t * obj3 = lv_obj_create(obj3_parent, obj2);
    lv_obj_align(obj3, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_click(obj3, true);
    lv_obj_set_drag_parent(obj3, true);

    /*Create a parent and 3 objects on it. Hide the parent but move 2 children to the screen*/

    lv_obj_t * obj4_parent = lv_obj_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(obj4_parent, lv_obj_get_x(obj1) + 10, lv_obj_get_y(obj1) + lv_obj_get_height(obj1) + 20);
    lv_obj_set_style(obj4_parent, &lv_style_pretty_color);
    lv_obj_set_hidden(obj4_parent, true); /*Hide this and all children objects*/

    lv_obj_t * obj4_1 = lv_obj_create(obj4_parent, obj2);
    lv_obj_set_pos(obj4_1, 10, 10);

    lv_obj_t * obj4_2 = lv_obj_create(obj4_parent, obj2);
    lv_obj_set_pos(obj4_2, 20, 20);

    lv_obj_t * obj4_3 = lv_obj_create(obj4_parent, obj2);
    lv_obj_set_pos(obj4_3, 30, 30);

    /*Move two children to the screen (now they will be visible)*/
    lv_obj_set_parent(obj4_2, lv_disp_get_scr_act(NULL));
    lv_obj_align(obj4_2, obj4_parent, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_set_parent(obj4_3, lv_disp_get_scr_act(NULL));
    lv_obj_align(obj4_3, obj4_parent, LV_ALIGN_OUT_RIGHT_MID, 20, 5);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_TESTS*/
