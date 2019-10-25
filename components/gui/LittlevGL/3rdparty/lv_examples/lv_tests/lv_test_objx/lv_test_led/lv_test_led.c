/**
 * @file lv_test_led.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_led.h"

#if LV_USE_LED && LV_USE_TESTS

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
 * Create LEDs to test their functionalities
 */
void lv_test_led_1(void)
{
    /* Create a default object*/
    lv_obj_t * led1 = lv_led_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(led1, 20, 20);

    /*Create styles LED*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_pretty_color);
    style.body.shadow.width = 10;
    style.body.radius = LV_RADIUS_CIRCLE;
    style.body.border.width = 3;
    style.body.border.opa = LV_OPA_30;
    style.body.main_color = lv_color_make(0xb5, 0x0f, 0x04);
    style.body.grad_color = lv_color_make(0x50, 0x07, 0x02);
    style.body.border.color = lv_color_make(0xfa, 0x0f, 0x00);
    style.body.shadow.color = lv_color_make(0xb5, 0x0f, 0x04);

    lv_obj_t * led2 = lv_led_create(lv_disp_get_scr_act(NULL), NULL);
    lv_led_set_style(led2, LV_LED_STYLE_MAIN, &style);
    lv_obj_align(led2, led1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /*Turned ON LED*/
    lv_obj_t * led_on = lv_led_create(lv_disp_get_scr_act(NULL), led2);
    lv_obj_align(led_on, led2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    lv_led_on(led_on);

    /*Tuned OFF LED*/
    lv_obj_t * led_off = lv_led_create(lv_disp_get_scr_act(NULL), led_on);
    lv_obj_align(led_off, led_on, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_led_off(led_off);

    /*Arbitrary brightness*/
    lv_obj_t * led_x = lv_led_create(lv_disp_get_scr_act(NULL), led_off);
    lv_obj_align(led_x, led_off, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_led_set_bright(led_x, 170);


}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_LED && LV_USE_TESTS*/
