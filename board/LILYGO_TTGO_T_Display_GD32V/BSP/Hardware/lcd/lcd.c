#include "lcd.h"
#include "lcdfont.h"

#define ST7789_SLPOUT       0x11
#define ST7789_NORON        0x13
#define ST7789_MADCTL       0x36      // Memory data access control
#define TFT_MAD_RGB         0x08
#define ST7789_COLMOD       0x3A
#define ST7789_PORCTRL      0xB2      // Porch control
#define ST7789_GCTRL        0xB7      // Gate control
#define ST7789_VCOMS        0xBB      // VCOMS setting
#define ST7789_LCMCTRL      0xC0      // LCM control
#define ST7789_VDVVRHEN     0xC2      // VDV and VRH command enable
#define ST7789_VRHS         0xC3      // VRH set
#define ST7789_VDVSET       0xC4      // VDV setting
#define ST7789_FRCTR2       0xC6      // FR Control 2
#define ST7789_PWCTRL1      0xD0      // Power control 1
#define ST7789_PVGAMCTRL    0xE0      // Positive voltage gamma control
#define ST7789_NVGAMCTRL    0xE1      // Negative voltage gamma control
#define ST7789_INVON        0x21
#define ST7789_CASET        0x2A
#define ST7789_RASET        0x2B
#define ST7789_RAMWR        0x2C
#define ST7789_DISPOFF      0x28
#define ST7789_DISPON       0x29

uint16_t bgcolor = BLACK;   //背景色
uint8_t max_width = 0;
uint8_t max_height= 0;
lcd_display_mode_t display_mode = LCD_DISPMODE_VERTICAL;

void LCD_Writ_Bus(uint8_t dat)
{
#if USE_HARDWARE_SPI
    LCD_CS_Clr();

    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
        spi_i2s_data_transmit(SPI0, dat);
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));
        spi_i2s_data_receive(SPI0);

    LCD_CS_Set();
#endif

#if USE_HARDWARE_DMA_SPI
    spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
#endif

#if USE_SOFTWARE_SPI
    uint8_t i;
    LCD_CS_Clr();
    for(i=0;i<8;i++)
    {
        LCD_SCLK_Clr();
        if(dat&0x80)
           LCD_SDIN_Set();
        else
           LCD_SDIN_Clr();
        LCD_SCLK_Set();
        dat<<=1;
    }
  LCD_CS_Set();
#endif
}


void LCD_WR_DATA8(uint8_t dat)
{
    LCD_DC_Set();
    LCD_Writ_Bus(dat);
}


void LCD_WR_DATA(uint16_t dat)
{
    LCD_DC_Set();
    LCD_Writ_Bus(dat>>8);
    LCD_Writ_Bus(dat);
}


void LCD_WR_REG(uint8_t dat)
{
    LCD_DC_Clr();
    LCD_Writ_Bus(dat);
}


void LCD_Address_Set(uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
    if(width == 0 || height == 0) {
        return ;
    }

    if(display_mode == LCD_DISPMODE_HORIZONTAL) {
        x += 40;
        y += 53;
    }

    if(display_mode == LCD_DISPMODE_HORIZONTAL_MIRROR) {
        x += 40;
        y += 52;
    }

    if(display_mode == LCD_DISPMODE_VERTICAL) {
        x += 52;
        y += 40;
    }
    if(display_mode == LCD_DISPMODE_VERTICAL_MIRROR) {
        x += 53;
        y += 40;
    }

    // column
    LCD_WR_REG(0x2a);
    LCD_WR_DATA(x);
    LCD_WR_DATA(x+width-1);

    // row
    LCD_WR_REG(0x2b);
    LCD_WR_DATA(y);
    LCD_WR_DATA(y+height-1);

    // store
    LCD_WR_REG(0x2c);
}

#if USE_HARDWARE_DMA_SPI
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    /* SPI0 transmit dma config:DMA0,DMA_CH2 */
    dma_deinit(DMA0, DMA_CH2);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI0);
    dma_init_struct.memory_addr  = (uint32_t)image;
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_LOW;
    dma_init_struct.number       = ((max_width)*(max_height)*2);
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA0, DMA_CH2, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH2);
    dma_memory_to_memory_disable(DMA0, DMA_CH2);
}
#endif

