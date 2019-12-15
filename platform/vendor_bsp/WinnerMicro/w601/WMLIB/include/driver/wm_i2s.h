/**************************************************************************//**
 * @file     wm_i2s.h
 * @author
 * @version
 * @date
 * @brief
 *
 * @copyright (c) 2014 Winner Microelectronics Co., Ltd. All rights reserved.
 *****************************************************************************/

#ifndef __WM_I2S_H
#define __WM_I2S_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wm_type_def.h"
#include "wm_regs.h"
#include "wm_osal.h"
#include "misc.h"
#include "wm_dma.h"

#define I2S_CTRL_FORMAT_Pos			(24)                                /*!< CTRL : FORMAT Position */
#define I2S_CTRL_FORMAT_MASK		(3UL<<I2S_CTRL_FORMAT_Pos)          /*!< CTRL : FORMAT mask */
#define I2S_CTRL_FORMAT_I2S			(0x0UL<<I2S_CTRL_FORMAT_Pos)        /*!< I2S data format*/
#define I2S_CTRL_FORMAT_MSB			(0x1UL<<I2S_CTRL_FORMAT_Pos)        /*!< MSB data format*/
#define I2S_CTRL_FORMAT_PCMA		(0x2UL<<I2S_CTRL_FORMAT_Pos)        /*!< PCMA data format*/
#define I2S_CTRL_FORMAT_PCMB		(0x3UL<<I2S_CTRL_FORMAT_Pos)        /*!< PCMB data format*/

#define I2S_CTRL_DATABIT_MASK		(0x30UL)                            /*!< CTRL : DATABIT mask */
#define I2S_CTRL_DATABIT_8			((uint8_t)0x0)                      /*!< I2S data width is 8-bit */
#define I2S_CTRL_DATABIT_16			((uint8_t)0x10)                     /*!< I2S data width is 16-bit */
#define I2S_CTRL_DATABIT_24			((uint8_t)0x20)                     /*!< I2S data width is 24-bit */
#define I2S_CTRL_DATABIT_32			((uint8_t)0x30)	                    /*!< I2S data width is 32-bit */

#define I2S_CTRL_CHSEL_Pos          (23)								/*!< CTRL : Channel select position */
#define I2S_CTRL_CHSEL_MASK		    (1UL<<I2S_CTRL_CHSEL_Pos)           /*!< CTRL: Channel select mask */
#define I2S_CTRL_CHSEL_LEFT			(1UL<<I2S_CTRL_CHSEL_Pos)           /*!< CTRL: Left channel select */
#define I2S_CTRL_CHSEL_RIGHT		(0UL<<I2S_CTRL_CHSEL_Pos)           /*!< CTRL: Right channel select */

#define I2S_CTRL_STEREO_MONO_Pos	(22)                                /*!< CTRL: STEREO MONO Position */
#define I2S_CTRL_STEREO_MONO_MASK	(1UL<<I2S_CTRL_STEREO_MONO_Pos)     /*!< CTRL: STEREO MONO MASK */
#define I2S_CTRL_MONO				(1UL<<I2S_CTRL_STEREO_MONO_Pos)     /*!< Monaural channel */
#define I2S_CTRL_STERO				(0UL<<I2S_CTRL_STEREO_MONO_Pos)     /*!< Stereo channel */

#define I2S_CTRL_RXDMA_Pos		 	(21)                                /*!< CTRL: RXDMA Position */
#define I2S_CTRL_RXDMA_MASK		    (1UL<<I2S_CTRL_RXDMA_Pos)           /*!< CTRL: RXDMA mask */

#define I2S_CTRL_TXDMA_Pos			(20)                                /*!< CTRL: TXDMA Position*/
#define I2S_CTRL_TXDMA_MASK		    (1UL<<I2S_CTRL_TXDMA_Pos)           /*!< CTRL: TXDMA mask */

#define I2S_CTRL_RX_CLR_Pos			(19)                                /*!< CTRL: RXFIFO clear Position */
#define I2S_CTRL_RX_CLR_MASK		(1UL<<I2S_CTRL_RX_CLR_Pos)          /*!< CTRL: RXFIFO clear mask */

#define I2S_CTRL_TX_CLR_Pos			(18)                                /*!< CTRL: TXFIFO clear Position */
#define I2S_CTRL_TX_CLR_MASK		(1UL<<I2S_CTRL_TX_CLR_Pos)          /*!< CTRL: TXFIFO clear mask */

