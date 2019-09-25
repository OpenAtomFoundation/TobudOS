/**************************************************************************//**
 * @file     wm_i2s.c
 * @author
 * @version
 * @date
 * @brief
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd. All rights reserved.
 *****************************************************************************/

#define TLS_CONFIG_I2S  1

#if TLS_CONFIG_I2S

#include "wm_i2s.h"
#include "string.h"
#include "wm_debug.h"
#include "wm_irq.h"
#include "wm_config.h"
#include "wm_mem.h"

#define I2S_CLK   (160000000)

#define WM_I2S_FIFO_LEN			(8)
static  tls_i2s_port_t  tls_i2s_port;

static void tls_i2s_fill_txfifo(void);

/**
 * @brief 	This function is used to initial i2s port.
 * @param[in] opts 	the i2s setting options,if this param is NULL,this function will use the default options.
 * @retval
 *	- \ref WM_SUCCESS
 *	- \ref WM_FAILED
 *
 */
int tls_i2s_port_init(tls_i2s_options_t *opts)
{
	tls_i2s_options_t opt;

	tls_i2s_port.regs = I2S;

	if(NULL == opts)
	{
		memset(&opt, 0, sizeof(tls_i2s_options_t));
		opt.data_width = I2S_CTRL_DATABIT_16;
		opt.format = I2S_CTRL_FORMAT_I2S;
		opt.stereo_mono = I2S_CTRL_STERO;
		opt.sample_rate = 8000;
		opt.tx_en = 1;
		tls_i2s_config(&opt);
	}
	else
	{
		tls_i2s_config(opts);
	}
	/* enable i2s interrupt */
	NVIC_ClearPendingIRQ(I2S_IRQn);
	tls_irq_enable(I2S_INT);

	return WM_SUCCESS;
}

/**
 * @brief	This function is used to initial i2s port.
 * @param[in] opts	the i2s setting options,if this param is NULL,this function will use the default options.
 *
 * @retval
 *
 * @note
 */
void tls_i2s_config(tls_i2s_options_t *opts)
{
	tls_i2s_port.regs->CTRL &= ~(I2S_CTRL_FORMAT_MASK | \
							I2S_CTRL_DATABIT_MASK | \
							I2S_CTRL_STEREO_MONO_MASK);
	tls_i2s_port.regs->CTRL = opts->data_width  | opts->format | opts->stereo_mono;
	if (opts->rx_en)
	{
	    TLS_I2S_RX_ENABLE();
	    TLS_I2S_RX_FIFO_CLEAR();
	}
	if (opts->tx_en)
	{
	    TLS_I2S_TX_ENABLE();
	    TLS_I2S_TX_FIFO_CLEAR();
	}
	tls_i2s_set_freq(opts->sample_rate);
}

/**
 * @brief
 * 	set the frequency of the i2s port.
 *
 * @param[in] freq
 *	the required frequency of the i2s module
 *
 * @retval
 *
 */
void tls_i2s_set_freq(uint32_t freq)
{
	uint32_t div;
	uint32_t reg;
	uint8_t width, stereo;

	reg = I2S->CTRL;
	width = (((reg>>4)&0x03)+1)<<3;
	stereo = tls_bitband_read(HR_I2S_CTRL, I2S_CTRL_STEREO_MONO_Pos) ? 1:2;
	div = (I2S_CLK + freq * width * stereo)/(freq * width * stereo) - 1;

	*(volatile uint32_t *)HR_CLK_I2S_CTL &= ~0xFF00;
	*(volatile uint32_t *)HR_CLK_I2S_CTL |= (uint32_t)div<<8;
}

/**
 * @brief
 * 	set the frequency of the i2s port.
 *
 * @param[in] freq
 *	the required frequency of the i2s module
 * @param[in] exclk
 *	the frequency of the ext clock
 *
 * @retval
 *
 * @note
 *		this function will be called after tls_i2s_port_init function
 */
