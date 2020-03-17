/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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
#ifndef H_MAX9880A
#define H_MAX9880A
// MAX9800A (Low-Power, High-Performance, Dual I²S, Stereo Audio Codec) is on EMSDP board
#define I2C_DEVICE_ADDRESS_MAX9880A       (0x10)

typedef enum {
	/**
	 * Use external MCLK of 13 MHz
	 * S1 pins are mapped on DAI1
	 * S2 pins are mapped on DAI2
	 */
	MAX9880A_I2SIN_LINEOUT_13MHZ = 0,               // Playback
	MAX9880A_LINEIN_I2SOUT_13MHZ,                   // Recording

	/**
	 * Use no external MCLK, but route Xtal to clk-prescaler
	 * S1 pins are mapped on DAI1
	 * S2 pins are mapped on DAI2
	 */
	MAX9880A_I2SIN_LINEOUT_12DOT288MHZ,             // Playback
	MAX9880A_LINEIN_I2SOUT_24DOT567MHZ,             // Recording

	/**
	 * Use external MCLK of 24.567 MHz
	 * S1 pins are mapped on DAI1
	 * S2 pins are mapped on DAI2
	 */
	MAX9880A_I2SIN_LINEOUT_24DOT567MHZ,         // Playback
	MAX9880A_LINEIN_I2SOUT_24DOT567MHZ,         // Recording

	/**
	 * !!!! Use these settings for HSDK-IC on BOARD !!!!
	 *
	 * Mapping is now different
	 * S1 pins are mapped on DAI2
	 * S2 pins are mapped on DAI1
	 *
	 * Use external MCLK of 24.567 MHz
	 * Use DAI2 (S1) for playback (I2S-TX), Use DAI1 (S2) for recording (I2S-RX)
	 */
	MAX9880A_DAI2_I2SIN_LINEOUT_24DOT567MHZ,        // Playback
	MAX9880A_LINEIN_DAI1_I2SOUT_24DOT567MHZ,        // Recording
	MAX9880A_24DOT567MHZ,                           // PlayBack and Recording (Regression)

} max9880a_config_t;

/* temperature sensor object type*/
typedef struct {
	uint32_t i2c_id;
	uint32_t slvaddr;
} MAX9880A_DEF, *MAX9880A_DEF_PTR;

#define MAX9880A_DEFINE(NAME, I2C_ID, SLAVE_ADDRESS) \
	MAX9880A_DEF __ ## NAME = {		     \
		.i2c_id = I2C_ID,		     \
		.slvaddr = SLAVE_ADDRESS,	     \
	};					     \
	MAX9880A_DEF_PTR NAME = &__ ## NAME

extern uint32_t max9880a_dump_reg(MAX9880A_DEF_PTR obj);
extern uint32_t max9880a_config(MAX9880A_DEF_PTR obj, max9880a_config_t config);
extern uint32_t max9880a_init(MAX9880A_DEF_PTR obj);
extern uint32_t max9880a_deinit(MAX9880A_DEF_PTR obj);

#endif /* H_MAX9880A */