#define I2S_CTRL_LZCEN_Pos			(17)                                /*!< CTRL: LZCEN Position */
#define I2S_CTRL_LZCEN_MASK		    (1UL<<I2S_CTRL_LZCEN_Pos)           /*!< CTRL: LZCEN mask */

#define I2S_CTRL_RZCEN_Pos			(16)                                /*!< CTRL: RZCEN Position */
#define I2S_CTRL_RZCEN_MASK		    (1UL<<I2S_CTRL_RZCEN_Pos)           /*!< CTRL: RZCEN mask */

#define I2S_CTRL_RXTH_Pos			(12)                                /*!< CTRL: RXTH Position */
#define I2S_CTRL_RXTH_MASK			(7UL<<I2S_CTRL_RXTH_Pos)            /*!< CTRL: RXTH mask */

#define I2S_CTRL_TXTH_Pos			(9)                                 /*!< CTRL: TXTH Position */
#define I2S_CTRL_TXTH_MASK			(7UL<<I2S_CTRL_TXTH_Pos)            /*!< CTRL: TXTH mask */

#define I2S_CTRL_MUTE_Pos			(3)                                 /*!< CTRL: MUTE Position */
#define I2S_CTRL_MUTE_MASK			(1UL<<I2S_CTRL_MUTE_Pos)            /*!< CTRL: MUTE mask */

#define I2S_CTRL_RXE_Pos			(2)                                 /*!< CTRL: RXEN Position */
#define I2S_CTRL_RXE_MASK			(1UL<<I2S_CTRL_RXE_Pos)             /*!< CTRL: RXEN mask */

#define I2S_CTRL_TXE_Pos			(1)                                 /*!< CTRL: TXEN Position */
#define I2S_CTRL_TXE_MASK			(1UL<<I2S_CTRL_TXE_Pos)             /*!< CTRL: TXEN mask */

#define I2S_CTRL_EN_Pos				(0)                                 /*!< CTRL: EN Position */
#define I2S_CTRL_EN_MASK			(1UL<<I2S_CTRL_EN_Pos)              /*!< CTRL: EN mask */

#define I2S_INT_MASK_LZC		    ((uint16_t)0x200)                   /*!< Left Channel Zero-Cross Interrupt MASK */
#define I2S_INT_MASK_RZC		    ((uint16_t)0x100)                   /*!< Right Channel Zero-Cross Interrupt MASK */
#define I2S_INT_MASK_TXDONE	        ((uint16_t)0x080)                   /*!< Transmit Done Interrupt Mask */
#define I2S_INT_MASK_TXTH		    ((uint16_t)0x040)                   /*!< Transmit FIFO Threshold level Interrupt MASK */
#define I2S_INT_MASK_TXOV		    ((uint16_t)0x020)                   /*!< Transmit FIFO Overflow Interrupt MASK */
#define I2S_INT_MASK_TXUD		    ((uint16_t)0x010)                   /*!< Transmit FIFO Underflow Interrupt MASK */
#define I2S_INT_MASK_RXDONE	        ((uint16_t)0x008)                   /*!< Receive Done Interrupt Mask */
#define I2S_INT_MASK_RXTH		    ((uint16_t)0x004)                   /*!< Receive FIFO Threshold level Interrupt MASK */
#define I2S_INT_MASK_RXOV		    ((uint16_t)0x002)                   /*!< Receive FIFO Overflow Interrupt MASK */
#define I2S_INT_MASK_RXUD		    ((uint16_t)0x002)                   /*!< Receive FIFO Underflow Interrupt MASK */

