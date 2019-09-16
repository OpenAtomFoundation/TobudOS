/*!
 * \file      board-config.h
 *
 * \brief     Board configuration
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
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 */
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

/*!
 * Defines the time required for the TCXO to wakeup [ms].
 */
#define BOARD_TCXO_WAKEUP_TIME                      5

/*!
 * Board MCU pins definitions
 */
#define RADIO_RESET                                 PB_11

#define RADIO_MOSI                                  PA_7
#define RADIO_MISO                                  PA_6
#define RADIO_SCLK                                  PA_5

#define RADIO_NSS                                   PA_4

#define RADIO_DIO_0                                 PB_10
#define RADIO_DIO_1                                 PB_2
#define RADIO_DIO_2                                 PB_0
#define RADIO_DIO_3                                 PB_1
#define RADIO_DIO_4                                 NC
#define RADIO_DIO_5                                 NC

#define RADIO_TCXO_POWER                            PA_8 // not used

#define RADIO_ANT_SWITCH_RX                         PA_1 // RADIO_ANT_SWITCH_RXTX1
#define RADIO_ANT_SWITCH_TX_BOOST                   PA_2 // RADIO_ANT_SWITCH_RXTX2
#define RADIO_ANT_SWITCH_TX_RFO                     PA_2

#define RADIO_BAND_SWHF                             PA_0
#define RADIO_BAND_SWLF                             PC_13

#define LED_1                                       PB_4
#define LED_2                                       PB_3
#define LED_3                                       PA_3
#define LED_4                                       PB_5

#define LED_GREEN                                   LED_1
#define LED_RED1                                    LED_2
#define LED_BLUE                                    LED_3
#define LED_RED2                                    LED_4

#define BTN_1                                       PA_9 // not used

#define OSC_LSE_IN                                  PC_14
#define OSC_LSE_OUT                                 PC_15

#define OSC_HSE_IN                                  PH_0
#define OSC_HSE_OUT                                 PH_1

#define SWCLK                                       PA_14
#define SWDAT                                       PA_13

#define I2C_SCL                                     PB_8
#define I2C_SDA                                     PB_9

#define UART_TX                                     PB_6 // PA_2
#define UART_RX                                     PB_7 // PA_3

// Debug pins definition.
#define RADIO_DBG_PIN_TX                            PB_13
#define RADIO_DBG_PIN_RX                            PB_14

#endif // __BOARD_CONFIG_H__
