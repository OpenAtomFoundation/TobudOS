/**
 * @file lv_test_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>  /*For printf in the action*/
#include "lv_test_sw.h"

#if LV_USE_SW && LV_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void event_handler(lv_obj_t * sw, lv_event_t event);

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
 * Create switches to test their functionalities
 */
void lv_test_sw_1(void)
{
    /* Default object */
    lv_obj_t * sw1 = lv_sw_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(sw1, 10, 10);
    lv_obj_set_event_cb(sw1, event_handler);

    static lv_style_t bg;
    static lv_style_t indic;

    lv_style_copy(&bg, &lv_style_pretty);
    bg.body.padding.left = -5;
    bg.body.padding.right = -5;
    bg.body.padding.top = -5;
    bg.body.padding.bottom = -5;

    lv_style_copy(&indic, &lv_style_pretty_color);
    indic.body.padding.left = 8;
    indic.body.padding.right = 8;
    indic.body.padding.top  = 8;
    indic.body.padding.bottom = 8;

    lv_obj_t * sw2 = lv_sw_create(lv_disp_get_scr_act(NULL), sw1);
    lv_sw_set_style(sw2, LV_SW_STYLE_BG, &bg);
    lv_sw_set_style(sw2, LV_SW_STYLE_INDIC, &indic);
    lv_sw_set_style(sw2, LV_SW_STYLE_KNOB_OFF, &lv_style_btn_pr);
    lv_sw_set_style(sw2, LV_SW_STYLE_KNOB_ON, &lv_style_btn_tgl_pr);

    lv_sw_on(sw2, LV_ANIM_OFF);
    lv_obj_align(sw2, sw1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    lv_obj_t * sw3 = lv_sw_create(lv_disp_get_scr_act(NULL), sw2);
    lv_obj_align(sw3, sw2, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void event_handler(lv_obj_t * sw, lv_event_t event)
{

    if(event == LV_EVENT_VALUE_CHANGED) {
#if LV_EX_PRINTF
        printf("Switch state: %d\n", lv_sw_get_state(sw));
#endif
    }
}


#endif /*LV_USE_SW && LV_USE_TESTS*/
