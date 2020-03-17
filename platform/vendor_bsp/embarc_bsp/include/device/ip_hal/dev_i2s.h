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

/**
 * @defgroup	DEVICE_HAL_I2S		I2S Device HAL Interface
 * @ingroup	DEVICE_HAL_DEF
 * @brief	Definitions for I2S device hardware layer (@ref dev_i2s.h)
 * @details	Provide unified APIs for I2S driver to implement.
 *
 * @{
 *
 * @file
 * @brief	Definitions for I2S device hardware layer
 * @details	Provide common definitions for I2S device,
 * 	then software developer can develop I2S driver
 * 	following this definitions, and the applications
 * 	can directly call this definition to realize functions
 *
 */

#ifndef H_DEV_I2S
#define H_DEV_I2S

#include "device/dev_common.h"

/**
 * @defgroup	DEVICE_HAL_I2S_CFG	I2S Related Configurations
 * @ingroup	DEVICE_HAL_I2S
 * @brief	Type and macro definitions for I2S device
 * @{
 */

/**
 * @defgroup	I2S_AUD_DATA_RES	I2S Audio Data Resolution
 * @ingroup	DEVICE_HAL_I2S_CFG
 * @brief	Definitions for I2S audio data resolution
 * @{
 */
typedef enum {
	I2S_AUD_DATA_NA         = 0,    /*!< N/A data resolution */
	I2S_AUD_DATA_12B        = 1,    /*!< 12 bits audio data resolution */
	I2S_AUD_DATA_16B        = 2,    /*!< 16 bits audio data resolution */
	I2S_AUD_DATA_20B        = 3,    /*!< 20 bits audio data resolution */
	I2S_AUD_DATA_24B        = 4,    /*!< 24 bits audio data resolution */
	I2S_AUD_DATA_32B        = 5     /*!< 32 bits audio data resolution */
} I2S_AUD_DATA_RES;
/** @} */

/**
 * @defgroup	I2S_AUD_SAMPLE_RATE	I2S Audio Sampling Rate
 * @ingroup	DEVICE_HAL_I2S_CFG
 * @brief	Definitions for I2S audio sampling rate
 * @{
 */
typedef enum {
	I2S_AUD_SR_NA           = 0,    /*!< N/A audio sample rate */
	I2S_AUD_SR_16K          = 1,    /*!< 16KHz audio sample rate */
	I2S_AUD_SR_32K          = 2,    /*!< 32KHz audio sample rate */
	I2S_AUD_SR_44_1K        = 3,    /*!< 44.1KHz audio sample rate */
	I2S_AUD_SR_48K          = 4,    /*!< 48KHz audio sample rate */
	I2S_AUD_SR_96K          = 5,    /*!< 96KHz audio sample rate */
	I2S_AUD_SR_192K         = 6     /*!< 192KHz audio sample rate */
} I2S_AUD_SAMPLE_RATE;
/** @} */

/**
 * @defgroup	I2S_DEVICE_TYPE		I2S Device Type
 * @ingroup	DEVICE_HAL_I2S_CFG
 * @brief	Definitions for I2S device type
 * @{
 */
typedef enum {
	I2S_DEVICE_TRANSMITTER  = 0,    /*!< I2S TX device */
	I2S_DEVICE_RECEIVER     = 1     /*!< I2S RX device */
} I2S_DEVICE_TYPE;
/** @} */

/**
 * @defgroup	I2S_WORKING_STATE	I2S Working State
 * @ingroup	DEVICE_HAL_I2S_CFG
 * @brief	Definitions for I2S working state
 * @{
 */
typedef enum {
	I2S_FREE        = 0,    /*!< Currently in I2S device free state */
	I2S_IN_TX       = 1,    /*!< Currently in I2S master transmit state */
	I2S_IN_RX       = 2     /*!< Currently in I2S master receive state */
} I2S_WORKING_STATE;
/** @} */

/**
 * @defgroup	I2S_ERROR_STATE		I2S Error State
 * @ingroup	DEVICE_HAL_I2S_CFG
 * @brief	Definitions for I2S error state
 * @{
 */
typedef enum {
	I2S_ERR_NONE            = 0,    /*!< Currently in I2S device free state */
	I2S_ERR_RX_OVERRUN      = 1,    /*!< RX FIFO data overrun */
	I2S_ERR_TX_OVERRUN      = 2,    /*!< TX FIFO data overrun */
	I2S_ERR_UNDEF           = 3     /*!< Undefined error cases */
} I2S_ERROR_STATE;
/** @} */

