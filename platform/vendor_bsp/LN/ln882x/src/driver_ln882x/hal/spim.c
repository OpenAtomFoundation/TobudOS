#include <string.h>
#include "types.h"
#include "proj_config.h"
#include "hal/spi_type.h"
#include "ll/ll_syscon.h"
#include "ll/ll_spim.h"
#include "hal/spim.h"

#define SLAVE_INDEX     1

enum {
    SPI_READ = 0,
    SPI_WRITE,
};

typedef union {
    uint32_t val;
    struct {
        uint32_t    data: 16;
        uint32_t    addr: 8;
        uint32_t    cmd: 1;
        uint32_t    reserved_0:7;
    } bits_field;
} SPI_XFER_PKT_T;

void spim_init(uint32_t baud)
{
    LL_SYSCON_SPIMEnable(SPIM_IO_EN1);

    LL_SPIM_Enable(SSI_Disabled);
    LL_SPIM_CtrlR0(DFS_32_25_bits, Control_Word_3_bit, 0, 0, Transmit_and_Receive, Inactive_Low, SCLK_Toggle_In_Middle, Motorola_SPI);
    LL_SPIM_CtrlR1(1);

    LL_SPIM_CLK_Div(APBUS0_CLOCK/baud);
    LL_SPIM_TX_FIFO_Threshold_Set(1);
    LL_SPIM_RX_FIFO_Threshold_Set(1);

    LL_SPIM_Slave_Select(SLAVE_INDEX);
    LL_SPIM_Enable(SSI_Enabled);
}

int spim_write(uint8_t addr, uint16_t data)
{
    int ret = OK;
    SPI_XFER_PKT_T pkt;

    memset(&pkt, 0, sizeof(pkt));
    pkt.bits_field.addr = addr;
    pkt.bits_field.data = data;
    pkt.bits_field.cmd = SPI_WRITE;

    while( LL_SPIM_Is_TxFIFONotFull() == Transmit_FIFO_Full );
    LL_SPIM_DataReg_Set(pkt.val);

    while( LL_SPIM_Is_RxFIFONotEmpty() == Receive_FIFO_Empty );
    pkt.val = LL_SPIM_DataReg_Get();

    return ret;
}

int spim_read(uint8_t addr, uint16_t *data)
{
    int ret = FAIL;
    SPI_XFER_PKT_T pkt;

    memset(&pkt, 0, sizeof(pkt));
    pkt.bits_field.addr = addr;
    pkt.bits_field.cmd = SPI_READ;

    while( LL_SPIM_Is_TxFIFONotFull() == Transmit_FIFO_Full );
    LL_SPIM_DataReg_Set(pkt.val);

    while( LL_SPIM_Is_RxFIFONotEmpty() == Receive_FIFO_Empty );
    pkt.val = LL_SPIM_DataReg_Get();

    *data = pkt.bits_field.data;
    ret = OK;

    return ret;
}