void wm_i2s_set_freq_exclk(uint32_t freq, uint32_t exclk)
{
	uint32_t div;
	uint32_t temp;
	uint8_t wdwidth, stereo;
	temp = I2S->CTRL;
	wdwidth = (((temp>>4)&0x03)+1)<<3;
	stereo = tls_bitband_read(HR_I2S_CTRL, 22) ? 1:2;	
	div = (exclk * 2 + freq * wdwidth * stereo)/(2* freq * wdwidth * stereo) - 1;
	*(volatile uint32_t *)0x40000718 &= ~0x3FF00;
	*(volatile uint32_t *)0x40000718 |= (uint32_t)div<<8;	
	*(volatile uint32_t *)0x40000718 |= 0x01;
}
void I2S_IRQHandler(void)
{
	uint8_t rx_fifocnt;

	/** LZC */
	if (tls_bitband_read(HR_I2S_INT_SRC, I2S_FLAG_LZC_Pos) && !tls_bitband_read(HR_I2S_INT_MASK, I2S_FLAG_LZC_Pos))
	{
		tls_reg_write32(HR_I2S_INT_SRC, I2S_FLAG_LZC);
	}
	/** RZC */
	if (tls_bitband_read(HR_I2S_INT_SRC, I2S_FLAG_RZC_Pos) && !tls_bitband_read(HR_I2S_INT_MASK, I2S_FLAG_RZC_Pos))
	{
		tls_reg_write32(HR_I2S_INT_SRC, I2S_FLAG_RZC);
	}
	/**  Tx Done*/
	if (tls_bitband_read(HR_I2S_INT_SRC, I2S_FLAG_TXDONE_Pos) && !tls_bitband_read(HR_I2S_INT_MASK, I2S_FLAG_TXDONE_Pos))
	{
		tls_reg_write32(HR_I2S_INT_SRC, I2S_FLAG_TXDONE);
	}
	/** TxTH*/
	if (tls_bitband_read(HR_I2S_INT_SRC, I2S_FLAG_TXTH_Pos)&& !tls_bitband_read(HR_I2S_INT_MASK, I2S_FLAG_TXTH_Pos))
	{
		tls_reg_write32(HR_I2S_INT_SRC, I2S_FLAG_TXTH);
		tls_i2s_fill_txfifo();
	}
	/** TXOV*/
	if (tls_bitband_read(HR_I2S_INT_SRC, I2S_FLAG_TXOV_Pos) && !tls_bitband_read(HR_I2S_INT_MASK, I2S_FLAG_TXOV_Pos))
	{
		tls_reg_write32(HR_I2S_INT_SRC, I2S_FLAG_TXOV);
	}
	/** TXUD*/
	if (tls_bitband_read(HR_I2S_INT_SRC, I2S_FLAG_TXUD_Pos) && !tls_bitband_read(HR_I2S_INT_MASK, I2S_FLAG_TXUD_Pos))
	{
		tls_reg_write32(HR_I2S_INT_SRC, I2S_FLAG_TXUD);
	}
	/** Rx Done*/
	if (tls_bitband_read(HR_I2S_INT_SRC, I2S_FLAG_RXDONE_Pos) && !tls_bitband_read(HR_I2S_INT_MASK, I2S_FLAG_RXDONE_Pos))
	{
		tls_reg_write32(HR_I2S_INT_SRC, I2S_FLAG_RXDONE);
	}
	/** RxTH */
	if (tls_bitband_read(HR_I2S_INT_SRC, I2S_FLAG_RXTH_Pos)&& !tls_bitband_read(HR_I2S_INT_MASK, I2S_FLAG_RXTH_Pos))
	{
		tls_reg_write32(HR_I2S_INT_SRC, I2S_FLAG_RXTH);
		rx_fifocnt = tls_i2s_port.regs->INT_STATUS & I2S_RX_FIFO_CNT_MASK;
		if (tls_i2s_port.rx_buf.buf != NULL)
        {      
    		while(rx_fifocnt-- > 0)
    		{
				tls_i2s_port.rx_buf.buf[tls_i2s_port.rx_buf.index] = tls_i2s_port.regs->RX;
	            tls_i2s_port.rx_buf.index++;
    		}
            if (tls_i2s_port.rx_buf.index >= (tls_i2s_port.rx_buf.len))
			{
				tls_i2s_port.rx_buf.buf = NULL;
				tls_i2s_int_config(I2S_INT_MASK_RXTH, 0);
				if (tls_i2s_port.rx_callback != NULL)
				{
					tls_i2s_port.rx_callback(tls_i2s_port.rx_buf.len);
				}
				tls_i2s_port.rx_buf.index = 0;
			}
        }

	}
	/** RXOV*/
	if (tls_bitband_read(HR_I2S_INT_SRC, I2S_FLAG_RXOV_Pos) && !tls_bitband_read(HR_I2S_INT_MASK, I2S_FLAG_RXOV_Pos))
	{
		tls_reg_write32(HR_I2S_INT_SRC, I2S_FLAG_RXOV);
	}
	/** RXUD*/
	if (tls_bitband_read(HR_I2S_INT_SRC, I2S_FLAG_RXUD_Pos) && !tls_bitband_read(HR_I2S_INT_MASK, I2S_FLAG_RXUD_Pos))
	{
		tls_reg_write32(HR_I2S_INT_SRC, I2S_FLAG_RXUD);
	}
}

