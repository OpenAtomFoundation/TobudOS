/**
 * @file wm_gpio.c
 *
 * @brief GPIO Driver Module
 *
 * @author dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#include "wm_gpio.h"
#include "wm_regs.h"
#include "wm_irq.h"
//#include "wm_osal.h"
#include "tls_common.h"
#include "stdio.h"

struct gpio_irq_context{
    tls_gpio_irq_callback callback;
    void *arg;
};


static struct gpio_irq_context gpio_context[WM_IO_PB_30 - WM_IO_PA_00 + 1] = {{0,0}};


void GPIOA_IRQHandler(void)
{
	u8  i     = 0;
	u8  found = 0;
	u32 reg   = 0;

    reg = tls_reg_read32(HR_GPIO_MIS);

    for (i = 0; i <= WM_IO_PA_15; i++)
    {
        if (reg & BIT(i))
        {
            found = 1;
            break;
        }
    }

    if (found)
    {
        if (NULL != gpio_context[i].callback)
            gpio_context[i].callback(gpio_context[i].arg);
    }
	return;

}

void GPIOB_IRQHandler(void)
    {
	u8  i     = 0;
	u8  found = 0;
    u32 reg   = 0;
	
    reg = tls_reg_read32(HR_GPIO_MIS + TLS_IO_AB_OFFSET);


    for (i = WM_IO_PB_00; i <= WM_IO_PB_31; i++)
    {
        if (reg & BIT(i - WM_IO_PB_00))
        {
            found = 1;
            break;
        }
    }

    if (found)
    {
        if (NULL != gpio_context[i].callback)
            gpio_context[i].callback(gpio_context[i].arg);
    }

    return;
}

/**
 * @brief          This function is used to config gpio function
 *
 * @param[in]      gpio_pin    gpio pin num
 * @param[in]      dir         gpio direction
 * @param[in]      attr        gpio attribute
 *
 * @return         None
 *
 * @note
 * From gpio3 to gpio7,attribute can set to WM_GPIO_ATTR_PULLHIGH,
 * but can not set to WM_GPIO_ATTR_PULLLOW,
 * the default attribute is WM_GPIO_ATTR_PULLHIGH.
 * Other gpio can set to WM_GPIO_ATTR_PULLLOW,but can not set to WM_GPIO_ATTR_PULLHIGH,the deault
 * attribute is WM_GPIO_ATTR_PULLLOW.
 * all gpio can set to WM_GPIO_ATTR_FLOATING
 */
void tls_gpio_cfg(enum tls_io_name gpio_pin, enum tls_gpio_dir dir, enum tls_gpio_attr attr)
{
    u8  pin;
    u16 offset;

    if (gpio_pin >= WM_IO_PB_00)
    {
        pin    = gpio_pin - WM_IO_PB_00;
        offset = TLS_IO_AB_OFFSET;
    }
    else
    {
        pin    = gpio_pin;
        offset = 0;
    }

    /* enable gpio function */
    tls_io_cfg_set(gpio_pin, WM_IO_OPT5_GPIO);

    /* gpio direction */
    if (WM_GPIO_DIR_OUTPUT == dir)
        tls_reg_write32(HR_GPIO_DIR + offset, tls_reg_read32(HR_GPIO_DIR + offset) | BIT(pin));      /* 1 set output */
    else if (WM_GPIO_DIR_INPUT == dir)
        tls_reg_write32(HR_GPIO_DIR + offset, tls_reg_read32(HR_GPIO_DIR + offset) & (~BIT(pin)));   /* 0 set input */

    /* gpio pull attribute */
    if (WM_GPIO_ATTR_FLOATING == attr)
        tls_reg_write32(HR_GPIO_PULL_EN + offset, tls_reg_read32(HR_GPIO_PULL_EN + offset) | BIT(pin));  	   /* 1 disable pull */
    if ((WM_GPIO_ATTR_PULLHIGH == attr) || (WM_GPIO_ATTR_PULLLOW == attr))
        tls_reg_write32(HR_GPIO_PULL_EN + offset, tls_reg_read32(HR_GPIO_PULL_EN + offset) & (~BIT(pin)));      /* 0 enable pull */
}

/**
 * @brief          This function is used to read gpio status
 *
 * @param[in]      gpio_pin    gpio pin num
 *
 * @retval         0     power level is low
 * @retval         1     power level is high
 *
 * @note           None
 */
u8 tls_gpio_read(enum tls_io_name gpio_pin)
{
	u32 reg_en;
	u32 reg;
	u8  pin;
    u16 offset;

    if (gpio_pin >= WM_IO_PB_00)
    {
        pin    = gpio_pin - WM_IO_PB_00;
        offset = TLS_IO_AB_OFFSET;
    }
    else
    {
        pin    = gpio_pin;
        offset = 0;
    }

	reg_en = tls_reg_read32(HR_GPIO_DATA_EN + offset);
	tls_reg_write32(HR_GPIO_DATA_EN + offset, reg_en | (1 << pin));
	reg = tls_reg_read32(HR_GPIO_DATA + offset);
	tls_reg_write32(HR_GPIO_DATA_EN + offset, reg_en);
	if(reg & (0x1 << pin))
		return 1;
	else
		return 0;
}

