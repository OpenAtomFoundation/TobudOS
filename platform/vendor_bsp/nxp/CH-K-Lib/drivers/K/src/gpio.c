/**
  * @file    gpio.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * \date    2015.9.25 FreeXc 完善了gpio.h & gpio.c文件的注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    gpio driver source file
  */
#include "common.h"
#include "gpio.h"

/* leagacy support for Kineis Z version(inital version) */
#if (!defined(GPIO_BASES))

#ifdef PTF
#define GPIO_BASES {PTA, PTB, PTC, PTD, PTE, PTF}
#define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE, PORTF}
#else
#define GPIO_BASES {PTA, PTB, PTC, PTD, PTE}
#define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE}
#endif

#endif

/* gloabl vars */
static GPIO_Type * const GPIO_InstanceTable[] = GPIO_BASES;
static PORT_Type * const PORT_InstanceTable[] = PORT_BASES;
static GPIO_CallBackType GPIO_CallBackTable[ARRAY_SIZE(PORT_InstanceTable)] = {NULL};
static const uint32_t SIM_GPIOClockGateTable[] =
{
    SIM_SCGC5_PORTA_MASK,
    SIM_SCGC5_PORTB_MASK,
    SIM_SCGC5_PORTC_MASK,
    SIM_SCGC5_PORTD_MASK,
    SIM_SCGC5_PORTE_MASK,
};
static const IRQn_Type GPIO_IRQnTable[] = 
{
    PORTA_IRQn,
    PORTB_IRQn,
    PORTC_IRQn,
    PORTD_IRQn,
    PORTE_IRQn,
};


 /**
 * @brief  set GPIO pin mux
 * @note   enable PORT clock before set pinmux number
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOx  GPIOx moudle
 * @param[in]  pin pin index number 0-31
 * @param[in]  pinMux pinmux function
 *              @arg kPinAltx  Pinmux function x
 * @retval None
 */
void PORT_PinMuxConfig(uint32_t instance, uint8_t pin, PORT_PinMux_Type pinMux)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    PORT_InstanceTable[instance]->PCR[pin] &= ~(PORT_PCR_MUX_MASK);
    PORT_InstanceTable[instance]->PCR[pin] |=  PORT_PCR_MUX(pinMux);
}

 /**
 * @brief  set pin internal pullup/down resistors
 * @note   pull resistor value is about 20K
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOx  GPIOx moudle
 * @param[in]  pin  pin index number 0-31
 * @param[in]  pull pull select
 *              @arg kPullDisabled  disable pull resistor
 *              @arg kPullUp        pull up
 *              @arg kPullDown      pull down
 * @retval None
 */
void PORT_PinPullConfig(uint32_t instance, uint8_t pin, PORT_Pull_Type pull)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    switch(pull)
    {
        case kPullDisabled:
            PORT_InstanceTable[instance]->PCR[pin] &= ~PORT_PCR_PE_MASK;
            break;
        case kPullUp:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_PE_MASK;
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_PS_MASK;
            break;
        case kPullDown:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_PE_MASK;
            PORT_InstanceTable[instance]->PCR[pin] &= ~PORT_PCR_PS_MASK;
            break;
        default:
            break;
    }
}
/**
 * @brief  端口引脚的开漏状态配置 用户一般不必调用
 * @code
 *      // 将PORTA端口的3引脚设置为开漏状态 
 *      PORT_PinOpenDrainConfig(HW_GPIOA, 3, ENABLE);
 * @endcode
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOA 芯片的PORTA端口
 *              @arg HW_GPIOB 芯片的PORTB端口
 *              @arg HW_GPIOC 芯片的PORTC端口
 *              @arg HW_GPIOD 芯片的PORTD端口
 *              @arg HW_GPIOE 芯片的PORTE端口
 * @param[in]  pin  端口上的引脚号 0~31
 * @param[in]  status  功能开关控制
 *              @arg ENABLE   开启功能 
 *              @arg DISABLE  关闭功能
 * @retval None
 */
void PORT_PinOpenDrainConfig(uint32_t instance, uint8_t pin, bool status)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (status) ? (PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_ODE_MASK):(PORT_InstanceTable[instance]->PCR[pin] &= ~PORT_PCR_ODE_MASK);
}

