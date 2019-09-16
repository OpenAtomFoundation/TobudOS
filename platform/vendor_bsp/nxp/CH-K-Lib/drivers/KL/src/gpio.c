/**
  ******************************************************************************
  * @file    gpio.c
  * @author  YANDLD
  * @version V2.6
  * @date    2015.6.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "common.h"
#include "gpio.h"

/* gloabl vars */
#ifndef GPIO_BASES
#define GPIO_BASES {PTA, PTB, PTC, PTD, PTE}
#endif

static GPIO_Type * const GPIO_IPTbl[] = GPIO_BASES;
static PORT_Type * const PORT_IPTbl[] = PORT_BASES;

static const Reg_t CLKTbl[] =
{
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTA_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTB_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTC_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTD_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTE_MASK, SIM_SCGC5_PORTE_SHIFT},
};

#if defined(MKL27Z4)
static const IRQn_Type GPIO_IrqTbl[] = 
{
    (IRQn_Type)PORTA_IRQn,
    (IRQn_Type)0,
    (IRQn_Type)PORTCD_IRQn,
    (IRQn_Type)PORTCD_IRQn,
};
#else
static const IRQn_Type GPIO_IrqTbl[] = 
{
    (IRQn_Type)(PORTA_IRQn+0),
    (IRQn_Type)(PORTA_IRQn+1),
    (IRQn_Type)(PORTA_IRQn+2),
    (IRQn_Type)(PORTA_IRQn+3),
    (IRQn_Type)(PORTA_IRQn+4),
};
#endif


//! @defgroup CHKinetis
//! @{


//! @defgroup GPIO
//! @brief GPIO API functions
//! @{


 /**
 * @brief  设置引脚为输入还是输出功能  用户一般不必调用
 * @note   只有当引脚作为GPIO时才有意义
 * @code
 *      // 将PORTB端口的3引脚设置输入引脚
 *      GPIO_PinConfig(HW_GPIOB, 3, kInpput);
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @param  pinIndex  :端口上的引脚号 0~31
 * @param  mode      :输入或者输出设置
 *         @arg kInpput  :输入功能选择
 *         @arg kOutput  :输出功能选择
 * @retval None
 */
void inline GPIO_SetPinDir(uint32_t instance, uint32_t pin, uint32_t dir)
{
    CLK_EN(CLKTbl, instance);
    (dir == 1) ? (GPIO_IPTbl[instance]->PDDR |= (1 << pin)):
    (GPIO_IPTbl[instance]->PDDR &= ~(1 << pin));
}

 /**
 * @brief  init a pin
 * @note   can also init mutiple pin
 * @param  instance:
 *         @arg HW_GPIOx : GPIO moudle x
 * @param  pin : pin number from 0-31
 * @param  mode : pin operation mode
 *         @arg kGPIO_Mode_IFT : input floating
 *         @arg kGPIO_Mode_IPD : pull down
 *         @arg kGPIO_Mode_IPU : pull up
 *         @arg kGPIO_Mode_OOD : open drain
 *         @arg kGPIO_Mode_OPP : push pull
 * @retval None
 */
uint32_t GPIO_Init(uint32_t instance, uint32_t pin, GPIO_Mode_t mode)
{
    int i;
    if(pin < 32)
    {
        switch(mode)
        {
            case kGPIO_Mode_IFT:
                SetPinPull(instance, pin, 0xFF);
                GPIO_SetPinDir(instance, pin, 0);
                break;
            case kGPIO_Mode_IPD:
                SetPinPull(instance, pin, 0);
                GPIO_SetPinDir(instance, pin, 0);
                break;
            case kGPIO_Mode_IPU:
                SetPinPull(instance, pin, 1);
                GPIO_SetPinDir(instance, pin, 0);
                break;
            case kGPIO_Mode_OPP:
                SetPinPull(instance, pin, 0xFF);
                GPIO_SetPinDir(instance, pin, 1);
                break;
            default:
                break;					
        }
        /* config pinMux */
        SetPinMux(instance, pin, 1);
    }
    else
    {
        for(i=0; i<31; i++)
        {
            if(pin & (1<<i))
            {
                switch(mode)
                {
                    case kGPIO_Mode_IFT:
                        SetPinPull(instance, i, 0xFF);
                        GPIO_SetPinDir(instance, i, 0);
                        break;
                    case kGPIO_Mode_IPD:
                        SetPinPull(instance, i, 0);
                        GPIO_SetPinDir(instance, i, 0);
                        break;
                    case kGPIO_Mode_IPU:
                        SetPinPull(instance, i, 1);
                        GPIO_SetPinDir(instance, i, 0);
                        break;
                    case kGPIO_Mode_OPP:
                        SetPinPull(instance, i, 0xFF);
                        GPIO_SetPinDir(instance, i, 1);
                        break;
                    default:
                        break;					
                }
            }
            SetPinMux(instance, i, 1);
        }
    }
    return instance;
}


 /**
 * @brief  设置指定引脚输出高电平或者低电平
 * @note   此引脚首先配置成输出引脚
 * @code
 *      //设置PORTB端口的10引脚输出高电平
 *      GPIO_WriteBit(HW_GPIOB, 10, 1);
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @param  pinIndex  :端口上的引脚号 0~31
 * @param  data   : 引脚的电平状态  
 *         @arg 0 : 低电平 
 *         @arg 1 : 高电平
 * @retval None
 */
