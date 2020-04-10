#include <string.h>
#include "proj_config.h"
#include "ln88xx.h"
#include "hal/qspi.h"
#include "hal/flash.h"
#include "hal/hal_common.h"
#include "ll/ll_common.h"
#include "hal/cache.h"


#pragma push
#pragma O2
/**
 * @brief Init FLASH.
 * @note Warning!!! If running on flash, do not init flash!
 */
void FLASH_Init(void)
{
    qspi_init(AHBUS_CLOCK/QSPI_CLK, 1);
}

/**
 * @brief Deinit FLASH.
 * @note Warning!!! If running on flash, do not deinit flash!
 */
void FLASH_Deinit(void)
{
    qspi_deinit();
}

/**
 * @brief Read a flash section.
 *
 * This function is used to read a part of the flash memory.
 *
 * @param offset Starting offset from the beginning of the flash device.
 * @param length Size of the portion of flash to read.
 * @param buffer Pointer on data to read.
 * @return uint8_t 0 if operation can start successfully.
 */
uint8_t FLASH_Read(uint32_t offset, uint32_t length, uint8_t *buffer)
{
    uint8_t shift = ((uint32_t)buffer & 0x03) ? (4 - (uint32_t)buffer & 0x03) : 0;
    uint32_t length_in_words = (length - shift) / sizeof(uint32_t);
    uint8_t remainder = (length - shift) % sizeof(uint32_t);
    uint8_t read_cmd_buf[4];

    assert_param( (offset+length) <= FALSH_SIZE_MAX );
#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    if(shift > 0)
    {
        read_cmd_buf[0]= FLASH_STANDARD_READ;
        read_cmd_buf[1]= (offset>>16)&0xff;
        read_cmd_buf[2]= (offset>>8)&0xff;
        read_cmd_buf[3]= (offset)&0xff;
        qspi_standard_read_byte(buffer, shift, read_cmd_buf, sizeof(read_cmd_buf));
        offset += shift;
        buffer += shift;
    }

    if(length_in_words > 0){
        qspi_standard_read_word((uint32_t *)buffer, length_in_words, FLASH_STANDARD_READ, offset);
        buffer += sizeof(uint32_t)*length_in_words;
        offset += sizeof(uint32_t)*length_in_words;
    }

    if(remainder)
    {
        read_cmd_buf[0]= FLASH_STANDARD_READ;
        read_cmd_buf[1]= (offset>>16)&0xff;
        read_cmd_buf[2]= (offset>>8)&0xff;
        read_cmd_buf[3]= (offset)&0xff;
        qspi_standard_read_byte(buffer, remainder, read_cmd_buf, sizeof(read_cmd_buf));
    }

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif
    return 0;
}

uint8_t FLASH_ReadByCache(uint32_t offset, uint32_t length, uint8_t *buffer)
{
    uint32_t addr = 0;
    uint32_t i = 0;
    assert_param( (offset+length) <= FALSH_SIZE_MAX );

    addr = REG_CACHE_FLASH_BASE + offset;
    for(i = 0; i < length; i++){
        *buffer++ = *(uint8_t *)(addr++);
    }
    return 0;
}


/**
 * @brief Flash Chip Erase
 */
void FLASH_ChipErase(void)
{
    uint8_t chip_erase_cmd = FLASH_CHIP_ERASE;

#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    FLASH_WriteEnable();
    qspi_standard_write(&chip_erase_cmd, sizeof(chip_erase_cmd));
    FLASH_OperationWait();

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif
}

static uint8_t FLASH_EraseBlock(uint32_t offset, Flash_EraseType_t type)
{
    uint8_t erase_cmd_addr[4];

    erase_cmd_addr[0]= FLASH_SECTOR_ERASE;//default:SECTOR ERASE
    erase_cmd_addr[1]= (offset >> 16)&0xff;
    erase_cmd_addr[2]= (offset >> 8 )&0xff;
    erase_cmd_addr[3]= (offset      )&0xff;
    switch(type)
    {
        case ERASE_SECTOR_4KB:
            erase_cmd_addr[0] = FLASH_SECTOR_ERASE;
            break;
        case ERASE_BLOCK_32KB:
            erase_cmd_addr[0] = FLASH_32KB_BLOCK_ERASE;
            break;
        case ERASE_BLOCK_64KB:
            erase_cmd_addr[0] = FLASH_64KB_BLOCK_ERASE;
            break;
        default:
            break;
    }

#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    FLASH_WriteEnable();
    qspi_standard_write(erase_cmd_addr, sizeof(erase_cmd_addr));
    FLASH_OperationWait();

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif

    return 0;
}

