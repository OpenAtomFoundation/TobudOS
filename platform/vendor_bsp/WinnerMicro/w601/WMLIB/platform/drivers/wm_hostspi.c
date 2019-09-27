/**
 * @file    wm_hostspi.c
 *
 * @brief   host spi Driver Module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wm_regs.h"
#include "wm_irq.h"
#include "wm_gpio.h"
#include "wm_hostspi.h"
#include "wm_dma.h"
#include "wm_dbg.h"
//#include "wm_mem.h"
#include "wm_cpu.h"
#include "wm_spi_hal.h"
//#include "wm_wl_task.h"
#include "tls_common.h"

static struct tls_spi_port *spi_port = NULL;

#define MSG_QUEUE_SIZE      (8)

#define SPI_SCHEDULER_STK_SIZE      (128)
static u32 spi_scheduler_stk[SPI_SCHEDULER_STK_SIZE];

#define SPI_SCHED_MSG_START_ENGINE      (1)
#define SPI_SCHED_MSG_TX_FIFO_READY      (2)
#define SPI_SCHED_MSG_RX_FIFO_READY      (3)
#define SPI_SCHED_MSG_TRANSFER_COMPLETE      (4)
#define SPI_SCHED_MSG_EXIT      (5)
#define SPI_SCHED_MSG_END      (6)
static void spi_start_transfer(u32 transfer_bytes);

int tls_spi_async(struct tls_spi_message *message);
int tls_spi_sync(struct tls_spi_message *message);

#ifdef SPI_USE_DMA
static void SpiMasterInit(u8 mode, u8 cs_active, u32 fclk)
{
	tls_sys_clk sysclk;

    SPIM_CHCFG_REG = SPI_CLEAR_FIFOS;
    while (SPIM_CHCFG_REG & SPI_CLEAR_FIFOS);

	tls_sys_clk_get(&sysclk);						//获取实际频率

    SPIM_CLKCFG_REG = sysclk.apbclk*UNIT_MHZ/(fclk*2) - 1;;
    SPIM_SPICFG_REG = 0;
    SPIM_SPICFG_REG = SPI_FRAME_FORMAT_MOTO | SPI_SET_MASTER_SLAVE(SPI_MASTER) | mode;
    SPIM_INTEN_REG = 0xff;      /* Disable INT */

    tls_dma_init();
}

int spiWaitIdle(void)
{
    unsigned long regVal;
    unsigned long timeout = 0;

    do
    {
        timeout++;
        if (timeout > 0x4FFFFF) // 5s
            return TLS_SPI_STATUS_EBUSY;
        regVal = SPIM_SPISTATUS_REG;
    }
    while (regVal & (1 << 12));

    return TLS_SPI_STATUS_OK;
}

static int SpiDmaBlockWrite(u8 * data, u32 len, u8 ifusecmd, u32 cmd)
{
    unsigned char dmaCh = 0;
    struct tls_dma_descriptor DmaDesc;
    u32 txlen, txlenbk;
    u32 i, blocknum, blocksize = 0;
    int ret = TLS_SPI_STATUS_OK;
    int txcmdover = 0;

    if (NULL == data)
    {
        return TLS_SPI_STATUS_EINVAL;
    }
    if (spiWaitIdle())
        return TLS_SPI_STATUS_EBUSY;
    SPIM_CHCFG_REG = SPI_CLEAR_FIFOS;
    while (SPIM_CHCFG_REG & SPI_CLEAR_FIFOS);

    if (ifusecmd)
        SPIM_TXDATA_REG = cmd;

    if (len % 4)
    {
        txlen = len & 0xfffffffc;   // 不够字的最后单独发
    }
    else
    {
        txlen = len;
    }

    txlenbk = txlen;
    if (txlen > 0)
    {
        blocknum = txlen / SPI_DMA_MAX_TRANS_SIZE;

    /* Request DMA Channel */
        dmaCh = tls_dma_request(1,TLS_DMA_FLAGS_CHANNEL_SEL(TLS_DMA_SEL_LSSPI_TX) |
                            TLS_DMA_FLAGS_HARD_MODE);
        for (i = 0; i <= blocknum; i++)
        {
            DmaDesc.src_addr = (int) (data + i * SPI_DMA_MAX_TRANS_SIZE);
            DmaDesc.dest_addr = HR_SPI_TXDATA_REG;
            blocksize = (txlen > SPI_DMA_MAX_TRANS_SIZE) ? SPI_DMA_MAX_TRANS_SIZE : txlen;

            if (0 == blocksize)
                break;
            DmaDesc.dma_ctrl = TLS_DMA_DESC_CTRL_SRC_ADD_INC |
                                     TLS_DMA_DESC_CTRL_DATA_SIZE_WORD |
                                     TLS_DMA_DESC_CTRL_TOTAL_BYTES(blocksize);
            DmaDesc.valid = TLS_DMA_DESC_VALID;
            DmaDesc.next = NULL;
            tls_dma_start(dmaCh, &DmaDesc, 0);
            /* Enable SPI TX DMA */
            SPIM_MODECFG_REG = SPI_RX_TRIGGER_LEVEL(0) | SPI_TX_TRIGGER_LEVEL(0) | SPI_TX_DMA_ON;
            SPIM_SPITIMEOUT_REG = SPI_TIMER_EN | SPI_TIME_OUT((u32) 0xffff);

            if (ifusecmd && 0 == i)
            {
                SPIM_CHCFG_REG = SPI_FORCE_SPI_CS_OUT | SPI_TX_CHANNEL_ON |
                                        SPI_CONTINUE_MODE | SPI_START |
                                        SPI_VALID_CLKS_NUM(((blocksize + 4) * 8));
                txcmdover = 1;
            }
            else
                SPIM_CHCFG_REG = SPI_FORCE_SPI_CS_OUT | SPI_TX_CHANNEL_ON | SPI_CONTINUE_MODE |
                                         SPI_START | SPI_VALID_CLKS_NUM((blocksize * 8));

            if (spiWaitIdle())
            {
                ret = TLS_SPI_STATUS_EBUSY;
                break;
            }
        /* Wait Dma Channel Complete and Free Dma channel */
            if (tls_dma_wait_complt(dmaCh))
            {
                ret = TLS_SPI_STATUS_EBUSY;
                break;
            }
            txlen -= blocksize;
        }
        tls_dma_free(dmaCh);
    }
    // tx 不够一个整字的几个字节
    if (len > txlenbk)
    {
        u32 word32 = 0;
        int temp = 0;
        for (i = 0; i < (len - txlenbk); i++)
        {
            word32 |= (data[txlenbk + i] << (i * 8));
        }
        SPIM_TXDATA_REG = word32;
        SPIM_MODECFG_REG = SPI_RX_TRIGGER_LEVEL(0) | SPI_TX_TRIGGER_LEVEL(0);
        SPIM_SPITIMEOUT_REG = SPI_TIMER_EN | SPI_TIME_OUT((u32) 0xffff);
        if (ifusecmd && 0 == txcmdover) // 需要发送命令，但是命令还没有发送出去，发送的字节数需要增加4
            temp = 4;
        SPIM_CHCFG_REG = SPI_FORCE_SPI_CS_OUT | SPI_TX_CHANNEL_ON | SPI_CONTINUE_MODE |
                            SPI_START | SPI_VALID_CLKS_NUM(((temp + len - txlenbk) * 8));
        if (spiWaitIdle())
        {
            ret = TLS_SPI_STATUS_EBUSY;
        }
    }
    SPIM_CHCFG_REG = 0x00000000;
    SPIM_MODECFG_REG = 0x00000000;
    SPIM_SPITIMEOUT_REG = 0x00000000;

    return ret;
}

