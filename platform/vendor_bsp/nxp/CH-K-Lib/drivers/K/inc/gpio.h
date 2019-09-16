/**
  * @file    gpio.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24 vdsav avfdsfdsafasgdsa
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片GPIO模块的底层功能函数
  */
#ifndef __CH_LIB_GPIO_H__
#define __CH_LIB_GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* GPIO端口定义 */
#define HW_GPIOA  (0x00U) /*GPIO模块A,依次类推*/  
#define HW_GPIOB  (0x01U)
#define HW_GPIOC  (0x02U)
#define HW_GPIOD  (0x03U)
#define HW_GPIOE  (0x04U)
#define HW_GPIOF  (0x05U)

/**
 * \enum PORT_PinMux_Type
 * \brief 端口复用选择
 * \see Signal Multiplexing and Signal Descriptions 章节
 */
typedef enum
{
    kPinAlt0,  /**<0功能复用*/
    kPinAlt1,  /**<1功能复用*/
    kPinAlt2,  /**<2功能复用*/
    kPinAlt3,  /**<3功能复用*/
    kPinAlt4,  /**<4功能复用*/
    kPinAlt5,  /**<5功能复用*/
    kPinAlt6,  /**<6功能复用*/
    kPinAlt7,  /**<7功能复用*/
}PORT_PinMux_Type;

/**
 * \enum PORT_Pull_Type
 * \brief 端口上下拉配置 ，电阻阻值约为20K
 */
typedef enum
{
    kPullDisabled,  /**<关闭上下拉电阻功能*/
    kPullUp,        /**<开启上拉电阻功能*/
    kPullDown,      /**<开启下拉电阻功能*/
}PORT_Pull_Type;

/**
 * \enum GPIO_Mode_Type
 * \brief GPIO端口模式配置
 */
typedef enum
{
    kGPIO_Mode_IFT = 0x00,       /**< 浮空输入 */
    kGPIO_Mode_IPD = 0x01,       /**< 下拉输入 */
    kGPIO_Mode_IPU = 0x02,       /**< 上拉输入 */
    kGPIO_Mode_OOD = 0x03,       /**< 开漏输出 */
    kGPIO_Mode_OPP = 0x04,       /**< 推挽输出 */
}GPIO_Mode_Type;

/**
 * \enum GPIO_PinConfig_Type
 * \brief 端口输入输出模式选择
 */
typedef enum
{
    kInput,                  /**< 引脚输入模式 */
    kOutput,                 /**< 引脚输出模式 */
}GPIO_PinConfig_Type;

/**
 * \enum GPIO_ITDMAConfig_Type
 * \brief 端口中断及DMA配置选择
 */
typedef enum
{
    kGPIO_DMA_RisingEdge,	      /**<上升沿触发DMA*/
    kGPIO_DMA_FallingEdge,        /**<下降沿触发DMA*/
    kGPIO_DMA_RisingFallingEdge,  /**<上升沿和下降沿触发DMA*/
    kGPIO_IT_Low,                 /**<低电平出发中断*/
    kGPIO_IT_RisingEdge,          /**<上升沿触发中断*/
    kGPIO_IT_FallingEdge,         /**<下降沿触发中断*/
    kGPIO_IT_RisingFallingEdge,   /**<上升沿和下降沿触发中断*/
    kGPIO_IT_High,                /**<高电平触发中断*/
}GPIO_ITDMAConfig_Type;

/**
 * \struct DAC_InitTypeDef
 * \brief GPIO端口初始化结构体 
 */
typedef struct
{
    uint8_t                instance;    ///<引脚端口HW_GPIOA~HW_GPIOF
    GPIO_Mode_Type         mode;        ///<工作模式
    uint32_t               pinx;        ///<引脚号0~31
}GPIO_InitTypeDef;

/* 端口中断回调函数定义 */
typedef void (*GPIO_CallBackType)(uint32_t pinxArray);


/* 位带操作 内存偏移计算 详见 Cortex-M4 Generic User Guide 2.25 */
/* CM4中有2块bitband区域 0x2000_0000-0x200F_FFFF 映射至 0x2200_0000-0x23FF_FFFF
                         0x4000_0000-0x4000_FFFF 映射至 0x4200_0000-0x43FF_FFFF
*/
#define PAout(n)   BITBAND_REG(PTA->PDOR, n)
#define PAin(n)    BITBAND_REG(PTA->PDIR, n)
  
#define PBout(n)   BITBAND_REG(PTB->PDOR, n)
#define PBin(n)    BITBAND_REG(PTB->PDIR, n)

#define PCout(n)   BITBAND_REG(PTC->PDOR, n)
#define PCin(n)    BITBAND_REG(PTC->PDIR, n)

#define PDout(n)   BITBAND_REG(PTD->PDOR, n)
#define PDin(n)    BITBAND_REG(PTD->PDIR, n)

#define PEout(n)   BITBAND_REG(PTE->PDOR, n)
#define PEin(n)    BITBAND_REG(PTE->PDIR, n)

#define PFout(n)   BITBAND_REG(PTF->PDOR, n)
#define PFin(n)    BITBAND_REG(PTF->PDIR, n)

#define PGout(n)   BITBAND_REG(PTG->PDOR, n)
#define PGin(n)    BITBAND_REG(PTG->PDIR, n)

//!< API functions
void GPIO_Init(GPIO_InitTypeDef * GPIO_InitStruct);
uint8_t GPIO_QuickInit(uint32_t instance, uint32_t pinx, GPIO_Mode_Type mode);
void GPIO_WriteBit(uint32_t instance, uint8_t pin, uint8_t data);
uint8_t GPIO_ReadBit(uint32_t instance, uint8_t pin);
void GPIO_ToggleBit(uint32_t instance, uint8_t pin);
void GPIO_SetBit(uint32_t instance, uint32_t pin);
void GPIO_ResetBit(uint32_t instance, uint32_t pin);
void GPIO_ITDMAConfig(uint32_t instance, uint8_t pin, GPIO_ITDMAConfig_Type config, bool status);
void GPIO_CallbackInstall(uint32_t instance, GPIO_CallBackType AppCBFun);
//!< low level functions
void PORT_PinPullConfig(uint32_t instance, uint8_t pin, PORT_Pull_Type pull);
void GPIO_PinConfig(uint32_t instance, uint8_t pin, GPIO_PinConfig_Type mode);
void PORT_PinMuxConfig(uint32_t instance, uint8_t pin, PORT_PinMux_Type pinMux);
uint32_t GPIO_ReadPort(uint32_t instance);
void GPIO_WritePort(uint32_t instance, uint32_t data);
void PORT_PinOpenDrainConfig(uint32_t instance, uint8_t pin, bool status);
void PORT_PinPassiveFilterConfig(uint32_t instance, uint8_t pin, bool status);
    
#ifdef __cplusplus
}
#endif

#endif


