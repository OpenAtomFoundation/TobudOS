/**
  ******************************************************************************
  * @file    adc.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片ADC模块的底层功能函数
  ******************************************************************************
  */
#ifndef __CH_LIB_ADC_H__
#define __CH_LIB_ADC_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* ADC外设模块号 */
#define HW_ADC0  (0) /* ADC外设模块号0，依次类推 */
#define HW_ADC1  (1) 
#define HW_ADC2  (2) 


/* ADC 快速初始化宏 */
#define ADC0_SE0_DP0        (0x00000000U) /* ADC0模块的0通道，DP0引脚，依次类推 */
#define ADC0_SE1_DP1        (0x00080000U)
#define ADC0_SE3_DP3        (0x00180000U)
#define ADC0_SE4B_PC2       (0x01204410U) /* ADC0模块的4B通道，PC2引脚  使用B通道时，需要设置 ADC_ChlMuxConfig */ 
#define ADC0_SE5B_PD1       (0x01284218U) 
#define ADC0_SE6B_PD5       (0x01304A18U)
#define ADC0_SE7B_PD6       (0x01384C18U)
#define ADC0_SE8_PB0        (0x00404008U)
#define ADC0_SE9_PB1        (0x00484208U)
#define ADC0_SE12_PB2       (0x00604408U)
#define ADC0_SE13_PB3       (0x00684608U)
#define ADC0_SE14_PC0       (0x00704010U)
#define ADC0_SE15_PC1       (0x00784210U)
#define ADC0_SE17_E24       (0x00887020U)
#define ADC0_SE18_E25       (0x00907220U)
#define ADC0_SE19_DM0       (0x00980000U)
#define ADC0_SE20_DM1       (0x00A00000U)
#define ADC0_SE26_TEMP      (0x00D00000U)
#define ADC1_SE0_DP0        (0x00000001U)
#define ADC1_SE1_DP1        (0x00080001U)
#define ADC1_SE3_DP3        (0x00180001U)
#define ADC1_SE4_PE0        (0x00204021U)
#define ADC1_SE5_PE1        (0x00284221U)
#define ADC1_SE6_PE2        (0x00304421U)
#define ADC1_SE7_PE3        (0x00384621U)
#define ADC1_SE4B_PC8       (0x01205011U)
#define ADC1_SE5B_PC9       (0x01285211U)
#define ADC1_SE6B_PC10      (0x01305411U)
#define ADC1_SE7B_PC11      (0x01385611U)
#define ADC1_SE8_PB0        (0x00404009U)
#define ADC1_SE9_PB1        (0x00484209U)
#define ADC1_SE14_PB10      (0x00705409U)
#define ADC1_SE15_PB11      (0x00785609U)
#define ADC1_SE17_PA17      (0x00886201U)
#define ADC1_SE19_DM0       (0x00980001U)
#define ADC1_SE20_DM1       (0x00A00001U)
#define ADC1_SE26_TEMP      (0x00D00001U)
#define ADC0_SE21           (0x00A80000U)
#define ADC0_SE22           (0x00B00000U)
#define ADC1_SE16           (0x00800001U)
#define ADC1_SE18           (0x00900001U)
#define ADC0_SE23           (0x00B80000U)
#define ADC1_SE23           (0x00B80001U)
#define ADC1_SE10_PB04      (0X00504809U)
#define ADC1_SE11_PB05      (0X00584A09U)
#define ADC1_SE12_PB06      (0X00604C09U)
#define ADC1_SE13_PB07      (0X00684E09U)

/**
 * \enum  ADC_ClockDiv_Type
 * \brief ADC 转换时钟分频因子,控制转换速度
 */
typedef enum
{
    kADC_ClockDiv1,         /**<不分频*/
    kADC_ClockDiv2,         /**<2分频*/
    kADC_ClockDiv4,         /**<4分频*/
    kADC_ClockDiv8,         /**<8分频*/
}ADC_ClockDiv_Type;         

/**
 * \enum ADC_ResolutionMode_Type
 * \brief ADC转换精度定义
 */
typedef enum
{
    kADC_SingleDiff8or9 = 0,    /**< 单端8位或 差分9位精度 */
    kADC_SingleDiff12or13 = 1,  /**< 单端12位或 差分13位精度*/
    kADC_SingleDiff10or11 = 2,  /**< 单端10位或 差分11位精度*/
    kADC_SingleDIff16 = 3,      /**< 单端/差分16位精度*/
}ADC_ResolutionMode_Type;

/**
 * \enum ADC_TriggerSelect_Type
 * \brief ADC 触发方式定义
 */