static int SpiDmaBlockRead(u8 * data, u32 len, u8 * txdata, u8 txlen)
{
    unsigned char dmaCh = 0;
    struct tls_dma_descriptor DmaDesc;
    u32 word32 = 0;
    u32 i;
    u32 rxlen, rxlenbk;
    u8 blocknum;
    u32 blocksize = 0;
    int ret = TLS_SPI_STATUS_OK;

// printf("\nentry SpiDmaBlockRead\n");
    if (NULL == data)
    {
        return TLS_SPI_STATUS_EINVAL;
    }
    if (spiWaitIdle())
    {
        return TLS_SPI_STATUS_EBUSY;
    }

    SPIM_CHCFG_REG = SPI_CLEAR_FIFOS;
    while (SPIM_CHCFG_REG & SPI_CLEAR_FIFOS);

    if (len % 4)
    {
        rxlen = len & 0xfffffffc;   // 不够字的最后单独取
    }
    else
    {
        rxlen = len;
    }
    rxlenbk = rxlen;
    blocknum = rxlen / SPI_DMA_MAX_TRANS_SIZE;

    if (txlen > 0 && txlen <= 32)
    {
        for (i = 0; i < txlen; i++)
        {
            if ((i > 0) && (0 == i % 4))
            {
                SPIM_TXDATA_REG = word32;
                word32 = 0;
            }
            word32 |= (txdata[i] << ((i % 4) * 8));
        }
        SPIM_TXDATA_REG = word32;
    }

/* Request DMA Channel */
    dmaCh = tls_dma_request(1,TLS_DMA_FLAGS_CHANNEL_SEL(TLS_DMA_SEL_LSSPI_RX) |
                        TLS_DMA_FLAGS_HARD_MODE);
    DmaDesc.src_addr = HR_SPI_RXDATA_REG;
    for (i = 0; i <= blocknum; i++)
    {
        if (rxlenbk > 0)
        {
        // 说明接收的数据大于4
        // printf("\ni =%d\n",i);
            DmaDesc.dest_addr = (int) (data + i * SPI_DMA_MAX_TRANS_SIZE);
            blocksize = (rxlen > SPI_DMA_MAX_TRANS_SIZE) ? SPI_DMA_MAX_TRANS_SIZE : rxlen;
            if (0 == blocksize)
                break;
        // printf("\nblocksize= %d\n",blocksize);
            DmaDesc.dma_ctrl =
                TLS_DMA_DESC_CTRL_DEST_ADD_INC | TLS_DMA_DESC_CTRL_BURST_SIZE1 |
                TLS_DMA_DESC_CTRL_DATA_SIZE_WORD |
                TLS_DMA_DESC_CTRL_TOTAL_BYTES(blocksize);
        // word32 = DmaDesc.dma_ctrl;
        // printf("\ndma ctrl = %x\n",DmaDesc.dma_ctrl);
            DmaDesc.valid = TLS_DMA_DESC_VALID;
            DmaDesc.next = NULL;
            tls_dma_start(dmaCh, &DmaDesc, 0);
            /* Enable SPI RX DMA */
            SPIM_MODECFG_REG = SPI_RX_TRIGGER_LEVEL(0) | SPI_TX_TRIGGER_LEVEL(0) |
                                    SPI_RX_DMA_ON;
        }
        else
        {
            SPIM_MODECFG_REG = SPI_RX_TRIGGER_LEVEL(0) | SPI_TX_TRIGGER_LEVEL(0);   // rx数据少于4个byte，不用开DMA
        }
        SPIM_SPITIMEOUT_REG = SPI_TIMER_EN | SPI_TIME_OUT((u32) 0xffff);
        if (0 == blocknum)
        {
            SPIM_CHCFG_REG =
                SPI_FORCE_SPI_CS_OUT | SPI_RX_CHANNEL_ON | SPI_TX_CHANNEL_ON |
                SPI_CONTINUE_MODE | SPI_START |
                SPI_VALID_CLKS_NUM(((len + txlen) * 8)) | SPI_RX_INVALID_BITS(txlen *8);
        }
        else
        {
            if (0 == i)         // 第一次需要打开TX
            {
                SPIM_CHCFG_REG =
                    SPI_FORCE_SPI_CS_OUT | SPI_RX_CHANNEL_ON | SPI_TX_CHANNEL_ON
                    | SPI_CONTINUE_MODE | SPI_START |
                    SPI_VALID_CLKS_NUM(((blocksize + txlen) *8)) | SPI_RX_INVALID_BITS(txlen * 8);
            }
            else if (i == blocknum)
            {
                SPIM_CHCFG_REG =
                    SPI_FORCE_SPI_CS_OUT | SPI_RX_CHANNEL_ON | SPI_CONTINUE_MODE
                    | SPI_START | SPI_VALID_CLKS_NUM((blocksize + len - rxlenbk) * 8);
            }
            else
            {
                SPIM_CHCFG_REG =
                    SPI_FORCE_SPI_CS_OUT | SPI_RX_CHANNEL_ON | SPI_CONTINUE_MODE
                    | SPI_START | SPI_VALID_CLKS_NUM(blocksize * 8);
            }
        }
        if (spiWaitIdle())
        {
            ret = TLS_SPI_STATUS_EBUSY;
            break;
        }

    /* Wait Dma Channel Complete and Free Dma channel */
        if (tls_dma_wait_complt(dmaCh))
        {
            ret = TLS_SPI_STATUS_EBUSY;
            break;
        }
        rxlen -= blocksize;
    }
    tls_dma_free(dmaCh);

    if (len > rxlenbk)          // 取最后的不够一个字的几个byte
    {
        word32 = SPIM_RXDATA_REG;
        *((int *) data + rxlenbk / 4) = word32;
    }
    SPIM_CHCFG_REG = 0x00000000;
    SPIM_MODECFG_REG = 0x00000000;
    SPIM_SPITIMEOUT_REG = 0x00000000;
    return ret;
}
#endif


