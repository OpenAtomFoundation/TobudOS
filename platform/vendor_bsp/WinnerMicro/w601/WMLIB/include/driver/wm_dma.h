/**
 * @file    wm_dma.h
 *
 * @brief   DMA Driver Module
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef __WM_DMA_H_
#define __WM_DMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TLS_DMA_SEL_UART_RX       0
#define TLS_DMA_SEL_UART_TX       1
#define TLS_DMA_SEL_PWM_CAP0       2
#define TLS_DMA_SEL_PWM_CAP1       3
#define TLS_DMA_SEL_LSSPI_RX       4
#define TLS_DMA_SEL_LSSPI_TX       5
#define TLS_DMA_SEL_SDADC_CH0      6
#define TLS_DMA_SEL_SDADC_CH1      7
#define TLS_DMA_SEL_SDADC_CH2      8
#define TLS_DMA_SEL_SDADC_CH3      9
#define TLS_DMA_SEL_SDADC_CH4      10
#define TLS_DMA_SEL_SDADC_CH5      11
#define TLS_DMA_SEL_SDADC_CH6      12
#define TLS_DMA_SEL_SDADC_CH7      13
#define TLS_DMA_SEL_I2S_RX         14
#define TLS_DMA_SEL_I2S_TX         15


#define TLS_DMA_FLAGS_HARD_MODE                 (1   << 0)
#define TLS_DMA_FLAGS_CHAIN_MODE                (1   << 1)
#define TLS_DMA_FLAGS_CHANNEL_SEL(n)            ((n) << 2)
#define TLS_DMA_FLAGS_CHAIN_LINK_EN             (1   << 6)
#define TLS_DMA_FLAGS_CHANNEL_VALID             (1   << 7)


#define TLS_DMA_DESC_VALID                      (1U  << 31)
#define TLS_DMA_DESC_CTRL_SRC_ADD_INC           (1   << 0)
#define TLS_DMA_DESC_CTRL_DEST_ADD_INC          (1   << 2)
#define TLS_DMA_DESC_CTRL_DATA_SIZE_BYTE        (0   << 4)
#define TLS_DMA_DESC_CTRL_DATA_SIZE_SHORT       (1   << 4)
#define TLS_DMA_DESC_CTRL_DATA_SIZE_WORD        (2   << 4)
#define TLS_DMA_DESC_CTRL_BURST_SIZE1           (0   << 6)
#define TLS_DMA_DESC_CTRL_BURST_SIZE4           (1   << 6)
#define TLS_DMA_DESC_CTRL_TOTAL_BYTES(n)        ((n) << 7)


/* dma interrupt flags */
#define TLS_DMA_IRQ_BURST_DONE                  (1 << 0)
#define TLS_DMA_IRQ_TRANSFER_DONE               (1 << 1)
#define TLS_DMA_IRQ_BOTH_DONE                   (TLS_DMA_IRQ_BURST_DONE | TLS_DMA_IRQ_TRANSFER_DONE)

