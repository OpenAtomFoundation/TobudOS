/**
  ******************************************************************************
  * @file    can.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.4.10
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#ifndef __CH_LIB_CAN_H__
#define __CH_LIB_CAN_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "common.h"  

/* hardware instances */
#define HW_CAN0  (0x00U)  /* CAN0模块，以下依次类推 */
#define HW_CAN1  (0x01U)    
     
/* RX FIFO */
/* when FIFO is enabled, MB5 interrupt is RXFIFO interrupt */
#define CAN_RX_FIFO_MB      (0x05)
    
/* CAN MB Type */
typedef enum
{
    kFlexCanTX_Inactive  = 0x08, 	/*!< MB is not active.*/
    kFlexCanTX_Abort     = 0x09, 	/*!< MB is aborted.*/
    kFlexCanTX_Data      = 0x0C, 	/*!< MB is a TX Data Frame(MB RTR must be 0).*/
    kFlexCanTX_Remote    = 0x1C, 	/*!< MB is a TX Remote Request Frame (MB RTR must be 1).*/
    kFlexCanTX_Tanswer   = 0x0E, 	/*!< MB is a TX Response Request Frame from.*/
																	/*!  an incoming Remote Request Frame.*/
    kFlexCanTX_NotUsed   = 0xF,  	/*!< Not used*/
    kFlexCanRX_Inactive  = 0x0, 	/*!< MB is not active.*/
    kFlexCanRX_Full      = 0x2, 	/*!< MB is full.*/
    kFlexCanRX_Empty     = 0x4, 	/*!< MB is active and empty.*/
    kFlexCanRX_Overrun   = 0x6, 	/*!< MB is overwritten into a full buffer.*/
    //kFlexCanRX_Busy      = 0x8, /*!< FlexCAN is updating the contents of the MB.*/
																	/*!  The CPU must not access the MB.*/
    kFlexCanRX_Ranswer   = 0xA, 	/*!< A frame was configured to recognize a Remote Request Frame*/
																	/*!  and transmit a Response Frame in return.*/
    kFlexCanRX_NotUsed   = 0xF, 	/*!< Not used*/
}CAN_MBCode_Type;



/**
 * \struct CAN_InitTypeDef
 * \brief CAN 初始化结构
 */
typedef struct
{
    uint32_t                instance;  ///< CAN通信端口 HW_CAN0/HW_CAN1
    uint32_t       baudrate;  ///< CAN通信速度设置
}CAN_InitTypeDef;


/* CAN QuickInit macro */
#define CAN1_TX_PE24_RX_PE25   (0xB0A1U)  //CAN1模块 发送引脚为PTE24 接收引脚为PTE25
#define CAN0_TX_PA12_RX_PA13   (0x9880U)  //CAN0模块 发送引脚为PTA12 接收引脚为PTA13
#define CAN0_TX_PB18_RX_PB19   (0xA488U)  //CAN0模块 发送引脚为PTB18 接收引脚为PTB19
#define CAN1_TX_PC17_RX_PC16   (0xA091U)  //CAN1模块 发送引脚为PTC17 接收引脚为PTC16

/**
 * \enum CAN_ITDMAConfig_Type
 * \brief CAN 中断DMA配置
 */
typedef enum
{
    kCAN_IT_Tx_Disable,  /**< 关闭发送中断 */
    kCAN_IT_Rx_Disable,  /**< 关闭接收中断 */
    kCAN_IT_Tx,          /**< 开启发送中断 */
    kCAN_IT_RX,          /**< 开启发送中断 */
}CAN_ITDMAConfig_Type;

/* CAN 回调函数声明 */
typedef void (*CAN_CallBackType)(void);

/* API functions */
void CAN_SetRxFilterMask(uint32_t instance, uint32_t mb, uint32_t mask);
uint32_t CAN_WriteData(uint32_t instance, uint32_t mb, uint32_t id, uint8_t* buf, uint8_t len);
uint32_t CAN_WriteRemote(uint32_t instance, uint32_t mb, uint32_t id, uint8_t len);
uint32_t CAN_ReadData(uint32_t instance, uint32_t mb, uint32_t *id, uint8_t *buf, uint8_t *len);
uint32_t CAN_QuickInit(uint32_t CANxMAP, uint32_t baudrate);
void CAN_ITDMAConfig(uint32_t instance, uint32_t mb, CAN_ITDMAConfig_Type config);
void CAN_SetRxMB(uint32_t instance, uint32_t mb, uint32_t id);
void CAN_CallbackInstall(uint32_t instance, CAN_CallBackType AppCBFun);
void CAN_Init(CAN_InitTypeDef* CAN_InitStruct);

void CAN_SetRxFIFO(uint32_t instance);
bool CAN_IsRxFIFOEnable(uint32_t instance);
uint32_t CAN_ReadFIFO(uint32_t instance, uint32_t *id, uint8_t *buf, uint8_t *len);

#ifdef __cplusplus
}
#endif

#endif
