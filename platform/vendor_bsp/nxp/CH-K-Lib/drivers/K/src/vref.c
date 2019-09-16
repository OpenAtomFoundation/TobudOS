/**
  ******************************************************************************
  * @file    vref.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * \date    2015.10.03 FreeXc完善了vref模块的相关注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
 #include "vref.h"
 #include "common.h"
 
 /**
 * @brief  初始化VREF 模块
 * @param[in]  VREF_InitStruct 指向VREF初始化结构体的指针
 * @retval None
 */
void VREF_Init(VREF_InitTypeDef *VREF_InitStruct)
{
    /* open clock gate */
    SIM->SCGC4 |= SIM_SCGC4_VREF_MASK;
    /* enable moudle */
    #ifdef VREF_SC_ICOMPEN_MASK
    VREF->SC = VREF_SC_VREFEN_MASK | VREF_SC_REGEN_MASK | VREF_SC_ICOMPEN_MASK | VREF_SC_MODE_LV(VREF_InitStruct->bufferMode);
    VREF->TRM |= VREF_TRM_CHOPEN_MASK;
    #else
    VREF->SC = VREF_SC_VREFEN_MASK | VREF_SC_REGEN_MASK  | VREF_SC_MODE_LV(VREF_InitStruct->bufferMode);
    #endif 
}

 /**
 * @brief  快速初始化VREF模块
 * @note   VREF被初始化后 需要经过大概35MS 才能有稳定的参考电压输出
* \attention bufferMode对于不同的芯片，其可选的参数视不同的，比如K60只能是00和10，而K64有三种模式可选
 * @retval None
 */
void VREF_QuickInit(void)
{
    VREF_InitTypeDef  VREF_InitStruct1;
    VREF_InitStruct1.instance = 0;
    VREF_InitStruct1.bufferMode = kVREF_BufferMode_Lv1;
    VREF_Init(&VREF_InitStruct1);
}

 /**
 * @brief  恢复VREF到默认状态
 * @retval None
 */
void VREF_DeInit(void)
{
    SIM->SCGC4 &= ~SIM_SCGC4_VREF_MASK;
    VREF->SC = 0;
}

 /**
 * @brief  设置VREF校准值
 * \attention the trim value changes the resulting VREF by approximately ± 0.5 mV for each step
 * \attention It is ± 0.5 mV,pay attention to the unit
 * @param[in]  val 校准值
 * @retval None
 */
void VREF_SetTrimValue(uint32_t val)
{
    VREF->TRM &= ~VREF_TRM_TRIM_MASK;
    VREF->TRM |= VREF_TRM_TRIM(val);
}
 