#define I2S_FLAG_TX_Pos			    (12)                                /*!< INT_SRC: TX interrupt flag Position */
#define I2S_FLAG_RX_Pos			    (11)                                /*!< INT_SRC: RX interrupt flag Position */
#define I2S_FLAG_I2S_Pos		    (10)                                /*!< INT_SRC: I2S module interrupt flag Position */
#define I2S_FLAG_LZC_Pos		    (9)                                 /*!< INT_SRC: LZC module interrupt flag Position */
#define I2S_FLAG_RZC_Pos		    (8)                                 /*!< INT_SRC: RZC module interrupt flag Position */
#define I2S_FLAG_TXDONE_Pos	        (7)                                 /*!< INT_SRC: TXDONE module interrupt flag Position */
#define I2S_FLAG_TXTH_Pos		    (6)                                 /*!< INT_SRC: TXTH module interrupt flag Position */
#define I2S_FLAG_TXOV_Pos		    (5)                                 /*!< INT_SRC: TXOV module interrupt flag Position */
#define I2S_FLAG_TXUD_Pos		    (4)                                 /*!< INT_SRC: TXUD module interrupt flag Position */
#define I2S_FLAG_RXDONE_Pos	        (3)                                 /*!< INT_SRC: RXDONE module interrupt flag Position */
#define I2S_FLAG_RXTH_Pos		    (2)                                 /*!< INT_SRC: RXTH module interrupt flag Position */
#define I2S_FLAG_RXOV_Pos		    (1)                                 /*!< INT_SRC: RXOV module interrupt flag Position */
#define I2S_FLAG_RXUD_Pos		    (0)                                 /*!< INT_SRC: RXUD module interrupt flag Position */

#define I2S_FLAG_TX					(1<<I2S_FLAG_TX_Pos)                /*!< INT_SRC: TX interrupt flag */
#define I2S_FLAG_RX					(1<<I2S_FLAG_RX_Pos)                /*!< INT_SRC: RX interrupt flag */
#define I2S_FLAG_I2S				(1<<I2S_FLAG_I2S_Pos)               /*!< INT_SRC: I2S module interrupt flag */
#define I2S_FLAG_LZC				(1<<I2S_FLAG_LZC_Pos)               /*!< INT_SRC: LZC module interrupt flag */
#define I2S_FLAG_RZC				(1<<I2S_FLAG_RZC_Pos)               /*!< INT_SRC: RZC module interrupt flag */
#define I2S_FLAG_TXDONE			    (1<<I2S_FLAG_TXDONE_Pos)            /*!< INT_SRC: TXDONE module interrupt flag */
#define I2S_FLAG_TXTH				(1<<I2S_FLAG_TXTH_Pos)              /*!< INT_SRC: TXTH module interrupt flag */
#define I2S_FLAG_TXOV				(1<<I2S_FLAG_TXOV_Pos)              /*!< INT_SRC: TXOV module interrupt flag */
#define I2S_FLAG_TXUD				(1<<I2S_FLAG_TXUD_Pos)              /*!< INT_SRC: TXUD module interrupt flag */
#define I2S_FLAG_RXDONE			    (1<<I2S_FLAG_RXDONE_Pos)            /*!< INT_SRC: RXDONE module interrupt flag */
#define I2S_FLAG_RXTH				(1<<I2S_FLAG_RXTH_Pos)              /*!< INT_SRC: RXTH module interrupt flag */
#define I2S_FLAG_RXOV				(1<<I2S_FLAG_RXOV_Pos)              /*!< INT_SRC: RXOV module interrupt flag */
#define I2S_FLAG_RXUD				(1<<I2S_FLAG_RXUD_Pos)              /*!< INT_SRC: RXUD module interrupt flag */

#define I2S_RX_FIFO_CNT_MASK		(0x0F)                              /*!< RX fifo count mask */
#define I2S_TX_FIFO_CNT_MASK		(0xF0)                              /*!< TX fifo count mask */

#define I2S_MAX_RXTXFIFO_LEVEL		(8)                                 /*!< max rx/tx fifo level */
#define WM_I2S_TX_DMA_CHANNEL       (4)
#define WM_I2S_RX_DMA_CHANNEL       (5)


typedef void (*tls_i2s_callback)();

typedef struct
{
	/** I2S Control Register */
	__IO uint32_t CTRL;
	/** I2S Interrupt mask Register */
	__IO uint32_t INT_MASK;
	/** I2S Interrupt source Register */
	__IO uint32_t INT_SRC;
	/** I2S Interrupt status Register */
	__I  uint32_t INT_STATUS;
	/** I2S TX data Register */
	__O  uint32_t TX;
	/** I2S RX data Register */
	__I	 uint32_t RX;
} I2S_T;

/** base address of the I2S module */
#define I2S	((I2S_T *)HR_I2S_REG_BASE)


typedef enum
{
	/** left channel select */
	I2S_CHANNEL_LEFT		= I2S_CTRL_CHSEL_LEFT,
	/** right channel select */
	I2S_CHANNEL_RIGHT		= I2S_CTRL_CHSEL_RIGHT
} I2S_CHANNEL_SEL;


