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
#include "spi_flash_w25qxx.h"

#include "embARC_error.h"
#include "board.h"
#include "device/ip_hal/dev_spi.h"
#include "arc/arc_exception.h"
#include "string.h"

/**
 * @name W25QXX SPI Flash Commands
 * @{
 */
#define RDID    0x9F    /*!<read chip ID */
#define RDSR    0x05    /*!< read status register */
#define WRSR    0x01    /*!< write status register */
#define WREN    0x06    /*!< write enablewaitDeviceReady */
#define WRDI    0x04    /*!< write disable */
#define READ    0x03    /*!< read data bytes */
#define SE      0x20    /*!< sector erase */
#define PP      0x02    /*!< page program */
/** @} end of name */

#define W25QXX_NOT_VALID    (0xFFFFFFFF)

/**
 * @fn int32_t spi_send_cmd(W25QXX_DEF_PTR dev, DEV_SPI_TRANSFER *xfer)
 * @brief	SPI flash spi send command to operate spi flash
 * @param[in]	xfer	SPI transfer that need to transfer to spi device
 * @retval	0	success
 * @retval	-1	fail
 */
Inline int32_t spi_send_cmd(W25QXX_DEF_PTR dev, DEV_SPI_TRANSFER *xfer)
{
	DEV_SPI_PTR spi_flash;
	int32_t ercd = 0;

	spi_flash = spi_get_dev(dev->spi_master);

	/* select device */
	spi_flash->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)dev->cs));
	spi_flash->spi_control(SPI_CMD_MST_SET_FREQ, CONV2VOID(dev->spi_freq));
	spi_flash->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(SPI_CLK_MODE_0));
	ercd = spi_flash->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(xfer));
	/* deselect device */
	spi_flash->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID((uint32_t)dev->cs));

	return ercd;
}

/**
 * @brief	Read the status of spi flash
 * @return	Current status of spi flash
 */
uint32_t w25qxx_read_status(W25QXX_DEF_PTR dev)
{

	uint8_t local_buf[2];
	DEV_SPI_TRANSFER cmd_xfer;

	local_buf[0] = RDSR;

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 1);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, local_buf, 1, 1);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

	if (spi_send_cmd(dev, &cmd_xfer) == 0) {
		return (uint32_t)local_buf[0];
	} else {
		return W25QXX_NOT_VALID;
	}
}

/**
 * @brief 	Enable to write flash
 * @retval	0	 Enable operation successfully
 * @retval	-1	Enable operation failed
 */
int32_t w25qxx_write_enable(W25QXX_DEF_PTR dev)
{
	uint8_t local_buf[3];
	DEV_SPI_TRANSFER cmd_xfer;

	uint32_t status = 0;

	do {
		local_buf[0] = WREN;

		DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 1);
		DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

		if (spi_send_cmd(dev, &cmd_xfer) != 0) {
			return -1;
		}

		status = w25qxx_read_status(dev);

		if (status == W25QXX_NOT_VALID) {
			return -1;
		}
		// clear protection bits
		//  Write Protect. and Write Enable.
		if ((status & 0xfc) && (status & 0x02)) {
			local_buf[0] = WRSR;    // write status
			local_buf[1] = 0x00;    // write status
			local_buf[2] = 0x00;    // write status

			DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 3);
			DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
			DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

			if (spi_send_cmd(dev, &cmd_xfer) != 0) {
				return -1;
			}
			status = 0;
		}
	} while (status != 0x02);

	return 0;
}

/**
 * @brief	Initialize spi flash interface
 * @retval	0	Initialize successfully
 * @retval	-1	Initialize failed
 */
int32_t w25qxx_init(W25QXX_DEF_PTR dev, uint32_t freq)
{
	DEV_SPI_PTR spi_flash;
	int32_t ercd = 0;

	dev->spi_freq = freq;
	spi_flash = spi_get_dev(dev->spi_master);

	ercd = spi_flash->spi_open(DEV_MASTER_MODE, freq);

	if (ercd != E_OK && ercd != E_OPNED) {
		return ercd;
	}

	spi_flash->spi_control(SPI_CMD_SET_DUMMY_DATA, CONV2VOID(0xFF));

	return E_OK;
}

/**
 * @brief	Read spi flash identification ID
 * @return	The id of the spi flash
 */
