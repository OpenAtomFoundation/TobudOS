/*!
    \file  gd32f1x0_gpio.c
    \brief GPIO driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform for GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform for GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#include "gd32f1x0_gpio.h"

/*!
    \brief      reset GPIO port
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F)
    \param[out] none
    \retval     none
*/
void gpio_deinit(uint32_t gpio_periph)
{
    switch(gpio_periph){
    case GPIOA:
        /* reset GPIOA */
        rcu_periph_reset_enable(RCU_GPIOARST);
        rcu_periph_reset_disable(RCU_GPIOARST);
        break;
    case GPIOB:
        /* reset GPIOB */
        rcu_periph_reset_enable(RCU_GPIOBRST);
        rcu_periph_reset_disable(RCU_GPIOBRST);
        break;
    case GPIOC:
        /* reset GPIOC */
        rcu_periph_reset_enable(RCU_GPIOCRST);
        rcu_periph_reset_disable(RCU_GPIOCRST);
        break;
    case GPIOD:
        /* reset GPIOD */
        rcu_periph_reset_enable(RCU_GPIODRST);
        rcu_periph_reset_disable(RCU_GPIODRST);
        break;
    case GPIOF:
        /* reset GPIOF */
        rcu_periph_reset_enable(RCU_GPIOFRST);
        rcu_periph_reset_disable(RCU_GPIOFRST);
        break;
    default:
        break;
    }
}

/*!
    \brief      set GPIO output mode
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F) 
    \param[in]  mode: gpio pin mode
      \arg        GPIO_MODE_INPUT: input mode
      \arg        GPIO_MODE_OUTPUT: output mode
      \arg        GPIO_MODE_AF: alternate function mode
      \arg        GPIO_MODE_ANALOG: analog mode
    \param[in]  pull_up_down: gpio pin with pull-up or pull-down resistor
      \arg        GPIO_PUPD_NONE: without weak pull-up and pull-down resistors
      \arg        GPIO_PUPD_PULLUP: with weak pull-up resistor
      \arg        GPIO_PUPD_PULLDOWN:with weak pull-down resistor
    \param[in]  pin: GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \param[out] none
    \retval     none
*/
void gpio_mode_set(uint32_t gpio_periph,uint32_t mode,uint32_t pull_up_down,uint32_t pin)
{
    uint16_t i;
    uint32_t ctl, pupd;

    ctl = GPIO_CTL(gpio_periph);
    pupd = GPIO_PUD(gpio_periph);

    for (i = 0U; i < 16U; i++){
        if((1U << i) & pin){
            /* clear the specified pin mode bits */
            ctl &= ~GPIO_MODE_MASK(i);
            /* set the specified pin mode bits */
            ctl |= GPIO_MODE_SET(i, mode);

            /* clear the specified pin pupd bits */
            pupd &= ~GPIO_PUPD_MASK(i);
            /* set the specified pin pupd bits */
            pupd |= GPIO_PUPD_SET(i, pull_up_down);
        }
    }

    GPIO_CTL(gpio_periph) = ctl;
    GPIO_PUD(gpio_periph) = pupd;
}

/*!
    \brief      set GPIO output type and speed
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F) 
    \param[in]  otype: gpio pin output mode
      \arg        GPIO_OTYPE_PP: push pull mode
      \arg        GPIO_OTYPE_OD: open drain mode
    \param[in]  speed
      \arg        GPIO_OSPEED_2MHZ: output max speed 2M 
      \arg        GPIO_OSPEED_10MHZ: output max speed 10M 
      \arg        GPIO_OSPEED_50MHZ: output max speed 50M 
    \param[in]  pin: GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \param[out] none
    \retval     none
*/
void gpio_output_options_set(uint32_t gpio_periph,uint8_t otype,uint32_t speed,uint32_t pin)
{
    uint16_t i;
    uint32_t ospeedr;

    if(0x1U == otype){
        GPIO_OMODE(gpio_periph) |= (uint32_t)pin;
    }else{
        GPIO_OMODE(gpio_periph) &= (uint32_t)(~pin);
    }

    /* get the specified pin output speed bits value */
    ospeedr = GPIO_OSPD(gpio_periph);

    for(i = 0U;i < 16U;i++){
        if((1U << i) & pin){
            /* clear the specified pin output speed bits */
            ospeedr &= ~GPIO_OSPEED_MASK(i);
            /* set the specified pin output speed bits */
            ospeedr |= GPIO_OSPEED_SET(i,speed);
        }
    }
    GPIO_OSPD(gpio_periph) = ospeedr;
}

/*!
    \brief      set GPIO pin
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F) 
    \param[in]  pin: GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \param[out] none
    \retval     none
*/
void gpio_bit_set(uint32_t gpio_periph,uint32_t pin)
{
    GPIO_BOP(gpio_periph) = (uint32_t)pin;
}

/*!
    \brief      reset GPIO pin
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F) 
    \param[in]  pin: GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \param[out] none
    \retval     none
*/
void gpio_bit_reset(uint32_t gpio_periph,uint32_t pin)
{
    GPIO_BC(gpio_periph) = (uint32_t)pin;
}

/*!
    \brief      write data to the specified GPIO pin
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F) 
    \param[in]  pin: GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \param[in]  bit_value: SET or RESET
      \arg        RESET: clear the port pin
      \arg        SET: set the port pin
    \param[out] none
    \retval     none
*/
void gpio_bit_write(uint32_t gpio_periph,uint32_t pin,bit_status bit_value)
{
    if(RESET != bit_value){
        GPIO_BOP(gpio_periph) = (uint32_t)pin;
    }else{
        GPIO_BC(gpio_periph) = (uint32_t)pin;
    }
}

