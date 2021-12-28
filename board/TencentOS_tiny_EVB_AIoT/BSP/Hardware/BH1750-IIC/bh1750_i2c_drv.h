#ifndef	_BH1750_I2C_DRV_H_
#define	_BH1750_I2C_DRV_H_

//#define USE_ST_HAL
#define USE_NXP_FSL

#if defined(USE_ST_HAL)

#include "stm32l4xx_hal.h"

// address(7 bit) + read or write(1 bit)
#define	BH1750_ADDR_WRITE	0x46
#define	BH1750_ADDR_READ	0x47
#define BH1750_I2C_HANDLE   hi2c1

#elif defined(USE_NXP_FSL)

#include "fsl_lpi2c.h"

#define	BH1750_DEVICE_ADDR_7BIT     0x23
#define BH1750_I2C_MASTER           LPI2C1
/* Select USB1 PLL (480 MHz) as master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_SELECT (0U)
/* Clock divider for master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_DIVIDER (5U)
/* Get frequency of lpi2c clock */
#define LPI2C_CLOCK_FREQUENCY ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (LPI2C_CLOCK_SOURCE_DIVIDER + 1U))
#define LPI2C_MASTER_CLOCK_FREQUENCY LPI2C_CLOCK_FREQUENCY
#define LPI2C_BAUDRATE 100000U

#endif /* USE_ST_HAL or USE_NXP_FSL */

typedef enum
{
    POWER_OFF_CMD	=	0x00,	// Power off
    POWER_ON_CMD	=	0x01,	// Power on
    RESET_REGISTER	=	0x07,	// Reset digital register
    CONT_H_MODE		=	0x10,	// Continuous high resolution mode, measurement time 120ms
    CONT_H_MODE2	=	0x11,	// Continuous high resolution mode2, measurement time 120ms
    CONT_L_MODE		=	0x13,	// Continuous low resolution mode, measurement time 16ms
    ONCE_H_MODE		=	0x20,	// Once high resolution mode, measurement time 120ms
    ONCE_H_MODE2	=	0x21,	// Once high resolution mode2, measurement time 120ms
    ONCE_L_MODE		=	0x23	// Once low resolution mode2, measurement time 120ms
} bh1750_mode_t;

int bh1750_init(void);
int bh1750_start(bh1750_mode_t mode);
int bh1750_read_lux(uint16_t *lux);

#endif
