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
 * @defgroup DEVICE_HAL_COMMON Common Device Layer Definitions
 * @ingroup DEVICE_HAL_DEF
 * @brief Common definitions for device layer (@ref dev_common.h)
 *
 * @{
 *
 * @file
 * @brief Header file to define common definitions for device layer
 * @details Here in this file provide definitions that need by other
 * devices in device layer
 */

#ifndef H_DEV_COMMON
#define H_DEV_COMMON

#include "embARC_toolchain.h"

/**
 * @defgroup DEVICE_HAL_COMMON_DEVSTATE	Common Device State
 * @ingroup DEVICE_HAL_COMMON
 * @brief Definitions for device state
 * @details Common types and macro definitions for device open/close,
 * device working good/error, used in @ref DEVICE_HAL_UART, @ref DEVICE_HAL_SPI,
 * @ref DEVICE_HAL_IIC, @ref DEVICE_HAL_GPIO
 * @{
 */
/*
 * macros for device open and close state
 */
#define DEV_CLOSED                  (0)         /*!< Indicate that device was closed */
#define DEV_OPENED                  (1)         /*!< Indicate that the device was opened */

/*
 * macros for device good and error state
 */
#define DEV_GOOD                    (0)         /*!< Indicate device is good */
#define DEV_ERROR                   (1)         /*!< Indicate device error */
/** @} */

/**
 * @defgroup DEVICE_HAL_COMMON_DEVMTHD Common Device Working Method
 * @ingroup DEVICE_HAL_COMMON
 * @brief Definitions for device working method(interrupt or poll)
 * @details Common macro definitions for working method,
 * interrupt or poll method,used in @ref DEVICE_HAL_UART, @ref DEVICE_HAL_SPI,
 * @ref DEVICE_HAL_IIC, @ref DEVICE_HAL_GPIO
 * @{
 */
/*
 * macros for device working method
 */
#define DEV_POLL_METHOD             (0)         /*!< Indicate that the device running in poll method */
#define DEV_INTERRUPT_METHOD        (1)         /*!< Indicate that the device running in interrupt method */
/** @} */

/**
 * @defgroup DEVICE_HAL_COMMON_DEVMODE Common Device Working Mode
 * @ingroup DEVICE_HAL_COMMON
 * @brief Definitions for device working mode(master or slave)
 * @details Common macro definitions for working mode, Master or Slave mode,
 * used in @ref DEVICE_HAL_IIC, @ref DEVICE_HAL_SPI.
 * @{
 */
/*
 * macros for device working mode
 */
#define DEV_MASTER_MODE             (0)         /*!< Indicate that the device working as master */
#define DEV_SLAVE_MODE              (1)         /*!< Indicate that the device working as slave */
/** @} */

/**
 * @defgroup DEVICE_HAL_COMMON_DEVSTATUS	Common Device Status
 * @ingroup DEVICE_HAL_COMMON
 * @brief Definitions for device status, 1 bit for 1 function
 * @{
 */
#define DEV_DISABLED                (0)         /*!< Bit 0 for device enabled state, disabled */
#define DEV_ENABLED                 (1 << 0)    /*!< Bit 0 for device enabled state, enabled */
#define DEV_IN_TX                   (1 << 1)    /*!< Bit 1 for device in transmit state */
#define DEV_IN_RX                   (1 << 2)    /*!< Bit 2 for device in receive state */
#define DEV_IN_XFER                 (1 << 3)    /*!< Bit 3 for device in transfer state */
#define DEV_IN_TX_ABRT              (1 << 4)    /*!< Bit 4 for device in transmit abort state */
#define DEV_IN_RX_ABRT              (1 << 5)    /*!< Bit 5 for device in receive abort state */
#define DEV_IN_XFER_ABRT            (1 << 6)    /*!< Bit 6 for device in transfer abort state */
/** @} */

/**
 * Invalid Interrupt Vector Number
 * When interrupt number is set to this value,
 * all interrupt related function in the device
 * driver source code shouldn't be called,
 * that means the interrupt processing maybe done
 * in outside of device driver, maybe done in the device
 * object instantiation
 */
#define DEV_INTNO_INVALID           (0xFFFFFFFF)

/**
 * @defgroup DEVICE_HAL_COMMON_DEFCMD Common Device Defining Command
 * @ingroup DEVICE_HAL_COMMON
 * @brief Definitions for defining command code
 * @details Common macro definitions to define command code,
 * in system code, use @ref DEV_SET_SYSCMD to define command code.
 * in user code, use @ref DEV_SET_USRCMD to define command code.
 * So that there will be no conflicts in system and user defined command code.
 * this used used in @ref DEVICE_HAL_UART, @ref DEVICE_HAL_SPI,
 * @ref DEVICE_HAL_IIC, @ref DEVICE_HAL_GPIO, and in user code
 * @{
 */
/*
 * macros for control command base
 */
#define DEV_SYS_CMDBSE              (0x00000000)                /*!< default system device control command base(defined by embARC) */
#define DEV_USR_CMDBSE              (0x80000000)                /*!< default user device control command base(defined by user) in user implementing */
#define DEV_SET_SYSCMD(cmd)         (DEV_SYS_CMDBSE | (cmd))    /*!< set device system control command */
#define DEV_SET_USRCMD(cmd)         (DEV_USR_CMDBSE | (cmd))    /*!< set device user control command */

#define CONV2VOID(param)            ((void *)(param))           /*!< convert param into void * type */
/** @} */

/**
 * Common Device Buffer Structure
 */
typedef struct {
	void *buf;      /*!< buffer pointer */
	uint32_t len;   /*!< buffer length in bytes */
	uint32_t ofs;   /*!< current offset in buffer */
} DEV_BUFFER;

/** Init device buffer */
#define DEV_BUFFER_INIT(devbuf, buffer, size)   { \
		(devbuf)->buf = (void *)(buffer); \
		(devbuf)->len = (uint32_t)(size); \
		(devbuf)->ofs = (uint32_t)(0);	  \
}

/**
 * Device callback function typedef.
 * This is usually used in device callback settings,
 * and @b ptr should be the device object pointer,
 * such as DEV_IIC * */
typedef void (*DEV_CALLBACK) (void *ptr);

/** @} */
#endif /* H_DEV_COMMON */
