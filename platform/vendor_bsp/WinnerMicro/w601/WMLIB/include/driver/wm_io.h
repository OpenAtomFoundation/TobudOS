/**
 * @file    wm_io.h
 *
 * @brief   IO Driver Module
 *
 * @author  lilm
 *
 * @copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_IO_H
#define WM_IO_H

#include "wm_type_def.h"

#define TLS_IO_AB_OFFSET  (0x40011200 - 0x40010C00)

/** io name */
enum tls_io_name {
    WM_IO_PA_00 = 0,    /**< gpio a0 */
    WM_IO_PA_01,        /**< gpio a1 */
    WM_IO_PA_02,        /**< gpio a2 */
    WM_IO_PA_03,        /**< gpio a3 */
    WM_IO_PA_04,        /**< gpio a4 */
    WM_IO_PA_05,        /**< gpio a5 */
    WM_IO_PA_06,        /**< gpio a6 */
    WM_IO_PA_07,        /**< gpio a7 */
    WM_IO_PA_08,        /**< gpio a8 */
    WM_IO_PA_09,        /**< gpio a9 */
    WM_IO_PA_10,        /**< gpio a10 */
    WM_IO_PA_11,        /**< gpio a11 */
    WM_IO_PA_12,        /**< gpio a12 */
    WM_IO_PA_13,        /**< gpio a13 */
    WM_IO_PA_14,        /**< gpio a14 */
    WM_IO_PA_15,        /**< gpio a15 */

    WM_IO_PB_00,        /**< gpio b0 */
    WM_IO_PB_01,        /**< gpio b1 */
    WM_IO_PB_02,        /**< gpio b2 */
    WM_IO_PB_03,        /**< gpio b3 */
    WM_IO_PB_04,        /**< gpio b4 */
    WM_IO_PB_05,        /**< gpio b5 */
    WM_IO_PB_06,        /**< gpio b6 */
    WM_IO_PB_07,        /**< gpio b7 */
    WM_IO_PB_08,        /**< gpio b8 */
    WM_IO_PB_09,        /**< gpio b9 */
    WM_IO_PB_10,        /**< gpio b10 */
    WM_IO_PB_11,        /**< gpio b11 */
    WM_IO_PB_12,        /**< gpio b12 */
    WM_IO_PB_13,        /**< gpio b13 */
    WM_IO_PB_14,        /**< gpio b14 */
    WM_IO_PB_15,        /**< gpio b15 */
    WM_IO_PB_16,        /**< gpio b16 */
    WM_IO_PB_17,        /**< gpio b17 */
    WM_IO_PB_18,        /**< gpio b18 */
    WM_IO_PB_19,        /**< gpio b19 */
    WM_IO_PB_20,        /**< gpio b20 */
    WM_IO_PB_21,        /**< gpio b21 */
    WM_IO_PB_22,        /**< gpio b22 */
    WM_IO_PB_23,        /**< gpio b23 */
    WM_IO_PB_24,        /**< gpio b24 */
    WM_IO_PB_25,        /**< gpio b25 */
    WM_IO_PB_26,        /**< gpio b26 */
    WM_IO_PB_27,        /**< gpio b27 */
    WM_IO_PB_28,        /**< gpio b28 */
    WM_IO_PB_29,        /**< gpio b29 */
    WM_IO_PB_30,        /**< gpio b30 */
    WM_IO_PB_31			/**< gpio b31 */
};

/** option 1 of the io */
#define WM_IO_OPTION1               1
/** option 2 of the io */
#define WM_IO_OPTION2               2
/** option 3 of the io */
#define WM_IO_OPTION3               3
/** option 4 of the io */
#define WM_IO_OPTION4               4
/** option 5 of the io */
#define WM_IO_OPTION5               5
/** option 6 of the io */
#define WM_IO_OPTION6               6


/* io option1 */
#define WM_IO_OPT1_I2C_DAT          WM_IO_OPTION1
#define WM_IO_OPT1_PWM1             WM_IO_OPTION1
#define WM_IO_OPT1_PWM2             WM_IO_OPTION1
#define WM_IO_OPT1_PWM3             WM_IO_OPTION1
#define WM_IO_OPT1_PWM4             WM_IO_OPTION1
#define WM_IO_OPT1_PWM5             WM_IO_OPTION1
#define WM_IO_OPT1_UART0_RXD        WM_IO_OPTION1
#define WM_IO_OPT1_UART0_TXD        WM_IO_OPTION1
#define WM_IO_OPT1_PWM_BRAKE        WM_IO_OPTION1
#define WM_IO_OPT1_I2S_M_EXTCLK     WM_IO_OPTION1
#define WM_IO_OPT1_SPI_M_DO         WM_IO_OPTION1
#define WM_IO_OPT1_SPI_M_DI         WM_IO_OPTION1
#define WM_IO_OPT1_SPI_M_CS         WM_IO_OPTION1
#define WM_IO_OPT1_SPI_M_CK         WM_IO_OPTION1
#define WM_IO_OPT1_I2S_S_RL         WM_IO_OPTION1
#define WM_IO_OPT1_I2S_S_SCL        WM_IO_OPTION1
#define WM_IO_OPT1_I2S_S_SDA        WM_IO_OPTION1
#define WM_IO_OPT1_I2S_M_RL         WM_IO_OPTION1
#define WM_IO_OPT1_I2S_M_SCL        WM_IO_OPTION1
#define WM_IO_OPT1_I2S_M_SDA        WM_IO_OPTION1
#define WM_IO_OPT1_JTAG_RST         WM_IO_OPTION1
#define WM_IO_OPT1_JTAG_TDO         WM_IO_OPTION1
#define WM_IO_OPT1_JTAG_TDI         WM_IO_OPTION1
#define WM_IO_OPT1_JTAG_TCK_SWDCK   WM_IO_OPTION1
#define WM_IO_OPT1_JTAG_TMS_SWDAT   WM_IO_OPTION1
#define WM_IO_OPT1_UART1_RXD        WM_IO_OPTION1
#define WM_IO_OPT1_UART1_TXD        WM_IO_OPTION1
#define WM_IO_OPT1_UART1_RTS        WM_IO_OPTION1
#define WM_IO_OPT1_UART1_CTS        WM_IO_OPTION1
#define WM_IO_OPT1_SDIO_DAT         WM_IO_OPTION1

