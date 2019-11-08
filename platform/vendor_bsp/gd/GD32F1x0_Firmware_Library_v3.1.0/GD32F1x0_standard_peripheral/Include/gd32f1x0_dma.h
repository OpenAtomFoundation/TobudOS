/*!
    \file  gd32f1x0_dma.h
    \brief definitions for the DMA
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#ifndef GD32F1X0_DMA_H
#define GD32F1X0_DMA_H

#include "gd32f1x0.h"

/* DMA definitions */
#define DMA                               DMA_BASE               /*!< DMA base address */

/* registers definitions */
#define DMA_INTF                          REG32(DMA + 0x00U)     /*!< DMA interrupt flag register */
#define DMA_INTC                          REG32(DMA + 0x04U)     /*!< DMA interrupt flag clear register */

#define DMA_CH0CTL                        REG32(DMA + 0x08U)     /*!< DMA channel 0 control register */
#define DMA_CH0CNT                        REG32(DMA + 0x0CU)     /*!< DMA channel 0 counter register */
#define DMA_CH0PADDR                      REG32(DMA + 0x10U)     /*!< DMA channel 0 peripheral base address register */
#define DMA_CH0MADDR                      REG32(DMA + 0x14U)     /*!< DMA channel 0 memory base address register */

#define DMA_CH1CTL                        REG32(DMA + 0x1CU)     /*!< DMA channel 1 control register */
#define DMA_CH1CNT                        REG32(DMA + 0x20U)     /*!< DMA channel 1 counter register */
#define DMA_CH1PADDR                      REG32(DMA + 0x24U)     /*!< DMA channel 1 peripheral base address register */
#define DMA_CH1MADDR                      REG32(DMA + 0x28U)     /*!< DMA channel 1 memory base address register */

#define DMA_CH2CTL                        REG32(DMA + 0x30U)     /*!< DMA channel 2 control register */
#define DMA_CH2CNT                        REG32(DMA + 0x34U)     /*!< DMA channel 2 counter register */
#define DMA_CH2PADDR                      REG32(DMA + 0x38U)     /*!< DMA channel 2 peripheral base address register */
#define DMA_CH2MADDR                      REG32(DMA + 0x3CU)     /*!< DMA channel 2 memory base address register */

#define DMA_CH3CTL                        REG32(DMA + 0x44U)     /*!< DMA channel 3 control register */
#define DMA_CH3CNT                        REG32(DMA + 0x48U)     /*!< DMA channel 3 counter register */
#define DMA_CH3PADDR                      REG32(DMA + 0x4CU)     /*!< DMA channel 3 peripheral base address register */
#define DMA_CH3MADDR                      REG32(DMA + 0x50U)     /*!< DMA channel 3 memory base address register */

#define DMA_CH4CTL                        REG32(DMA + 0x58U)     /*!< DMA channel 4 control register */
#define DMA_CH4CNT                        REG32(DMA + 0x5CU)     /*!< DMA channel 4 counter register */
#define DMA_CH4PADDR                      REG32(DMA + 0x60U)     /*!< DMA channel 4 peripheral base address register */
#define DMA_CH4MADDR                      REG32(DMA + 0x64U)     /*!< DMA channel 4 memory base address register */

#define DMA_CH5CTL                        REG32(DMA + 0x6CU)     /*!< DMA channel 5 control register */
#define DMA_CH5CNT                        REG32(DMA + 0x70U)     /*!< DMA channel 5 counter register */
#define DMA_CH5PADDR                      REG32(DMA + 0x74U)     /*!< DMA channel 5 peripheral base address register */
#define DMA_CH5MADDR                      REG32(DMA + 0x78U)     /*!< DMA channel 5 memory base address register */

#define DMA_CH6CTL                        REG32(DMA + 0x80U)     /*!< DMA channel 6 control register */
#define DMA_CH6CNT                        REG32(DMA + 0x84U)     /*!< DMA channel 6 counter register */
#define DMA_CH6PADDR                      REG32(DMA + 0x88U)     /*!< DMA channel 6 peripheral base address register */
#define DMA_CH6MADDR                      REG32(DMA + 0x8CU)     /*!< DMA channel 6 memory base address register */

/* bits definitions */
/* DMA_INTF */
#define DMA_INTF_GIF                      BIT(0)                 /*!< global interrupt flag of channel */
#define DMA_INTF_FTFIF                    BIT(1)                 /*!< transfer complete flag of channel */
#define DMA_INTF_HTFIF                    BIT(2)                 /*!< half transfer complete flag of channel */
#define DMA_INTF_ERRIF                    BIT(3)                 /*!< error flag of channel */

