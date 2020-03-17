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
#include "embARC.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "board.h"
#include "max9880a.h"

#define DBG_LESS
#include "embARC_debug.h"

#define MAX9880A_CHECK_EXP(EXPR, ERROR_CODE)    CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

// Register names
typedef enum {
	MAX9880A_STATUS = 0x00,                         // RO 0x00
	MAX9880A_JACKSTATUS,                            // RO 0x01
	MAX9880A_AUXHIGH,                               // RO 0x02
	MAX9880A_AUXLOW,                                // RO 0x03
	MAX9880A_INTERRUPT_ENABLE,                      // RW 0x04
	MAX9880A_SYSTEM_CLOCK,                          // RW 0x05
	// DAI1-Clock Control
	MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_HIGH,       // RW 0x06
	MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_LOW,        // RW 0x07
	// DAI1-Configuration
	MAX9880A_DAI1_INTERFACE_MODE_A,                 // RW 0x08
	MAX9880A_DAI1_INTERFACE_MODE_B,                 // RW 0x09
	MAX9880A_DAI1_TIME_DIVISION_MUL,                // RW 0x0A
	// DAI2-Clock Control
	MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_HIGH,       // RW 0x0B
	MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_LOW,        // RW 0x0C
	// DAI2-Configuration
	MAX9880A_DAI2_INTERFACE_MODE_A,                 // RW 0x0D
	MAX9880A_DAI2_INTERFACE_MODE_B,                 // RW 0x0E
	MAX9880A_DAI2_TIME_DIVISION_MUL,                // RW 0x0F
	// Digital Mixers
	MAX9880A_DAC_LR_MIXER,                          // RW 0x10
	// Digital Filtering
	MAX9880A_CODEC_FILTERS,                         // RW 0x11
	// SPDM Outputs
	MAX9880A_SPDM_CONFIGURATIONS,                   // RW 0x12
	MAX9880A_SPDM_INPUT,                            // RW 0x13
	MAX9880A_REV_ID,                                // RO 0x14
	// LEVEL control
	MAX9880A_SIDETONE,                              // RW 0x15
	MAX9880A_STEREO_DAC_LEVEL,                      // RW 0x16
	MAX9880A_VOICE_DAC_LEVEL,                       // RW 0x17
	MAX9880A_LEFT_ADC_LEVEL,                        // RW 0x18
	MAX9880A_RIGHT_ADC_LEVEL,                       // RW 0x19
	MAX9880A_LEFT_LINE_INPUT_LEVEL,                 // RW 0x1A
	MAX9880A_RIGHT_LINE_INPUT_LEVEL,                // RW 0x1B
	MAX9880A_LEFT_VOLUME_CTRL,                      // RW 0x1C
	MAX9880A_RIGHT_VOLUME_CTRL,                     // RW 0x1D
	MAX9880A_LEFT_LINE_OUTPUT_LEVEL,                // RW 0x1E
	MAX9880A_RIGHT_LINE_OUTPUT_LEVEL,               // RW 0x1F
	MAX9880A_LEFT_MIC_GAIN,                         // RW 0x20
	MAX9880A_RIGHT_MIC_GAIN,                        // RW 0x21
	// Configuration
	MAX9880A_INPUT,                                 // RW 0x22
	MAX9880A_MICROPHONE,                            // RW 0x23
	MAX9880A_MODE,                                  // RW 0x24
	MAX9880A_JACK_DETECT,                           // RW 0x25
	// Power Management
	MAX9880A_ENABLE,                                // RW 0x26
	MAX9880A_SYSTEM_SHUTDOWN,                       // RW 0x27
	MAX9880A_REVISION_ID = 0xFF,                    // RO 0xFF
} max9880a_reg_t;

typedef struct max9880a_settings {
	max9880a_reg_t reg;
	uint8_t val;
} max9880a_settings_t;

/******************************************************************************/
/***********       MAX9880A Configs      **************************************/
/******************************************************************************/

/**
 * MAX9880A_I2SIN_LINEOUT_13MHZ
 *
 * Playback: I2S In -> LineOut, Stereo 16Bit I2S, Fs=16 Khz (LRclk=32Khz)
 *
 * Measured with scope:
 * => I2S clk  from FPGA-HSDK to MAX: 512 KHz, divider is 48. (24.567 / 48 = 512 KHz).
 * => I2S ws	from FPGA-HSDK to MAX: 16 KHz.
 * => I2S data from FPGA-HSDK to MAX: 16 bit
 *
 * => MCLK on MAX9880 board is not from FPGA-HSDK but a on board Chrystal of 13.00 MHz.
 */
