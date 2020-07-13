#ifndef __LL_DMA_H_
#define __LL_DMA_H_

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"
//////////////////////////////  Data Types and Macros  ////////////////////////

/**
 * @brief enable/disable DMA controller.
 * 
 */
#define LL_DMA_CONTROLLER_DISABLE       0
#define LL_DMA_CONTROLLER_ENABLE        1

/**
 * @brief Set the DMA channel to operate.
 * 
 */
#define LL_DMA_CHANNEL_0        0x01
#define LL_DMA_CHANNEL_1        0x02
#define LL_DMA_CHANNEL_2        0x04

#define LL_DMA_CHANNEL_NUM      3


/**
 * @brief enable/disable DMA interrupt.
 * 
 */
#define LL_DMA_INTERRUPT_DISABLE        0
#define LL_DMA_INTERRUPT_ENABLE         1

/**
 * @brief Handshake ploarity.
 * 
 */
#define LL_DMA_HANDSHAKE_POLARITY_ACTIVE_HIGH   0
#define LL_DMA_HANDSHAKE_POLARITY_ACTIVE_LOW    1

/**
 * @brief Source Software or Hardware Handshaking Select.
 * 
 */
#define LL_DMA_HANDSHAKE_SOURCE_HARDWARE        0
#define LL_DMA_HANDSHAKE_SOURCE_SOFTWARE        1

/**
 * @brief Channel Suspend.
 * 0 -- Not suspended.
 * 1 -- Suspend DMA transfer from the source.
 */
#define LL_DMA_CHANNEL_SUSPEND_NO               0
#define LL_DMA_CHANNEL_SUSPEND_YES              1

/**
 * @brief Channel priority.
 * 0 is the lowest.
 * 
 */
#define LL_DMA_CHANNEL_PRIORITY_0       0
#define LL_DMA_CHANNEL_PRIORITY_1       1
#define LL_DMA_CHANNEL_PRIORITY_2       2

// FIXME: only support 8, 16, 32bits?
/**
 * @brief Transfer width of src/dst.
 * 
 */
#define LL_DMA_TRANSFER_WIDTH_8_BITS    0
#define LL_DMA_TRANSFER_WIDTH_16_BITS   1
#define LL_DMA_TRANSFER_WIDTH_32_BITS   2
#define LL_DMA_TRANSFER_WIDTH_64_BITS   3
#define LL_DMA_TRANSFER_WIDTH_128_BITS  4
#define LL_DMA_TRANSFER_WIDTH_256_BITS  5

/**
 * @brief Define the address increasement of source of destination device.
 * when working in memory to memory, source and destination address are all set to INCREMENT;
 * when working in memory to peripheral mode, source address is set to INCREMENT and destination address is set to UNCHANGE;
 * when working in peripheral to memory to mode, destination address is set to INCREMENT and source address is set to UNCHANGE.
 * 
 */
#define LL_DMA_ADDRESS_INCREMENT        0
#define LL_DMA_ADDRESS_DECREMENT        1
#define LL_DMA_ADDRESS_UNCHANGE         2

// FIXME: only support burst transaction length 8, 16, 32bits?
/**
 * @brief Set the burst transaction length. The threshold to request of dma transfer in peripheral should be less than the length set here.
 * Normally, the threshold = msize-1. For example, msize = LL_DMA_BURST_TRANSACTION_LENGTH_4, then the threshold should set to 3.
 * 
 */
#define LL_DMA_BURST_TRANSACTION_LENGTH_1       0
#define LL_DMA_BURST_TRANSACTION_LENGTH_4       1
#define LL_DMA_BURST_TRANSACTION_LENGTH_8       2
#define LL_DMA_BURST_TRANSACTION_LENGTH_16      3
#define LL_DMA_BURST_TRANSACTION_LENGTH_32      4
#define LL_DMA_BURST_TRANSACTION_LENGTH_64      5
#define LL_DMA_BURST_TRANSACTION_LENGTH_128     6
#define LL_DMA_BURST_TRANSACTION_LENGTH_256     7

#define LL_DMA_MAX_TRANSFER_SIZE    4095

/**
 * @brief Set the DMA Transfer type and Flow Control.
 * 
 */
#define LL_DMA_TRANS_TYPE_MEMORY_TO_MEMORY      0
#define LL_DMA_TRANS_TYPE_MEMORY_TO_PERIPHERAL  1
#define LL_DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY  2

/**
 * @brief Set the device type.
 * 
 */
