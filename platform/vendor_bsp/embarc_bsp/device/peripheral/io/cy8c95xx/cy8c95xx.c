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

#include <stdint.h>
#include "embARC_error.h"
#include "embARC_debug.h"
#include "board.h"
#include "cy8c95xx.h"

#define CY8C95XX_CHECK_EXP_NORTN(EXPR, ERROR_CODE)      CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

int32_t cy8c95xx_reg_write(CY8C95XX_DEF_PTR obj, uint8_t dev, uint8_t regaddr, uint8_t *val, uint8_t len)
{
	int32_t ercd = E_OK;
	uint8_t data[1];
	uint32_t slvaddr;
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	dbg_printf(DBG_LESS_INFO, "[%s]%d: obj 0x%x, regaddr 0x%x, val 0x%x\r\n", __FUNCTION__, __LINE__, obj, regaddr, *val);
	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	CY8C95XX_CHECK_EXP_NORTN(iic_obj != NULL, E_PAR);
	CY8C95XX_CHECK_EXP_NORTN((dev == 0) || (dev == 1), E_PAR);

	if (dev == 0) {
		slvaddr = obj->slvaddr_io;
	} else {
		slvaddr = obj->slvaddr_eep;
	}

	data[0] = (uint8_t)(regaddr & 0xff);
	/** make sure set the temp sensor's slave address */
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(slvaddr));
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	ercd = iic_obj->iic_write(data, 1);
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = iic_obj->iic_write(val, len);

error_exit:
	return ercd;
}

int32_t cy8c95xx_reg_read(CY8C95XX_DEF_PTR obj, uint8_t dev, uint8_t regaddr, uint8_t *val, uint8_t len)
{
	int32_t ercd = E_OK;
	uint8_t data[1];
	uint32_t slvaddr;
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	CY8C95XX_CHECK_EXP_NORTN(iic_obj != NULL, E_PAR);
	CY8C95XX_CHECK_EXP_NORTN((dev == 0) || (dev == 1), E_PAR);

	if (dev == 0) {
		slvaddr = obj->slvaddr_io;
	} else {
		slvaddr = obj->slvaddr_eep;
	}

	data[0] = (uint8_t)(regaddr & 0xff);
	/** make sure set the temp sensor's slave address */
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(slvaddr));
	/** write register address then read register value */
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	ercd = iic_obj->iic_write(data, 1);
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = iic_obj->iic_read(val, len);

error_exit:
	return ercd;
}

int32_t cy8c95xx_readid(CY8C95XX_DEF_PTR obj, uint8_t *value)
{
	int32_t ercd = E_OK;
	uint8_t buffer_rx[1];

	CY8C95XX_CHECK_EXP_NORTN(value != NULL, E_PAR);
	CY8C95XX_CHECK_EXP_NORTN(cy8c95xx_reg_read(obj, CY8C95XX_DEV_IO, CY8C95XX_DEVICE_ID_REG, buffer_rx, 1) == E_OK, E_SYS);
	*value = buffer_rx[0];

error_exit:
	return ercd;
}

int32_t cy8c95xx_writeport(CY8C95XX_DEF_PTR obj, uint8_t port, uint8_t value)
{
	int32_t ercd = E_OK;
	uint8_t buffer_tx[1];
	uint8_t val = 0;

	CY8C95XX_CHECK_EXP_NORTN(port < CY8C95XX_PORT_NONE, E_PAR);
	// Set port [0..7]
	buffer_tx[0] = value;
	CY8C95XX_CHECK_EXP_NORTN(cy8c95xx_reg_write(obj, CY8C95XX_DEV_IO, CY8C95XX_OUTPUT_PORT0_REG + port, buffer_tx, 1) == E_OK, E_SYS);
	CY8C95XX_CHECK_EXP_NORTN(cy8c95xx_reg_read(obj, CY8C95XX_DEV_IO, CY8C95XX_OUTPUT_PORT0_REG + port, &val, 1) == E_OK, E_SYS);
	CY8C95XX_CHECK_EXP_NORTN(val == value, E_OBJ);

error_exit:
	return ercd;
}

int32_t cy8c95xx_readport(CY8C95XX_DEF_PTR obj, uint8_t port, uint8_t *value)
{
	int32_t ercd = E_OK;

	CY8C95XX_CHECK_EXP_NORTN(port < CY8C95XX_PORT_NONE, E_PAR);
	CY8C95XX_CHECK_EXP_NORTN(cy8c95xx_reg_read(obj, CY8C95XX_DEV_IO, CY8C95XX_INPUT_PORT0_REG + port, value, 1) == E_OK, E_SYS);

error_exit:
	return ercd;
}