uint32_t w25qxx_read_id(W25QXX_DEF_PTR dev)
{
	uint32_t id = 0;
	uint8_t local_buf[4];
	DEV_SPI_TRANSFER cmd_xfer;

	local_buf[0] = RDID;

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 1);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, local_buf, 1, 3);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

	if (spi_send_cmd(dev, &cmd_xfer) == 0) {
		id = (local_buf[0] << 16) | (local_buf[1] << 8) | local_buf[2];
	} else {
		id = W25QXX_NOT_VALID;
	}

	return id;
}

/**
 * @brief 	Read status and wait while busy flag is set
 * @retval	0	Success
 * @retval	-1	Fail
 */
int32_t w25qxx_wait_ready(W25QXX_DEF_PTR dev)
{
	uint32_t status = 0x01;

	do {
		status = w25qxx_read_status(dev);

		if (status == W25QXX_NOT_VALID) {
			return -1;
		}
	} while (status & 0x01);

	return 0;
}

/**
 * @brief	Read data from flash
 * @param[in]	address		Start address
 * @param[in]	size		Data size
 * @param[out]	data		Pointer to the return data
 *
 * @retval	-1		Read failed
 * @retval	>= 0		Data size
 */
int32_t w25qxx_read(W25QXX_DEF_PTR dev, uint32_t address, uint32_t size, void *data)
{

	uint8_t local_buf[4];
	DEV_SPI_TRANSFER cmd_xfer;

	local_buf[0] = READ;
	local_buf[1] = (address >> 16) & 0xff;
	local_buf[2] = (address >> 8) & 0xff;
	local_buf[3] = address  & 0xff;

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 4);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, data, 4, size);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

	if (spi_send_cmd(dev, &cmd_xfer) == 0) {
		return size;
	} else {
		return -1;
	}
}

/**
 * @brief 	Flash erase in sectors
 *
 * @param[in]	address		Erase start address of spi flash
 * @param[in]	size		Erase size
 *
 * @retval	-1 		Erase failed
 * @retval	>= 0		Sector count erased
 */
int32_t w25qxx_erase(W25QXX_DEF_PTR dev, uint32_t address, uint32_t size)
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
		if (w25qxx_write_enable(dev) != 0) {
			return -1;
		}

		if (w25qxx_wait_ready(dev) != 0) {
			return -1;
		}

		local_buf[0] = SE;
		local_buf[1] = (address >> 16) & 0xff;
		local_buf[2] = (address >> 8) & 0xff;
		local_buf[3] =  address & 0xff;

		DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 4);
		DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

		if (spi_send_cmd(dev, &cmd_xfer) != 0) {
			return -1;
		}

		address += dev->sector_sz;
		count++;
	} while (address <= last_address);

	if (w25qxx_wait_ready(dev) != 0) {
		return -1;
	}

	return (int32_t)count;
}

/**
 * @brief	Write data to spi flash
 *
 * @param[in]	address	Start address
 * @param[in]	size	Data size
 * @param[in]	data	Pointer to data
 *
 * @retval	>= 0		Written bytes number
 * @retval 	< 0 		Write data failed
 */
int32_t w25qxx_write(W25QXX_DEF_PTR dev, uint32_t address, uint32_t size, const void *data)
{
	DEV_SPI_TRANSFER cmd_xfer;

	uint32_t first = 0;
	uint32_t size_orig = size;

	if (w25qxx_wait_ready(dev) != 0) {
		return -1;
	}

	first = FLASH_PAGE_SIZE - (address & (FLASH_PAGE_SIZE - 1));

	do {
		// send write enable command to flash
		if (w25qxx_write_enable(dev) != 0) {
			return -1;
		}
		if (w25qxx_wait_ready(dev) != 0) {
			return -1;
		}

		first = first < size ? first : size;

		dev->write_buf[0] = PP;
		dev->write_buf[1] = (address >> 16) & 0xff;
		dev->write_buf[2] = (address >> 8) & 0xff;
		dev->write_buf[3] = address  & 0xff;

		memcpy(&(dev->write_buf[4]), data, first);

		// DEV_SPI_XFER_SET_TXBUF(&data_xfer, data, 0, first);
		// DEV_SPI_XFER_SET_RXBUF(&data_xfer, NULL, 0, 0);
		// DEV_SPI_XFER_SET_NEXT(&data_xfer, NULL);

		DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, dev->write_buf, 0, 4 + first);
		DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

		if (spi_send_cmd(dev, &cmd_xfer) != 0) {
			return -1;
		}

		size -= first;
		address += first;
		data += first;
		first = FLASH_PAGE_SIZE;

	} while (size);

	if (w25qxx_wait_ready(dev) != 0) {
		return -1;
	}

	return (int32_t)(size_orig);
}
