/**
  ******************************************************************
  * @file    pad_config.h
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   IOMUXC  PAD属性配置使用的功能宏定义
  ******************************************************************
  * @attention
  *
  * 实验平台:野火  i.MXRT1052开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */
  
#ifndef __PAD_CONFIG_H
#define	__PAD_CONFIG_H

#include "fsl_common.h"

/* SRE 压摆率选择 */
#define SRE_0_SLOW_SLEW_RATE                IOMUXC_SW_PAD_CTL_PAD_SRE(0)
#define SRE_1_FAST_SLEW_RATE                IOMUXC_SW_PAD_CTL_PAD_SRE(1)

/* 驱动能力配置，配置阻值的大小 */
#define DSE_0_OUTPUT_DRIVER_DISABLED        IOMUXC_SW_PAD_CTL_PAD_DSE(0)
/* R0 260 Ohm @ 3.3V, 150Ohm@1.8V, 240 Ohm for DDR */
#define DSE_1_R0_1                          IOMUXC_SW_PAD_CTL_PAD_DSE(1) 
/* R0/2 */
#define DSE_2_R0_2                          IOMUXC_SW_PAD_CTL_PAD_DSE(2)
/* R0/3 */
#define DSE_3_R0_3                          IOMUXC_SW_PAD_CTL_PAD_DSE(3)
/* R0/4 */
#define DSE_4_R0_4                          IOMUXC_SW_PAD_CTL_PAD_DSE(4)
/* R0/5 */
#define DSE_5_R0_5                          IOMUXC_SW_PAD_CTL_PAD_DSE(5)
/* R0/6 */
#define DSE_6_R0_6                          IOMUXC_SW_PAD_CTL_PAD_DSE(6)
/* R0/7 */
#define DSE_7_R0_7                          IOMUXC_SW_PAD_CTL_PAD_DSE(7)

/* SPEED 带宽配置 */
#define SPEED_0_LOW_50MHz                   IOMUXC_SW_PAD_CTL_PAD_SPEED(0)
#define SPEED_1_MEDIUM_100MHz               IOMUXC_SW_PAD_CTL_PAD_SPEED(1)
#define SPEED_2_MEDIUM_100MHz               IOMUXC_SW_PAD_CTL_PAD_SPEED(2)
#define SPEED_3_MAX_200MHz                  IOMUXC_SW_PAD_CTL_PAD_SPEED(3)

/* ODE 是否使用开漏模式 */
#define ODE_0_OPEN_DRAIN_DISABLED           IOMUXC_SW_PAD_CTL_PAD_ODE(0)     
#define ODE_1_OPEN_DRAIN_ENABLED            IOMUXC_SW_PAD_CTL_PAD_ODE(1)     

/* PKE 是否使能保持器或上下拉功能 */
#define PKE_0_PULL_KEEPER_DISABLED          IOMUXC_SW_PAD_CTL_PAD_PKE(0)      
#define PKE_1_PULL_KEEPER_ENABLED           IOMUXC_SW_PAD_CTL_PAD_PKE(1)      

/* PUE 选择使用保持器还是上下拉 */
#define PUE_0_KEEPER_SELECTED               IOMUXC_SW_PAD_CTL_PAD_PUE(0)   
#define PUE_1_PULL_SELECTED                 IOMUXC_SW_PAD_CTL_PAD_PUE(1)   

/* PUS 上下拉配置 */
#define PUS_0_100K_OHM_PULL_DOWN            IOMUXC_SW_PAD_CTL_PAD_PUS(0)     
#define PUS_1_47K_OHM_PULL_UP               IOMUXC_SW_PAD_CTL_PAD_PUS(1)   
#define PUS_2_100K_OHM_PULL_UP              IOMUXC_SW_PAD_CTL_PAD_PUS(2)   
#define PUS_3_22K_OHM_PULL_UP               IOMUXC_SW_PAD_CTL_PAD_PUS(3)   

/* HYS 滞后功能 */
#define HYS_0_HYSTERESIS_DISABLED           IOMUXC_SW_PAD_CTL_PAD_HYS(0)  
#define HYS_1_HYSTERESIS_ENABLED            IOMUXC_SW_PAD_CTL_PAD_HYS(1)  

#endif /* __PAD_CONFIG_H */
