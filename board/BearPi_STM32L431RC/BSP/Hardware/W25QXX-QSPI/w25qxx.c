#include "w25qxx.h"
#include "quadspi.h"

int w25qxx_init(void)
{
    MX_QUADSPI_Init();
    return 0;
}

int w25qxx_memory_mapped(void)
{
    QSPI_CommandTypeDef cmd = {
        // Instruction
        .InstructionMode = QSPI_INSTRUCTION_1_LINE,
        .Instruction = FAST_READ_DUAL_IO_CMD,
        // Address
        .AddressMode = QSPI_ADDRESS_2_LINES,
        .Address = 0, // NOT USED for memory-mapped mode
        .AddressSize = QSPI_ADDRESS_24_BITS,
        // AlternateByte
        .AlternateByteMode = QSPI_ALTERNATE_BYTES_2_LINES,
        .AlternateBytes = 0xF0, // M7-M0 should be set to Fxh
        .AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS, 
        // Dummy
        .DummyCycles = 0,
        // Data
        .DataMode = QSPI_DATA_2_LINES,
        .NbData = 0, // NOT USED for memory-mapped mode
        // DDR
        .DdrMode = QSPI_DDR_MODE_DISABLE,
    };
    QSPI_MemoryMappedTypeDef cfg = {
        .TimeOutPeriod = 0,
        .TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE,
    };
    if (HAL_QSPI_GetState(&hqspi) != HAL_QSPI_STATE_BUSY_MEM_MAPPED) {
        if (HAL_QSPI_Abort(&hqspi) != HAL_OK) {
            return -1;
        }
        if (HAL_QSPI_MemoryMapped(&hqspi, &cmd, &cfg) != HAL_OK) {
            return -1;
        }
    }
    return 0;
}

uint16_t w25qxx_read_deviceid(void)
{
    uint8_t recv_buf[2] = {0};
    uint16_t device_id = 0;
    QSPI_CommandTypeDef cmd = {
        // Instruction
        .InstructionMode = QSPI_INSTRUCTION_1_LINE,
        .Instruction = ManufactDeviceID_CMD,
        // Address
        .AddressMode = QSPI_ADDRESS_1_LINE,
        .Address = 0,
        .AddressSize = QSPI_ADDRESS_24_BITS,
        // AlternateByte
        .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
        // Dummy
        .DummyCycles = 0,
        // Data
        .DataMode = QSPI_DATA_1_LINE,
        .NbData = 2,
        // DDR
        .DdrMode = QSPI_DDR_MODE_DISABLE,
    };
    if (HAL_QSPI_Abort(&hqspi) != HAL_OK) {
        return 0;
    }
    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return 0;
    }
    if (HAL_QSPI_Receive(&hqspi, recv_buf, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return 0;
    }
    device_id = (recv_buf[0] << 8) | recv_buf[1];
    return device_id;
}

static void w25qxx_wait_busy(void) 
{
    uint8_t status;
    QSPI_CommandTypeDef cmd = {
        // Instruction
        .InstructionMode = QSPI_INSTRUCTION_1_LINE, 
        .Instruction = READ_STATU_REGISTER_1,
        // Address
        .AddressMode = QSPI_ADDRESS_NONE,
        // AlternateByte
        .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
        // Dummy
        .DummyCycles = 0,
        // Data
        .DataMode = QSPI_DATA_1_LINE,
        .NbData = 1,
        // DDR
        .DdrMode = QSPI_DDR_MODE_DISABLE,
    };
    HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
    do {
        HAL_QSPI_Receive(&hqspi, &status, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
    } while ((status & 0x01) == 0x01);
}

int w25qxx_read(uint8_t *buffer, uint32_t start_addr, uint16_t nbytes)
{
    QSPI_CommandTypeDef cmd = {
        // Instruction
        .InstructionMode = QSPI_INSTRUCTION_1_LINE,
        .Instruction = FAST_READ_DUAL_IO_CMD,
        // Address
        .AddressMode = QSPI_ADDRESS_2_LINES,
        .Address = start_addr,
        .AddressSize = QSPI_ADDRESS_24_BITS,
        // AlternateByte
        .AlternateByteMode = QSPI_ALTERNATE_BYTES_2_LINES,
        .AlternateBytes = 0xF0, // M7-M0 should be set to Fxh
        .AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS, 
        // Dummy
        .DummyCycles = 0,
        // Data
        .DataMode = QSPI_DATA_2_LINES,
        .NbData = nbytes, 
        // DDR
        .DdrMode = QSPI_DDR_MODE_DISABLE,
    };
    if (HAL_QSPI_Abort(&hqspi) != HAL_OK) {
        return -1;
    }
    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return -1;
    }
    if (HAL_QSPI_Receive(&hqspi, buffer, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return -1;
    }
    return 0;
}

static void w25qxx_write_enable(void)
{
    QSPI_CommandTypeDef cmd = {
        // Instruction
        .InstructionMode = QSPI_INSTRUCTION_1_LINE,
        .Instruction = WRITE_ENABLE_CMD,
        // Address
        .AddressMode = QSPI_ADDRESS_NONE,
        // AlternateByte
        .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
        // Dummy
        .DummyCycles = 0,
        // Data
        .DataMode = QSPI_DATA_NONE,
        // DDR
        .DdrMode = QSPI_DDR_MODE_DISABLE,
    };
    HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
    // w25qxx_wait_busy();
}

static void w25qxx_write_disable(void)
{
    QSPI_CommandTypeDef cmd = {
        // Instruction
        .InstructionMode = QSPI_INSTRUCTION_1_LINE,
        .Instruction = WRITE_DISABLE_CMD,
        // Address
        .AddressMode = QSPI_ADDRESS_NONE,
        // AlternateByte
        .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
        // Dummy
        .DummyCycles = 0,
        // Data
        .DataMode = QSPI_DATA_NONE,
        // DDR
        .DdrMode = QSPI_DDR_MODE_DISABLE,
    };
    HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
    // w25qxx_wait_busy();
}

int w25qxx_erase_sector(uint32_t sector_addr)
{
    QSPI_CommandTypeDef cmd = {
        // Instruction
        .InstructionMode = QSPI_INSTRUCTION_1_LINE,
        .Instruction = SECTOR_ERASE_CMD,
        // Address
        .AddressMode = QSPI_ADDRESS_1_LINE,
        .Address = sector_addr,
        .AddressSize = QSPI_ADDRESS_24_BITS,
        // AlternateByte
        .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
        // Dummy
        .DummyCycles = 0,
        // Data
        .DataMode = QSPI_DATA_NONE,
        // DDR
        .DdrMode = QSPI_DDR_MODE_DISABLE,
    };

    if (HAL_QSPI_Abort(&hqspi) != HAL_OK) {
        return -1;
    }
        
    w25qxx_wait_busy();

    w25qxx_write_enable();
    
    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return -1;
    }
    
    w25qxx_wait_busy();    

    return 0;
}

