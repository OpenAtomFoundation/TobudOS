/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-02-15 20:51:38
 * @LastEditTime : 2020-02-16 00:05:59
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "bsp_led.h"   
#include "bsp_dwt.h"   


void clk_produce(void)
{
    SCL_LOW;        // SCL=0
    delay_us(50);
    
    SCL_HIGH;       // SCL=1
    delay_us(50);
}


void send_32bit_zero(void)
{
    unsigned char i;
    SDA_LOW;   // SDA=0
    for (i=0; i<32; i++)
    {
        clk_produce();
    }
}


uint8_t take_anti_code(uint8_t dat)
{
    uint8_t tmp = 0;

    tmp=((~dat) & 0xC0)>>6;
    return tmp;
}


void send_data(uint32_t dx)
{
    uint8_t i;

    for (i=0; i<32; i++)
    {
        if ((dx & 0x80000000) != 0)
            SDA_HIGH;     //  SDA=1;
        else
            SDA_LOW;    //  SDA=0;

        dx <<= 1;
        clk_produce();	
    }
}


void data_deal_with_send(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t dx = 0;

    dx |= (uint32_t)0x03 << 30;             // The front of the two bits 1 is flag bits
    dx |= (uint32_t)take_anti_code(b) << 28;
    dx |= (uint32_t)take_anti_code(g) << 26;
    dx |= (uint32_t)take_anti_code(r) << 24;

    dx |= (uint32_t)b << 16;
    dx |= (uint32_t)g << 8;
    dx |= r;

    send_data(dx);
}

void rgb_led_init(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef  GPIO_InitStruct;
    
    RGB_LED_GPIO_CLK_ENABLE();
    
    /*选择要控制的GPIO引脚*/															   
    GPIO_InitStruct.Pin = RGB_LED_CLK_PIN;	

    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  

    /*设置引脚为上拉模式*/
    GPIO_InitStruct.Pull  = GPIO_PULLUP;

    /*设置引脚速率为高速 */   
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    /*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
    HAL_GPIO_Init(RGB_LED_CLK_GPIO_PORT, &GPIO_InitStruct);	

    /*选择要控制的GPIO引脚*/															   
    GPIO_InitStruct.Pin = RGB_LED_SDA_PIN;	
    HAL_GPIO_Init(RGB_LED_SDA_GPIO_PORT, &GPIO_InitStruct);	

    send_32bit_zero();
    data_deal_with_send(0,0,0);	 
    data_deal_with_send(0,0,0);
}

void rgb_led_control(uint8_t r, uint8_t g, uint8_t b)
{
    send_32bit_zero();
    data_deal_with_send(r, g, b);
    data_deal_with_send(r, g, b);
}

/*********************************************END OF FILE**********************/