/** @} */

/**
 * @defgroup	DEVICE_HAL_I2S_CTRLCMD		I2S Device Control Commands
 * @ingroup	DEVICE_HAL_I2S
 * @brief	Definitions for I2S control command, used in @ref DEV_I2S::i2s_control "I2S Control"
 * @details	These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - @ref E_OK,	Control device successfully
 *   - @ref E_CLSED,	Device is not opened
 *   - @ref E_OBJ,	Device object is not valid or not exists
 *   - @ref E_PAR,	Parameter is not valid for current control command
 *   - @ref E_SYS,	Control device failed, due to hardware issues such as device is disabled
 *   - @ref E_CTX,	Control device failed, due to different reasons like in transfer state
 *   - @ref E_NOSPT,	Control command is not supported or not valid
 * @{
 */

/** Define I2S control commands for common usage */
#define DEV_SET_I2S_SYSCMD(cmd)     DEV_SET_SYSCMD((cmd))
/** Define I2S control commands for master usage */
#define DEV_SET_I2S_MST_SYSCMD(cmd) DEV_SET_SYSCMD(0x00010000 | (cmd))

/* ++++ Common commands for SPI Device ++++ */
/**
 * Get @ref DEV_I2S_INFO::status "current device status"
 * - Param type : uint32_t *
 * - Param usage : store result of current status
 * - Return value explanation :
 */
#define I2S_CMD_GET_STATUS          DEV_SET_I2S_SYSCMD(0)
/**
 * Set @ref DEV_I2S_CBS::tx_cb "I2S transmit success callback" function
 * when all required bytes are transmitted for interrupt method
 * - Param type : @ref DEV_CALLBACK * or NULL
 * - Param usage : transmit success callback function for I2S
 * - Return value explanation :
 */
#define I2S_CMD_SET_TXCB            DEV_SET_I2S_SYSCMD(1)
/**
 * Set @ref DEV_I2S_CBS::rx_cb "I2S receive success callback" function
 * when all required bytes are received for interrupt method
 * - Param type : @ref DEV_CALLBACK * or NULL
 * - Param usage : receive success callback function for I2S
 * - Return value explanation :
 */
#define I2S_CMD_SET_RXCB            DEV_SET_I2S_SYSCMD(2)
/**
 * Set @ref DEV_I2S_CBS::err_cb "I2S transfer error callback" function
 * when something error happened for interrupt method
 * - Param type : @ref DEV_CALLBACK * or NULL
 * - Param usage : transfer error callback function for I2S
 * - Return value explanation :
 */
#define I2S_CMD_SET_ERRCB           DEV_SET_I2S_SYSCMD(3)
/**
 * Enable or disable transmit interrupt
 * - Param type : uint32_t
 * - Param usage : enable(none-zero) or disable(zero) flag
 * - Return value explanation :
 */
#define I2S_CMD_SET_TXINT           DEV_SET_I2S_SYSCMD(4)
/**
 * Enable or disable receive interrupt
 * - Param type : uint32_t
 * - Param usage : enable(none-zero) or disable(zero) flag
 * - Return value explanation :
 */
#define I2S_CMD_SET_RXINT           DEV_SET_I2S_SYSCMD(5)
/**
 * Do a software reset for I2S device, it will stop current transfer,
 * and clear error state and bring device to normal state, set next condition to STOP
 * - Param type : NULL
 * - Param usage :
 * - Return value explanation :
 */
#define I2S_CMD_RESET               DEV_SET_I2S_SYSCMD(6)
/**
 * Flush I2S device transmit buffer or fifo
 * - Param type : NULL
 * - Param usage :
 * - Return value explanation :
 */
#define I2S_CMD_FLUSH_TX            DEV_SET_I2S_SYSCMD(7)
/**
 * Flush I2S device receive buffer or fifo
 * - Param type : NULL
 * - Param usage :
 * - Return value explanation :
 */
#define I2S_CMD_FLUSH_RX            DEV_SET_I2S_SYSCMD(8)
/**
 * Enable I2S device
 * - Param type : Device channel nummber
 * - Param usage : Specified device channel number
 * - Return value explanation :
 */
