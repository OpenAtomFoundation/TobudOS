/**
 * @file lv_test_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_arc.h"
#if LV_USE_ARC && LV_USE_TESTS

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
 * Create arcs to test their functionalities
 */
void lv_test_arc_1(void)
{
    /* Create a default object*/
    lv_obj_t * arc1 = lv_arc_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(arc1, 10, 10);

    /* Modify size, position and angles*/
    lv_obj_t * arc2 = lv_arc_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(arc2, 100, 100);
    lv_obj_align(arc2, arc1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    lv_arc_set_angles(arc2, 0, 250);

    /* Copy 'arc2' and set a new style for it */
    static lv_style_t style1;
    lv_style_copy(&style1, &lv_style_plain);
    style1.line.color = LV_COLOR_RED;
    style1.line.width = 8;
    lv_obj_t * arc3 = lv_arc_create(lv_disp_get_scr_act(NULL), arc2);
    lv_obj_set_style(arc3, &style1);
    lv_obj_align(arc3, arc2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_ARC && LV_USE_TESTS*/
