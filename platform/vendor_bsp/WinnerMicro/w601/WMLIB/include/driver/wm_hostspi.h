/**
 * @file    wm_hostspi.h
 *
 * @brief   host spi Driver Module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_HOST_SPI_H
#define WM_HOST_SPI_H

#include "wm_type_def.h"
#include "list.h"
//#include "wm_osal.h"
#include "wm_ram_config.h"


#define SPI_USE_DMA

#define SPI_TX_DMA_MASK	0x01
#define SPI_RX_DMA_MASK	0x02

#define SPI_DMA_CMD_ADDR 		((u32)(MASTER_SPI_DMA_ADDR))
#define SPI_DMA_CMD_MAX_SIZE       (0x20)

#define SPI_DMA_BUF_ADDR		((u32)(SPI_DMA_CMD_ADDR + SPI_DMA_CMD_MAX_SIZE))
#define SPI_DMA_BUF_MAX_SIZE	8160

#define SPI_DMA_MAX_TRANS_SIZE	4092

/**
 *  error code.
 */
#define TLS_SPI_STATUS_OK		(0)
#define TLS_SPI_STATUS_EINVAL		(-1)
#define TLS_SPI_STATUS_ENOMEM		(-2)
#define TLS_SPI_STATUS_EBUSY		(-3)
#define TLS_SPI_STATUS_ESHUTDOWN	(-4)
#define TLS_SPI_STATUS_EPERM		(-5)
#define TLS_SPI_STATUS_ECLKNOSUPPORT	(-6)
#define TLS_SPI_STATUS_EMODENOSUPPORT	(-7)

#define SPI_MASTER_FIFO_SIZE		(32)

/**
 *  the SPI master controller's configuration data.
 */
 /**    configuration data.    */
#define SPI_CPHA			(0x01)  /** clock phase. */
#define SPI_CPOL			(0x02)  /** clock polarity. */
#define TLS_SPI_MODE_0			(0|0)   /** motorola mode. */
#define TLS_SPI_MODE_1			(0|SPI_CPHA)
#define TLS_SPI_MODE_2			(SPI_CPOL|0)
#define TLS_SPI_MODE_3      (SPI_CPOL|SPI_CPHA)
#define TLS_SPI_CS_LOW      0x00    /** chipselect active low. */
#define TLS_SPI_CS_HIGH      0x01   /** chipselect active high. */
#define TLS_SPI_FCLK_MIN      (1000)    /** minimum work clock rate(Hz). */
#define TLS_SPI_FCLK_MAX      (APB_CLK/2)   /** maximum work clock rate(Hz). */


/**    default configuration data.    */
#define SPI_DEFAULT_SPEED      (2000000)    /** default clock rate is 2MHz. */
#define SPI_DEFAULT_MODE      (TLS_SPI_MODE_0)  /** default mode MODE_0. */
#define SPI_CS_ACTIVE_MODE      (TLS_SPI_CS_LOW)    /** default chipselect mode is active low. */
#define SPI_CS_INACTIVE_MODE      (TLS_SPI_CS_HIGH)

/** SPI transaction message status. */
#define SPI_MESSAGE_STATUS_IDLE      (0)
#define SPI_MESSAGE_STATUS_INPROGRESS      (1)
#define SPI_MESSAGE_STATUS_DONE      (2)

/**slave type*/
#define SPI_SLAVE_FLASH		0   /**flash */
#define SPI_SLAVE_CARD		1   /** SD card */
#define SPI_SLAVE_CONTROL_PIN		0
/**transfer type*/
#define SPI_BYTE_TRANSFER			0   /**byte transfer*/
#define SPI_WORD_TRANSFER			1   /**word transfer*/
#define SPI_DMA_TRANSFER			2   /** DMA transfer */

/**
 *  a read/write buffer pair
 *
 *  SPI transfers always write the same number of bytes as they read.
 *  If the transmit buffer is null, zeroes will be shifted out while
 *  filling rx_buf. If the receive buffer is null, the data shifted in
 *  will be discarded.
 */
struct tls_spi_transfer
{
    struct dl_list transfer_list;    /**< transfers are sequenced through
                                          tls_spi_message.transfers. */

    const void *tx_buf;    /**< data to be written, or NULL. */
    void *rx_buf;              /**< data to be read, or NULL. */
    u32 len;                    /**< size of rx and tx buffers (in bytes). */
    u32 delay_usecs;       /**< microseconds to delay after this transfer. */
};


/**
 *  one multi-segment SPI transaction
 *
 *  A struct tls_spi_message is used to execute an atomic sequence of data
 *  transfers, each represented by a struct tls_spi_transfer.  The sequence
 *  is "atomic" in the sense that no other spi_message may use that SPI bus
 *  until that sequence completes.
  */
struct tls_spi_message
{
    struct dl_list queue;       /**< transaction messages are sequenced through
                                     tls_spi_port.wait_queue. */

    struct dl_list transfers;   /**< list of transfer segments in this transaction. */
    void (*complete) (void *);  /**< called to report transaction completions. */
    void *context;              /**< the argument to complete() when it's called. */
    u32 status;                 /**< transaction message status. */
};

/**
 *  driver structure to SPI master controller
 *
 *  This data structure presents the SPI master controller's configuration
 *  data. The device attached to this SPI master controller share the same
 *  transfer mode, chipselect mode and clock rate. And this structure maintains
 *  a queue of tls_spi_message transactions and uses this tls_spi_message transaction
 *  to access to the SPI device. For each such message it queues, it calls the message's
 *  completion function when the transaction completes.
 */
