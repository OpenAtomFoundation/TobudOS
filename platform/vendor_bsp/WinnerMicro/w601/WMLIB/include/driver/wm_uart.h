/**
 * @file    wm_uart.h
 *
 * @brief   uart Driver Module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_UART_H
#define WM_UART_H
#include "list.h"
//#include "wm_regs.h"
#include "wm_type_def.h"
//#include "wm_osal.h"

#define TLS_UART_RX_BUF_SIZE   4096
#define TLS_UART_TX_BUF_SIZE   4096
#define WAKEUP_CHARS           256

#define MBOX_MSG_UART_RX       1
#define MBOX_MSG_UART_TX       2


/** baud rate definition */
#define UART_BAUDRATE_B600          600
#define UART_BAUDRATE_B1200	        1200
#define UART_BAUDRATE_B1800         1800
#define UART_BAUDRATE_B2400         2400
#define UART_BAUDRATE_B4800         4800
#define UART_BAUDRATE_B9600         9600
#define UART_BAUDRATE_B19200        19200
#define UART_BAUDRATE_B38400        38400
#define UART_BAUDRATE_B57600        57600
#define UART_BAUDRATE_B115200       115200
#define UART_BAUDRATE_B230400       230400
#define UART_BAUDRATE_B460800       460800
#define UART_BAUDRATE_B921600      921600
#define UART_BAUDRATE_B1000000      1000000
#define UART_BAUDRATE_B1250000      1250000
#define UART_BAUDRATE_B1500000      1500000
#define UART_BAUDRATE_B2000000      2000000

#define UART_RX_INT_FLAG (UIS_RX_FIFO | UIS_RX_FIFO_TIMEOUT | UIS_BREAK |\
        UIS_OVERRUN | UIS_FRM_ERR | UIS_PARITY_ERR)
#define UART_RX_ERR_INT_FLAG (UIS_BREAK | UIS_FRM_ERR | \
        UIS_PARITY_ERR)

#define UART_TX_INT_FLAG (UIS_TX_FIFO | UIS_TX_FIFO_EMPTY)


#ifndef UART0_INT
#define UART0_INT                   (23UL)
#endif
#ifndef UART1_INT
#define UART1_INT                   (24UL)
#endif

/** return count in buffer.  */
#define CIRC_CNT(head,tail,size) (((head) - (tail)) & ((size)-1))

/** Return space available, 0..size-1.  We always leave one free char
   as a completely full buffer has head == tail, which is the same as
   empty.  */
#define CIRC_SPACE(head,tail,size) CIRC_CNT((tail),((head)+1),(size))

/** Return count up to the end of the buffer.  Carefully avoid
   accessing head and tail more than once, so they can change
   underneath us without returning inconsistent results.  */
#define CIRC_CNT_TO_END(head,tail,size) \
	({int end = (size) - (tail); \
	  int n = ((head) + end) & ((size)-1); \
	  n < end ? n : end;})

/** Return space available up to the end of the buffer.  */
#define CIRC_SPACE_TO_END(head,tail,size) \
	({int end = (size) - 1 - (head); \
	  int n = (end + (tail)) & ((size)-1); \
	  n <= end ? n : end+1;})

#define CIRC_SPACE_TO_END_FULL(head,tail,size) \
	({int end = (size) - 1 - (head); \
	  int n = (end + (tail)) & ((size)-1); \
	  n < end ? n : end+1;})

#define uart_circ_empty(circ)		((circ)->head == (circ)->tail)
#define uart_circ_chars_pending(circ)	\
	(CIRC_CNT((circ)->head, (circ)->tail, TLS_UART_TX_BUF_SIZE))

/**
 * @struct tls_uart_baud_rate    baudrate define
 */
struct tls_uart_baud_rate
{
    u32 baud_rate;
    u16 ubdiv;
    u16 ubdiv_frac;
};


/**
 * @enum    uart number enum
 */
enum
{
    TLS_UART_0 = 0,
    TLS_UART_1 = 1,
	TLS_UART_2 = 2,
};


/**
 * @typedef enum TLS_UART_PMODE    Parity Mode
 */
typedef enum TLS_UART_PMODE
{
    TLS_UART_PMODE_DISABLED = 0,    /**< No Parity */
    TLS_UART_PMODE_ODD = 1,     /**< Odd Parity */
    TLS_UART_PMODE_EVEN = 2,    /**< Even Parity */
    TLS_UART_PMODE_MARK = 3,    /**< The parity bit is always 1. */
    TLS_UART_PMODE_SPACE = 4,   /**< The parity bit is always 0. */
} TLS_UART_PMODE_T;

/**
 * @typedef enum TLS_UART_CHSIZE    Character Size
 */
