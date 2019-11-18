/*!
    \file  gd32f1x0_dma.c
    \brief DMA driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2014-12-26, V1.0.0, platform GD32F1x0(x=3,5)
    2016-01-15, V2.0.0, platform GD32F1x0(x=3,5,7,9)
    2016-04-30, V3.0.0, firmware update for GD32F1x0(x=3,5,7,9)
    2017-06-19, V3.1.0, firmware update for GD32F1x0(x=3,5,7,9)
*/

#include "gd32f1x0_dma.h"

/*!
    \brief      deinitialize DMA a channel registers 
    \param[in]  channelx: specify which DMA channel is deinitialized
      \arg        DMA_CHx(x=0..6)
    \param[out] none
    \retval     none
*/
void dma_deinit(dma_channel_enum channelx)
{
    /* disable DMA a channel */
    DMA_CHCTL(channelx) &= ~DMA_CHXCTL_CHEN;
    /* reset DMA channel registers */
    DMA_CHCTL(channelx) = DMA_CHCTL_RESET_VALUE;
    DMA_CHCNT(channelx) = DMA_CHCNT_RESET_VALUE;
    DMA_CHPADDR(channelx) = DMA_CHPADDR_RESET_VALUE;
    DMA_CHMADDR(channelx) = DMA_CHMADDR_RESET_VALUE;
    DMA_INTC |= DMA_FLAG_ADD(DMA_CHINTF_RESET_VALUE,channelx);
}

/*!
    \brief      initialize DMA channel 
    \param[in]  channelx: specify which DMA channel is initialized
      \arg        DMA_CHx(x=0..6)
    \param[in]  init_struct: the data needed to initialize DMA channel
                  periph_addr: peripheral base address
                  periph_width: DMA_PERIPHERAL_WIDTH_8BIT, DMA_PERIPHERAL_WIDTH_16BIT, DMA_PERIPHERAL_WIDTH_32BIT
                  periph_inc: DMA_PERIPH_INCREASE_ENABLE, DMA_PERIPH_INCREASE_DISABLE 
                  memory_addr: memory base address
                  memory_width: DMA_MEMORY_WIDTH_8BIT, DMA_MEMORY_WIDTH_16BIT, DMA_MEMORY_WIDTH_32BIT
                  memory_inc: DMA_MEMORY_INCREASE_ENABLE, DMA_MEMORY_INCREASE_DISABLE
                  direction: DMA_PERIPHERAL_TO_MEMORY, DMA_MEMORY_TO_PERIPHERAL
                  number: the number of remaining data to be transferred by the DMA
                  priority: DMA_PRIORITY_LOW, DMA_PRIORITY_MEDIUM, DMA_PRIORITY_HIGH, DMA_PRIORITY_ULTRA_HIGH
    \param[out] none
    \retval     none
*/
void dma_init(dma_channel_enum channelx, dma_parameter_struct init_struct)
{
    uint32_t ctl;
    /* configure peripheral base address */
    DMA_CHPADDR(channelx) = init_struct.periph_addr;
    
    /* configure memory base address */
    DMA_CHMADDR(channelx) = init_struct.memory_addr;
    
    /* configure the number of remaining data to be transferred */
    DMA_CHCNT(channelx) = init_struct.number;
    
    /* configure peripheral transfer width,memory transfer width, */
    ctl = DMA_CHCTL(channelx);
    ctl &= ~(DMA_CHXCTL_PWIDTH | DMA_CHXCTL_MWIDTH | DMA_CHXCTL_PRIO);
    ctl |= (init_struct.periph_width | init_struct.memory_width | init_struct.priority);
    DMA_CHCTL(channelx)=ctl;

    /* configure peripheral increasing mode */
    if(DMA_PERIPH_INCREASE_ENABLE == init_struct.periph_inc){
        DMA_CHCTL(channelx) |= DMA_CHXCTL_PNAGA;
    }else{
        DMA_CHCTL(channelx) &= ~DMA_CHXCTL_PNAGA;
    }

    /* configure memory increasing mode */
    if(DMA_MEMORY_INCREASE_ENABLE == init_struct.memory_inc){
        DMA_CHCTL(channelx) |= DMA_CHXCTL_MNAGA;
    }else{
        DMA_CHCTL(channelx) &= ~DMA_CHXCTL_MNAGA;
    }
    
    /* configure the direction of  data transfer */
    if(DMA_PERIPHERAL_TO_MEMORY == init_struct.direction){
        DMA_CHCTL(channelx) &= ~DMA_CHXCTL_DIR;
    }else{
        DMA_CHCTL(channelx) |= DMA_CHXCTL_DIR;
    } 
}

