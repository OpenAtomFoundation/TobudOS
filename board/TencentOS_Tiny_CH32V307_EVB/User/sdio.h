/********************************** (C) COPYRIGHT *******************************
* File Name          : sdio.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains the headers of the SDIO.
*******************************************************************************/
#ifndef __SDIO_H
#define __SDIO_H

#include"debug.h"

#define  delay_ms(x)  Delay_Ms(x)


/*SDIO flag*/
#define SDIO_FLAG_CCRCFAIL                  ((uint32_t)0x00000001)
#define SDIO_FLAG_DCRCFAIL                  ((uint32_t)0x00000002)
#define SDIO_FLAG_CTIMEOUT                  ((uint32_t)0x00000004)
#define SDIO_FLAG_DTIMEOUT                  ((uint32_t)0x00000008)
#define SDIO_FLAG_TXUNDERR                  ((uint32_t)0x00000010)
#define SDIO_FLAG_RXOVERR                   ((uint32_t)0x00000020)
#define SDIO_FLAG_CMDREND                   ((uint32_t)0x00000040)
#define SDIO_FLAG_CMDSENT                   ((uint32_t)0x00000080)
#define SDIO_FLAG_DATAEND                   ((uint32_t)0x00000100)
#define SDIO_FLAG_STBITERR                  ((uint32_t)0x00000200)
#define SDIO_FLAG_DBCKEND                   ((uint32_t)0x00000400)
#define SDIO_FLAG_CMDACT                    ((uint32_t)0x00000800)
#define SDIO_FLAG_TXACT                     ((uint32_t)0x00001000)
#define SDIO_FLAG_RXACT                     ((uint32_t)0x00002000)
#define SDIO_FLAG_TXFIFOHE                  ((uint32_t)0x00004000)
#define SDIO_FLAG_RXFIFOHF                  ((uint32_t)0x00008000)
#define SDIO_FLAG_TXFIFOF                   ((uint32_t)0x00010000)
#define SDIO_FLAG_RXFIFOF                   ((uint32_t)0x00020000)
#define SDIO_FLAG_TXFIFOE                   ((uint32_t)0x00040000)
#define SDIO_FLAG_RXFIFOE                   ((uint32_t)0x00080000)
#define SDIO_FLAG_TXDAVL                    ((uint32_t)0x00100000)
#define SDIO_FLAG_RXDAVL                    ((uint32_t)0x00200000)
#define SDIO_FLAG_SDIOIT                    ((uint32_t)0x00400000)
#define SDIO_FLAG_CEATAEND                  ((uint32_t)0x00800000)

/* SDIO clock */
#define SDIO_INIT_CLK_DIV        0xFE
#define SDIO_TRANSFER_CLK_DIV    0x00

/* SDIO work mode */
#define SD_POLLING_MODE     0
#define SD_DMA_MODE         1

/*SDIO Err define */
typedef enum
{
    /* special err */
    SD_CMD_CRC_FAIL                    = (1),
    SD_DATA_CRC_FAIL                   = (2),
    SD_CMD_RSP_TIMEOUT                 = (3),
    SD_DATA_TIMEOUT                    = (4),
    SD_TX_UNDERRUN                     = (5),
    SD_RX_OVERRUN                      = (6),
    SD_START_BIT_ERR                   = (7),
    SD_CMD_OUT_OF_RANGE                = (8),
    SD_ADDR_MISALIGNED                 = (9),
    SD_BLOCK_LEN_ERR                   = (10),
    SD_ERASE_SEQ_ERR                   = (11),
    SD_BAD_ERASE_PARAM                 = (12),
    SD_WRITE_PROT_VIOLATION            = (13),
    SD_LOCK_UNLOCK_FAILED              = (14),
    SD_COM_CRC_FAILED                  = (15),
    SD_ILLEGAL_CMD                     = (16),
    SD_CARD_ECC_FAILED                 = (17),
    SD_CC_ERROR                        = (18),
    SD_GENERAL_UNKNOWN_ERROR           = (19),
    SD_STREAM_READ_UNDERRUN            = (20),
    SD_STREAM_WRITE_OVERRUN            = (21),
    SD_CID_CSD_OVERWRITE               = (22),
    SD_WP_ERASE_SKIP                   = (23),
    SD_CARD_ECC_DISABLED               = (24),
    SD_ERASE_RESET                     = (25),
    SD_AKE_SEQ_ERROR                   = (26),
    SD_INVALID_VOLTRANGE               = (27),
    SD_ADDR_OUT_OF_RANGE               = (28),
    SD_SWITCH_ERROR                    = (29),
    SD_SDIO_DISABLED                   = (30),
    SD_SDIO_FUNCTION_BUSY              = (31),
    SD_SDIO_FUNCTION_FAILED            = (32),
    SD_SDIO_UNKNOWN_FUNCTION           = (33),
    /* Standard err */
    SD_INTERNAL_ERROR,
    SD_NOT_CONFIGURED,
    SD_REQUEST_PENDING,
    SD_REQUEST_NOT_APPLICABLE,
    SD_INVALID_PARAMETER,
    SD_UNSUPPORTED_FEATURE,
    SD_UNSUPPORTED_HW,
    SD_ERROR,
    SD_OK = 0
} SD_Error;

