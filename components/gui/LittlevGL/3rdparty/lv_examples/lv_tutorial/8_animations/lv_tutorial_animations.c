/**
 * @file lv_tutorial_animation.h
 *
 */

/*
 * YOu can add animations to improve the user experience.
 * Basically you can animate any attribute by writing a
 * 'void func(void *ptr, int32_t value)' function.
 * The animation will call a function like this to update the value of 'ptr' to 'value'.
 * Fortunately near all 'set' functions in LittlevGL have a similar prototype.
 * E.g. lv_obj_set_width(lv_obj_t *obj, lv_coord_t w)
 * You will see below how declare and start an animation.
 *
 * There are built in animation which can be started with the lv_obj_anim() function.
 *
 * The other type of animations are the style animations where you can fade
 * one style into an other. See the example below.
 *
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tutorial_animations.h"
#if LV_USE_TUTORIALS && LV_USE_ANIMATION

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
lv_style_t btn3_style;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Crate some objects an animate them
 */
void lv_tutorial_animations(void)
{
    lv_obj_t * scr = lv_disp_get_scr_act(NULL);     /*Get the current screen*/

    lv_obj_t * label;

    /*Create a button the demonstrate built-in animations*/
    lv_obj_t * btn1;
    btn1 = lv_btn_create(scr, NULL);
    lv_obj_set_pos(btn1, 10, 10);       /*Set a position. It will be the animation's destination*/
    lv_obj_set_size(btn1, 80, 50);

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Float");

    /* Float in the button using a built-in function
     * Delay the animation with 2000 ms and float in 300 ms. NULL means no end callback*/
    lv_anim_t a;
    a.var = btn1;
    a.start = -lv_obj_get_height(btn1);
    a.end = lv_obj_get_y(btn1);
    a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
    a.path_cb = lv_anim_path_linear;
    a.ready_cb = NULL;
    a.act_time = -2000; /*Delay the animation*/
    a.time = 300;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 0;
    a.repeat_pause = 0;
    a.user_data = NULL;
    lv_anim_create(&a);

    /*Create a button to demonstrate user defined animations*/
    lv_obj_t * btn2;
    btn2 = lv_btn_create(scr, NULL);
    lv_obj_set_pos(btn2, 10, 80);       /*Set a position. It will be the animation's destination*/
    lv_obj_set_size(btn2, 80, 50);

    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Move");

    /*Create an animation to move the button continuously left to right*/
    a.var = btn2;
    a.start = lv_obj_get_x(btn2);
    a.end = a.start + (100);
    a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_x;
    a.path_cb = lv_anim_path_linear;
    a.ready_cb = NULL;
    a.act_time = -1000;                         /*Negative number to set a delay*/
    a.time = 400;                               /*Animate in 400 ms*/
    a.playback = 1;                             /*Make the animation backward too when it's ready*/
    a.playback_pause = 0;                       /*Wait before playback*/
    a.repeat = 1;                               /*Repeat the animation*/
    a.repeat_pause = 500;                       /*Wait before repeat*/
    lv_anim_create(&a);

    /*Create a button to demonstrate the style animations*/
    lv_obj_t * btn3;
    btn3 = lv_btn_create(scr, NULL);
    lv_obj_set_pos(btn3, 10, 150);       /*Set a position. It will be the animation's destination*/
    lv_obj_set_size(btn3, 80, 50);

    label = lv_label_create(btn3, NULL);
    lv_label_set_text(label, "Style");

    /*Create a unique style for the button*/
    lv_style_copy(&btn3_style, lv_btn_get_style(btn3, LV_BTN_STYLE_REL));
    lv_btn_set_style(btn3, LV_BTN_STATE_REL, &btn3_style);

    /*Animate the new style*/
    lv_anim_t sa;
    lv_style_anim_init(&sa);
    lv_style_anim_set_styles(&sa, &btn3_style, &lv_style_btn_rel, &lv_style_pretty);
    lv_style_anim_set_time(&sa, 500, 500);
    lv_style_anim_set_playback(&sa, 500);
    lv_style_anim_set_repeat(&sa, 500);
    lv_style_anim_create(&sa);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_TUTORIALS && LV_USE_ANIMATION*/

