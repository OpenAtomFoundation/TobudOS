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
#include "spi_flash_fl256s.h"

// #define DBG_MORE
#include "embARC_debug.h"

#define RDID    0x9F    /*!<read chip ID */
#define RDSR1   0x05    /*!< read status register-1 */
#define WRSR1   0x01    /*!< write status registe-1 */
#define RDSR2   0x07    /*!< read status register-2 */
#define RDCR    0x35    /*!< read config register */
#define WREN    0x06    /*!< write enablewaitDeviceReady */
#define WRDI    0x04    /*!< write disable */
#define READ    0x03    /*!< read data bytes */
#define QORD    0x6B
#define SE      0x20    /*!< sector erase */
#define PP      0x02    /*!< page program */
#define QPP     0x32    /*!< page program */

#define FL256S_NOT_VALID    (0xFFFFFFFF)

/**
 * @fn int32_t _spi_send_cmd(FL256S_DEF_PTR dev, DEV_SPI_TRANSFER *xfer)
 */
Inline int32_t _spi_send_cmd(FL256S_DEF_PTR dev, DEV_SPI_TRANSFER *xfer)
{
	uint32_t cpu_status;
	DEV_SPI_PTR spi_flash;
	int32_t ercd = 0;

	spi_flash = spi_get_dev(dev->spi_master);

	// cpu_status = cpu_lock_save();

	/* select device */
	spi_flash->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)dev->cs));
	spi_flash->spi_control(SPI_CMD_MST_SET_FREQ, CONV2VOID(dev->spi_freq));
	spi_flash->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(SPI_CLK_MODE_0));
	ercd = spi_flash->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(xfer));
	/* deselect device */
	spi_flash->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID((uint32_t)dev->cs));

	// cpu_unlock_restore(cpu_status);
	return ercd;
}

/**
 * @fn int32_t _spi_send_cmd_quad(FL256S_DEF_PTR dev, DEV_SPI_PAK_PTR pak_ptr)
 */
Inline int32_t _spi_send_cmd_quad(FL256S_DEF_PTR dev, DEV_SPI_PAK_PTR pak_ptr)
{
	uint32_t cpu_status;
	DEV_SPI_PTR spi_flash;
	int32_t ercd = 0;

	spi_flash = spi_get_dev(dev->spi_master);

	// cpu_status = cpu_lock_save();

	/* select device */
	spi_flash->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)dev->cs));
	spi_flash->spi_control(SPI_CMD_MST_SET_FREQ, CONV2VOID(dev->spi_freq));
	spi_flash->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(SPI_CLK_MODE_0));

	ercd = spi_flash->spi_control(SPI_CMD_QUAD_READ, CONV2VOID(pak_ptr));
	/* deselect device */
	spi_flash->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID((uint32_t)dev->cs));

	// cpu_unlock_restore(cpu_status);
	return ercd;
}

uint32_t fl256s_read_reg(FL256S_DEF_PTR dev, uint8_t reg, uint8_t *data, uint32_t len)
{

	DEV_SPI_TRANSFER cmd_xfer = { 0 };
	DEV_SPI_TRANSFER data_xfer = { 0 };

	DEV_SPI_XFER_SET_TXBUF(&data_xfer, NULL, 0, 0);
	DEV_SPI_XFER_SET_RXBUF(&data_xfer, data, 0, len);
	DEV_SPI_XFER_SET_NEXT(&data_xfer, NULL);

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, &reg, 0, 1);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, &data_xfer);

	if (_spi_send_cmd(dev, &cmd_xfer) == 0) {
		return len;
	} else {
		return FL256S_NOT_VALID;
	}
}

uint32_t fl256s_write_reg(FL256S_DEF_PTR dev, uint8_t reg, uint8_t *data, uint32_t len)
{

	DEV_SPI_TRANSFER cmd_xfer = { 0 };
	DEV_SPI_TRANSFER data_xfer = { 0 };

	DEV_SPI_XFER_SET_TXBUF(&data_xfer, data, 0, len);
	DEV_SPI_XFER_SET_RXBUF(&data_xfer, NULL, 0, 0);
	DEV_SPI_XFER_SET_NEXT(&data_xfer, NULL);

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, &reg, 0, 1);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, &data_xfer);

	if (_spi_send_cmd(dev, &cmd_xfer) == 0) {
		return len;
	} else {
		return FL256S_NOT_VALID;
	}
}

int32_t fl256s_quad_enable(FL256S_DEF_PTR dev)
{
	uint32_t status = 0;
	uint8_t w_data[2] = { 0 };
	uint8_t r_data = 0;

	status = fl256s_read_reg(dev, RDCR, &r_data, 1);
	if (status == FL256S_NOT_VALID) {
		return -1;
	}
	if (!(r_data & 0x02)) {
		w_data[1] = r_data | 0x02;
		status = fl256s_read_reg(dev, RDSR1, &r_data, 1);
		if (status == FL256S_NOT_VALID) {
			return -1;
		}
		w_data[0] = r_data;
		if (fl256s_write_enable(dev) != 0) {
			return -1;
		}
		status = fl256s_write_reg(dev, WRSR1, w_data, 2);
		if (status == FL256S_NOT_VALID) {
			return -1;
		}
		dev->quad_en = 1;
	} else {
		dev->quad_en = 1;
	}
	return 0;
}

