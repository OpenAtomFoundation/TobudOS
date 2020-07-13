#ifndef __SD_CARD_H__
#define __SD_CARD_H__
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "types.h"

///////////////////////////////////////////////////////////////////////////////
////////      SD card interface, only supported for SPI operations   //////////
///////////////////////////////////////////////////////////////////////////////


////////////////////////////////  SD Type  ////////////////////////////////////
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06

//////////////////////////////////  SD command  ///////////////////////////////
#define CMD0    0       // reset
#define CMD1    1
#define CMD8    8       // SEND_IF_COND
#define CMD9    9       // read CSD
#define CMD10   10      // read CID
#define CMD12   12      // stop transmission
#define CMD16   16      // set SectorSize, it should return 0x00
#define CMD17   17      // read sector
#define CMD18   18      // read multi sectors
#define CMD23   23      // set multi sectors, erase multi blocks before writing.
#define CMD24   24      // write sector
#define CMD25   25      // write multi sectors
#define CMD41   41      // it should return 0x00
#define CMD55   55      // it should return 0x01
#define CMD58   58      // read OCR information
#define CMD59   59      // enable/disable CRC, it should return 0x00

// response when writing data
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF

// response when sending command
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF

uint8_t  SD_Init(void);
uint8_t  SD_GetType(void);
uint8_t  SD_SPI_ReadWriteByte(uint8_t data);
void     SD_SPI_SpeedLow(void);
void     SD_SPI_SpeedHigh(void);
uint8_t  SD_WaitReady(void);
uint8_t  SD_GetResponse(uint8_t Response);
uint8_t  SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);
uint8_t  SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);
uint32_t SD_GetSectorCount(void);
uint8_t  SD_GetCID(uint8_t *cid_data);
uint8_t  SD_GetCSD(uint8_t *csd_data);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __SD_CARD_H__