#define I2S_CMD_ENA_DEV             DEV_SET_I2S_SYSCMD(9)
/**
 * Disable I2S device, when device is disabled,
 * only @ref I2S_CMD_ENA_DEV, @ref I2S_CMD_DIS_DEV,
 * @ref I2S_CMD_GET_STATUS and @ref I2S_CMD_RESET
 * commands can be executed, other commands will return @ref E_SYS
 * - Param type : Device channel nummber
 * - Param usage : Specified device channel number
 * - Return value explanation :
 */
#define I2S_CMD_DIS_DEV             DEV_SET_I2S_SYSCMD(10)
/**
 * Set buffer for interrupt transmit, and it will set @ref DEV_I2S_INFO::tx_buf "tx_buf".
 * - I2S transmitter command @n
 *    If setting TX buffer to NULL, when tx interrupt is enabled and entered into tx interrupt,
 * it will automatically disable the tx interrupt, so when you want to transfer something, you need to set the
 * tx buffer to Non-NULL and enable tx interrupt, when the tx buffer is sent, it will disable the tx interrupt
 * and call tx callback function if available.
 * - Param type : DEV_BUFFER * or NULL
 * - Param usage : buffer structure pointer, if param is NULL, then it will set tx_buf to NULL
 * - Return value explanation :
 */
#define I2S_CMD_SET_TXINT_BUF       DEV_SET_I2S_SYSCMD(11)
/**
 * Set buffer for interrupt receive, and it will set @ref DEV_I2S_INFO::rx_buf "rx_buf"
 * - I2S receiver command @n
 *   Similiar to @ref I2S_CMD_SET_TXINT_BUF
 * - Param type : DEV_BUFFER * or NULL
 * - Param usage : buffer structure pointer, if param is NULL, then it will set rx_buf to NULL
 * - Return value explanation :
 */
#define I2S_CMD_SET_RXINT_BUF       DEV_SET_I2S_SYSCMD(12)

/**
 * Set buffer empty threshold in the TX FIFO, these bit program the empty trigger level in the TX FIFO
 * - I2S transmitter command @n
 *   These command is used to set trigger level in the TX FIFO at which empty threshold reached flag/interrupt
 *   is generated.
 * - Param type : uint32_t
 * - Param usage : Specified trigger level value (16b) + channel (16b)
 * - Return value explanation :
 */
#define I2S_CMD_SET_TXCHET_BUF      DEV_SET_I2S_SYSCMD(13)

/**
 * Set buffer available data threshold in the TX FIFO, these bit program the trigger level in the RX FIFO
 * - I2S receiver command @n
 *   These command is used to set trigger level in the RX FIFO at which received data available flag/interrupt
 *   is generated.
 * - Param type : uint32_t
 * - Param usage : Specified trigger level value  (16b) + channel (16b)
 * - Return value explanation :
 */
#define I2S_CMD_SET_RXCHDT_BUF      DEV_SET_I2S_SYSCMD(14)

/* ++++ Master only commands for I2S Device ++++ */
/**
 * Enable or disable clock for master
 * - Param type : uint32_t
 * - Param usage : enable(none-zero) or disable(zero) flag
 * - Return value explanation :
 */
#define I2S_CMD_MST_SET_CLK         DEV_SET_I2S_MST_SYSCMD(0)
/**
 * Program the number of sclk cycles for word select line stays
 * - Param type : uint32_t
 * - Param usage : clock cycles
 * - Return value explanation :
 */
#define I2S_CMD_MST_SET_WSS         DEV_SET_I2S_MST_SYSCMD(1)
/**
 * Program the number of gating of sclk cycles for word select line stays
 * - Param type : uint32_t
 * - Param usage : gating of sclk cycles
 * - Return value explanation :
 */
#define I2S_CMD_MST_SET_SCLKG       DEV_SET_I2S_MST_SYSCMD(2)

/** @} */

/**
 * @defgroup	DEVICE_HAL_I2S_CALLBACK	I2S Interrupt Callback Functions
 * @ingroup	DEVICE_HAL_I2S
 * @brief	Structure definition for I2S device callback
 * @{
 */
typedef struct {
	DEV_CALLBACK tx_cb;     /*!< I2S data transmit success required bytes callback */
	DEV_CALLBACK rx_cb;     /*!< I2S data receive success required bytes callback */
	DEV_CALLBACK err_cb;    /*!< I2S error callback */
} DEV_I2S_CBS, *DEV_I2S_CBS_PTR;
/** @} */