struct tls_spi_port
{
    u32 speed_hz;               /**< clock rate to be used. */
    u8 cs_active;               /**< chipselect mode, maybe active low or active
                                   high. */
    u8 mode;                    /**< SPI transfer mode: mode_0(CPHA=0, CHOL=0),
                                   mode_1(CPHA=0, CHOL=1), mode_2(CPHA=1,
                                   CHOL=0), mode_3(CPHA=1, CHOL=1). */
    u8 reconfig;

    struct dl_list wait_queue;  /**< wait list of transaction messages. */
    tls_os_queue_t *lock;

    tls_os_queue_t *msg_queue;  /**< notify the schedule thread that there's
                                   transaction message queued. */
    struct tls_spi_message *current_message;    /**< current transaction message
                                                   in-progressing. */
    u32 current_remaining_transfer; /**< remaining transfer segments count in
                                       current transaction message. */

    struct tls_spi_transfer *current_transfer;  /**< current transfer segment
                                                   in-progressing. */
    u32 current_remaining_bytes;    /**< remaining data length in current
                                       transfer segment. */

    u8 transtype;               /**< transfer type  */
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
 * @defgroup HOSTSPI_Driver_APIs HOSTSPI Driver APIs
 * @brief HOSTSPI driver APIs
 */

/**
 * @addtogroup HOSTSPI_Driver_APIs
 * @{
 */

/**
 * @brief          This function is used to initialize the SPI master driver.
 *
 * @param[in]      None
 *
 * @retval         TLS_SPI_STATUS_OK			if initialize success
 * @retval         TLS_SPI_STATUS_EBUSY		if SPI is already initialized
 * @retval         TLS_SPI_STATUS_ENOMEM		if malloc SPI memory fail
 *
 * @note           None
 */
int tls_spi_init(void);


/**
 * @brief          This function is used to setup the spi controller.
 *
 * @param[in]      mode         is CPOL and CPHA type defined in TLS_SPI_MODE_0 to TLS_SPI_MODE_3
 * @param[in]      cs_active    is cs mode, defined as TLS_SPI_CS_LOW or TLS_SPI_CS_HIGH
 * @param[in]      fclk            is spi clock,the unit is HZ.
 *
 * @retval         TLS_SPI_STATUS_OK			if setup success
 * @retval         TLS_SPI_STATUS_EMODENOSUPPORT	if mode is not support
 * @retval         TLS_SPI_STATUS_EINVAL			if cs_active is not support
 * @retval         TLS_SPI_STATUS_ECLKNOSUPPORT	if fclk is not support
 *
 * @note           None
 */
int tls_spi_setup(u8 mode, u8 cs_active, u32 fclk);

/**
 * @brief          This function is used to synchronous write data by SPI.
 *
 * @param[in]      buf          data to be sent.
 * @param[in]      len          data length.
 *
 * @retval         TLS_SPI_STATUS_OK			if write success.
 * @retval         TLS_SPI_STATUS_EINVAL		if argument is invalid.
 * @retval         TLS_SPI_STATUS_ENOMEM			if there is no enough memory.
 * @retval         TLS_SPI_STATUS_ESHUTDOWN		if SPI driver does not installed.
 *
 * @note           None
 */
int tls_spi_write(const u8 * buf, u32 len);

/**
 * @brief          This function is used to synchronously read data from SPI.
 *
 * @param[in]      buf          is the buffer for saving SPI data.
 * @param[in]      len          is the data length.
 *
 * @retval         TLS_SPI_STATUS_OK			if write success.
 * @retval         TLS_SPI_STATUS_EINVAL		if argument is invalid.
 * @retval         TLS_SPI_STATUS_ENOMEM			if there is no enough memory.
 * @retval         TLS_SPI_STATUS_ESHUTDOWN		if SPI driver does not installed.
 *
 * @note           None
 */
int tls_spi_read(u8 * buf, u32 len);

/**
 * @brief          This function is used to synchronously write command and then read data from SPI.
 *
 * @param[in]      txbuf        is the write data buffer.
 * @param[in]      n_tx         is the write data length.
 * @param[in]      rxbuf        is the read data buffer.
 * @param[in]      n_rx         is the read data length.
 *
 * @retval         TLS_SPI_STATUS_OK			if write success.
 * @retval         TLS_SPI_STATUS_EINVAL		if argument is invalid.
 * @retval         TLS_SPI_STATUS_ENOMEM		if there is no enough memory.
 * @retval         TLS_SPI_STATUS_ESHUTDOWN		if SPI driver has not been installed.
 *
 * @note           None
 */
int tls_spi_read_with_cmd(const u8 * txbuf, u32 n_tx, u8 * rxbuf, u32 n_rx);

/**
 * @brief          This function is used to synchronous write 32bit command then write data from SPI.
 *
 * @param[in]      cmd                    is the command data.
 * @param[in]      n_cmd                 is the command len,can not bigger than four
 * @param[in]      txbuf                   is the write data buffer.
 * @param[in]      n_tx                    is the write data length.
 *
 * @retval         TLS_SPI_STATUS_OK			if write success.
 * @retval         TLS_SPI_STATUS_EINVAL		if argument is invalid.
 * @retval         TLS_SPI_STATUS_ENOMEM			if there is no enough memory.
 * @retval         TLS_SPI_STATUS_ESHUTDOWN		if SPI driver does not installed.
 *
 * @note           None
 */
int tls_spi_write_with_cmd(const u8 * cmd, u32 n_cmd, const u8 * txbuf, u32 n_tx);

/**
 * @brief          This function is used to set SPI transfer mode.
 *
 * @param[in]      type             is the transfer type.
 *				                SPI_BYTE_TRANSFER ->byte transfer;
 *	                        	SPI_WORD_TRANSFER ->word transfer;
 *				                SPI_DMA_TRANSFER ->DMA transfer;

 *
 * @return         None
 *
 * @note           None
 */
void tls_spi_trans_type(u8 type);

/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_HOST_SPI_H */