/**
 * @brief          This function is used to modify gpio status
 *
 * @param[in]      gpio_pin    gpio pin num
 * @param[in]      value       power level
 *                             0: low  power level
 * 					           1: high power level
 *
 * @return         None
 *
 * @note           None
 */
void tls_gpio_write(enum tls_io_name gpio_pin, u8 value)
{
//	u32 cpu_sr = 0;
	u32 reg;
	u32	reg_en;
  u8  pin;
    u16 offset;

    if (gpio_pin >= WM_IO_PB_00)
    {
        pin    = gpio_pin - WM_IO_PB_00;
        offset = TLS_IO_AB_OFFSET;
    }
    else
    {
        pin    = gpio_pin;
        offset = 0;
    }

	
	//cpu_sr = tls_os_set_critical();
	
	reg_en = tls_reg_read32(HR_GPIO_DATA_EN + offset);
	tls_reg_write32(HR_GPIO_DATA_EN + offset, reg_en | (1 << pin));

	reg = tls_reg_read32(HR_GPIO_DATA + offset);
	if(value)
		tls_reg_write32(HR_GPIO_DATA + offset, reg |  (1 << pin));	/* write high */
	else
		tls_reg_write32(HR_GPIO_DATA + offset, reg & (~(1 << pin)));/* write low */

    tls_reg_write32(HR_GPIO_DATA_EN + offset, reg_en);

	//tls_os_release_critical(cpu_sr);
}

/**
 * @brief          This function is used to config gpio interrupt
 *
 * @param[in]      gpio_pin    gpio pin num
 * @param[in]      mode        interrupt trigger type
 *
 * @return         None
 *
 * @note           None
 */
void tls_gpio_irq_enable(enum tls_io_name gpio_pin, enum tls_gpio_irq_trig mode)
{
	u32 reg;
	u8  pin;
    u16 offset;
    u8  vec_no;

    if (gpio_pin >= WM_IO_PB_00)
    {
        pin    = gpio_pin - WM_IO_PB_00;
        offset = TLS_IO_AB_OFFSET;
        vec_no = GPIOB_INT;
    }
    else
    {
        pin    = gpio_pin;
        offset = 0;
        vec_no = GPIO_INT;
    }

//	TLS_DBGPRT_INFO("\r\ntls_gpio_int_enable gpio pin =%d,mode==%d\r\n",gpio_pin,mode);

	switch(mode)
	{
		case WM_GPIO_IRQ_TRIG_RISING_EDGE:
			reg = tls_reg_read32(HR_GPIO_IS + offset);
			reg &= (~(0x1 << pin));
		//	TLS_DBGPRT_INFO("\r\nrising edge is ret=%x\r\n",reg);
			tls_reg_write32(HR_GPIO_IS + offset, reg);		/* 0 edge trigger */
			reg = tls_reg_read32(HR_GPIO_IBE + offset);
			reg &= (~(0x1 << pin));
		//	TLS_DBGPRT_INFO("\r\nrising edge ibe ret=%x\r\n",reg);
			tls_reg_write32(HR_GPIO_IBE + offset, reg);	/* 0 edge trigger */
			reg = tls_reg_read32(HR_GPIO_IEV + offset);
			reg |= (0x1 << pin);
		//	TLS_DBGPRT_INFO("\r\nrising edge iev ret=%x\r\n",reg);
			tls_reg_write32(HR_GPIO_IEV + offset, reg);	/* 1 rising edge trigger */
			break;
		case WM_GPIO_IRQ_TRIG_FALLING_EDGE:
			reg = tls_reg_read32(HR_GPIO_IS + offset);
			reg &= (~(0x1 << pin));
		//	TLS_DBGPRT_INFO("\falling edge is ret=%x\n",reg);
			tls_reg_write32(HR_GPIO_IS + offset, reg);		/* 0 edge trigger */
			reg = tls_reg_read32(HR_GPIO_IBE + offset);
			reg &= (~(0x1 << pin));
		//	TLS_DBGPRT_INFO("\falling edge ibe ret=%x\n",reg);
			tls_reg_write32(HR_GPIO_IBE + offset, reg);	/* 0 edge trigger */
			reg = tls_reg_read32(HR_GPIO_IEV + offset);
			reg &= (~(0x1 << pin));
		//	TLS_DBGPRT_INFO("\falling edge iev ret=%x\n",reg);
			tls_reg_write32(HR_GPIO_IEV + offset, reg);	/* 0 falling edge trigger */
			break;
		case WM_GPIO_IRQ_TRIG_DOUBLE_EDGE:
			reg = tls_reg_read32(HR_GPIO_IS + offset);
			tls_reg_write32(HR_GPIO_IS + offset, reg & (~(0x1 << pin)));	/* 0 edge trigger */
			reg = tls_reg_read32(HR_GPIO_IBE + offset);
			tls_reg_write32(HR_GPIO_IBE + offset, reg | (0x1 << pin));       /* 1 double edge trigger */
			break;
		case WM_GPIO_IRQ_TRIG_HIGH_LEVEL:
			reg = tls_reg_read32(HR_GPIO_IS + offset);
			tls_reg_write32(HR_GPIO_IS + offset, reg | (0x1 << pin));		/* 1 level trigger */
			reg = tls_reg_read32(HR_GPIO_IEV + offset);
			tls_reg_write32(HR_GPIO_IEV + offset, reg | (0x1 << pin));		/* 1 high level trigger */
			break;
		case WM_GPIO_IRQ_TRIG_LOW_LEVEL:
			reg = tls_reg_read32(HR_GPIO_IS + offset);
			tls_reg_write32(HR_GPIO_IS + offset, reg | (0x1 << pin));		/* 1 level trigger */
			reg = tls_reg_read32(HR_GPIO_IEV + offset);
			tls_reg_write32(HR_GPIO_IEV + offset, reg & (~(0x1 << pin)));	/* 0 low level trigger */
			break;
	}

	reg = tls_reg_read32(HR_GPIO_IE + offset);
	reg |= (0x1 << pin);
//	TLS_DBGPRT_INFO("\nie ret=%x\n",reg);
	tls_reg_write32(HR_GPIO_IE + offset, reg);		/* enable interrupt */

	tls_irq_enable(vec_no);
}