/**
 * @defgroup	DEVICE_HAL_I2S_DEVSTRUCT	I2S Device Structure
 * @ingroup	DEVICE_HAL_I2S
 * @brief	Definitions for I2S device structure.
 * @details	This structure will be used in user implemented code, which was called
 *     Device Driver Implement Layer for I2S to realize in user code.
 * @{
 */
/**
 * @brief	I2S information struct definition
 * @details	Informations about I2S open state, working state,
 * 	baurate, I2S registers, working method, interrupt number
 */
typedef struct {
	void *i2s_ctrl;         /*!< I2S control related pointer, implemented by BSP developer, and this should be set during i2s object implementation */
	void *i2s_config;       /*!< I2S device configuration, implemented by BSP developer, and this shoud be set in object implementation */
	uint32_t opn_cnt;       /*!< I2S open count, open it will increase 1, close it will decrease 1, 0 for close, >0 for open */
	uint32_t status;        /*!< current working status, refer to @ref DEVICE_HAL_COMMON_DEVSTATUS, this should be @ref DEV_ENABLED for first open */
	uint32_t device;        /*!< device type, refer to @ref I2S_DEVICE_TYPE, this should be set for object initialization */
	uint32_t mode;          /*!< current working mode, which can be @ref DEV_MASTER_MODE "master mode" or @ref DEV_SLAVE_MODE "slave mode" */
	uint32_t cur_state;     /*!< @ref I2S_WORKING_STATE "current working state for I2S device", this should be @ref I2S_FREE for first open */
	uint32_t err_state;     /*!< @ref I2S_ERROR_STATE "current error state for I2S device", this should be @ref I2S_ERR_NONE for first open */
	DEV_BUFFER tx_buf;      /*!< transmit buffer via interrupt, this should be all zero for first open */
	DEV_BUFFER rx_buf;      /*!< receive buffer via interrupt, this should be all zero for first open */
	DEV_I2S_CBS i2s_cbs;    /*!< I2S callbacks, for both master and slave mode, this should be all NULL for first open */
	void *extra;            /*!< a extra pointer to get hook to applications which should not used by BSP developer,
	                           this should be NULL for first open and you can @ref DEV_I2S_INFO_SET_EXTRA_OBJECT "set"
	                           or @ref DEV_I2S_INFO_GET_EXTRA_OBJECT "get" the extra information pointer */
} DEV_I2S_INFO, *DEV_I2S_INFO_PTR;

/** Set extra information pointer of I2S */
#define DEV_I2S_INFO_SET_EXTRA_OBJECT(i2s_info_ptr, extra_info)     (i2s_info_ptr)->extra = (void *)(extra_info)
/** Get extra information pointer of I2S */
#define DEV_I2S_INFO_GET_EXTRA_OBJECT(i2s_info_ptr)                 ((i2s_info_ptr)->extra)

/**
 * @brief	Definition for I2S device interface
 * @details	Define I2S device interface, like I2S information structure,
 * 		fuctions to get I2S info, open/close/control I2S, send/receive data by I2S
 * @note	All this details are implemented by user in user porting code
 */
typedef struct {
	DEV_I2S_INFO i2s_info;                                  /*!< I2S device information */
	int32_t (*i2s_open)(uint32_t mode, uint32_t param);     /*!< open I2S device in master/slave mode, \
	                                                           when in master mode, param stands for speed mode, \
	                                                           when in slave mode, param stands for slave address */
	int32_t (*i2s_close)(void);                             /*!< close I2S device */
	int32_t (*i2s_control)(uint32_t ctrl_cmd, void *param); /*!< control i2s device */
	int32_t (*i2s_write)(const void *data, uint32_t len);   /*!< send data by i2s TX device (blocking method) */
	int32_t (*i2s_read)(void *data, uint32_t len);          /*!< read data from i2s RX device (blocking method) */
} DEV_I2S, *DEV_I2S_PTR;

