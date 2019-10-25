/**
 * @file demo.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "demo.h"
#if LV_USE_DEMO

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void write_create(lv_obj_t * parent);
static void text_area_event_handler(lv_obj_t * text_area, lv_event_t event);
static void keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event);
#if LV_USE_ANIMATION
static void kb_hide_anim_end(lv_anim_t * a);
#endif
static void list_create(lv_obj_t * parent);
static void chart_create(lv_obj_t * parent);
static void slider_event_handler(lv_obj_t * slider, lv_event_t event);
static void list_btn_event_handler(lv_obj_t * slider, lv_event_t event);
#if LV_DEMO_SLIDE_SHOW
static void tab_switcher(void * tv);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * chart;
static lv_obj_t * ta;
static lv_obj_t * kb;

static lv_style_t style_kb;
static lv_style_t style_kb_rel;
static lv_style_t style_kb_pr;

#if LV_DEMO_WALLPAPER
LV_IMG_DECLARE(img_bubble_pattern)
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a demo application
 */
void demo_create(void)
{
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

#if LV_DEMO_WALLPAPER
    lv_obj_t * wp = lv_img_create(lv_disp_get_scr_act(NULL), NULL);
    lv_img_set_src(wp, &img_bubble_pattern);
    lv_obj_set_width(wp, hres * 4);
    lv_obj_set_protect(wp, LV_PROTECT_POS);
#endif

    static lv_style_t style_tv_btn_bg;
    lv_style_copy(&style_tv_btn_bg, &lv_style_plain);
    style_tv_btn_bg.body.main_color = lv_color_hex(0x487fb7);
    style_tv_btn_bg.body.grad_color = lv_color_hex(0x487fb7);
    style_tv_btn_bg.body.padding.top = 0;
    style_tv_btn_bg.body.padding.bottom = 0;

    static lv_style_t style_tv_btn_rel;
    lv_style_copy(&style_tv_btn_rel, &lv_style_btn_rel);
    style_tv_btn_rel.body.opa = LV_OPA_TRANSP;
    style_tv_btn_rel.body.border.width = 0;

    static lv_style_t style_tv_btn_pr;
    lv_style_copy(&style_tv_btn_pr, &lv_style_btn_pr);
    style_tv_btn_pr.body.radius = 0;
    style_tv_btn_pr.body.opa = LV_OPA_50;
    style_tv_btn_pr.body.main_color = LV_COLOR_WHITE;
    style_tv_btn_pr.body.grad_color = LV_COLOR_WHITE;
    style_tv_btn_pr.body.border.width = 0;
    style_tv_btn_pr.text.color = LV_COLOR_GRAY;

    lv_obj_t * tv = lv_tabview_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(tv, hres, vres);

#if LV_DEMO_WALLPAPER
    lv_obj_set_parent(wp, ((lv_tabview_ext_t *) tv->ext_attr)->content);
    lv_obj_set_pos(wp, 0, -5);
#endif

    lv_obj_t * tab1 = lv_tabview_add_tab(tv, "Write");
    lv_obj_t * tab2 = lv_tabview_add_tab(tv, "List");
    lv_obj_t * tab3 = lv_tabview_add_tab(tv, "Chart");

#if LV_DEMO_WALLPAPER == 0
    /*Blue bg instead of wallpaper*/
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BG, &style_tv_btn_bg);
#endif
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_BG, &style_tv_btn_bg);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_INDIC, &lv_style_plain);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_REL, &style_tv_btn_rel);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_PR, &style_tv_btn_pr);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_TGL_REL, &style_tv_btn_rel);
    lv_tabview_set_style(tv, LV_TABVIEW_STYLE_BTN_TGL_PR, &style_tv_btn_pr);

    write_create(tab1);
    list_create(tab2);
    chart_create(tab3);

#if LV_DEMO_SLIDE_SHOW
    lv_task_create(tab_switcher, 3000, LV_TASK_PRIO_MID, tv);
