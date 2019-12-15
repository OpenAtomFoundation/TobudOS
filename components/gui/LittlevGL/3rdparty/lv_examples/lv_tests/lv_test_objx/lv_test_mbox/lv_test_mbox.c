/**
 * @file lv_test_mbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_mbox.h"
#if LV_USE_MBOX && LV_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void mbox_event_cb(lv_obj_t * mbox, lv_event_t event);

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
 * Create message boxes to test their functionalities
 */
void lv_test_mbox_1(void)
{
    /* Default object */
    lv_obj_t * mbox1 = lv_mbox_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(mbox1, 10, 10);

    /*Add buttons and modify text*/
    static const char * btns2[] = {"Ok", "Cancel", ""};
    lv_obj_t * mbox2 = lv_mbox_create(lv_disp_get_scr_act(NULL), NULL);
    lv_mbox_add_btns(mbox2, btns2);
    lv_mbox_set_text(mbox2, "Message");
    lv_obj_set_width(mbox2, lv_disp_get_hor_res(NULL) / 2);
    lv_obj_align(mbox2, mbox1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    /*Add styles*/
    static lv_style_t bg;
    static lv_style_t btn_bg;
    lv_style_copy(&bg, &lv_style_pretty);
    lv_style_copy(&btn_bg, &lv_style_pretty);
    bg.body.padding.left = 20;
    bg.body.padding.right = 20;
    bg.body.padding.top = 20;
    bg.body.padding.bottom = 20;
    bg.body.padding.inner = 20;
    bg.body.main_color = LV_COLOR_BLACK;
    bg.body.grad_color = LV_COLOR_MAROON;
    bg.text.color = LV_COLOR_WHITE;

    btn_bg.body.padding.left = 10;
    btn_bg.body.padding.right = 10;
    btn_bg.body.padding.top = 5;
    btn_bg.body.padding.bottom = 5;
    btn_bg.body.padding.inner = 40;
    btn_bg.body.opa = LV_OPA_TRANSP;
    btn_bg.body.border.color = LV_COLOR_WHITE;
    btn_bg.text.color = LV_COLOR_WHITE;

    static lv_style_t btn_rel;
    lv_style_copy(&btn_rel, &lv_style_btn_rel);
    btn_rel.body.opa = LV_OPA_TRANSP;
    btn_rel.body.border.color = LV_COLOR_WHITE;

    lv_obj_t * mbox3 = lv_mbox_create(lv_disp_get_scr_act(NULL), mbox2);
    lv_mbox_set_style(mbox3, LV_MBOX_STYLE_BTN_REL, &btn_rel);
    lv_mbox_set_style(mbox3, LV_MBOX_STYLE_BTN_BG,  &btn_bg);
    lv_mbox_set_style(mbox3, LV_MBOX_STYLE_BG, &bg);
    lv_obj_align(mbox3, mbox1, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
    lv_obj_set_event_cb(mbox3, mbox_event_cb);

    /*Copy with styles and set button width*/
    lv_obj_t * mbox4 = lv_mbox_create(lv_disp_get_scr_act(NULL), mbox3);
    lv_mbox_set_text(mbox4, "A quite long message text which is\n"
                     "manually broken into multiple lines");

    static const char * btns3[] = {"Ok", "Cancel", "Third", ""};
    lv_mbox_add_btns(mbox4, btns3);
    lv_obj_set_event_cb(mbox4, mbox_event_cb);
    lv_obj_align(mbox4, mbox3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void mbox_event_cb(lv_obj_t * mbox, lv_event_t event)
{
    if(event != LV_EVENT_CLICKED) return;

    const char * btn_txt = lv_mbox_get_active_btn_text(mbox);
    if(btn_txt) {
        lv_mbox_set_text(mbox, btn_txt);
    }
}

#endif /*LV_USE_MBOX && LV_USE_TESTS*/