/**
 * @brief  端口引脚的开启无源滤波器 作为输入时有效
 * @code
 *      // 将PORTA端口的3引脚设置为开漏状态 
 *      PORT_PinPassiveFilterConfig(HW_GPIOA, 3, ENABLE);
 * @endcode
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOA 芯片的PORTA端口
 *              @arg HW_GPIOB 芯片的PORTB端口
 *              @arg HW_GPIOC 芯片的PORTC端口
 *              @arg HW_GPIOD 芯片的PORTD端口
 *              @arg HW_GPIOE 芯片的PORTE端口
 * @param[in]  pin  端口上的引脚号 0~31
 * @param[in]  status  功能开关控制
 *              @arg ENABLE   开启功能
 *              @arg DISABLE  关闭功能
 * @retval None
 */
void PORT_PinPassiveFilterConfig(uint32_t instance, uint8_t pin, bool status)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (status) ? (PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_PFE_MASK):(PORT_InstanceTable[instance]->PCR[pin] &= ~PORT_PCR_PFE_MASK);
}

 /**
 * @brief  设置引脚为输入还是输出功能  用户一般不必调用
 * @note   只有当引脚作为GPIO时才有意义
 * @code
 *      // 将PORTB端口的3引脚设置输入引脚
 *      GPIO_PinConfig(HW_GPIOB, 3, kInpput);
 * @endcode
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOA 芯片的PORTA端口
 *              @arg HW_GPIOB 芯片的PORTB端口
 *              @arg HW_GPIOC 芯片的PORTC端口
 *              @arg HW_GPIOD 芯片的PORTD端口
 *              @arg HW_GPIOE 芯片的PORTE端口
 * @param[in]  pin  端口上的引脚号 0~31
 * @param[in]  mode 输入或者输出设置
 *              @arg kInpput  输入功能选择
 *              @arg kOutput  输出功能选择
 * @retval None
 */
void GPIO_PinConfig(uint32_t instance, uint8_t pin, GPIO_PinConfig_Type mode)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (mode == kOutput) ? (GPIO_InstanceTable[instance]->PDDR |= (1 << pin)):(GPIO_InstanceTable[instance]->PDDR &= ~(1 << pin));
}

 /**
 * @brief  GPIO初始化配置
 * @code
 *    //初始化配置PORTB端口的10引脚为推挽输出引脚
 *    GPIO_InitTypeDef GPIO_InitStruct1;      //申请一个结构变量
 *    GPIO_InitStruct1.instance = HW_GPIOB;   //选择PORTB端口
 *    GPIO_InitStruct1.mode = kGPIO_Mode_OPP; //推挽输出
 *    GPIO_InitStruct1.pinx = 10;             //选择10引脚
 *    //调用初始化GPIO函数 
 *    GPIO_Init(&GPIO_InitStruct1);
 * @endcode
 * @param[in]  GPIO_InitStruct GPIO初始化结构体，包含了引脚状态参数  
 * @see GPIO初始化配置例程
 * @retval None
 */
void GPIO_Init(GPIO_InitTypeDef * GPIO_InitStruct)
{
    /* config state */
    switch(GPIO_InitStruct->mode)
    {
        case kGPIO_Mode_IFT:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDisabled);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInput);
            break;
        case kGPIO_Mode_IPD:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDown);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInput);
            break;
        case kGPIO_Mode_IPU:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullUp);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInput);
            break;
        case kGPIO_Mode_OOD:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullUp);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, ENABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kOutput);
            break;
        case kGPIO_Mode_OPP:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDisabled);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kOutput);
            break;
        default:
            break;					
    }
    /* config pinMux */
    PORT_PinMuxConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPinAlt1);
}

 /**
 * @brief  快速初始化一个GPIO引脚 实际上是GPIO_Init的最简单配置
 * @code
 *      //初始化配置PORTB端口的10引脚为推挽输出引脚
 *      GPIO_QuickInit(HW_GPIOB, 10, kGPIO_Mode_OPP);
 * @endcode
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOA 芯片的PORTA端口
 *              @arg HW_GPIOB 芯片的PORTB端口
 *              @arg HW_GPIOC 芯片的PORTC端口
 *              @arg HW_GPIOD 芯片的PORTD端口
 *              @arg HW_GPIOE 芯片的PORTE端口
 * @param[in]  pinx 端口上的引脚号 0~31
 * @param[in]  mode 引脚工作模式
 *              @arg kGPIO_Mode_IFT 悬空输入
 *              @arg kGPIO_Mode_IPD 下拉输入
 *              @arg kGPIO_Mode_IPU 上拉输入
 *              @arg kGPIO_Mode_OOD 开漏输出
 *              @arg kGPIO_Mode_OPP 推挽输出
 * @retval instance GPIO模块号
 */