int32_t fl256s_quad_disable(FL256S_DEF_PTR dev)
{
	uint32_t status = 0;
	uint8_t w_data[2] = { 0 };
	uint8_t r_data = 0;

	status = fl256s_read_reg(dev, RDCR, &r_data, 1);
	if (status == FL256S_NOT_VALID) {
		return -1;
	}
	if (r_data & 0x02) {
		w_data[1] = r_data & 0xFD;
		status = fl256s_read_reg(dev, RDSR1, &r_data, 1);
		if (status == FL256S_NOT_VALID) {
			return -1;
		}
		w_data[0] = r_data;
		if (fl256s_write_enable(dev) != 0) {
			return -1;
		}
		status = fl256s_write_reg(dev, WRSR1, w_data, 2);
		if (status == FL256S_NOT_VALID) {
			return -1;
		}
		dev->quad_en = 0;
	} else {
		dev->quad_en = 0;
	}
	return 0;
}

uint32_t fl256s_read_id(FL256S_DEF_PTR dev)
{
	uint32_t id = 0;
	uint8_t local_buf[5];
	DEV_SPI_TRANSFER cmd_xfer;

	local_buf[0] = RDID;

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 1);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, local_buf, 0, 5);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

	if (_spi_send_cmd(dev, &cmd_xfer) == 0) {
		id = (local_buf[1] << 24) | (local_buf[2] << 16) | (local_buf[3] << 8) | local_buf[4];
	} else {
		id = FL256S_NOT_VALID;
	}
	dbg_printf(DBG_MORE_INFO, "fl256s_read_id buf: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\r\n", local_buf[0], local_buf[1], local_buf[2], local_buf[3], local_buf[4]);

	return id;
}

int32_t fl256s_wait_ready(FL256S_DEF_PTR dev)
{
	uint32_t status = 0x01;
	uint8_t r_data = 0;

	do {
		status = fl256s_read_reg(dev, RDSR1, &r_data, 1);
		if (status == FL256S_NOT_VALID) {
			return -1;
		}
	} while (r_data & 0x01);// Status Register 1 Bit 0

	return 0;
}

int32_t fl256s_init(FL256S_DEF_PTR dev, uint32_t freq)
{
	DEV_SPI_PTR spi_flash;
	int32_t ercd = 0;

	dev->spi_freq = freq;
	spi_flash = spi_get_dev(dev->spi_master);

	ercd = spi_flash->spi_open(DEV_MASTER_MODE, freq);

	if (ercd != E_OK && ercd != E_OPNED) {
		return ercd;
	}

	// spi_flash->spi_control(SPI_CMD_SET_DUMMY_DATA, CONV2VOID(0xFF));
	ercd = fl256s_wait_ready(dev);
	dbg_printf(DBG_MORE_INFO, "fl256s_wait_ready ret %d\r\n", ercd);
	return E_OK;
}

int32_t fl256s_write_enable(FL256S_DEF_PTR dev)
{
	uint32_t status = 0;
	uint8_t w_data, r_data = 0;

	do {
		status = fl256s_write_reg(dev, WREN, &w_data, 0);
		if (status == FL256S_NOT_VALID) {
			return -1;
		}

		status = fl256s_read_reg(dev, RDSR1, &r_data, 1);

		if (status == FL256S_NOT_VALID) {
			return -1;
		}
		// clear protection bits
		//  Write Protect. and Write Enable.
		if ((r_data & 0xfc) && (r_data & 0x02)) {
			w_data = 0;

			status = fl256s_write_reg(dev, WRSR1, &w_data, 1);
			if (status == FL256S_NOT_VALID) {
				return -1;
			}
			r_data = 0;
		}
	} while (r_data != 0x02);

	return 0;
}

int32_t fl256s_write_disable(FL256S_DEF_PTR dev)
{
	uint32_t status = 0;
	uint8_t w_data, r_data = 0;

	do {
		status = fl256s_write_reg(dev, WRDI, &w_data, 0);
		if (status == FL256S_NOT_VALID) {
			return -1;
		}

		status = fl256s_read_reg(dev, RDSR1, &r_data, 1);
		if (status == FL256S_NOT_VALID) {
			return -1;
		}

		dbg_printf(DBG_MORE_INFO, "fl256s_write_disable status = 0x%x\r\n", r_data);
		if (status == FL256S_NOT_VALID) {
			return -1;
		}
	} while (r_data & 0x02);// Status Register 1 Bit 1

	return 0;

}