typedef enum TLS_UART_CHSIZE
{
    TLS_UART_CHSIZE_5BIT = (0x00 << 0), /**< Character size: 5 bit */
    TLS_UART_CHSIZE_6BIT = (0x01 << 0), /**< Character size: 6 bit */
    TLS_UART_CHSIZE_7BIT = (0x02 << 0), /**< Character size: 7 bit */
    TLS_UART_CHSIZE_8BIT = (0x03 << 0), /**< Character size: 8 bit */
} TLS_UART_CHSIZE_T;

/**
 * @typedef enum TLS_UART_FLOW_CTRL_MODE    flow control mode
 */
typedef enum TLS_UART_FLOW_CTRL_MODE
{
    TLS_UART_FLOW_CTRL_NONE,
    TLS_UART_FLOW_CTRL_HARDWARE,
} TLS_UART_FLOW_CTRL_MODE_T;

/**
 * @typedef enum TLS_UART_RX_FLOW_CTRL_FLAG    flow control rx flag
 */
typedef enum TLS_UART_RX_FLOW_CTRL_FLAG
{
    TLS_UART_RX_DISABLE,
    TLS_UART_RX_ENABLE,
} TLS_UART_RX_FLOW_CTRL_FLAG_T;

/**
 * @typedef enum TLS_UART_STOPBITS
 */
typedef enum TLS_UART_STOPBITS
{
    TLS_UART_ONE_STOPBITS,
    TLS_UART_TWO_STOPBITS,
} TLS_UART_STOPBITS_T;


/**
 * @typedef enum TLS_UART_STATUS
 */
typedef enum TLS_UART_STATUS
{
    TLS_UART_STATUS_OK,
    TLS_UART_STATUS_ERROR,
} TLS_UART_STATUS_T;


/**
 * @typedef enum TLS_UART_MODE   operation mode
 */
typedef enum TLS_UART_MODE
{
    TLS_UART_MODE_POLL,         /**< uart operation mode: poll */
    TLS_UART_MODE_INT,          /**< uart operation mode: interrupt mode */
} TLS_UART_MODE_T;

/**
 * @struct tls_uart_icount
 */
struct tls_uart_icount
{
    u32 cts;
    u32 dsr;
    u32 rng;
    u32 dcd;
    u32 rx;
    u32 tx;
    u32 frame;
    u32 overrun;
    u32 parity;
    u32 brk;
    u32 buf_overrun;
};



/**
 * @typedef struct tls_uart_options
 */
typedef struct tls_uart_options
{
    u32 baudrate;    /**< Set baud rate of the UART */

    TLS_UART_CHSIZE_T charlength;   /**< Number of bits to transmit as a character (5 to 8). */

    TLS_UART_PMODE_T paritytype;    /**< Parity type */

    TLS_UART_FLOW_CTRL_MODE_T flow_ctrl;    /**< Flow control type */

    TLS_UART_STOPBITS_T stopbits;    /**< Number of stop bits */

} tls_uart_options_t;


/**
 * @typedef struct tls_uart_circ_buf
 */
typedef struct tls_uart_circ_buf
{
    u8 *buf;
    u32 head;
    u32 tail;
} tls_uart_circ_buf_t;

/**
 * @typedef struct TLS_UART_REGS
 */
typedef struct TLS_UART_REGS
{
    u32 UR_LC;                       /**< line control register */
    u32 UR_FC;                       /**<  flow control register */
    u32 UR_DMAC;                  /**< dma control register */
    u32 UR_FIFOC;                  /**< fifo control register */
    u32 UR_BD;                       /**< baud rate register */
    u32 UR_INTM;                   /**< interrupt mask register */
    u32 UR_INTS;                    /**< interrupt source register */
    u32 UR_FIFOS;                  /**< fifo status register */
    u32 UR_TXW;                    /**< tx windows register */
    u32 UR_RES0;
    u32 UR_RES1;
    u32 UR_RES2;
    u32 UR_RXW;                     /**< rx windows register */
} TLS_UART_REGS_T;


/**
 * @typedef struct tls_uart_port
 */
typedef struct tls_uart_port
{
    u32 uart_no;                    /**< uart number: 0 or 1 */

    u32 uart_irq_no;             /**< uart interrupt number */

    u32 plus_char_cnt;

    TLS_UART_MODE_T uart_mode;      /**< uart work mode: interrupt mode or poll mode */

    struct tls_uart_options opts;       /**< uart config parameters */

    int fcStatus;                           /**< flow ctrl status,0 closed ,1 opened */

    enum TLS_UART_RX_FLOW_CTRL_FLAG rxstatus;

    u32 tx_fifofull;                    /**< uart tx fifo trigger level */

    TLS_UART_REGS_T volatile *regs;     /**< uart registers struct pointer */

    struct tls_uart_icount icount;          /**< uart statistics information */

    struct tls_uart_circ_buf recv;          /**< uart ring buffer */

// struct tls_uart_circ_buf xmit;

    struct dl_list tx_msg_pending_list;

    struct dl_list tx_msg_to_be_freed_list;

    u8 hw_stopped;

    //tls_os_sem_t *tx_sem;

    char *buf_ptr;

    u16 buf_len;

    s16(*rx_callback) (u16 len);

    s16(*tx_callback) (struct tls_uart_port * port);

    bool tx_dma_on;
} tls_uart_port_t;

