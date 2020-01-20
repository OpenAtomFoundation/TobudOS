#include "tos_k.h"
#include "lvgl/lvgl.h"
#include "tft.h"
#include "touchpad.h"
#include "lv_examples/lv_apps/demo/demo.h"
#include "lv_theme.h"
#include "lv_hal_disp.h"
#include "lv_test_theme_1.h"
#include "lv_test_theme_2.h"
#include "sysmon.h"
#include "benchmark.h"
#include "terminal.h"
#include "tpcal.h"

void application_entry(void *arg)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        tos_task_delay(50);
    }

    lv_init();
    tft_init();
    touchpad_init();

    //benchmark_create();
    //sysmon_create();
    //terminal_create();
    //tpcal_create();
    //demo_create();

    //lv_theme_t *th = lv_theme_alien_init(21, NULL);
    //lv_test_theme_1(th);
    lv_test_theme_2();

    while (1)
    {
        tos_task_delay(5);
        lv_task_handler();
    }
}


