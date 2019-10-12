#include "tos.h"
#include "lvgl/lvgl.h"
#include "tft.h"
#include "touchpad.h"
#include "lv_examples/lv_apps/demo/demo.h"

void application_entry(void *arg)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        tos_task_delay(50);
    }

    lv_init();
    tft_init();
    touchpad_init();

    demo_create();
    while (1)
    {
        tos_task_delay(5);
        lv_task_handler();
    }    
}


