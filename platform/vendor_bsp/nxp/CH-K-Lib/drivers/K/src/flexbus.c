/**
  ******************************************************************************
  * @file    flexbus.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * \date    2015.10.08 FreeXc 完善了对 flexbus 模块的相关注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为芯片FlexBus模块的底层功能函数
  ******************************************************************************
  */
#include "flexbus.h"
#include "common.h"

#if (defined(FB))

/**
 * @brief  初始化FlexBus模块
 * @note   具体的配置应用详见关于FlexBus的使用例程  
 * @param[in]  FLEXBUS_InitStruct 指向FlexBus初始化配置结构体的指针，详见FlexBus.h
 * @retval None
 */
void FLEXBUS_Init(FLEXBUS_InitTypeDef* FLEXBUS_InitStruct)
{
    /* enable clock gate enable seruriy mode */
    SIM->SOPT2 |= SIM_SOPT2_FBSL(3);
    SIM->SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
    /* div */
    SIM->CLKDIV1 &= ~SIM_CLKDIV1_OUTDIV3_MASK;
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV3(FLEXBUS_InitStruct->div);
    /* we must set V_MASK in CS0, because CS0.CSMR.V_MASK act as a global CS */
    FB->CS[0].CSMR |= FB_CSMR_V_MASK; 
    /* clear registers */
    FB->CS[FLEXBUS_InitStruct->CSn].CSCR = 0;
    /* base address */
    FB->CS[FLEXBUS_InitStruct->CSn].CSAR = FLEXBUS_InitStruct->baseAddress;
    /* address space */
    FB->CS[FLEXBUS_InitStruct->CSn].CSMR = FB_CSMR_BAM(FLEXBUS_InitStruct->ADSpaceMask) | FB_CSMR_V_MASK;
    /* port size */
    FB->CS[FLEXBUS_InitStruct->CSn].CSCR &= FB_CSCR_PS_MASK;
    FB->CS[FLEXBUS_InitStruct->CSn].CSCR |= FB_CSCR_PS(FLEXBUS_InitStruct->dataWidth);
    /* AutoAcknogement(AA) Config */
    if(FLEXBUS_InitStruct->autoAckMode == kFLEXBUS_AutoAckEnable)
    {
        FB->CS[FLEXBUS_InitStruct->CSn].CSCR |= FB_CSCR_AA_MASK;
    }
    else
    {
        FB->CS[FLEXBUS_InitStruct->CSn].CSCR &= ~FB_CSCR_AA_MASK;
    }
    /* data align */
    if(FLEXBUS_InitStruct->dataAlignMode == kFLEXBUS_DataLeftAligned)
    {
        FB->CS[FLEXBUS_InitStruct->CSn].CSCR &= ~FB_CSCR_BLS_MASK;
    }
    else
    {
        FB->CS[FLEXBUS_InitStruct->CSn].CSCR |= FB_CSCR_BLS_MASK;
    }
    /* byte enable mode */
    if(FLEXBUS_InitStruct->ByteEnableMode == kFLEXBUS_BE_AssertedWrite)
    {
        FB->CS[FLEXBUS_InitStruct->CSn].CSCR &= ~FB_CSCR_BEM_MASK;
    }
    else
    {
        FB->CS[FLEXBUS_InitStruct->CSn].CSCR |= FB_CSCR_BEM_MASK;
    }
    /* assert wait status */
   FB->CS[FLEXBUS_InitStruct->CSn].CSCR &= ~FB_CSCR_WS_MASK;
   FB->CS[FLEXBUS_InitStruct->CSn].CSCR |= FB_CSCR_WS(1);
}

/**
 * \brief 管脚复用配置
 * \param[in] group 组号
 * \param[in] config multiplex control config
 * \see Reference manual --> Chip select port multiplexing control register (FB_CSPMCR)
 */