/**
 * @brief          This function is used to set SPI transfer mode.
 *
 * @param[in]      type             is the transfer type.
 *				                type == SPI_BYTE_TRANSFER	byte transfer
 *	                        	         type == SPI_WORD_TRANSFER	word transfer
 *				                type == SPI_DMA_TRANSFER	DMA transfer
 *
 * @return         None
 *
 * @note           None
 */
void tls_spi_trans_type(u8 type)
{

    spi_port->transtype = type;
    if (SPI_WORD_TRANSFER == type)
    {
        spi_set_endian(0);
    }
    else if (SPI_DMA_TRANSFER == type)
    {
#ifdef SPI_USE_DMA
        SpiMasterInit(spi_port->mode, TLS_SPI_CS_LOW, spi_port->speed_hz);
#endif
    }
}

static void spi_message_init(struct tls_spi_message *m)
{
    memset(m, 0, sizeof(*m));
    dl_list_init(&m->transfers);
}


static void spi_complete(void *arg)
{
    tls_os_sem_t *sem;

    sem = (tls_os_sem_t *) arg;
    tls_os_sem_release(sem);
}

static u32 spi_fill_txfifo(struct tls_spi_transfer *current_transfer,
                           u32 current_remaining_bytes)
{
    u8 fifo_level;
    u16 rw_words;
    u16 rw_bytes;
    u8 data8;
    u8 i;
    u32 data32 = 0;
    u32 tx_remaining_bytes;
    if ((current_transfer == NULL) || (current_remaining_bytes == 0))
        return 0;

    tx_remaining_bytes = current_remaining_bytes;

// printf("ready to write to fifo size - %d.\n", tx_remaining_bytes);
    spi_get_status(NULL, NULL, &fifo_level);

// TLS_DBGPRT_SPI("\nfifo_level 0= %d\n",fifo_level);
    rw_words =
        ((fifo_level > tx_remaining_bytes) ? tx_remaining_bytes : fifo_level) / 4;
    rw_bytes =
        ((fifo_level > tx_remaining_bytes) ? tx_remaining_bytes : fifo_level) % 4;

// TLS_DBGPRT_SPI("write to spi fifo words - %d, bytes - %d.\n", rw_words,
// rw_bytes);

//下面代码17us
    for (i = 0; i < rw_words; i++)
    {
        if (current_transfer->tx_buf)
        {
            if (SPI_BYTE_TRANSFER == spi_port->transtype)
            {
                data32 = 0;
                data32 |= ((u8 *) current_transfer->tx_buf +
                     (current_transfer->len - tx_remaining_bytes))[0] << 24;
                data32 |= ((u8 *) current_transfer->tx_buf +
                     (current_transfer->len - tx_remaining_bytes))[1] << 16;
                data32 |= ((u8 *) current_transfer->tx_buf +
                     (current_transfer->len - tx_remaining_bytes))[2] << 8;
                data32 |= ((u8 *) current_transfer->tx_buf +
                     (current_transfer->len - tx_remaining_bytes))[3] << 0;
            }
            else if (SPI_WORD_TRANSFER == spi_port->transtype)
            {
                data32 = *((u32 *) ((u8 *) current_transfer->tx_buf +
                               current_transfer->len - tx_remaining_bytes));
            }
        }
        else
        {
            data32 = 0xffffffff;
        }
    // TLS_DBGPRT_SPI("write to spi fifo word[%d]: 0x%x.\n", i, data32);
        spi_data_put(data32);
        tx_remaining_bytes -= 4;
    }

    if (rw_bytes)
    {
        data32 = 0;
        for (i = 0; i < rw_bytes; i++)
        {
            if (current_transfer->tx_buf)
            {
                data8 =  ((u8 *) current_transfer->tx_buf)[current_transfer->len -
                                  tx_remaining_bytes];
            }
            else
            {
                data8 = 0xff;
            }
            if (SPI_BYTE_TRANSFER == spi_port->transtype)
                data32 |= data8 << ((3 - i) * 8);
            else if (SPI_WORD_TRANSFER == spi_port->transtype)
            {
                data32 |= data8 << (i * 8);
            }
            tx_remaining_bytes -= 1;
        }
    // TLS_DBGPRT_SPI("write to spi fifo word: 0x%x.\n", data32);
        spi_data_put(data32);
    }

    return (current_remaining_bytes - tx_remaining_bytes);
}