const max9880a_settings_t settings_i2sin_lineout_13mhz[] =
{
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x04 },        // 0x27,
	{ MAX9880A_SYSTEM_CLOCK,                         0x10 },        // 0x05, MCLK between 10-20 Mhz (PCLK=MCLK)

	// For recording (S1 pins are mapped on DAI1)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_HIGH,      0x9E },        // 0x06, PLL=enabled, Ni=0x1E3F, DAI1 for LineIn -> I2S out, See table 6 datasheet (LRCLK=16 KHz PCLK=13 MHz)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_LOW,       0x3F },        // 0x07
	{ MAX9880A_DAI1_INTERFACE_MODE_A,                0x10 },        // 0x08, Slave mode, Delay Mode
	{ MAX9880A_DAI1_INTERFACE_MODE_B,                0x30 },        // 0x09, SEL1 = 0, Map S1 to DAI1
	// For playback (S2 pins are mapped on DAI2)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_HIGH,      0x9E },        // 0x0B, PLL=enabled, Ni=0x1E3F, DAI2 for I2S in -> LineOut, See table 6 datasheet (LRCLK=16 KHz PCLK=13 MHz)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_LOW,       0x3F },        // 0x0C
	{ MAX9880A_DAI2_INTERFACE_MODE_A,                0x10 },        // 0x0D, Slave mode, Delay Mode, 16BitI2S
	{ MAX9880A_DAI2_INTERFACE_MODE_B,                0x50 },        // 0x0E, SEL2 = 1, Map S2 to DAI2

	{ MAX9880A_DAC_LR_MIXER,                         0x21 },        // 0x10, DAi2 left channel, DAi2 right channel (audio out)
	{ MAX9880A_CODEC_FILTERS,                        0x80 },        // 0x11, select music filters

	{ MAX9880A_LEFT_ADC_LEVEL,                       0x0D },        // 0x18, Gain 0dB, Gain -10dB
	{ MAX9880A_RIGHT_ADC_LEVEL,                      0x0D },        // 0x19, Gain 0dB, Gain -10dB
	{ MAX9880A_LEFT_LINE_INPUT_LEVEL,                0x0C },        // 0x1A, Gain 0dB
	{ MAX9880A_RIGHT_LINE_INPUT_LEVEL,               0x0C },        // 0x1B, Gain 0dB

	{ MAX9880A_LEFT_VOLUME_CTRL,                     0x0C },        // 0x1C, Gain 0dB
	{ MAX9880A_RIGHT_VOLUME_CTRL,                    0x0C },        // 0x1D, Gain 0dB
	{ MAX9880A_LEFT_LINE_OUTPUT_LEVEL,               0x00 },        // 0x1E, unmuted, Gain 0dB
	{ MAX9880A_RIGHT_LINE_OUTPUT_LEVEL,              0x00 },        // 0x1F, unmuted, Gain 0dB

	{ MAX9880A_INPUT,                                0x00 },        // 0x22, No input selected
	{ MAX9880A_ENABLE,                               0x3C },        // 0x26, LineOut-Enable, LineIn-Disable, Left/Right DAC enabled
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x80 },        // 0x27, Power On, Mclk to clk prescaler, disable internal clk osc
};

#define SETTINGS_I2SIN_LINEOUT_13MHZ_LENGTH  (sizeof(settings_i2sin_lineout_13mhz) / sizeof(settings_i2sin_lineout_13mhz[0]))

/**
 * MAX9880A_LINEIN_I2SOUT_13MHZ
 *
 * Recording: LineIn -> I2S out, Stereo 16Bit I2S, Fs=16 Khz (LRclk=32Khz)
 *
 * Measured with scope:
 * => I2S clk from HSDK to MAX: 512 KHz, divider is 48. (24.567 / 48 = 512 KHz).
 * => I2S ws  from HSDK to MAX: 16 KHz.
 *
 * => MCLK on MAX9880 board is not from FPGA-HSDK but a on board Chrystal of 13.00 MHz.
 */
const max9880a_settings_t settings_linein_i2sout_13mhz[] =
{
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x04 },        // 0x27
	{ MAX9880A_SYSTEM_CLOCK,                         0x10 },        // 0x05, MCLK between 10-20 Mhz (PCLK=MCLK)

	// For recording (S1 pins are mapped on DAI1)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_HIGH,      0x9E },        // 0x06, PLL=enabled, Ni=0x1E3F, DAI1 for LineIn -> I2S out, See table 6 datasheet (LRCLK=16 KHz PCLK=13 MHz)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_LOW,       0x3F },        // 0x07
	{ MAX9880A_DAI1_INTERFACE_MODE_A,                0x10 },        // 0x08, Slave mode, Delay Mode
	{ MAX9880A_DAI1_INTERFACE_MODE_B,                0x30 },        // 0x09, SEL1 = 0, Map S1 to DAI1
	// For playback (S2 pins are mapped on DAI2)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_HIGH,      0x9E },        // 0x0B, PLL=enabled, Ni=0x1E3F, DAI2 for I2S in -> LineOut, See table 6 datasheet (LRCLK=16 KHz PCLK=13 MHz)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_LOW,       0x3F },        // 0x0C
	{ MAX9880A_DAI2_INTERFACE_MODE_A,                0x10 },        // 0x0D, Slave mode, Delay Mode, 16BitI2S
	{ MAX9880A_DAI2_INTERFACE_MODE_B,                0x50 },        // 0x0E, SEL2 = 1, Map S2 to DAI2

	{ MAX9880A_DAC_LR_MIXER,                         0x21 },        // 0x10, DAi2 left channel, DAi2 right channel (audio out)
	{ MAX9880A_CODEC_FILTERS,                        0x80 },        // 0x11, Select music filters

	{ MAX9880A_LEFT_ADC_LEVEL,                       0x0D },        // 0x18, Gain 0dB, Gain -10dB
	{ MAX9880A_RIGHT_ADC_LEVEL,                      0x0D },        // 0x19, Gain 0dB, Gain -10dB
	{ MAX9880A_LEFT_LINE_INPUT_LEVEL,                0x0C },        // 0x1A, Gain 0dB
	{ MAX9880A_RIGHT_LINE_INPUT_LEVEL,               0x0C },        // 0x1B, Gain 0dB

	{ MAX9880A_LEFT_VOLUME_CTRL,                     0x0C },        // 0x1C, Gain 0dB
	{ MAX9880A_RIGHT_VOLUME_CTRL,                    0x0C },        // 0x1D, Gain 0dB
	{ MAX9880A_LEFT_LINE_OUTPUT_LEVEL,               0x40 },        // 0x1E, muted
	{ MAX9880A_RIGHT_LINE_OUTPUT_LEVEL,              0x40 },        // 0x1F, muted

	{ MAX9880A_INPUT,                                0xA0 },        // 0x22, Left & Right Line input
	{ MAX9880A_ENABLE,                               0xC3 },        // 0x26, LineIn-Enable, LineOut-Disable, Left/Right ADC enabled
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x80 },        // 0x27, Power On, Mclk to clk prescaler, disable internal clk osc
};

#define SETTINGS_LINEIN_I2SOUT_13MHZ_LENGTH  (sizeof(settings_linein_i2sout_13mhz) / sizeof(settings_linein_i2sout_13mhz[0]))

