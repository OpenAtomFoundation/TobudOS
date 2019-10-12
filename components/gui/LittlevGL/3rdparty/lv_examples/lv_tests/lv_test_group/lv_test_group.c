/**
 * @file lv_test_group.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "stdio.h"
#include "lv_test_group.h"
#if LV_USE_GROUP && LV_USE_TESTS

#if LV_EX_KEYBOARD || LV_EX_MOUSEWHEEL
#include "lv_drv_conf.h"
#endif

#if LV_EX_KEYBOARD
#include "lv_drivers/indev/keyboard.h"
#endif

#if LV_EX_MOUSEWHEEL
#include "lv_drivers/indev/mousewheel.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
/*To emulate some keys on the window header*/
static bool win_btn_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static void win_btn_event_handler(lv_obj_t * btn, lv_event_t event);

static void group_focus_cb(lv_group_t * group);

static void general_event_handler(lv_obj_t * obj, lv_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t last_key;
static lv_indev_state_t last_key_state = LV_INDEV_STATE_REL;
static lv_group_t * g;
static lv_obj_t * win;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create base groups to test their functionalities
 */
void lv_test_group_1(void)
{
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    g = lv_group_create();
    lv_group_set_focus_cb(g, group_focus_cb);

    /*A keyboard will be simulated*/
    lv_indev_drv_t sim_kb_drv;
    lv_indev_drv_init(&sim_kb_drv);
    sim_kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    sim_kb_drv.read_cb = win_btn_read;
    lv_indev_t * win_kb_indev = lv_indev_drv_register(&sim_kb_drv);
    lv_indev_set_group(win_kb_indev, g);

#if LV_EX_KEYBOARD
    lv_indev_drv_t real_kb_drv;
    lv_indev_drv_init(&real_kb_drv);
    real_kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    real_kb_drv.read_cb = keyboard_read;
    lv_indev_t * real_kb_indev = lv_indev_drv_register(&real_kb_drv);
    lv_indev_set_group(real_kb_indev, g);
#endif

#if LV_EX_MOUSEWHEEL
    lv_indev_drv_t enc_drv;
    lv_indev_drv_init(&enc_drv);
    enc_drv.type = LV_INDEV_TYPE_ENCODER;
    enc_drv.read_cb = mousewheel_read;
    lv_indev_t * enc_indev = lv_indev_drv_register(&enc_drv);
    lv_indev_set_group(enc_indev, g);
#endif

    /*Create a window to hold all the objects*/
    static lv_style_t win_style;
    lv_style_copy(&win_style, &lv_style_transp);
    win_style.body.padding.left= LV_DPI / 6;
    win_style.body.padding.right = LV_DPI / 6;
    win_style.body.padding.top = LV_DPI / 6;
    win_style.body.padding.bottom = LV_DPI / 6;
    win_style.body.padding.inner = LV_DPI / 6;

    win = lv_win_create(lv_disp_get_scr_act(NULL), NULL);
    lv_win_set_title(win, "Group test");
    lv_page_set_scrl_layout(lv_win_get_content(win), LV_LAYOUT_PRETTY);
    lv_win_set_style(win, LV_WIN_STYLE_CONTENT, &win_style);
    lv_group_add_obj(g, lv_win_get_content(win));

    lv_obj_t * win_btn = lv_win_add_btn(win, LV_SYMBOL_RIGHT);
    lv_obj_set_protect(win_btn, LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = lv_win_add_btn(win, LV_SYMBOL_NEXT);
    lv_obj_set_protect(win_btn, LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = lv_win_add_btn(win, LV_SYMBOL_OK);
    lv_obj_set_protect(win_btn, LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = lv_win_add_btn(win, LV_SYMBOL_PREV);
    lv_obj_set_protect(win_btn, LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = lv_win_add_btn(win, LV_SYMBOL_LEFT);
    lv_obj_set_protect(win_btn, LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = lv_win_add_btn(win, LV_SYMBOL_DUMMY"a");
    lv_obj_set_protect(win_btn, LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_event_cb(win_btn, win_btn_event_handler);

    lv_obj_t * obj;

    obj = lv_spinbox_create(win, NULL);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_spinbox_set_digit_format(obj, 5, 2);
    lv_group_add_obj(g, obj);

    obj = lv_btn_create(win, NULL);
    lv_group_add_obj(g, obj);
    lv_btn_set_toggle(obj, true);
    lv_obj_set_event_cb(obj, general_event_handler);
    obj = lv_label_create(obj, NULL);
    lv_label_set_text(obj, "Button");

    LV_IMG_DECLARE(imgbtn_img_1);
    LV_IMG_DECLARE(imgbtn_img_2);
    obj = lv_imgbtn_create(win, NULL);
    lv_imgbtn_set_src(obj, LV_BTN_STATE_REL, &imgbtn_img_1);
    lv_imgbtn_set_src(obj, LV_BTN_STATE_PR, &imgbtn_img_2);
    lv_group_add_obj(g, obj);

    obj = lv_cb_create(win, NULL);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_slider_create(win, NULL);
    lv_slider_set_range(obj, 0, 10);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_sw_create(win, NULL);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_ddlist_create(win, NULL);
    lv_ddlist_set_options(obj, "Item1\nItem2\nItem3\nItem4\nItem5\nItem6");
    lv_ddlist_set_fix_height(obj, LV_DPI);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_roller_create(win, NULL);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    lv_obj_t * ta = lv_ta_create(win, NULL);
    lv_ta_set_cursor_type(ta, LV_CURSOR_BLOCK);
    lv_obj_set_event_cb(ta, general_event_handler);
    lv_group_add_obj(g, ta);

    obj = lv_kb_create(win, NULL);
    lv_obj_set_size(obj, hres - LV_DPI, vres / 2);
    lv_kb_set_ta(obj, ta);
    lv_kb_set_cursor_manage(obj, true);
    lv_group_add_obj(g, obj);

    static const char * mbox_btns[] = {"Yes", "No", ""};
    obj = lv_mbox_create(win, NULL);
    lv_mbox_add_btns(obj, mbox_btns);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_list_create(win, NULL);
    lv_obj_set_event_cb(obj, general_event_handler);
    const char * list_txts[] = {"File 1", "File 2", "File 3", "File 4", "File 5", "File 6", ""};

    uint32_t i;
    for(i = 0; list_txts[i][0] != '\0'; i++) {
        lv_obj_t * b;
        b = lv_list_add_btn(obj, LV_SYMBOL_FILE, list_txts[i]);
        lv_obj_set_event_cb(b, general_event_handler);
    }

    lv_group_add_obj(g, obj);

    obj = lv_page_create(win, NULL);
    lv_obj_set_size(obj, 2 * LV_DPI, LV_DPI);
    lv_group_add_obj(g, obj);

    obj = lv_label_create(obj, NULL);
    lv_label_set_text(obj, "I'm a page\nwith a long \ntext.\n\n"
                      "You can try \nto scroll me\nwith UP and DOWN\nbuttons.");
    lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);

    obj = lv_tabview_create(win, NULL);
    lv_obj_set_size(obj, hres / 2, vres / 2);
    lv_obj_t * t1 = lv_tabview_add_tab(obj, "Tab 1");
    lv_obj_t * t2 = lv_tabview_add_tab(obj, "Tab 2");
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_label_create(t1, NULL);
    lv_label_set_text(obj, "This is the content\nof the first tab");

    obj = lv_label_create(t2, NULL);
    lv_label_set_text(obj, "This is the content\nof the second tab");
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Read function for the input device which emulates keys on the window header
 * @param indev_drv pointer to the related input device driver
 * @param data store the last key and its state here
 * @return false because the reading in not buffered
 */
static bool win_btn_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    (void) indev_drv;      /*Unused*/

    data->state = last_key_state;
    data->key = last_key;

    return false;
}

/**
 * Called when a control button on the window header is released to change the key state to RELEASED
 * @param btn pointer t to a button on the window header
 * @return LV_RES_OK  because the button is not deleted
 */
static void win_btn_event_handler(lv_obj_t * btn, lv_event_t event)
{
    (void) btn; /*Unused*/

    uint32_t key = 0;

    lv_obj_t * label = lv_obj_get_child(btn, NULL);
    const char * txt = lv_label_get_text(label);

    if(strcmp(txt, LV_SYMBOL_PREV) == 0) key = LV_KEY_PREV;
    else if(strcmp(txt, LV_SYMBOL_NEXT) == 0) key = LV_KEY_NEXT;
    else if(strcmp(txt, LV_SYMBOL_LEFT) == 0) key = LV_KEY_LEFT;
    else if(strcmp(txt, LV_SYMBOL_RIGHT) == 0) key = LV_KEY_RIGHT;
    else if(strcmp(txt, LV_SYMBOL_OK) == 0) key = LV_KEY_ENTER;
    else key = 'a';

    switch(event) {
        case LV_EVENT_PRESSED:
            last_key_state = LV_INDEV_STATE_PR;
            last_key = key;
            break;

        case LV_EVENT_CLICKED:
        case LV_EVENT_PRESS_LOST:
            last_key_state = LV_INDEV_STATE_REL;
            last_key = 0;
            break;
        default:
            break;
    }
}


static void group_focus_cb(lv_group_t * group)
{
    lv_obj_t * f = lv_group_get_focused(group);
    if(f != win) lv_win_focus(win, f, LV_ANIM_ON);
}

static void general_event_handler(lv_obj_t * obj, lv_event_t event)
{
    (void) obj; /*Unused*/

#if LV_EX_PRINTF
    switch(event) {
        case LV_EVENT_PRESSED:
            printf("Pressed\n");
            break;

        case LV_EVENT_SHORT_CLICKED:
            printf("Short clicked\n");
            break;

        case LV_EVENT_CLICKED:
            printf("Clicked\n");
            break;

        case LV_EVENT_LONG_PRESSED:
            printf("Long press\n");
            break;

        case LV_EVENT_LONG_PRESSED_REPEAT:
            printf("Long press repeat\n");
            break;

        case LV_EVENT_VALUE_CHANGED:
            printf("Value changed: %s\n", lv_event_get_data() ? (const char *)lv_event_get_data() : "");
            break;

        case LV_EVENT_RELEASED:
            printf("Released\n");
            break;

        case LV_EVENT_DRAG_BEGIN:
            printf("Drag begin\n");
            break;

        case LV_EVENT_DRAG_END:
            printf("Drag end\n");
            break;

        case LV_EVENT_DRAG_THROW_BEGIN:
            printf("Drag throw begin\n");
            break;

        case LV_EVENT_FOCUSED:
            printf("Foused\n");
            break;
        case LV_EVENT_DEFOCUSED:
            printf("Defocused\n");
            break;
        default:
            break;
    }
#endif
}

#endif /* LV_USE_GROUP && LV_USE_TESTS */