/**
 * @brief Erase a portion of flash, starts from offset.
 *
 * @param offset(Note:4K Aligned)
 * @param length(Note:4K Aligned)
 */
void FLASH_Erase(uint32_t offset, uint32_t length)
{
    assert_param((offset%FALSH_SIZE_4K == 0));
    assert_param((length%FALSH_SIZE_4K == 0));

    uint16_t num_32k = 0, num_4k = 0, i;

    uint32_t tmpoffset      = 0;
    uint32_t tailing_length = 0;
    uint8_t  tailing_4k_num = 0;
    uint32_t remainder_len  = 0;

    tmpoffset = offset;
    if(length >= FALSH_SIZE_BLOCK_32K){
        tailing_length = ((offset&(~(FALSH_SIZE_BLOCK_32K-1))) + FALSH_SIZE_BLOCK_32K) - offset;
        tailing_4k_num = tailing_length / FALSH_SIZE_4K;

        for(i = 0; i < tailing_4k_num; i++){
            FLASH_EraseBlock(tmpoffset, ERASE_SECTOR_4KB);
            tmpoffset += FALSH_SIZE_4K;
        }

        remainder_len = length - tailing_length;
        num_32k       = remainder_len / FALSH_SIZE_BLOCK_32K;
        remainder_len = remainder_len % FALSH_SIZE_BLOCK_32K;
        num_4k        = remainder_len / FALSH_SIZE_4K + ((remainder_len % FALSH_SIZE_4K) ? 1:0);

        for(i = 0; i < num_32k; i++){
            FLASH_EraseBlock(tmpoffset, ERASE_BLOCK_32KB);
            tmpoffset += FALSH_SIZE_BLOCK_32K;
        }

        for(i = 0; i < num_4k; i++){
            FLASH_EraseBlock(tmpoffset, ERASE_SECTOR_4KB);
            tmpoffset += FALSH_SIZE_4K;
        }
    }else{
        num_4k        = length / FALSH_SIZE_4K + ((length % FALSH_SIZE_4K) ? 1:0);
        for(i = 0; i < num_4k; i++){
            FLASH_EraseBlock(tmpoffset, ERASE_SECTOR_4KB);
            tmpoffset += FALSH_SIZE_4K;
        }
    }
}


static void FLASH_PageProgramGeneral(Flash_AreaType type, uint32_t offset, uint32_t length, uint8_t *buffer)
{
    assert_param(length <= FLASH_PAGE_SIZE);

    uint8_t page_program_buf[4 + FLASH_PAGE_SIZE];

    if(SECURITY_AREA == type){
        page_program_buf[0]= FLASH_SECURITY_PROGRAM;
    } else {
        page_program_buf[0]= FLASH_PAGE_PROGRAM;
    }
    page_program_buf[1]= (offset >> 16)&0xff;
    page_program_buf[2]= (offset >> 8 )&0xff;
    page_program_buf[3]= (offset      )&0xff;

    memcpy(&page_program_buf[4],buffer, length);

#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    FLASH_WriteEnable();
    qspi_standard_write(page_program_buf, length+4);
    FLASH_OperationWait();

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif
}

