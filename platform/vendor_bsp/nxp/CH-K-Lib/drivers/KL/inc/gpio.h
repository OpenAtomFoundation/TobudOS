/**
  ******************************************************************************
  * @file    gpio.h
  * @author  YANDLD
  * @version V2.6
  * @date    2015.6.21
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_KL_GPIO_H__
#define __CH_LIB_KL_GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 

#include <stdint.h>
#include <stdbool.h>

/* GPIO instance define */
#define HW_GPIOA        (0x00U)
#define HW_GPIOB        (0x01U)
#define HW_GPIOC        (0x02U)
#define HW_GPIOD        (0x03U)
#define HW_GPIOE        (0x04U)
#define HW_GPIOF        (0x05U)


#define PIN0        (0x80000001U)
#define PIN1        (0x80000002U)
#define PIN2        (0x80000004U)
#define PIN3        (0x80000008U)
#define PIN4        (0x80000010U)
#define PIN5        (0x80000020U)
#define PIN6        (0x80000040U)
#define PIN7        (0x80000080U)
#define PIN8        (0x80000100U)
#define PIN31       (0x80000000U)

typedef enum
{
    kGPIO_Mode_IFT = 0x00,       /* floating input   */
    kGPIO_Mode_IPD = 0x01,       /* Pull down input */
    kGPIO_Mode_IPU = 0x02,       /* Pull up input    */
    kGPIO_Mode_OPP = 0x04,       /* Push pull output */
}GPIO_Mode_t;

typedef enum
{
    kGPIO_Int_RE, /* raising edge */
    kGPIO_Int_FE, /* falling edge */
    kGPIO_Int_EE, /* falling edge and raising edge */
}GPIO_Int_t;

//!< API functions
uint32_t GPIO_Init(uint32_t instance, uint32_t pin, GPIO_Mode_t mode);
void GPIO_SetPinDir(uint32_t instance, uint32_t pin, uint32_t dir);
int GPIO_SetIntMode(uint32_t instance, uint32_t pin, GPIO_Int_t mode, bool val);
void GPIO_PinWrite(uint32_t instance, uint32_t pin, uint8_t data);
void GPIO_PinToggle(uint32_t instance, uint8_t pin);
uint32_t GPIO_PinRead(uint32_t instance, uint32_t pin);
void GPIO_WritePort(uint32_t instance, uint32_t data);
uint32_t GPIO_ReadPort(uint32_t instance);

#ifdef __cplusplus
}
#endif

#endif


