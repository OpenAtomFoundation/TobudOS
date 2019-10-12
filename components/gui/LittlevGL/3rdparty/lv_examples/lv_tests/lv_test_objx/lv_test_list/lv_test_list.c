/**
 * @file lv_test_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_list.h"

#if LV_USE_LIST && LV_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void list_move_event_handler(lv_obj_t * btn, lv_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * list1;
static lv_obj_t * list2;
static lv_obj_t * list3;
static lv_obj_t * list4;

static lv_obj_t * btn_up;
static lv_obj_t * btn_down;

LV_IMG_DECLARE(img_flower_icon) /*Comes from lv_test_img*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create lists to test their functionalities
 */
void lv_test_list_1(void)
{
    /* Default object. It will be an empty list*/
    list1 = lv_list_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(list1, 10, 10);

    list2 = lv_list_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_align(list2, list1, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_list_add_btn(list2, LV_SYMBOL_FILE, "File");
    lv_list_add_btn(list2, LV_SYMBOL_DIRECTORY, "Directory");
    lv_list_add_btn(list2, &img_flower_icon, "Image icon");
    lv_obj_set_width(list2, 100);

    list3 = lv_list_create(lv_disp_get_scr_act(NULL), list2);
    lv_obj_align(list3, list2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_list_add_btn(list3, NULL, "No icon");
    lv_list_add_btn(list3, LV_SYMBOL_CLOSE, "");
    lv_list_add_btn(list3, LV_SYMBOL_UP, "Up");
    lv_list_add_btn(list3, LV_SYMBOL_DOWN, "Down");

    static lv_style_t sb;
    static lv_style_t bg;
    lv_style_copy(&sb, &lv_style_pretty_color);
    lv_style_copy(&bg, &lv_style_pretty_color);
    sb.body.padding.right = -10;
    sb.body.padding.inner = 10;

    bg.body.padding.left = 20;
    bg.body.padding.right = 20;

    list4 = lv_list_create(lv_disp_get_scr_act(NULL), list3);
    lv_list_set_style(list4, LV_LIST_STYLE_BG, &bg);
    lv_list_set_style(list4, LV_LIST_STYLE_SB, &sb);
    lv_obj_align(list4, list3, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_obj_set_width(list4, 200);

    /*Add list up/down buttons*/
    btn_up = lv_btn_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_align(btn_up, list1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_obj_set_event_cb(btn_up, list_move_event_handler);
    lv_obj_t * label = lv_label_create(btn_up, NULL);
    lv_label_set_text(label, LV_SYMBOL_UP);

    btn_down = lv_btn_create(lv_disp_get_scr_act(NULL), btn_up);
    lv_obj_align(btn_down, btn_up, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    label = lv_label_create(btn_down, NULL);
    lv_label_set_text(label, LV_SYMBOL_DOWN);

}



/**********************
 *   STATIC FUNCTIONS
 **********************/

static void list_move_event_handler(lv_obj_t * btn, lv_event_t event)
{
    if(event != LV_EVENT_SHORT_CLICKED) return;

    if(btn == btn_up) {
        lv_list_up(list1);
        lv_list_up(list2);
        lv_list_up(list3);
        lv_list_up(list4);
    } else if(btn == btn_down) {
        lv_list_down(list1);
        lv_list_down(list2);
        lv_list_down(list3);
        lv_list_down(list4);
    }
}

#endif /*LV_USE_LIST && LV_USE_TESTS*/