/**
 * @typedef struct tls_uart_tx_msg
 */
typedef struct tls_uart_tx_msg
{
    struct dl_list list;
    char *buf;
    u16 buflen;
    u16 offset;
    void (*finish_callback) (void *arg);
    void *callback_arg;
} tls_uart_tx_msg_t;

/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup UART_Driver_APIs UART Driver APIs
 * @brief UART driver APIs
 */

/**
 * @addtogroup UART_Driver_APIs
 * @{
 */

/**
 * @brief          This function is used to initial uart port.
 *
 * @param[in]      uart_no  is the uart number. TLS_UART_0 or TLS_UART_1
 * @param[in]      opts       is the uart setting options,if this param is NULL,this function will use the default options.
 *
 * @retval         WM_SUCCESS     success
 * @retval         WM_FAILED        failed
 *
 * @note           When the system is initialized, the function has been called, so users can not call the function.
 */
int tls_uart_port_init(u16 uart_no, tls_uart_options_t * opts, u8 modeChoose);


/**
 * @brief          This function is used to register uart rx interrupt.
 *
 * @param[in]      uart_no      TLS_UART_0 or TLS_UART_1
 * @param[in]      rx_callback  is the uart rx interrupt call back function.
 *
 * @return         None
 *
 * @note           None
 */
void tls_uart_rx_callback_register(u16 uart_no, s16(*rx_callback) (u16 len));


/**
 * @brief          This function is used to copy circular buffer data to user buffer.
 *
 * @param[in]      uart_no    is the uart numer
 * @param[in]      buf          is the user buffer
 * @param[in]      readsize   is the user read size
 *
 * @retval         copy data size
 *
 * @note           None
 */
int tls_uart_read(u16 uart_no, u8 * buf, u16 readsize);


/**
 * @brief          This function is used to transfer data synchronously.
 *
 * @param[in]      uart_no      is the uart number
 * @param[in]      buf            is a buf for saving user data
 * @param[in]      writesize    is the user data length
 *
 * @retval         WM_SUCCESS    tx success
 * @retval         WM_FAILED       tx failed
 *
 * @note           None
 */
int tls_uart_write(u16 uart_no, char *buf, u16 writesize);


/**
 * @brief          This function is used to transfer data with DMA.
 *
 * @param[in]      buf                is a buf for saving user data
 * @param[in]      writesize        is the user data length
 * @param[in]      cmpl_callback  function point,when the transfer is completed, the function will be called.
 *
 * @retval         WM_SUCCESS    success
 * @retval         WM_FAILED       failed
 *
 * @note           Only uart1 support DMA transfer.
 */
int tls_uart_dma_write(char *buf, u16 writesize, void (*cmpl_callback) (void *p), u16 uart_no);


/**
 * @brief          This function is used to set uart parity.
 *
 * @param[in]      uart_no      is the uart number
 * @param[in]      paritytype   is a parity type defined in TLS_UART_PMODE_T
 *
 * @retval         WM_SUCCESS	if setting success
 * @retval         WM_FAILED	 	if setting fail
 *
 * @note           None
 */
int tls_uart_set_parity(u16 uart_no, TLS_UART_PMODE_T paritytype);


/**
 * @brief          This function is used to set uart baudrate.
 *
 * @param[in]      uart_no     is the uart number
 * @param[in]      baudrate    is the baudrate user want used,the unit is HZ.
 *
 * @retval         WM_SUCCESS	if setting success
 * @retval         WM_FAILED 	if setting fail
 *
 * @note           None
 */
int tls_uart_set_baud_rate(u16 uart_no, u32 baudrate);

/**
 * @brief          This function is used to set uart stop bits.
 *
 * @param[in]      uart_no     is the uart number
 * @param[in]      stopbits    is a stop bit type defined in TLS_UART_STOPBITS_T
 *
 * @retval         WM_SUCCESS	if setting success
 * @retval         WM_FAILED	if setting fail
 *
 * @note           None
 */
int tls_uart_set_stop_bits(u16 uart_no, TLS_UART_STOPBITS_T stopbits);

/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_UART_H */