#if USE_HARDWARE_SPI
void spi_config(void)
{
    spi_parameter_struct spi_init_struct;
    /* deinitilize SPI and the parameters */
    LCD_CS_Set();
    spi_struct_para_init(&spi_init_struct);

    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_8;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    spi_crc_polynomial_set(SPI0,7);
    spi_enable(SPI0);
}
#endif

void delay_1ms(uint32_t count)
{
	uint64_t start_mtime, delta_mtime;

	/* don't start measuruing until we see an mtime tick */
	uint64_t tmp = get_timer_value();

	do{
		start_mtime = get_timer_value();
	}while(start_mtime == tmp);

	do{
		delta_mtime = get_timer_value() - start_mtime;
	}while(delta_mtime <(SystemCoreClock/4000.0 *count));
}


void LCD_On() {
    LCD_WR_REG(0x29);
}

lcd_display_mode_t lcd_display_mode = LCD_DISPMODE_HORIZONTAL;
lcd_display_mode_t LCD_GetDisplayMode() {
	return lcd_display_mode;
}


void LCD_SetDisplayMode(lcd_display_mode_t m) {
	lcd_display_mode = m;
    uint8_t d = 0x00;
    const uint8_t M = 240;
    const uint8_t N = 135;
    switch(m) {
    case LCD_DISPMODE_HORIZONTAL:
        d = 0x60;
        max_width   = M;
        max_height  = N;
        break;
    case LCD_DISPMODE_HORIZONTAL_MIRROR:
        d = 0xA0;
        max_width   = M;
        max_height  = N;
        break;
    case LCD_DISPMODE_VERTICAL:
        d = 0x00;
        max_width   = N;
        max_height  = M;
        break;
    case LCD_DISPMODE_VERTICAL_MIRROR:
        d = 0xC0;
        max_width   = N;
        max_height  = M;
        break;
    default:
        return;
    };

    LCD_WR_REG(0x36);
    LCD_WR_DATA8(d | 0x08);
    display_mode = m;
}


void LCD_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

#if USE_HARDWARE_SPI
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_SPI0);
    /* SPI0 GPIO config: NSS/PA4, SCK/PA5, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2 | GPIO_PIN_10);
    gpio_bit_set(GPIOB, GPIO_PIN_10);

    spi_config();
#endif

#if USE_HARDWARE_DMA_SPI
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_SPI0);

    /* SPI0 GPIO config: NSS/PA4, SCK/PA5, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
    /* SPI0 GPIO config: MISO/PA6 */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    dma_config();

    dma_channel_enable(DMA0,DMA_CH2);
#endif

