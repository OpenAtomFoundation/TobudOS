/*!
    \file  touch_panel.c 
    \brief LCD touch panel functions

    \version 2019-06-05, V1.0.0, demo for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32vf103.h"
#include "touch_panel.h"
#include "math.h"
#include <stdlib.h>

/* number of filter reads */
#define FILTER_READ_TIMES           5
/* lost value of filter */
#define FILTER_LOST_VAL             1
/* error range of AD sample value */  
#define AD_ERR_RANGE                6


uint16_t touch_ad_x=0,touch_ad_y=0;

/* set or reset touch screen chip select pin */
static void spi_cs(uint8_t a);
/* set or reset SPI MOSI pin */
static void spi_mosi(uint8_t a);
/* set or reset touch screen clock SPI SCK pin */
static void spi_clk(uint8_t a);
/* get SPI MISO pin input status */
static FlagStatus spi_miso(void);
/* SPI delay function */
static void spi_delay(uint16_t i);

/*!
    \brief      touch panel gpio configure
    \param[in]  none 
    \param[out] none
    \retval     none
*/
void touch_panel_gpio_configure(void)
{
    /* GPIO clock enable */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOE);
    
    /* SCK(PA5) pin configure */
    gpio_init(SPI_SCK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI_SCK_PIN);
    /* MOSI(PA7) pin configure */
    gpio_init(SPI_MOSI_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI_MOSI_PIN);
    /* MISO(PA6) pins configure */
    gpio_init(SPI_MISO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SPI_MISO_PIN);

    /* Chip select(SPI-Touch) PE4 pin configure */
    gpio_init(SPI_TOUCH_CS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI_TOUCH_CS_PIN);
    
    /* Touch pen IRQ pin configure */
    gpio_init(TOUCH_PEN_INT_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, TOUCH_PEN_INT_PIN);
    
    /* Set chip select pin high */
    SPI_TOUCH_CS_HIGH();
}


/*!
    \brief      set or reset touch screen chip select pin
    \param[in]  a: specified the low or high level of chip select pin output
    \param[out] none
    \retval     none
*/
static void spi_cs(uint8_t a)
{
    if(a){
        SPI_TOUCH_CS_HIGH();
    }else{
        SPI_TOUCH_CS_LOW();
    }
}

/*!
    \brief      set or reset SPI MOSI pin
    \param[in]  a: specified the low or high level of SPI MOSI pin output
    \param[out] none
    \retval     none
*/
static void spi_mosi(uint8_t a)
{
    if(a){
        SPI_MOSI_HIGH();
    }else{
        SPI_MOSI_LOW();
    }
}

/*!
    \brief      set or reset touch screen clock SPI SCK pin
    \param[in]  a: specified the low or high level of SPI SCK pin output
    \param[out] none
    \retval     none
*/
static void spi_clk(uint8_t a)
{
    if(a){
        SPI_SCK_HIGH();
    }else{
        SPI_SCK_LOW();
    }
}

/*!
    \brief      get SPI MISO pin input status
    \param[in]  none
    \param[out] none
    \retval     input status of gpio pin: SET or RESET
*/
static FlagStatus spi_miso(void)
{
    return SPI_MISO_READ();
}

/*!
    \brief      SPI delay function
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi_delay(uint16_t i)
{
    uint16_t k;
    for (k=0;k<i;k++);
}

/*!
    \brief      touch start
    \param[in]  none
    \param[out] none
    \retval     none
*/
void touch_start(void)
{
    spi_clk(0);
    spi_cs(1);
    spi_mosi(1);
    spi_clk(1);
    spi_cs(0);
}

/*!
    \brief      write data to touch screen
    \param[in]  d: the data to be written
    \param[out] none
    \retval     none
*/
void touch_write(uint8_t d)
{
    uint8_t buf, i ;
    spi_clk(0);
    for( i = 0; i < 8; i++){
        buf = ((d >> (7-i)) & 0x1);
        spi_mosi(buf);
        spi_clk(0);
        spi_clk(1);
        spi_clk(0);
    }
}

/*!
    \brief      read the touch AD value
    \param[in]  None
    \param[out] none
    \retval     the value of touch AD
*/
uint16_t touch_read(void)
{
    uint16_t buf ;
    uint8_t i ;
    
    buf=0;
    for( i = 0; i < 12; i++){
        buf = buf << 1 ;
        spi_clk(1);
        spi_clk(0);
        if(RESET != spi_miso()){
            buf = buf + 1 ;
        }
    }
    return( buf );
}

/*!
    \brief      read the touch pen interrupt request signal
    \param[in]  none
    \param[out] none
    \retval     the status of touch pen: SET or RESET
      \arg        SET: touch pen is inactive
      \arg        RESET: touch pen is active
*/
FlagStatus touch_pen_irq(void)
{
    return TOUCH_PEN_INT_READ();
}

/*!
    \brief      get the AD sample value of touch location at X coordinate
    \param[in]  none
    \param[out] none
    \retval     channel X+ AD sample value
*/
uint16_t touch_ad_x_get(void)
{
    if (RESET != touch_pen_irq()){
        /* touch pen is inactive */
        return 0;
    }
    touch_start();
    touch_write(0x00);
    touch_write(CH_X);
    return (touch_read());
}

