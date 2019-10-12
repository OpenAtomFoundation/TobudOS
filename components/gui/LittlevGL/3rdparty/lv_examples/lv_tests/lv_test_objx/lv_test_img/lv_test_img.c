/**
 * @file lv_test_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_img.h"

#if LV_USE_IMG && LV_USE_TESTS

/*********************
 *      DEFINES
 *********************/
LV_IMG_DECLARE(img_flower_icon)

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
 * Create images to test their functionalities
 */
void lv_test_img_1(void)
{
    /*Create an image object from a varibale*/
    lv_obj_t * img1 = lv_img_create(lv_disp_get_scr_act(NULL), NULL);
    lv_img_set_src(img1, &img_flower_icon);
    lv_obj_set_pos(img1, 10, 10);

    /*Copy the previous image and set a redish style*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.image.color = LV_COLOR_RED;
    style.image.intense = LV_OPA_70;

    lv_obj_t * img2 = lv_img_create(lv_disp_get_scr_act(NULL), img1);
    lv_img_set_style(img2, LV_IMG_STYLE_MAIN, &style);
    lv_obj_align(img2, img1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /*Copy the previous image and test the mosaic feature*/
    lv_obj_t * img3 = lv_img_create(lv_disp_get_scr_act(NULL), img2);
    lv_obj_set_size(img3, 100, 100);
    lv_obj_align(img3, img2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /*Test symbol drawing*/
    lv_obj_t * img4 = lv_img_create(lv_disp_get_scr_act(NULL), NULL);
    lv_img_set_src(img4, LV_SYMBOL_SETTINGS LV_SYMBOL_OK);
    lv_obj_align(img4, img3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_IMG && LV_USE_TESTS*/