/*SD——CSD*/
typedef struct
{
    u8  CSDStruct;
    u8  SysSpecVersion;
    u8  Reserved1;
    u8  TAAC;
    u8  NSAC;
    u8  MaxBusClkFrec;
    u16 CardComdClasses;
    u8  RdBlockLen;
    u8  PartBlockRead;
    u8  WrBlockMisalign;
    u8  RdBlockMisalign;
    u8  DSRImpl;
    u8  Reserved2;
    u32 DeviceSize;
    u8  MaxRdCurrentVDDMin;
    u8  MaxRdCurrentVDDMax;
    u8  MaxWrCurrentVDDMin;
    u8  MaxWrCurrentVDDMax;
    u8  DeviceSizeMul;
    u8  EraseGrSize;
    u8  EraseGrMul;
    u8  WrProtectGrSize;
    u8  WrProtectGrEnable;
    u8  ManDeflECC;
    u8  WrSpeedFact;
    u8  MaxWrBlockLen;
    u8  WriteBlockPaPartial;
    u8  Reserved3;
    u8  ContentProtectAppli;
    u8  FileFormatGrouop;
    u8  CopyFlag;
    u8  PermWrProtect;
    u8  TempWrProtect;
    u8  FileFormat;
    u8  ECC;
    u8  CSD_CRC;
    u8  Reserved4;
} SD_CSD;   

/*SD——CID*/
typedef struct
{
    u8  ManufacturerID;
    u16 OEM_AppliID;
    u32 ProdName1;
    u8  ProdName2;
    u8  ProdRev;
    u32 ProdSN;
    u8  Reserved1;
    u16 ManufactDate;
    u8  CID_CRC;
    u8  Reserved2;
} SD_CID;

/* SD statue */
typedef enum
{
    SD_CARD_READY                  = ((uint32_t)0x00000001),
    SD_CARD_IDENTIFICATION         = ((uint32_t)0x00000002),
    SD_CARD_STANDBY                = ((uint32_t)0x00000003),
    SD_CARD_TRANSFER               = ((uint32_t)0x00000004),
    SD_CARD_SENDING                = ((uint32_t)0x00000005),
    SD_CARD_RECEIVING              = ((uint32_t)0x00000006),
    SD_CARD_PROGRAMMING            = ((uint32_t)0x00000007),
    SD_CARD_DISCONNECTED           = ((uint32_t)0x00000008),
    SD_CARD_ERROR                  = ((uint32_t)0x000000FF)
}SDCardState;

/* SD informatiion */
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  long long CardCapacity;
  u32 CardBlockSize;
  u16 RCA;
  u8 CardType;
} SD_CardInfo;
extern SD_CardInfo SDCardInfo;