struct tls_dma_descriptor {
	unsigned int valid;
	unsigned int dma_ctrl;
	unsigned int src_addr;
	unsigned int dest_addr;
	struct tls_dma_descriptor *next;    /**< next dms descriptor */
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
 * @defgroup DMA_Driver_APIs DMA Driver APIs
 * @brief DMA driver APIs
 */

/**
 * @addtogroup DMA_Driver_APIs
 * @{
 */

/**
 * @brief          	This function is used to clear dma interrupt flag.
 *
 * @param[in]     	ch		Channel no.[0~7]
 * @param[in]     	flags		Flags setted to TLS_DMA_IRQ_BURST_DONE, TLS_DMA_IRQ_TRANSFER_DONE, TLS_DMA_IRQ_BOTH_DONE.
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_dma_irq_clr(unsigned char ch, unsigned char flags);


/**
 * @brief          	This function is used to register dma interrupt callback function.
 *
 * @param[in]     	ch		Channel no.[0~7]
 * @param[in]     	callback	is the dma interrupt call back function.
 * @param[in]     	arg	the param of the callback function.
 * @param[in]     	flags		Flags setted to TLS_DMA_IRQ_BURST_DONE, TLS_DMA_IRQ_TRANSFER_DONE, TLS_DMA_IRQ_BOTH_DONE.
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_dma_irq_register(unsigned char ch, void (*callback)(void *p), void *arg, unsigned char flags);


/**
 * @brief          This function is used to register dma interrupt
 *
 * @param[in]     	ch		DMA channel no.[0~7]
 *
 * @return         None
 *
 * @note           	None
 */
int tls_dma_wait_complt(unsigned char ch);


/**
 * @brief          This function is used to Start the DMA controller by Wrap
 *
 * @param[in]     	 autoReload 	Does restart when current transfer complete?	
 * @param[in]     	 ch            	Channel no.[0~7]
 * @param[in]     	 pDmaDesc    	Pointer to DMA channel descriptor structure.
 *
 * @retval         	Always STATUS_SUCCESS.	
 *
 * @note
 *                  DMA Descriptor:
 *            		+--------------------------------------------------------------+
 *            		|Vld[31] |                    RSV                              |
 *            	 	+--------------------------------------------------------------+
 *            	 	|                  RSV           |         Dma_Ctrl[16:0]      |
 *            	 	+--------------------------------------------------------------+
 *            	 	|                         Src_Addr[31:0]                       |
 *            	 	+--------------------------------------------------------------+
 *            	 	|                         Dest_Addr[31:0]                      |
 *            	 	+--------------------------------------------------------------+
 *            	 	|                       Next_Desc_Add[31:0]                    |
 *            	 	+--------------------------------------------------------------+
 */
unsigned char tls_dma_start_by_wrap(unsigned char ch, struct tls_dma_descriptor *dma_desc,
                                    unsigned char auto_reload, unsigned short src_zize,
                                    unsigned short dest_zize);


/**
 * @brief          This function is used to Wait until DMA operation completes
 *
 * @param[in]     	 autoReload 	Does restart when current transfer complete?	
 * @param[in]     	 ch            	Channel no.[0~7]
 * @param[in]     	 pDmaDesc    	Pointer to DMA channel descriptor structure.
 *
 * @retval         	 Always STATUS_SUCCESS.
 *
 * @note
 *                  DMA Descriptor:
 *            		+--------------------------------------------------------------+
 *            		|Vld[31] |                    RSV                              |
 *            	 	+--------------------------------------------------------------+
 *            	 	|                  RSV           |         Dma_Ctrl[16:0]      |
 *            	 	+--------------------------------------------------------------+
 *            	 	|                         Src_Addr[31:0]                       |
 *            	 	+--------------------------------------------------------------+
 *            	 	|                         Dest_Addr[31:0]                      |
 *            	 	+--------------------------------------------------------------+
 *            	 	|                       Next_Desc_Add[31:0]                    |
 *            	 	+--------------------------------------------------------------+
 */
unsigned char tls_dma_start(unsigned char ch, struct tls_dma_descriptor *dma_desc,
                            unsigned char auto_reload);

/**
 * @brief          This function is used to To stop current DMA channel transfer
 *
 * @param[in]      ch    channel no. to be stopped
 *
 * @retval         	Always STATUS_SUCCESS
 *
 * @note           	If channel stop, DMA_CHNL_CTRL_CHNL_ON bit in DMA_CHNLCTRL_REG is cleared.
 */
unsigned char tls_dma_stop(unsigned char ch);


/**
 * @brief          	This function is used to Request a free dma channel.
 *				If ch is 0, the function will select a random free channel,
 *		         	else return the selected channel no. if free.
 *
 * @param[in]      ch       channel no.
 * @param[in]      flags    flags setted to selected channel
 *
 * @return         Channel no. that is free now
 *
 * @note           	 Channel no. that is free now
 */
unsigned char tls_dma_request(unsigned char ch, unsigned char flags);


/**
 * @brief          This function is used to Free the DMA channel when not use
 *
 * @param[in]      ch    channel no. that is ready to free
 *
 * @return         None
 *
 * @note           None
 */
void tls_dma_free(unsigned char ch);


/**
 * @brief          This function is used to Initialize DMA Control
 *
 * @param[in]      None
 *
 * @return         None
 *
 * @note           None
 */
void tls_dma_init(void);


/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* end of __WM_DMA_H_ */

/*** (C) COPYRIGHT 2014 Winner Microelectronics Co., Ltd. ***/

