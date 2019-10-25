/**
 * @file lv_tutorial_responsive.h
 *
 */

/*
 * -------------------------------------------------
 * See how to create responsive user interfaces
 * ------------------------------------------------
 *
 * Changing the display to different resolution, updating the GUI design
 * or working with dynamic content are much more easier if you use some
 * useful features of the library and follow a few rules.
 *
 * LV_DPI
 * - In lv_conf.h LV_DPI shows how many pixels are there in 1 inch
 * - You should use it as general unit. For example:
 *     lv_obj_set_pos(btn1, LV_DPI / 2, LV_DPI);
 * - Built-in styles and themes also use this to set padding and sizes.
 *   So lowering LV_DPI will make paddings smaller.
 * - This way changing to higher pixel density display won't brake your design
 *
 * ALIGN
 * - Use the 'lv_obj_align()' function to align the object relative to each other
 *     lv_obj_align(btn1, btn2, LV_ALIGN_OUT_RIGHT_MID, LV_DPI / 2, 0);
 * - It helps to keep an arrangement even is an object is moved
 * - the align happens only once when you call the function.
 *
 * AUTO FIT
 * - The container like objects (lv_cont, lv_btn, lv_page) support auto-fit
 * - It means the object's size will automatically set to include all its children
 * - Can be enabled separately horizontally and vertically
 * - It is useful if you have dynamic content
 * - For example a message box will be as high as its text needs
 * - It uses the style.body.padding.hor/ver to make a padding
 * - Auto-fit runs every time when a children changes (not only once when applied)
 *
 * LAYOUT
 * - You can apply a layout on any container like object
 * - It automatically arranges the children according to a policy
 * - For example `lv_list` uses it to put elements below each other
 * - Layout runs every time when a children changes (not only once when applied)
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tutorial_responsive.h"
#if  LV_USE_TUTORIALS

#include "lvgl/lvgl.h"

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
 * Create some objects an animate them
 */
void lv_tutorial_responsive(void)
{
    lv_obj_t * scr = lv_disp_get_scr_act(NULL);     /*Get the current screen*/

    lv_obj_t * label;

    /*LV_DPI*/
    lv_obj_t * btn1;
    btn1 = lv_btn_create(scr, NULL);
    lv_obj_set_pos(btn1, LV_DPI / 10, LV_DPI / 10);     /*Use LV_DPI to set the position*/
    lv_obj_set_size(btn1, LV_DPI, LV_DPI / 2);          /*Use LVDOI to set the size*/

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "LV_DPI");

    /*ALIGN*/
    lv_obj_t * btn2;
    btn2 = lv_btn_create(scr, btn1);
    lv_obj_align(btn2, btn1, LV_ALIGN_OUT_RIGHT_MID, LV_DPI / 4, 0);

    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Align");

    /*AUTO FIT*/
    lv_obj_t * btn3;
    btn3 = lv_btn_create(scr, btn1);
    lv_btn_set_fit(btn3, LV_FIT_TIGHT);

    label = lv_label_create(btn3, NULL);
    lv_label_set_text(label, "Fit");

    lv_obj_align(btn3, btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, LV_DPI / 4);   /*Align when already resized because of the label*/

    /*LAYOUT*/
    lv_obj_t * btn4;
    btn4 = lv_btn_create(scr, btn1);
    lv_btn_set_fit(btn4, LV_FIT_TIGHT);           /*Enable fit too*/
    lv_btn_set_layout(btn4, LV_LAYOUT_COL_R);   /*Right aligned column layout*/

    label = lv_label_create(btn4, NULL);
    lv_label_set_text(label, "First");

    label = lv_label_create(btn4, NULL);
    lv_label_set_text(label, "Second");

    label = lv_label_create(btn4, NULL);
    lv_label_set_text(label, "Third");

    lv_obj_align(btn4, btn2, LV_ALIGN_OUT_BOTTOM_MID, 0, LV_DPI / 4);   /*Align when already resized because of the label*/

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_TUTORIALS*/
