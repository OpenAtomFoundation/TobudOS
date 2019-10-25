/**
 * @file lv_test_canvas.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_canvas.h"

#if LV_USE_CANVAS && LV_USE_TESTS

/*********************
 *      DEFINES
 *********************/
#define CANVAS_WIDTH    100
#define CANVAS_HEIGHT   100
#define TEST_ROTATE     0

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
 * Create canvas to test its functionalities
 */
void lv_test_canvas_1(void)
{
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.body.main_color = LV_COLOR_RED;
    style.body.grad_color = LV_COLOR_MAROON;
    style.body.radius = 4;
    style.body.border.width = 2;
    style.body.border.color = LV_COLOR_WHITE;
    style.body.shadow.color = LV_COLOR_WHITE;
    style.body.shadow.width = 4;
    style.line.width = 2;
    style.line.color = LV_COLOR_BLACK;
    style.text.color = LV_COLOR_BLUE;

    lv_obj_t * canvas = lv_canvas_create(lv_scr_act(), NULL);
    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_set_pos(canvas, 10, 10);
    lv_canvas_fill_bg(canvas, LV_COLOR_SILVER);

    lv_canvas_draw_rect(canvas, 40, 10, 50, 30, &style);

    lv_canvas_draw_text(canvas, 5, 5, 100, &style, "ABC", LV_LABEL_ALIGN_LEFT);

    const lv_point_t points[] = {{5, 40}, {35, 45}, {30, 80}, {10, 90}, {5, 40}};

    lv_canvas_draw_polygon(canvas, points, 5, &style);
    lv_canvas_draw_line(canvas, points, 5, &style);

    lv_canvas_draw_arc(canvas, 70, 70, 20, 20, 250, &style);

#if TEST_ROTATE
    /*Copy the current image to buffer and rotate it to the canvas */
    lv_color_t cbuf_tmp[CANVAS_WIDTH * CANVAS_HEIGHT];
    memcpy(cbuf_tmp, cbuf, sizeof(cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (void *)cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = CANVAS_WIDTH;
    img.header.h = CANVAS_HEIGHT;

    lv_canvas_fill_bg(canvas, LV_COLOR_SILVER);
    lv_canvas_rotate(canvas, &img, 30, 0, 0, CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_CANVAS && LV_USE_TESTS*/
