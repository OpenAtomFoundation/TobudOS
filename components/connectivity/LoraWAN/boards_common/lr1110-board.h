/*!
 * \file      lr1110-board.h
 *
 * \brief     Target board LR1110 driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#ifndef __LR1110_BOARD_H__
#define __LR1110_BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "lr1110.h"

/*!
 * \brief Initializes the radio I/Os pins interface
 */
void lr1110_board_init_io( const void* context );

/*!
 * \brief De-initializes the radio I/Os pins interface.
 *
 * \remark Useful when going in MCU low power modes
 */
void lr1110_board_deinit_io( const void* context );

/*!
 * \brief Initializes the radio debug pins.
 */
void lr1110_board_init_dbg_io( const void* context );

/*!
 * \brief Sets the radio output power.
 *
 * \param [IN] power Sets the RF output power
 */
void lr1110_board_set_rf_tx_power( const void* context, int8_t power );

/*!
 * \brief Gets the Defines the time required for the TCXO to wakeup [ms].
 *
 * \retval time Board TCXO wakeup time in ms.
 */
uint32_t lr1110_board_get_tcxo_wakeup_time( const void* context );

/*!
 * \brief Initializes the radio driver
 */
void lr1110_board_init( const void* context, lr1110_dio_irq_handler dio_irq );

#ifdef __cplusplus
}
#endif

#endif  // __LR1110_BOARD_H__
