#include <string.h>
#include "ln88xx.h"
#include "proj_config.h"
#include "hal/spi_type.h"
#include "hal/syscon_types.h"
#include "ll/ll_syscon.h"
#include "ll/ll_spim.h"
#include "hal/hal_spim.h"
#include "hal/exflash.h"
#include "hal/hal_common.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////  External FLASH  //////////////////////////////
////////////  Use SPIM to driver external FLASH, like XTX FLASH  //////////////
///////////////////////////////////////////////////////////////////////////////

#define RETRY_LIMIT         0xFFFF

/**
 * @brief Configurate baudrate of the External FLASH interface.
 *
 * @param baud like 1000000
 */
void ExFlash_Init(uint32_t baud)
{
    LL_SYSCON_SPIMEnable(SPIM_IO_EN0);
    LL_SPIM_Enable(SSI_Disabled);
    LL_SPIM_Slave_Select(0);
    LL_SPIM_CtrlR0(DFS_32_8_bits, Control_Word_1_bit, 0, 0, Transmit_and_Receive, Inactive_Low, SCLK_Toggle_In_Middle, Motorola_SPI);
    // FIXME: 控制SPI发送、接收（N个字节+1）以后把CS拉高终止协议，这里设置的并没有意义，因为后面每一次调用都会重新设置
    LL_SPIM_CtrlR1(1);

    LL_SPIM_CLK_Div(APBUS0_CLOCK / baud);
    LL_SPIM_TX_FIFO_Threshold_Set(1);
    LL_SPIM_RX_FIFO_Threshold_Set(1);
    LL_SPIM_Slave_Select(1);
    LL_SPIM_Enable(SSI_Enabled);
}

/**
 * @brief Deinit the interface of FLASH.
 */
void ExFlash_Deinit(void)
{
    LL_SPIM_Enable(SSI_Disabled);
    LL_SYSCON_SPIMDisable(SPIM_IO_EN0);
}

/**
 * @brief Send some data and then receive some data via SPIM interface.
 *
 * @param rd_ptr start point of receive buffer
 * @param rd_len how many bytes to be received.
 * @param wr_ptr send buffer
 * @param wr_len how many bytes to be sent.
 * @return int8_t return 0 on succes; return other value on failure.
 */
static int8_t _spim_read(uint8_t *rd_ptr, uint32_t rd_len, uint8_t *wr_ptr, uint8_t wr_len)
{
    uint32_t i = 0;
    uint32_t retry = 0;
    int8_t retval = 0;

    LL_SPIM_Enable(SSI_Disabled);
    LL_SPIM_Slave_Select(0);
    LL_SPIM_CtrlR0(DFS_32_8_bits, Control_Word_1_bit, 0, 0, EEPROM_Read, Inactive_Low, SCLK_Toggle_In_Middle, Motorola_SPI);
    // 为什么要减1呢，参见寄存器手册
    LL_SPIM_CtrlR1( rd_len - 1 );
    LL_SPIM_Enable(SSI_Enabled);

    // 首先发送
    if (wr_len <= TX_FIFO_DEPTH) { // TX_FIFO_DEPTH
        for (i = 0; i < wr_len; i++) {
            LL_SPIM_DataReg_Set(*(wr_ptr+i));
        }
        LL_SPIM_Slave_Select(1);
    } else {
        for (i = 0; i < TX_FIFO_DEPTH; i++) {
            LL_SPIM_DataReg_Set( *(wr_ptr+i) );
        }
        LL_SPIM_Slave_Select(1);

        for (i = TX_FIFO_DEPTH; i < wr_len; i++) {
            retry = 0;
            while((LL_SPIM_Is_TxFIFONotFull() == Transmit_FIFO_Full) && (retry++ < RETRY_LIMIT));
            if (retry >= RETRY_LIMIT) {
                // 超时，没有收到数据
                retval = -1;
                break;
            }
            LL_SPIM_DataReg_Set( *(wr_ptr+i) );
        }
    }

    // 等待发送完成
    while(LL_SPIM_Is_TxFIFOEmpty() != Transmit_FIFO_Empty);

    // 开始接收
    for (i = 0; i < rd_len; i++) {
        retry = 0;
        while( (LL_SPIM_Is_RxFIFONotEmpty() == Receive_FIFO_Empty) && (retry++ < RETRY_LIMIT) );
        if ( retry >= RETRY_LIMIT ) {
            retval = -1;
            break;
        }
        *(rd_ptr + i) = LL_SPIM_DataReg_Get();
    }
    while(LL_SPIM_Is_Busy() == SSI_Busy);

    // 结束
    LL_SPIM_Enable(SSI_Disabled);
    return retval;
}