/**
 * MAX9880A_I2SIN_LINEOUT_12DOT288MHZ
 *
 * Playback: I2S In -> LineOut, Stereo 16Bit I2S, Fs=16 Khz (LRclk=32Khz)
 *
 * Measured with scope:
 * => I2S clk  from FPGA-HSDK to MAX: 512 KHz, divider is 48. (24.567 / 48 = 512 KHz).
 * => I2S ws	from FPGA-HSDK to MAX: 16 KHz.
 * => I2S data from FPGA-HSDK to MAX: 16 bit
 *
 * => MCLK disconnected, route 12.288 XTAL to clk-prescaler (JU12 on max9880a board)
 *
 * Difference with 13 Mhz settings above is "*" regs
 */
const max9880a_settings_t settings_i2sin_lineout_12_288mhz[] =
{
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x04 },        // 0x27,
	{ MAX9880A_SYSTEM_CLOCK,                         0x10 },        // 0x05,  MCLK between 10-20 Mhz (PCLK=MCLK)

	// For recording (S1 pins are mapped on DAI1)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x06*, PLL=enabled, Ni=0x2000, DAI1 for LineIn -> I2S out, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x07*
	{ MAX9880A_DAI1_INTERFACE_MODE_A,                0x10 },        // 0x08,  Slave mode, Delay Mode
	{ MAX9880A_DAI1_INTERFACE_MODE_B,                0x30 },        // 0x09,  SEL1 = 0, Map S1 to DAI1
	// For playback (S2 pins are mapped on DAI2)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x0B*, PLL=enabled, Ni=0x2000, DAI2 for I2S in -> LineOut, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x0C*
	{ MAX9880A_DAI2_INTERFACE_MODE_A,                0x10 },        // 0x0D,  Slave mode, Delay Mode, 16BitI2S
	{ MAX9880A_DAI2_INTERFACE_MODE_B,                0x50 },        // 0x0E,  SEL2 = 1, Map S2 to DAI2

	{ MAX9880A_DAC_LR_MIXER,                         0x21 },        // 0x10,  DAi2 left channel, DAi2 right channel (audio out)
	{ MAX9880A_CODEC_FILTERS,                        0x80 },        // 0x11,  select music filters

	{ MAX9880A_LEFT_ADC_LEVEL,                       0x0D },        // 0x18,  Gain 0dB, Gain -10dB
	{ MAX9880A_RIGHT_ADC_LEVEL,                      0x0D },        // 0x19,  Gain 0dB, Gain -10dB
	{ MAX9880A_LEFT_LINE_INPUT_LEVEL,                0x0C },        // 0x1A,  Gain 0dB
	{ MAX9880A_RIGHT_LINE_INPUT_LEVEL,               0x0C },        // 0x1B,  Gain 0dB

	{ MAX9880A_LEFT_VOLUME_CTRL,                     0x0C },        // 0x1C,  Gain 0dB
	{ MAX9880A_RIGHT_VOLUME_CTRL,                    0x0C },        // 0x1D,  Gain 0dB
	{ MAX9880A_LEFT_LINE_OUTPUT_LEVEL,               0x00 },        // 0x1E,  unmuted, Gain 0dB
	{ MAX9880A_RIGHT_LINE_OUTPUT_LEVEL,              0x00 },        // 0x1F,  unmuted, Gain 0dB

	{ MAX9880A_INPUT,                                0x00 },        // 0x22,  No input selected
	{ MAX9880A_ENABLE,                               0x3C },        // 0x26,  LineOut-Enable, LineIn-Disable, Left/Right DAC enabled
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x88 },        // 0x27*, Power On, Xtal to clk-prescaler
};

#define SETTINGS_I2SIN_LINEOUT_12_288MHZ_LENGTH  (sizeof(settings_i2sin_lineout_12_288mhz) / sizeof(settings_i2sin_lineout_12_288mhz[0]))

/**
 * MAX9880A_LINEIN_I2SOUT_12DOT288MHZ
 *
 * Recording: LineIn -> I2S out, Stereo 16Bit I2S, Fs=16 Khz (LRclk=32Khz)
 *
 * Measured with scope:
 * => I2S clk from FPGA-HSDK to MAX: 512 KHz, divider is 48. (24.567 / 48 = 512 KHz).
 * => I2S ws  from FPGA-HSDK to MAX: 16 KHz.
 *
 * => MCLK disconnected, route 12.288 XTAL to clk-prescaler
 *
 * Difference with 13 Mhz settings above is "*" regs
 */
const max9880a_settings_t settings_linein_i2sout_12_288mhz[] =
{
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x04 },        // 0x27
	{ MAX9880A_SYSTEM_CLOCK,                         0x10 },        // 0x05,  MCLK between 10-20 Mhz (PCLK=MCLK)

	// For recording (S1 pins are mapped on DAI1)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x06*, PLL=enabled, Ni=0x2000, DAI1 for LineIn -> I2S out, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x07*
	{ MAX9880A_DAI1_INTERFACE_MODE_A,                0x10 },        // 0x08,  Slave mode, Delay Mode
	{ MAX9880A_DAI1_INTERFACE_MODE_B,                0x30 },        // 0x09,  SEL1 = 0, Map S1 to DAI1
	// For playback (S2 pins are mapped on DAI2)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x0B*, PLL=enabled, Ni=0x2000, DAI2 for I2S in -> LineOut, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x0C*
	{ MAX9880A_DAI2_INTERFACE_MODE_A,                0x10 },        // 0x0D,  Slave mode, Delay Mode, 16BitI2S
	{ MAX9880A_DAI2_INTERFACE_MODE_B,                0x50 },        // 0x0E,  SEL2 = 1, Map S2 to DAI2

	{ MAX9880A_DAC_LR_MIXER,                         0x21 },        // 0x10,  DAi2 left channel, DAi2 right channel (audio out)
	{ MAX9880A_CODEC_FILTERS,                        0x80 },        // 0x11,  Select music filters

	{ MAX9880A_LEFT_ADC_LEVEL,                       0x0D },        // 0x18,  Gain 0dB, Gain -10dB
	{ MAX9880A_RIGHT_ADC_LEVEL,                      0x0D },        // 0x19,  Gain 0dB, Gain -10dB
	{ MAX9880A_LEFT_LINE_INPUT_LEVEL,                0x0C },        // 0x1A,  Gain 0dB
	{ MAX9880A_RIGHT_LINE_INPUT_LEVEL,               0x0C },        // 0x1B,  Gain 0dB

	{ MAX9880A_LEFT_VOLUME_CTRL,                     0x0C },        // 0x1C,  Gain 0dB
	{ MAX9880A_RIGHT_VOLUME_CTRL,                    0x0C },        // 0x1D,  Gain 0dB
	{ MAX9880A_LEFT_LINE_OUTPUT_LEVEL,               0x40 },        // 0x1E,  muted
	{ MAX9880A_RIGHT_LINE_OUTPUT_LEVEL,              0x40 },        // 0x1F,  muted

	{ MAX9880A_INPUT,                                0xA0 },        // 0x22,  Left & Right Line input
	{ MAX9880A_ENABLE,                               0xC3 },        // 0x26,  LineIn-Enable, LineOut-Disable, Left/Right ADC enabled
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x88 },        // 0x27*, Power On, Xtal to clk-prescaler
};

