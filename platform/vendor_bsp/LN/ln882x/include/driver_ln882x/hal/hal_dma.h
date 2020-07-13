#ifndef __HAL_DMA_H_
#define __HAL_DMA_H_

#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

/******************************  Data Type and Macros  **********************/

/**
 * @brief enum to enable/disable DMA controller.
 *
 */
typedef enum 
{
    DMAC_DISABLE = 0,
    DMAC_ENABLE
} DMAC_Enable;

/**
 * @brief
 *
 */
typedef enum 
{
    DMA_CHANNEL_0   = 0x01,
    DMA_CHANNEL_1   = 0x02,
    DMA_CHANNEL_2   = 0x04,
    DMA_CHANNEL_NUM = 0x03
} DMA_Channel;


/**
 * @brief enum of DMA_Int_En to enable or disable DMA interrupt.
 *
 */
typedef enum 
{
    DMA_INTERRUPT_DISABLE   = 0,
    DMA_INTERRUPT_ENABLE
} DMA_Int_En;

/**
 * @brief Handshake ploarity.
 *
 */
typedef enum 
{
    DMA_HS_POLARITY_ACTIVE_HIGH = 0,
    DMA_HS_POLARITY_ACTIVE_LOW
} DMA_Handshake_Polarity;

/**
 * @brief Select handshake interface to software or hardware.
 *
 */
typedef enum 
{
    DMA_HANDSHAKE_HARDWARE = 0,
    DMA_HANDSHAKE_SOFTWARE
} DMA_Handshake_Source;

/**
 * @brief Channel Suspend.
 * 0 -- Not suspended.
 * 1 -- Suspend DMA transfer from the source.
 */
typedef enum 
{
    DMA_CHAN_SUSPEND_NO = 0,
    DMA_CHAN_SUSPEND_YES
} DMA_Chan_Suspend;

/**
 * @brief enum of transfer width.
 *
 */
typedef enum 
{
    DMA_TRANSFER_WIDTH_8_BITS   = 0,
    DMA_TRANSFER_WIDTH_16_BITS,
    DMA_TRANSFER_WIDTH_32_BITS,
    DMA_TRANSFER_WIDTH_64_BITS,
    DMA_TRANSFER_WIDTH_128_BITS,
    DMA_TRANSFER_WIDTH_256_BITS
} DMA_Src_Dst_Width;

/**
 * @brief Define the address increasement of source or destination device.
 * when working in memory to memory mode, source and destination address are all set to DMA_ADDRESS_INCREMENT;
 * when working in memory to peripheral mdoe, source address is set to DMA_ADDRESS_INCREMENT and destination is set to DMA_ADDRESS_UNCHANGE;
 * when working in peripheral to memory mode, destination address is set to DMA_ADDRESS_INCREMENT and source address is set to DMA_ADDRESS_UNCHANGE.
 *
 */
typedef enum 
{
    DMA_ADDRESS_INCREMENT = 0,
    DMA_ADDRESS_DECREMENT,
    DMA_ADDRESS_UNCHANGE
} DMA_Sinc_Dinc;

/**
 * @brief Set the burst transaction length. The threashold to request of DMA transfer in peripheral should be less than the length set here.
 * Normally, the threshold = msize - 1. For example, msize = DMA_BURST_TRANSACTION_LENGTH_4, then the threshold should set to 3.
 *
 */
typedef enum 
{
    DMA_BURST_TRANSACTION_LENGTH_1  = 0,
    DMA_BURST_TRANSACTION_LENGTH_4,
    DMA_BURST_TRANSACTION_LENGTH_8,
    DMA_BURST_TRANSACTION_LENGTH_16,
    DMA_BURST_TRANSACTION_LENGTH_32,
    DMA_BURST_TRANSACTION_LENGTH_64,
    DMA_BURST_TRANSACTION_LENGTH_128,
    DMA_BURST_TRANSACTION_LENGTH_256
} DMA_Src_Dst_Msize;

/**
 * @brief Set the DMA Transfer Type.
 *
 */