static u32 spi_get_rxfifo(struct tls_spi_transfer *current_transfer,
                          u32 current_remaining_bytes)
{
    u8 fifo_level;
    u8 rw_words;
    u8 rw_bytes;
    u8 data8 = 0;
    u8 i;
    u32 data32;
    u32 rx_remaining_bytes;

    if ((current_transfer == NULL) || (current_remaining_bytes == 0))
        return 0;

    rx_remaining_bytes = current_remaining_bytes;
    spi_get_status(NULL, &fifo_level, NULL);
//  TLS_DBGPRT_SPI("rx fifo level: %d.\n", fifo_level);

    rw_words = fifo_level / 4;
    rw_bytes = fifo_level % 4;

// TLS_DBGPRT_SPI("rx data: %d words, %d bytes.\n", rw_words, rw_bytes);

//下面代码大概10us
    for (i = 0; i < rw_words; i++)
    {

        data32 = spi_data_get();

    // TLS_DBGPRT_SPI("rx data[%d](w): 0x%08x.\n", i, data32);
        if (current_transfer->rx_buf)
        {

            if (SPI_BYTE_TRANSFER == spi_port->transtype)
            {
                data32 = swap_32(data32);
                (((u8 *) current_transfer->rx_buf +
                (current_transfer->len - rx_remaining_bytes)))[0] = (u8) data32;
                (((u8 *) current_transfer->rx_buf +
                  (current_transfer->len - rx_remaining_bytes)))[1] = (u8) (data32 >> 8);
                (((u8 *) current_transfer->rx_buf +
                  (current_transfer->len - rx_remaining_bytes)))[2] = (u8) (data32 >> 16);
                (((u8 *) current_transfer->rx_buf +
                  (current_transfer->len - rx_remaining_bytes)))[3] = (u8) (data32 >> 24);
            }
            else if (SPI_WORD_TRANSFER == spi_port->transtype)
            {
                *((u32 *) ((u8 *) current_transfer->rx_buf +
                           current_transfer->len - rx_remaining_bytes)) = data32;
            }
        }
        rx_remaining_bytes -= 4;

    }

    if (rw_bytes)
    {
        data32 = spi_data_get();
    // TLS_DBGPRT_SPI("\nrx data=%x\n",data32);
        if (current_transfer->rx_buf)
        {
            for (i = 0; i < rw_bytes; i++)
            {
                if (SPI_BYTE_TRANSFER == spi_port->transtype)
                    data8 = (u8) (data32 >> ((3 - i) * 8));
                else if (SPI_WORD_TRANSFER == spi_port->transtype)
                    data8 = (u8) (data32 >> (i * 8));

            // TLS_DBGPRT_SPI("rx data[%d](b): 0x%02x.\n", i, data8);
                ((u8 *) current_transfer->rx_buf)[current_transfer->len -rx_remaining_bytes] = data8;
                rx_remaining_bytes -= 1;
            }
        }
        else
        {
            rx_remaining_bytes -= rw_bytes;
        }
    }

    return (current_remaining_bytes - rx_remaining_bytes);
}

static struct tls_spi_transfer *spi_next_transfer(struct tls_spi_message
                                                  *current_message)
{
    if (current_message == NULL)
    {
        return NULL;
    }

    return dl_list_first(&current_message->transfers, struct tls_spi_transfer, transfer_list);
}

static struct tls_spi_message *spi_next_message(void)
{
    struct tls_spi_message *current_message;

    current_message =
        dl_list_first(&spi_port->wait_queue, struct tls_spi_message, queue);
    if (current_message == NULL)
    {
        return NULL;
    }

    spi_port->current_transfer = spi_next_transfer(current_message);
    current_message->status = SPI_MESSAGE_STATUS_INPROGRESS;

    return current_message;
}

int gSpiCsFlag = 0;
static void spi_start_transfer(u32 transfer_bytes)
{
    if (spi_port->reconfig)
    {
        TLS_DBGPRT_SPI_INFO("reconfig the spi master controller.\n");
        spi_set_mode(spi_port->mode);
        spi_set_chipselect_mode(spi_port->cs_active);
        spi_set_sclk(spi_port->speed_hz);

        spi_port->reconfig = 0;
    }

    spi_set_sclk_length(transfer_bytes * 8, 0);
// if(0 == gSpiCsFlag)
    {
        spi_set_chipselect_mode(SPI_CS_ACTIVE_MODE);
    }
    spi_sclk_start();
}

static void spi_continue_transfer(void)
{
    struct tls_spi_message *current_message;
    struct tls_spi_transfer *current_transfer;
    u32 transfer_bytes;

    current_message = spi_port->current_message;
    current_transfer = spi_port->current_transfer;
    if ((current_message == NULL) || (current_transfer == NULL))
    {
        return;
    }
    transfer_bytes =
        spi_get_rxfifo(current_transfer, spi_port->current_remaining_bytes);

    spi_port->current_remaining_bytes -= transfer_bytes;
    if (spi_port->current_remaining_bytes == 0)
    {
        tls_os_sem_acquire(spi_port->lock, 0);

        dl_list_del(&current_transfer->transfer_list);
        spi_port->current_transfer =
            spi_next_transfer(spi_port->current_message);
        if (spi_port->current_transfer == NULL)
        {
//            tls_sys_clk_set(CPU_CLK_40M);
            spi_set_chipselect_mode(SPI_CS_INACTIVE_MODE);
            current_message->status = SPI_MESSAGE_STATUS_DONE;
            dl_list_del(&current_message->queue);
            spi_port->current_message = spi_next_message();
        }

        tls_os_sem_release(spi_port->lock);

    // TLS_DBGPRT_SPI("get the next spi transfer pair.\n");
        current_transfer = spi_port->current_transfer;
        if (current_transfer != NULL)
        {
            spi_port->current_remaining_bytes = current_transfer->len;
        }
    }

    transfer_bytes =
        spi_fill_txfifo(current_transfer, spi_port->current_remaining_bytes);

    if (transfer_bytes)
    {
        spi_start_transfer(transfer_bytes);
    }

    if (current_message->status == SPI_MESSAGE_STATUS_DONE)
    {
    // TLS_DBGPRT_SPI("current spi transaction finish and notify the
    // submitter.\n");
        current_message->complete(current_message->context);
    }
}