/*!
    \brief      enable DMA circulation mode  
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[out] none
    \retval     none 
*/
void dma_circulation_enable(dma_channel_enum channelx)
{
    DMA_CHCTL(channelx) |= DMA_CHXCTL_CMEN;
}

/*!
    \brief      disable DMA circulation mode  
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[out] none
    \retval     none 
*/
void dma_circulation_disable(dma_channel_enum channelx)
{
    DMA_CHCTL(channelx) &= ~DMA_CHXCTL_CMEN;
}

/*!
    \brief      enable memory to memory mode
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[out] none
    \retval     none
*/
void dma_memory_to_memory_enable(dma_channel_enum channelx)
{
    DMA_CHCTL(channelx) |= DMA_CHXCTL_M2M;
}

/*!
    \brief      disable memory to memory mode
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[out] none
    \retval     none
*/
void dma_memory_to_memory_disable(dma_channel_enum channelx)
{
    DMA_CHCTL(channelx) &= ~DMA_CHXCTL_M2M;
}

/*!
    \brief      enable DMA channel 
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[out] none
    \retval     none 
*/
void dma_channel_enable(dma_channel_enum channelx)
{
    DMA_CHCTL(channelx) |= DMA_CHXCTL_CHEN;
}

/*!
    \brief      disable DMA channel 
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[out] none
    \retval     none 
*/
void dma_channel_disable(dma_channel_enum channelx)
{
    DMA_CHCTL(channelx) &= ~DMA_CHXCTL_CHEN;
}

/*!
    \brief      set DMA peripheral base address  
    \param[in]  channelx: specify which DMA channel to set peripheral base address 
      \arg        DMA_CHx(x=0..6)
    \param[in]  address: peripheral base address
    \param[out] none
    \retval     none 
*/
void dma_periph_address_config(dma_channel_enum channelx, uint32_t address)
{
    DMA_CHPADDR(channelx) = address;
}

/*!
    \brief      set DMA Memory base address  
    \param[in]  channelx: specify which DMA channel to set Memory base address 
      \arg        DMA_CHx(x=0..6)
    \param[in]  address: Memory base address
    \param[out] none
    \retval     none 
*/
void dma_memory_address_config(dma_channel_enum channelx, uint32_t address)
{
    DMA_CHMADDR(channelx) = address;
}

/*!
    \brief      set the number of remaining data to be transferred by the DMA  
    \param[in]  channelx: specify which DMA channel to set number 
      \arg        DMA_CHx(x=0..6)
    \param[in]  number: the number of remaining data to be transferred by the DMA
    \param[out] none
    \retval     none 
*/
void dma_transfer_number_config(dma_channel_enum channelx, uint32_t number)
{
    DMA_CHCNT(channelx) = number;
}

/*!
    \brief      get the number of remaining data to be transferred by the DMA  
    \param[in]  channelx: specify which DMA channel to set number 
      \arg        DMA_CHx(x=0..6)
    \param[out] none
    \retval     uint32_t: the number of remaining data to be transferred by the DMA 
*/
uint32_t dma_transfer_number_get(dma_channel_enum channelx)
{
    return (uint32_t)DMA_CHCNT(channelx);
}

/*!
    \brief      configure priority level of DMA channel   
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[in]  priority: priority Level of this channel
      \arg        DMA_PRIORITY_LOW: low priority
      \arg        DMA_PRIORITY_MEDIUM: medium priority
      \arg        DMA_PRIORITY_HIGH: high priority
      \arg        DMA_PRIORITY_ULTRA_HIGH: ultra high priority
    \param[out] none
    \retval     none 
*/
void dma_priority_config(dma_channel_enum channelx, uint32_t priority)
{
    uint32_t ctl;
    /* acquire DMA_CHxCTL register */
    ctl = DMA_CHCTL(channelx);
    /* assign regiser */
    ctl &= ~DMA_CHXCTL_PRIO;
    ctl |= priority;
    DMA_CHCTL(channelx) = ctl;
}

