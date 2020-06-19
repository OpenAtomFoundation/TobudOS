#ifndef __MEMORY_MAP_LN882X_H__
#define __MEMORY_MAP_LN882X_H__

#define SIZE_4KB                    (0x1000)
#define SIZE_8KB                    (2 * SIZE_4KB)
#define SIZE_16KB                   (4 * SIZE_4KB)
#define SIZE_32KB                   (8 * SIZE_4KB)
#define SIZE_64KB                   (16 * SIZE_4KB)
#define SIZE_128KB                  (32 * SIZE_4KB)
#define SIZE_2MB                    (32 * SIZE_64KB)

// Memory map is shown in the following figure:

// *--------------------------------------------------------------------------------------------------------------------*
// |        RAM_BLOCK0            |               RAM_BLOCK1                    |  RETENTION_MEM   | User unavailable   |
// *--------------------------------------------------------------------------------------------------------------------*
// |                              |              |               |              |                  |                    |
// |        SRAM0 128KB           |   SRAM1 64KB |   SRAM2 64KB  | SRAM3_0  32KB|  SRAM3_1  8KB    |    SRAM3_2  32KB   |
// |                              |              |               |              |  retention mem   |    cache mem       |
// *--------------------------------------------------------------------------------------------------------------------*
// @0x1FFE0000                    @0x20000000    @0x20010000     @0x20020000    @0x20028000        @0x2002A000          @0x20032000

//RAM
#define RAM_BASE					(0x1ffe0000)
#define SRAM0_BASE                  (RAM_BASE)
#define SRAM0_SIZE                  (SIZE_128KB)

#define SRAM1_BASE                  (SRAM0_BASE + SRAM0_SIZE)
#define SRAM1_SIZE                  (SIZE_64KB)

#define SRAM2_BASE                  (SRAM1_BASE + SRAM1_SIZE)
#define SRAM2_SIZE                  (SIZE_64KB)

#define SRAM3_0_BASE                (SRAM2_BASE + SRAM2_SIZE)
#define SRAM3_0_SIZE                (SIZE_32KB)

#define SRAM3_1_BASE                (SRAM3_0_BASE + SRAM3_0_SIZE)
#define SRAM3_1_SIZE                (SIZE_8KB)

#define SRAM3_2_BASE                (SRAM3_1_BASE + SRAM3_1_SIZE)
#define SRAM3_2_SIZE                (SIZE_32KB)

#define RETENTION_MEM_BASE			(SRAM3_1_BASE)
#define RETENTION_MEM_SIZE			(SIZE_8KB)

#define CACHE_MEM_BASE				(SRAM3_2_BASE)
#define CACHE_MEM_SIZE				(SIZE_32KB)

#define RAM_TOP						(CACHE_MEM_BASE + CACHE_MEM_SIZE)

#define RAM_BLOCK0_BASE             (RAM_BASE)
#define RAM_BLOCK0_SIZE             (SRAM0_SIZE)
#define RAM_BLOCK1_BASE             (SRAM1_BASE)
#define RAM_BLOCK1_SIZE             (SRAM1_SIZE + SRAM2_SIZE + SRAM3_0_SIZE)

//ROM and BOOTROM
#define ROM_MEM_BASE                (0x00000000)
#define ROM_MEM_SIZE				(SIZE_16KB)

#define BOOTROM_BASE 				(ROM_MEM_BASE)
#define BOOTROM_LIMIT				(ROM_MEM_SIZE)

#define BOOTROM_RW_SECTION_BASE	    (SRAM2_BASE)
#define BOOTROM_RW_SECTION_LIMIT    (SIZE_32KB)

//BOOTRAM
#define BOOTRAM_BASE                (RAM_BASE)
#define BOOTRAM_LIMIT               (SIZE_4KB*9)

//FLASH
#define CACHE_FLASH_BASE            (0x10000000)




#endif /* __MEMORY_MAP_LN882X_H__ */