#define LL_DMA_DEV_TYPE_QSPI_RX     0
#define LL_DMA_DEV_TYPE_QSPI_TX     1
#define LL_DMA_DEV_TYPE_UART0_TX    2
#define LL_DMA_DEV_TYPE_UART0_RX    3
#define LL_DMA_DEV_TYPE_UART1_TX    4
#define LL_DMA_DEV_TYPE_UART1_RX    5
#define LL_DMA_DEV_TYPE_SPIM_TX     6
#define LL_DMA_DEV_TYPE_SPIM_RX     7
#define LL_DMA_DEV_TYPE_SPIS_TX     8
#define LL_DMA_DEV_TYPE_SPIS_RX     9
#define LL_DMA_DEV_TYPE_I2C0_TX     10
#define LL_DMA_DEV_TYPE_I2C0_RX     11
#define LL_DMA_DEV_TYPE_I2C1_TX     12
#define LL_DMA_DEV_TYPE_I2C1_RX     13
#define LL_DMA_DEV_TYPE_AUXADC      14

/**
 * @brief define the bitmap of DMA interrupt.
 * 
 */
#define LL_DMA_INT_STATUS_TFR           0x01
#define LL_DMA_INT_STATUS_BLOCK         0x02
#define LL_DMA_INT_STATUS_SRCTRAN       0x04
#define LL_DMA_INT_STATUS_DSTTRAN       0x08
#define LL_DMA_INT_STATUS_ERR           0x10


#define LL_DMA_I2C0_RW_ADDRESS          ((void *)0x40007010)
#define LL_DMA_SPIM_RW_ADDRESS          ((void *)0x40005060)
#define LL_DMA_UART0_RW_ADDRESS         ((void *)0x40002000)


//////////////////////////////   Function Declaration  ////////////////////////

/**
 * @brief Enable/disalbe DMA controller, which must be done before 
 * any channel activity can begin.
 * 
 * @param enable @see LL_DMA_CONTROLLER_ENABLE.
 */
void LL_DMA_Enable(bool enable);

/**
 * @brief Enable/disalbe the channel.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 */
void LL_DMA_ChanEnable(uint8_t chanIndex);

/**
 * @brief Software can poll the channel bits to determine when these channl bits
 * are free for a new DMA transfer.
 * 
 * @return uint8_t ORed value from these enabled channels.
 */
uint8_t LL_DMA_GetBusyChannel(void);

/**
 * @brief Maximum AMBA Burst Length Set/Get.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param maxAbrst
 */
void LL_DMA_MaxAbrstSet(uint8_t chanIndex, uint16_t maxAbrst);

uint8_t LL_DMA_MaxAbrstGet(uint8_t chanIndex);

/**
 * @brief Source Handshaking Interface Polarity Set/Get.
 * 
 * @param chanIndex 
 * @param src_hs_pol 
 */
void LL_DMA_SrcHandshakePolSet(uint8_t chanIndex, uint8_t src_hs_pol);

uint8_t LL_DMA_SrcHandshakePolGet(uint8_t chanIndex);

/**
 * @brief Destination Handshaking interface Polarity Set/Get.
 * 
 * @param chanIndex 
 * @param dst_hs_pol 
 */
void LL_DMA_DstHandShakePolSet(uint8_t chanIndex, uint8_t dst_hs_pol);

uint8_t LL_DMA_DstHandShakePolGet(uint8_t chanIndex);

/**
 * @brief Source Software or Hardware Handshaking Select.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param hs_sel_src @see LL_DMA_HANDSHAKE_SOURCE_HARDWARE
 */
void LL_DMA_HandshakeSelectSrcSet(uint8_t chanIndex, uint8_t hs_sel_src);

uint8_t LL_DMA_HandShakeSelectSrcGet(uint8_t chanIndex);

/**
 * @brief Destination Software or Hardware Handshaking Select.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param hs_sel_dst @see LL_DMA_HANDSHAKE_SOURCE_HARDWARE
 */
void LL_DMA_HandshakeSelectDstSet(uint8_t chanIndex, uint8_t hs_sel_dst);

uint8_t LL_DMA_HandshakeSelectDstGet(uint8_t chanIndex);

/**
 * @brief Channel Suspend. Suspends all DMA data transfers from the source until this bit is cleared.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param ch_susp @see LL_DMA_CHANNEL_SUSPEND_NO
 */
void LL_DMA_ChanSuspSet(uint8_t chanIndex, uint8_t ch_susp);

uint8_t LL_DMA_ChanSuspGet(uint8_t chanIndex);

/**
 * @brief Channel priority.
 * 0 is the lowest.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param ch_prior @see LL_DMA_CHANNEL_PRIORITY_0
 */
void LL_DMA_ChanPrioritySet(uint8_t chanIndex, uint8_t ch_prior);