/**
 * @brief
 *	This function is used to register i2s rx interrupt.
 *
 * @param[in] rx_callback
 * 	is the i2s interrupt call back function.
 *
 * @retval
 *
 */

void tls_i2s_rx_register( void (*rx_callback)(u16 len))
{
	tls_i2s_port.rx_callback = rx_callback;
}

/**
 * @brief
 *	This function is used to register i2s tx interrupt.
 *
 * @param[in] tx_callback
 * 	is the i2s interrupt call back function.
 *
 * @retval
 *
 */
void tls_i2s_tx_register(void (* tx_callback)(u16 len))
{
    tls_i2s_port.tx_callback = tx_callback;
}

static void tls_i2s_fill_txfifo()
{
    s8 fifo_left = 0;

	if (tls_i2s_port.tx_buf.index < tls_i2s_port.tx_buf.len)
	{
        fifo_left = I2S_MAX_RXTXFIFO_LEVEL - ((tls_i2s_port.regs->INT_STATUS & I2S_TX_FIFO_CNT_MASK)>>4);
        while (fifo_left--)
        {
        	tls_i2s_port.regs->TX = *(uint32_t *)(tls_i2s_port.tx_buf.buf + tls_i2s_port.tx_buf.index);
            tls_i2s_port.tx_buf.index ++;
        }
        if(tls_i2s_port.tx_buf.index >= tls_i2s_port.tx_buf.len)
        {
            while((tls_i2s_port.regs->INT_STATUS & I2S_TX_FIFO_CNT_MASK)>>4);
            if(tls_i2s_port.tx_sem)
            {			
    		    memset(&tls_i2s_port.tx_buf, 0, sizeof(tls_i2s_buf));
    		    tls_os_sem_release(tls_i2s_port.tx_sem);
    		    return;
            }
            if (tls_i2s_port.tx_callback)
            {
                tls_i2s_port.tx_callback(tls_i2s_port.tx_buf.len);
            }
    	}
    }
}

/**
 * @brief
 *	This function is used to transfer data in blocking mode.
 *
 * @param[in]	buf
 *	pointer to the transfering data.
 *
 * @param[in] len
 *	is the data length.
 *
 * @retval
 *
 */
int tls_i2s_tx_block(uint32_t *buf, uint16_t len)
{
    uint8_t err;


    err = tls_os_sem_create(&tls_i2s_port.tx_sem, 0);
    if (err != TLS_OS_SUCCESS)
    {
        TLS_DBGPRT_ERR("\ni2s tx sem create fail\n");
        return WM_FAILED;
    }
	TLS_I2S_TX_FIFO_CLEAR();
	tls_reg_write32(HR_I2S_INT_SRC, 0x1FF);
	NVIC_ClearPendingIRQ(I2S_IRQn);

    tls_i2s_port.tx_buf.buf = buf;
    tls_i2s_port.tx_buf.len = len;
    tls_i2s_port.tx_buf.index = 0;

    tls_i2s_set_txth(5);
    tls_i2s_int_config(I2S_INT_MASK_TXTH, 1);
    TLS_I2S_TX_ENABLE();
    TLS_I2S_ENABLE();
    
    tls_os_sem_acquire(tls_i2s_port.tx_sem, 0);
    tls_os_sem_delete(tls_i2s_port.tx_sem);
    tls_i2s_port.tx_sem = NULL;
    TLS_I2S_TX_DISABLE();

    return WM_SUCCESS;
}

/**
 * @brief
 *	This function is used to transfer data in non blocking mode.
 *
 * @param[in]	buf
 *	is a buf for user data.
 *
 * @param[in] len
 *	is the data length.
 *
 * @param[in] tx_callback
 *	a function pointer,which will be called when the designated data sent
 *
 * @retval
 *
 * @note the \ref len in words
 */
int tls_i2s_tx_nonblock(uint32_t *buf, uint16_t len, void (*tx_callback)(u16 len))
{
	tls_i2s_port.tx_buf.buf = (uint32_t *)buf;
	tls_i2s_port.tx_buf.len = len;
	tls_i2s_port.tx_buf.index = 0;

	tls_i2s_set_txth(5);
    tls_i2s_int_config(I2S_INT_MASK_TXTH, 1);
	tls_i2s_tx_register( tx_callback);

	TLS_I2S_TX_FIFO_CLEAR();
	TLS_I2S_TX_ENABLE();
	TLS_I2S_ENABLE();
	return WM_SUCCESS;
}

