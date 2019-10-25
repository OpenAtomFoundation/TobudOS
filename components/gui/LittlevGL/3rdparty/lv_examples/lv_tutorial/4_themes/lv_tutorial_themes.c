/**
 * @file lv_tutorial_themes.c
 *
 */

/*
 * -------------------------------------------------------------
 * See how the customize with themes much faster and simpler
 * --------------------------------------------------------------
 *
 * To set up styles you need some deeper knowledge about graphics library and
 * requires to be a designer a little bit. In addition it takes quite much time.
 *
 * To address this issue you can use 'themes'.
 * The themes are style collections for every object type with all the required styles.
 *
 * In 'lv_conf.h' you can enable the themes. E.g.: LV_USE_THEME_ALIEN   1
 *
 * When you initialize a theme you can assign a HUE (from HSV color space) and a font:
 * For example to initialize the 'Alien' theme with a greenish color:
 * lv_theme_t *th = lv_theme_alien_init(130, &lv_font_dejavu_40);
 *
 * You have two options to use the themes:
 * 1. Set the styles in it directly: lv_bar_set_style(my_bar, LV_BAR_STYLE_BG, th->bar.bg);
 * 2. Set a system theme and let the library to create objects with the theme's styles
 *    E.g. lv_theme_set_current(th);
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_tutorial_themes.h"
#if LV_USE_TUTORIALS && LV_USE_THEME_ALIEN

#include "../2_objects/lv_tutorial_objects.h"


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
 * Initialize a theme and create the same objects like "lv_tutorial_objects'
 */
void lv_tutorial_themes(void)
{
    /*Initialize the alien theme
     * 210: a green HUE value
     * NULL: use the default font (LV_FONT_DEFAULT)*/
    lv_theme_t * th = lv_theme_alien_init(210, NULL);

    /*Set the surent system theme*/
    lv_theme_set_current(th);

    /*Create the 'lv_tutorial_objects'*/
    lv_tutorial_objects();

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_TUTORIALS*/