static uint8_t FLASH_ProgramGeneral(Flash_AreaType type, uint32_t offset, uint32_t length, uint8_t *buffer)
{
    assert_param(offset < FALSH_SIZE_MAX);

    uint32_t page_base = offset&(~(FLASH_PAGE_SIZE-1));
    uint16_t cycles = 0;
    uint16_t remainder = 0;
    uint16_t i;

    if(page_base != offset){
        uint32_t tailing_length = (page_base + FLASH_PAGE_SIZE) - offset;
        uint32_t prog_length = (length < tailing_length) ? length : tailing_length;
        FLASH_PageProgramGeneral(type, offset, prog_length, buffer);
        buffer += prog_length;
        offset += prog_length;
        length -= prog_length;
    }

    cycles = length / FLASH_PAGE_SIZE;
    for(i = 0; i < cycles; i++){
        FLASH_PageProgramGeneral(type, offset, FLASH_PAGE_SIZE, buffer);
        offset += FLASH_PAGE_SIZE;
        buffer += FLASH_PAGE_SIZE;
    }

    remainder = length % FLASH_PAGE_SIZE;
    if(remainder) {
        FLASH_PageProgramGeneral(type, offset, remainder, buffer);
    }

    return 0;
}

uint8_t FLASH_Program(uint32_t offset, uint32_t length, uint8_t *buffer)
{
    return FLASH_ProgramGeneral(NORMAL_AREA, offset, length, buffer);
}


/**
 * @brief The Read Identification (RDID) command provides read access to
 * manufacturer identification, device identification, and Common Flash Interface
 * (CFI) information.The device identification indicates the memory type in the first byte,
 *and the memory capacity of the device in the second  byte.
 * @return uint32_t a value Manufacturer ID Device identification(Memory Type,Capacity)
 */
uint32_t FLASH_ReadID(void)
{
    uint8_t cmd;
    uint8_t read_back[3];
    uint32_t value = 0;

    cmd = FLASH_READ_ID;
    read_back[0] = 0; // Manufacturer ID
    read_back[1] = 0; // Memory Type (device identification high 8 bit)
    read_back[2] = 0; // Capacity    (device identification low 8 bit)

#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    qspi_standard_read_byte(read_back, sizeof(read_back), &cmd, sizeof(cmd));

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif

    value = (read_back[0] << 16) | (read_back[1] << 8) | (read_back[2]);
    return value;
}

/**
 * @brief The READ_ID command identifies the Device Manufacturer ID and the
 * Device ID. The command is also referred to as Read Electronic Manufacturer
 * and device Signature (REMS).
 *
 * @return uint32_t a value contains Manufacturer ID and Device ID.
 */
uint16_t FLASH_ReadDeviceID(void)
{
    uint8_t cmd_buf[4];
    uint8_t read_back[2]; // Manufacturer ID, Device ID
    uint16_t value = 0;

    cmd_buf[0] = FLASH_READ_DEVICE_ID;
    cmd_buf[1] = 0;//dumy data
    cmd_buf[2] = 0;//dumy data
    cmd_buf[3] = 0;//dumy data

#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    qspi_standard_read_byte(read_back, sizeof(read_back), cmd_buf, sizeof(cmd_buf));

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif

    value = (read_back[0] << 8) | (read_back[1]);
    return value;
}

/**
 * @brief The Read Status Register-1 (RDSR1) command allows the Status Register-1
 * contents to be read from SO. The Status Register-1 contents may be read at any
 * time, even while a program, erase, or write operation is in progress.
 *
 * @return uint8_t status value
 */
uint8_t FLASH_ReadSR1(void)
{
    uint8_t cmd = FLASH_READ_STATUS_REG_1;
    uint8_t status = 0;

#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    qspi_standard_read_byte(&status, sizeof(status), &cmd, sizeof(cmd));

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif

    return status;
}

/**
 * @brief The Read Status Register-2 (RDSR2) command allows the Status Register-2
 * contents to be read from SO. The Status Register-2 contents may be read at any
 * time, even while a program, erase, or write operation is in progress.
 *
 * @return uint8_t status value
 */
uint8_t FLASH_ReadSR2(void)
{
    uint8_t cmd = FLASH_READ_STATUS_REG_2;
    uint8_t status = 0;

#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    qspi_standard_read_byte( &status, sizeof(status), &cmd, sizeof(cmd));

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif

    return status;
}

/**
 * @brief Read FLASH status 16bits.
 * @return uint32_t status 16bits.
 */
