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
#ifndef H_DEV_SDIO
#define H_DEV_SDIO

#include  "device/dev_common.h"

#define SDIO_CMD_SET_BUS_WIDTH  DEV_SET_SYSCMD(1)       /* set bus width */
#define SDIO_CMD_SET_BUS_FREQ   DEV_SET_SYSCMD(2)       /* set bus freq */

#define SDIO_OCR_BUSY           (0x80000000)
#define SDIO_OCR_HCS            (0x40000000)
#define SDIO_OCR_VOLTAGE_MASK   (0x00FF8000)

#define SDIO_RSP_PRESENT        (1 << 0)
#define SDIO_RSP_136            (1 << 1)        /* 136 bit response */
#define SDIO_RSP_CRC            (1 << 2)        /* expect valid crc */
#define SDIO_RSP_BUSY           (1 << 3)        /* card may send busy */
#define SDIO_RSP_OPCODE         (1 << 4)        /* response contains opcode */

#define SDIO_RSP_NONE   (0)
#define SDIO_RSP_R1     (SDIO_RSP_PRESENT | SDIO_RSP_CRC | SDIO_RSP_OPCODE)
#define SDIO_RSP_R1b    (SDIO_RSP_PRESENT | SDIO_RSP_CRC | SDIO_RSP_OPCODE | SDIO_RSP_BUSY)
#define SDIO_RSP_R2     (SDIO_RSP_PRESENT | SDIO_RSP_136 | SDIO_RSP_CRC)
#define SDIO_RSP_R3     (SDIO_RSP_PRESENT)
#define SDIO_RSP_R4     (SDIO_RSP_PRESENT)
#define SDIO_RSP_R5     (SDIO_RSP_PRESENT | SDIO_RSP_CRC | SDIO_RSP_OPCODE)
#define SDIO_RSP_R6     (SDIO_RSP_PRESENT | SDIO_RSP_CRC | SDIO_RSP_OPCODE)
#define SDIO_RSP_R7     (SDIO_RSP_PRESENT | SDIO_RSP_CRC | SDIO_RSP_OPCODE)

#define SDIO_STATUS_MASK_ERR        (~0x0206BF7F)
#define SDIO_STATUS_SWITCH_ERROR    (1 << 7)
#define SDIO_STATUS_RDY_FOR_DATA    (1 << 8)
#define SDIO_STATUS_CURR_STATE      (0xf << 9)
#define SDIO_STATUS_ERROR           (1 << 19)

#define SDIO_STATUS_STATE_PRG       (7 << 9)

#define SDIO_DATA_WRITE     1
#define SDIO_DATA_READ      2

typedef struct {
	uint16_t card;          /* card  numer */
	uint16_t cmdidx;        /* cmd index */
	uint32_t resp_type;     /* respone type */
	uint32_t arg;           /* cmd argument */
	uint32_t resp[4];       /* respone */
} SDIO_CMD, *SDIO_CMD_PTR;

typedef struct {
	union {
		uint8_t *in;            /* read */
		const uint8_t *out;     /* write */
	};
	uint32_t flags;                 /* r/w flags */
	uint32_t blkcnt;
	uint32_t blksz;
} SDIO_DATA, *SDIO_DATA_PTR;

typedef struct {
	uint16_t card;          /* card number */
	uint16_t cmd;           /* ctrl cmd */
} SDIO_CTRL_CMD, *SDIO_CTRL_CMD_PTR;

typedef struct dev_sdio_info {
	void *sdio_ctrl;    /* hardward specific data */
	uint32_t opn_cnt;
	uint32_t status;
	DEV_CALLBACK cb;        /* current operation callback */
	void *extra;            /*!< a extra pointer to get hook to applications which should not used by bsp developer */
} DEV_SDIO_INFO, *DEV_SDIO_INFO_PTR;

typedef struct {
	DEV_SDIO_INFO sdio_info;                                                        /*!< sdio device information */
	int32_t (*open)(uint32_t card);                                                 /*!< Open sdio device */
	int32_t (*close)(uint32_t card);                                                /*!< Close sdio device */
	int32_t (*control)(SDIO_CTRL_CMD_PTR ctrl_cmd, void *param);                    /*!< Control sdio device */
	int32_t (*cmd_poll)(SDIO_CMD_PTR cmd, SDIO_DATA_PTR data);
	int32_t (*cmd_int)(SDIO_CMD_PTR cmd, SDIO_DATA_PTR data, DEV_CALLBACK cb);      /* interrupt based operation */
	int32_t (*cd)(uint32_t card);
	int32_t (*wp)(uint32_t card);
} DEV_SDIO, *DEV_SDIO_PTR;

#ifdef __cplusplus
extern "C" {
#endif

extern DEV_SDIO_PTR sdio_get_dev(uint32_t sdio_id);

#ifdef __cplusplus
}
#endif
#endif /* H_DEV_SDIO */