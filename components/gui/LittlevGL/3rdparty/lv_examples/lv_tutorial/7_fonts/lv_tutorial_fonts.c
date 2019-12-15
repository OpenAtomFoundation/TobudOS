/**
 * @file lv_tutorial_fonts.h
 *
 */

/*
 * --------------------------------------------------------------
 * How to add new fonts (even Unicode) to the graphic library
 * --------------------------------------------------------------
 *
 * BUILT-IN FONTS
 * - There are several built in font in the library which can be enabled in lv_conf.h
 * - For example: lv_font_roboto_16,

 * USING A FONT
 * - Just set a font in style.text.font
 * - For example: style.text.font = &lv_font_roboto_16
 *
 * SYMBOLS
 * - There are symbols as well which stored as fonts.
 * - To reference a symbol use the defines LV_SYMBOL_... (LV_SYMBOL_FILE, LV_SYMBOL_OK, etc see lv_symbol_def.h)
 *
 * BIT-PER-PIXEL
 * - The fonts can describe a pixel with 1, 2 or 4 bit. The higher value results smoother letters
 * but larger foot memory foot print.
 *
 * ADDING NEW FONTS
 *  - You can generate your own fonts using the online font converter tool:
 *    https://littlevgl.com/ttf-font-to-c-array
 *  - All information is provided on the website
 *
 */



/*********************
 *      INCLUDES
 *********************/
#include "lv_tutorial_fonts.h"
#if LV_USE_TUTORIALS

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
LV_FONT_DECLARE(arial_20)        /*Declare a font*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Concat two font and create label with Unicode characters
 */
void lv_tutorial_fonts(void)
{
    /*Create a style and use the new font*/
    static lv_style_t style1;
    lv_style_copy(&style1, &lv_style_plain);
    style1.text.font = &arial_20; /*Set the base font whcih is concatenated with the others*/

    /*Create a label and set new text*/
    lv_obj_t * label = lv_label_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(label, 10, 10);
    lv_label_set_style(label, LV_LABEL_STYLE_MAIN, &style1);
    lv_label_set_text(label, "Hello\nпривет\n∞∑");      /*Use ASCII and Cyrillic letters together*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_TUTORIALS*/