typedef enum 
{
    DMA_TRANS_TYPE_MEMORY_TO_MEMORY_DMAC_FLOW_CONTROLLER    = 0,
    DMA_TRANS_TYPE_MEMORY_TO_PERIPHERAL_DMAC_FLOW_CONTROLLER,
    DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY_DMAC_FLOW_CONTROLLER
} DMA_Transfer_Type;

/**
 * @brief Set the device type.
 *
 */
typedef enum 
{
    DMA_DEVICE_QSPI_RX = 0,
    DMA_DEVICE_QSPI_TX,
    DMA_DEVICE_UART0_TX,
    DMA_DEVICE_UART0_RX,
    DMA_DEVICE_UART1_TX,
    DMA_DEVICE_UART1_RX,
    DMA_DEVICE_SPIM_TX,
    DMA_DEVICE_SPIM_RX,
    DMA_DEVICE_SPIS_TX,
    DMA_DEVICE_SPIS_RX,
    DMA_DEVICE_I2C0_TX,
    DMA_DEVICE_I2C0_RX,
    DMA_DEVICE_I2C1_TX,
    DMA_DEVICE_I2C1_RX,
    DMA_DEVICE_AUXADC
} DMA_Dev_Type;

/**
 * @brief define the bitmap of DMA interrupt.
 *
 */
typedef enum 
{
    DMA_INT_STATUS_TFR      = 0x01,
    DMA_INT_STATUS_BLOCK    = 0x02,
    DMA_INT_STATUS_SRCTRAN  = 0x04,
    DMA_INT_STATUS_DSTTRAN  = 0x08,
    DMA_INT_STATUS_ERR      = 0x10
} DMA_Int_Status;

#define MAX_DMAC_TRANSFER_SIZE  (4095)

#define DMA_I2C0_RW_ADDRESS     ( (void *)0x40007010 )
#define DMA_SPIM_RW_ADDRESS     ( (void *)0x40005060 )
#define DMA_UART0_RW_ADDRESS    ( (void *)0x40002000 )

typedef struct 
{
    DMA_Dev_Type device;        /**< device: select which device need to perform a DMA operation */
    DMA_Transfer_Type type;     /**< type: define the DMA transfer type */
    DMA_Src_Dst_Msize msize;    /**< msize: define the burst transaction length */
    DMA_Src_Dst_Width width;    /**< width: define the DMA transfer width */
    DMA_Sinc_Dinc src_inc;      /**< src_inc: define the source address increasement */
    DMA_Sinc_Dinc dst_inc;      /**< dst_inc: define the destination address increasement */
    DMA_Int_En int_en;          /**< int_en: enable or disable the interrupt */
} DMA_InitTypeDef;

/***************************** sanity check *********************************/
#define IS_DMAC_ENABLEX(en)     ((en) == DMAC_DISABLE || (en) == DMAC_ENABLE)
#define IS_DMA_CHANNELS(chan)   ((chan) == DMA_CHANNEL_0 || (chan) == DMA_CHANNEL_1 || (chan) == DMA_CHANNEL_2)
#define IS_DMA_INT_ENX(en)      ((en) == DMA_INTERRUPT_DISABLE || (en) == DMA_INTERRUPT_ENABLE)
#define IS_DMA_HS_POLX(pol)     ((pol) == DMA_HS_POLARITY_ACTIVE_HIGH || (pol) == DMA_HS_POLARITY_ACTIVE_LOW)
#define IS_DMA_HS_SOURCEX(src)  ((src) == DMA_HANDSHAKE_HARDWARE || (src) == DMA_HANDSHAKE_SOFTWARE)
#define IS_DMA_CHAN_SUSPX(susp) ((susp) == DMA_CHAN_SUSPEND_NO || (susp) == DMA_CHAN_SUSPEND_YES)
#define IS_DMA_SRCDST_WIDTH(w)  ((w) == DMA_TRANSFER_WIDTH_8_BITS || \
                                 (w) == DMA_TRANSFER_WIDTH_16_BITS || \
                                 (w) == DMA_TRANSFER_WIDTH_32_BITS || \
                                 (w) == DMA_TRANSFER_WIDTH_64_BITS || \
                                 (w) == DMA_TRANSFER_WIDTH_128_BITS ||\
                                 (w) == DMA_TRANSFER_WIDTH_256_BITS)
