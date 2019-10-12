/**
 * @file lv_test_ddlist.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_test_ddlist.h"

#if LV_EX_PRINTF
#include <stdio.h>
#endif


#if LV_USE_DDLIST && LV_USE_TESTS

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void event_handler(lv_obj_t * ddlist, lv_event_t event);

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
 * Create drop down lists to test their functionalities
 */
void lv_test_ddlist_1(void)
{
    /* Create a default object*/
    lv_obj_t * ddlist1 = lv_ddlist_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(ddlist1, 10, 10);

    /* Create a drop down list with a lot of options, fix height and anim time.
     * Open it by default without animation and assign an action*/
    lv_obj_t * ddlist2 = lv_ddlist_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_align(ddlist2, ddlist1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_ddlist_set_options(ddlist2, "First\nSecond\nThird\nForth\nFifth\nSixth");
    lv_ddlist_set_fix_height(ddlist2, LV_DPI);
    lv_ddlist_set_selected(ddlist2, 2);
    lv_ddlist_set_anim_time(ddlist2, 100);
    lv_ddlist_open(ddlist2, false);
    lv_ddlist_set_fix_width(ddlist2, LV_DPI * 2);
    lv_obj_set_event_cb(ddlist2, event_handler);

    /*Copy the previous drop down list and modify its style*/
    static lv_style_t ddlist3_style;
    lv_style_copy(&ddlist3_style, &lv_style_pretty);
    ddlist3_style.body.main_color = LV_COLOR_GRAY;
    ddlist3_style.body.grad_color = LV_COLOR_BLACK;
    ddlist3_style.body.padding.left = 20;
    ddlist3_style.body.padding.right = 20;
    ddlist3_style.body.padding.top = 30;
    ddlist3_style.body.padding.bottom = 30;

    ddlist3_style.text.color = LV_COLOR_RED;
    ddlist3_style.text.letter_space = 5;
    ddlist3_style.text.line_space = 15;

    lv_obj_t * ddlist3 = lv_ddlist_create(lv_disp_get_scr_act(NULL), ddlist2);
    lv_obj_align(ddlist3, ddlist2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_ddlist_set_style(ddlist3, LV_DDLIST_STYLE_BG, &ddlist3_style);
    lv_ddlist_set_style(ddlist3, LV_DDLIST_STYLE_SEL, &lv_style_plain_color);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void event_handler(lv_obj_t * ddlist, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {

#if LV_EX_PRINTF
    char buf[64];
    lv_ddlist_get_selected_str(ddlist, buf, sizeof(buf));
    printf("New option selected on a drop down list: %s\n", buf);
#endif
    }
}

#endif /*LV_USE_DDLIST && LV_USE_TESTS*/