#define SETTINGS_LINEIN_I2SOUT_12_288MHZ_LENGTH  (sizeof(settings_linein_i2sout_12_288mhz) / sizeof(settings_linein_i2sout_12_288mhz[0]))

/**
 * MAX9880A_I2SIN_LINEOUT_24DOT567MHZ
 *
 * Playback: I2S In -> LineOut, Stereo 16Bit I2S, Fs=16 Khz (LRclk=32Khz)
 *
 * Measured with scope:
 * => I2S clk  from FPGA-HSDK to MAX: 512 KHz, divider is 48. (24.567 / 48 = 512 KHz).
 * => I2S ws	from FPGA-HSDK to MAX: 16 KHz.
 * => I2S data from FPGA-HSDK to MAX: 16 bit
 *
 * => MCLK from FPGA-HSDK of 24.567 MHz
 *
 * Difference with 13 Mhz settings above is "*" regs
 */
const max9880a_settings_t settings_i2sin_lineout_24_567mhz[] =
{
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x04 },        // 0x27,
	{ MAX9880A_SYSTEM_CLOCK,                         0x20 },        // 0x05*, MCLK between 20-40 Mhz (PCLK=MCLK/2)

	// For recording (S1 pins are mapped on DAI1)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x06*, PLL=enabled, Ni=0x2000, DAI1 for LineIn -> I2S out, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x07*
	{ MAX9880A_DAI1_INTERFACE_MODE_A,                0x10 },        // 0x08,  Slave mode, Delay Mode
	{ MAX9880A_DAI1_INTERFACE_MODE_B,                0x30 },        // 0x09,  SEL1 = 0, Map S1 to DAI1
	// For playback (S2 pins are mapped on DAI2)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x0B*, PLL=enabled, Ni=0x2000, DAI2 for I2S in -> LineOut, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x0C*
	{ MAX9880A_DAI2_INTERFACE_MODE_A,                0x10 },        // 0x0D,  Slave mode, Delay Mode, 16BitI2S
	{ MAX9880A_DAI2_INTERFACE_MODE_B,                0x50 },        // 0x0E,  SEL2 = 1, Map S2 to DAI2

	{ MAX9880A_DAC_LR_MIXER,                         0x21 },        // 0x10,  DAi2 left channel, DAi2 right channel (audio out)
	{ MAX9880A_CODEC_FILTERS,                        0x80 },        // 0x11,  select music filters

	{ MAX9880A_LEFT_ADC_LEVEL,                       0x0D },        // 0x18,  Gain 0dB, Gain -10dB
	{ MAX9880A_RIGHT_ADC_LEVEL,                      0x0D },        // 0x19,  Gain 0dB, Gain -10dB
	{ MAX9880A_LEFT_LINE_INPUT_LEVEL,                0x0C },        // 0x1A,  Gain 0dB
	{ MAX9880A_RIGHT_LINE_INPUT_LEVEL,               0x0C },        // 0x1B,  Gain 0dB

	{ MAX9880A_LEFT_VOLUME_CTRL,                     0x0C },        // 0x1C,  Gain 0dB
	{ MAX9880A_RIGHT_VOLUME_CTRL,                    0x0C },        // 0x1D,  Gain 0dB
	{ MAX9880A_LEFT_LINE_OUTPUT_LEVEL,               0x00 },        // 0x1E,  unmuted, Gain 0dB
	{ MAX9880A_RIGHT_LINE_OUTPUT_LEVEL,              0x00 },        // 0x1F,  unmuted, Gain 0dB

	{ MAX9880A_INPUT,                                0x00 },        // 0x22,  No input selected
	{ MAX9880A_ENABLE,                               0x3C },        // 0x26,  LineOut-Enable, LineIn-Disable, Left/Right DAC enabled
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x80 },        // 0x27,  Power On, Mclk to clk-prescaler
};

#define SETTINGS_I2SIN_LINEOUT_24_567MHZ_LENGTH  (sizeof(settings_i2sin_lineout_24_567mhz) / sizeof(settings_i2sin_lineout_24_567mhz[0]))

/**
 * MAX9880A_LINEIN_I2SOUT_24DOT567MHZ
 *
 * Recording: LineIn -> I2S out, Stereo 16Bit I2S, Fs=16 Khz (LRclk=32Khz)
 *
 * Measured with scope:
 * => I2S clk from FPGA-HSDK to MAX: 512 KHz, divider is 48. (24.567 / 48 = 512 KHz).
 * => I2S ws  from FPGA-HSDK to MAX: 16 KHz.
 *
 * => MCLK from HSDK of 24.567 MHz
 *
 * Difference with 13 Mhz settings above is "*" regs
 */
