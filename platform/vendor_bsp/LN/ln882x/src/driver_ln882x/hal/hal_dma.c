#include "types.h"
#include "ll/ll_dma.h"
#include "hal/hal_dma.h"
#include "hal/hal_common.h"
#include "utils/debug/art_assert.h"

#pragma push
#pragma O2

/**
 * @brief DMA init.
 *
 * @param chanIndex
 * @param config
 */
void HAL_DMA_Init(DMA_Channel chanIndex, DMA_InitTypeDef *config)
{
    LL_DMA_Enable(LL_DMA_CONTROLLER_ENABLE);
    if(!config){
        return;
    }
    HAL_DMA_Config(chanIndex, config);
}

/**
 * @brief DMA Deinit.
 *
 * @param chanIndex
 */

void HAL_DMA_DeInit(DMA_Channel chanIndex)
{
    LL_DMA_Enable(LL_DMA_CONTROLLER_DISABLE);
}


/**
 * @brief DMA config.
 *
 * @param chanIndex
 * @param config
 */
void HAL_DMA_Config(DMA_Channel chanIndex, DMA_InitTypeDef *config)
{
    assert_param(config);
    assert_param(IS_DMA_CHANNELS(chanIndex));
    assert_param(IS_DMA_DEV_TYPE(config->device));
    assert_param(IS_DMA_TT_FC(config->type));
    assert_param(IS_DMA_SRCDST_MSIZE(config->msize));
    assert_param(IS_DMA_SRCDST_WIDTH(config->width));
    assert_param(IS_DMA_SINCDINC(config->src_inc));
    assert_param(IS_DMA_INT_ENX(config->int_en));

    switch(chanIndex) {
        case DMA_CHANNEL_0:
            {
                LL_DMA_ChanConfigureLowReg(LL_DMA_CHANNEL_0,
                        0, // max_abrst
                        LL_DMA_HANDSHAKE_POLARITY_ACTIVE_HIGH, // src_hs_pol
                        LL_DMA_HANDSHAKE_POLARITY_ACTIVE_HIGH, // dst_hs_pol
                        LL_DMA_HANDSHAKE_SOURCE_HARDWARE, // hs_sel_src
                        LL_DMA_HANDSHAKE_SOURCE_HARDWARE, // hs_sel_dst
                        LL_DMA_CHANNEL_SUSPEND_NO, // ch_susp
                        DMA_CHANNEL_NUM - 1); // ch_prior
                if (config->type == DMA_TRANS_TYPE_MEMORY_TO_MEMORY_DMAC_FLOW_CONTROLLER) {
                    LL_DMA_ChanConfigureHighReg(LL_DMA_CHANNEL_0,
                            0, // dest_per
                            0, // src_per
                            1, // protctl
                            0, // fifo_mode
                            0); // fc_mode
                } else if (config->type == DMA_TRANS_TYPE_MEMORY_TO_PERIPHERAL_DMAC_FLOW_CONTROLLER) {
                    LL_DMA_ChanConfigureHighReg(LL_DMA_CHANNEL_0,
                            config->device, // dest_per
                            0, // src_per
                            1, // protctl
                            0, // fifo_mode
                            0); // fc_mode
                } else if (config->type == DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY_DMAC_FLOW_CONTROLLER) {
                    LL_DMA_ChanConfigureHighReg(LL_DMA_CHANNEL_0,
                            0, // dest_per
                            config->device, // src_per
                            1, // protctl
                            0, // fifo_mode
                            0); // fc_mode
                } else {
                    assert_param(config->type <= DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY_DMAC_FLOW_CONTROLLER);
                }
                LL_DMA_ChanCtrlLowReg(LL_DMA_CHANNEL_0,
                        config->type, // tt_fc
                        config->msize, // src_msize
                        config->msize, // dest_msize
                        config->src_inc, // sinc
                        config->dst_inc, // dinc
                        config->width, // src_tr_width
                        config->width, // dst_tr_widt
                        config->int_en); // int_en
            }
            break;
        case DMA_CHANNEL_1:
            {
                LL_DMA_ChanConfigureLowReg(LL_DMA_CHANNEL_1,
                        0, // max_abrst
                        LL_DMA_HANDSHAKE_POLARITY_ACTIVE_HIGH, // src_hs_pol
                        LL_DMA_HANDSHAKE_POLARITY_ACTIVE_HIGH, // dst_hs_pol
                        LL_DMA_HANDSHAKE_SOURCE_HARDWARE, // hs_sel_src
                        LL_DMA_HANDSHAKE_SOURCE_HARDWARE, // hs_sel_dst
                        0, // ch_susp
                        LL_DMA_CHANNEL_NUM - 2); // ch_prior
                if (config->type == DMA_TRANS_TYPE_MEMORY_TO_MEMORY_DMAC_FLOW_CONTROLLER) {
                    LL_DMA_ChanConfigureHighReg(LL_DMA_CHANNEL_1,
                            0, // dest_per
                            0, // src_per
                            1, // protctl
                            0, // fifo_mode
                            0); // fc_mode
                } else if (config->type == DMA_TRANS_TYPE_MEMORY_TO_PERIPHERAL_DMAC_FLOW_CONTROLLER) {
                    LL_DMA_ChanConfigureHighReg(LL_DMA_CHANNEL_1,
                            config->device, // dest_per
                            0, // src_per
                            1, // protctl
                            0, // fifo_mode
                            0); // fc_mode
                } else if (config->type == DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY_DMAC_FLOW_CONTROLLER) {
                    LL_DMA_ChanConfigureHighReg(LL_DMA_CHANNEL_1,
                            0, // dest_per
                            config->device, // src_per
                            1, // protctl
                            0, // fifo_mode
                            0); // fc_mode
                } else {
                    assert_param(config->type <= DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY_DMAC_FLOW_CONTROLLER);
                }
                LL_DMA_ChanCtrlLowReg(LL_DMA_CHANNEL_1,
                        config->type, // tt_fc
                        config->msize, // src_msize
                        config->msize, // dest_msize
                        config->src_inc, // sinc
                        config->dst_inc, // dinc
                        config->width, // src_tr_width
                        config->width, // dst_tr_width
                        config->int_en); // int_en
            }
            break;
        case DMA_CHANNEL_2:
            {
                LL_DMA_ChanConfigureLowReg(LL_DMA_CHANNEL_2,
                        0, // max_abrst
                        LL_DMA_HANDSHAKE_POLARITY_ACTIVE_HIGH, // src_hs_pol
                        LL_DMA_HANDSHAKE_POLARITY_ACTIVE_HIGH, // dst_hs_pol
                        LL_DMA_HANDSHAKE_SOURCE_HARDWARE, // hs_sel_src
                        LL_DMA_HANDSHAKE_SOURCE_HARDWARE, // hs_sel_dst
                        0, // ch_susp
                        LL_DMA_CHANNEL_NUM - 3); // ch_prior
                if (config->type == DMA_TRANS_TYPE_MEMORY_TO_MEMORY_DMAC_FLOW_CONTROLLER) {
                    LL_DMA_ChanConfigureHighReg(LL_DMA_CHANNEL_2,
                            0, // dest_per
                            0, // src_per
                            1, // protctl
                            0, // fifo_mode
                            0); // fc_mode
                } else if (config->type == DMA_TRANS_TYPE_MEMORY_TO_PERIPHERAL_DMAC_FLOW_CONTROLLER) {
                    LL_DMA_ChanConfigureHighReg(LL_DMA_CHANNEL_2,
                            config->device, // dest_per
                            0, // src_per
                            1, // protctl
                            0, // fifo_mode
                            0); // fc_mode
                } else if (config->type == DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY_DMAC_FLOW_CONTROLLER) {
                    LL_DMA_ChanConfigureHighReg(LL_DMA_CHANNEL_2,
                            0, // dest_per
                            config->device, // src_per
                            1, // protctl
                            0, // fifo_mode
                            0); // fc_mode
                } else {
                    assert_param(config->type <= DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY_DMAC_FLOW_CONTROLLER);
                }

                LL_DMA_ChanCtrlLowReg(LL_DMA_CHANNEL_2,
                        config->type, // tt_fc
                        config->msize, // src_msize
                        config->msize, // dest_msize
                        config->src_inc, // sinc
                        config->dst_inc, // dinc
                        config->width, // src_tr_width
                        config->width, // dst_tr_width
                        config->int_en); // int_en
            }
            break;
        default:
            assert_param(chanIndex <= DMA_CHANNEL_2);
            break;
    }
}

