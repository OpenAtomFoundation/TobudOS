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
#ifndef H_SPI_FLASH_W25QXX
#define H_SPI_FLASH_W25QXX

#define FLASH_PAGE_SIZE     0x100
#define FLASH_SECTOR_SIZE   0x1000

#include "embARC_toolchain.h"
#include "embARC_error.h"

/** flash data type */
typedef struct {
	uint32_t head;          /*!< 0x68656164 ='head' */
	uint32_t cpu_type;      /*!< = 0 - all images, reserved for future */
	uint32_t start;         /*!< start address of application image in spi flash */
	uint32_t size;          /*!< size of image in bytes */
	uint32_t ramaddr;       /*!< address of ram for loading image */
	uint32_t ramstart;      /*!< start address of application in RAM !!!! */
	uint32_t checksum;      /*!< checksum of all bytes in image */
} image_t;

/** w25qxx object type */
typedef struct {
	uint8_t spi_master;
	uint8_t cs;
	uint32_t spi_freq;

	uint32_t page_sz;
	uint32_t sector_sz;
	uint8_t write_buf[4 + FLASH_PAGE_SIZE];
} W25QXX_DEF, *W25QXX_DEF_PTR;

#define W25QXX_DEF(name, spi_master_id, cs_line, page, sector) \
	W25QXX_DEF __ ## name = {			       \
		.spi_master = spi_master_id,		       \
		.cs = cs_line,				       \
		.page_sz = page,			       \
		.sector_sz = sector			       \
	};						       \
	W25QXX_DEF_PTR name = &__ ## name		       \

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t w25qxx_read_status(W25QXX_DEF_PTR dev);
extern uint32_t w25qxx_read_id(W25QXX_DEF_PTR dev);
extern int32_t w25qxx_read(W25QXX_DEF_PTR dev, uint32_t address, uint32_t size, void *data);
extern int32_t w25qxx_write_enable(W25QXX_DEF_PTR dev);
extern int32_t w25qxx_erase(W25QXX_DEF_PTR dev, uint32_t address, uint32_t size);
extern int32_t w25qxx_write(W25QXX_DEF_PTR dev, uint32_t address, uint32_t size, const void *data);
extern int32_t w25qxx_wait_ready(W25QXX_DEF_PTR dev);
extern int32_t w25qxx_init(W25QXX_DEF_PTR dev, uint32_t freq);

#ifdef __cplusplus
}
#endif

#endif /* H_SPI_FLASH_W25QXX */