/* DMA_INTC */
#define DMA_INTFC_GIFC                    BIT(0)                 /*!< clear global interrupt flag of channel */
#define DMA_INTFC_FTFIFC                  BIT(1)                 /*!< clear transfer complete flag of channel */
#define DMA_INTFC_HTFIFC                  BIT(2)                 /*!< clear half transfer complete flag of channel */
#define DMA_INTFC_ERRIFC                  BIT(3)                 /*!< clear error flag of channel */

/* DMA_CHxCTL,x=0..6 */
#define DMA_CHXCTL_CHEN                   BIT(0)                 /*!< channel x enable */
#define DMA_CHXCTL_FTFIE                  BIT(1)                 /*!< enable bit for channel x transfer complete interrupt */
#define DMA_CHXCTL_HTFIE                  BIT(2)                 /*!< enable bit for channel x transfer half complete interrupt */
#define DMA_CHXCTL_ERRIE                  BIT(3)                 /*!< enable bit for channel x error interrupt */
#define DMA_CHXCTL_DIR                    BIT(4)                 /*!< transfer direction */
#define DMA_CHXCTL_CMEN                   BIT(5)                 /*!< circulation mode */
#define DMA_CHXCTL_PNAGA                  BIT(6)                 /*!< next address generation algorithm of peripheral */
#define DMA_CHXCTL_MNAGA                  BIT(7)                 /*!< next address generation algorithm of memory */
#define DMA_CHXCTL_PWIDTH                 BITS(8,9)              /*!< transfer data size of peripheral */
#define DMA_CHXCTL_MWIDTH                 BITS(10,11)            /*!< transfer data size of memory */
#define DMA_CHXCTL_PRIO                   BITS(12,13)            /*!< priority level of channelx */
#define DMA_CHXCTL_M2M                    BIT(14)                /*!< memory to memory mode */

/* DMA_CHxCNT, x=0..6 */
#define DMA_CHXCNT_CNT                    BITS(0,15)             /*!< transfer counter */

/* DMA_CHxPADDR, x=0..6 */
#define DMA_CHXPADDR_PADDR                BITS(0,31)             /*!< peripheral base address */

/* DMA_CHxMADDR, x=0..6 */
#define DMA_CHXMADDR_MADDR                BITS(0,31)             /*!< memory base address */

/* constants definitions */
/* DMA channel select */
typedef enum 
{
    DMA_CH0 = 0,            /*!< DMA Channel0 */
    DMA_CH1,                /*!< DMA Channel1 */ 
    DMA_CH2,                /*!< DMA Channel2 */ 
    DMA_CH3,                /*!< DMA Channel3 */ 
    DMA_CH4,                /*!< DMA Channel4 */ 
    DMA_CH5,                /*!< DMA Channel5 */ 
    DMA_CH6                 /*!< DMA Channel6 */
} dma_channel_enum;

/* DMA initialize struct */
typedef struct
{
    uint32_t periph_addr;   /*!< peripheral base address */
    uint32_t periph_width;  /*!< transfer data size of peripheral */
    uint32_t periph_inc;    /*!< peripheral increasing mode */
    uint32_t memory_addr;   /*!< memory base address */
    uint32_t memory_width;  /*!< transfer data size of memory */
    uint32_t memory_inc;    /*!< memory increasing mode */
    uint32_t direction;     /*!< channel data transfer direction */
    uint32_t number;        /*!< channel transfer number */
    uint32_t priority;      /*!< channel priority level */
} dma_parameter_struct;

/* flag bits */
#define DMA_FLAG_G                        DMA_INTF_GIF           /*!< global interrupt flag of channel */
#define DMA_FLAG_FTF                      DMA_INTF_FTFIF         /*!< full transfer finish flag of channel */
#define DMA_FLAG_HTF                      DMA_INTF_HTFIF         /*!< half transfer finish flag of channel */
#define DMA_FLAG_ERR                      DMA_INTF_ERRIF         /*!< error flag of channel */