/* io option2 */
#define WM_IO_OPT2_PWM1             WM_IO_OPTION2
#define WM_IO_OPT2_PWM2             WM_IO_OPTION2
#define WM_IO_OPT2_PWM3             WM_IO_OPTION2
#define WM_IO_OPT2_PWM4             WM_IO_OPTION2
#define WM_IO_OPT2_PWM5             WM_IO_OPTION2
#define WM_IO_OPT2_SPI_M_DO         WM_IO_OPTION2
#define WM_IO_OPT2_SPI_M_DI         WM_IO_OPTION2
#define WM_IO_OPT2_SPI_M_CS         WM_IO_OPTION2
#define WM_IO_OPT2_SPI_M_CK         WM_IO_OPTION2
#define WM_IO_OPT2_I2C_SCL          WM_IO_OPTION2
#define WM_IO_OPT2_I2S_M_EXTCLK     WM_IO_OPTION2
#define WM_IO_OPT2_UART1_RXD        WM_IO_OPTION2
#define WM_IO_OPT2_UART1_TXD        WM_IO_OPTION2
#define WM_IO_OPT2_UART1_RTS        WM_IO_OPTION2
#define WM_IO_OPT2_UART1_CTS        WM_IO_OPTION2
#define WM_IO_OPT2_I2C_DAT          WM_IO_OPTION2
#define WM_IO_OPT2_PWM_BRAKE        WM_IO_OPTION2
#define WM_IO_OPT2_UART0_RTS        WM_IO_OPTION2
#define WM_IO_OPT2_UART0_CTS        WM_IO_OPTION2
#define WM_IO_OPT2_SDIO_DAT         WM_IO_OPTION2
#define WM_IO_OPT2_HSPI_CK          WM_IO_OPTION2
#define WM_IO_OPT2_HSPI_INT         WM_IO_OPTION2
#define WM_IO_OPT2_HSPI_CS          WM_IO_OPTION2
#define WM_IO_OPT2_HSPI_DI          WM_IO_OPTION2
#define WM_IO_OPT2_HSPI_DO          WM_IO_OPTION2

/* io option3 */
#define WM_IO_OPT3_UART0_RXD        WM_IO_OPTION3
#define WM_IO_OPT3_UART0_TXD        WM_IO_OPTION3
#define WM_IO_OPT3_UART0_RTS        WM_IO_OPTION3
#define WM_IO_OPT3_UART0_CTS        WM_IO_OPTION3
#define WM_IO_OPT3_SPI_M_DO         WM_IO_OPTION3
#define WM_IO_OPT3_SPI_M_DI         WM_IO_OPTION3
#define WM_IO_OPT3_SPI_M_CS         WM_IO_OPTION3
#define WM_IO_OPT3_SDIO_CK          WM_IO_OPTION3
#define WM_IO_OPT3_SDIO_CMD         WM_IO_OPTION3
#define WM_IO_OPT3_SDIO_DAT         WM_IO_OPTION3

/* io option4 */
#define WM_IO_OPT4_I2S_M_MCLK       WM_IO_OPTION4
#define WM_IO_OPT4_I2S_M_RL         WM_IO_OPTION4
#define WM_IO_OPT4_I2S_M_SCL        WM_IO_OPTION4
#define WM_IO_OPT4_I2S_M_SDA        WM_IO_OPTION4

/* io option5 */
#define WM_IO_OPT5_GPIO             WM_IO_OPTION5

/* io option6 */
#define WM_IO_OPT6_ADC              WM_IO_OPTION6
#define WM_IO_OPT6_LCD_COM          WM_IO_OPTION6
#define WM_IO_OPT6_LCD_SEG          WM_IO_OPTION6


/**
 * @brief          	This function is used to config io function
 *
 * @param[in]      	name      io name
 * @param[in]      	option    io function option, value is WM_IO_OPT*_*, also is WM_IO_OPTION1~6
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_io_cfg_set(enum tls_io_name name, u8 option);


/**
 * @brief          	This function is used to get io function config 
 *
 * @param[in]      	name      io name
 *
 * @retval         	WM_IO_OPTION1~6  Mapping io function
 *
 * @note           	None
 */
int tls_io_cfg_get(enum tls_io_name name);


#endif  /* end of WM_IO_H */

