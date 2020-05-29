/*!
 * \file      lr1110.h
 *
 * \brief     LR1110 driver implementation
 *
 * \copyright Revised BSD License, see file LICENSE.txt
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2019-2019 Semtech
 *
 * \endcode
 */

#ifndef __LR1110_H__
#define __LR1110_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "gpio.h"
#include "spi.h"
#include "radio.h"
#include "lr1110_hal.h"
#include "lr1110_radio_types.h"

/*!
 * \brief The type describing the modulation parameters for every packet types
 */
typedef struct
{
    lr1110_radio_packet_types_t packet_type;  //!< Packet to which the modulation parameters are referring to.
    struct
    {
        lr1110_radio_modulation_param_gfsk_t gfsk;
        lr1110_radio_modulation_param_lora_t lora;
    }modulation;  //!< Holds the modulation parameters structure
} lr1110_modulation_params_t;

/*!
 * \brief The type describing the packet parameters for every packet types
 */
typedef struct
{
    lr1110_radio_packet_types_t packet_type;  //!< Packet to which the packet parameters are referring to.
    struct
    {
        lr1110_radio_packet_param_gfsk_t gfsk;
        lr1110_radio_packet_param_lora_t lora;
    }packet;  //!< Holds the packet parameters structure
} lr1110_packet_params_t;

/*!
 * \brief Radio operating modes
 */
typedef enum lr1110_hal_operating_mode_e
{
    LR1110_HAL_OP_MODE_SLEEP = 0x00,  //! The radio is in sleep mode
    LR1110_HAL_OP_MODE_STDBY_RC,      //! The radio is in standby mode with RC oscillator
    LR1110_HAL_OP_MODE_STDBY_XOSC,    //! The radio is in standby mode with XOSC oscillator
    LR1110_HAL_OP_MODE_FS,            //! The radio is in frequency synthesis mode
    LR1110_HAL_OP_MODE_TX,            //! The radio is in transmit mode
    LR1110_HAL_OP_MODE_RX,            //! The radio is in receive single mode
    LR1110_HAL_OP_MODE_RX_C,          //! The radio is in receive continuous mode
    LR1110_HAL_OP_MODE_RX_DC,         //! The radio is in receive duty cycle mode
    LR1110_HAL_OP_MODE_CAD            //! The radio is in channel activity detection mode
} lr1110_hal_operating_mode_t;

/*!
 * Radio hardware and global parameters
 */
typedef struct lr1110_s
{
    Gpio_t                      reset;
    Gpio_t                      busy;
    Gpio_t                      dio_1;
    Spi_t                       spi;
    lr1110_hal_operating_mode_t op_mode;
    lr1110_modulation_params_t  modulation_params;
    lr1110_packet_params_t      packet_params;
} lr1110_t;

/*!
 * Hardware IO IRQ callback function definition
 */
typedef void ( *lr1110_dio_irq_handler )( void* context );

/*!
 * Get radio operating mode
 *
 * \remark Must be implemented by the upper layer
 *
 * \param [in] context Radio implementation parameters
 * \retval     op_mode Radio current operating mode
 */
lr1110_hal_operating_mode_t lr1110_hal_get_operating_mode( const void* context );

/*!
 * Set radio operating mode
 *
 * \remark Must be implemented by the upper layer
 *
 * \param [in] context Radio implementation parameters
 * \param [in] op_mode Radio operating mode
 */
void lr1110_hal_set_operating_mode( const void* context, const lr1110_hal_operating_mode_t op_mode );

#endif  // __LR1110_H__