void FLEXBUS_PortMuxConfig(FLEXBUS_PortMultiplexingSelect_Type group, uint32_t config)
{
    /* CS Port Multiplexing Cotrol */
    switch(group)
    {
        case kFLEXBUS_CSPMCR_Group1:
            FB->CSPMCR &= ~FB_CSPMCR_GROUP1_MASK;
            FB->CSPMCR |= FB_CSPMCR_GROUP1(config);
            break;
        case kFLEXBUS_CSPMCR_Group2:
            FB->CSPMCR &= ~FB_CSPMCR_GROUP2_MASK;
            FB->CSPMCR |= FB_CSPMCR_GROUP2(config);
            break;
        case kFLEXBUS_CSPMCR_Group3:
            FB->CSPMCR &= ~FB_CSPMCR_GROUP3_MASK;
            FB->CSPMCR |= FB_CSPMCR_GROUP3(config);
            break;
        case kFLEXBUS_CSPMCR_Group4:
            FB->CSPMCR &= ~FB_CSPMCR_GROUP4_MASK;
            FB->CSPMCR |= FB_CSPMCR_GROUP4(config);
            break;
        case kFLEXBUS_CSPMCR_Group5:
            FB->CSPMCR &= ~FB_CSPMCR_GROUP5_MASK;
            FB->CSPMCR |= FB_CSPMCR_GROUP5(config);
            break;
        default:
            break;
    }
}

/**
 * @brief  高级Flexbus 配置选项
 * @note   具体的配置应用详见关于FlexBus的使用例程
 * \param[in] CS 片选通道信号
 * @param[in] FLEXBUS_AdvancedConfigStruct
 * @retval None
 */
void FLEXBUS_AdvancedConfig(uint32_t CS, FLEXBUS_AdvancedConfigTypeDef* FLEXBUS_AdvancedConfigStruct)
{
    /* Wait States
    Specifies the number of wait states inserted after FlexBus asserts the associated chip-select and before
    an internal transfer acknowledge is generated (WS = 00h inserts 0 wait states, ..., WS = 3Fh inserts 63
    wait states).
     */
    FB->CS[CS].CSCR &= ~FB_CSCR_WS_MASK;
    FB->CS[CS].CSCR |= FB_CSCR_WS(FLEXBUS_AdvancedConfigStruct->kFLEXBUS_WS);
    
    /* Address Setup
    Controls when the chip-select is asserted with respect to assertion of a valid address and attributes.
    */
    FB->CS[CS].CSCR &= ~FB_CSCR_ASET_MASK;
    FB->CS[CS].CSCR |= FB_CSCR_ASET(FLEXBUS_AdvancedConfigStruct->kFLEXBUS_ASET);
    
    /* Read Address Hold or Deselect
    Controls the address and attribute hold time after the termination during a read cycle that hits in the
    associated chip-select's address space.
    */
    FB->CS[CS].CSCR &= ~FB_CSCR_RDAH_MASK;
    FB->CS[CS].CSCR |= FB_CSCR_RDAH(FLEXBUS_AdvancedConfigStruct->kFLEXBUS_RDAH);
    
    /* Write Address Hold or Deselect
    Controls the address, data, and attribute hold time after the termination of a write cycle that hits in the
    associated chip-select's address space.
    */
    FB->CS[CS].CSCR &= ~FB_CSCR_WRAH_MASK;
    FB->CS[CS].CSCR |= FB_CSCR_WRAH(FLEXBUS_AdvancedConfigStruct->kFLEXBUS_WRAH);
    
    /* brust read enable */
    (FLEXBUS_AdvancedConfigStruct->kFLEXBUS_brustReadEnable)?
    (FB->CS[CS].CSCR |= FB_CSCR_BSTR_MASK):
    (FB->CS[CS].CSCR &= ~FB_CSCR_BSTR_MASK);
   
    /* brust write enable */
    (FLEXBUS_AdvancedConfigStruct->kFLEXBUS_brustWriteEnable)?
    (FB->CS[CS].CSCR |= FB_CSCR_BSTW_MASK):
    (FB->CS[CS].CSCR &= ~FB_CSCR_BSTW_MASK);
}

#endif