#endif
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void write_create(lv_obj_t * parent)
{
    lv_page_set_style(parent, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(parent, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    lv_page_set_sb_mode(parent, LV_SB_MODE_OFF);

    static lv_style_t style_ta;
    lv_style_copy(&style_ta, &lv_style_pretty);
    style_ta.body.opa = LV_OPA_30;
    style_ta.body.radius = 0;
    style_ta.text.color = lv_color_hex3(0x222);

    ta = lv_ta_create(parent, NULL);
    lv_obj_set_size(ta, lv_page_get_scrl_width(parent), lv_obj_get_height(parent) / 2);
    lv_ta_set_style(ta, LV_TA_STYLE_BG, &style_ta);
    lv_ta_set_text(ta, "");
    lv_obj_set_event_cb(ta, text_area_event_handler);
    lv_style_copy(&style_kb, &lv_style_plain);
    lv_ta_set_text_sel(ta, true);

    style_kb.body.opa = LV_OPA_70;
    style_kb.body.main_color = lv_color_hex3(0x333);
    style_kb.body.grad_color = lv_color_hex3(0x333);
    style_kb.body.padding.left = 0;
    style_kb.body.padding.right = 0;
    style_kb.body.padding.top = 0;
    style_kb.body.padding.bottom = 0;
    style_kb.body.padding.inner = 0;

    lv_style_copy(&style_kb_rel, &lv_style_plain);
    style_kb_rel.body.opa = LV_OPA_TRANSP;
    style_kb_rel.body.radius = 0;
    style_kb_rel.body.border.width = 1;
    style_kb_rel.body.border.color = LV_COLOR_SILVER;
    style_kb_rel.body.border.opa = LV_OPA_50;
    style_kb_rel.body.main_color = lv_color_hex3(0x333);    /*Recommended if LV_VDB_SIZE == 0 and bpp > 1 fonts are used*/
    style_kb_rel.body.grad_color = lv_color_hex3(0x333);
    style_kb_rel.text.color = LV_COLOR_WHITE;

    lv_style_copy(&style_kb_pr, &lv_style_plain);
    style_kb_pr.body.radius = 0;
    style_kb_pr.body.opa = LV_OPA_50;
    style_kb_pr.body.main_color = LV_COLOR_WHITE;
    style_kb_pr.body.grad_color = LV_COLOR_WHITE;
    style_kb_pr.body.border.width = 1;
    style_kb_pr.body.border.color = LV_COLOR_SILVER;

}

static void text_area_event_handler(lv_obj_t * text_area, lv_event_t event)
{
    (void) text_area;    /*Unused*/

    /*Text area is on the scrollable part of the page but we need the page itself*/
    lv_obj_t * parent = lv_obj_get_parent(lv_obj_get_parent(ta));

    if(event == LV_EVENT_CLICKED) {
        if(kb == NULL) {
            kb = lv_kb_create(parent, NULL);
            lv_obj_set_size(kb, lv_obj_get_width_fit(parent), lv_obj_get_height_fit(parent) / 2);
            lv_obj_align(kb, ta, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
            lv_kb_set_ta(kb, ta);
            lv_kb_set_style(kb, LV_KB_STYLE_BG, &style_kb);
            lv_kb_set_style(kb, LV_KB_STYLE_BTN_REL, &style_kb_rel);
            lv_kb_set_style(kb, LV_KB_STYLE_BTN_PR, &style_kb_pr);
            lv_obj_set_event_cb(kb, keyboard_event_cb);

#if LV_USE_ANIMATION
            lv_anim_t a;
            a.var = kb;
            a.start = LV_VER_RES;
            a.end = lv_obj_get_y(kb);
            a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
            a.path_cb = lv_anim_path_linear;
            a.ready_cb = NULL;
            a.act_time = 0;
            a.time = 300;
            a.playback = 0;
            a.playback_pause = 0;
            a.repeat = 0;
            a.repeat_pause = 0;
            a.user_data = NULL;
            lv_anim_create(&a);
#endif
        }
    }

}

/**
 * Called when the close or ok button is pressed on the keyboard
 * @param keyboard pointer to the keyboard
 * @return
 */
static void keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event)
{
    (void) keyboard;    /*Unused*/

    lv_kb_def_event_cb(kb, event);

    if(event == LV_EVENT_APPLY || event == LV_EVENT_CANCEL) {
#if LV_USE_ANIMATION
        lv_anim_t a;
        a.var = kb;
        a.start = lv_obj_get_y(kb);
        a.end = LV_VER_RES;
        a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
        a.path_cb = lv_anim_path_linear;
        a.ready_cb = kb_hide_anim_end;
        a.act_time = 0;
        a.time = 300;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;
        a.user_data = NULL;
        lv_anim_create(&a);
#else
        lv_obj_del(kb);
        kb = NULL;
#endif
    }
}

static void list_create(lv_obj_t * parent)
{
    lv_coord_t hres = lv_disp_get_hor_res(NULL);

    lv_page_set_style(parent, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(parent, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    lv_page_set_sb_mode(parent, LV_SB_MODE_OFF);

    /*Create styles for the buttons*/
    static lv_style_t style_btn_rel;
    static lv_style_t style_btn_pr;
    lv_style_copy(&style_btn_rel, &lv_style_btn_rel);
    style_btn_rel.body.main_color = lv_color_hex3(0x333);
    style_btn_rel.body.grad_color = LV_COLOR_BLACK;
    style_btn_rel.body.border.color = LV_COLOR_SILVER;
    style_btn_rel.body.border.width = 1;
    style_btn_rel.body.border.opa = LV_OPA_50;
    style_btn_rel.body.radius = 0;

    lv_style_copy(&style_btn_pr, &style_btn_rel);
    style_btn_pr.body.main_color = lv_color_make(0x55, 0x96, 0xd8);
    style_btn_pr.body.grad_color = lv_color_make(0x37, 0x62, 0x90);
    style_btn_pr.text.color = lv_color_make(0xbb, 0xd5, 0xf1);

    lv_obj_t * list = lv_list_create(parent, NULL);
    lv_obj_set_height(list, 2 * lv_obj_get_height(parent) / 3);
    lv_list_set_style(list, LV_LIST_STYLE_BG, &lv_style_transp_tight);
    lv_list_set_style(list, LV_LIST_STYLE_SCRL, &lv_style_transp_tight);
    lv_list_set_style(list, LV_LIST_STYLE_BTN_REL, &style_btn_rel);
    lv_list_set_style(list, LV_LIST_STYLE_BTN_PR, &style_btn_pr);
    lv_obj_align(list, NULL, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 4);

    lv_obj_t * list_btn;
    list_btn = lv_list_add_btn(list, LV_SYMBOL_FILE, "New");
    lv_obj_set_event_cb(list_btn, list_btn_event_handler);

    list_btn = lv_list_add_btn(list, LV_SYMBOL_DIRECTORY, "Open");
    lv_obj_set_event_cb(list_btn, list_btn_event_handler);

    list_btn = lv_list_add_btn(list, LV_SYMBOL_TRASH, "Delete");
    lv_obj_set_event_cb(list_btn, list_btn_event_handler);

    list_btn = lv_list_add_btn(list, LV_SYMBOL_EDIT, "Edit");
    lv_obj_set_event_cb(list_btn, list_btn_event_handler);

    list_btn = lv_list_add_btn(list, LV_SYMBOL_SAVE, "Save");
    lv_obj_set_event_cb(list_btn, list_btn_event_handler);

    list_btn = lv_list_add_btn(list, LV_SYMBOL_WIFI, "WiFi");
    lv_obj_set_event_cb(list_btn, list_btn_event_handler);

    list_btn = lv_list_add_btn(list, LV_SYMBOL_GPS, "GPS");
    lv_obj_set_event_cb(list_btn, list_btn_event_handler);

    lv_obj_t * mbox = lv_mbox_create(parent, NULL);
    lv_mbox_set_text(mbox, "Click a button to copy its text to the Text area ");
    lv_obj_set_width(mbox, hres - LV_DPI);
    static const char * mbox_btns[] = {"Got it", ""};
    lv_mbox_add_btns(mbox, mbox_btns);    /*The default action is close*/
    lv_obj_align(mbox, parent, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 2);
}

#if LV_USE_ANIMATION
static void kb_hide_anim_end(lv_anim_t * a)
{
    lv_obj_del(a->var);
    kb = NULL;
}
#endif

static void chart_create(lv_obj_t * parent)
{

    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    lv_page_set_style(parent, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(parent, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    lv_page_set_scrl_height(parent, lv_obj_get_height(parent));
    lv_page_set_sb_mode(parent, LV_SB_MODE_OFF);

    static lv_style_t style_chart;
    lv_style_copy(&style_chart, &lv_style_pretty);
    style_chart.body.opa = LV_OPA_60;
    style_chart.body.radius = 0;
    style_chart.line.color = LV_COLOR_GRAY;

    chart = lv_chart_create(parent, NULL);
    lv_obj_set_size(chart, 2 * lv_obj_get_width(parent) / 3, lv_obj_get_height(parent) / 2);
    lv_obj_align(chart, NULL,  LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 4);
    lv_chart_set_type(chart, LV_CHART_TYPE_COLUMN);
    lv_chart_set_style(chart, LV_CHART_STYLE_MAIN, &style_chart);
    lv_chart_set_series_opa(chart, LV_OPA_70);
    lv_chart_series_t * ser1;
    ser1 = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_set_next(chart, ser1, 40);
    lv_chart_set_next(chart, ser1, 30);
    lv_chart_set_next(chart, ser1, 47);
    lv_chart_set_next(chart, ser1, 59);
    lv_chart_set_next(chart, ser1, 59);
    lv_chart_set_next(chart, ser1, 31);
    lv_chart_set_next(chart, ser1, 55);
    lv_chart_set_next(chart, ser1, 70);
    lv_chart_set_next(chart, ser1, 82);
    lv_chart_set_next(chart, ser1, 91);

    /*Create a bar, an indicator and a knob style*/
    static lv_style_t style_bar;
    static lv_style_t style_indic;
    static lv_style_t style_knob;

    lv_style_copy(&style_bar, &lv_style_pretty);
    style_bar.body.main_color =  LV_COLOR_BLACK;
    style_bar.body.grad_color =  LV_COLOR_GRAY;
    style_bar.body.radius = LV_RADIUS_CIRCLE;
    style_bar.body.border.color = LV_COLOR_WHITE;
    style_bar.body.opa = LV_OPA_60;
    style_bar.body.padding.left = 0;
    style_bar.body.padding.right = 0;
    style_bar.body.padding.top = LV_DPI / 10;
    style_bar.body.padding.bottom = LV_DPI / 10;

    lv_style_copy(&style_indic, &lv_style_pretty);
    style_indic.body.grad_color =  LV_COLOR_MAROON;
    style_indic.body.main_color =  LV_COLOR_RED;
    style_indic.body.radius = LV_RADIUS_CIRCLE;
    style_indic.body.shadow.width = LV_DPI / 10;
    style_indic.body.shadow.color = LV_COLOR_RED;
    style_indic.body.padding.left = LV_DPI / 30;
    style_indic.body.padding.right = LV_DPI / 30;
    style_indic.body.padding.top = LV_DPI / 30;
    style_indic.body.padding.bottom = LV_DPI / 30;

    lv_style_copy(&style_knob, &lv_style_pretty);
    style_knob.body.radius = LV_RADIUS_CIRCLE;
    style_knob.body.opa = LV_OPA_70;

    /*Create a second slider*/
    lv_obj_t * slider = lv_slider_create(parent, NULL);
    lv_slider_set_style(slider, LV_SLIDER_STYLE_BG, &style_bar);
    lv_slider_set_style(slider, LV_SLIDER_STYLE_INDIC, &style_indic);
    lv_slider_set_style(slider, LV_SLIDER_STYLE_KNOB, &style_knob);
    lv_obj_set_size(slider, lv_obj_get_width(chart), LV_DPI / 3);
    lv_obj_align(slider, chart, LV_ALIGN_OUT_BOTTOM_MID, 0, (vres - chart->coords.y2 - lv_obj_get_height(slider)) / 2); /*Align to below the chart*/
    lv_obj_set_event_cb(slider, slider_event_handler);
    lv_slider_set_range(slider, 10, 1000);
    lv_slider_set_value(slider, 700, false);
    slider_event_handler(slider, LV_EVENT_VALUE_CHANGED);          /*Simulate a user value set the refresh the chart*/
}

/**
 * Called when a new value on the slider on the Chart tab is set
 * @param slider pointer to the slider
 * @return LV_RES_OK because the slider is not deleted in the function
 */
static void slider_event_handler(lv_obj_t * slider, lv_event_t event)
{

    if(event == LV_EVENT_VALUE_CHANGED) {
        int16_t v = lv_slider_get_value(slider);
        v = 1000 * 100 / v; /*Convert to range modify values linearly*/
        lv_chart_set_range(chart, 0, v);
    }
}

/**
 * Called when a a list button is clicked on the List tab
 * @param btn pointer to a list button
 * @return LV_RES_OK because the button is not deleted in the function
 */
static void list_btn_event_handler(lv_obj_t * btn, lv_event_t event)
{

    if(event == LV_EVENT_SHORT_CLICKED) {
        lv_ta_add_char(ta, '\n');
        lv_ta_add_text(ta, lv_list_get_btn_text(btn));
    }
}

#if LV_DEMO_SLIDE_SHOW
/**
 * Called periodically (lv_task) to switch to the next tab
 */
static void tab_switcher(void * tv)
{
    static uint8_t tab = 0;

    tab++;
    if(tab >= 3) tab = 0;
    lv_tabview_set_tab_act(tv, tab, true);
}
#endif


#endif  /*LV_USE_DEMO*/
