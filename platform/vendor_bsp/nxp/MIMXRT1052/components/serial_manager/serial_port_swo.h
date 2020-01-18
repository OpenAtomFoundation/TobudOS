/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __SERIAL_PORT_SWO_H__
#define __SERIAL_PORT_SWO_H__

/*!
 * @addtogroup serial_port_swo
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief serial port swo handle size*/
#define SERIAL_PORT_SWO_HANDLE_SIZE (12U)

/*! @brief serial port swo protocol*/
typedef enum _serial_port_swo_protocol
{
    kSerialManager_SwoProtocolManchester = 1U, /*!< SWO Manchester protocol */
    kSerialManager_SwoProtocolNrz        = 2U, /*!< SWO UART/NRZ protocol */
} serial_port_swo_protocol_t;

/*! @brief serial port swo config struct*/
typedef struct _serial_port_swo_config
{
    uint32_t clockRate;                  /*!< clock rate */
    uint32_t baudRate;                   /*!< baud rate */
    uint32_t port;                       /*!< Port used to transfer data */
    serial_port_swo_protocol_t protocol; /*!< SWO protocol */
} serial_port_swo_config_t;
/*! @} */
#endif /* __SERIAL_PORT_SWO_H__ */