uint8_t LL_DMA_ChanPriorityGet(uint8_t chanIndex);

/**
 * @brief Assigns a hradware handshaking interface to the destination of this 
 * channel if the channel handshake select destination is LL_DMA_HANDSHAKE_SOURCE_HARDWARE; 
 * otherwise, this field is ignored.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0 
 * @param dest_per @see LL_DMA_DEV_TYPE_QSPI_RX
 */
void LL_DMA_DestPeripheralSet(uint8_t chanIndex, uint8_t dest_per);

uint8_t LL_DMA_DestPeripheralGet(uint8_t chanIndex);

/**
 * @brief Assigns a hardware handshakeing interface (0-15) to the source of 
 * this channel if the chan handshake select srouce field is LL_DMA_HANDSHAKE_SOURCE_HARDWARE; 
 * otherwise, this field is ignored.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param src_per @see LL_DMA_DEV_TYPE_QSPI_RX
 */
void LL_DMA_SrcPeripheralSet(uint8_t chanIndex, uint8_t src_per);

uint8_t LL_DMA_SrcPeripheralGet(uint8_t chanIndex);

// FIXME: not used by user.
/**
 * @brief Protection Control bits used to drive the AHB HPROT[3:1] bus.
 * 
 * @param chanIndex 
 * @param protctl 
 */
void LL_DMA_ProtectionCtrlSet(uint8_t chanIndex, uint8_t protctl);

uint8_t LL_DMA_ProtectionCtrlGet(uint8_t chanIndex);

/**
 * @brief FIFO Mode Select.
 * Determines how much space or data needs to be available in the FIFO before 
 * a burst transaction request is serviced.
 * 
 * @param chanIndex 
 * @param fifo_mode 
 */
void LL_DMA_FifoModeSet(uint8_t chanIndex, uint8_t fifo_mode);

uint8_t LL_DMA_FifoModeGet(uint8_t chanIndex);

/**
 * @brief Flow Control Mode.
 * Determines when source transaction requests are serviced when the Destination
 *  Peripheral is the flow controller.
 * 
 * 0 -- Source transaction reqeusts are serviced when they occur.
 * 1 -- Source transaction requests are not serviced until a destination transaction
 * request occurs.
 * 
 * @param chanIndex 
 * @param fc_mode 
 */
void LL_DMA_FlowControlModeSet(uint8_t chanIndex, uint8_t fc_mode);

uint8_t LL_DMA_FlowControlModeGet(uint8_t chanIndex);

void LL_DMA_ChanConfigureLowReg(uint8_t chanIndex, 
                                uint16_t max_abrst, 
                                uint8_t src_hs_pol, 
                                uint8_t dst_hs_pol, 
                                uint8_t hs_sel_src,
                                uint8_t hs_sel_dst,
                                uint8_t ch_susp,
                                uint8_t ch_prior);

void LL_DMA_ChanConfigureHighReg(uint8_t chanIndex,
                                uint8_t dest_per,
                                uint8_t src_per,
                                uint8_t protctl,
                                uint8_t fifo_mode,
                                uint8_t fc_mode);

/**
 * @brief Set Chan CFG register.
 * 
 * @param chanIndex 
 * @param max_abrst 
 * @param src_hs_pol 
 * @param dst_hs_pol 
 * @param hs_sel_src 
 * @param hs_sel_dst 
 * @param ch_susp 
 * @param ch_prior 
 * @param dest_per 
 * @param src_per 
 * @param protctl 
 * @param fifo_mode 
 * @param fc_mode 
 */
void LL_DMA_ChanConfigure(  uint8_t chanIndex,
                            uint16_t max_abrst,
                            uint8_t src_hs_pol,
                            uint8_t dst_hs_pol,
                            uint8_t hs_sel_src,
                            uint8_t hs_sel_dst,
                            uint8_t ch_susp,
                            uint8_t ch_prior,
                            uint8_t dest_per,
                            uint8_t src_per,
                            uint8_t protctl,
                            uint8_t fifo_mode,
                            uint8_t fc_mode );

/**
 * @brief Current Source Address of DMA transfer.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param src_addr source address.
 */
void LL_DMA_SrcAddrSet(uint8_t chanIndex, uint32_t src_addr);

uint32_t LL_DAM_SrcAddrGet(uint8_t chanIndex);

/**
 * @brief Current Destination address of DMA transfer.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param dst_addr destination address.
 */
void LL_DMA_DstAddrSet(uint8_t chanIndex, uint32_t dst_addr);

uint32_t LL_DMA_DstAddrGet(uint8_t chanIndex);