/*!
    \brief      get the AD sample value of touch location at Y coordinate
    \param[in]  none
    \param[out] none
    \retval     channel Y+ AD sample value
*/
uint16_t touch_ad_y_get(void)
{
    if (RESET != touch_pen_irq()){
        /* touch pen is inactive */
        return 0;
    }
    touch_start();
    touch_write(0x00);
    touch_write(CH_Y);
    return (touch_read());
}

/*!
    \brief      get channel X+ AD average sample value
    \param[in]  none
    \param[out] none
    \retval     channel X+ AD average sample value
*/
uint16_t touch_average_ad_x_get(void)
{
    uint8_t i;
    uint16_t temp=0;
    for (i=0;i<8;i++){
        temp+=touch_ad_x_get();
        spi_delay(1000);
    }
    temp>>=3;
    
    return temp;
}

/*!
    \brief      get channel Y+ AD average sample value
    \param[in]  none
    \param[out] none
    \retval     channel Y+ AD average sample value
*/
uint16_t touch_average_ad_y_get(void)
{
    uint8_t i;
    uint16_t temp=0;
    for (i=0;i<8;i++){
        temp+=touch_ad_y_get();
        spi_delay(1000);
    }
    temp>>=3;

    return temp;
}

/*!
    \brief      get X coordinate value of touch point on LCD screen
    \param[in]  adx : channel X+ AD average sample value
    \param[out] none
    \retval     X coordinate value of touch point
*/
uint16_t touch_coordinate_x_get(uint16_t adx)
{
    uint16_t sx = 0;
    uint32_t
    r = adx - AD_Left;
    r *= LCD_X - 1;
    sx =  r / (AD_Right - AD_Left);
    if (sx <= 0 || sx > LCD_X)
        return 0;
    return sx;
}

/*!
    \brief      get Y coordinate value of touch point on LCD screen
    \param[in]  ady : channel Y+ AD average sample value
    \param[out] none
    \retval     Y coordinate value of touch point
*/
uint16_t touch_coordinate_y_get(uint16_t ady)
{
    uint16_t sy = 0;
    uint32_t
    r = ady - AD_Top;
    r *= LCD_Y - 1;
    sy =  r / (AD_Bottom - AD_Top);  
    if (sy <= 0 || sy > LCD_Y)
        return 0;
    return sy;  
}

/*!
    \brief      get a value (X or Y) for several times. Order these values, 
                remove the lowest and highest and obtain the average value
    \param[in]  channel_select: select channel X or Y
      \arg        CH_X: channel X
      \arg        CH_Y: channel Y
    \param[out] none
    \retval     a value(X or Y) of touch point
*/
uint16_t touch_data_filter(uint8_t channel_select)
{
    uint16_t i=0, j=0; 
    uint16_t buf[FILTER_READ_TIMES]; 
    uint16_t sum=0; 
    uint16_t temp=0;
    /* Read data in FILTER_READ_TIMES times */
    for(i=0; i < FILTER_READ_TIMES; i++){
        if (CH_X == channel_select){
            buf[i] = touch_ad_x_get();
        }else{   /* CH_Y == channel_select */
            buf[i] = touch_ad_y_get();
        }
    }
    /* Sort in ascending sequence */
    for(i = 0; i < FILTER_READ_TIMES - 1; i++){
        for(j = i + 1; j < FILTER_READ_TIMES; j++){
            if(buf[i] > buf[j]){
                temp = buf[i]; 
                buf[i] = buf[j]; 
                buf[j] = temp;
            }
        }
    }
    sum = 0;
    for(i = FILTER_LOST_VAL; i < FILTER_READ_TIMES - FILTER_LOST_VAL; i++){
        sum += buf[i];
    }
    temp = sum / (FILTER_READ_TIMES - 2 * FILTER_LOST_VAL);
    
    return temp;
}

/*!
    \brief      get the AD sample value of touch location. 
                get the sample value for several times,order these values,remove the lowest and highest and obtain the average value
    \param[in]  channel_select: select channel X or Y
    \param[out] none
      \arg        ad_x: channel X AD sample value
      \arg        ad_y: channel Y AD sample value
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus touch_ad_xy_get(uint16_t *ad_x, uint16_t *ad_y)
{
    uint16_t ad_x1=0, ad_y1=0, ad_x2=0, ad_y2=0; 

    ad_x1 = touch_data_filter(CH_X); 
    ad_y1 = touch_data_filter(CH_Y); 
    ad_x2 = touch_data_filter(CH_X); 
    ad_y2 = touch_data_filter(CH_Y);
    
    if((abs(ad_x1 - ad_x2) > AD_ERR_RANGE) || (abs(ad_y1 - ad_y2) > AD_ERR_RANGE)){
        return ERROR;
    }
    *ad_x = (ad_x1 + ad_x2) / 2; 
    *ad_y = (ad_y1 + ad_y2) / 2;
   
    return SUCCESS;
}

/*!
    \brief      detect the touch event
    \param[in]  none
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus touch_scan(void)
{
    uint8_t invalid_count = 0;
    if (RESET == touch_pen_irq()){ /* touch pen is active */
        while((SUCCESS != touch_ad_xy_get(&touch_ad_x, &touch_ad_y))&& (invalid_count < 20)){
            invalid_count++; 
        }
        
        if(invalid_count >= 20){ 
            touch_ad_x = 0; 
            touch_ad_y = 0;
            return ERROR;
        }
    }else{ 
        touch_ad_x = 0; 
        touch_ad_y = 0;
        return ERROR;
    }
    return SUCCESS;
}
