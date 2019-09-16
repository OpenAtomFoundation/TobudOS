#ifndef __CH_LIB_KL_SPI_H__
#define __CH_LIB_KL_SPI_H__

#include <stdint.h>
#include <stdbool.h>
  
//!< SPI模块设备
#define HW_SPI0     (0x00)
#define HW_SPI1     (0x01)
#define HW_SPI2     (0x02)

//!< SPI帧格式选择
typedef enum
{
    kSPI_CPOL0_CPHA0,
    kSPI_CPOL0_CPHA1,
    kSPI_CPOL1_CPHA0,
    kSPI_CPOL1_CPHA1
}SPI_FrameFormat_Type;

typedef enum
{
    kSPI_Master,
    kSPI_Slave,
} SPI_Mode_Type;


//!< interrupt and DMA select
typedef enum
{
    kSPI_IT_Rx,            //!< SPI接收到一帧
    kSPI_IT_Tx,            //!< SPI发送完一帧
}SPI_ITDMAConfig_Type;


#define kSPI_MSBFirst             (0x00)      //!< 先发送最高位
#define kSPI_LSBFirst             (0x01)      //!< 先发送最低位

//!< 初始化结构
typedef struct
{
    uint32_t                instance;               //!< 模块号
	SPI_Mode_Type           mode;                   //!< 主从模式
    uint8_t                 dataSize;               //!< 每帧数据有多少位 通常为8或16
    uint8_t                 bitOrder;               //!< 先发高位还是先发地位
    SPI_FrameFormat_Type    frameFormat;            //!< 四种帧格式选择
    uint32_t                baudrate;               //!< 速率
}SPI_InitTypeDef;

//!< 快速初始化结构
#define SPI1_SCK_PE02_MOSI_PE01_MISO_PE00  (0X0000C0A1U)
#define SPI0_SCK_PE17_MOSI_PE18_MISO_PE19  (0X0000E2A0U)
#define SPI0_SCK_PA15_MOSI_PA16_MISO_PA17  (0X0000DE80U)
#define SPI0_SCK_PC05_MOSI_PC06_MISO_PC07  (0X0000CA90U)
#define SPI0_SCK_PD01_MOSI_PD02_MISO_PD03  (0X0000C298U)
#define SPI1_SCK_PD05_MOSI_PD06_MISO_PD07  (0X0000CA99U)


//!< Callback Type
typedef void (*SPI_CallBackType)(void);

//!< API functions
void SPI_Init(SPI_InitTypeDef * SPI_InitStruct);
uint32_t SPI_QuickInit(uint32_t MAP, SPI_FrameFormat_Type frameFormat, uint32_t baudrate);
uint16_t SPI_ReadWriteByte(uint32_t instance, uint16_t data);
void SPI_ITDMAConfig(uint32_t instance, SPI_ITDMAConfig_Type config, bool status);
void SPI_CallbackInstall(uint32_t instance, SPI_CallBackType AppCBFun);



#endif