#if USE_SOFTWARE_SPI
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    gpio_bit_reset(GPIOA, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_bit_reset(GPIOB, GPIO_PIN_2);
#endif

    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);
    gpio_bit_reset(GPIOB, GPIO_PIN_0 | GPIO_PIN_1);

    LCD_RST_Clr();
    delay_1ms(200);
    LCD_RST_Set();
    delay_1ms(20);

    LCD_WR_REG(ST7789_SLPOUT);
    delay_1ms(100);

    LCD_WR_REG(ST7789_NORON);    // Normal display mode on

    // display inversion on
    LCD_WR_REG(0x21);

    LCD_WR_REG(ST7789_MADCTL);
    //LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(TFT_MAD_RGB);

    // JLX240 display datasheet
    LCD_WR_REG(0xB6);
    LCD_WR_DATA8(0x0A);
    LCD_WR_DATA8(0x82);

    LCD_WR_REG(ST7789_COLMOD);
    LCD_WR_DATA8(0x55);
    delay_1ms(10);

    //--------------------------------ST7789V Frame rate setting----------------------------------//
    LCD_WR_REG(ST7789_PORCTRL);
    LCD_WR_DATA8(0x0c);
    LCD_WR_DATA8(0x0c);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(ST7789_GCTRL);      // Voltages: VGH / VGL
    LCD_WR_DATA8(0x35);


    //---------------------------------ST7789V Power setting--------------------------------------//
    LCD_WR_REG(ST7789_VCOMS);
    LCD_WR_DATA8(0x28);       // JLX240 display datasheet

    LCD_WR_REG(ST7789_LCMCTRL);
    LCD_WR_DATA8(0x0C);

    LCD_WR_REG(ST7789_VDVVRHEN);
    LCD_WR_DATA8(0x01);
    LCD_WR_DATA8(0xFF);

    LCD_WR_REG(ST7789_VRHS);       // voltage VRHS
    LCD_WR_DATA8(0x10);

    LCD_WR_REG(ST7789_VDVSET);
    LCD_WR_DATA8(0x20);

    LCD_WR_REG(ST7789_FRCTR2);
    LCD_WR_DATA8(0x0f);

    LCD_WR_REG(ST7789_PWCTRL1);
    LCD_WR_DATA8(0xa4);
    LCD_WR_DATA8(0xa1);


    //--------------------------------ST7789V gamma setting---------------------------------------//
    LCD_WR_REG(ST7789_PVGAMCTRL);
    LCD_WR_DATA8(0xd0);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x02);
    LCD_WR_DATA8(0x07);
    LCD_WR_DATA8(0x0a);
    LCD_WR_DATA8(0x28);
    LCD_WR_DATA8(0x32);
    LCD_WR_DATA8(0x44);
    LCD_WR_DATA8(0x42);
    LCD_WR_DATA8(0x06);
    LCD_WR_DATA8(0x0e);
    LCD_WR_DATA8(0x12);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x17);

    LCD_WR_REG(ST7789_NVGAMCTRL);
    LCD_WR_DATA8(0xd0);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x02);
    LCD_WR_DATA8(0x07);
    LCD_WR_DATA8(0x0a);
    LCD_WR_DATA8(0x28);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x54);
    LCD_WR_DATA8(0x47);
    LCD_WR_DATA8(0x0e);
    LCD_WR_DATA8(0x1c);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x1b);
    LCD_WR_DATA8(0x1e);

    LCD_WR_REG(ST7789_INVON);

    LCD_WR_REG(ST7789_CASET);    // Column address set
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0xE5);    // 239

    LCD_WR_REG(ST7789_RASET);    // Row address set
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x01);
    LCD_WR_DATA8(0x3F);    // 319

    LCD_WR_REG(ST7789_LCMCTRL);
    LCD_WR_DATA8(0x0C);

    LCD_SetDisplayMode(LCD_DISPMODE_HORIZONTAL);

    LCD_On();
}



void LCD_Clear(uint16_t Color)
{
	bgcolor = Color;
    LCD_Address_Set(0, 0, max_width, max_height);
    for(uint16_t i=0; i<max_width; i++) {
        for (uint16_t j=0; j<max_height; j++) {
            LCD_WR_DATA(Color);
        }
    }
}

void LCD_ClearRect(uint16_t Color, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    LCD_Address_Set(x, y, width, height);
    for(uint16_t i=0; i<max_width; i++) {
        for (uint16_t j=0; j<max_height; j++) {
            LCD_WR_DATA(Color);
        }
    }
}

void LCD_ShowChinese(uint16_t x, uint16_t y, uint8_t index, uint16_t color)
{
    LCD_ShowChineseWithFonts(x, y, Hzk, index, color);
}

void LCD_ShowChineseWithFonts(uint16_t x, uint16_t y, const char fonts[][16], uint8_t index, uint16_t color)
{
    const uint8_t *p = (uint8_t*) &(fonts[index*2][0]);

    //[数据排列]:从左到右从上到下 [取模方式]:纵向8点下高位 [正负反色]:否
    LCD_Address_Set(x, y, 16, 16);

    for(uint8_t i=0; i<8; i++) {
        for(uint8_t j=0; j<16; j++) {
            LCD_WR_DATA((p[j] & (1<<i)) == 0 ? bgcolor : color);
        }
    }

    for(uint8_t i=0; i<8; i++) {
        for(uint8_t j=16; j<32; j++) {
            LCD_WR_DATA((p[j] & (1<<i)) == 0 ? bgcolor : color);
        }
    }
}