/**
 * @brief Send some data via SPIM interface.
 *
 * @param wr_ptr send buffer.
 * @param wr_len how many bytes to be sent.
 * @return int8_t return 0 on success; return other value on failure.
 */
static int8_t _spim_send_only(uint8_t *wr_ptr, uint16_t wr_len)
{
    uint16_t i = 0;
    uint32_t retry = 0;
    int8_t retval = 0;

    LL_SPIM_Enable(SSI_Disabled);
    LL_SPIM_Slave_Select(0);
    LL_SPIM_CtrlR0(DFS_32_8_bits, Control_Word_1_bit, 0, 0, Transmit_Only, Inactive_Low, SCLK_Toggle_In_Middle, Motorola_SPI);
    LL_SPIM_CtrlR1(0);
    LL_SPIM_Enable(SSI_Enabled);

    if (wr_len <= TX_FIFO_DEPTH) {
        for (i = 0; i < wr_len; i++) {
            LL_SPIM_DataReg_Set( *(wr_ptr + i) );
        }
        LL_SPIM_Slave_Select(1);
    } else {
        // TX FIFO一次填充不完整，分批次填充
        for (i = 0; i < TX_FIFO_DEPTH; i++) {
            LL_SPIM_DataReg_Set( *(wr_ptr + i) );
        }
        LL_SPIM_Slave_Select(1);

        // 发送剩余的
        for (i = TX_FIFO_DEPTH; i < wr_len; i++) {
            retry = 0;
            while ( (LL_SPIM_Is_TxFIFONotFull() == Transmit_FIFO_Full) && (retry++ < RETRY_LIMIT) );
            if (retry >= RETRY_LIMIT) {
                retval = -1;
                break;
            }
            LL_SPIM_DataReg_Set( *(wr_ptr + i) );
        }
    }

    while( LL_SPIM_Is_TxFIFOEmpty() != Transmit_FIFO_Empty );   // 等待发送完成
    while( LL_SPIM_Current_TxFIFO_Level() || LL_SPIM_Is_Busy() );

    // 结束
    LL_SPIM_Enable(SSI_Disabled);
    return retval;
}

/**
 * @brief Read Manufacture ID/Device ID (REMS) (90H)
 *
 * @return uint16_t MSB is Manufacturer ID, LSB is Device ID.
 */
uint16_t ExFlash_Read_ManufactureID(void)
{
    uint8_t cmd_seq[] = {0x90, 0x00, 0x00, 0x00};
    uint8_t recv_buf[2] = {0};
    uint16_t id = 0x00;

    cmd_seq[0] = EXFLASH_READ_MANUFACTURER_ID;
    _spim_read(recv_buf, 2, cmd_seq, 4);

    id = (recv_buf[0] << 8) | (recv_buf[1]);
    return id;
}

/**
 * @brief Read FLASH Status Register all 16 bits.
 *
 * @return uint16_t FLASH status.
 */
uint16_t ExFlash_Read_Status(void)
{
    uint8_t cmd_seq = 0;
    uint8_t recv_buf[2] = {0};
    uint16_t status = 0;

    cmd_seq = EXFLASH_READ_STATUS_REG_LOW;
    _spim_read(recv_buf, 1, &cmd_seq, 1);
    status = recv_buf[0] & 0xFF;    // 低8位

    cmd_seq = EXFLASH_READ_STATUS_REG_HIGH;
    _spim_read(recv_buf, 1, &cmd_seq, 1);
    status |= (recv_buf[0] & 0xFF) << 8; // 高8位

    return status;
}

/**
 * @brief Read Identification (RDID, 0x9F)
 *
 * @return uint32_t identification
 */
uint32_t ExFlash_Read_Identification(void)
{
    uint8_t cmd_seq = 0;
    uint8_t recv_buf[3] = {0};
    uint32_t id = 0;

    cmd_seq = EXFLASH_READ_IDENTIFICATION;
    _spim_read(recv_buf, 3, &cmd_seq, 1);

    id |= recv_buf[0] << 16; // ManufacturerID
    id |= recv_buf[1] << 8; // Memory Type
    id |= recv_buf[2] & 0xFF;   // Memory Capacity

    return id;
}

