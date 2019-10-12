/**
 * @file lv_tutorial_antialiasing.h
 *
 */

/*
 * --------------------------------------------------------------
 * Learn how to make the drawings smoother with anti aliasing
 * --------------------------------------------------------------
 *
 * You have several options to make your GUI smoother:
 *
 * 1. ANTI-ALIASED DRAWING
 *  By setting LV_ANTIALAIS  1 in lv_conf.h the library will draw
 *  smooth lines and curves (radius of rectangles). It has very low
 *  resource requirement because only the required pixels are calculated
 *  on the edges.
 *
 * 2. HIGHER BPP FONTS
 *  You enable can built-in fonts in lv_conf.h.
 *  By setting for example LV_FONT_DEJAVU_20  4 the font will describe one pixel
 *  with 4 bits meaning 16 values for one pixel. It will result smoother letters.
 *  The possible values are 1, 2, 4 or 8.  Not that the size of the font is increasing
 *  by increasing the bpp.
 *  With the font converter tool you can also create your own fonts with the desired bpp:
 *  https://littlevgl.com/ttf-font-to-c-array
 *
 * 3. PIXEL LEVEL OPACITY ON IMAGES
 *  In the font converter you can enable 'Transparency: Alpha byte' which
 *  will add an alpha byte the every pixel. It ensures smooth edges and holes on images.
 *  Check the Image converter here: https://littlevgl.com/image-to-c-array
 *
 * Try the example by changing the following settings in lv_conf.h:
 * - LV_ANTIALAIS 0 or 1
 * - LV_FONT_DEJAVU_...     1 or 2 or 4 or 8
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tutorial_antialiasing.h"
#if LV_USE_TUTORIALS

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
LV_IMG_DECLARE(apple_icon_chroma)
LV_IMG_DECLARE(apple_icon_alpha)

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create object to see how they change from the anti aliasing
 */
void lv_tutorial_antialiasing(void)
{
    lv_obj_t * scr = lv_disp_get_scr_act(NULL);     /*Get the current screen*/

    lv_obj_t * label;

    static lv_style_t style1;
    lv_style_copy(&style1, &lv_style_btn_rel);
    style1.body.radius = 20;
    style1.body.border.width = 4;

    lv_obj_t * btn1;
    btn1 = lv_btn_create(scr, NULL);
    lv_obj_set_pos(btn1, 10, 10);
    lv_obj_set_size(btn1, 100, 60);
    lv_btn_set_style(btn1, LV_BTN_STYLE_REL, &style1);

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button");

    /*Crate an image which is NOT automatically upscaled to compensate the anti aliasing*/
    lv_obj_t * img_normal = lv_img_create(scr, NULL);
    lv_img_set_src(img_normal, &apple_icon_chroma);
    lv_obj_align(img_normal, btn1, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);

    /*Crate an image which is automatically upscaled to compensate the anti aliasing*/
    lv_obj_t * img_alpha_byte = lv_img_create(scr, img_normal); /*Crate an image object*/
    lv_img_set_src(img_alpha_byte, &apple_icon_alpha);
    lv_obj_align(img_alpha_byte, img_normal, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_TUTORIALS*/
