#ifndef __FLASH_H__
#define __FLASH_H__

#include "types.h"

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

/****************************  Data types and Macros  ************************/
#define FLASH_QUAD_READ_DUMMY_CLOCKS    8

#define FLASH_SECURITY_SIZE_MAX  (4*256)
#define FLASH_SECURITY_PAGE_SIZE (256)
        
#define FLASH_PAGE_SIZE         (256)
#define FALSH_SIZE_4K           (4 *1024)
#define FALSH_SIZE_BLOCK_32K    (32*1024)
#define FALSH_SIZE_BLOCK_64K    (64*1024)
#define FALSH_SIZE_MAX		    (2 *1024*1024)


typedef enum
{
    FLASH_WRITE_NON_VOLATILE_SR = 0x01,
    FLASH_WRITE_VOLATILE_SR     = 0x50,
    FLASH_READ_STATUS_REG_1   = 0x05,
    FLASH_READ_STATUS_REG_2   = 0x35,
                              
    FLASH_WRITE_DISABLE       = 0x04,
    FLASH_WRITE_ENABLE        = 0x06,
                              
    FLASH_STANDARD_READ       = 0x03,
    FLASH_DUAL_FAST_READ      = 0x3B,
    FLASH_QUAD_FAST_READ      = 0x6B,
                              
    FLASH_PAGE_PROGRAM        = 0x02,
    FLASH_QUAD_PAGE_PROGRAMM  = 0x32,
                              
    FLASH_SECTOR_ERASE        = 0x20,
    FLASH_32KB_BLOCK_ERASE    = 0x52,
    FLASH_64KB_BLOCK_ERASE    = 0xd8,
    FLASH_CHIP_ERASE          = 0x60,//0x60 or 0xc7
                              
    FLASH_SUSPEND             = 0x75,//Program/Erase Suspend
    FLASH_RESUME              = 0x7A,//Program/Erase Resume
    FLASH_READ_ID             = 0x9F,//Read Identification
    FLASH_READ_DEVICE_ID      = 0x90,//Read Manufacture ID/Device ID
    
    FLASH_SECURITY_ERASE      = 0x44,
    FLASH_SECURITY_PROGRAM    = 0x42,
    FLASH_SECURITY_READ       = 0x48,

    FLASH_POWER_DOWN          = 0xB9,
    FLASH_POWER_ON            = 0xAB,
} Flash_Cmd_t;

typedef enum
{
    NORMAL_AREA = 0,
    SECURITY_AREA,   
} Flash_AreaType;


typedef union
{
  struct
  {
      uint8_t WIP   : 1;//Write  In Progress
      uint8_t WEL   : 1;//Write  Enable Latch
      uint8_t BP0_4 : 5;//Block  Protect
      uint8_t SRP   : 1;//Status Register Protect        
  }bits;
  uint8_t reg1_data;
} FlashStatusReg1_t;

typedef union
{
  struct
  {
      uint8_t Res1  : 1;// 
      uint8_t QE    : 1;//Quad Enable
      uint8_t LB    : 1;//One Time Program   
      uint8_t Res2  : 3;// 
      uint8_t CMP   : 1;// 
      uint8_t Res3  : 1;
  }bits;
  uint8_t reg2_data;
} FlashStatusReg2_t;


typedef enum
{
    ERASE_SECTOR_4KB,
    ERASE_BLOCK_32KB,
    ERASE_BLOCK_64KB,
    ERASE_CHIP,
}Flash_EraseType_t;


/****************************  FLASH API  **************************/
void     FLASH_Init(void);
void     FLASH_Deinit(void);
uint8_t  FLASH_ReadByCache(uint32_t offset, uint32_t length, uint8_t *buffer);
uint8_t  FLASH_Read(uint32_t offset, uint32_t length, uint8_t *buffer);
uint8_t  FLASH_Program(uint32_t offset, uint32_t length, uint8_t *buffer);
void     FLASH_Erase(uint32_t offset, uint32_t length);//@param: offset,length(Note:4K Aligned)
void     FLASH_ChipErase(void);
uint8_t  FLASH_QuadProgram(uint32_t offset, uint32_t length, uint8_t *buffer);
uint32_t FLASH_ReadID(void);
uint16_t FLASH_ReadDeviceID(void);
uint8_t  FLASH_ReadSR1(void);
uint8_t  FLASH_ReadSR2(void);
uint16_t FLASH_ReadStatus(void);
void     FLASH_ProgramEraseSuspend(void);
void     FLASH_ProgramEraseResume(void);
void     FLASH_WriteEnable(void);
void     FLASH_WriteDisable(void);
void     FLASH_LockOTP(void);
uint8_t  FLASH_GetOTPState(void);
void     FLASH_QuadModeEnable(uint8_t enable);
void     FLASH_OperationWait(void);

void     FLASH_SecurityAreaErase(uint32_t offset);
uint8_t  FLASH_SecurityAreaProgram(uint32_t offset, uint32_t len, uint8_t * buf);
void     FLASH_SecurityAreaRead(uint32_t offset, uint32_t len, uint8_t * buf);


#ifdef __cplusplus
    }
#endif // __cplusplus
#endif // __FLASH_H__