typedef struct tls_i2s_options
{
	/** I2S receiver enable */
	uint8_t rx_en;
	/** I2S transfer enable */
	uint8_t tx_en;
	/** I2S Stereo or Monaural
	    - \ref I2S_CTRL_STERO
	    - \ref I2S_CTRL_MONO
	*/
	uint32_t stereo_mono;
	/** I2S module format
		- \ref I2S_CTRL_FORMAT_I2S
		- \ref I2S_CTRL_FORMAT_MSB
		- \ref I2S_CTRL_FORMAT_PCMA
		- \ref I2S_CTRL_FORMAT_PCMB
	*/
	uint32_t format;
	/** I2S module data width
		- \ref I2S_CTRL_DATABIT_8
		- \ref I2S_CTRL_DATABIT_16
		- \ref I2S_CTRL_DATABIT_24
		- \ref I2S_CTRL_DATABIT_32
	*/
	uint32_t data_width;
	/** I2S module sample rate  max 128KHz */
	uint32_t sample_rate;
} tls_i2s_options_t;

typedef struct tls_i2s_buf_t
{
    /** pointer the bufffer address */
    uint32_t * buf;
    /** the buffer size in bytes */
    uint16_t len;
    /** current position of  the data */
    uint16_t index;
}tls_i2s_buf;

typedef struct _tls_i2s_port
{
    /** i2s config params */
    struct tls_i2s_options opts;
    /** pointer to the I2S registers */
    I2S_T * regs;
    /** the semaphore used to synchronous transmission */
    tls_os_sem_t *tx_sem;
    /** instance of tls_i2s_buf struct for data receive */
    tls_i2s_buf  rx_buf;
    /** instance of tls_i2s_buf struct for data receive */
    tls_i2s_buf  tx_buf;
    /** function pointer for data receiver when in interrupt mode */
    void 	(*rx_callback)(u16 len);
    /** function pointer for data transmit when in interrupt mode */
    void 	(*tx_callback)(u16 len);

} tls_i2s_port_t;

/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup I2S_Driver_APIs I2S Driver APIs
 * @brief I2S driver APIs
 */

/**
 * @addtogroup I2S_Driver_APIs
 * @{
 */

/**
 * @brief 	This function is used to initial i2s port.
 * @param[in] opts	the i2s setting options,if this param is NULL,this function will use the default options.
 * @retval
 *	- \ref WM_SUCCESS
 *	- \ref WM_FAILED
 *
 */
int tls_i2s_port_init(tls_i2s_options_t *opts);

/**
 * @brief 	This function is used to config i2s port.
 * @param[in] opts	is the i2s setting options
 *
 */
void tls_i2s_config(tls_i2s_options_t *opts);

/**
 * @brief 	Set the frequency of the i2s port.
 * @param[in] freq	the required frequency of the i2s module
 * @retval
 *
 */
void tls_i2s_set_freq(uint32_t freq);

/**
 * @brief
 * 	set the frequency of the i2s port.
 *
 * @param[in] freq
 *	the required frequency of the i2s module
 * @param[in] exclk
 *	the frequency of the external clock
 *
 * @retval
 *
 */
void wm_i2s_set_freq_exclk(uint32_t freq, uint32_t exclk);

/**
 * @brief	This function is used to transfer data in blocking mode.
 * @param[in] buf	pointer to data buffer
 * @param[in] len	amount of data to be sent.
 * @retval
 * 	- \ref WM_SUCCESS
 *	- \ref WM_FAILED
 */
int tls_i2s_tx_block(uint32_t *buf, uint16_t len);

/**
 * @brief	This function is used to transfer data in non-blocking mode.
 * @param[in] buf	pointer to data buffer
 * @param[in] len	bytes of data to be sent.
 * @param[in] tx_callback   function pointer to a callback,when the data have been sent this specified function will be called
 *  @retval
 *     - \ref WM_SUCCESS
 *     - \ref WM_FAILED
 *  @note
 *      this function only submit the data
 *      when the data transfer finished the tx_callback function will be called
 */
int tls_i2s_tx_nonblock(uint32_t *buf, uint16_t len, void (*tx_callback)(u16 len));

/**
 * @brief	This function is used to receive data in non-blocking mode.
 * @param[in] buf	pointer to data buffer
 * @param[in] len	bytes of data to be sent.
 * @param[in] rx_callback    function pointer to a callback,when the data have been received this specified function will be called
 *
 *  @retval
 *     - \ref WM_SUCCESS
 *     - \ref WM_FAILED
 *  @note
 *
 */