const max9880a_settings_t settings_linein_i2sout_24_567mhz[] =
{
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x04 },        // 0x27
	{ MAX9880A_SYSTEM_CLOCK,                         0x20 },        // 0x05*, MCLK between 20-40 Mhz (PCLK=MCLK/2)

	// For recording (S1 pins are mapped on DAI1)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x06*, PLL=enabled, Ni=0x2000, DAI1 for LineIn -> I2S out, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x07*
	{ MAX9880A_DAI1_INTERFACE_MODE_A,                0x10 },        // 0x08,  Slave mode, Delay Mode
	{ MAX9880A_DAI1_INTERFACE_MODE_B,                0x30 },        // 0x09,  SEL1 = 0, Map S1 to DAI1
	// For playback (S2 pins are mapped on DAI2)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x0B*, PLL=enabled, Ni=0x2000, DAI2 for I2S in -> LineOut, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x0C*
	{ MAX9880A_DAI2_INTERFACE_MODE_A,                0x10 },        // 0x0D,  Slave mode, Delay Mode, 16BitI2S
	{ MAX9880A_DAI2_INTERFACE_MODE_B,                0x50 },        // 0x0E,  SEL2 = 1, Map S2 to DAI2

	{ MAX9880A_DAC_LR_MIXER,                         0x21 },        // 0x10,  DAi2 left channel, DAi2 right channel (audio out)
	{ MAX9880A_CODEC_FILTERS,                        0x80 },        // 0x11,  Select music filters

	{ MAX9880A_LEFT_ADC_LEVEL,                       0x0D },        // 0x18,  Gain 0dB, Gain -10dB
	{ MAX9880A_RIGHT_ADC_LEVEL,                      0x0D },        // 0x19,  Gain 0dB, Gain -10dB
	{ MAX9880A_LEFT_LINE_INPUT_LEVEL,                0x0C },        // 0x1A,  Gain 0dB
	{ MAX9880A_RIGHT_LINE_INPUT_LEVEL,               0x0C },        // 0x1B,  Gain 0dB

	{ MAX9880A_LEFT_VOLUME_CTRL,                     0x0C },        // 0x1C,  Gain 0dB
	{ MAX9880A_RIGHT_VOLUME_CTRL,                    0x0C },        // 0x1D,  Gain 0dB
	{ MAX9880A_LEFT_LINE_OUTPUT_LEVEL,               0x40 },        // 0x1E,  muted
	{ MAX9880A_RIGHT_LINE_OUTPUT_LEVEL,              0x40 },        // 0x1F,  muted

	{ MAX9880A_INPUT,                                0xA0 },        // 0x22,  Left & Right Line input
	{ MAX9880A_ENABLE,                               0xC3 },        // 0x26,  LineIn-Enable, LineOut-Disable, Left/Right ADC enabled
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x80 },        // 0x27,  Power On, Mclk to clk-prescaler
};

#define SETTINGS_LINEIN_I2SOUT_24_567MHZ_LENGTH  (sizeof(settings_linein_i2sout_24_567mhz) / sizeof(settings_linein_i2sout_24_567mhz[0]))

/**
 * MAX9880A_DAI2_I2SIN_LINEOUT_24DOT567MHZ
 *
 * Playback: I2S In -> LineOut, Stereo 16Bit I2S, Fs=16 Khz (LRclk=32Khz)
 *			  But now we have PLAYBACK on DAI2 (input I2S on S1 BCLKS1, LRCLKS1, SDINS1 from I2S-TX)
 *
 * Measured with scope:
 * => I2S clk  from IC-HSDK to MAX: 512 KHz, divider is 48. (24.567 / 48 = 512 KHz).
 * => I2S ws	from IC-HSDK to MAX: 16 KHz.
 * => I2S data from IC-HSDK to MAX: 16 bit
 *
 * => MCLK from HSDK of 24.567 MHz
 *
 */
const max9880a_settings_t settings_dai2_i2sin_lineout_24_567mhz[] =
{
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x04 },        // 0x27,
	{ MAX9880A_SYSTEM_CLOCK,                         0x20 },        // 0x05, MCLK between 20-40 Mhz (PCLK=MCLK/2)

	// For recording (S2 pins are mapped on DAI1)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x06,  PLL=enabled, Ni=0x2000, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x07
	{ MAX9880A_DAI1_INTERFACE_MODE_A,                0x10 },        // 0x08,  Slave mode, Delay Mode
	{ MAX9880A_DAI1_INTERFACE_MODE_B,                0x50 },        // 0x09,  SEL1 = 1, Map S1 to DAI2, SDIEN1=1, SDOEN1=0
	// For playback (S1 pins are mapped on DAI2)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x0B,  PLL=enabled, Ni=0x2000, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x0C
	{ MAX9880A_DAI2_INTERFACE_MODE_A,                0x10 },        // 0x0D,  Slave mode, Delay Mode, 16BitI2S
	{ MAX9880A_DAI2_INTERFACE_MODE_B,                0x30 },        // 0x0E,  SEL2 = 0, Map S2 to DAI1, SDIEN2=1, SDOEN2=1

	{ MAX9880A_DAC_LR_MIXER,                         0x21 },        // 0x10,  DAi2 left channel, DAi2 right channel (i2s in)
	{ MAX9880A_CODEC_FILTERS,                        0x80 },        // 0x11,  select music filters

	{ MAX9880A_LEFT_ADC_LEVEL,                       0x0D },        // 0x18,  Gain 0dB, Gain -10dB
	{ MAX9880A_RIGHT_ADC_LEVEL,                      0x0D },        // 0x19,  Gain 0dB, Gain -10dB
	{ MAX9880A_LEFT_LINE_INPUT_LEVEL,                0x0C },        // 0x1A,  Gain 0dB
	{ MAX9880A_RIGHT_LINE_INPUT_LEVEL,               0x0C },        // 0x1B,  Gain 0dB

	{ MAX9880A_LEFT_VOLUME_CTRL,                     0x0C },        // 0x1C,  Gain 0dB
	{ MAX9880A_RIGHT_VOLUME_CTRL,                    0x0C },        // 0x1D,  Gain 0dB
	{ MAX9880A_LEFT_LINE_OUTPUT_LEVEL,               0x00 },        // 0x1E,  unmuted, Gain 0dB
	{ MAX9880A_RIGHT_LINE_OUTPUT_LEVEL,              0x00 },        // 0x1F,  unmuted, Gain 0dB

	{ MAX9880A_INPUT,                                0x00 },        // 0x22,  No input selected
	{ MAX9880A_ENABLE,                               0x3C },        // 0x26,  LineOut-Enable, LineIn-Disable, Left/Right DAC enabled
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x80 },        // 0x27,  Power On, Mclk to clk-prescaler
};