static void spi_scheduler(void *data)
{
    u8 err;
    u32 msg;
    u32 transfer_bytes;
    struct tls_spi_transfer *current_transfer;

    while (1)
    {
        err = tls_os_queue_receive(spi_port->msg_queue, (void **) &msg, 4, 0);
        if (err == TLS_OS_SUCCESS)
        {
            switch (msg)
            {
                case SPI_SCHED_MSG_START_ENGINE:
//                    tls_sys_clk_set(CPU_CLK_80M);			//80MHZ
                    if (spi_port->current_message)
                    {
                        TLS_DBGPRT_WARNING
                            ("spi transaction scheduler is running now!\n");
                        break;
                    }

                    TLS_DBGPRT_SPI_INFO
                        ("acquire the first transaction message in waiting queue.\n");
                    tls_os_sem_acquire(spi_port->lock, 0);

                    spi_port->current_message = spi_next_message();

                    tls_os_sem_release(spi_port->lock);

                // TLS_DBGPRT_SPI("acquire the first transfer pair in the
                // current transaction message.\n");
                    current_transfer = spi_port->current_transfer;
                    if (current_transfer == NULL)
                    {
                        break;
                    }
                    spi_port->current_remaining_bytes = current_transfer->len;

                // TLS_DBGPRT_SPI("current transfer lenght - %d.\n",
                // spi_port->current_remaining_bytes);

                // TLS_DBGPRT_SPI("fill the tx fifo.\n");

                    transfer_bytes =
                        spi_fill_txfifo(current_transfer, spi_port->current_remaining_bytes);

                // printf("start the spi transfer - %d.\n", transfer_bytes);
                    spi_start_transfer(transfer_bytes);

                    break;

                case SPI_SCHED_MSG_TX_FIFO_READY:
                    TLS_DBGPRT_SPI_INFO("process SPI_SCHED_MSG_TX_FIFO_READY.\n");
                    break;

                case SPI_SCHED_MSG_RX_FIFO_READY:
                    TLS_DBGPRT_SPI_INFO("process SPI_SCHED_MSG_RX_FIFO_READY.\n");
                    break;

                case SPI_SCHED_MSG_TRANSFER_COMPLETE:
                    spi_continue_transfer();
                    break;

                case SPI_SCHED_MSG_EXIT:
                    break;

                default:
                    break;
            }
        }
    }
}

void SPI_LS_IRQHandler(void)
{

    u32 int_status;
    u32 int_mask;
    int_status = spi_get_int_status();
// printf("\nspi int sta=%x\n",int_status);
    spi_clear_int_status(int_status);

    int_mask = spi_int_mask();
    int_status &= ~int_mask;

// printf("spi interrupt - 0x%x.\n", int_status);

    if (int_status & SPI_INT_TX_FIFO_RDY)
    {
        tls_os_queue_send(spi_port->msg_queue,
                          (void *) SPI_SCHED_MSG_TX_FIFO_READY, 4);
    }

    if (int_status & SPI_INT_RX_FIFO_RDY)
    {
        tls_os_queue_send(spi_port->msg_queue,
                          (void *) SPI_SCHED_MSG_RX_FIFO_READY, 4);
    }

    if (int_status & SPI_INT_TRANSFER_DONE)
    {
        if (SPI_WORD_TRANSFER == spi_port->transtype)
            spi_continue_transfer();
        else
            tls_os_queue_send(spi_port->msg_queue,
                              (void *) SPI_SCHED_MSG_TRANSFER_COMPLETE, 4);

    }
}


 /**
 * @brief          This function is used to setup the spi CPOL,CPHA,cs signal and clock.
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
int tls_spi_setup(u8 mode, u8 cs_active, u32 fclk)
{
	tls_sys_clk sysclk;

    if ((spi_port->mode == mode) && (spi_port->cs_active == cs_active)
        && (spi_port->speed_hz == fclk))
    {
        TLS_DBGPRT_WARNING
            ("@mode, @cs_activer, @fclk is the same as settings of the current spi master driver!\n");
        return TLS_SPI_STATUS_OK;
    }

    switch (mode)
    {
        case TLS_SPI_MODE_0:
        case TLS_SPI_MODE_1:
        case TLS_SPI_MODE_2:
        case TLS_SPI_MODE_3:
            spi_port->mode = mode;
            break;

        default:
            TLS_DBGPRT_ERR("@mode is invalid!\n");
            return TLS_SPI_STATUS_EMODENOSUPPORT;
    }

    if ((cs_active != TLS_SPI_CS_HIGH) && (cs_active != TLS_SPI_CS_LOW))
    {
        TLS_DBGPRT_ERR("@cs_active  is invalid!\n");
        return TLS_SPI_STATUS_EINVAL;
    }
    else
    {
        spi_port->cs_active = cs_active;
    }
	
	tls_sys_clk_get(&sysclk);

    if ((fclk < TLS_SPI_FCLK_MIN) || (fclk > sysclk.apbclk*UNIT_MHZ/2))		//TLS_SPI_FCLK_MAX
    {
        TLS_DBGPRT_ERR("@fclk is invalid!\n");
        return TLS_SPI_STATUS_ECLKNOSUPPORT;
    }
    else
    {
        spi_port->speed_hz = fclk;
    }

#ifdef SPI_USE_DMA
    if (SPI_DMA_TRANSFER == spi_port->transtype)
    {
        SpiMasterInit(mode, TLS_SPI_CS_LOW, fclk);
        return TLS_SPI_STATUS_OK;
    }
#endif

    spi_port->reconfig = 1;

    return TLS_SPI_STATUS_OK;
}


/**
 * @brief          This function is used to synchronous write command then read data by SPI.
 *
 * @param[in]      txbuf        is the write data buffer.
 * @param[in]      n_tx         is the write data length.
 * @param[in]      rxbuf        is the read data buffer.
 * @param[in]      n_rx         is the read data length.
 *
 * @retval         TLS_SPI_STATUS_OK			if write success.
 * @retval         TLS_SPI_STATUS_EINVAL		if argument is invalid.
 * @retval         TLS_SPI_STATUS_ENOMEM			if there is no enough memory.
 * @retval         TLS_SPI_STATUS_ESHUTDOWN		if SPI driver does not installed.
 *
 * @note           None
 */
