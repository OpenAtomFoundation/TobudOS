/**
 * @file    wm_hspi.h
 *
 *
 * @brief   High speed spi slave Module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */

#ifndef WM_HSPI_H
#define WM_HSPI_H

#include "wm_type_def.h"
#include "wm_ram_config.h"

#define HSPI_TX_MEM_MALLOC			0		/** tx mem dynamic malloc*/

#define HSPI_IO_REUSE_NUM			0
#define SDIO_IO_REUSE_NUM			2
#define HSPI_INTERFACE_SPI			2		/** spi interface*/
#define HSPI_INTERFACE_SDIO			3		/** sdio interface*/

/**rx message*/
#define HSPI_RX_CMD_MSG     1
#define HSPI_RX_DATA_MSG    2

/**spi/sdio buffer, Wraper controller can only access the address after the 0x60000*/
#define HSPI_TXBUF_NUM              2
#define HSPI_TX_DESC_NUM            HSPI_TXBUF_NUM
#define HSPI_RXBUF_NUM              3//10
#define HSPI_RX_DESC_NUM            HSPI_RXBUF_NUM
#define HSPI_TXBUF_SIZE             1500
#define HSPI_RXBUF_SIZE             1500

#define HSPI_TX_DESC_SIZE          sizeof(struct tls_hspi_tx_desc)
#define HSPI_RX_DESC_SIZE          sizeof(struct tls_hspi_rx_desc)

/*****************************************************************************
 * sdio/hspi sram partition
 * total size : 0x61800 - 0x67FFFF   (26KB)
 ******************************************************************************/
/* HSPI txbuf zone */
#define HSPI_TXBUF_BASE_ADDR        ((u32)(SLAVE_HSPI_SDIO_ADDR))

#if HSPI_TX_MEM_MALLOC
#define HSPI_TXBUF_TOTAL_SIZE        0
#else
#define HSPI_TXBUF_TOTAL_SIZE        (HSPI_TXBUF_SIZE * HSPI_TXBUF_NUM)
#endif
/** HSPI tx desc zone */
#define HSPI_TX_DESC_BASE_ADDR      ((u32)(HSPI_TXBUF_BASE_ADDR + HSPI_TXBUF_TOTAL_SIZE))
#define HSPI_TX_DESC_TOTAL_SIZE     (HSPI_TX_DESC_SIZE * HSPI_TX_DESC_NUM)	//28*3=84
/** HSPI rxbuf zone */
#define HSPI_RXBUF_BASE_ADDR        ((u32)(HSPI_TX_DESC_BASE_ADDR + HSPI_TX_DESC_TOTAL_SIZE))
#define HSPI_RXBUF_TOTAL_SIZE       (HSPI_RXBUF_NUM * HSPI_RXBUF_SIZE)	//4500
/** HSPI rx desc zone */
#define HSPI_RX_DESC_BASE_ADDR      ((u32)(HSPI_RXBUF_BASE_ADDR + HSPI_RXBUF_TOTAL_SIZE))
#define HSPI_RX_DESC_TOTAL_SIZE     (HSPI_RX_DESC_SIZE * HSPI_RX_DESC_NUM)	//36

#if 0
#define SDIO_CMD_RXBUF_SIZE          (0x100)
#define SDIO_CMD_RXBUF_ADDR          (0x00068000 -  SDIO_CMD_RXBUF_SIZE)
#define SDIO_CIS1_ADDR              (SDIO_CMD_RXBUF_ADDR - 0x80)
#define SDIO_CIS0_ADDR              (SDIO_CIS1_ADDR - 0x80)
#else
#define SDIO_CIS0_ADDR              (HSPI_RX_DESC_BASE_ADDR + HSPI_RX_DESC_TOTAL_SIZE)	//128
#define SDIO_CIS1_ADDR              (SDIO_CIS0_ADDR + 0x80)						//128
#define SDIO_CMD_RXBUF_ADDR          (SDIO_CIS1_ADDR + 0x80)
#define SDIO_CMD_RXBUF_SIZE          256//(1500)				//256
#endif
#define CIS_FUN0_ADDR	SDIO_CIS0_ADDR
#define CIS_FUN1_ADDR	SDIO_CIS1_ADDR

#define FN0_TPL_FUNCID				(CIS_FUN0_ADDR + 0x00)
#define FN0_TPL_FUNCE				(CIS_FUN0_ADDR + 0x04)
#define FN0_TPL_FUNCE_MAXBLK		(CIS_FUN0_ADDR + 0x08)
#define FN0_TPL_MANFID_MID		    (CIS_FUN0_ADDR + 0x0C)
#define FN0_TPL_END					(CIS_FUN0_ADDR + 0x10)