/*!
    \brief      configure transfer data size of memory 
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[in]  msize: transfer data size of memory
      \arg        DMA_MEMORY_WIDTH_8BIT: transfer data size of memory is 8-bit
      \arg        DMA_MEMORY_WIDTH_16BIT: transfer data size of memory is 16-bit
      \arg        DMA_MEMORY_WIDTH_32BIT: transfer data size of memory is 32-bit
    \param[out] none
    \retval     none
*/
void dma_memory_width_config (dma_channel_enum channelx, uint32_t msize)
{
    uint32_t ctl;
    /* acquire DMA_CHxCTL register */
    ctl = DMA_CHCTL(channelx);
    /* assign regiser */
    ctl &= ~DMA_CHXCTL_MWIDTH;
    ctl |= msize;
    DMA_CHCTL(channelx) = ctl;
}

/*!
    \brief      configure transfer data size of peripheral 
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[in]  msize: transfer data size of peripheral
      \arg        DMA_PERIPHERAL_WIDTH_8BIT: transfer data size of peripheral is 8-bit
      \arg        DMA_PERIPHERAL_WIDTH_16BIT: transfer data size of peripheral is 16-bit
      \arg        DMA_PERIPHERAL_WIDTH_32BIT: transfer data size of peripheral is 32-bit
    \param[out] none
    \retval     none
*/
void dma_periph_width_config (dma_channel_enum channelx, uint32_t psize)
{
    uint32_t ctl;
    /* acquire DMA_CHxCTL register */
    ctl = DMA_CHCTL(channelx);
    /* assign regiser */
    ctl &= ~DMA_CHXCTL_PWIDTH;
    ctl |= psize;
    DMA_CHCTL(channelx) = ctl;
}

/*!
    \brief      enable next address increasement algorithm of memory  
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[out] none
    \retval     none
*/
void dma_memory_increase_enable(dma_channel_enum channelx)
{
    DMA_CHCTL(channelx) |= DMA_CHXCTL_MNAGA;
}

/*!
    \brief      disable next address increasement algorithm of memory  
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[out] none
    \retval     none
*/
void dma_memory_increase_disable(dma_channel_enum channelx)
{
    DMA_CHCTL(channelx) &= ~DMA_CHXCTL_MNAGA;
}

/*!
    \brief      enable next address increasement algorithm of peripheral  
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[out] none
    \retval     none
*/
void dma_periph_increase_enable(dma_channel_enum channelx)
{
    DMA_CHCTL(channelx) |= DMA_CHXCTL_PNAGA;
}

/*!
    \brief      disable next address increasement algorithm of peripheral  
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[out] none
    \retval     none
*/
void dma_periph_increase_disable(dma_channel_enum channelx)
{
    DMA_CHCTL(channelx) &= ~DMA_CHXCTL_PNAGA;
}

/*!
    \brief      configure the direction of  data transfer on the channel  
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[in]  direction: specify the direction of  data transfer
      \arg        DMA_PERIPHERAL_TO_MEMORY: read from peripheral and write to memory
      \arg        DMA_MEMORY_TO_PERIPHERAL: read from memory and write to peripheral
    \param[out] none
    \retval     none
*/
void dma_transfer_direction_config(dma_channel_enum channelx, uint8_t direction)
{
    if(DMA_PERIPHERAL_TO_MEMORY == direction){
        DMA_CHCTL(channelx) &= ~DMA_CHXCTL_DIR;
    } else {
        DMA_CHCTL(channelx) |= DMA_CHXCTL_DIR;
    }
}