int tls_spi_read_with_cmd(const u8 * txbuf, u32 n_tx, u8 * rxbuf, u32 n_rx)
{
    int status;
    struct tls_spi_message message;
    struct tls_spi_transfer x[2];

    if ((txbuf == NULL) || (n_tx == 0) || (rxbuf == NULL) || (n_rx == 0))
    {
        return TLS_SPI_STATUS_EINVAL;
    }

#ifdef SPI_USE_DMA
    if (SPI_DMA_TRANSFER == spi_port->transtype)
    {
        if (n_rx > SPI_DMA_BUF_MAX_SIZE || n_tx > SPI_DMA_CMD_MAX_SIZE)
        {
            TLS_DBGPRT_ERR("\nread length too long\n");
            return TLS_SPI_STATUS_EINVAL;
        }
        tls_os_sem_acquire(spi_port->lock, 0);
        MEMCPY((u8 *) SPI_DMA_CMD_ADDR, txbuf, n_tx);
        SpiDmaBlockRead((u8 *) SPI_DMA_BUF_ADDR, n_rx, (u8 *) SPI_DMA_CMD_ADDR,
                        n_tx);
        MEMCPY(rxbuf, (u8 *) SPI_DMA_BUF_ADDR, n_rx);
        tls_os_sem_release(spi_port->lock);
        return TLS_SPI_STATUS_OK;
    }
#endif

    spi_message_init(&message);

    memset(x, 0, sizeof(x));
    if (n_tx)
    {
        x[0].len = n_tx;
        x[0].tx_buf = txbuf;
        dl_list_add_tail(&message.transfers, &x[0].transfer_list);
    }
    if (n_rx)
    {
        x[1].len = n_rx;
        x[1].rx_buf = rxbuf;
        dl_list_add_tail(&message.transfers, &x[1].transfer_list);
    }

/* do the i/o. */
    status = tls_spi_sync(&message);

    return status;
}

/**
 * @brief          This function is used to synchronous read data by SPI.
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
int tls_spi_read(u8 * buf, u32 len)
{
    struct tls_spi_transfer t;
    struct tls_spi_message m;

    if ((buf == NULL) || (len == 0))
    {
        return TLS_SPI_STATUS_EINVAL;
    }

#ifdef SPI_USE_DMA
    if (SPI_DMA_TRANSFER == spi_port->transtype)
    {
        u32 data32 = 0;
        u16 rxBitLen;
        u32 rdval1 = 0;
        u32 i;
        tls_os_sem_acquire(spi_port->lock, 0);
         // 直接传输，这样做的原因是DMA不能连续读取4个字节以内的数据,SPI FIFO读取单位为word
        if (len <= 4)
        {
            SPIM_CHCFG_REG = SPI_CLEAR_FIFOS;
            while (SPIM_CHCFG_REG & SPI_CLEAR_FIFOS);

            rxBitLen = 8 * len;
            rdval1 =
                SPI_FORCE_SPI_CS_OUT | SPI_CS_LOW | SPI_TX_CHANNEL_ON |
                SPI_RX_CHANNEL_ON | SPI_CONTINUE_MODE | SPI_START |
                SPI_VALID_CLKS_NUM(rxBitLen);
            SPIM_CHCFG_REG = rdval1;
            spiWaitIdle();
            SPIM_CHCFG_REG |= SPI_CS_HIGH;

            data32 = SPIM_RXDATA_REG;

            for (i = 0; i < len; i++)
            {
                *(buf + i) = (u8) (data32 >> i * 8);
            }
            SPIM_CHCFG_REG = 0x00000000;
            SPIM_MODECFG_REG = 0x00000000;
        }
        else                    // DMA传输
        {
            if (len > SPI_DMA_BUF_MAX_SIZE)
            {
                TLS_DBGPRT_ERR("\nread len too long\n");
                tls_os_sem_release(spi_port->lock);
                return TLS_SPI_STATUS_EINVAL;
            }
            SpiDmaBlockRead((u8 *) SPI_DMA_BUF_ADDR, len, NULL, 0);
            MEMCPY(buf, (u8 *) SPI_DMA_BUF_ADDR, len);
        }
        tls_os_sem_release(spi_port->lock);
        return TLS_SPI_STATUS_OK;
    }
#endif

    memset(&t, 0, sizeof(t));
    t.rx_buf = buf;
    t.len = len;

    spi_message_init(&m);

    dl_list_add_tail(&m.transfers, &t.transfer_list);

    return tls_spi_sync(&m);
}

/**
 * @brief          This function is used to synchronous write data by SPI.
 *
 * @param[in]      buf          is the user data.
 * @param[in]      len          is the data length.
 *
 * @retval         TLS_SPI_STATUS_OK			if write success.
 * @retval         TLS_SPI_STATUS_EINVAL		if argument is invalid.
 * @retval         TLS_SPI_STATUS_ENOMEM			if there is no enough memory.
 * @retval         TLS_SPI_STATUS_ESHUTDOWN		if SPI driver does not installed.
 *
 * @note           None
 */