#define IS_DMA_SINCDINC(inc)    ((inc) == DMA_ADDRESS_INCREMENT || (inc) == DMA_ADDRESS_DECREMENT || (inc) == DMA_ADDRESS_UNCHANGE)
#define IS_DMA_SRCDST_MSIZE(s)  ((s) == DMA_BURST_TRANSACTION_LENGTH_1 || \
                                 (s) == DMA_BURST_TRANSACTION_LENGTH_4 || \
                                 (s) == DMA_BURST_TRANSACTION_LENGTH_8 || \
                                 (s) == DMA_BURST_TRANSACTION_LENGTH_16 || \
                                 (s) == DMA_BURST_TRANSACTION_LENGTH_32 || \
                                 (s) == DMA_BURST_TRANSACTION_LENGTH_64 || \
                                 (s) == DMA_BURST_TRANSACTION_LENGTH_128 || \
                                 (s) == DMA_BURST_TRANSACTION_LENGTH_256)
#define IS_DMA_TT_FC(tt_fc)     ((tt_fc) == DMA_TRANS_TYPE_MEMORY_TO_MEMORY_DMAC_FLOW_CONTROLLER || \
                                 (tt_fc) == DMA_TRANS_TYPE_MEMORY_TO_PERIPHERAL_DMAC_FLOW_CONTROLLER || \
                                 (tt_fc) == DMA_TRANS_TYPE_PERIPHERAL_TO_MEMORY_DMAC_FLOW_CONTROLLER)
#define IS_DMA_DEV_TYPE(dev)    ((dev) == DMA_DEVICE_QSPI_RX || \
                                 (dev) == DMA_DEVICE_QSPI_TX || \
                                 (dev) == DMA_DEVICE_UART0_TX || \
                                 (dev) == DMA_DEVICE_UART0_RX || \
                                 (dev) == DMA_DEVICE_UART1_TX || \
                                 (dev) == DMA_DEVICE_UART1_RX || \
                                 (dev) == DMA_DEVICE_SPIM_TX || \
                                 (dev) == DMA_DEVICE_SPIM_RX || \
                                 (dev) == DMA_DEVICE_SPIS_TX || \
                                 (dev) == DMA_DEVICE_SPIS_RX || \
                                 (dev) == DMA_DEVICE_I2C0_TX || \
                                 (dev) == DMA_DEVICE_I2C0_RX || \
                                 (dev) == DMA_DEVICE_I2C1_TX || \
                                 (dev) == DMA_DEVICE_I2C1_RX || \
                                 (dev) == DMA_DEVICE_AUXADC)
#define IS_DMA_INT_STATUS(st)   ((st) == DMA_INT_STATUS_TFR || \
                                 (st) == DMA_INT_STATUS_BLOCK || \
                                 (st) == DMA_INT_STATUS_SRCTRAN || \
                                 (st) == DMA_INT_STATUS_DSTTRAN || \
                                 (st) == DMA_INT_STATUS_ERR)


/******************************  Function Declaration  **********************/

/**
 * @brief DMA init.
 *
 * @param chanIndex
 * @param config
 */
void HAL_DMA_Init(DMA_Channel chanIndex, DMA_InitTypeDef *config);

/**
 * @brief DMA Deinit.
 *
 * @param chanIndex
 */

void HAL_DMA_DeInit(DMA_Channel chanIndex);

/**
 * @brief DMA config.
 *
 * @param chanIndex
 * @param config
 */
void HAL_DMA_Config(DMA_Channel chanIndex, DMA_InitTypeDef *config);

/**
 * @brief Start DMA transfer.
 *
 * @param chanIndex DMA channel.
 * @param src Source address.
 * @param dst Destination address.
 * @param length data length.
 */
void HAL_DMA_StartTransfer(DMA_Channel chanIndex, void *src, void *dst, uint32_t length);

/**
 * @brief Wait for DMA transfer to finish.
 *
 * @param chanIndex DMA channel.
 */
void HAL_DMA_WaitDone(DMA_Channel chanIndex);

/**
 * @brief Enable/disalbe DMA controller, which must be done before
 * any channel activity can begin.
 *
 * @param enable @see DMAC_Enable.
 */