#define SETTINGS_DAI2_I2SIN_LINEOUT_24_567MHZ_LENGTH     (sizeof(settings_dai2_i2sin_lineout_24_567mhz) / sizeof(settings_dai2_i2sin_lineout_24_567mhz[0]))

/**
 * MAX9880A_LINEIN_DAI1_I2SOUT_24DOT567MHZ
 *
 * Recording: LineIn -> I2S out, Stereo 16Bit I2S, Fs=16 Khz (LRclk=32Khz)
 *				But now we have RECORDING on DAI1 (output I2S on S2 BCLKS2, LRCLKS2, SDOUTS2 to I2S-RX)
 *
 * Measured with scope:
 * => I2S clk from IC-HSDK to MAX: 512 KHz, divider is 48. (24.567 / 48 = 512 KHz).
 * => I2S ws  from IC-HSDK to MAX: 16 KHz.
 *
 * => MCLK from IC-HSDK of 24.567 MHz
 */
const max9880a_settings_t settings_linein_dai1_i2sout_24_567mhz[] =
{
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x04 },        // 0x27
	{ MAX9880A_SYSTEM_CLOCK,                         0x20 },        // 0x05,  MCLK between 20-40 Mhz (PCLK=MCLK/2)

	// For recording (S2 pins are mapped on DAI1)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x06,  PLL=enabled, Ni=0x2000, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x07
	{ MAX9880A_DAI1_INTERFACE_MODE_A,                0x10 },        // 0x08,  Slave mode, Delay Mode
	{ MAX9880A_DAI1_INTERFACE_MODE_B,                0x50 },        // 0x09,  SEL1 = 1, Map S1 to DAI2, SDIEN1=1, SDOEN1=0
	// For playback (S1 pins are mapped on DAI2)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x0B,  PLL=enabled, Ni=0x2000, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x0C
	{ MAX9880A_DAI2_INTERFACE_MODE_A,                0x10 },        // 0x0D,  Slave mode, Delay Mode, 16BitI2S
	{ MAX9880A_DAI2_INTERFACE_MODE_B,                0x30 },        // 0x0E,  SEL2 = 0, Map S2 to DAI1, SDIEN2=1, SDOEN2=1

	{ MAX9880A_DAC_LR_MIXER,                         0x84 },        // 0x10,  DAi1 left channel, DAi1 right channel (i2s out)
	{ MAX9880A_CODEC_FILTERS,                        0x80 },        // 0x11,  Select music filters

	{ MAX9880A_LEFT_ADC_LEVEL,                       0x0D },        // 0x18,  Gain 0dB, Gain -10dB
	{ MAX9880A_RIGHT_ADC_LEVEL,                      0x0D },        // 0x19,  Gain 0dB, Gain -10dB
	{ MAX9880A_LEFT_LINE_INPUT_LEVEL,                0x0C },        // 0x1A,  Gain 0dB
	{ MAX9880A_RIGHT_LINE_INPUT_LEVEL,               0x0C },        // 0x1B,  Gain 0dB

	{ MAX9880A_LEFT_VOLUME_CTRL,                     0x0C },        // 0x1C,  Gain 0dB
	{ MAX9880A_RIGHT_VOLUME_CTRL,                    0x0C },        // 0x1D,  Gain 0dB
	{ MAX9880A_LEFT_LINE_OUTPUT_LEVEL,               0x40 },        // 0x1E,  muted
	{ MAX9880A_RIGHT_LINE_OUTPUT_LEVEL,              0x40 },        // 0x1F,  muted

	{ MAX9880A_INPUT,                                0xA0 },        // 0x22,  Left & Right Line input
	{ MAX9880A_ENABLE,                               0xC3 },        // 0x26,  LineIn-Enable, LineOut-Disable, Left/Right ADC enabled
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x80 },        // 0x27,  Power On, Mclk to clk-prescaler
};

#define SETTINGS_LINEIN_DAI1_I2SOUT_24_567MHZ_LENGTH     (sizeof(settings_linein_dai1_i2sout_24_567mhz) / sizeof(settings_linein_dai1_i2sout_24_567mhz[0]))

/**
 * MAX9880A_24DOT567MHZ
 *
 * Playback & Recording (both tables mixed)
 */