uint8_t GPIO_QuickInit(uint32_t instance, uint32_t pinx, GPIO_Mode_Type mode)
{
    GPIO_InitTypeDef GPIO_InitStruct1;
    GPIO_InitStruct1.instance = instance;
    GPIO_InitStruct1.mode = mode;
    GPIO_InitStruct1.pinx = pinx;
    GPIO_Init(&GPIO_InitStruct1);
    return  instance;
}

 /**
 * @brief  设置指定引脚输出高电平或者低电平
 * @note   此引脚首先配置成输出引脚
 * @code
 *      //设置PORTB端口的10引脚输出高电平
 *      GPIO_WriteBit(HW_GPIOB, 10, 1);
 * @endcode
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOA 芯片的PORTA端口
 *              @arg HW_GPIOB 芯片的PORTB端口
 *              @arg HW_GPIOC 芯片的PORTC端口
 *              @arg HW_GPIOD 芯片的PORTD端口
 *              @arg HW_GPIOE 芯片的PORTE端口
 * @param[in]  pin  端口上的引脚号 0~31
 * @param[in]  data 引脚的电平状态  
 *              @arg 0  低电平 
 *              @arg 1  高电平
 * @retval None
 */
void GPIO_WriteBit(uint32_t instance, uint8_t pin, uint8_t data)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (data) ? (GPIO_InstanceTable[instance]->PSOR |= (1 << pin)):(GPIO_InstanceTable[instance]->PCOR |= (1 << pin));
}

 /**
 * @brief  置位指定引脚的电平状态（即为高电平）
 * @note   此引脚首先配置成输出引脚
 * @code
 *      //置位PORTB端口的10引脚
 *      GPIO_SetBit(HW_GPIOB, 10);
 * @endcode
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOA 芯片的PORTA端口
 *              @arg HW_GPIOB 芯片的PORTB端口
 *              @arg HW_GPIOC 芯片的PORTC端口
 *              @arg HW_GPIOD 芯片的PORTD端口
 *              @arg HW_GPIOE 芯片的PORTE端口
 * @param[in]  pin  端口上的引脚号 0~31
 * @retval None
 */
void GPIO_SetBit(uint32_t instance, uint32_t pin)
{
    GPIO_InstanceTable[instance]->PSOR |= (1 << pin);
}

 /**
 * @brief  复位指定引脚的电平状态（即为低电平）
 * @note   此引脚首先配置成输出引脚
 * @code
 *      //复位PORTB端口的10引脚
 *      GPIO_ResetBit(HW_GPIOB, 10);
 * @endcode
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOA 芯片的PORTA端口
 *              @arg HW_GPIOB 芯片的PORTB端口
 *              @arg HW_GPIOC 芯片的PORTC端口
 *              @arg HW_GPIOD 芯片的PORTD端口
 *              @arg HW_GPIOE 芯片的PORTE端口
 * @param[in]  pin  端口上的引脚号 0~31
 * @retval None
 */
void GPIO_ResetBit(uint32_t instance, uint32_t pin)
{
    GPIO_InstanceTable[instance]->PCOR |= (1 << pin);
}

 /**
 * @brief  读取一个引脚上的电平状态
 * @code
 *      //读取PORTB端口的10引脚的电平状态
 *      uint8_t status ; //用于存储引脚的状态
 *      status = GPIO_ReadBit(HW_GPIOB, 10); //获取引脚的状态并存储到status中
 * @endcode
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOA 芯片的PORTA端口
 *              @arg HW_GPIOB 芯片的PORTB端口
 *              @arg HW_GPIOC 芯片的PORTC端口
 *              @arg HW_GPIOD 芯片的PORTD端口
 *              @arg HW_GPIOE 芯片的PORTE端口
 * @param[in]  pin  端口上的引脚号 0~31
 * @retval 0 低电平
 * @retval 1 高电平
 */