void inline GPIO_PinWrite(uint32_t instance, uint32_t pin, uint8_t data)
{
    (data) ? (GPIO_IPTbl[instance]->PSOR |= (1 << pin)):
    (GPIO_IPTbl[instance]->PCOR |= (1 << pin));
}
 /**
 * @brief  读取一个引脚上的电平状态
 * @code
 *      //读取PORTB端口的10引脚的电平状态
 *      uint8_t status ; //用于存储引脚的状态
 *      status = GPIO_ReadBit(HW_GPIOB, 10); //获取引脚的状态并存储到status中
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @param  pinIndex  :端口上的引脚号 0~31
 * @retval 
 *         @arg 0 : 低电平
 *         @arg 1 : 高电平
 */
uint32_t GPIO_PinRead(uint32_t instance, uint32_t pin)
{
    return ((GPIO_IPTbl[instance]->PDIR >> pin) & 0x01);
}

 /**
 * @brief  翻转一个引脚的电平状态
 * @code
 *      //翻转PORTB端口的10引脚的电平状态
 *      GPIO_ToggleBit(HW_GPIOB, 10); 
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @param  pinIndex  :端口上的引脚号 0~31
 * @retval None
 */
void GPIO_PinToggle(uint32_t instance, uint8_t pin)
{
    GPIO_IPTbl[instance]->PTOR |= (1 << pin);
}

/**
 * @brief  读取一个端口32位的数据
 * @code
 *      //获取PORTB端口的所有引脚的电平状态
 *      uint32_t status ; //用于存储引脚的状态
 *      status = GPIO_ReadPort(HW_GPIOB); //获取引脚的状态并存储到status中
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @retval 端口的32位数据
 */
uint32_t GPIO_ReadPort(uint32_t instance)
{
    return (GPIO_IPTbl[instance]->PDIR);
}
/**
 * @brief  向一个端口写入32位数据
 * @code
 *      //向PORTB端口写入0xFFFFFFFF
 *      GPIO_WriteByte(HW_GPIOB, 0xFFFFFFFF); 
 * @endcode
 * @param  instance: GPIO模块号
 *         @arg HW_GPIOA :芯片的PORTA端口
 *         @arg HW_GPIOB :芯片的PORTB端口
 *         @arg HW_GPIOC :芯片的PORTC端口
 *         @arg HW_GPIOD :芯片的PORTD端口
 *         @arg HW_GPIOE :芯片的PORTE端口
 * @param  data  :32位数据
 * @retval None
 */
void GPIO_WritePort(uint32_t instance, uint32_t data)
{
    GPIO_IPTbl[instance]->PDOR = data;
}

/**
 * @brief  set pin interrupt mode
 * @retval None
 */
int GPIO_SetIntMode(uint32_t instance, uint32_t pin, GPIO_Int_t mode, bool val)
{
    CLK_EN(CLKTbl, instance);
    if(!val)
    {
        PORT_IPTbl[instance]->PCR[pin] &= ~PORT_PCR_IRQC_MASK;
        return 0;
    }
    NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
    PORT_IPTbl[instance]->PCR[pin] &= ~PORT_PCR_IRQC_MASK;
    switch(mode)
    {
        case kGPIO_Int_RE:
            PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_IRQC(9);
            break;
        case kGPIO_Int_FE:
            PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_IRQC(10);
            break;
        case kGPIO_Int_EE:
            PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_IRQC(11);
            break;
    }
    return 0;
}


//! @}
