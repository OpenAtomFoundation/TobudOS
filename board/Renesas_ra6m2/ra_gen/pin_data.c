/* generated pin source file - do not edit */
#include "bsp_api.h"
#include "r_ioport_api.h"
const ioport_pin_cfg_t g_bsp_pin_cfg_data[] = {
	{
		.pin = BSP_IO_PORT_01_PIN_00,
		.pin_cfg = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_SCI0_2_4_6_8),
	},
	{
		.pin = BSP_IO_PORT_01_PIN_01,
		.pin_cfg = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_SCI0_2_4_6_8),
	},
	{
		.pin = BSP_IO_PORT_01_PIN_08,
		.pin_cfg = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_DEBUG),
	},
	{
		.pin = BSP_IO_PORT_01_PIN_09,
		.pin_cfg = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_DEBUG),
	},
	{
		.pin = BSP_IO_PORT_01_PIN_10,
		.pin_cfg = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_DEBUG),
	},
	{
		.pin = BSP_IO_PORT_03_PIN_00,
		.pin_cfg = ((uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_DEBUG),
	},
};
const ioport_cfg_t g_bsp_pin_cfg = {
	.number_of_pins = sizeof(g_bsp_pin_cfg_data)/sizeof(ioport_pin_cfg_t),
	.p_pin_cfg_data = &g_bsp_pin_cfg_data[0],
};