/*!
    \brief      write data to the specified GPIO port
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F) 
    \param[in]  data: specify the value to be written to the port output control register
    \param[out] none
    \retval     none
*/
void gpio_port_write(uint32_t gpio_periph,uint16_t data)
{
    GPIO_OCTL(gpio_periph) = (uint32_t)data;
}

/*!
    \brief      get GPIO pin input status
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F) 
    \param[in]  pin: GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \param[out] none
    \retval     input state of gpio pin: SET or RESET
*/
FlagStatus gpio_input_bit_get(uint32_t gpio_periph,uint32_t pin)
{
    if((uint32_t)RESET != (GPIO_ISTAT(gpio_periph)&(pin))){
        return SET; 
    }else{
        return RESET;
    }
}

/*!
    \brief      get GPIO all pins input status
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F) 
    \param[out] none
    \retval     input state of gpio all pins
*/
uint16_t gpio_input_port_get(uint32_t gpio_periph)
{
    return (uint16_t)(GPIO_ISTAT(gpio_periph));
}

/*!
    \brief      get GPIO pin output status
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F) 
    \param[in]  pin: GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \param[out] none
    \retval     output state of gpio pin: SET or RESET
*/
FlagStatus gpio_output_bit_get(uint32_t gpio_periph,uint32_t pin)
{
    if((uint32_t)RESET !=(GPIO_OCTL(gpio_periph)&(pin))){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      get GPIO all pins output status
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F) 
    \param[out] none
    \retval     output state of gpio all pins
*/
uint16_t gpio_output_port_get(uint32_t gpio_periph)
{
    return ((uint16_t)GPIO_OCTL(gpio_periph));
}

/*!
    \brief      set GPIO alternate function
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F) 
    \param[in]  alt_func_num: gpio pin af function, please refer to specific device datasheet
      \arg        GPIO_AF_0: TIMER2, TIMER13, TIMER14, TIMER16, SPI0, I2S0, SPI1, SPI2, I2S2, CK_OUT, 
                            SWDIO, SWCLK, USART0, CEC, IFRP, I2C0, I2C1, TSI, EVENTOUT
      \arg        GPIO_AF_1: USART0, USART1, IFRP, CEC, TIMER2, TIMER14, I2C0, I2C1, I2C2, EVENTOUT
      \arg        GPIO_AF_2: TIMER0, TIMER1, TIMER15, TIMER16, EVENTOUT
      \arg        GPIO_AF_3: TSI, I2C0, TIMER14, EVENTOUT
      \arg        GPIO_AF_4(port A,B only): TIMER13, I2C0, I2C1, I2C2, USART1
      \arg        GPIO_AF_5(port A,B only): TIMER15, TIMER16, SPI2, I2S2, I2C0, I2C1
      \arg        GPIO_AF_6(port A,B only): SPI1, EVENTOUT 
      \arg        GPIO_AF_7(port A,B only): CMP0, CMP1
      \arg        GPIO_AF_9(port A,B only): CAN0, CAN1 (for GD32F170xx and GD32F190xx devices)
      \arg        GPIO_AF_11: SLCD (for GD32F170xx and GD32F190xx devices)
    \param[in]  pin: GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \param[out] none    
    \retval     none
*/
void gpio_af_set(uint32_t gpio_periph,uint32_t alt_func_num,uint32_t pin)
{
    uint16_t i;
    uint32_t afrl, afrh;

    afrl = GPIO_AFSEL0(gpio_periph);
    afrh = GPIO_AFSEL1(gpio_periph);

    for (i = 0U; i < 8U; i++){
        if((1U << i) & pin){
            /* clear the specified pin alternate function bits */
            afrl &= ~GPIO_AFR_MASK(i);
            afrl |= GPIO_AFR_SET(i,alt_func_num);
        }
    }

    for (i = 8U; i < 16U; i++){
        if((1U << i) & pin){
            /* clear the specified pin alternate function bits */
            afrh &= ~GPIO_AFR_MASK(i - 8U);
            afrh |= GPIO_AFR_SET(i - 8U,alt_func_num);
        }
    }

    GPIO_AFSEL0(gpio_periph) = afrl;
    GPIO_AFSEL1(gpio_periph) = afrh;
}

/*!
    \brief      lock GPIO pin
    \param[in]  gpio_periph: GPIOx(x = A,B) 
    \param[in]  pin: GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \param[out] none
    \retval     none
*/
void gpio_pin_lock(uint32_t gpio_periph,uint32_t pin)
{
    uint32_t lock = 0x00010000U;
    lock |= pin;

    /* lock key writing sequence: write 1->write 0->write 1-> read 0-> read 1 */
    GPIO_LOCK(gpio_periph) = (uint32_t)lock;
    GPIO_LOCK(gpio_periph) = (uint32_t)pin;
    GPIO_LOCK(gpio_periph) = (uint32_t)lock;
    lock = GPIO_LOCK(gpio_periph);
    lock = GPIO_LOCK(gpio_periph);
}

#ifdef GD32F170_190
/*!
    \brief      toggle GPIO pin
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F) 
    \param[in]  pin: GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \param[out] none
    \retval     none
*/
void gpio_bit_toggle(uint32_t gpio_periph,uint32_t pin)
{
    GPIO_TG(gpio_periph) = (uint32_t)pin;
}

/*!
    \brief      toggle GPIO port
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,F)
    \param[out] none
    \retval     none
*/
void gpio_port_toggle(uint32_t gpio_periph)
{
    GPIO_TG(gpio_periph) = 0x0000FFFFU;
}

#endif /* GD32F170_190 */
