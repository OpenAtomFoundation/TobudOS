/**
  ******************************************************************************
  * @file    sd.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */  
#ifndef __CH_LIB_SD_H__
#define __CH_LIB_SD_H__

#include <stdint.h>


/* SD卡类型定义 */
#define SD_CARD_TYPE_NONE      (0)
#define SD_CARD_TYPE_UNKNOW    (1)
#define SD_CARD_TYPE_SD        (2)
#define SD_CARD_TYPE_SDHC      (3)
#define SD_CARD_TYPE_SDIO      (4)
#define SD_CARD_TYPE_SDCOMBO   (5)
#define SD_CARD_TYPE_SDHCCOMBO (6)
#define SD_CARD_TYPE_MMC       (7)
#define SD_CARD_CEATA          (8)   

/**
 * \struct SD_InitTypeDef
 * \brief SD初始化结构体
 */
typedef struct 
{
    uint32_t baudrate; //通信速度
}SD_InitTypeDef;

typedef struct 
{
    uint32_t attribute;
    uint32_t *address;
}adma2_t;


typedef struct 
{
  uint32_t cmd;
  uint32_t arg;
  uint32_t blkSize;
  uint32_t blkCount;
  uint32_t resp[4];
}SDHC_Cmd_t;

struct sd_card_handler
{
    uint32_t card_type;  
    uint32_t OCR;
    uint32_t CID[4];
    uint32_t CSD[4];
    uint16_t RCA;
    uint32_t CSR[2];
};     


#define ESDHC_OK                            (0)
#define ESDHC_ERROR_INIT_FAILED             (1)
#define ESDHC_ERROR_cmd_FAILED              (2)
#define ESDHC_ERROR_cmd_TIMEOUT             (3)
#define ESDHC_ERROR_DATA_TRANSFER           (4)
#define ESDHC_ERROR_INVALID_BUS_WIDTH       (5)


/* API funtctions */
uint8_t SD_Init(SD_InitTypeDef* SD_InitStruct);
uint32_t SD_GetSizeInMB(void);
uint8_t SD_ReadSingleBlock(uint32_t sector, uint8_t *buf);
uint8_t SD_WriteSingleBlock(uint32_t sector,  uint8_t *buf);
uint8_t SD_ReadMultiBlock(uint32_t sector, uint8_t *buf, uint32_t len);
uint8_t SD_WriteMultiBlock(uint32_t sector, uint8_t *buf, uint32_t len);
uint32_t SD_QuickInit(uint32_t baudrate);


#endif