/**
 * @brief Transfer Type and Flow Contrl.
 * The following transfer types are supported.
 * - Memory to Memory
 * - Memory to Peripheral
 * - Peripheral to Memory
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param tt_fc @see LL_DMA_TRANS_TYPE_MEMORY_TO_MEMORY
 */
void LL_DMA_TranTypeFlowCtrlSet(uint8_t chanIndex, uint8_t tt_fc);

uint8_t LL_DMA_TranTypeFlowCtrlGet(uint8_t chanIndex);

/**
 * @brief Source Burst Transaction Length.
 * Number of data items, each of width src_tr_width, to be read from the source
 * every time a source burst transaction request is made from either the corresponding
 * hardware or software handshaking interface.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param src_msize @see LL_DMA_BURST_TRANSACTION_LENGTH_8
 */
void LL_DMA_SrcMsizeSet(uint8_t chanIndex, uint8_t src_msize);

uint8_t LL_DMA_SrcMsizeGet(uint8_t chanIndex);

/**
 * @brief Destination Burst Transaction Length.
 * Number of data items, each of width dst_tr_width, to be written to the destination
 * every time a destination burst transaction request is made from either the 
 * corresponding hardware or software handshaking interface.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param dest_msize @see LL_DMA_BURST_TRANSACTION_LENGTH_8
 */
void LL_DMA_DestMsizeSet(uint8_t chanIndex, uint8_t dest_msize);

uint8_t LL_DMA_DestMsizeGet(uint8_t chanIndex);

/**
 * @brief Source Address Increment.
 * Indicates whether to increment or decrement the source address on every source transfer.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param sinc @see LL_DMA_ADDRESS_INCREMENT
 */
void LL_DMA_SrcAddrIncSet(uint8_t chanIndex, uint8_t sinc);

uint8_t LL_DMA_SrcAddrIncGet(uint8_t chanIndex);

/**
 * @brief Destination Address Increment.
 * Indicates whether to increment or decrement the destination address on every
 * destination transfer.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param dinc @see LL_DMA_ADDRESS_INCREMENT
 */
void LL_DMA_DestAddrIncSet(uint8_t chanIndex, uint8_t dinc);

uint8_t LL_DMA_DestAddrIncGet(uint8_t chanIndex);

/**
 * @brief Source Transfer width.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param src_tr_width @see LL_DMA_TRANSFER_WIDTH_8_BITS
 */
void LL_DMA_SrcTrWidthSet(uint8_t chanIndex, uint8_t src_tr_width);

uint8_t LL_DMA_SrcTrWidthGet(uint8_t chanIndex);

/**
 * @brief Destination Transfer width.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param dst_tr_width @see LL_DMA_TRANSFER_WIDTH_8_BITS
 */
void LL_DMA_DestTrWidthSet(uint8_t chanIndex, uint8_t dst_tr_width);

uint8_t LL_DMA_DestTrWidthGet(uint8_t chanIndex);

/**
 * @brief Interrupt Enable.
 * If set, then all interrupt-generating sources are enabled.
 * Functions as a global mask bit for all interrupts for the channel;
 * raw interrupt registers still assert if channel int_en is 0.
 * 
 * @param chanIndex 
 * @param int_en 
 */
void LL_DMA_ChanIntEnSet(uint8_t chanIndex, uint8_t int_en);

uint8_t LL_DMA_ChanIntEnGet(uint8_t chanIndex);

/**
 * @brief Done bit.
 * If status write-back is enabled, the upper word of the control register, CTLX[63:32],
 * is written to the control register location of the linked List Item ( LLI ) in 
 * system memory at the end of the block transfer with the done bit set.
 * 
 * Software can poll the LLI CTLX.DONE bit to see when a block transfer is complete.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param done 
 */
void LL_DMA_ChanDoneSet(uint8_t chanIndex, uint8_t done);

uint8_t LL_DMA_ChanDoneGet(uint8_t chanIndex);

/**
 * @brief Block Transfer Size.
 * The number programmed into BLOCK_TS indicates the total number of single transactions
 * to perform for every block transfer; a single transaction is mapped to a single AMBA beat.
 * 
 * @param chanIndex @see LL_DMA_CHANNEL_0
 * @param block_ts 
 */
void LL_DMA_ChanBlockTranSizeSet(uint8_t chanIndex, uint32_t block_ts);

uint32_t LL_DMA_ChanBlockTranSizeGet(uint8_t chanIndex);

void LL_DMA_ChanCtrlLowReg(uint8_t chanIndex,
                            uint8_t tt_fc,
                            uint8_t src_msize,
                            uint8_t dest_msize,
                            uint8_t sinc,
                            uint8_t dinc,
                            uint8_t src_tr_width,
                            uint8_t dst_tr_width,
                            uint8_t int_en);