int w25qxx_erase_chip(void) 
{
    QSPI_CommandTypeDef cmd = {
        // Instruction
        .InstructionMode = QSPI_INSTRUCTION_1_LINE,
        .Instruction = CHIP_ERASE_CMD,
        // Address
        .AddressMode = QSPI_ADDRESS_NONE,
        // AlternateByte
        .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
        // Dummy
        .DummyCycles = 0,
        // Data
        .DataMode = QSPI_DATA_NONE,
        // DDR
        .DdrMode = QSPI_DDR_MODE_DISABLE,
    };

    if (HAL_QSPI_Abort(&hqspi) != HAL_OK) {
        return -1;
    }
        
    w25qxx_wait_busy();

    w25qxx_write_enable();

    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return -1;
    }
    
    w25qxx_wait_busy();

    return 0;
}

int w25qxx_page_program(uint8_t *dat, uint32_t write_addr, uint16_t nbytes) 
{
    QSPI_CommandTypeDef cmd = {
        // Instruction
        .InstructionMode = QSPI_INSTRUCTION_1_LINE,
        .Instruction = PAGE_PROGRAM_CMD,
        // Address
        .AddressMode = QSPI_ADDRESS_1_LINE,
        .Address = write_addr,
        .AddressSize = QSPI_ADDRESS_24_BITS,
        // AlternateByte
        .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
        // Dummy
        .DummyCycles = 0,
        // Data
        .DataMode = QSPI_DATA_1_LINE,
        .NbData = nbytes,
        // DDR
        .DdrMode = QSPI_DDR_MODE_DISABLE,
    };

    if (HAL_QSPI_Abort(&hqspi) != HAL_OK) {
        return -1;
    }

    w25qxx_wait_busy();

    w25qxx_write_enable();

    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return -1;
    }
    if (HAL_QSPI_Transmit(&hqspi, dat, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return -1;
    }

    w25qxx_wait_busy();

    return 0;
}

int w25qxx_reset(void)
{
    QSPI_CommandTypeDef cmd1 = {
        // Instruction
        .InstructionMode = QSPI_INSTRUCTION_1_LINE,
        .Instruction = ENABLE_RESET_CMD,
        // Address
        .AddressMode = QSPI_ADDRESS_NONE,
        // AlternateByte
        .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
        // Dummy
        .DummyCycles = 0,
        // Data
        .DataMode = QSPI_DATA_NONE,
        // DDR
        .DdrMode = QSPI_DDR_MODE_DISABLE,
    };
    QSPI_CommandTypeDef cmd2 = {
        // Instruction
        .InstructionMode = QSPI_INSTRUCTION_1_LINE,
        .Instruction = RESET_DEVICE_CMD,
        // Address
        .AddressMode = QSPI_ADDRESS_NONE,
        // AlternateByte
        .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
        // Dummy
        .DummyCycles = 0,
        // Data
        .DataMode = QSPI_DATA_NONE,
        // DDR
        .DdrMode = QSPI_DDR_MODE_DISABLE,
    };
        
    if (HAL_QSPI_Abort(&hqspi) != HAL_OK) {
        return -1;
    }
        
    if (HAL_QSPI_Command(&hqspi, &cmd1, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return -1;
    }
    if (HAL_QSPI_Command(&hqspi, &cmd2, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return -1;
    }
        
    w25qxx_wait_busy();
    
    return 0;
}