uint8_t GPIO_ReadBit(uint32_t instance, uint8_t pin)
{
    /* input or output */
    if(((GPIO_InstanceTable[instance]->PDDR) >> pin) & 0x01)
    {
        return ((GPIO_InstanceTable[instance]->PDOR >> pin) & 0x01);
    }
    else
    {
        return ((GPIO_InstanceTable[instance]->PDIR >> pin) & 0x01);
    }
}

 /**
 * @brief  翻转一个引脚的电平状态
 * @code
 *      //翻转PORTB端口的10引脚的电平状态
 *      GPIO_ToggleBit(HW_GPIOB, 10); 
 * @endcode
 * @param[in]  instance: GPIO模块号
 *              @arg HW_GPIOA 芯片的PORTA端口
 *              @arg HW_GPIOB 芯片的PORTB端口
 *              @arg HW_GPIOC 芯片的PORTC端口
 *              @arg HW_GPIOD 芯片的PORTD端口
 *              @arg HW_GPIOE 芯片的PORTE端口
 * @param[in]  pin  端口上的引脚号 0~31
 * @retval None
 */
void GPIO_ToggleBit(uint32_t instance, uint8_t pin)
{
    GPIO_InstanceTable[instance]->PTOR |= (1 << pin);
}

/**
 * @brief  读取一个端口32位的数据
 * @code
 *      //获取PORTB端口的所有引脚的电平状态
 *      uint32_t status ; //用于存储引脚的状态
 *      status = GPIO_ReadPort(HW_GPIOB); //获取引脚的状态并存储到status中
 * @endcode
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOA 芯片的PORTA端口
 *              @arg HW_GPIOB 芯片的PORTB端口
 *              @arg HW_GPIOC 芯片的PORTC端口
 *              @arg HW_GPIOD 芯片的PORTD端口
 *              @arg HW_GPIOE 芯片的PORTE端口
 * @retval 端口的32位数据
 */
uint32_t GPIO_ReadPort(uint32_t instance)
{
    return (GPIO_InstanceTable[instance]->PDIR);
}
/**
 * @brief  向一个端口写入32位数据
 * @code
 *      //向PORTB端口写入0xFFFFFFFF
 *      GPIO_WriteByte(HW_GPIOB, 0xFFFFFFFF); 
 * @endcode
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOA 芯片的PORTA端口
 *              @arg HW_GPIOB 芯片的PORTB端口
 *              @arg HW_GPIOC 芯片的PORTC端口
 *              @arg HW_GPIOD 芯片的PORTD端口
 *              @arg HW_GPIOE 芯片的PORTE端口
 * @param[in]  data  32位数据
 * @retval None
 */
void GPIO_WritePort(uint32_t instance, uint32_t data)
{
    GPIO_InstanceTable[instance]->PDOR = data;
}


/**
 * @brief  设置GPIO引脚中断类型或者DMA功能
 * @code
 *      //设置PORTB端口的10引脚为下降沿触发中断
 *      GPIO_ITDMAConfig(HW_GPIOB, 10, kGPIO_IT_FallingEdge, true); 
 * @endcode
 * @param[in]  instance GPIO模块号
 *              @arg HW_GPIOA 芯片的PORTA端口
 *              @arg HW_GPIOB 芯片的PORTB端口
 *              @arg HW_GPIOC 芯片的PORTC端口
 *              @arg HW_GPIOD 芯片的PORTD端口
 *              @arg HW_GPIOE 芯片的PORTE端口
 * @param[in]  pin 端口上的引脚号 0~31
 * @param[in]  config 配置模式
 *              @arg kGPIO_DMA_RisingEdge DMA上升沿触发
 *              @arg kGPIO_DMA_FallingEdge DMA下降沿触发
 *              @arg kGPIO_DMA_RisingFallingEdge DMA上升和下降沿都触发
 *              @arg kGPIO_IT_Low 低电平触发中断
 *              @arg kGPIO_IT_RisingEdge 上升沿触发中断
 *              @arg kGPIO_IT_FallingEdge 下降沿触发中断
 *              @arg kGPIO_IT_RisingFallingEdge 上升和下降沿都触发中断
 *              @arg kGPIO_IT_High 高电平触发中断
 * @param[in]  status 是否使能
 *              @arg 0 disable
 *              @arg 1 enable
 * @retval None
 */