/**
 * @brief The WREN command is for setting the Write Enable Latch bit, which
 * must be set prior to every Page Program, Sector Erase, Block Erase, Chip
 * Erase and Write Status Register command.
 *
 */
void ExFlash_Write_Enable(void)
{
    uint8_t cmd_seq = 0;

    cmd_seq = EXFLASH_WRITE_ENABLE;
    _spim_send_only(&cmd_seq, 1);
}

/**
 * @brief The WRDI command is for setting the Write Enable Latch (WEL) bit.
 * The WEL bit is reset by following condition: Power-up and upon completion
 * of the Write Status Register, Page Program, Sector Erase, Block Erase and
 * Chip Erase commands.
 *
 */
void ExFlash_Write_Disable(void)
{
    uint8_t cmd_seq = 0;

    cmd_seq = EXFLASH_WRITE_DISABLE;
    _spim_send_only(&cmd_seq, 1);
}

/**
 * @brief Read data from FLASH.
 *
 * @param addr start address
 * @param length how many bytes to read
 * @param recv_buf receive buffer
 * @return int8_t return 0 on success; return others on failure.
 */
int8_t ExFlash_Read_Data_Bytes(uint32_t addr, uint8_t *recv_buf, uint32_t length)
{
    uint8_t cmd_seq[4] = {0x00};
    int8_t retval = 0;

    cmd_seq[0] = EXFLASH_READ_DATA_BYTES;
    cmd_seq[1] = (addr >> 16 ) & 0xFF; // A23~A16
    cmd_seq[2] = (addr >> 8) & 0xFF; // A15~A8
    cmd_seq[3] = addr & 0xFF; // A7~A0
    retval = _spim_read(recv_buf, length, cmd_seq, 4);
    return retval;
}

/**
 * @brief Read Data Bytes at Higher Speed (Fast Read).
 *
 * @param addr start address to read
 * @param length how many bytes to read
 * @param recv_buf receive buffer
 * @return int8_t return 0 on success; return others on failure.
 */
int8_t ExFlash_Fast_Read(uint32_t addr, uint8_t *recv_buf, uint32_t length)
{
    uint8_t cmd_seq[5] = {0};
    int8_t retval = 0;

    addr = addr % EXFALSH_SIZE_MAX;
    if ( (addr + length) > EXFALSH_SIZE_MAX ) {
        length = EXFALSH_SIZE_MAX - addr;
    }
    cmd_seq[0] = EXFLASH_FAST_READ;
    cmd_seq[1] = (addr >> 16) & 0xFF;   // A23~A16
    cmd_seq[2] = (addr >> 8) & 0xFF;    // A15~A8
    cmd_seq[3] = addr & 0xFF;   // A7~A0

    retval = _spim_read(recv_buf, length, cmd_seq, sizeof(cmd_seq));
    return retval;
}

/**
 * @brief Wait while WIP bit is set.
 */
static void ExFlash_Operation_Wait(void)
{
    uint16_t status = 0;
    do {
        status = ExFlash_Read_Status();
    } while( status & 0x01 );
}

/**
 * @brief Erase all data of the chip.
 */
void ExFlash_Chip_Erase(void)
{
    uint8_t cmd_seq = 0x00;

    cmd_seq = EXFLASH_CHIP_ERASE;
    ExFlash_Write_Enable();
    _spim_send_only(&cmd_seq, 1);
    ExFlash_Operation_Wait();
}

/**
 * @brief Program one page.
 *
 * @param addr  start of address of one page.
 * @param data  data
 * @param length length of data.
 */
void ExFlash_Page_Program(uint32_t addr, uint8_t *data, uint16_t length)
{
    uint8_t page_buf[EXFLASH_PAGE_SIZE + 4] = {0};

    page_buf[0] = EXFLASH_PAGE_PROGRAM;
    page_buf[1] = (addr >> 16) & 0xFF; // A23~A16
    page_buf[2] = (addr >> 8) & 0xFF; // A15~A8
    page_buf[3] = addr & 0XFF; // A7~A0

    memcpy(page_buf+4, data, length);
    ExFlash_Write_Enable();
    _spim_send_only(page_buf, length + 4);
    ExFlash_Operation_Wait();
}

/**
 * @brief Program at any address inside the FLASH address space.
 *
 * @param offset start address
 * @param length how many bytes to program
 * @param buffer start address of the buffer
 */