int tls_i2s_rx_nonblock(uint32_t *buf, uint16_t len, void (*rx_callback)(u16 len));

/**
 * @brief	This function is used to receive data in DMA mode.
 * @param[in] buf	pointer to data buffer
 * @param[in] len	bytes of data to be sent.
 * @param[in] callback     function pointer to a callback,when the data have been sent this specified function will be called
 * @retval
 *     - \ref WM_SUCCESS
 *     - \ref WM_FAILED
 * @note
 *
 */
int tls_i2s_tx_dma(uint32_t * addr, uint16_t len, tls_i2s_callback callback);

/**
 * @brief	This function is used to receive data in DMA mode.
 * @param[in] addr	pointer to data buffer
 * @param[in] len	bytes of data to be sent.
 * @param[in] callback   function pointer to a callback,when the data have been received this specified function will be called
 *  @retval
 *     - \ref WM_SUCCESS
 *     - \ref WM_FAILED
 *  @note
 *      this function only submit the data
 *      when the data transfer finished the callback function will be called
 */
int tls_i2s_rx_dma(uint32_t * addr, uint16_t len, tls_i2s_callback callback);


/**
 * @brief   Enable I2S module
 *
 */
#define TLS_I2S_ENABLE()		(I2S->CTRL |= I2S_CTRL_EN_MASK)

/**
 * @brief   Disable I2S module
 *
 */
#define TLS_I2S_DISABLE()		(I2S->CTRL &= ~I2S_CTRL_EN_MASK)

/**
 * @brief   Enable  clock of I2S module
 *
 */
#define TLS_I2S_CLK_ENABLE()   \
	do { \
		tls_bitband_write(HR_CLK_I2S_CTL, HR_CLK_I2S_GATE_Pos , 1); \
	} while(0)

/**
 * @brief   Disnable  clock of I2S module
 *
 */
#define TLS_I2S_CLK_DISABLE()   \
	do { \
		tls_bitband_write(HR_CLK_I2S_CTL, HR_CLK_I2S_GATE_Pos , 0); \
	} while(0)

/**
 * @brief   Enable transmit of I2S module
 *
 */
#define TLS_I2S_TX_ENABLE()   \
	do { \
		tls_bitband_write(HR_I2S_CTRL, I2S_CTRL_TXE_Pos, 1); \
	} while(0)

/**
 * @brief   Disable transmit of I2S module
 *
 */
#define TLS_I2S_TX_DISABLE()   \
	do { \
		tls_bitband_write(HR_I2S_CTRL, I2S_CTRL_TXE_Pos, 0); \
	} while(0)

/**
 * @brief   Enable receive of I2S module
 *
 */
#define TLS_I2S_RX_ENABLE() \
	do { \
		tls_bitband_write(HR_I2S_CTRL, I2S_CTRL_RXE_Pos, 1); \
		} while(0)

/**
 * @brief   Disable receive of I2S module
 *
 */
#define TLS_I2S_RX_DISABLE() \
	do { \
		tls_bitband_write(HR_I2S_CTRL, I2S_CTRL_RXE_Pos, 0); \
		} while(0)

/**
 * @brief   Clear receive FIFO
 *
 */
#define TLS_I2S_RX_FIFO_CLEAR() \
	do { \
		tls_bitband_write(HR_I2S_CTRL, I2S_CTRL_RX_CLR_Pos, 1); \
		} while(0)

/**
 * @brief   Clear transmit FIFO
 *
 */
#define TLS_I2S_TX_FIFO_CLEAR() \
	do { \
		tls_bitband_write(HR_I2S_CTRL, I2S_CTRL_TX_CLR_Pos, 1); \
		} while(0)

/**
 * @brief   Enable transmit DMA request
 *
 */
#define TLS_I2S_TXDMA_ENABLE()		(I2S->CTRL |= I2S_CTRL_TXDMA_MASK)

/**
 * @brief   Disnable transmit DMA request
 *
 */
#define TLS_I2S_TXDMA_DISABLE()  	(I2S->CTRL &= ~I2S_CTRL_TXDMA_MASK)

/**
 * @brief   Enable receive DMA request
 *
 */
#define TLS_I2S_RXDMA_ENABLE()		(I2S->CTRL |= I2S_CTRL_RXDMA_MASK)

/**
 * @brief   Disnable receive DMA request
 *
 */