void GPIO_ITDMAConfig(uint32_t instance, uint8_t pin, GPIO_ITDMAConfig_Type config, bool status)
{
    /* init moudle */
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    PORT_InstanceTable[instance]->PCR[pin] &= ~PORT_PCR_IRQC_MASK;
    
    if(!status)
    {
        NVIC_DisableIRQ(GPIO_IRQnTable[instance]);
        return;
    }
    
    switch(config)
    {
        case kGPIO_DMA_RisingEdge:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(1);
            break;
        case kGPIO_DMA_FallingEdge:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(2);
            break;
        case kGPIO_DMA_RisingFallingEdge:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(3);
            break;
        case kGPIO_IT_Low:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(8);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_RisingEdge:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(9);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_FallingEdge:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(10);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_RisingFallingEdge:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(11);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_High:
            PORT_InstanceTable[instance]->PCR[pin] |= PORT_PCR_IRQC(12);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        default:
            break;
    }
}
/**
 * @brief  注册中断回调函数
 * @code
 *      //注册PTB10的EXTI中断回调函数
 *      void PTB10_EXTI_ISR(uint32_t pinxArray)
 *      {
 *          if(pinxArray & (1u << 10))
 *              ;//用户自定义函数功能
 *      }
 *      GPIO_CallbackInstall(instance,PTB10_EXTI_ISR);
 * @endcode
 * @attention  该回调函数是有形参的
 * @param[in]  instance GPIO模块中断入口号
 *              @arg HW_GPIOA 芯片的PORTA端口中断入口
 *              @arg HW_GPIOB 芯片的PORTB端口中断入口
 *              @arg HW_GPIOC 芯片的PORTC端口中断入口
 *              @arg HW_GPIOD 芯片的PORTD端口中断入口
 *              @arg HW_GPIOE 芯片的PORTE端口中断入口
 * @param[in]  AppCBFun 回调函数指针入口
 * @retval None
 * @see 对于此函数的具体应用请查阅应用实例
 */
void GPIO_CallbackInstall(uint32_t instance, GPIO_CallBackType AppCBFun)
{
    /* init moudle */
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    if(AppCBFun != NULL)
    {
        GPIO_CallBackTable[instance] = AppCBFun;
    }
}


/**
 * @brief  GPIO中断总处理函数，用户一般无需使用和修改
 * @param[in]  instance GPIO模块中断入口号
 *              @arg HW_GPIOA 芯片的PORTA端口中断入口
 *              @arg HW_GPIOB 芯片的PORTB端口中断入口
 *              @arg HW_GPIOC 芯片的PORTC端口中断入口
 *              @arg HW_GPIOD 芯片的PORTD端口中断入口
 *              @arg HW_GPIOE 芯片的PORTE端口中断入口
 * @attention 静态函数，仅本文件可见
 * @retval None
 */
static void PORT_IRQHandler(uint32_t instance)
{
    uint32_t ISFR;
    /* safe copy */
    ISFR = PORT_InstanceTable[instance]->ISFR;
    /* clear IT pending bit */
    PORT_InstanceTable[instance]->ISFR = 0xFFFFFFFF;
    if(GPIO_CallBackTable[instance])
    {
        GPIO_CallBackTable[instance](ISFR);
    }
}

/**
 * @brief  系统GPIO中断函数，用户无需使用
 */
void PORTA_IRQHandler(void)
{
    PORT_IRQHandler(HW_GPIOA);
}

/**
 * @brief  系统GPIO中断函数，用户无需使用
 */
void PORTB_IRQHandler(void)
{
    PORT_IRQHandler(HW_GPIOB);
}

/**
 * @brief  系统GPIO中断函数，用户无需使用
 */
void PORTC_IRQHandler(void)
{
    PORT_IRQHandler(HW_GPIOC);
}

/**
 * @brief  系统GPIO中断函数，用户无需使用
 */
void PORTD_IRQHandler(void)
{
    PORT_IRQHandler(HW_GPIOD);
}

/**
 * @brief  系统GPIO中断函数，用户无需使用
 */
void PORTE_IRQHandler(void)
{
    PORT_IRQHandler(HW_GPIOE);
}

#if (defined(MK70F12))
/**
 * @brief  系统GPIO中断函数，用户无需使用
 */
void PORTF_IRQHandler(void)
{
    PORT_IRQHandler(HW_GPIOF);
}
#endif


