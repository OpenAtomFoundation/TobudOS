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

#ifndef H_ADC088S022
#define H_ADC088S022
// ADC088S022 (a low-power, eight-channel. 23• Eight Input Channels. CMOS 8-bit analog-to-digital converter) is on EMSDP board

#define ADC_REF_VDD         3300        /*< power supply voltage is 3300mV, used as V_ref*/
#define ADC_FCLK_MIN        800000      // 0.8MHz
#define ADC_FLCK_MAX        3200000     // 3.2MHz

#define EMSDP_ADC_SPI_ID    DFSS_SPI_2_ID
#define BOARD_ADC_SPI_HZ    1000000 // 1.0MHz
#if BOARD_ADC_SPI_HZ < ADC_FCLK_MIN || BOARD_ADC_SPI_HZ > ADC_FLCK_MAX
#error  "BOARD_ADC_SPI_HZ should be within range of [ADC_FCLK_MIN, ADC_FLCK_MAX]"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** ADC088S022 object type */
typedef struct {
	uint32_t dev_id;
} ADC088S022_DEF, *ADC088S022_DEF_PTR;

#define ADC088S022_DEFINE(NAME, DEVICE_ID) \
	ADC088S022_DEF __ ## NAME = {	   \
		.dev_id = DEVICE_ID,	   \
	};				   \
	ADC088S022_DEF_PTR NAME = &__ ## NAME

extern int32_t adc088s022_adc_init(ADC088S022_DEF_PTR obj);
extern int32_t adc088s022_adc_deinit(ADC088S022_DEF_PTR obj);
extern int32_t adc088s022_adc_read(ADC088S022_DEF_PTR obj, float *val, int8_t channel);

#ifdef __cplusplus
}
#endif

#endif /* H_ADC088S022 */