int tls_spi_write(const u8 * buf, u32 len)
{
    struct tls_spi_transfer t;
    struct tls_spi_message m;

    if ((buf == NULL) || (len == 0))
    {
        return TLS_SPI_STATUS_EINVAL;
    }

#ifdef SPI_USE_DMA
    if (SPI_DMA_TRANSFER == spi_port->transtype)
    {
        u32 data32 = 0;
        u16 txBitLen;
        u32 rdval1 = 0;
        u32 i;
        tls_os_sem_acquire(spi_port->lock, 0);
        if (len <= 4)           // 直接传输，这样做的原因是DMA不能连续传输少于4个字节的数据，SPI
        {
            SPIM_CHCFG_REG = SPI_CLEAR_FIFOS;
            while (SPIM_CHCFG_REG & SPI_CLEAR_FIFOS);
            for (i = 0; i < len; i++)
            {
                data32 |= (((u8) (buf[i])) << (i * 8));
            }
            SPIM_TXDATA_REG = data32;
            txBitLen = 8 * len;
            rdval1 =
                SPI_FORCE_SPI_CS_OUT | SPI_CS_LOW | SPI_TX_CHANNEL_ON |
                SPI_RX_CHANNEL_ON | SPI_CONTINUE_MODE | SPI_START |
                SPI_VALID_CLKS_NUM(txBitLen);
            SPIM_CHCFG_REG = rdval1;
            spiWaitIdle();
            SPIM_CHCFG_REG |= SPI_CS_HIGH;

            SPIM_CHCFG_REG = 0x00000000;
            SPIM_MODECFG_REG = 0x00000000;
        }
        else                    // DMA传输
        {
            if (len > SPI_DMA_BUF_MAX_SIZE)
            {
                TLS_DBGPRT_ERR("\nwrite len too long\n");
                tls_os_sem_release(spi_port->lock);
                return TLS_SPI_STATUS_EINVAL;
            }
            MEMCPY((u8 *) SPI_DMA_BUF_ADDR, buf, len);
            SpiDmaBlockWrite((u8 *) SPI_DMA_BUF_ADDR, len, 0, 0);
        }
        tls_os_sem_release(spi_port->lock);
        return TLS_SPI_STATUS_OK;
    }

#endif
    memset(&t, 0, sizeof(t));
    t.tx_buf = buf;
    t.len = len;

    spi_message_init(&m);

    dl_list_add_tail(&m.transfers, &t.transfer_list);

    return tls_spi_sync(&m);
}

/**
 * @brief          This function is used to synchronous write 32bit command then write data by SPI.
 *
 * @param[in]      cmd                     is the command data.
 * @param[in]      n_cmd          	    is the command len,can not bigger than four
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
int tls_spi_write_with_cmd(const u8 * cmd, u32 n_cmd, const u8 * txbuf,
                           u32 n_tx)
{
    int status;
    struct tls_spi_message message;
    struct tls_spi_transfer x[2];

    if ((cmd == NULL) || (n_cmd == 0) || (txbuf == NULL) || (n_tx == 0))
    {
        return TLS_SPI_STATUS_EINVAL;
    }

#ifdef SPI_USE_DMA
    if (SPI_DMA_TRANSFER == spi_port->transtype)
    {
        if (n_tx > SPI_DMA_BUF_MAX_SIZE)
        {
            TLS_DBGPRT_ERR("\nwriten len too long\n");
            return TLS_SPI_STATUS_EINVAL;
        }
        tls_os_sem_acquire(spi_port->lock, 0);
        MEMCPY((u8 *) SPI_DMA_BUF_ADDR, (u8 *) cmd, n_cmd);
        MEMCPY((u8 *) (SPI_DMA_BUF_ADDR + n_cmd), txbuf, n_tx);
        SpiDmaBlockWrite((u8 *) SPI_DMA_BUF_ADDR, (n_cmd + n_tx), 0, 0);
        tls_os_sem_release(spi_port->lock);
        return TLS_SPI_STATUS_OK;
    }
#endif

    spi_message_init(&message);

    memset(x, 0, sizeof(x));
    if (n_cmd)
    {
        x[0].len = n_cmd;
        x[0].tx_buf = (const void *) cmd;
        dl_list_add_tail(&message.transfers, &x[0].transfer_list);
    }
    if (n_tx)
    {
        x[1].len = n_tx;
        x[1].tx_buf = txbuf;
        dl_list_add_tail(&message.transfers, &x[1].transfer_list);
    }

/* do the i/o. */
    status = tls_spi_sync(&message);

    return status;
}

/**
 * @brief
 *
 * @param message
 *
 * @return
 */
int tls_spi_sync(struct tls_spi_message *message)
{
    int status;
    u8 err;
    tls_os_sem_t *sem;

    err = tls_os_sem_create(&sem, 0);
    if (err != TLS_OS_SUCCESS)
    {
        TLS_DBGPRT_ERR
            ("create spi transaction synchronizing semaphore fail!\n");
        return TLS_SPI_STATUS_ENOMEM;
    }

    message->context = (void *) sem;
    message->complete = spi_complete;

    status = tls_spi_async(message);
    if (status == TLS_SPI_STATUS_OK)
    {
        TLS_DBGPRT_SPI_INFO("waiting spi transaction finishing!\n");
        tls_os_sem_acquire(sem, 0);
    }

    tls_os_sem_delete(sem);
    message->context = NULL;
    message->complete = NULL;

    return status;
}

/**
 * @brief
 *
 * @param message
 *
 * @return
 */