#define TLS_I2S_RXDMA_DISABLE()	(I2S->CTRL &= ~I2S_CTRL_RXDMA_MASK)

/**
 * @brief  Left channel zero-cross detection enable
 *
 */
#define TLS_I2S_LZC_ENABLE()	(I2S->CTRL |= I2S_CTRL_LZCEN_MASK)

/**
 * @brief  Left channel zero-cross detection disable
 *
 */
#define TLS_I2S_LZC_DISABLE()	(I2S->CTRL &= ~I2S_CTRL_LZCEN_MASK)

/**
 * @brief  Right channel zero-cross detection enable
 *
 */
#define TLS_I2S_RZC_ENABLE()	(I2S->CTRL |= I2S_CTRL_RZCEN_MASK)

/**
 * @brief  Right channel zero-cross detection disable
 *
 */
#define TLS_I2S_RZC_DISABLE()	(I2S->CTRL &= ~I2S_CTRL_RZCEN_MASK)

/** @brief  Enable transmit mute
 *
 */
#define TLS_I2S_TX_MUTE_ENABLE()	(I2S->CTRL |= I2S_CTRL_MUTE_MASK)

/**
 * @brief   Disable transmit mute
 *
 */
#define TLS_I2S_TX_MUTE_DISABLE()  (I2S->CTRL &= ~ I2S_CTRL_MUTE_MASK)

/**
 * @brief  Set receive FIFO threshold level
 * @param[in] level  The value set to Rx threshold level
 *
 */
static __inline__ void tls_i2s_set_rxth(uint8_t level)
{
    if (level > 8)
    {
        level = 3;
    }
    I2S->CTRL &= ~ I2S_CTRL_RXTH_MASK;
    I2S->CTRL |= ((level - 1)<<I2S_CTRL_RXTH_Pos);
}

/**
 * @brief   Set transmit FIFO threshold level
 * @param[in] level   The value set to Tx threshold level
 */
static __inline__ void tls_i2s_set_txth(uint8_t level)
{
	if (level > 8)
		level = 4;
	I2S->CTRL &= ~ I2S_CTRL_TXTH_MASK;
	I2S->CTRL |= (level<<I2S_CTRL_TXTH_Pos);
}

/**
 * @brief   Enable or disable receiver of I2S module
 * @param[in] enable   When one enable the receiver of I2S module, when zero disable
 */
static __inline__ void tls_i2s_channel_sel(I2S_CHANNEL_SEL channel_sel)
{
	I2S->CTRL &= ~ I2S_CTRL_CHSEL_MASK;
	I2S->CTRL |= channel_sel;
}

/**
 * @brief   Enables or disables the specified I2S interrupts
 * @param[in] i2s_int_mask	specifies the I2S interrupt sources to be enabled or disabled.
 *   This parameter can be one of the following values:
 *     - \ref I2S_INT_MASK_LZC
 *     - \ref I2S_INT_MASK_RZC
 *     - \ref I2S_INT_MASK_TXDONE
 *     - \ref I2S_INT_MASK_TXTH
 *     - \ref I2S_INT_MASK_TXOV
 *     - \ref I2S_INT_MASK_TXUD
 *     - \ref I2S_INT_MASK_RXDONE
 *     - \ref I2S_INT_MASK_RXTH
 *     - \ref I2S_INT_MASK_RXOV
 *     - \ref I2S_INT_MASK_RXUD
 * @param[in] enable 	when set ,enable interrupt, zero disabled
 * @retval None
 */
static __inline__ void tls_i2s_int_config(uint32_t  i2s_int_mask, uint8_t enable)
{
	if(enable)
	{
		I2S->INT_MASK &= ~i2s_int_mask;
	}
	else
	{
		I2S->INT_MASK |= i2s_int_mask;
	}
}

/**
 * @brief  Enable master clock (MCLK).
 * @param[in] clock   The target MCLK clock rate.
 * @retval   actual MCLK clock rate
 * @details Set the master clock rate according to u32BusClock parameter and enable master clock output.
 *          The actual master clock rate may be different from the target master clock rate.
 *          The real master clock rate will be returned for reference.
 */
uint32_t tls_i2s_mclk_enable(uint32_t clock);

/**
 * @brief  disable master clock (MCLK).
 *
 */
void tls_i2s_mclk_disable(void);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif  /** __WM_I2S_H*/


/*** (C) COPYRIGHT 2014 Winner Microelectronics Co., Ltd. ***/
