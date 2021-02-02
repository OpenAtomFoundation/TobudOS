#include "cmsis_os.h"
#include "lcd_spi2_drv.h"


void delay_ms(int ms)
{
	tos_task_delay(ms);
}

//Transfer GuiLite 32 bits color to your LCD color
#define GL_RGB_32_to_16(rgb) (((((unsigned int)(rgb)) & 0xFF) >> 3) | ((((unsigned int)(rgb)) & 0xFC00) >> 5) | ((((unsigned int)(rgb)) & 0xF80000) >> 8))
//Encapsulate your LCD driver:
void gfx_draw_pixel(int x, int y, unsigned int rgb)
{
	//LCD_Fast_DrawPoint(x, y, GL_RGB_32_to_16(rgb));
	LCD_Draw_ColorPoint(x, y, GL_RGB_32_to_16(rgb));
}
//Implement it, if you have more fast solution than drawing pixels one by one.
//void gfx_fill_rect(int x0, int y0, int x1, int y1, unsigned int rgb){}

//UI entry
struct EXTERNAL_GFX_OP
{
	void (*draw_pixel)(int x, int y, unsigned int rgb);
	void (*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
} my_gfx_op;
extern void startHelloStar(void* phy_fb, int width, int height, int color_bytes, struct EXTERNAL_GFX_OP* gfx_op);

void application_entry(void *arg)
{
		printf("This is GuiLite Demo!\r\n");
		my_gfx_op.draw_pixel = gfx_draw_pixel;
		my_gfx_op.fill_rect = NULL;//gfx_fill_rect;
		startHelloStar(NULL, 240, 240, 2, &my_gfx_op);
    
}