/* SDIO instruction*/
#define SD_CMD_GO_IDLE_STATE                       ((u8)0)
#define SD_CMD_SEND_OP_COND                        ((u8)1)
#define SD_CMD_ALL_SEND_CID                        ((u8)2)
#define SD_CMD_SET_REL_ADDR                        ((u8)3)
#define SD_CMD_SET_DSR                             ((u8)4)
#define SD_CMD_SDIO_SEN_OP_COND                    ((u8)5)
#define SD_CMD_HS_SWITCH                           ((u8)6)
#define SD_CMD_SEL_DESEL_CARD                      ((u8)7)
#define SD_CMD_HS_SEND_EXT_CSD                     ((u8)8)
#define SD_CMD_SEND_CSD                            ((u8)9)
#define SD_CMD_SEND_CID                            ((u8)10)
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((u8)11)
#define SD_CMD_STOP_TRANSMISSION                   ((u8)12)
#define SD_CMD_SEND_STATUS                         ((u8)13)
#define SD_CMD_HS_BUSTEST_READ                     ((u8)14)
#define SD_CMD_GO_INACTIVE_STATE                   ((u8)15)
#define SD_CMD_SET_BLOCKLEN                        ((u8)16)
#define SD_CMD_READ_SINGLE_BLOCK                   ((u8)17)
#define SD_CMD_READ_MULT_BLOCK                     ((u8)18)
#define SD_CMD_HS_BUSTEST_WRITE                    ((u8)19)
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((u8)20) 
#define SD_CMD_SET_BLOCK_COUNT                     ((u8)23) 
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((u8)24)
#define SD_CMD_WRITE_MULT_BLOCK                    ((u8)25)
#define SD_CMD_PROG_CID                            ((u8)26)
#define SD_CMD_PROG_CSD                            ((u8)27)
#define SD_CMD_SET_WRITE_PROT                      ((u8)28)
#define SD_CMD_CLR_WRITE_PROT                      ((u8)29)
#define SD_CMD_SEND_WRITE_PROT                     ((u8)30)
#define SD_CMD_SD_ERASE_GRP_START                  ((u8)32)
#define SD_CMD_SD_ERASE_GRP_END                    ((u8)33)
#define SD_CMD_ERASE_GRP_START                     ((u8)35)
#define SD_CMD_ERASE_GRP_END                       ((u8)36)
#define SD_CMD_ERASE                               ((u8)38)
#define SD_CMD_FAST_IO                             ((u8)39)
#define SD_CMD_GO_IRQ_STATE                        ((u8)40)
#define SD_CMD_LOCK_UNLOCK                         ((u8)42)
#define SD_CMD_APP_CMD                             ((u8)55)
#define SD_CMD_GEN_CMD                             ((u8)56)
#define SD_CMD_NO_CMD                              ((u8)64)

/*Following commands are SD Card Specific commands.
  SDIO_APP_CMD ：CMD55 should be sent before sending these commands.
*/
#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((u8)6)
#define SD_CMD_SD_APP_STAUS                        ((u8)13)
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((u8)22)
#define SD_CMD_SD_APP_OP_COND                      ((u8)41)
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((u8)42)
#define SD_CMD_SD_APP_SEND_SCR                     ((u8)51)
#define SD_CMD_SDIO_RW_DIRECT                      ((u8)52)
#define SD_CMD_SDIO_RW_EXTENDED                    ((u8)53)

/*Following commands are SD Card Specific security commands.
  SDIO_APP_CMD should be sent before sending these commands.
*/
#define SD_CMD_SD_APP_GET_MKB                      ((u8)43)
#define SD_CMD_SD_APP_GET_MID                      ((u8)44)
#define SD_CMD_SD_APP_SET_CER_RN1                  ((u8)45)
#define SD_CMD_SD_APP_GET_CER_RN2                  ((u8)46)
#define SD_CMD_SD_APP_SET_CER_RES2                 ((u8)47)
#define SD_CMD_SD_APP_GET_CER_RES1                 ((u8)48)
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK   ((u8)18)
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  ((u8)25)
#define SD_CMD_SD_APP_SECURE_ERASE                 ((u8)38)
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA           ((u8)49)
#define SD_CMD_SD_APP_SECURE_WRITE_MKB             ((u8)48)