int tls_spi_async(struct tls_spi_message *message)
{
    u8 need_sched;
    struct tls_spi_transfer *transfer;

    if (spi_port == NULL)
    {
        TLS_DBGPRT_ERR("spi master driver module not beed installed!\n");
        return TLS_SPI_STATUS_ESHUTDOWN;
    }

    if ((message == NULL) || (dl_list_empty(&message->transfers)))
    {
        TLS_DBGPRT_ERR("@message is NULL or @message->transfers is empty!\n");
        return TLS_SPI_STATUS_EINVAL;
    }

    dl_list_for_each(transfer, &message->transfers, struct tls_spi_transfer,
                     transfer_list)
    {
        if (transfer->len == 0)
        {
            TLS_DBGPRT_ERR("\"@transfer->len\" belong to @message is 0!\n");
            return TLS_SPI_STATUS_EINVAL;
        }
    }

    tls_os_sem_acquire(spi_port->lock, 0);

    if (dl_list_empty(&spi_port->wait_queue))
    {
        need_sched = 1;
    }
    else
    {
        need_sched = 0;
    }
    message->status = SPI_MESSAGE_STATUS_IDLE;
    dl_list_add_tail(&spi_port->wait_queue, &message->queue);

    tls_os_sem_release(spi_port->lock);

    if (need_sched == 1)
    {
        tls_os_queue_send(spi_port->msg_queue,
                          (void *) SPI_SCHED_MSG_START_ENGINE, 4);
    }

    return TLS_SPI_STATUS_OK;
}


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
int tls_spi_init(void)
{
    u8 err;
    struct tls_spi_port *port;


    if (spi_port != NULL)
    {
        TLS_DBGPRT_ERR("spi driver module has been installed!\n");
        return TLS_SPI_STATUS_EBUSY;
    }

    TLS_DBGPRT_SPI_INFO("initialize spi master driver module.\n");

    port = (struct tls_spi_port *) tls_mem_alloc(sizeof(struct tls_spi_port));
    if (port == NULL)
    {
        TLS_DBGPRT_ERR("allocate \"struct tls_spi_port\" fail!\n");
        return TLS_SPI_STATUS_ENOMEM;
    }

    err = tls_os_sem_create(&port->lock, 1);
    if (err != TLS_OS_SUCCESS)
    {
        TLS_DBGPRT_ERR("create semaphore @spi_port->lock fail!\n");
        tls_mem_free(port);
        return TLS_SPI_STATUS_ENOMEM;
    }

    err = tls_os_queue_create(&port->msg_queue, MSG_QUEUE_SIZE);
    if (err != TLS_OS_SUCCESS)
    {
        TLS_DBGPRT_ERR("create message queue @spi_port->msg_queue fail!\n");
        tls_os_sem_delete(port->lock);
        tls_mem_free(port);
        return TLS_SPI_STATUS_ENOMEM;
    }

    err = tls_os_task_create(NULL, "hostspi",
                             spi_scheduler,
                             (void *) spi_port,
                             (void *) &spi_scheduler_stk,
                             SPI_SCHEDULER_STK_SIZE * sizeof(u32),
                             TLS_SPI_SCHEDULER_TASK_PRIO, 0);
    if (err != TLS_OS_SUCCESS)
    {
        TLS_DBGPRT_ERR("create spi master driver scheduler task fail!\n");
        tls_os_sem_delete(port->lock);
        tls_os_queue_delete(port->msg_queue);
        tls_mem_free(port);
        return TLS_SPI_STATUS_ENOMEM;
    }

    port->speed_hz = SPI_DEFAULT_SPEED; /* 默认2M */
    port->cs_active = SPI_CS_ACTIVE_MODE;
    port->mode = SPI_DEFAULT_MODE;  /* CPHA = 0,CPOL = 0 */
    port->reconfig = 0;

    dl_list_init(&port->wait_queue);

    port->current_message = NULL;
    port->current_remaining_transfer = 0;
    port->current_transfer = NULL;
    port->current_remaining_bytes = 0;

    spi_port = port;

    TLS_DBGPRT_SPI_INFO("initialize spi master controller.\n");

    spi_clear_fifo();
    spi_set_endian(1);
    tls_spi_trans_type(SPI_BYTE_TRANSFER);
    spi_set_mode(spi_port->mode);
    spi_set_chipselect_mode(SPI_CS_INACTIVE_MODE);  /* cs=1 ,片选无效 */
    spi_force_cs_out(1);        /* 片选由软件控制 */
    spi_set_sclk(spi_port->speed_hz);

    spi_set_tx_trigger_level(0);
    spi_set_rx_trigger_level(7);

    spi_set_rx_channel(1);
    spi_set_tx_channel(1);
    spi_unmask_int(SPI_INT_TRANSFER_DONE  /* | SPI_INT_RX_FIFO_RDY |SPI_INT_TX_FIFO_RDY */ );


    TLS_DBGPRT_SPI_INFO("register spi master interrupt handler.\n");

    tls_irq_enable(SPI0_INT);

    TLS_DBGPRT_SPI_INFO("spi master driver module initialization finish.\n");

    return TLS_SPI_STATUS_OK;
}

/**
 * @brief
 *
 * @return
 */
int tls_spi_exit(void)
{
    TLS_DBGPRT_SPI_INFO("Not support spi master driver module uninstalled!\n");
    return TLS_SPI_STATUS_EPERM;
}



/**********************************************************************************************************
* Description: 	This function is used to select SPI slave type.
*
* Arguments  : 	slave					is the slave type,defined as follow:
* 							slave == SPI_SLAVE_FLASH	:flash
* 							slave == SPI_SLAVE_CARD		: sd card
*
* Returns    : 	Before communicate with different SPI device, must call the function.
**********************************************************************************************************/
void tls_spi_slave_sel(u16 slave)
{
// u16 ret;
/*gpio0控制cs信号*/
    tls_gpio_cfg((enum tls_io_name) SPI_SLAVE_CONTROL_PIN, WM_GPIO_DIR_OUTPUT,
                 WM_GPIO_ATTR_FLOATING);
    if (SPI_SLAVE_FLASH == slave)
    {
        tls_gpio_write((enum tls_io_name) SPI_SLAVE_CONTROL_PIN, 0);
    // ret = tls_gpio_read(SPI_SLAVE_CONTROL_PIN);
    // printf("\nflash gpio 0 ===%d\n",ret);
    }
    else if (SPI_SLAVE_CARD == slave)
    {
        tls_gpio_write((enum tls_io_name) SPI_SLAVE_CONTROL_PIN, 1);
    // ret = tls_gpio_read(SPI_SLAVE_CONTROL_PIN);
    // printf("\ncard gpio 0 ===%d\n",ret);
    }
}
