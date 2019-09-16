/*!
 * \file      adc.h
 *
 * \brief     Generic ADC driver implementation
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
#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>
#include "gpio.h"

/*!
 * ADC object type definition
 */
typedef struct
{
    Gpio_t AdcInput;
}Adc_t;

/*!
 * \brief Initializes the ADC input
 *
 * \param [IN] obj  ADC object
 * \param [IN] scl  ADC input pin name to be used
 */
void AdcInit( Adc_t *obj, PinNames adcInput );

/*!
 * \brief DeInitializes the ADC
 *
 * \param [IN] obj  ADC object
 */
void AdcDeInit( Adc_t *obj );

/*!
 * \brief Read the analogue voltage value
 *
 * \param [IN] obj  ADC object
 * \param [IN] channel ADC channel
 * \retval value    Analogue pin value
 */
uint16_t AdcReadChannel( Adc_t *obj, uint32_t channel );

#endif // __ADC_H__
