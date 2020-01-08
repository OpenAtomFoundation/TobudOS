#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "fsl_common.h"

/*********************************************************
 * LED GPIO端口、引脚号及IOMUXC复用宏定义
 *********************************************************/
#define CORE_BOARD_LED_GPIO             GPIO1
#define CORE_BOARD_LED_GPIO_PIN         (9U)
#define CORE_BOARD_LED_IOMUXC           IOMUXC_GPIO_AD_B0_09_GPIO1_IO09

#define RGB_RED_LED_GPIO                GPIO1
#define RGB_RED_LED_GPIO_PIN            (24U)
#define RGB_RED_LED_IOMUXC              IOMUXC_GPIO_AD_B1_08_GPIO1_IO24

#define RGB_GREEN_LED_GPIO              GPIO1
#define RGB_GREEN_LED_GPIO_PIN          (25U)
#define RGB_GREEN_LED_IOMUXC            IOMUXC_GPIO_AD_B1_09_GPIO1_IO25

#define RGB_BLUE_LED_GPIO               GPIO1
#define RGB_BLUE_LED_GPIO_PIN           (10U)
#define RGB_BLUE_LED_IOMUXC             IOMUXC_GPIO_AD_B0_10_GPIO1_IO10

#define LED_DELAY_COUNT                 70000000

/** 控制LED灯亮灭的宏，
  * LED低电平亮，设置ON=0，OFF=1
  * 若LED高电平亮，把宏设置成ON=1 ，OFF=0 即可
 */
#define ON  1
#define OFF 0

/* 使用标准的固件库控制IO 
   使用范例：CORE_BOARD_LED(ON); */
   
#define   CORE_BOARD_LED(a) if (a)  \
          GPIO_PinWrite(CORE_BOARD_LED_GPIO, CORE_BOARD_LED_GPIO_PIN, 0U);\
          else    \
          GPIO_PinWrite(CORE_BOARD_LED_GPIO, CORE_BOARD_LED_GPIO_PIN, 1U);

#define   RGB_RED_LED(a)  if (a)  \
          GPIO_PinWrite(RGB_RED_LED_GPIO, RGB_RED_LED_GPIO_PIN, 0U);\
          else    \
          GPIO_PinWrite(RGB_RED_LED_GPIO, RGB_RED_LED_GPIO_PIN, 1U);

#define   RGB_GREEN_LED(a)  if (a)  \
          GPIO_PinWrite(RGB_RGB_GREEN_LED_GPIO, RGB_RGB_GREEN_LED_GPIO_PIN, 0U);\
          else    \
          GPIO_PinWrite(RGB_GREEN_LED_GPIO, RGB_GREEN_LED_GPIO_PIN, 1U);

#define   RGB_BLUE_LED(a) if (a)  \
          GPIO_PinWrite(RGB_BLUE_LED_GPIO, RGB_BLUE_LED_GPIO_PIN, 0U);\
          else    \
          GPIO_PinWrite(RGB_BLUE_LED_GPIO, RGB_BLUE_LED_GPIO_PIN, 1U);     


/* 直接操作寄存器的方法控制IO */
#define digitalHi(p,i)      {p->DR |= (1U << i);}   //输出为高电平    
#define digitalLo(p,i)      {p->DR &= ~(1U << i);}  //输出低电平
#define digitalToggle(p,i)  {p->DR ^= (1U<<i);}     //输出反转状态


/* 定义控制IO的宏 */
#define CORE_BOARD_LED_TOGGLE       digitalToggle(CORE_BOARD_LED_GPIO,CORE_BOARD_LED_GPIO_PIN)
#define CORE_BOARD_LED_OFF          digitalHi(CORE_BOARD_LED_GPIO,CORE_BOARD_LED_GPIO_PIN)
#define CORE_BOARD_LED_ON           digitalLo(CORE_BOARD_LED_GPIO,CORE_BOARD_LED_GPIO_PIN)

#define RGB_RED_LED_TOGGLE          digitalToggle(RGB_RED_LED_GPIO,RGB_RED_LED_GPIO_PIN)
#define RGB_RED_LED_OFF             digitalHi(RGB_RED_LED_GPIO,RGB_RED_LED_GPIO_PIN)
#define RGB_RED_LED_ON              digitalLo(RGB_RED_LED_GPIO,RGB_RED_LED_GPIO_PIN)

#define RGB_GREEN_LED_TOGGLE        digitalToggle(RGB_GREEN_LED_GPIO,RGB_GREEN_LED_GPIO_PIN)
#define RGB_GREEN_LED_OFF           digitalHi(RGB_GREEN_LED_GPIO,RGB_GREEN_LED_GPIO_PIN)
#define RGB_GREEN_LED_ON            digitalLo(RGB_GREEN_LED_GPIO,RGB_GREEN_LED_GPIO_PIN)

#define RGB_BLUE_LED_TOGGLE         digitalToggle(RGB_BLUE_LED_GPIO,RGB_BLUE_LED_GPIO_PIN)
#define RGB_BLUE_LED_OFF            digitalHi(RGB_BLUE_LED_GPIO,RGB_BLUE_LED_GPIO_PIN)
#define RGB_BLUE_LED_ON             digitalLo(RGB_BLUE_LED_GPIO,RGB_BLUE_LED_GPIO_PIN)

/* 基本混色，后面高级用法使用PWM可混出全彩颜色,且效果更好 */

//红
#define RGB_LED_COLOR_RED  \
          RGB_RED_LED_ON;\
          RGB_GREEN_LED_OFF;\
          RGB_BLUE_LED_OFF

//绿
#define RGB_LED_COLOR_GREEN   \
          RGB_RED_LED_OFF;\
          RGB_GREEN_LED_ON;\
          RGB_BLUE_LED_OFF

//蓝
#define RGB_LED_COLOR_BLUE  \
          RGB_RED_LED_OFF;\
          RGB_GREEN_LED_OFF;\
          RGB_BLUE_LED_ON
          
//黄(红+绿)          
#define RGB_LED_COLOR_YELLOW  \
          RGB_RED_LED_ON;\
          RGB_GREEN_LED_ON;\
          RGB_BLUE_LED_OFF
          
//紫(红+蓝)
#define RGB_LED_COLOR_PURPLE  \
          RGB_RED_LED_ON;\
          RGB_GREEN_LED_OFF;\
          RGB_BLUE_LED_ON

//青(绿+蓝)
#define RGB_LED_COLOR_CYAN \
          RGB_RED_LED_OFF;\
          RGB_GREEN_LED_ON;\
          RGB_BLUE_LED_ON
          
//白(红+绿+蓝)
#define RGB_LED_COLOR_WHITE \
          RGB_RED_LED_ON;\
          RGB_GREEN_LED_ON;\
          RGB_BLUE_LED_ON
          
//黑(全部关闭)
#define RGB_LED_COLOR_OFF \
          RGB_RED_LED_OFF;\
          RGB_GREEN_LED_OFF;\
          RGB_BLUE_LED_OFF  

/*******************************************************************************
 * 函数声明
 ******************************************************************************/
void LED_GPIO_Config(void);

#endif /* __BSP_LED_H */
