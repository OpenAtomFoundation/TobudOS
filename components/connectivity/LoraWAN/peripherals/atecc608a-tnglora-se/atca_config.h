/* Cryptoauthlib Configuration File */
#ifndef ATCA_CONFIG_H
#define ATCA_CONFIG_H

/* Include HALS */
#define ATCA_HAL_I2C

/* Included device support */
#define ATCA_ATECC608A_SUPPORT

/* ATECC608A I2C bus configuration */ 
#define ATCA_HAL_ATECC608A_I2C_FREQUENCY 100000U
#define ATCA_HAL_ATECC608A_I2C_BUS_PINS 2U
#define ATCA_HAL_ATECC608A_I2C_ADDRESS 0x59U
#define ATCA_HAL_ATECC608A_I2C_RX_RETRIES 20
#define ATCA_HAL_ATECC608A_I2C_WAKEUP_DELAY 1500U

/* \brief How long to wait after an initial wake failure for the POST to
 *         complete.
 * If Power-on self test (POST) is enabled, the self test will run on waking
 * from sleep or during power-on, which delays the wake reply.
 */
#ifndef ATCA_POST_DELAY_MSEC
#define ATCA_POST_DELAY_MSEC 25
#endif

#endif