int32_t fl256s_erase(FL256S_DEF_PTR dev, uint32_t address, uint32_t size)
{
	uint32_t last_address;
	uint32_t count = 0;
	uint8_t local_buf[4];
	DEV_SPI_TRANSFER cmd_xfer;

	// start address of last sector
	last_address = (address + size) & (~(dev->sector_sz - 1));

	// start address of first sector
	address &= ~(dev->sector_sz - 1);

	do {
		if (fl256s_write_enable(dev) != 0) {
			return -1;
		}

		if (fl256s_wait_ready(dev) != 0) {
			return -1;
		}

		local_buf[0] = SE;
		local_buf[1] = (address >> 16) & 0xff;
		local_buf[2] = (address >> 8) & 0xff;
		local_buf[3] =  address & 0xff;

		DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 4);
		DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

		if (_spi_send_cmd(dev, &cmd_xfer) != 0) {
			return -1;
		}

		address += dev->sector_sz;
		count++;
	} while (address <= last_address);

	if (fl256s_wait_ready(dev) != 0) {
		return -1;
	}
	if (fl256s_write_disable(dev) != 0) {
		return -1;
	}

	return (int32_t)count;
}

int32_t fl256s_write(FL256S_DEF_PTR dev, uint32_t address, uint32_t size, const void *data)
{

	uint8_t local_buf[4];
	DEV_SPI_TRANSFER cmd_xfer;
	DEV_SPI_TRANSFER data_xfer;

	uint32_t first = 0;
	uint32_t size_orig = size;

	if (fl256s_wait_ready(dev) != 0) {
		return -1;
	}

	first = FLASH_PAGE_SIZE - (address & (FLASH_PAGE_SIZE - 1));

	do {
		// send write enable command to flash
		if (fl256s_write_enable(dev) != 0) {
			return -1;
		}
		if (fl256s_wait_ready(dev) != 0) {
			return -1;
		}

		first = first < size ? first : size;

		local_buf[0] = PP;
		local_buf[1] = (address >> 16) & 0xff;
		local_buf[2] = (address >> 8) & 0xff;
		local_buf[3] = address  & 0xff;

		DEV_SPI_XFER_SET_TXBUF(&data_xfer, data, 0, first);
		DEV_SPI_XFER_SET_RXBUF(&data_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&data_xfer, NULL);

		DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 4);
		DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&cmd_xfer, &data_xfer);

		if (_spi_send_cmd(dev, &cmd_xfer) != 0) {
			return -1;
		}

		size -= first;
		address += first;
		data += first;
		first = FLASH_PAGE_SIZE;

	} while (size);

	if (fl256s_wait_ready(dev) != 0) {
		return -1;
	}
	if (fl256s_write_disable(dev) != 0) {
		return -1;
	}

	return (int32_t)(size_orig);
}

int32_t fl256s_read(FL256S_DEF_PTR dev, uint32_t address, uint32_t size, void *data)
{
	uint8_t local_buf[4];
	DEV_SPI_TRANSFER cmd_xfer;
	DEV_SPI_TRANSFER data_xfer;
	uint32_t first = 0;
	uint32_t size_orig = size;

	first = FLASH_PAGE_SIZE - (address & (FLASH_PAGE_SIZE - 1));

	do {
		if (fl256s_wait_ready(dev) != 0) {
			return -1;
		}

		first = first < size ? first : size;

		local_buf[0] = READ;
		local_buf[1] = (address >> 16) & 0xff;
		local_buf[2] = (address >> 8) & 0xff;
		local_buf[3] = address  & 0xff;

		DEV_SPI_XFER_SET_TXBUF(&data_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_RXBUF(&data_xfer, data, 0, first);
		DEV_SPI_XFER_SET_NEXT(&data_xfer, NULL);

		DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 4);
		DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&cmd_xfer, &data_xfer);

		if (_spi_send_cmd(dev, &cmd_xfer) != 0) {
			return -1;
		}
		size -= first;
		address += first;
		data += first;
		first = FLASH_PAGE_SIZE;

	} while (size);

	return (int32_t)(size_orig);
}

int32_t fl256s_quad_read(FL256S_DEF_PTR dev, uint32_t address, uint32_t size, void *data)
{
	DEV_SPI_PAK spi_pak;
	uint32_t first = 0;
	uint32_t size_orig = size;

	uint32_t per_read_size = 32;

	first = per_read_size - (address & (per_read_size - 1));

	do {
		if (fl256s_wait_ready(dev) != 0) {
			return -1;
		}

		first = first < size ? first : size;

		spi_pak.cmd = QORD;
		spi_pak.addr32 = address;
		spi_pak.data_len = first;
		spi_pak.data_ptr = data;

		if (_spi_send_cmd_quad(dev, &spi_pak) != 0) {
			return -1;
		}
		size -= first;
		address += first;
		data += first;
		first = per_read_size;

	} while (size);

	return (int32_t)(size_orig);
}