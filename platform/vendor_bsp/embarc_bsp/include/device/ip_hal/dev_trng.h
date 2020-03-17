/* ------------------------------------------
 * Copyright (c) 2019, Synopsys, Inc. All rights reserved.

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
/**
 * @defgroup	DEVICE_HAL_TRNG		TRNG Device HAL Interface
 * @ingroup	DEVICE_HAL_DEF
 * @brief	Definitions for trng device hardware layer (@ref dev_trng.h)
 * @details	Provide interfaces for trng driver to implement
 *
 * @{
 */
#ifndef H_DEV_TRNG
#define H_DEV_TRNG

#include "device/dev_common.h"

#define TRNG_VERSION_NIST               // switch between DW_NIST_TRNG version and DW_TRNG version: define this value to choose NIST, otherwise undefine it.

#define TRNG_CMD_SET_RAND_128_BIT_OUTPUT            DEV_SET_SYSCMD(0)

#define TRNG_CMD_SET_RAND_256_BIT_OUTPUT            DEV_SET_SYSCMD(1)

#define TRNG_CMD_SET_RAND_RESEED_AUTO               DEV_SET_SYSCMD(2)

#define TRNG_CMD_SET_RAND_RESEED_NONCE              DEV_SET_SYSCMD(3)

#define TRNG_CMD_SET_IN_CB                          DEV_SET_SYSCMD(4)

#define TRNG_CMD_SET_OUT_CB                         DEV_SET_SYSCMD(5)

#define TRNG_CMD_SET_ERR_CB                         DEV_SET_SYSCMD(6)

typedef struct {
	DEV_CALLBACK in_cb;     /*!< trng data transmit success required bytes callback */
	DEV_CALLBACK out_cb;    /*!< trng data receive success required bytes callback */
	DEV_CALLBACK err_cb;    /*!< trng error callback */
} DEV_TRNG_CBS, *DEV_TRNG_CBS_PTR;

/**
 * @defgroup	DEVICE_HAL_TRNG_DEVSTRUCT	TRNG Device Interface Definition
 * @ingroup	DEVICE_HAL_TRNG
 * @brief	Structure definitions for trng device interface
 * @details	This structure will be used in user implemented code, which was called
 *     @ref DEVICE_IMPL "Device Driver Implement Layer" for trng to use in implementation code.
 *     Application developer should use the TRNG API provided here to access to TRNG devices.
 *     BSP developer should follow the API definition to implement TRNG device drivers.
 * @{
 */
/**
 * @brief	Structure definitions for trng information
 * @details	Informations about trng open count, working status,
 *     baudrate, trng registers and ctrl structure, trng dps format
 */
typedef struct {
	void *trng_ctrl;                /*!< trng control related pointer, implemented by bsp developer, and this should be set during trng object implementation */
	uint32_t opn_cnt;               /*!< trng open count, open it will increase 1, close it will decrease 1, 0 for close, >0 for open */
	uint32_t length;                /*!<  output length */
	uint32_t byte_generated;        /*!< count the bytes generated since last reseed, can be a reference and a reminder for next reseed operation */
	DEV_BUFFER in_buf;              /*!< input buffer via interrupt, this should be all zero for first open */
	DEV_BUFFER out_buf;             /*!< output buffer via interrupt, this should be all zero for first open */
	DEV_TRNG_CBS trng_cbs;
	void *extra;                    /*!< a extra pointer to get hook to applications which should not used by bsp developer,
	                                   this should be NULL for first open and you can @ref DEV_TRNG_INFO_SET_EXTRA_OBJECT "set"
	                                   or @ref DEV_TRNG_INFO_GET_EXTRA_OBJECT "get" the extra information pointer */
} DEV_TRNG_INFO, *DEV_TRNG_INFO_PTR;

/** Set extra information pointer of trng info */
#define DEV_TRNG_INFO_SET_EXTRA_OBJECT(trng_info_ptr, extra_info)   (trng_info_ptr)->extra = (void *)(extra_info)
/** Get extra information pointer of trng info */
#define DEV_TRNG_INFO_GET_EXTRA_OBJECT(trng_info_ptr)               ((trng_info_ptr)->extra)

typedef struct {
	DEV_TRNG_INFO trng_info;                                        /*!< device information */

	int32_t (*trng_open)(void);                                     /*!< Open trng device */
	int32_t (*trng_close)(void);                                    /*!< Close trng device */
	int32_t (*trng_control)(uint32_t ctrl_cmd, void *param);        /*!< Control trng device */
	int32_t (*trng_read)(uint32_t *data_buf);                       /*!< Read data from trng device(blocked) */
} DEV_TRNG, *DEV_TRNG_PTR;
/**
 * @fn		int32_t *DEV_TRNG::trng_open(void)
 * @details	Open trng device
 * @retval	E_OK	Open successfully without any issues
 * @retval	E_OPNED	If device was opened before with different parameters,
 *			then just increase the @ref DEV_TRNG_INFO::opn_cnt "opn_cnt" and return @ref E_OPNED
 * @retval	E_OBJ	Device object is not valid
 * @retval	E_PAR	Parameter is not valid
 * @retval	E_NOSPT	Open settings are not supported
 */

/**
 * @fn		int32_t *DEV_TRNG::trng_close(void)
 * @details	Close an trng device, just decrease the @ref DEV_TRNG_INFO::opn_cnt "opn_cnt",
 *      if @ref DEV_TRNG_INFO::opn_cnt "opn_cnt" equals 0, then close the device
 * @retval	E_OK	Close successfully without any issues(including scenario that device is already closed)
 * @retval	E_OPNED	Device is still opened, the device @ref DEV_TRNG_INFO::opn_cnt "opn_cnt" decreased by 1
 * @retval	E_OBJ	Device object is not valid
 */

/**
 * @fn		int32_t *DEV_TRNG::trng_control(uint32_t ch, uint32_t cmd, void *param2)
 * @details	Control a trng device [channel number: ch] by ctrl_cmd, with passed param.
 * @param[in]		ch			Channel number of trng to control, must >= 0
 * @param[in]		ctrl_cmd	Control command, to change or get some thing related to trng
 * @param[in,out]	param		Parameters that maybe argument of the command, or return values of the command
 * @retval	E_OK	Control device successfully
 * @retval	E_CLSED	Device is not opened
 * @retval	E_OBJ	Device object is not valid or not exists
 * @retval	E_PAR	Parameter is not valid for current control command
 * @retval	E_SYS	Control device failed, due to hardware issues, such as device is disabled
 * @retval	E_CTX	Control device failed, due to different reasons like in transfer state
 * @retval	E_NOSPT	Control command is not supported or not valid
 */

/**
 * @fn		int32_t *DEV_TRNG::trng_read(uint32_t *data_buf)
 * @details	Read the configuration of trng.
 * @param[out]	data_buf	pointer to data buffer read from trng, must not be NULL
 * @retval	E_OK	Read device successfully
 * @retval	E_OBJ	Device object is not valid or not exists
 * @retval	E_PAR	Parameter is not valid
 * @retval	E_SYS	Can't receive data from hardware due to hardware issues, such as device is disabled
 */
/** @} */

/** @} */
#endif /* H_DEV_TRNG */