uint16_t FLASH_ReadStatus(void)
{
    uint8_t low = 0, high = 0;
    uint16_t value = 0;

    low  = FLASH_ReadSR1();
    high = FLASH_ReadSR2();

    value = ( high << 8 ) | (low & 0xFF);
    return value;
}

/**
 * @brief Send command Program/Erase Suspend
 * @return uint16_t
 */
void FLASH_ProgramEraseSuspend(void)
{
    uint8_t cmd = FLASH_SUSPEND;
#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    qspi_standard_write(&cmd, sizeof(cmd));

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif
}

/**
 * @brief Send command Program/Erase Resume
 * @return uint16_t
 */
void FLASH_ProgramEraseResume(void)
{
    uint8_t cmd = FLASH_RESUME;
#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    qspi_standard_write(&cmd, sizeof(cmd));

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif
}

/**
 * @brief Flash Write Enable
 */
void FLASH_WriteEnable(void)
{
    uint8_t cmd = 0;
    cmd = FLASH_WRITE_ENABLE;

    qspi_standard_write(&cmd, sizeof(cmd));
}

/**
 * @brief Flash Write Disable
 */
void FLASH_WriteDisable(void)
{
    uint8_t cmd = 0;
    cmd = FLASH_WRITE_DISABLE;

    qspi_standard_write(&cmd, sizeof(cmd));
}

/**
 * @brief Flash One Time Program(OTP)
 * @note Warning!!! There is only one chance to lock the OTP£¡
 */
void FLASH_LockOTP(void)
{
    uint8_t read_sr1_cmd = FLASH_READ_STATUS_REG_1;
    uint8_t read_sr2_cmd = FLASH_READ_STATUS_REG_2;
    uint8_t write_sr_cmd[3];
    FlashStatusReg1_t status1;
    FlashStatusReg2_t status2;

#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    qspi_standard_read_byte(&status1.reg1_data, sizeof(uint8_t), &read_sr1_cmd, sizeof(read_sr1_cmd));
    qspi_standard_read_byte(&status2.reg2_data, sizeof(uint8_t), &read_sr2_cmd, sizeof(read_sr2_cmd));

    status2.bits.LB = 1;

    write_sr_cmd[0] = FLASH_WRITE_NON_VOLATILE_SR;
    write_sr_cmd[1] = status1.reg1_data;
    write_sr_cmd[2] = status2.reg2_data;

    FLASH_WriteEnable();
    qspi_standard_write(write_sr_cmd, sizeof(write_sr_cmd));
    FLASH_OperationWait();

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif
}

/**
 * @brief Get Flash OTP State
 * @return 1:locked  0:unlock
 */
uint8_t FLASH_GetOTPState(void)
{
    uint8_t read_sr2_cmd = FLASH_READ_STATUS_REG_2;
    FlashStatusReg2_t status2;

#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    qspi_standard_read_byte( &status2.reg2_data, sizeof(uint8_t), &read_sr2_cmd, sizeof(read_sr2_cmd));

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif

    return (uint8_t)status2.bits.LB;
}

/**
 * @brief Flash Erase Security Area
 * @note Warning!!!(If the OTP Lock Bit is already set to 1, erase operation does not take effect.)
 */
void FLASH_SecurityAreaErase(uint32_t offset)
{
    uint8_t erase_cmd_addr[4];
    assert_param(offset <= FLASH_SECURITY_SIZE_MAX);

    erase_cmd_addr[0]= FLASH_SECURITY_ERASE;
    erase_cmd_addr[1]= (offset >> 16)&0xff;
    erase_cmd_addr[2]= (offset >> 8 )&0xff;
    erase_cmd_addr[3]= (offset      )&0xff;

#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    FLASH_WriteEnable();
    qspi_standard_write(erase_cmd_addr, sizeof(erase_cmd_addr));
    FLASH_OperationWait();

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif
}

/**
 * @brief Flash Program Security Area
 * @note Warning!!!(If the OTP Lock Bit is already set to 1, program operation does not take effect.)
 */