void LL_DMA_ChanCtrlHighReg(uint8_t chanIndex,
                            uint8_t done,
                            uint16_t block_ts);

/**
 * @brief 
 * 
 * @param chanIndex 
 * @param tt_fc 
 * @param src_msize 
 * @param dest_msize 
 * @param sinc 
 * @param dinc 
 * @param src_tr_width 
 * @param dst_tr_width 
 * @param int_en 
 * @param done 
 * @param block_ts 
 */
void LL_DMA_ChanCtrl(uint8_t chanIndex, 
                    uint8_t tt_fc,
                    uint8_t src_msize,
                    uint8_t dest_msize,
                    uint8_t sinc,
                    uint8_t dinc,
                    uint8_t src_tr_width,
                    uint8_t dst_tr_width,
                    uint8_t int_en,
                    uint8_t done,
                    uint16_t block_ts);

/**
 * @brief Interrupt events are stored in these Raw Interrupt Status registers before masking.
 * Each Raw Interrupt Status register has a bit allocated per channel; for example,
 * RawTfr[2] is the Channel 2 raw transfer complete interrupt.
 * 
 * @return uint8_t 
 */
uint8_t LL_DMA_RawStatusBlock(void);

uint8_t LL_DMA_RawStatusErr(void);

uint8_t LL_DMA_RawStatusTfr(void);

uint8_t LL_DMA_RawStatusDstTran(void);

uint8_t LL_DMA_RawStatusSrcTran(void);

/**
 * @brief All interrupt events from all channles are stored in these Interrupt 
 * Status registers after masking.
 * Each Interrupt Status register has a bit allocated per channel: for example, 
 * StatusTfr[2] is the Channel 2 status transfer complete interrupt.
 * 
 * @return uint8_t 
 */
uint8_t LL_DMA_StatusBlock(void);

uint8_t LL_DMA_StatusErr(void);

uint8_t LL_DMA_StatusTfr(void);

uint8_t LL_DMA_StatusDstTran(void);

uint8_t LL_DMA_StatusSrcTran(void);

/**
 * @brief The contents of the Raw Status registers are masked with the contents of the Mask registers.
 * Each Interrupt Mask register has a bit allocated per channel: for example, MaksTfr[2] is the mask
 * bit for the Channel 2 transfer complete interrupt.
 * @param chanIndex 
 */
void LL_DMA_UnMaskBlock(uint8_t chanIndex);

void LL_DMA_UnMaskErr(uint8_t chanIndex);

void LL_DMA_UnMaskTfr(uint8_t chanIndex);

void LL_DMA_UnMaskDstTran(uint8_t chanIndex);

void LL_DMA_UnMaskSrcTran(uint8_t chanIndex);

/**
 * @brief Each Interrupt Clear register has a bit allocated per channel; for example, ClearTfr[2] 
 * is the clear bit for the Channel 2 transfer complete interrupt.
 * 
 * @param chanIndex 
 */
void LL_DMA_ClearBlock(uint8_t chanIndex);

void LL_DMA_ClearErr(uint8_t chanIndex);

void LL_DMA_ClearTfr(uint8_t chanIndex);

void LL_DMA_ClearDstTran(uint8_t chanIndex);

void LL_DMA_ClearSrcTran(uint8_t chanIndex);

/**
 * @brief Combined Interrupt Status Register.
 * 
 * @return uint8_t The contents of each of teh five status registers is ORed to produce a single bit for
 * each interrupt type in the Combined Status register.
 */
uint8_t LL_DMA_IntStatus(void);

void LL_DMA_SWSrcTranRequest(uint8_t chanIndex);

uint8_t LL_DMA_SWSrcTranRead(void);

void LL_DMA_SWDstTranRequest(uint8_t chanIndex);

uint8_t LL_DMA_SWDstTranRead(void);

void LL_DMA_SingleSrcTranRequest(uint8_t chanIndex);

uint8_t LL_DMA_SingleSrcTranRead(void);

void LL_DMA_SingleDstTranRequest(uint8_t chanIndex);

uint8_t LL_DMA_SingleDstTranRead(void);

void LL_DMA_LastSrcTranRequest(uint8_t chanIndex);

uint8_t LL_DMA_LastSrcTranRead(void);

void LL_DMA_LastDstTranRequest(uint8_t chanIndex);

uint8_t LL_DMA_LastDstTranRead(void);

#ifdef __cplusplus
    }
#endif // __cplusplus

#endif // __LL_DMA_H_
