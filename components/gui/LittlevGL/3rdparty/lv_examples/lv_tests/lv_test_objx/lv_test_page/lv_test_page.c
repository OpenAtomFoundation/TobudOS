/**
 * @file lv_test_page.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_page.h"

#if LV_USE_PAGE && LV_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void event_handler(lv_obj_t * page, lv_event_t event);

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
 * Create pages to test their basic functionalities
 */
void lv_test_page_1(void)
{
    /*Create a page which should look well*/
    lv_obj_t * page1 = lv_page_create(lv_disp_get_scr_act(NULL), NULL);

    /*Resize the page*/
    lv_obj_t * page2 = lv_page_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(page2, LV_DPI, LV_DPI * 2);
    lv_obj_align(page2, page1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /*Add some text to text the scrolling*/
    lv_obj_t * page3 = lv_page_create(lv_disp_get_scr_act(NULL), page2);
    lv_obj_set_size(page3, LV_DPI, LV_DPI * 2);
    lv_obj_align(page3, page2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);

    lv_obj_t * label = lv_label_create(page3, NULL);
    lv_label_set_text(label, "First line of a text\n"
                      "Second line of a text\n"
                      "Third line of a text\n"
                      "Forth line of a text\n"
                      "Fifth line of a text\n"
                      "Sixth line of a text\n"
                      "Seventh line of a text\n"
                      "Eight line of a text\n"
                      "Ninth line of a text\n"
                      "Tenth line of a text\n");

    /*Enable horizontal fit to set scrolling in both directions*/
    lv_obj_t * page4 = lv_page_create(lv_disp_get_scr_act(NULL), page3);
    lv_obj_align(page4, page3, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    label = lv_label_create(page4, label);

}

/**
 * Test styling, scrollbar modes, layout and action
 */
void lv_test_page_2(void)
{
    static lv_style_t bg;
    static lv_style_t scrl;
    static lv_style_t sb;

    lv_style_copy(&bg, &lv_style_pretty);
    lv_style_copy(&scrl, &lv_style_pretty);
    lv_style_copy(&sb, &lv_style_pretty);

    bg.body.main_color = LV_COLOR_SILVER;
    bg.body.grad_color = LV_COLOR_GRAY;
    bg.body.padding.left = 5;
    bg.body.padding.right = 5;
    bg.body.padding.top = 20;
    bg.body.padding.bottom = 20;

    scrl.body.main_color = LV_COLOR_BLUE;
    scrl.body.grad_color = LV_COLOR_NAVY;
    scrl.body.padding.left = 3;
    scrl.body.padding.right = 3;
    scrl.body.padding.top = 3;
    scrl.body.padding.bottom = 3;
    scrl.body.shadow.width = 15;
    scrl.text.color = LV_COLOR_SILVER;

    sb.body.padding.right = -10;    /*Out of the page*/
    sb.body.padding.bottom = 10;
    sb.body.padding.inner = 10;
    sb.body.main_color = LV_COLOR_WHITE;
    sb.body.grad_color = LV_COLOR_WHITE;
    sb.body.opa = LV_OPA_70;

    /* Create a page with new style, layout, fit, action and scrollbar OFF*/
    lv_obj_t * page1 = lv_page_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(page1, LV_DPI, LV_DPI * 2);
    lv_page_set_scrl_layout(page1, LV_LAYOUT_COL_L);
    lv_page_set_sb_mode(page1, LV_SB_MODE_OFF);
    lv_obj_set_event_cb(page1, event_handler);
    lv_page_set_style(page1, LV_PAGE_STYLE_BG, &bg);
    lv_page_set_style(page1, LV_PAGE_STYLE_SCRL, &scrl);
    lv_page_set_style(page1, LV_PAGE_STYLE_SB, &sb);

    lv_obj_t * label = lv_label_create(page1, NULL);
    lv_label_set_text(label, "First line of a text\n"
                      "Second line of a text\n"
                      "Third line of a text\n"
                      "Forth line of a text\n"
                      "Fifth line of a text\n");

    /*Copy 'page1' and set scrollbar ON*/
    lv_obj_t * page2 = lv_page_create(lv_disp_get_scr_act(NULL), page1);
    lv_obj_align(page2, page1, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    label = lv_label_create(page2, label);
    lv_page_set_sb_mode(page2, LV_SB_MODE_ON);

    /*Copy 'page1' and set scrollbar AUTO*/
    lv_obj_t * page3 = lv_page_create(lv_disp_get_scr_act(NULL), page1);
    lv_obj_align(page3, page2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    label = lv_label_create(page3, label);
    lv_page_set_sb_mode(page3, LV_SB_MODE_AUTO);

    /*Copy 'page1' and set scrollbar DRAG*/
    lv_obj_t * page4 = lv_page_create(lv_disp_get_scr_act(NULL), page1);
    lv_obj_align(page4, page3, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    label = lv_label_create(page4, label);
    lv_page_set_sb_mode(page4, LV_SB_MODE_DRAG);


}


/**********************
 *   STATIC FUNCTIONS
 **********************/


static void event_handler(lv_obj_t * page, lv_event_t event)
{
    if(event == LV_EVENT_SHORT_CLICKED) {
        lv_obj_t * label = lv_label_create(page, NULL);
        lv_label_set_text(label, "First line of a text\n"
                "Second line of a text\n"
                "Third line of a text\n"
                "Forth line of a text\n"
                "Fifth line of a text\n");
    }
}


#endif /*LV_USE_PAGE && LV_USE_TESTS*/