/* interrupt flag bits */
#define DMA_INT_FLAG_G                    DMA_INTF_GIF           /*!< global interrupt flag of channel */
#define DMA_INT_FLAG_FTF                  DMA_INTF_FTFIF         /*!< full transfer finish interrupt flag of channel */
#define DMA_INT_FLAG_HTF                  DMA_INTF_HTFIF         /*!< half transfer finish interrupt flag of channel */
#define DMA_INT_FLAG_ERR                  DMA_INTF_ERRIF         /*!< error interrupt flag of channel */

/* interrupt enable bits */
#define DMA_INT_FTF                       DMA_CHXCTL_FTFIE       /*!< enable bit for channel full transfer finish interrupt */
#define DMA_INT_HTF                       DMA_CHXCTL_HTFIE       /*!< enable bit for channel half transfer finish interrupt */
#define DMA_INT_ERR                       DMA_CHXCTL_ERRIE       /*!< enable bit for channel error interrupt */

/* DMA_CHCTL base address */
#define DMA_CHXCTL_BASE                   (DMA + 0x08U)          /*!< the base address of DMA channel CHXCTL register */
#define DMA_CHXCNT_BASE                   (DMA + 0x0CU)          /*!< the base address of DMA channel CHXCNT register */
#define DMA_CHXPADDR_BASE                 (DMA + 0x10U)          /*!< the base address of DMA channel CHXPADDR register */
#define DMA_CHXMADDR_BASE                 (DMA + 0x14U)          /*!< the base address of DMA channel CHXMADDR register */
#define DMA_FLAG_ADD(flag,shift)          ((uint32_t)(flag)<<((uint32_t)(shift)*4U))  /*!< DMA channel flag shift */

/* DMA channel shift bit */
#define DMA_CHCTL(channel)                REG32(DMA_CHXCTL_BASE + 0x14U*(channel))    /*!< the address of DMA channel CHXCTL register  */
#define DMA_CHCNT(channel)                REG32(DMA_CHXCNT_BASE + 0x14U*(channel))    /*!< the address of DMA channel CHXCNT register */
#define DMA_CHPADDR(channel)              REG32(DMA_CHXPADDR_BASE + 0x14U*(channel))  /*!< the address of DMA channel CHXPADDR register */
#define DMA_CHMADDR(channel)              REG32(DMA_CHXMADDR_BASE + 0x14U*(channel))  /*!< the address of DMA channel CHXMADDR register */

/* channel priority level */
#define CHCTL_PRIO(regval)                (BITS(12,13) & ((regval) << 12U))           /*!< DMA channel priority level */
#define DMA_PRIORITY_LOW                  CHCTL_PRIO(0)                               /*!< low priority */
#define DMA_PRIORITY_MEDIUM               CHCTL_PRIO(1)                               /*!< medium priority */
#define DMA_PRIORITY_HIGH                 CHCTL_PRIO(2)                               /*!< high priority */
#define DMA_PRIORITY_ULTRA_HIGH           CHCTL_PRIO(3)                               /*!< ultra high priority */

/* transfer data size of memory */
#define CHCTL_MSIZE(regval)               (BITS(10,11) & ((regval) << 10U))           /*!< transfer data size of memory */
#define DMA_MEMORY_WIDTH_8BIT             CHCTL_MSIZE(0)                              /*!< transfer data size of memory is 8-bit */
#define DMA_MEMORY_WIDTH_16BIT            CHCTL_MSIZE(1)                              /*!< transfer data size of memory is 16-bit */
#define DMA_MEMORY_WIDTH_32BIT            CHCTL_MSIZE(2)                              /*!< transfer data size of memory is 32-bit */

/* transfer data size of peripheral */
#define CHCTL_PSIZE(regval)               (BITS(8,9) & ((regval) << 8U))              /*!< transfer data size of peripheral */
#define DMA_PERIPHERAL_WIDTH_8BIT         CHCTL_PSIZE(0)                              /*!< transfer data size of peripheral is 8-bit */
#define DMA_PERIPHERAL_WIDTH_16BIT        CHCTL_PSIZE(1)                              /*!< transfer data size of peripheral is 16-bit */
#define DMA_PERIPHERAL_WIDTH_32BIT        CHCTL_PSIZE(2)                              /*!< transfer data size of peripheral is 32-bit */

/* channel data transfer direction */
#define DMA_PERIPHERAL_TO_MEMORY          ((uint32_t)0x00000000U)                     /*!< read from peripheral and write to memory */
#define DMA_MEMORY_TO_PERIPHERAL          ((uint32_t)0x00000001U)                     /*!< read from memory and write to peripheral */