#define FN1_TPL_FUNCID				(CIS_FUN1_ADDR + 0x00)
#define FN1_TPL_FUNCE				(CIS_FUN1_ADDR + 0x04)
#define FN1_TPL_FUNCE_VER			(CIS_FUN1_ADDR + 0x08)
#define FN1_TPL_FUNCE_NSN			(CIS_FUN1_ADDR + 0x0C)
#define FN1_TPL_FUNCE_CSASIZE		(CIS_FUN1_ADDR + 0x10)
#define FN1_TPL_FUNCE_OCR			(CIS_FUN1_ADDR + 0x14)
#define FN1_TPL_FUNCE_MINPWR		(CIS_FUN1_ADDR + 0x18)
#define FN1_TPL_FUNCE_STANDBY		(CIS_FUN1_ADDR + 0x1C)
#define FN1_TPL_FUNCE_OPTBW		    (CIS_FUN1_ADDR + 0x20)
#define FN1_TPL_FUNCE_NTIMEOUT	    (CIS_FUN1_ADDR + 0x24)
#define FN1_TPL_FUNCE_AVGPWR		(CIS_FUN1_ADDR + 0x28)
#define FN1_TPL_END					(CIS_FUN1_ADDR + 0x30)

/**Definition of SDIO interrupt in the system*/
#define SDIO_RX_DATA_INT            (0UL)
#define SDIO_TX_DATA_INT          	(1UL)
#define SDIO_RX_CMD_INT             (2UL)
#define SDIO_TX_CMD_INT             (3UL)

/**IO control register*/
#define HR_IOCTL_GP_SDIO       HR_IOCTL_GP_SDIO_I2C


/** SDIO interrupt bit definition */
#define SDIO_WP_INT_SRC_CMD_DOWN         (1UL<<3)
#define SDIO_WP_INT_SRC_CMD_UP           (1UL<<2)
#define SDIO_WP_INT_SRC_DATA_DOWN        (1UL<<1)
#define SDIO_WP_INT_SRC_DATA_UP          (1UL<<0)


/** Definition of send data  descriptor structure */
struct tls_hspi_tx_desc {
    volatile u32 valid_ctrl;
    u32 buf_info;
    u32 buf_addr[3];
    u32 next_desc_addr;
#if HSPI_TX_MEM_MALLOC
    u32 txbuf_addr;    /**< txbuf addr, pbuf and buf_addr[0] are different */
#endif
};

/** Definition of receive data  descriptor structure */
struct tls_hspi_rx_desc {
    u32 valid_ctrl;
    u32 buf_addr;
    u32 next_desc_addr;
};


/** struct tls_slave_hspi */
struct tls_slave_hspi {
    u8 ifusermode;

    s16 (*rx_cmd_callback)(char *buf);

    s16 (*rx_data_callback)(char *buf);

    s16 (*tx_data_callback)(char *buf);

    struct tls_hspi_tx_desc *curr_tx_desc;    /**< Upstream data management */

    struct tls_hspi_rx_desc   *curr_rx_desc;    /**< Downlink data management */

#if HSPI_TX_MEM_MALLOC
	u8 txdoneflag;		                        /**< tx done falg*/
#endif
};

/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup HSPI_Driver_APIs HSPI Driver APIs
 * @brief HSPI driver APIs
 */

/**
 * @addtogroup HSPI_Driver_APIs
 * @{
 */

/**
 * @brief          This function is used to initial HSPI register.
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         other failed
 *
 * @note           When the system is initialized, the function has been called, so users can not call this function.
 */
int tls_slave_spi_init(void);

/**
 * @brief          This function is used to enable or disable user mode.
 *
 * @param[in]      ifenable     TRUE or FALSE
 *
 * @return         None
 *
 * @note           If the user enables the user mode, RICM instruction in the system will not be used by SPI.
 *		        If the user wants to use the SPI interface as other use, need to enable the user mode.
 *		        This function must be called before the register function.
 */
void tls_set_hspi_user_mode(u8 ifenable);


/**
 * @brief          This function is used to set high speed interface type.
 *
 * @param[in]      type    is the interface type. HSPI_INTERFACE_SPI or HSPI_INTERFACE_SDIO
 *
 * @return         None
 *
 * @note           None
 */
void tls_set_high_speed_interface_type(int type);


/**
 * @brief          This function is used to register hspi rx command interrupt.
 *
 * @param[in]      rx_cmd_callback		is the hspi rx interrupt call back function.
 *
 * @return         None
 *
 * @note           None
 */
void tls_hspi_rx_cmd_callback_register(s16 (*rx_cmd_callback)(char *buf));


/**
 * @brief          This function is used to register hspi rx data interrupt.
 *
 * @param[in]      rx_data_callback		is the hspi rx interrupt call back function.
 *
 * @return         None
 *
 * @note           None
 */
void tls_hspi_rx_data_callback_register(s16 (*rx_data_callback)(char *buf));

/**
 * @brief          This function is used to register hspi tx data interrupt.
 *
 * @param[in]      tx_data_callback		is the hspi tx interrupt call back function.
 *
 * @return         None
 *
 * @note           None
 */
void tls_hspi_tx_data_callback_register(s16 (*tx_data_callback)(char *buf));


/**
 * @brief          This function is used to transfer data.
 *
 * @param[in]      txbuf			is a buf for saving user data.
 * @param[in]      len                 is the data length.
 *
 * @retval         transfer data len     success
 * @retval         0                          failed
 *
 * @note           None
 */
int tls_hspi_tx_data(char *txbuf, int len);

/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_HSPI_H */