const max9880a_settings_t settings_24_567mhz[] =
{
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x04 },        // 0x27,
	{ MAX9880A_SYSTEM_CLOCK,                         0x20 },        // 0x05, MCLK between 20-40 Mhz (PCLK=MCLK/2)

	// For recording (S2 pins are mapped on DAI1)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x06, PLL=enabled, Ni=0x2000, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x07
	{ MAX9880A_DAI1_INTERFACE_MODE_A,                0x10 },        // 0x08, Slave mode, Delay Mode
	{ MAX9880A_DAI1_INTERFACE_MODE_B,                0x50 },        // 0x09, SEL1 = 1, Map S1 to DAI2, SDIEN1=1, SDOEN1=0
	// For playback (S1 pins are mapped on DAI2)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_HIGH,      0xA0 },        // 0x0B, PLL=enabled, Ni=0x2000, See table 6 datasheet (LRCLK=16 KHz PCLK=12.288 MHz)
	{ MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_LOW,       0x00 },        // 0x0C
	{ MAX9880A_DAI2_INTERFACE_MODE_A,                0x10 },        // 0x0D, Slave mode, Delay Mode, 16BitI2S
	{ MAX9880A_DAI2_INTERFACE_MODE_B,                0x30 },        // 0x0E, SEL2 = 0, Map S2 to DAI1, SDIEN2=1, SDOEN2=1

	{ MAX9880A_DAC_LR_MIXER,                         0xA5 },        // 0x10, DAi2 left channel, DAi2 right channel (i2s in/out)
	{ MAX9880A_CODEC_FILTERS,                        0x80 },        // 0x11, select music filters

	{ MAX9880A_LEFT_ADC_LEVEL,                       0x0D },        // 0x18, Gain 0dB, Gain -10dB
	{ MAX9880A_RIGHT_ADC_LEVEL,                      0x0D },        // 0x19, Gain 0dB, Gain -10dB
	{ MAX9880A_LEFT_LINE_INPUT_LEVEL,                0x0C },        // 0x1A, Gain 0dB
	{ MAX9880A_RIGHT_LINE_INPUT_LEVEL,               0x0C },        // 0x1B, Gain 0dB

	{ MAX9880A_LEFT_VOLUME_CTRL,                     0x0C },        // 0x1C, Gain 0dB
	{ MAX9880A_RIGHT_VOLUME_CTRL,                    0x0C },        // 0x1D, Gain 0dB
	{ MAX9880A_LEFT_LINE_OUTPUT_LEVEL,               0x00 },        // 0x1E, unmuted, Gain 0dB
	{ MAX9880A_RIGHT_LINE_OUTPUT_LEVEL,              0x00 },        // 0x1F, unmuted, Gain 0dB

	{ MAX9880A_INPUT,                                0xA0 },        // 0x22, Left & Right Line input
	{ MAX9880A_ENABLE,                               0xFF },        // 0x26, LineOut-Enable, LineIn-Enable, Left/Right ADC/DAC enabled
	{ MAX9880A_SYSTEM_SHUTDOWN,                      0x80 },        // 0x27, Power On, Mclk to clk-prescaler
};

#define SETTINGS_24_567MHZ_LENGTH    (sizeof(settings_24_567mhz) / sizeof(settings_24_567mhz[0]))

/******************************************************************************/
/***********      END of MAX9880A Configs      ********************************/
/******************************************************************************/

// Max9880a register name list
typedef struct max9880a_reg_table {
	char str[10];
	max9880a_reg_t reg;
} max9880a_reg_table_t;

const max9880a_reg_table_t max9880a_reg_table[] =
{
	{ "STS      ", MAX9880A_STATUS },
	{ "JACK STS ", MAX9880A_JACKSTATUS },
	{ "AUX HIGH ", MAX9880A_AUXHIGH },
	{ "AUX LOW  ", MAX9880A_AUXLOW },
	{ "INT ENA  ", MAX9880A_INTERRUPT_ENABLE },
	{ "SYS CLK  ", MAX9880A_SYSTEM_CLOCK },
	// DAI1-Clock Control
	{ "DAI1 CLKH", MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_HIGH },
	{ "DAI1 CLKL", MAX9880A_DAI1_STEREO_AUDIO_CLK_CTRL_LOW },
	// DAI1-Configuration
	{ "INTMODE A", MAX9880A_DAI1_INTERFACE_MODE_A },
	{ "INTMODE B", MAX9880A_DAI1_INTERFACE_MODE_B },
	{ "TIME DIV ", MAX9880A_DAI1_TIME_DIVISION_MUL },
	// DAI2-Clock Control
	{ "DAI2 CLKH", MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_HIGH },
	{ "DAI2 CLKL", MAX9880A_DAI2_STEREO_AUDIO_CLK_CTRL_LOW },
	// DAI2-Configuration
	{ "INTMODE A", MAX9880A_DAI2_INTERFACE_MODE_A },
	{ "INTMODE B", MAX9880A_DAI2_INTERFACE_MODE_B },
	{ "TIME DIV ", MAX9880A_DAI2_TIME_DIVISION_MUL },
	// Digital Mixers
	{ "DAC-LRMIX", MAX9880A_DAC_LR_MIXER },
	// Digital Filtering
	{ "CODEC    ", MAX9880A_CODEC_FILTERS },
	// SPDM Outputs
	{ "SPDM CONF", MAX9880A_SPDM_CONFIGURATIONS },
	{ "SPDM INP ", MAX9880A_SPDM_INPUT },
	{ "REV ID   ", MAX9880A_REV_ID },
	// LEVEL control
	{ "SIDE TONE", MAX9880A_SIDETONE },
	{ "STDAC LVL", MAX9880A_STEREO_DAC_LEVEL },
	{ "V-DAC LVL", MAX9880A_VOICE_DAC_LEVEL },
	{ "ADC LVL L", MAX9880A_LEFT_ADC_LEVEL },
	{ "ADC LVL R", MAX9880A_RIGHT_ADC_LEVEL },
	{ "LIINLVL L", MAX9880A_LEFT_LINE_INPUT_LEVEL },
	{ "LIINLVL R", MAX9880A_RIGHT_LINE_INPUT_LEVEL },
	{ "VOLCTRL L", MAX9880A_LEFT_VOLUME_CTRL },
	{ "VOLCTRL R", MAX9880A_RIGHT_VOLUME_CTRL },
	{ "LIOULVL L", MAX9880A_LEFT_LINE_OUTPUT_LEVEL },
	{ "LIOULVL R", MAX9880A_RIGHT_LINE_OUTPUT_LEVEL },
	{ "MICGAIN L", MAX9880A_LEFT_MIC_GAIN },
	{ "MICGAIN R", MAX9880A_RIGHT_MIC_GAIN },
	// Configuration
	{ "INPUT    ", MAX9880A_INPUT },
	{ "MIC      ", MAX9880A_MICROPHONE },
	{ "MODE     ", MAX9880A_MODE },
	{ "JACK DET ", MAX9880A_JACK_DETECT },
	// Power Management
	{ "ENABLE   ", MAX9880A_ENABLE },
	{ "SHUTDOWN ", MAX9880A_SYSTEM_SHUTDOWN },
	{ "REV ID   ", MAX9880A_REVISION_ID },
};

#define  MAX9880A_REG_LENGTH     (sizeof(max9880a_reg_table) / sizeof(max9880a_reg_table[0]))

