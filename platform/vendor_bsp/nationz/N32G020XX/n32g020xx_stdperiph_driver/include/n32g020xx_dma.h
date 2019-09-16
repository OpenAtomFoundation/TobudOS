/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file n32g020xx_dma.h
 * @author Nations Solution Team
 * @version v1.0.0
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @brief    DMA控制模块接口声明文件
 * @defgroup DMA DMA控制模块
 * @{
 */

#ifndef __N32G020XX_DMA_H__
#define __N32G020XX_DMA_H__

#include <stdint.h>

/** @brief  RAM起始地址 */
#define DMA_RAM_START_ADDR          0x20000000

/** @brief  RAM容量 */
#define DMA_RAM_SIZE                0x5400

/** @brief  RAM结束地址 */
#define DMA_RAM_END_ADDR            (DMA_RAM_START_ADDR+DMA_RAM_SIZE-1)

/** @brief  配置使能DMA配置参数验证 */
#define DMA_CONFIG_SANITY           1

/** @brief  DMA RAM清除中断完成位 */
#define DMA_RAMCLR_INIT_DONE                ((uint32_t)1<<0)		
/** @brief  DMA RAM清除初始化开始地址配置异常 */
#define DMA_RAMCLR_STARTADDR_ERROR		    ((uint32_t)1<<1)	
/** @brief  DMA RAM清除初始化结束地址配置异常 */
#define DMA_RAMCLR_ENDADDR_ERROR		    ((uint32_t)1<<2)	
/** @brief  DMA RAM清除初始化开始/结束地址反向异常 */
#define DMA_RAMCLR_ADDR_REVERSED		    ((uint32_t)1<<3)	

/** @brief  DMA 0-7通道传输完成中断位 */
#define DMA_INT_TC_S                0
#define DMA_INT_TC_CHALL            ((uint32_t)(0xff)<<DMA_INT_TC_S)

/** @brief  DMA 0-7通道总线错误位 */
#define DMA_INT_BE_S                8
#define DMA_INT_BE_CHALL            ((uint32_t)(0xff) << DMA_INT_BE_S)

/** @brief  DMA 0-7通道非法访问错误位(已不再使用) */
#define DMA_INT_AE_S                16
#define DMA_INT_AE_CHALL            ((uint32_t)(0xff) << DMA_INT_AE_S)

/** @brief  DMA 0-7通道块传输一半结束位 */
#define DMA_INT_HF_S                24
#define DMA_INT_HF_CHALL            ((uint32_t)(0xff) << DMA_INT_HF_S)

/** @brief  DMA无效通道 */
#define DMA_INVALID_CHANNEL    0xFF


/** @defgroup DMA_PRIORITY  DMA 通道传输优先级 
 * @{
*/
#define DMA_CHCTRL_PRIORITY_S       (30)
#define DMA_CHCTRL_PRIORITY_MASK    (0x03<<DMA_CHCTRL_PRIORITY_S)
#define DMA_CHCTRL_PRIORITY_0       (0<<DMA_CHCTRL_PRIORITY_S)
#define DMA_CHCTRL_PRIORITY_1       (1<<DMA_CHCTRL_PRIORITY_S)
#define DMA_CHCTRL_PRIORITY_2       (2<<DMA_CHCTRL_PRIORITY_S)
#define DMA_CHCTRL_PRIORITY_3       (3<<DMA_CHCTRL_PRIORITY_S)

/** @} */

/** @defgroup DMA_WIDTH  DMA 通道传输数据位宽
 * @{
*/
#define DMA_CHCTRL_WIDTH_S          (28)
#define DMA_CHCTRL_WIDTH_MASK       (0x03<<DMA_CHCTRL_WIDTH_S)
#define DMA_CHCTRL_WIDTH_8BIT       (0<<DMA_CHCTRL_WIDTH_S)
#define DMA_CHCTRL_WIDTH_16BIT      (1<<DMA_CHCTRL_WIDTH_S)
#define DMA_CHCTRL_WIDTH_32BIT      (2<<DMA_CHCTRL_WIDTH_S)
/** @} */

/** @defgroup DMA_TRANSFER_SIZE  DMA 通道传输数据长度
 * @{
*/
#define DMA_CHCTRL_TRANSFER_SIZE_S          (16)
#define DMA_CHCTRL_TRANSFER_SIZE_MASK       (0xFFF<<DMA_CHCTRL_TRANSFER_SIZE_S)
#define DMA_CHCTRL_TRANSFER_SIZE(size)      (((size)&0xFFF)<<DMA_CHCTRL_TRANSFER_SIZE_S)
/** @} */

/** @defgroup DMA_FLOW  DMA 通道传输流控
 * @{
*/
#define DMA_CHCTRL_FLOW_S           (14)
#define DMA_CHCTRL_FLOW_MASK        (0x03<<DMA_CHCTRL_FLOW_S)
#define DMA_CHCTRL_FLOW_M2M         (0<<DMA_CHCTRL_FLOW_S)
#define DMA_CHCTRL_FLOW_M2P         (1<<DMA_CHCTRL_FLOW_S)
#define DMA_CHCTRL_FLOW_P2M         (2<<DMA_CHCTRL_FLOW_S)
/** @} */

/** @defgroup DMA_DST  DMA 通道传输目的外设
 * @{
*/
#define DMA_CHCTRL_DSTPER_S         (9)
#define DMA_CHCTRL_DSTPER_MASK      (0x1F<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_I2C0      (0x00<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_SCD       (0x01<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_SCC       (0x02<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_I2C1      (0x04<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_DAC       (0x05<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_PWM7      (0x07<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_PWM6      (0x08<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_PWM5      (0x09<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_PWM4      (0x0A<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_PWM3      (0x0B<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_PWM2      (0x0C<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_PWM1      (0x0D<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_PWM0      (0x0E<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_UART2TX   (0x10<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_UART1TX   (0x12<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_UART0TX   (0x14<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_SPIS1     (0x15<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_SPIS0     (0x16<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_SPIM1MOSI (0x18<<DMA_CHCTRL_DSTPER_S)
#define DMA_CHCTRL_DSTPER_SPIM0MOSI (0x1A<<DMA_CHCTRL_DSTPER_S)
/** @} */

/** @defgroup DMA_SRC  DMA 通道传输源外设
 * @{
*/
#define DMA_CHCTRL_SRCPER_S         (4)
#define DMA_CHCTRL_SRCPER_MASK      (0x1F<<DMA_CHCTRL_SRCPER_S)
#define DMA_CHCTRL_SRCPER_I2C0      (0x00<<DMA_CHCTRL_SRCPER_S)
#define DMA_CHCTRL_SRCPER_SCD       (0x01<<DMA_CHCTRL_SRCPER_S)
#define DMA_CHCTRL_SRCPER_SCC       (0x02<<DMA_CHCTRL_SRCPER_S)
#define DMA_CHCTRL_SRCPER_I2C1      (0x04<<DMA_CHCTRL_SRCPER_S)
#define DMA_CHCTRL_SRCPER_ADC       (0x06<<DMA_CHCTRL_SRCPER_S)
#define DMA_CHCTRL_SRCPER_UART2RX   (0x0F<<DMA_CHCTRL_SRCPER_S)
#define DMA_CHCTRL_SRCPER_UART1RX   (0x11<<DMA_CHCTRL_SRCPER_S)
#define DMA_CHCTRL_SRCPER_UART0RX   (0x13<<DMA_CHCTRL_SRCPER_S)
#define DMA_CHCTRL_SRCPER_SPIS1     (0x15<<DMA_CHCTRL_SRCPER_S)
#define DMA_CHCTRL_SRCPER_SPIS0     (0x16<<DMA_CHCTRL_SRCPER_S)
#define DMA_CHCTRL_SRCPER_SPIM1MISO (0x17<<DMA_CHCTRL_SRCPER_S)
#define DMA_CHCTRL_SRCPER_SPIM0MISO (0x19<<DMA_CHCTRL_SRCPER_S)
/** @} */

#define DMA_CHCTRL_PERIPHERAL_NUM   0x1B

/** @defgroup DMA_CRIC DMA 通道传输循环搬移模式
 * @{
*/
#define DMA_CHCTRL_CRIC_S           (3)
#define DMA_CHCTRL_CRIC_MASK        (0x01<<DMA_CHCTRL_CRIC_S)
#define DMA_CHCTRL_CRIC             (0x01<<DMA_CHCTRL_CRIC_S)
/** @} */

/** @@defgroup DMA_INC  DMA 通道传输递增指示
 * @{
*/
#define DMA_CHCTRL_INC_S             (1)
#define DMA_CHCTRL_INC_MASK          (0x03<<DMA_CHCTRL_INC_S)
#define DMA_CHCTRL_INC_SRC           (0x01<<DMA_CHCTRL_INC_S)
#define DMA_CHCTRL_INC_DST           (0x02<<DMA_CHCTRL_INC_S)
#define DMA_CHCTRL_INC_SRCDST        (0x03<<DMA_CHCTRL_INC_S)
/** @} */

/** @brief 返回值定义 */
typedef enum DmaRetCode
{
    DMA_SUCCESS         = 0,    ///< @brief  成功
    DMA_PARAM_INCORRECT = -1,   ///< @brief  参数错误
    DMA_BUSY            = 1,    ///< @brief  通道忙
    DMA_IDLE            = 2,    ///< @brief  通道空闲
} DmaRetCode;

typedef void(* DMAIntCallback)(void);

/** @brief  DMA配置结构体 */
typedef struct _DMAConfig
{
    uint32_t addr_src;  ///<  源地址
    uint32_t addr_dst;  ///<  目的地址
    uint32_t chctrl;    ///<  通道控制参数
    uint32_t irqmask;   ///中断使能配置
    DMAIntCallback cmpltcb; ///< DMA完成回调
    DMAIntCallback hfcb;    ///< DMA块传输一半回调
}DMAConfig;


/**
 * @brief DMA初始化
 * @param[in] pDmaCfg DMA配置参数指针
 * @return
 * - @ref DMA_SUCCESS ： 表示成功
 */
int32_t DMA_Init(DMAConfig * pDmaCfg);

/**
 * @brief DMA反初始化，关闭DMA模块时钟
 * 
 * @return
 * - @ref DMA_SUCCESS ： 表示成功 
 */
int32_t DMA_DeInit(void);

/**
 * @brief 获取DMA通道状态
 * 
 * @param[in] ch DMA通道
 * @return
 * - @ref DMA_PARAM_INCORRECT : 通道错误
 * - @ref DMA_IDLE : 通道空闲
 * - @ref DMA_BUSY : 通道忙
 */
int32_t DMA_GetChannelStatus(uint32_t ch);

/**
 * @brief 获取一个空闲DMA通道
 * 
 * @return 通道值
 * - @ref DMA_INVALID_CHANNEL : 无效通道
 */
int32_t DMA_GetFreeChannel(void);

/**
 * @brief 配置DMA通道
 * 
 * @param[in] ch DMA通道
 * @param[in] config 配置参数结构体
 * @return 
 * - @ref DMA_SUCCESS ： 表示成功
 * - @ref DMA_PARAM_INCORRECT : 表示参数错误
 */
int32_t DMA_SetChannelConfig(uint32_t ch, const DMAConfig *config);

/**
 * @brief 开启DMA传输
 * 
 * @param[in] ch DMA通道
 * @return
 * - @ref DMA_SUCCESS ： 表示成功
 * - @ref DMA_PARAM_INCORRECT : 表示参数错误
 */
int32_t DMA_TransferStart(uint32_t ch);

#endif // __N32G020XX_DMA_H__

/** @} */