void ExFlash_Program(uint32_t offset, uint8_t *buffer, uint32_t length)
{
    uint32_t page_base = 0;
    uint16_t cycles = 0;
    uint16_t remainder = 0;
    uint16_t i = 0;

    assert_param(offset < EXFALSH_SIZE_MAX);

    page_base = offset & (~ (EXFLASH_PAGE_SIZE - 1));
    if (page_base != offset) {
        uint32_t tailing_length = (page_base + EXFLASH_PAGE_SIZE) - offset;
        uint32_t prog_length = (length < tailing_length) ? length : tailing_length;
        ExFlash_Page_Program(offset, buffer, prog_length);
        buffer += prog_length;
        offset += prog_length;
        length -= prog_length;
    }
    cycles = length / EXFLASH_PAGE_SIZE;
    for (i = 0; i < cycles; i++) {
        ExFlash_Page_Program(offset, buffer, EXFLASH_PAGE_SIZE);
        offset += EXFLASH_PAGE_SIZE;
        buffer += EXFLASH_PAGE_SIZE;
    }
    remainder = length % EXFLASH_PAGE_SIZE;
    if (remainder) {
        ExFlash_Page_Program(offset, buffer, remainder);
    }
}

/**
 * @brief Erase an sector at which the addr is located.
 *
 * @param addr Any addrss inside the sector is a valid address for
 *              the Sector Erase command.
 */
void ExFlash_Erase_Sector(uint32_t addr)
{
    uint8_t cmd_seq[4] = {0};

    addr = addr % EXFALSH_SIZE_MAX;
    cmd_seq[0] = EXFLASH_SECTOR_ERASE;
    cmd_seq[1] = (addr >> 16) & 0xFF;   // A23~A16
    cmd_seq[2] = (addr >> 8) & 0xFF;    // A15~A8
    cmd_seq[3] = addr & 0xFF;           // A7~A0

    ExFlash_Write_Enable();
    _spim_send_only(cmd_seq, sizeof(cmd_seq));
    ExFlash_Operation_Wait();
}

/**
 * @brief Block erase by 32KB or 64KB according to type.
 *
 * @param addr Any address inside the block is a valid address for the block erase command.
 * @param type to indicate a 32KB or 64KB block erase.
 */
void ExFlash_Erase_Block(uint32_t addr, uint8_t type)
{
    uint8_t cmd_seq[4] = {0};

    addr = addr % EXFALSH_SIZE_MAX;
    cmd_seq[0] = (type == EXFLASH_BLOCK_ERASE_32KB) ? EXFLASH_BLOCK_ERASE_32KB : EXFLASH_BLOCK_ERASE_64KB;
    cmd_seq[1] = (addr >> 16) & 0xFF;   // A23~A16
    cmd_seq[2] = (addr >> 8) & 0xFF;    // A15~A8
    cmd_seq[3] = addr & 0xFF;           // A7~A0

    ExFlash_Write_Enable();
    _spim_send_only(cmd_seq, sizeof(cmd_seq));
    ExFlash_Operation_Wait();
}

/**
 * @brief Erase flash start from offset.
 *
 * @param offset start address inside the external FLASH.
 * @param length how many bytes to erase.
 */
void ExFlash_Erase(uint32_t offset, uint32_t length)
{
    uint32_t num_64k = 0, num_32k = 0, num_4k = 0, tmpoffset = offset, i;

    num_64k = length / EXFALSH_SIZE_64K;
    length = length % EXFALSH_SIZE_64K;
    num_32k = length / EXFALSH_SIZE_32K;
    length = length % EXFALSH_SIZE_32K;
    num_4k = length / EXFALSH_SIZE_4K + ( (length % EXFALSH_SIZE_4K) ? 1 : 0);

    for (i = 0; i < num_64k; i++) {
        ExFlash_Erase_Block(tmpoffset, EXFLASH_BLOCK_ERASE_64KB);
        tmpoffset += EXFALSH_SIZE_64K;
    }
    for (i = 0; i < num_32k; i++) {
        ExFlash_Erase_Block(tmpoffset, EXFLASH_BLOCK_ERASE_32KB);
        tmpoffset += EXFALSH_SIZE_32K;
    }
    for (i = 0; i < num_4k; i++) {
        ExFlash_Erase_Sector(tmpoffset);
        tmpoffset += EXFALSH_SIZE_4K;
    }
}