void HAL_DMA_GlobalEnable(DMAC_Enable en);

/**
 * @brief Software can poll the channel bits to determine when these channl bits
 * are free for a new DMA transfer.
 *
 * @return uint8_t ORed value from these enabled channels.
 */
uint8_t HAL_DMA_GetBusyChannel(void);

/**
 * @brief Interrupt events are stored in these Raw Interrupt Status registers before masking.
 * Each Raw Interrupt Status register has a bit allocated per channel; for example,
 * RawTfr[2] is the Channel 2 raw transfer complete interrupt.
 *
 * @return uint8_t
 */
uint8_t HAL_DMA_RawStatusTfr(void);
uint8_t HAL_DMA_RawStatusBlock(void);
uint8_t HAL_DMA_RawStatusSrcTran(void);
uint8_t HAL_DMA_RawStatusDstTran(void);
uint8_t HAL_DMA_RawStatusErr(void);

/**
 * @brief All interrupt events from all channles are stored in these Interrupt
 * Status registers after masking.
 * Each Interrupt Status register has a bit allocated per channel: for example,
 * StatusTfr[2] is the Channel 2 status transfer complete interrupt.
 *
 * @return uint8_t
 */
uint8_t HAL_DMA_StatusTfr(void);
uint8_t HAL_DMA_StatusBlock(void);
uint8_t HAL_DMA_StatusSrcTran(void);
uint8_t HAL_DMA_StatusDstTran(void);
uint8_t HAL_DMA_StatusErr(void);

/**
 * @brief The contents of the Raw Status registers are masked with the contents of the Mask registers.
 * Each Interrupt Mask register has a bit allocated per channel: for example, MaksTfr[2] is the mask
 * bit for the Channel 2 transfer complete interrupt.
 * @param chanIndex
 */
void HAL_DMA_UnMaskTfr(DMA_Channel chanIndex);
void HAL_DMA_UnMaskBlock(DMA_Channel chanIndex);
void HAL_DMA_UnMaskSrcTran(DMA_Channel chanIndex);
void HAL_DMA_UnMaskDstTran(DMA_Channel chanIndex);
void HAL_DMA_UnMaskErr(DMA_Channel chanIndex);

/**
 * @brief Each Interrupt Clear register has a bit allocated per channel; for example, ClearTfr[2]
 * is the clear bit for the Channel 2 transfer complete interrupt.
 *
 * @param chanIndex
 */
void HAL_DMA_ClearTfr(DMA_Channel chanIndex);
void HAL_DMA_ClearBlock(DMA_Channel chanIndex);
void HAL_DMA_ClearSrcTran(DMA_Channel chanIndex);
void HAL_DMA_ClearDstTran(DMA_Channel chanIndex);
void HAL_DMA_ClearErr(DMA_Channel chanIndex);

/**
 * @brief Combined Interrupt Status Register.
 *
 * @return uint8_t The contents of each of teh five status registers is ORed to produce a single bit for
 * each interrupt type in the Combined Status register.
 */
uint8_t HAL_DMA_IntStatus(void);

// TODO: add annotation here.
void HAL_DMA_SWSrcTranRequest(DMA_Channel chanIndex);

uint8_t HAL_DMA_SWSrcTranRead(void);

void HAL_DMA_SWDstTranRequest(DMA_Channel chanIndex);

uint8_t HAL_DMA_SWDstTranRead(void);

void HAL_DMA_SingleSrcTranRequest(DMA_Channel chanIndex);

uint8_t HAL_DMA_SingleSrcTranRead(void);

void HAL_DMA_SingleDstTranRequest(DMA_Channel chanIndex);

uint8_t HAL_DMA_SingleDstTranRead(void);

void HAL_DMA_LastSrcTranRequest(DMA_Channel chanIndex);

uint8_t HAL_DMA_LastSrcTranRead(void);

void HAL_DMA_LastDstTranRequest(DMA_Channel chanIndex);

uint8_t HAL_DMA_LastDstTranRead(void);




#ifdef __cplusplus
    }
#endif // __cplusplus
#endif // __HAL_DMA_H_