/**
 * @brief
 *	This function is used to receiver data in non blocking mode.
 *
 * @param[in]	buf
 *	the buffer for storing the received data.
 *
 * @param[in] len
 *	the length of the data will be receiving
 *
 * @param[in] rx_callback
 *	a function pointer,which will be called when the demanded data have been received
 *
 * @retval
 *
 */
int tls_i2s_rx_nonblock(uint32_t *buf, uint16_t len, void (*rx_callback)(u16 len))
{
    tls_i2s_port.rx_buf.buf = buf;
    tls_i2s_port.rx_buf.len = len;
    tls_i2s_port.rx_buf.index = 0;

    tls_i2s_set_rxth(5);
    tls_i2s_int_config(I2S_INT_MASK_RXTH, 1);
    tls_i2s_rx_register( rx_callback);

    TLS_I2S_RX_FIFO_CLEAR();
    NVIC_ClearPendingIRQ(I2S_IRQn);
    TLS_I2S_RX_ENABLE();
    TLS_I2S_ENABLE();

    return WM_SUCCESS;
}

/**
 * @brief
 *	This function is used to the tx DMA configuration of the I2S module.
 *
 * @param[in] dma_channel
 *	the free dma channel number which returned by calling function tls_dma_request()
 *
 * @param[in] addr
 *	point to the data buffer will be sent
 *
 * @param[in] len
 *	length of the data to be transfered in bytes
 *
 * @retval
 */
void tls_i2s_tx_dma_config(uint8_t dma_channel, uint32_t * addr, uint16_t len)
{

	struct tls_dma_descriptor DmaDesc;

	TLS_I2S_TX_DISABLE();
	TLS_I2S_TX_FIFO_CLEAR();
	/** Mask i2s txth interrupt*/
	tls_i2s_set_txth(4);
	tls_i2s_int_config(I2S_INT_MASK_TXTH, 0);
	DmaDesc.src_addr = (unsigned int) addr;
    DmaDesc.dest_addr = (unsigned int)HR_I2S_TX;

	DmaDesc.dma_ctrl = TLS_DMA_DESC_CTRL_SRC_ADD_INC | TLS_DMA_DESC_CTRL_DATA_SIZE_WORD | 
                        (len << 7);
	DmaDesc.valid = TLS_DMA_DESC_VALID;
    DmaDesc.next = NULL;
    tls_dma_start(dma_channel, &DmaDesc, 0);

}

/**
 * @brief
 *	datas transmit throuth DMA
 * 
 * @param[in] addr
 *	point to the data buffer will be sent
 * @param[in] len
 *	length of the data to be transfered in bytes
 * @param[in] callback
 *	the callback function when datas transfer finished
 *
 * @retval
 *
 * @note the \ref len in bytes
 */
int tls_i2s_tx_dma(uint32_t * addr, uint16_t len, tls_i2s_callback callback)
{
    uint8_t tx_channel;
    
	TLS_I2S_TX_FIFO_CLEAR();
	tls_reg_write32(HR_I2S_INT_SRC, 0x1FF);
	NVIC_ClearPendingIRQ(I2S_IRQn);

    TLS_I2S_TX_FIFO_CLEAR();
    
    tx_channel = tls_dma_request(WM_I2S_TX_DMA_CHANNEL, TLS_DMA_FLAGS_CHANNEL_SEL(TLS_DMA_SEL_I2S_TX) | TLS_DMA_FLAGS_HARD_MODE);
    if (tx_channel == 0)
    {
        return WM_FAILED;
    }
    if (tls_dma_stop(tx_channel))
    {
        return WM_FAILED;
    }
    tls_i2s_tx_dma_config(tx_channel, addr, len);
    tls_dma_irq_register(tx_channel, callback, NULL, TLS_DMA_IRQ_TRANSFER_DONE);
    TLS_I2S_TXDMA_ENABLE();
    TLS_I2S_TX_ENABLE();
    TLS_I2S_ENABLE();

    return WM_SUCCESS;
}

/**
 * @brief
 *	This function is used to the rx DMA configuration of the I2S module.
 *
 * @param[in] addr
 *	point to the storge buffer of the received data
 *
 * @param[in] len
 *	the storge buffer length in bytes
 *
 * @retval
 */
