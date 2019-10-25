/**
 * @file lv_test_btnm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>  /*For printf in the action*/

#include "lv_test_btnm.h"

#if  LV_USE_BTNM && LV_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void btnm_event_cb(lv_obj_t * btnm, lv_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char * btnm_map[] = {"One line", "\n", "\212", "\242Ina", "\204üŰöŐ", "\221éÉ", "\n", "\214", "\202Left", ""};
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create button matrixes to test their functionalities
 */
void lv_test_btnm_1(void)
{
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    /* Default object
     * GOAL: A button matrix with default buttons */
    lv_obj_t * btnm1 = lv_btnm_create(lv_disp_get_scr_act(NULL), NULL);

    /* Test map, size and position. Also try some features.
     * GOAL: A button matrix with default buttons.  */
    static lv_style_t rel;
    lv_style_copy(&rel, &lv_style_btn_tgl_rel);
    rel.body.main_color = LV_COLOR_RED;
    rel.body.grad_color = LV_COLOR_BLACK;
    rel.text.color = LV_COLOR_YELLOW;

    static lv_style_t pr;
    lv_style_copy(&pr, &lv_style_btn_tgl_rel);
    pr.body.main_color = LV_COLOR_ORANGE;
    pr.body.grad_color = LV_COLOR_BLACK;
    pr.text.color = LV_COLOR_WHITE;


    lv_obj_t * btnm2 = lv_btnm_create(lv_disp_get_scr_act(NULL), NULL);
    lv_btnm_set_map(btnm2, btnm_map);
    lv_obj_set_size(btnm2, hres / 2, vres / 3);
    lv_obj_align(btnm2, btnm1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    lv_btnm_set_btn_ctrl(btnm2, 2, LV_BTNM_CTRL_TGL_STATE);
    lv_obj_set_event_cb(btnm2, btnm_event_cb);
    lv_btnm_set_style(btnm2, LV_BTNM_STYLE_BTN_REL, &rel);
    lv_btnm_set_style(btnm2, LV_BTNM_STYLE_BTN_PR, &pr);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void btnm_event_cb(lv_obj_t * btnm, lv_event_t event)
{
    (void) btnm; /*Unused*/

    if(event != LV_EVENT_CLICKED) return;



#if LV_EX_PRINTF
    const char * txt = lv_btnm_get_active_btn_text(btnm);
    if(txt) {
        printf("Key pressed: %s\n", txt);
    }
#endif
}

#endif /* LV_USE_BTNM && LV_USE_TESTS*/