/**
 * @fn		int32_t *DEV_I2S::i2s_open(uint32_t mode, uint32_t param)
 * @details	Open an I2S device with selected mode (master or slave) with defined param
 * @param[in]	mode	Working mode (@ref DEV_MASTER_MODE "master" or @ref DEV_SLAVE_MODE "slave")
 * @param[in]	param	Device type (@ref I2S_DEVICE_TRANSMITTER "transmitter" OR @ref I2S_DEVICE_RECEIVER "receiver")
 * @retval	E_OK	Open successfully without any issues
 * @retval	E_OPNED	Device was opened before
 * @retval	E_OBJ	Device object is not valid
 * @retval	E_SYS	Device initialized parameters are different with parameters in IP
 * @retval	E_PAR	Parameter is not valid
 * @retval	E_NOSPT	Open settings are not supported
 */

/**
 * @fn		int32_t *DEV_I2S::i2s_close(void)
 * @details	close an I2S device, just decrease the @ref DEV_I2S_INFO::opn_cnt "opn_cnt",
 *      if @ref DEV_I2S_INFO::opn_cnt "opn_cnt" equals 0, then close the device
 * @retval	E_OK	Close successfully without any issues(including scenario that device is already closed)
 * @retval	E_OPNED	Device is still opened, the device @ref DEV_I2S_INFO::opn_cnt "opn_cnt" decreased by 1
 * @retval	E_OBJ	Device object is not valid
 */

/**
 * @fn		int32_t *DEV_I2S::i2s_control(uint32_t ctrl_cmd, void *param)
 * @details	Control an I2S device by ctrl_cmd, with passed param.
 * 	you can control I2S device using predefined I2S control commands defined using @ref DEV_SET_SYSCMD
 * 	(which must be implemented by bsp developer), @ref DEVICE_HAL_I2S_CTRLCMD "command details".
 * 	And you can also control I2S device using your own specified commands defined using @ref DEV_SET_USRCMD,
 * 	but these specified commands should be defined in your own I2S device driver implementation.
 * @param[in]		ctrl_cmd	@ref DEVICE_HAL_I2S_CTRLCMD "control command", to change or get some thing related to I2S
 * @param[in,out]	param		Parameters that maybe argument of the command,
 * 					or return values of the command, must not be NULL
 * @retval	E_OK	Control device successfully
 * @retval	E_CLSED	Device is not opened
 * @retval	E_OBJ	Device object is not valid or not exists
 * @retval	E_PAR	Parameter is not valid for current control command
 * @retval	E_SYS	Control device failed, due to hardware issues, such as device is disabled
 * @retval	E_CTX	Control device failed, due to different reasons like in transfer state
 * @retval	E_NOSPT	Control command is not supported or not valid
 */

/**
 * @fn		int32_t *DEV_I2S::i2s_write(const void *data, uint32_t len)
 * @details	Send data through I2S with defined len to RX device.
 * @param[in]	data	Pointer to data need to send by I2S
 * @param[in]	len	Length of data to be sent
 * @retval	> 0	Byte count that was successfully sent for poll method,
 * 			it might can't send that much due to @ref DEV_I2S_INFO::err_state "different error state".
 * @retval	E_OBJ	Device object is not valid or not exists
 * @retval	E_PAR	Parameter is not valid
 * @retval	E_CTX	Device is still in transfer state
 * @retval	E_SYS	Can't write data to hardware due to hardware issues, such as device is disabled
 */

/**
 * @fn		int32_t *DEV_I2S::i2s_read(void *data, uint32_t len)
 * @details	Receive data of defined len through I2S from TX device.
 * @param[out]	data	Pointer to data need to received by I2S
 * @param[in]	len	Length of data to be received
 * @retval	> 0	Byte count that was successfully received for poll method,
 * 			it might can't send that much due to @ref DEV_I2S_INFO::err_state "different error state".
 * @retval	E_OBJ	Device object is not valid or not exists
 * @retval	E_CTX	Device is still in transfer state
 * @retval	E_PAR	Parameter is not valid
 * @retval	E_SYS	Can't receive data from hardware due to hardware issues, such as device is disabled
 */
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	Get an @ref DEV_I2S "i2s device" by i2s device id.
 * 	For how to use i2s device hal refer to @ref DEVICE_HAL_I2S_DEVSTRUCT "Functions in i2s device structure"
 * @param[in]	i2s_id	ID of i2s, defined by user
 * @retval	!NULL	Pointer to an @ref DEV_I2S "i2s device structure"
 * @retval	NULL	Failed to find the i2s device by i2s_id
 * @note	Need to implemented by user in user code
 */
extern DEV_I2S_PTR i2s_get_dev(int32_t i2s_id);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* H_DEV_I2S */
