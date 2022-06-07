#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H
#include "debug.h"
#include "string.h"

/* Winbond SPIFalsh ID */
#define W25Q80  0XEF13
#define W25Q16  0XEF14
#define W25Q32  0XEF15
#define W25Q64  0XEF16
#define W25Q128 0XEF17

/* Winbond W25Q64 */
#define SPI_FLASH_PAGESIZE                         256
#define SPI_FLASH_SECTOR                           4096
#define SPI_FLASH_TOTAL_SIZE                       0x7FFFFF

/* Winbond SPIFalsh Instruction List */
#define W25X_WriteEnable		  0x06
#define W25X_WriteDisable		  0x04
#define W25X_ReadStatusReg		0x05
#define W25X_WriteStatusReg		0x01
#define W25X_ReadData			    0x03
#define W25X_FastReadData		  0x0B
#define W25X_FastReadDual		  0x3B
#define W25X_PageProgram		  0x02
#define W25X_BlockErase			  0xD8
#define W25X_SectorErase		  0x20
#define W25X_ChipErase			  0xC7
#define W25X_PowerDown			  0xB9
#define W25X_ReleasePowerDown	0xAB
#define W25X_DeviceID			    0xAB
#define W25X_ManufactDeviceID	0x90
#define W25X_JedecDeviceID		0x9F

#define SIZE sizeof(TEXT_Buf)
u8 SPI_FLASH_BUF[4096];






u8 SPI1_ReadWriteByte(u8 TxData);
void SPI_Flash_Init(void);
u8 SPI_Flash_ReadSR(void);
void SPI_FLASH_Write_SR(u8 sr);
void SPI_Flash_Wait_Busy(void);
void SPI_FLASH_Write_Enable(void);
void SPI_FLASH_Write_Disable(void);
u16 SPI_Flash_ReadID(void);
void SPI_Flash_Erase_Sector(u32 Dst_Addr);
int SPI_Flash_Erase(u32 Addr, int size);
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 size);
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 size);
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 size);
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 size);
void SPI_Flash_Erase_Chip(void);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);

#endif