/* SD definition */
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((u32)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((u32)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((u32)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((u32)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((u32)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((u32)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((u32)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((u32)0x00000007)

/* SDIO parameter definition */
#define SDIO_STATIC_FLAGS               ((u32)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((u32)0x00010000)
#define SDIO_DATATIMEOUT                ((u32)0xFFFFFFFF)
#define SDIO_FIFO_Address               ((u32)0x40018080)

/* Mask for errors Card Status R1 (OCR Register) */
#define SD_OCR_ADDR_OUT_OF_RANGE        ((u32)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((u32)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((u32)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((u32)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((u32)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((u32)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((u32)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((u32)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((u32)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((u32)0x00200000)
#define SD_OCR_CC_ERROR                 ((u32)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((u32)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((u32)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((u32)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((u32)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((u32)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((u32)0x00004000)
#define SD_OCR_ERASE_RESET              ((u32)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((u32)0x00000008)
#define SD_OCR_ERRORBITS                ((u32)0xFDFFE008)

/* Masks for R6 Response */
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((u32)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((u32)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((u32)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((u32)0x80100000)
#define SD_HIGH_CAPACITY                ((u32)0x40000000)
#define SD_STD_CAPACITY                 ((u32)0x00000000)
#define SD_CHECK_PATTERN                ((u32)0x000001AA)
#define SD_VOLTAGE_WINDOW_MMC           ((u32)0x80FF8000)

#define SD_MAX_VOLT_TRIAL               ((u32)0x0000FFFF)
#define SD_ALLZERO                      ((u32)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((u32)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((u32)0x00010000)
#define SD_CARD_LOCKED                  ((u32)0x02000000)
#define SD_CARD_PROGRAMMING             ((u32)0x00000007)
#define SD_CARD_RECEIVING               ((u32)0x00000006)
#define SD_DATATIMEOUT                  ((u32)0xFFFFFFFF)
#define SD_0TO7BITS                     ((u32)0x000000FF)
#define SD_8TO15BITS                    ((u32)0x0000FF00)
#define SD_16TO23BITS                   ((u32)0x00FF0000)
#define SD_24TO31BITS                   ((u32)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((u32)0x01FFFFFF)

#define SD_HALFFIFO                     ((u32)0x00000008)
#define SD_HALFFIFOBYTES                ((u32)0x00000020)

/*Command Class Supported */
#define SD_CCCC_LOCK_UNLOCK             ((u32)0x00000080)
#define SD_CCCC_WRITE_PROT              ((u32)0x00000040)
#define SD_CCCC_ERASE                   ((u32)0x00000020)

/*CMD8*/
#define SDIO_SEND_IF_COND               ((u32)0x00000008)

SD_Error SD_Init(void);
void SDIO_Clock_Set(u8 clkdiv);
void SDIO_Send_Cmd(u8 cmdindex,u8 waitrsp,u32 arg);
void SDIO_Send_Data_Cfg(u32 datatimeout,u32 datalen,u8 blksize,u8 dir);
SD_Error SD_PowerON(void);    
SD_Error SD_PowerOFF(void);
SD_Error SD_InitializeCards(void);
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo);
SD_Error SD_EnableWideBusOperation(u32 wmode);
SD_Error SD_SetDeviceMode(u32 mode);
SD_Error SD_SelectDeselect(u32 addr); 
SD_Error SD_SendStatus(uint32_t *pcardstatus);
SDCardState SD_GetState(void);
SD_Error SD_ReadBlock(u8 *buf,long long addr,u16 blksize);  
SD_Error SD_ReadMultiBlocks(u8 *buf,long long  addr,u16 blksize,u32 nblks);  
SD_Error SD_WriteBlock(u8 *buf,long long addr,  u16 blksize);
SD_Error SD_WriteMultiBlocks(u8 *buf,long long addr,u16 blksize,u32 nblks);
SD_Error SD_ProcessIRQSrc(void);
void SD_DMA_Config(u32*mbuf,u32 bufsize,u32 dir); 
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt);
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt);

#endif