/**
 * @brief Start DMA transfer.
 *
 * @param chanIndex DMA channel.
 * @param src Source address.
 * @param dst Destination address.
 * @param length data length.
 */
void HAL_DMA_StartTransfer(DMA_Channel chanIndex, void *src, void *dst, uint32_t length)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_SrcAddrSet(chanIndex, (uint32_t)src);
    LL_DMA_DstAddrSet(chanIndex, (uint32_t)dst);

    LL_DMA_ChanBlockTranSizeSet(chanIndex, length);

    LL_DMA_ChanEnable(chanIndex);
}


/**
 * @brief Wait for DMA transfer to finish.
 *
 * @param chanIndex DMA channel.
 */
void HAL_DMA_WaitDone(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    while( (LL_DMA_RawStatusBlock() & chanIndex ) == 0);
    LL_DMA_ClearBlock(chanIndex);
}

/**
 * @brief Enable/disalbe DMA controller, which must be done before
 * any channel activity can begin.
 *
 * @param enable @see DMAC_Enable.
 */
void HAL_DMA_GlobalEnable(DMAC_Enable en)
{
    assert_param(IS_DMAC_ENABLEX(en));

    LL_DMA_Enable(en);
}

/**
 * @brief Software can poll the channel bits to determine when these channl bits
 * are free for a new DMA transfer.
 *
 * @return uint8_t ORed value from these enabled channels.
 */
