/**
 * @file lv_test_tabview.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_tabview.h"

#if LV_USE_TABVIEW && LV_USE_TESTS

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
 * Create tab views to test their functionalities
 */
void lv_test_tabview_1(void)
{
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    /* Default object. It will be an empty tab view*/
    lv_obj_t * tv1 = lv_tabview_create(lv_disp_get_scr_act(NULL), NULL);
    lv_tabview_add_tab(tv1, "First");
    lv_tabview_add_tab(tv1, "Second");
    lv_obj_set_size(tv1, hres / 2 - 10, vres / 2 - 10);

    /*Copy the first tabview and add some texts*/
    lv_obj_t * tv2 = lv_tabview_create(lv_disp_get_scr_act(NULL), tv1);
    lv_obj_align(tv2, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    lv_obj_t * tab = lv_tabview_get_tab(tv2, 0);
    lv_obj_t * label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = lv_tabview_get_tab(tv2, 1);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "This is the second tab");


    /*Create styles*/
    static lv_style_t bg;
    static lv_style_t sb;
    static lv_style_t btns_bg;
    static lv_style_t tab_bg;
    static lv_style_t rel;
    static lv_style_t pr;
    static lv_style_t tgl_rel;
    static lv_style_t tgl_pr;
    static lv_style_t indic;

    lv_style_copy(&btns_bg, &lv_style_plain_color);
    lv_style_copy(&tab_bg, &lv_style_pretty_color);
    lv_style_copy(&bg, &lv_style_pretty_color);
    lv_style_copy(&sb, &lv_style_pretty);
    lv_style_copy(&btns_bg, &lv_style_transp_fit);
    lv_style_copy(&rel, &lv_style_plain);
    lv_style_copy(&pr, &lv_style_plain);
    lv_style_copy(&tgl_rel, &lv_style_plain);
    lv_style_copy(&tgl_pr, &lv_style_plain);
    lv_style_copy(&indic, &lv_style_plain);

    rel.body.main_color = LV_COLOR_SILVER;
    pr.body.main_color = LV_COLOR_GRAY;
    tgl_rel.body.main_color = LV_COLOR_RED;
    tgl_pr.body.main_color = LV_COLOR_MAROON;
    indic.body.main_color = LV_COLOR_ORANGE;
    indic.body.grad_color = LV_COLOR_ORANGE;
    indic.body.padding.inner = LV_DPI / 16;
    tab_bg.body.main_color = LV_COLOR_SILVER;
    tab_bg.body.grad_color = LV_COLOR_GREEN;
    tab_bg.text.color = LV_COLOR_YELLOW;

    /*Apply the styles*/
    lv_obj_t * tv3 = lv_tabview_create(lv_disp_get_scr_act(NULL), tv2);
    lv_obj_align(tv3, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_BG, &bg);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_BTN_BG, &btns_bg);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_BTN_REL, &rel);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_BTN_PR, &pr);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_BTN_TGL_REL, &tgl_rel);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_BTN_TGL_PR, &tgl_pr);
    lv_tabview_set_style(tv3, LV_TABVIEW_STYLE_INDIC, &indic);

    tab = lv_tabview_get_tab(tv3, 0);
    lv_page_set_style(tab, LV_PAGE_STYLE_BG, &tab_bg);
    lv_page_set_style(tab, LV_PAGE_STYLE_SB, &sb);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = lv_tabview_get_tab(tv3, 1);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "This is the second tab");

    /*Copy the styles tab view*/
    lv_obj_t * tv4 = lv_tabview_create(lv_disp_get_scr_act(NULL), tv3);
    lv_obj_align(tv4, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
}

void lv_test_tabview_2(void)
{
    lv_obj_t * tab;
    lv_obj_t * label;

    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    // Tabview 1
    lv_obj_t * tv1 = lv_tabview_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(tv1, hres / 2 - 10, vres / 2 - 10);
    lv_obj_align(tv1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_tabview_set_btns_pos(tv1, LV_TABVIEW_BTNS_POS_TOP);

    lv_tabview_add_tab(tv1, "111");
    lv_tabview_add_tab(tv1, "222");
    lv_tabview_add_tab(tv1, "333");
    lv_tabview_add_tab(tv1, "444");

    tab = lv_tabview_get_tab(tv1, 0);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "1111This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = lv_tabview_get_tab(tv1, 1);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "2222");

    tab = lv_tabview_get_tab(tv1, 2);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "3333");

    tab = lv_tabview_get_tab(tv1, 3);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "444");

    // Tabview 2
    lv_obj_t * tv2 = lv_tabview_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(tv2, hres / 2 - 10, vres / 2 - 10);
    lv_obj_align(tv2, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_tabview_set_btns_pos(tv2, LV_TABVIEW_BTNS_POS_BOTTOM);

    lv_tabview_add_tab(tv2, "111");
    lv_tabview_add_tab(tv2, "222");
    lv_tabview_add_tab(tv2, "333");
    lv_tabview_add_tab(tv2, "444");        

    tab = lv_tabview_get_tab(tv2, 0);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "1111This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = lv_tabview_get_tab(tv2, 1);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "2222");

    tab = lv_tabview_get_tab(tv2, 2);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "3333");

    tab = lv_tabview_get_tab(tv2, 3);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "444");

    // Tabview 3
    lv_obj_t * tv3 = lv_tabview_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(tv3, hres / 2 - 10, vres / 2 - 10);
    lv_obj_align(tv3, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_tabview_set_btns_pos(tv3, LV_TABVIEW_BTNS_POS_LEFT);

    lv_tabview_add_tab(tv3, "111");
    lv_tabview_add_tab(tv3, "222");
    lv_tabview_add_tab(tv3, "333");
    lv_tabview_add_tab(tv3, "444");        

    tab = lv_tabview_get_tab(tv3, 0);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "1111This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = lv_tabview_get_tab(tv3, 1);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "2222");

    tab = lv_tabview_get_tab(tv3, 2);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "3333");

    tab = lv_tabview_get_tab(tv3, 3);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "444");

    // Tabview 4
    lv_obj_t * tv4 = lv_tabview_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(tv4, hres / 2 - 10, vres / 2 - 10);
    lv_obj_align(tv4, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    lv_tabview_set_btns_pos(tv4, LV_TABVIEW_BTNS_POS_RIGHT);

    lv_tabview_add_tab(tv4, "111");
    lv_tabview_add_tab(tv4, "222");
    lv_tabview_add_tab(tv4, "333");
    lv_tabview_add_tab(tv4, "444");        

    tab = lv_tabview_get_tab(tv4, 0);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "1111This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = lv_tabview_get_tab(tv4, 1);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "2222");

    tab = lv_tabview_get_tab(tv4, 2);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "3333");

    tab = lv_tabview_get_tab(tv4, 3);
    label = lv_label_create(tab, NULL);
    lv_label_set_text(label, "444");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_TABVIEW && LV_USE_TESTS*/