uint32_t max9880a_dump_reg(MAX9880A_DEF_PTR obj)
{
	int32_t ercd;
	uint8_t buffer_rx[MAX9880A_REG_LENGTH - 1];// ignore last register (rev id)
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	MAX9880A_CHECK_EXP(iic_obj != NULL, E_OBJ);

	ercd = iic_obj->iic_read((void *)buffer_rx, MAX9880A_REG_LENGTH - 1);
	for (int i = 0; i < MAX9880A_REG_LENGTH - 1; i++) {
		dbg_printf(DBG_LESS_INFO, " %s (0x%x) = 0x%x\n\r", max9880a_reg_table[i].str, max9880a_reg_table[i].reg, buffer_rx[i]);
	}

error_exit:
	return ercd;
}

uint32_t max9880a_config(MAX9880A_DEF_PTR obj, max9880a_config_t config)
{
	int32_t ercd;
	max9880a_settings_t *ptr;
	uint32_t length;
	uint8_t value[2];
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	MAX9880A_CHECK_EXP(iic_obj != NULL, E_OBJ);

	switch (config) {
	case MAX9880A_I2SIN_LINEOUT_13MHZ:
		ptr = (max9880a_settings_t *)settings_i2sin_lineout_13mhz;
		length = SETTINGS_I2SIN_LINEOUT_13MHZ_LENGTH;
		break;
		break;
	case MAX9880A_LINEIN_I2SOUT_13MHZ:
		ptr = (max9880a_settings_t *)settings_linein_i2sout_13mhz;
		length = SETTINGS_LINEIN_I2SOUT_13MHZ_LENGTH;
		break;
		break;
	case MAX9880A_I2SIN_LINEOUT_12DOT288MHZ:
		ptr = (max9880a_settings_t *)settings_i2sin_lineout_12_288mhz;
		length = SETTINGS_I2SIN_LINEOUT_12_288MHZ_LENGTH;
		break;
		break;
	case MAX9880A_LINEIN_I2SOUT_12DOT288MHZ:
		ptr = (max9880a_settings_t *)settings_linein_i2sout_12_288mhz;
		length = SETTINGS_LINEIN_I2SOUT_12_288MHZ_LENGTH;
		break;
		break;
	case MAX9880A_I2SIN_LINEOUT_24DOT567MHZ:
		ptr = (max9880a_settings_t *)settings_i2sin_lineout_24_567mhz;
		length = SETTINGS_I2SIN_LINEOUT_24_567MHZ_LENGTH;
		break;
		break;
	case MAX9880A_LINEIN_I2SOUT_24DOT567MHZ:
		ptr = (max9880a_settings_t *)settings_linein_i2sout_24_567mhz;
		length = SETTINGS_LINEIN_I2SOUT_24_567MHZ_LENGTH;
		break;
		break;
	case MAX9880A_DAI2_I2SIN_LINEOUT_24DOT567MHZ:
		ptr = (max9880a_settings_t *)settings_dai2_i2sin_lineout_24_567mhz;
		length = SETTINGS_DAI2_I2SIN_LINEOUT_24_567MHZ_LENGTH;
		break;
		break;
	case MAX9880A_LINEIN_DAI1_I2SOUT_24DOT567MHZ:
		ptr = (max9880a_settings_t *)settings_linein_dai1_i2sout_24_567mhz;
		length = SETTINGS_LINEIN_DAI1_I2SOUT_24_567MHZ_LENGTH;
		break;
		break;
	case MAX9880A_24DOT567MHZ:
		ptr = (max9880a_settings_t *)settings_24_567mhz;
		length = SETTINGS_24_567MHZ_LENGTH;
		break;
	default:
		ptr = NULL;
		length = 0;
		break;
	}

	for (int i = 0; i < length; i++) {
		value[0] = (uint8_t)(ptr[i].reg);
		value[1] = (uint8_t)(ptr[i].val);
		ercd = iic_obj->iic_write(value, 2);
		dbg_printf(DBG_MORE_INFO, "[%d]iic_write 0x%x 0x%x ret %d\r\n", i, value[0], value[1], ercd);
		MAX9880A_CHECK_EXP(ercd == 2, E_SYS);
	}
	ercd = E_OK;
error_exit:
	return ercd;
}

uint32_t max9880a_init(MAX9880A_DEF_PTR obj)
{
	int32_t ercd;
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	MAX9880A_CHECK_EXP(iic_obj != NULL, E_OBJ);
	ercd = iic_obj->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD);
	dbg_printf(DBG_MORE_INFO, "[%s]iic_open ret %d\r\n", __FUNCTION__, ercd);// should return E_OK
	MAX9880A_CHECK_EXP(ercd == E_OK, E_SYS);
	ercd = iic_obj->iic_control(IIC_CMD_ENA_DEV, (void *)obj->slvaddr);
	dbg_printf(DBG_MORE_INFO, "[%s]iic_control IIC_CMD_ENA_DEV ret %d\r\n", __FUNCTION__, ercd);// should return E_OK
	MAX9880A_CHECK_EXP(ercd == E_OK, E_SYS);
	ercd = iic_obj->iic_control(IIC_CMD_SET_ADDR_MODE, IIC_7BIT_ADDRESS);
	dbg_printf(DBG_MORE_INFO, "[%s]iic_control IIC_CMD_SET_ADDR_MODE ret %d\r\n", __FUNCTION__, ercd);// should return E_OK
	MAX9880A_CHECK_EXP(ercd == E_OK, E_SYS);
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, (void *)obj->slvaddr);
	dbg_printf(DBG_MORE_INFO, "[%s]iic_control IIC_CMD_MST_SET_TAR_ADDR ret %d\r\n", __FUNCTION__, ercd);// should return E_OK
	MAX9880A_CHECK_EXP(ercd == E_OK, E_SYS);

	// dbg_printf(DBG_LESS_INFO, "max9880a_init\n\r");
error_exit:
	return ercd;
}

uint32_t max9880a_deinit(MAX9880A_DEF_PTR obj)
{
	int32_t ercd;
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	MAX9880A_CHECK_EXP(iic_obj != NULL, E_OBJ);
	ercd = iic_obj->iic_close();

error_exit:
	return ercd;
}
