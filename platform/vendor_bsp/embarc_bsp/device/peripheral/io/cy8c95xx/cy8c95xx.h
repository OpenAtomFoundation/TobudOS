/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
   --------------------------------------------- */
#ifndef H_CY8C95XX
#define H_CY8C95XX

#define CY8C95XX_I2C_TIMEOUT            (5000000)

// CY8C95xx Series Interface is a multi-port IO expander using I2C bus, on HSDK board
typedef enum {
	CY8C95XX_DEV_IO = 0,
	CY8C95XX_DEV_EEP
} CY8C95XX_DEV_T;

typedef enum {
	CY8C95XX_PORT_0 = 0,
	CY8C95XX_PORT_1,
	CY8C95XX_PORT_2,
	CY8C95XX_PORT_NONE
} CY8C95XX_PORT_T;

typedef enum {
	CY8C95XX_PIN_0 = 0,
	CY8C95XX_PIN_1,
	CY8C95XX_PIN_2,
	CY8C95XX_PIN_3,
	CY8C95XX_PIN_4,
	CY8C95XX_PIN_5,
	CY8C95XX_PIN_6,
	CY8C95XX_PIN_7,
	CY8C95XX_PIN_NONE
} CY8C95XX_PIN_T;

typedef enum {
	CY8C95XX_PWM_0 = 0,
	CY8C95XX_PWM_1,
	CY8C95XX_PWM_2,
	CY8C95XX_PWM_3,
	CY8C95XX_PWM_NONE
} CY8C95XX_PWM_T;

typedef enum {
	CY8C95XX_PWMFREQ_100HZ = 0,
	CY8C95XX_PWMFREQ_300HZ,
	CY8C95XX_PWMFREQ_500HZ,
	CY8C95XX_PWMFREQ_700HZ,
	CY8C95XX_PWMFREQ_900HZ,
	CY8C95XX_PWMFREQ_NONE
} CY8C95XX_PWMFREQ_T;

typedef enum {
	CY8C95XX_ENABLE_WDE = 0,
	CY8C95XX_ENABLE_EEE,
	CY8C95XX_ENABLE_EERO,
	CY8C95XX_ENABLE_NONE
} CY8C95XX_ENABLE_T;

typedef struct {
	uint32_t i2c_id;
	uint32_t slvaddr_io;
	uint32_t slvaddr_eep;
} CY8C95XX_DEF, *CY8C95XX_DEF_PTR;

#define CY8C95XX_INPUT_PORT0_REG    0x00
#define CY8C95XX_INPUT_PORT1_REG    0x01
#define CY8C95XX_INPUT_PORT2_REG    0x02
#define CY8C95XX_INPUT_PORT3_REG    0x03
#define CY8C95XX_INPUT_PORT4_REG    0x04
#define CY8C95XX_INPUT_PORT5_REG    0x05
#define CY8C95XX_INPUT_PORT6_REG    0x06
#define CY8C95XX_INPUT_PORT7_REG    0x07

#define CY8C95XX_OUTPUT_PORT0_REG   0x08
#define CY8C95XX_OUTPUT_PORT1_REG   0x09
#define CY8C95XX_OUTPUT_PORT2_REG   0x0A
#define CY8C95XX_OUTPUT_PORT3_REG   0x0B
#define CY8C95XX_OUTPUT_PORT4_REG   0x0C
#define CY8C95XX_OUTPUT_PORT5_REG   0x0D
#define CY8C95XX_OUTPUT_PORT6_REG   0x0E
#define CY8C95XX_OUTPUT_PORT7_REG   0x0F

#define CY8C95XX_INTSTS_PORT0_REG   0x10
#define CY8C95XX_INTSTS_PORT1_REG   0x11
#define CY8C95XX_INTSTS_PORT2_REG   0x12
#define CY8C95XX_INTSTS_PORT3_REG   0x13
#define CY8C95XX_INTSTS_PORT4_REG   0x14
#define CY8C95XX_INTSTS_PORT5_REG   0x15
#define CY8C95XX_INTSTS_PORT6_REG   0x16
#define CY8C95XX_INTSTS_PORT7_REG   0x17

#define CY8C95XX_PORT_SELECT_REG    0x18
#define CY8C95XX_INTMSK_REG     0x19
#define CY8C95XX_PORT_SELECT_PWM    0x1A
#define CY8C95XX_PIN_DIRECTION_REG  0x1C
#define CY8C95XX_DRIVE_MODE_PU_REG  0x1D
#define CY8C95XX_DRIVE_MODE_PD_REG  0x1E
#define CY8C95XX_DRIVE_MODE_ODH_REG 0x1F
#define CY8C95XX_DRIVE_MODE_ODL_REG 0x20
#define CY8C95XX_DRIVE_MODE_S_REG   0x21
#define CY8C95XX_DRIVE_MODE_SS_REG  0x22
#define CY8C95XX_DRIVE_MODE_HZ_REG  0x23

#define CY8C95XX_PWM_SELECT     0x28
#define CY8C95XX_PWM_CONFIG     0x29
#define CY8C95XX_PWM_PERIOD     0x2A
#define CY8C95XX_PWM_PULSEWIDTH     0x2B
#define CY8C95XX_PWM_DIVIDER        0x2C

#define CY8C95XX_ENABLE_REG     0x2D
#define CY8C95XX_DEVICE_ID_REG      0x2E
#define CY8C95XX_COMMAND_REG        0x30

extern int32_t cy8c95xx_readid(CY8C95XX_DEF_PTR obj, uint8_t *value);

extern int32_t cy8c95xx_writeport(CY8C95XX_DEF_PTR obj, uint8_t port, uint8_t value);

extern int32_t cy8c95xx_readport(CY8C95XX_DEF_PTR obj, uint8_t port, uint8_t *value);

extern int32_t cy8c95xx_reg_write(CY8C95XX_DEF_PTR obj, uint8_t dev, uint8_t regaddr, uint8_t *val, uint8_t len);

extern int32_t cy8c95xx_reg_read(CY8C95XX_DEF_PTR obj, uint8_t dev, uint8_t regaddr, uint8_t *val, uint8_t len);

#endif /* H_CY8C95XX */
