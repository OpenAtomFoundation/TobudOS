/**
 * @file indev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "tft.h"
#include "lvgl/src/lv_hal/lv_hal.h"

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_ts.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

/**********************
 *  STATIC VARIABLES
 **********************/
static TS_StateTypeDef  TS_State;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize your input devices here
 */
void touchpad_init(void)
{
    BSP_TS_Init(TFT_HOR_RES, TFT_VER_RES);

    lv_indev_drv_t indev_drv;                       /*Descriptor of an input device driver*/
    lv_indev_drv_init(&indev_drv);                  /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;         /*The touchpad is pointer type device*/
    indev_drv.read_cb = touchpad_read;

    lv_indev_drv_register(&indev_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Read an input device
 * @param indev_id id of the input device to read
 * @param x put the x coordinate here
 * @param y put the y coordinate here
 * @return true: the device is pressed, false: released
 */
static bool touchpad_read(lv_indev_drv_t *indev, lv_indev_data_t *data)
{
    /* Read your touchpad */
    static int16_t last_x = 0;
    static int16_t last_y = 0;
    BSP_LED_Toggle(LED1);

    BSP_TS_GetState(&TS_State);
    if(TS_State.touchDetected) {
            data->point.x = TS_State.touchX[0];
            data->point.y = TS_State.touchY[0];
            last_x = data->point.x;
            last_y = data->point.y;
            data->state = LV_INDEV_STATE_PR;
    } else {
            data->point.x = last_x;
            data->point.y = last_y;
            data->state = LV_INDEV_STATE_REL;
    }

    return false;   /*false: no more data to read because we are no buffering*/
}
