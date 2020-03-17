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

// ADC088S022 is ADC on EMSDP board
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "board.h"

// #define DBG_MORE
#include "embARC_debug.h"

#include "adc088s022.h"

#define ADC_CHECK_EXP_NORTN(EXPR)       CHECK_EXP_NOERCD(EXPR, error_exit)

/**
 * @brief	Initialize adc088s022
 * @param[in]	obj	adc088s022 object
 * @retval	E_OK	Initialize adc088s022 successfully
 * @retval	!E_OK	Initialize adc088s022 failed
 */
int32_t adc088s022_adc_init(ADC088S022_DEF_PTR obj)
{
	int32_t ercd = E_OK;
	DEV_SPI_PTR spi_obj = spi_get_dev(obj->dev_id);

	dbg_printf(DBG_MORE_INFO, "[%s]%d: spi_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, spi_obj, *spi_obj);
	ADC_CHECK_EXP_NORTN(spi_obj != NULL);

	ercd = spi_obj->spi_open(DEV_MASTER_MODE, BOARD_ADC_SPI_HZ);
	if ((ercd == E_OPNED) || (ercd == E_OK)) {
		ercd = spi_obj->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(SPI_CLK_MODE_0));
	}

error_exit:
	return ercd;
}

/**
 * @brief	Deinitialize adc088s022
 * @param[in]	obj	adc088s022 object
 * @retval	E_OK	Deinitialize adc088s022 successfully
 * @retval	!E_OK	Deinitialize adc088s022 failed
 */
int32_t adc088s022_adc_deinit(ADC088S022_DEF_PTR obj)
{
	int32_t ercd = E_OK;
	DEV_SPI_PTR spi_obj = spi_get_dev(obj->dev_id);

	ercd = spi_obj->spi_close();
	return ercd;
}

/**
 * @brief	Read adc088s022 channel
 * @param[in]	obj	adc088s022 object
 * @param[out]	val	Pointer to the return value (Unit is mV)
 * @param[in]	channel ADC channel
 * @retval	E_OK	Read adc088s022 channel successfully
 * @retval	!E_OK	Read adc088s022 channel failed
 */
int32_t adc088s022_adc_read(ADC088S022_DEF_PTR obj, float *val, int8_t channel)
{
	int32_t ercd = E_OK;
	DEV_SPI_PTR spi_obj = spi_get_dev(obj->dev_id);

	dbg_printf(DBG_MORE_INFO, "[%s]%d: spi_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, spi_obj, *spi_obj);
	ADC_CHECK_EXP_NORTN(spi_obj != NULL);

	uint8_t adc_value = 0;
	uint8_t spi_buffer_rx[2];
	uint8_t spi_buffer_tx[2];
	DEV_SPI_TRANSFER spi_xfer;
	/* Master and Slave transmit */
	DEV_SPI_XFER_SET_TXBUF(&spi_xfer, spi_buffer_tx, 0, 2);
	DEV_SPI_XFER_SET_RXBUF(&spi_xfer, spi_buffer_rx, 0, 2);
	DEV_SPI_XFER_SET_NEXT(&spi_xfer, NULL);

	spi_buffer_tx[1] = ((channel & 0x07) << 3); // Select channel
	ercd = spi_obj->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(0));
	ercd = spi_obj->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&spi_xfer));
	// Read it again to have valid number (otherwise you might get value from previous channel)
	ercd = spi_obj->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&spi_xfer));
	ercd = spi_obj->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(0));

	adc_value = (spi_buffer_rx[1] << 4) | (spi_buffer_rx[0] >> 4);
	dbg_printf(DBG_MORE_INFO, "ADC-Channel(%d) = %d (0x%x)\r\n", channel, adc_value, adc_value);
	*val = adc_value * ADC_REF_VDD / 256.0; // adc088s022 is an 8-bit ADC

error_exit:
	return ercd;
}
