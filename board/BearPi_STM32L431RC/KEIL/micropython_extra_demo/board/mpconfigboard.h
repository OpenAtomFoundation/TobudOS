/* board specific config */
#define MICROPY_HW_BOARD_NAME                       "BearPi"
#define MICROPY_HW_MCU_NAME                         "STM32F431RC"

#define MICROPY_HW_UART_NUM                         4
#define MICROPY_HW_UART_REPL                        2
#define MICROPY_HW_SPI_NUM                          4

/* project specific config */
#define MICROPY_CONFIG_ROM_LEVEL                    (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

#define MP_USING_QSPI_FLASH
#define MP_USING_VFS
#define MP_USING_NETWORK
#define MP_USING_MACHINE_SPI
#define MP_USING_MACHINE_I2C