/* peripheral increasing mode */
#define DMA_PERIPH_INCREASE_ENABLE        ((uint32_t)0x00000000U)                     /*!< next address of peripheral is increasing address mode */
#define DMA_PERIPH_INCREASE_DISABLE       ((uint32_t)0x00000001U)                     /*!< next address of peripheral is fixed address mode */

/* memory increasing mode */
#define DMA_MEMORY_INCREASE_ENABLE        ((uint32_t)0x00000000U)                     /*!< next address of memory is increasing address mode */
#define DMA_MEMORY_INCREASE_DISABLE       ((uint32_t)0x00000001U)                     /*!< next address of memory is fixed address mode */

/* DMA reset value */
#define DMA_CHCTL_RESET_VALUE             ((uint32_t)0x00000000U)                     /*!< the reset value of DMA channel CHXCTL register  */
#define DMA_CHCNT_RESET_VALUE             ((uint32_t)0x00000000U)                     /*!< the reset value of DMA channel CHXCNT register  */
#define DMA_CHPADDR_RESET_VALUE           ((uint32_t)0x00000000U)                     /*!< the reset value of DMA channel CHXPADDR register  */
#define DMA_CHMADDR_RESET_VALUE           ((uint32_t)0x00000000U)                     /*!< the reset value of DMA channel CHXMADDR register  */
#define DMA_CHINTF_RESET_VALUE            ((uint32_t)0x0000000FU)                     /*!< clear DMA channel CHXINTFS register  */

/* function declarations */
/* deinitialize DMA a channel registers */
void dma_deinit(dma_channel_enum channelx);
/* initialize DMA channel */
void dma_init(dma_channel_enum channelx, dma_parameter_struct init_struct);
/* enable DMA circulation mode */
void dma_circulation_enable(dma_channel_enum channelx);
/* disable DMA circulation mode */
void dma_circulation_disable(dma_channel_enum channelx);
/* enable memory to memory mode */
void dma_memory_to_memory_enable(dma_channel_enum channelx);
/* disable memory to memory mode */
void dma_memory_to_memory_disable(dma_channel_enum channelx);
/* enable DMA channel */
void dma_channel_enable(dma_channel_enum channelx);
/* disable DMA channel */
void dma_channel_disable(dma_channel_enum channelx);

/* set DMA peripheral base address */
void dma_periph_address_config(dma_channel_enum channelx, uint32_t address);
/* set DMA Memory base address */
void dma_memory_address_config(dma_channel_enum channelx, uint32_t address);
/* set the number of remaining data to be transferred by the DMA */
void dma_transfer_number_config(dma_channel_enum channelx, uint32_t number);
/* get the number of remaining data to be transferred by the DMA */
uint32_t dma_transfer_number_get(dma_channel_enum channelx);
/* configure priority level of DMA channel */
void dma_priority_config(dma_channel_enum channelx, uint32_t priority);
/* configure transfer data size of memory */
void dma_memory_width_config (dma_channel_enum channelx, uint32_t msize);
/* configure transfer data size of peripheral */
void dma_periph_width_config (dma_channel_enum channelx, uint32_t psize);
/* enable next address increasement algorithm of memory */
void dma_memory_increase_enable(dma_channel_enum channelx);
/* disable next address increasement algorithm of memory */
void dma_memory_increase_disable(dma_channel_enum channelx);
/* enable next address increasement algorithm of peripheral */
void dma_periph_increase_enable(dma_channel_enum channelx);
/* disable next address increasement algorithm of peripheral */
void dma_periph_increase_disable(dma_channel_enum channelx);
/* configure the direction of data transfer on the channel */
void dma_transfer_direction_config(dma_channel_enum channelx, uint8_t direction);

/* check DMA flag is set or not */
FlagStatus dma_interrupt_flag_get(dma_channel_enum channelx, uint32_t flag);
/* clear DMA a channel flag */
void dma_interrupt_flag_clear(dma_channel_enum channelx, uint32_t flag);
/* enable DMA interrupt */
void dma_interrupt_enable(dma_channel_enum channelx, uint32_t source);
/* disable DMA interrupt */
void dma_interrupt_disable(dma_channel_enum channelx,uint32_t source);
/* check DMA flag is set or not */
FlagStatus dma_flag_get(dma_channel_enum channelx, uint32_t flag);
/* clear DMA a channel flag */
void dma_flag_clear(dma_channel_enum channelx, uint32_t flag);

#endif /* GD32F1X0_DMA_H */