void tls_i2s_rx_dma_config(uint8_t dma_channel, uint32_t * addr,  uint16_t len)
{

	struct tls_dma_descriptor DmaDesc;

	tls_i2s_int_config(I2S_INT_MASK_RXTH, 0);
	TLS_I2S_RX_DISABLE();
	TLS_I2S_RXDMA_DISABLE();
	TLS_I2S_RX_FIFO_CLEAR();
	/** Mask i2s rxth interrupt*/
	tls_i2s_set_rxth(4);

	tls_i2s_int_config(I2S_INT_MASK_RXTH, 0);
	DmaDesc.src_addr = (int)HR_I2S_RX;
    DmaDesc.dest_addr = (int)addr;

	DmaDesc.dma_ctrl = TLS_DMA_DESC_CTRL_DEST_ADD_INC | TLS_DMA_DESC_CTRL_DATA_SIZE_WORD |
                        (len << 7);
	DmaDesc.valid = TLS_DMA_DESC_VALID;
    DmaDesc.next = NULL;
    tls_dma_start(dma_channel, &DmaDesc, 0);
}

int tls_i2s_rx_dma(uint32_t * addr, uint16_t len, tls_i2s_callback callback)
{
    uint8_t rx_channel;

    rx_channel = tls_dma_request(WM_I2S_RX_DMA_CHANNEL, TLS_DMA_FLAGS_CHANNEL_SEL(TLS_DMA_SEL_I2S_RX) | TLS_DMA_FLAGS_HARD_MODE);
    if (rx_channel == 0)
    {
        return WM_FAILED;
    }
    if (tls_dma_stop(rx_channel))
    {
    	return WM_FAILED;
    }
    tls_i2s_rx_dma_config(rx_channel, addr, len);
    tls_dma_irq_register(rx_channel, callback, NULL, TLS_DMA_IRQ_TRANSFER_DONE);
    TLS_I2S_RXDMA_ENABLE();
    NVIC_ClearPendingIRQ(I2S_IRQn);
    TLS_I2S_RX_ENABLE();
    TLS_I2S_ENABLE();

    return WM_SUCCESS;
}
#if 1
uint8_t tst_flag = 0;

void tls_i2s_tx_dma_callback()
{
	I2S->TX = 0x00;
	while(!tls_bitband_read(HR_I2S_INT_SRC, 7));
    TLS_I2S_TX_DISABLE();
    DMA_CHNLCTRL_REG(WM_I2S_TX_DMA_CHANNEL) |= DMA_CHNL_CTRL_CHNL_OFF;
	tls_dma_free(WM_I2S_TX_DMA_CHANNEL);
	tst_flag = 1;
}
void tls_i2s_rx_dma_callback()
{	
    TLS_I2S_RX_DISABLE();
    DMA_CHNLCTRL_REG(WM_I2S_RX_DMA_CHANNEL) |= DMA_CHNL_CTRL_CHNL_OFF;
	tls_dma_free(WM_I2S_RX_DMA_CHANNEL);
	tst_flag = 1;
}
void tls_i2s_rx_callback()
{	
	TLS_I2S_RX_DISABLE();
	tst_flag = 1;
}
void tls_i2s_test()
{
	uint32_t *ptr = NULL;
	uint32_t len;
    uint32_t * i2s_tx_test = tls_mem_alloc(1024);
	tls_i2s_port.opts.data_width = I2S_CTRL_DATABIT_16;
	tls_i2s_port.opts.format = I2S_CTRL_FORMAT_I2S;
	tls_i2s_port.opts.tx_en = 1;
	tls_i2s_port.opts.sample_rate = 8000;
	tls_i2s_port.opts.stereo_mono = I2S_CTRL_STERO;
	tls_i2s_port_init(&tls_i2s_port.opts);
	if (i2s_tx_test == NULL)
    {
        return;
    }
	ptr = i2s_tx_test;
    for (len = 0; len < 256; len++)
    {
        *ptr++ = 0xABCD0200 + len;
    }
	i2s_tx_test[255] = 0xABCD02FA;
	tls_i2s_tx_dma(i2s_tx_test, 1024,tls_i2s_tx_dma_callback);
	while(!tst_flag);
	tst_flag = 0;	
	tls_i2s_tx_dma(i2s_tx_test, 1024,tls_i2s_tx_dma_callback);
	while(!tst_flag);
	tst_flag = 0;
}
#endif
#endif  /** TLS_CONFIG_I2S */


/*** (C) COPYRIGHT 2014 Winner Microelectronics Co., Ltd. ***/
