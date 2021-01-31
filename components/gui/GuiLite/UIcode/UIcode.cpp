#define GUILITE_ON  //Do not define this macro once more!!!
#include "GuiLite.h"
#include <stdlib.h>

#define UI_WIDTH 	240
#define UI_HEIGHT 240

static c_surface* s_surface;
static c_display* s_display;

class c_star {
public:
	c_star(){
		initialize();
	}
	void initialize() {
		m_x = m_start_x = rand() % UI_WIDTH;
		m_y = m_start_y = rand() % UI_HEIGHT;
		m_size = 1;
		m_x_factor = UI_WIDTH;
		m_y_factor = UI_HEIGHT;
		m_size_factor = 1;
	}
	void move() {
		s_surface->fill_rect(m_x, m_y, m_x + m_size - 1, m_y + m_size - 1, 0, Z_ORDER_LEVEL_0);//clear star footprint

		m_x_factor -= 6;
		m_y_factor -= 6;
		m_size += m_size / 20;
		if (m_x_factor < 1 || m_y_factor < 1)
		{
			return initialize();
		}
		if (m_start_x > (UI_WIDTH / 2) && m_start_y > (UI_HEIGHT / 2))
		{
			m_x = (UI_WIDTH / 2) + (UI_WIDTH * (m_start_x - (UI_WIDTH / 2)) / m_x_factor);
			m_y = (UI_HEIGHT / 2) + (UI_HEIGHT * (m_start_y - (UI_HEIGHT / 2)) / m_y_factor);
		}
		else if (m_start_x <= (UI_WIDTH / 2) && m_start_y > (UI_HEIGHT / 2))
		{
			m_x = (UI_WIDTH / 2) - (UI_WIDTH * ((UI_WIDTH / 2) - m_start_x) / m_x_factor);
			m_y = (UI_HEIGHT / 2) + (UI_HEIGHT * (m_start_y - (UI_HEIGHT / 2)) / m_y_factor);
		}
		else if (m_start_x > (UI_WIDTH / 2) && m_start_y <= (UI_HEIGHT / 2))
		{
			m_x = (UI_WIDTH / 2) + (UI_WIDTH * (m_start_x - (UI_WIDTH / 2)) / m_x_factor);
			m_y = (UI_HEIGHT / 2) - (UI_HEIGHT * ((UI_HEIGHT / 2) - m_start_y) / m_y_factor);
		}
		else if (m_start_x <= (UI_WIDTH / 2) && m_start_y <= (UI_HEIGHT / 2))
		{
			m_x = (UI_WIDTH / 2) - (UI_WIDTH * ((UI_WIDTH / 2) - m_start_x) / m_x_factor);
			m_y = (UI_HEIGHT / 2) - (UI_HEIGHT * ((UI_HEIGHT / 2) - m_start_y) / m_y_factor);
		}

		if (m_x < 0 || (m_x + m_size - 1) >= UI_WIDTH ||
			m_y < 0 || (m_y + m_size - 1) >= UI_HEIGHT)
		{
			return initialize();
		}
		s_surface->fill_rect(m_x, m_y, m_x + m_size - 1, m_y + m_size - 1, GL_RGB(255, 255, 255), Z_ORDER_LEVEL_0);//draw star
	}
	int m_start_x, m_start_y;
	float m_x, m_y, m_x_factor, m_y_factor, m_size_factor, m_size;
};

//////////////////////// start UI ////////////////////////

c_star stars[100];
void create_ui(void* phy_fb, int screen_width, int screen_height, int color_bytes, struct EXTERNAL_GFX_OP* gfx_op) {
	c_display display = c_display(phy_fb, screen_width, screen_height, UI_WIDTH, UI_HEIGHT, color_bytes, 1, gfx_op);
	s_display = &display;
	s_surface = display.alloc_surface(Z_ORDER_LEVEL_0);
	s_surface->set_active(true);

	s_surface->fill_rect(0, 0, UI_WIDTH - 1, UI_HEIGHT - 1, 0, Z_ORDER_LEVEL_0);

	while(1) {
		for (int i = 0; i < sizeof(stars)/sizeof(c_star); i++) {
			stars[i].move();
		}
		thread_sleep(50);
	}
}

//////////////////////// interface for all platform ////////////////////////
extern "C" void startHelloStar(void* phy_fb, int width, int height, int color_bytes, struct EXTERNAL_GFX_OP* gfx_op) {
	create_ui(phy_fb, width, height, color_bytes, gfx_op);
}

void* getUiOfHelloStar(int* width, int* height, bool force_update)
{
	if (s_display)
	{
		return s_display->get_updated_fb(width, height, force_update);
	}
	return NULL;
}