uint8_t FLASH_SecurityAreaProgram(uint32_t offset, uint32_t len, uint8_t * buf)
{
    assert_param(offset <= FLASH_SECURITY_SIZE_MAX);
    assert_param((offset+len) <= FLASH_SECURITY_SIZE_MAX);

    return FLASH_ProgramGeneral(SECURITY_AREA, offset, len, buf);
}


/**
 * @brief Flash Read Security Area
 */
void FLASH_SecurityAreaRead(uint32_t offset, uint32_t len, uint8_t * buf)
{
    assert_param((offset+len) <= FLASH_SECURITY_SIZE_MAX);

    uint8_t read_cmd[5];
    uint8_t page_num   = len / FLASH_SECURITY_PAGE_SIZE;
    uint16_t remainder = len % FLASH_SECURITY_PAGE_SIZE;
    uint16_t i = 0;

#if (FLASH_XIP == 1)
    GLOBAL_INT_DISABLE();
    flash_cache_disable();
#endif

    for(i = 0; i < page_num; i++){
        read_cmd[0]= FLASH_SECURITY_READ;
        read_cmd[1]= (offset>>16)&0xff;
        read_cmd[2]= (offset>>8 )&0xff;
        read_cmd[3]= (offset    )&0xff;
        read_cmd[4]= 0;//dumy data @XTX Flash Datasheet.
        qspi_standard_read_byte(buf, FLASH_SECURITY_PAGE_SIZE, read_cmd, sizeof(read_cmd));
        buf    += FLASH_SECURITY_PAGE_SIZE;
        offset += FLASH_SECURITY_PAGE_SIZE;
    }

    if(remainder){
        read_cmd[0]= FLASH_SECURITY_READ;
        read_cmd[1]= (offset>>16)&0xff;
        read_cmd[2]= (offset>>8 )&0xff;
        read_cmd[3]= (offset    )&0xff;
        read_cmd[4]= 0;//dumy data @XTX Flash Datasheet.
        qspi_standard_read_byte(buf, remainder, read_cmd, sizeof(read_cmd));
    }

#if (FLASH_XIP == 1)
    flash_cache_init(0);
    GLOBAL_INT_RESTORE();
#endif
}

/**
 * @brief Set Quad mode.
 * That is, WP# becomes IO2 and HOLD# becoms IO3. The WP# and HOLD# inputs are not monitored
 * for their normal functions and are internally set to haigh (inactive).
 *
 * @return uint8_t
 */
void FLASH_QuadModeEnable(uint8_t enable)
{
    uint8_t read_sr1_cmd = FLASH_READ_STATUS_REG_1;
    uint8_t read_sr2_cmd = FLASH_READ_STATUS_REG_2;
    uint8_t write_sr_cmd[3];
    FlashStatusReg1_t status1={0};
    FlashStatusReg2_t status2={0};

    qspi_standard_read_byte(&status1.reg1_data, sizeof(uint8_t), &read_sr1_cmd, sizeof(read_sr1_cmd));
    qspi_standard_read_byte(&status2.reg2_data, sizeof(uint8_t), &read_sr2_cmd, sizeof(read_sr2_cmd));
    
    enable = (enable ? 1 : 0);
    if(((status2.bits.CMP != 0)||(status1.bits.BP0_4 != 0)||(status1.bits.SRP != 0)||(status2.bits.QE != enable)))
    {
        status2.bits.CMP = 0;
        status1.bits.BP0_4 = 0;
        status1.bits.SRP = 0;
        status2.bits.QE = enable;
    }
    
    write_sr_cmd[0] = FLASH_WRITE_NON_VOLATILE_SR;
    write_sr_cmd[1] = status1.reg1_data;
    write_sr_cmd[2] = status2.reg2_data;

    FLASH_WriteEnable();
    qspi_standard_write(write_sr_cmd, sizeof(write_sr_cmd));
    FLASH_OperationWait();
}

void FLASH_OperationWait(void)
{
    uint8_t flash_status,read_stat_cmd = FLASH_READ_STATUS_REG_1;
    do
    {
        qspi_standard_read_byte(&flash_status, sizeof(flash_status), &read_stat_cmd, sizeof(read_stat_cmd));
    } while( flash_status & 0x1);
}

#pragma pop


