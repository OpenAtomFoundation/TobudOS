#include "cmsis_os.h"

/*
   tos_bsp_tickless_setup 函数是板级的 tickless 设置接口
   此 sample 中：

   tos_tickless_wkup_alarm_install(TOS_LOW_POWER_MODE_SLEEP, &tickless_wkup_alarm_tim);
   tos_tickless_wkup_alarm_init(TOS_LOW_POWER_MODE_SLEEP);
   tos_pm_cpu_lpwr_mode_set(TOS_LOW_POWER_MODE_SLEEP);

   我们通过 tos_tickless_wkup_alarm_install 注册了 sleep 模式下的唤醒闹钟（使用定时器 6 作为
   sleep 模式下的唤醒闹钟，具体实现可以参考 bsp_tickless_alarm.c）
   通过 tos_pm_cpu_lpwr_mode_set 我们设置 tickless 模式为 sleep 模式（如果用户选定 tickless
   只进入一种模式，那么可以只实现、注册此模式下的唤醒闹钟，其他模式可以不理）

   验证：用户可以在 idle 任务入口 knl_idle_entry 中加入打印 log，在 tickless 模式下，idle 任务
   不会一直唤醒，因而 idle 中的日志打印不会频繁出现（正常模式下 idle 在系统中没有就绪任务时
   就会被唤醒，打印 log 会频繁出现）

   注意：板子烧录 tickless 使能的镜像后，会导致通过 keil 烧录新的镜像出现问题（低功耗模式下
   debug 会有问题）
   - 如果是 TencentOS tiny 定制的开发板，可以在按下板子上的 RESET 键后立马点击 keil 上的烧录按钮
   - 如果是阿波罗的开发板，可以拔掉板子的串口 USB 线，脱离 stlink（或 jlink）的 USB 口与
   PC 主机的接触，再重新将 stlink（或 jlink）的 USB 插上 PC 主机后，大概 0.5S 后再点击 keil 上的烧录按钮。
 */

void application_entry(void *arg)
{
    int count = 1;
    while (1) {
        printf("###This is task1, %d\r\n", count++);
        osDelay(3000);
    }
}