void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
    LCD_Address_Set(x, y, 1, 1);
    LCD_WR_DATA(color);
}



void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    LCD_Address_Set(xsta, ysta, xend-xsta, yend-ysta);
    for(uint8_t i=ysta; i<=yend; i++) {
        for(uint8_t j=xsta;j<=xend;j++) {
            LCD_WR_DATA(color);
        }
    }
}

int abs(int n) {
    return n >= 0 ? n : -n;
}

void LCD_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    int16_t dx = x2 - x1;
    int16_t dy = y2 - y1;
    int16_t ux = ((dx > 0) << 1) - 1;//x的增量方向，取或-1
    int16_t uy = ((dy > 0) << 1) - 1;//y的增量方向，取或-1
    int16_t x = x1, y = y1, eps;//eps为累加误差

    eps = 0;dx = abs(dx); dy = abs(dy);
    if (dx > dy) {
        for (x = x1; x != x2; x += ux) {
            LCD_DrawPoint(x, y, color);
            eps += dy;
            if ((eps << 1) >= dx) {
                y += uy; eps -= dx;
            }
        }
    } else {
        for (y = y1; y != y2; y += uy) {
            LCD_DrawPoint(x, y, color);
            eps += dx;
            if ((eps << 1) >= dy) {
                x += ux; eps -= dy;
            }
        }
    }
}

void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    LCD_DrawLine(x1,y1,x2,y1,color);
    LCD_DrawLine(x1,y1,x1,y2,color);
    LCD_DrawLine(x1,y2,x2,y2,color);
    LCD_DrawLine(x2,y1,x2,y2,color);
}


void LCD_DrawCircle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color)
{
    int a,b;
    a=0;b=r;
    while(a<=b)
    {
        LCD_DrawPoint(x0-b,y0-a,color);             //3
        LCD_DrawPoint(x0+b,y0-a,color);             //0
        LCD_DrawPoint(x0-a,y0+b,color);             //1
        LCD_DrawPoint(x0-a,y0-b,color);             //2
        LCD_DrawPoint(x0+b,y0+a,color);             //4
        LCD_DrawPoint(x0+a,y0-b,color);             //5
        LCD_DrawPoint(x0+a,y0+b,color);             //6
        LCD_DrawPoint(x0-b,y0+a,color);             //7
        a++;
        if((a*a+b*b)>(r*r))//判断要画的点是否过远
        {
            b--;
        }
    }
}


void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t chr, uint16_t color)
{
    if(x > max_width || y > max_height) {
        return;
    }

    const uint8_t *p = F8X16 + (chr - ' ')*16;

    LCD_Address_Set(x, y, 8, 16);      //设置光标位置

    for(uint8_t i=0; i<8; i++) {
        for(uint8_t j=0; j<8; j++) {
            LCD_WR_DATA((p[0+j] & (1<<i)) == 0 ? bgcolor : color);
        }
    }

    for(uint8_t i=0; i<8; i++) {
        for(uint8_t j=0; j<8; j++) {
            LCD_WR_DATA((p[8+j] & (1<<i)) == 0 ? bgcolor : color);
        }
    }
}


void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t color)
{
    while(*p) {
        LCD_ShowChar(x,y,*p,color);
        x += 8;
        p++;
    }
}



void LCD_ShowNum(uint16_t x, uint16_t y, uint16_t n, uint16_t color)
{
    const uint8_t max_len = 5; // 65536 max length 5
    uint8_t buf[max_len];

    int pos = max_len;
    do {
        pos--;
        buf[pos] = n % 10 + '0';
        n /= 10;
    } while(n > 0);

    for(uint8_t i=0; pos < max_len; i++, pos++) {
        LCD_ShowChar(x+i*8, y, buf[pos], color);
    }
}
