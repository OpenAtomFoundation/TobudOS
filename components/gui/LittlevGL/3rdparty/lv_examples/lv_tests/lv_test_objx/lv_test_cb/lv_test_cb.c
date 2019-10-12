/**
 * @file lv_test_cb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_cb.h"

#if LV_USE_CB && LV_USE_TESTS

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
 * Create check boxes to test their functionalities
 */
void lv_test_cb_1(void)
{
    /* Create a default object*/
    lv_obj_t * cb1 = lv_cb_create(lv_disp_get_scr_act(NULL), NULL);

    /*Create an other check box and set its text*/
    lv_obj_t * cb2 = lv_cb_create(lv_disp_get_scr_act(NULL), NULL);
    lv_cb_set_text(cb2, "UTF8-text: üŰ öŐ íÍ");
    lv_obj_align(cb2, cb1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    /*Create styles for the bullets*/
    static lv_style_t cb3_styles[LV_BTN_STATE_NUM];
    lv_style_copy(&cb3_styles[LV_BTN_STATE_REL], &lv_style_plain);
    cb3_styles[LV_BTN_STATE_REL].body.radius = LV_DPI / 20;
    cb3_styles[LV_BTN_STATE_REL].body.border.width = 1;
    cb3_styles[LV_BTN_STATE_REL].body.border.color = LV_COLOR_GRAY;
    cb3_styles[LV_BTN_STATE_REL].body.main_color = LV_COLOR_WHITE;
    cb3_styles[LV_BTN_STATE_REL].body.grad_color = LV_COLOR_SILVER;

    lv_style_copy(&cb3_styles[LV_BTN_STATE_PR], &cb3_styles[LV_BTN_STATE_REL]);
    cb3_styles[LV_BTN_STATE_PR].body.main_color = LV_COLOR_SILVER;
    cb3_styles[LV_BTN_STATE_PR].body.grad_color = LV_COLOR_GRAY;

    lv_style_copy(&cb3_styles[LV_BTN_STATE_TGL_REL], &cb3_styles[LV_BTN_STATE_REL]);
    cb3_styles[LV_BTN_STATE_TGL_REL].body.border.width = 4;
    cb3_styles[LV_BTN_STATE_TGL_REL].body.border.color = LV_COLOR_WHITE;
    cb3_styles[LV_BTN_STATE_TGL_REL].body.border.opa = LV_OPA_70;
    cb3_styles[LV_BTN_STATE_TGL_REL].body.main_color = LV_COLOR_GRAY;
    cb3_styles[LV_BTN_STATE_TGL_REL].body.grad_color = LV_COLOR_BLACK;

    lv_style_copy(&cb3_styles[LV_BTN_STATE_TGL_PR], &cb3_styles[LV_BTN_STATE_TGL_REL]);
    cb3_styles[LV_BTN_STATE_TGL_PR].body.border.color = LV_COLOR_SILVER;
    cb3_styles[LV_BTN_STATE_TGL_PR].body.border.opa = LV_OPA_70;
    cb3_styles[LV_BTN_STATE_TGL_PR].body.main_color = LV_COLOR_GRAY;
    cb3_styles[LV_BTN_STATE_TGL_PR].body.grad_color = LV_COLOR_BLACK;

    lv_style_copy(&cb3_styles[LV_BTN_STATE_INA], &cb3_styles[LV_BTN_STATE_TGL_REL]);
    cb3_styles[LV_BTN_STATE_INA].body.border.width = 1;
    cb3_styles[LV_BTN_STATE_INA].body.border.color = LV_COLOR_GRAY;
    cb3_styles[LV_BTN_STATE_INA].body.main_color = LV_COLOR_SILVER;
    cb3_styles[LV_BTN_STATE_INA].body.grad_color = LV_COLOR_SILVER;


    /*Copy the previous check box and apply the new styles*/
    lv_obj_t * cb3 = lv_cb_create(lv_disp_get_scr_act(NULL), cb2);
    lv_cb_set_style(cb3, LV_CB_STYLE_BOX_REL, &cb3_styles[LV_BTN_STATE_REL]);
    lv_cb_set_style(cb3, LV_CB_STYLE_BOX_PR, &cb3_styles[LV_BTN_STATE_PR]);
    lv_cb_set_style(cb3, LV_CB_STYLE_BOX_TGL_REL, &cb3_styles[LV_BTN_STATE_TGL_REL]);
    lv_cb_set_style(cb3, LV_CB_STYLE_BOX_TGL_PR, &cb3_styles[LV_BTN_STATE_TGL_PR]);
    lv_cb_set_style(cb3, LV_CB_STYLE_BOX_INA, &cb3_styles[LV_BTN_STATE_INA]);
    lv_obj_align(cb3, cb2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    /*Copy the previous check box and set it to INACTIVE*/
    lv_obj_t * cb4 = lv_cb_create(lv_disp_get_scr_act(NULL), cb3);
    lv_obj_align(cb4, cb3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_btn_set_state(cb4, LV_BTN_STATE_INA);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_CB && LV_USE_TESTS*/