/*!
    \brief      check DMA flag and interrupt enable bit is set or not 
    \param[in]  channelx: specify which DMA channel to get flag
      \arg        DMA_CHx(x=0..6)
    \param[in]  flag: specify get which flag
                only one parameter can be selected which is shown as below:
      \arg        DMA_INT_FLAG_G: global interrupt flag of channel
      \arg        DMA_INT_FLAG_FTF: full transfer finish interrupt flag of channel
      \arg        DMA_INT_FLAG_HTF: half transfer finish interrupt flag of channel
      \arg        DMA_INT_FLAG_ERR: error interrupt flag of channel
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus dma_interrupt_flag_get(dma_channel_enum channelx, uint32_t flag)
{
    uint32_t interrupt_enable = 0U, interrupt_flag = 0U;
    uint32_t gif_check = 0x0FU, gif_enable = 0x0EU;
    
    switch(flag){
        case DMA_INT_FLAG_FTF:
            interrupt_flag = DMA_INTF & DMA_FLAG_ADD(flag, channelx);
            interrupt_flag = interrupt_flag >> ((channelx) * 4U);
            interrupt_enable = DMA_CHCTL(channelx) & DMA_CHXCTL_FTFIE;
            break;
        case DMA_INT_FLAG_HTF:
            interrupt_flag = DMA_INTF & DMA_FLAG_ADD(flag, channelx);
            interrupt_flag = interrupt_flag >> ((channelx) * 4U);
            interrupt_enable = DMA_CHCTL(channelx) & DMA_CHXCTL_HTFIE;
            break;
        case DMA_INT_FLAG_ERR:
            interrupt_flag = DMA_INTF & DMA_FLAG_ADD(flag, channelx);
            interrupt_flag = interrupt_flag >> ((channelx) * 4U);
            interrupt_enable = DMA_CHCTL( channelx) & DMA_CHXCTL_ERRIE;
            break;
        case DMA_INT_FLAG_G:
            interrupt_flag = DMA_INTF & DMA_FLAG_ADD(gif_check, channelx);
            interrupt_flag = interrupt_flag >> ((channelx) * 4U);
            interrupt_enable = DMA_CHCTL(channelx) & gif_enable;
             break;    
        default:
            break;
    }
    
    if(interrupt_flag & interrupt_enable){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear DMA a channel flag
    \param[in]  channelx: specify which DMA channel to clear flag
      \arg        DMA_CHx(x=0..6)
    \param[in]  flag: specify get which flag
                only one parameter can be selected which is shown as below:
      \arg        DMA_INT_FLAG_G: global interrupt flag of channel
      \arg        DMA_INT_FLAG_FTF: full transfer finish interrupt flag of channel
      \arg        DMA_INT_FLAG_HTF: half transfer finish interrupt flag of channel
      \arg        DMA_INT_FLAG_ERR: error interrupt flag of channel
    \param[out] none
    \retval     none
*/
void dma_interrupt_flag_clear(dma_channel_enum channelx, uint32_t flag)
{
    DMA_INTC |= DMA_FLAG_ADD(flag, channelx);
}

/*!
    \brief      enable DMA interrupt
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[in]  source: specify which interrupt to enbale
      \arg        DMA_INT_ERR: channel error interrupt
      \arg        DMA_INT_HTF: channel transfer half complete interrupt
      \arg        DMA_INT_FTF: channel transfer complete interrupt
    \param[out] none
    \retval     none
*/
void dma_interrupt_enable(dma_channel_enum channelx, uint32_t source)
{
    DMA_CHCTL(channelx) |= source;
}

/*!
    \brief      disable DMA interrupt
    \param[in]  channelx: specify which DMA channel 
      \arg        DMA_CHx(x=0..6)
    \param[in]  source: specify which interrupt to disbale
      \arg        DMA_INT_ERR: channel error interrupt
      \arg        DMA_INT_HTF: channel transfer half complete interrupt
      \arg        DMA_INT_FTF: for channel transfer complete interrupt
    \param[out] none
    \retval     none
*/
void dma_interrupt_disable(dma_channel_enum channelx, uint32_t source)
{
    DMA_CHCTL(channelx) &= ~source;
}

/*!
    \brief      check DMA flag is set or not 
    \param[in]  channelx: specify which DMA channel to get flag
      \arg        DMA_CHx(x=0..6)
    \param[in]  flag: specify get which flag
                only one parameter can be selected which is shown as below:
      \arg        DMA_FLAG_G: global interrupt flag of channel
      \arg        DMA_FLAG_FTF: full transfer finish flag of channel
      \arg        DMA_FLAG_HTF: half transfer finish flag of channel
      \arg        DMA_FLAG_ERR: error flag of channel
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus dma_flag_get(dma_channel_enum channelx, uint32_t flag)
{
    FlagStatus reval;

    if(RESET != (DMA_INTF & DMA_FLAG_ADD(flag, channelx))){
        reval = SET;
    }else{
        reval = RESET;
    }
    
    return reval;
}

/*!
    \brief      clear DMA a channel flag
    \param[in]  channelx: specify which DMA channel to clear flag
      \arg        DMA_CHx(x=0..6)
    \param[in]  flag: specify get which flag
                only one parameter can be selected which is shown as below:
      \arg        DMA_FLAG_G: global interrupt flag of channel
      \arg        DMA_FLAG_FTF: full transfer finish flag of channel
      \arg        DMA_FLAG_HTF: half transfer finish flag of channel
      \arg        DMA_FLAG_ERR: error flag of channel
    \param[out] none
    \retval     none
*/
void dma_flag_clear(dma_channel_enum channelx, uint32_t flag)
{
    DMA_INTC |= DMA_FLAG_ADD(flag, channelx);
}