typedef enum
{
    kADC_TriggerSoftware,  /**<软件触发*/
    kADC_TriggerHardware,  /**<硬件触发*/
}ADC_TriggerSelect_Type;


/**
 * \enum ADC_ContinueMode_Type
 * \brief 是否连续转换
 */
typedef enum
{
    kADC_ContinueConversionEnable,   /**<开启连续转换*/
    kADC_ContinueConversionDisable,  /**<关闭连续转换*/
}ADC_ContinueMode_Type;

/**
 * \enum ADC_SingleOrDiffMode_Type
 * \brief 单端ADC还是差分ADC
 */
typedef enum
{
    kADC_Single,         /**<单端模式*/
    kADC_Differential,   /**<差分模式*/
}ADC_SingleOrDiffMode_Type;

#define kADC_MuxA                (0x00)
#define kADC_MuxB                (0x01)
//!< ADC 通道触发器复用选择(每个ADC通道有2个ADC转换触发器 为MuxA 和 MuxB. MuxB 只能硬件触发)

#define kADC_ChlMuxA             (0x00)
#define kADC_ChlMuxB             (0x01)
//!< ADC 通道复用选择(SEA 还是SEB) 硬件通道选择 

/**
 * \enum ADC_VoltageRef_Type
 * \brief 模拟电压参考源
 */
typedef enum
{
    kADC_VoltageVREF,  /**< 使用外部引脚VREFH VREFL 作为参考电源 */
    kADC_VoltageVALT,  /**< 可选的第二电压参考源 不同的型号可能连接到不同的模块上 K60DN512Vxxx 连接到 VREF模块 */
}ADC_VoltageRef_Type;

/**
 * \enum ADC_HardwareAveMode_Type
 * \brief 硬件平均
 */
typedef enum
{
    kADC_HardwareAverageDisable,    /**<关闭硬件平均*/
    kADC_HardwareAverage_4,         /**<4均值*/
    kADC_HardwareAverage_8,         /**<8均值*/
    kADC_HardwareAverage_16,        /**<16均值*/
    kADC_HardwareAverage_32,        /**<32均值*/
}ADC_HardwareAveMode_Type;

/**
 * \enum ADC_ITDMAConfig_Type
 * \brief ADC中断及DMA配置选择
 */
typedef enum
{
    kADC_IT_Disable,        /**<AD中断功能禁止*/
    kADC_DMA_Disable,       /**<ADC DMA功能禁止*/
    kADC_IT_EOF,            /**<打开ADC 转换完成中断*/
    kADC_DMA_EOF,           /**<打开ADC DMA 完成中断*/
}ADC_ITDMAConfig_Type;

//!< ADC 回调函数定义
typedef void (*ADC_CallBackType)(void);

/**
 * \struct ADC_ITDMAConfig_Type
 * \brief ADC 初始化结构
 */
typedef struct
{
    uint32_t                    instance;                   ///<模块号
    ADC_TriggerSelect_Type      triggerMode;                ///<触发模式 软件触发 或 硬件触发
    ADC_ClockDiv_Type           clockDiv;                   ///<ADC时钟分频
    ADC_ResolutionMode_Type     resolutionMode;             ///<分频率选择 8 10 12 16位精度等
    ADC_SingleOrDiffMode_Type   singleOrDiffMode;           ///<单端 还是 差分输入
    ADC_ContinueMode_Type       continueMode;               ///<是否启动连续转换
    ADC_HardwareAveMode_Type    hardwareAveMode;            ///<硬件平均功能选择
    ADC_VoltageRef_Type         vref;                       ///<模拟电压参考源
}ADC_InitTypeDef;


//!< API functions
void ADC_CallbackInstall(uint32_t instance, ADC_CallBackType AppCBFun);
void ADC_Init(ADC_InitTypeDef* ADC_InitStruct);
uint8_t ADC_QuickInit(uint32_t MAP, ADC_ResolutionMode_Type resolutionMode);
void ADC_ITDMAConfig(uint32_t instance, uint32_t mux, ADC_ITDMAConfig_Type config);
int32_t ADC_QuickReadValue(uint32_t MAP);
int32_t ADC_ReadValue(uint32_t instance, uint32_t mux);
void ADC_StartConversion(uint32_t instance, uint32_t chl, uint32_t mux);
uint8_t ADC_IsConversionCompleted(uint32_t instance, uint32_t mux);
void ADC_ChlMuxConfig(uint32_t instance, uint32_t mux);
int32_t ADC_Calibration(uint32_t instance);
//hardware trigger 
void ADC_EnableHardwareTrigger(uint32_t instance, bool status);

#ifdef __cplusplus
}
#endif



#endif