uint8_t HAL_DMA_GetBusyChannel(void)
{
    return LL_DMA_GetBusyChannel();
}

/**
 * @brief Interrupt events are stored in these Raw Interrupt Status registers before masking.
 * Each Raw Interrupt Status register has a bit allocated per channel; for example,
 * RawTfr[2] is the Channel 2 raw transfer complete interrupt.
 *
 * @return uint8_t
 */
uint8_t HAL_DMA_RawStatusTfr(void)
{
    return LL_DMA_RawStatusTfr();
}

uint8_t HAL_DMA_RawStatusBlock(void)
{
    return LL_DMA_RawStatusBlock();
}

uint8_t HAL_DMA_RawStatusSrcTran(void)
{
    return LL_DMA_RawStatusSrcTran();
}

uint8_t HAL_DMA_RawStatusDstTran(void)
{
    return LL_DMA_RawStatusDstTran();
}

uint8_t HAL_DMA_RawStatusErr(void)
{
    return LL_DMA_RawStatusErr();
}


uint8_t HAL_DMA_StatusTfr(void)
{
    return LL_DMA_StatusTfr();
}

uint8_t HAL_DMA_StatusBlock(void)
{
    return LL_DMA_StatusBlock();
}

uint8_t HAL_DMA_StatusSrcTran(void)
{
    return LL_DMA_StatusSrcTran();
}

uint8_t HAL_DMA_StatusDstTran(void)
{
    return LL_DMA_StatusDstTran();
}

uint8_t HAL_DMA_StatusErr(void)
{
    return LL_DMA_StatusErr();
}

void HAL_DMA_UnMaskTfr(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_UnMaskTfr(chanIndex);
}

void HAL_DMA_UnMaskBlock(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_UnMaskBlock(chanIndex);
}

void HAL_DMA_UnMaskSrcTran(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_UnMaskSrcTran(chanIndex);
}

void HAL_DMA_UnMaskDstTran(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_UnMaskDstTran(chanIndex);
}

void HAL_DMA_UnMaskErr(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_UnMaskErr(chanIndex);
}

void HAL_DMA_ClearTfr(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_ClearTfr(chanIndex);
}

void HAL_DMA_ClearBlock(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_ClearBlock(chanIndex);
}

void HAL_DMA_ClearSrcTran(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_ClearSrcTran(chanIndex);
}

void HAL_DMA_ClearDstTran(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_ClearDstTran(chanIndex);
}

void HAL_DMA_ClearErr(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_ClearErr(chanIndex);
}

uint8_t HAL_DMA_IntStatus(void)
{
    return LL_DMA_IntStatus();
}

void HAL_DMA_SWSrcTranRequest(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_SWSrcTranRequest(chanIndex);
}

uint8_t HAL_DMA_SWSrcTranRead(void)
{
    return LL_DMA_SWSrcTranRead();
}

void HAL_DMA_SWDstTranRequest(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_SWDstTranRequest(chanIndex);
}

uint8_t HAL_DMA_SWDstTranRead(void)
{
    return LL_DMA_SWDstTranRead();
}

void HAL_DMA_SingleSrcTranRequest(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_SingleSrcTranRequest(chanIndex);
}

uint8_t HAL_DMA_SingleSrcTranRead(void)
{
    return LL_DMA_SingleSrcTranRead();
}

void HAL_DMA_SingleDstTranRequest(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_SingleDstTranRequest(chanIndex);
}

uint8_t HAL_DMA_SingleDstTranRead(void)
{
    return LL_DMA_SingleDstTranRead();
}

void HAL_DMA_LastSrcTranRequest(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_LastSrcTranRequest(chanIndex);
}

uint8_t HAL_DMA_LastSrcTranRead(void)
{
    return LL_DMA_LastSrcTranRead();
}

void HAL_DMA_LastDstTranRequest(DMA_Channel chanIndex)
{
    assert_param(IS_DMA_CHANNELS(chanIndex));

    LL_DMA_LastDstTranRequest(chanIndex);
}

uint8_t HAL_DMA_LastDstTranRead(void)
{
    return LL_DMA_LastDstTranRead();
}

#pragma pop