/**
 * @brief          This function is used to disable gpio interrupt
 *
 * @param[in]      gpio_pin    gpio pin num
 *
 * @return         None
 *
 * @note           None
 */
void tls_gpio_irq_disable(enum tls_io_name gpio_pin)
{
	u32 reg;
	u8  pin;
    u16 offset;

    if (gpio_pin >= WM_IO_PB_00)
    {
        pin    = gpio_pin - WM_IO_PB_00;
        offset = TLS_IO_AB_OFFSET;
    }
    else
    {
        pin    = gpio_pin;
        offset = 0;
    }

	reg = tls_reg_read32(HR_GPIO_IE + offset);
	tls_reg_write32(HR_GPIO_IE + offset, reg & (~(0x1 << pin)));	/* disable interrupt */
	tls_irq_disable(GPIO_INT);
}

/**
 * @brief          This function is used to get gpio interrupt status
 *
 * @param[in]      gpio_pin    gpio pin num
 *
 * @retval         0     no interrupt happened
 * @retval         1     interrupt happened
 *
 * @note           None
 */
u8 tls_get_gpio_irq_status(enum tls_io_name gpio_pin)
{
	u32 reg;
	u8  pin;
    u16 offset;

    if (gpio_pin >= WM_IO_PB_00)
    {
        pin    = gpio_pin - WM_IO_PB_00;
        offset = TLS_IO_AB_OFFSET;
    }
    else
    {
        pin    = gpio_pin;
        offset = 0;
    }

	reg = tls_reg_read32(HR_GPIO_RIS + offset);
	if(reg & (0x1 << pin))
		return	1;
	else
		return	0;
}

/**
 * @brief          This function is used to clear gpio interrupt flag
 *
 * @param[in]      gpio_pin    gpio pin num
 *
 * @return         None
 *
 * @note           None
 */
void tls_clr_gpio_irq_status(enum tls_io_name gpio_pin)
{
	u32 reg;
	u8  pin;
    u16 offset;

    if (gpio_pin >= WM_IO_PB_00)
    {
        pin    = gpio_pin - WM_IO_PB_00;
        offset = TLS_IO_AB_OFFSET;
    }
    else
    {
        pin    = gpio_pin;
        offset = 0;
    }

	reg = tls_reg_read32(HR_GPIO_IC + offset);
	tls_reg_write32(HR_GPIO_IC + offset, reg | (0x1 << pin));		/* 1 clear interrupt status */
}

/**
 * @brief          This function is used to register gpio interrupt
 *
 * @param[in]      gpio_pin    gpio pin num
 * @param[in]      callback    the gpio interrupt call back function
 * @param[in]      arg         parammeter for the callback
 *
 * @return         None
 *
 * @note
 * gpio callback function is called in interrupt,
 * so can not operate the critical data in the callback fuuction,
 * recommendation to send messages to other tasks to operate it.
 */
void tls_gpio_isr_register(enum tls_io_name gpio_pin,
                           tls_gpio_irq_callback callback,
                           void *arg)
{
    gpio_context[gpio_pin].callback = callback;
    gpio_context[gpio_pin].arg = arg;
}

