#include "cmsis_os.h"

/*
   tos_bsp_tickless_setup函数是板级的tickless设置接口
   此sample中:

   tos_tickless_wkup_alarm_install(TOS_LOW_POWER_MODE_SLEEP, &tickless_wkup_alarm_tim);
   tos_tickless_wkup_alarm_init(TOS_LOW_POWER_MODE_SLEEP);
   tos_pm_cpu_lpwr_mode_set(TOS_LOW_POWER_MODE_SLEEP);

   我们通过tos_tickless_wkup_alarm_install注册了sleep模式下的唤醒闹钟（使用定时器6作为
   sleep模式下的唤醒闹钟，具体实现可以参考bsp_tickless_alarm.c）
   通过tos_pm_cpu_lpwr_mode_set我们设置tickless模式为sleep模式（如果用户选定tickless
   只进入一种模式，那么可以只实现、注册此模式下的唤醒闹钟，其他模式可以不理）

   验证：用户可以在idle任务入口knl_idle_entry中加入打印log，在tickless模式下，idle任务
   不会一直唤醒，因而idle中的日志打印不会频繁出现（正常模式下idle在系统中没有就绪任务时
   就会被唤醒，打印log会频繁出现）

   注意：板子烧录tickless使能的镜像后，会导致通过keil烧录新的镜像出现问题（低功耗模式下
   debug会有问题）
   - 如果是TencentOS tiny定制的开发板，可以在按下板子上的RESET键后立马点击keil上的烧录按钮
   - 如果是阿波罗的开发板，可以拔掉板子的串口USB线，脱离stlink（或jlink）的USB口与
   PC主机的接触，再重新将stlink（或jlink）的USB插上PC主机后，大概0.5S后再点击keil上的烧录按钮。
 */

void application_entry(void *arg)
{
    int count = 1;
    while (1) {
        printf("###This is task1, %d\r\n", count++);
        osDelay(3000);
    }
}

