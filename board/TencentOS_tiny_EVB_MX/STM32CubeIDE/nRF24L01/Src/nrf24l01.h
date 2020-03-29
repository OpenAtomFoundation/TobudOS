#ifndef NRF24L01_H_
#define NRF24L01_H_

#define REG_CONFIG 0x00
    // 屏蔽因RX_DR触发的中断
    // 0 不屏蔽， 1 屏蔽
    #define MASK_RX_DR  6
    // 屏蔽因TX_DS触发的中断
    // 0 不屏蔽， 1 屏蔽
    #define MASK_TX_DS  5
    // 屏蔽因MAX_RT触发的中断
    // 0 不屏蔽， 1 屏蔽
    #define MASK_MAX_RT 4
    // 启用CRC
    // 如果EN_AA有一个比特为1，就会被强制置1
    #define EN_CRC      3
    // CRC编码方式
    // 0 一字节，1 二字节
    #define CRCO        2
    // 0 POWER DOWN, 1 POWER UP
    #define PWR_UP      1
    // 收发控制
    // 0 PTX, 1 PRX
    #define PRIM_RX     0


#define REG_EN_AA 0x01
    // Enable相应data pipe的自动Ack
    #define ENAA_P5     5
    #define ENAA_P4     4
    #define ENAA_P3     3
    #define ENAA_P2     2
    #define ENAA_P1     1
    #define ENAA_P0     0


#define REG_EN_RXADDR 0x02
    // Enable相应的data pipe
    #define ERX_P5      5
    #define ERX_P4      4
    #define ERX_P3      3
    #define ERX_P2      2
    #define ERX_P1      1
    #define ERX_P0      0


#define REG_SETUP_AW  0x03
    // RX、TX地址宽度, AW占两个比特位
    // 00 illegal
    // 01 3字节
    // 10 4字节
    // 11 5字节
    #define AW          0


#define REG_SETUP_RETR 0x04
    // ARD, Auto Retransmit Delay占7:4共4个比特位
    // 0000 250us
    // 0001 500us
    // 0010 750us
    // ...
    // 1111 4000us
    #define ARD         4

    // ARC, Auto Retransmit Count占3:0共4个比特
    // 0000 Re-Transmit disabled
    // 0001 最多重试一次
    // ...
    // 1111 最多重度15次
    #define ARC         0


// RF Channel选择
// 占6:0共7个比特
#define REG_RF_CH 0x05


#define REG_RF_SETUP 0x06
    #define PLL_LOCK    4 // 仅用在测试
    // Air Data Rate
    // 0 1Mbps
    // 1 2Mbps
	// 2 250Kbps
    #define RF_DR       3
		#define NRF_1Mbps	0
		#define NRF_2Mbps	1
		#define NRF_250Kbps	2

    // PWR, 占2:1共2个比特
    // 00 -18dBm
    // 01 -12dBm
    // 10  -6dBm
    // 11   0dBm
    #define RF_PWR      1
    // Non-P omissions
    #define LNA_HCURR   0

#define REG_STATUS 0x07
    // RX FIFO数据READY中断标记位
    // 写1清除此位
    #define RX_DR       6
    // 数据已发送中断标记位
    // 如果启用AUTO_ACK，则此位仅在收到ACK后置1
    // 写1清除此位
    #define TX_DS       5
    // 达到最大重发次数中断标记位
    // 为了继续使用通信功能，此位必需被清0
    // 写1清除此位
    #define MAX_RT      4
    // RX_P_NO 占用3:1共3个比特位
    // RX_FIFIO中数据来自哪个data pipe的number
    // 000-101 Data Pipe Number
    // 110 未用
    // 111 RX FIFO空
    #define RX_P_NO     1
    // TX FIFO满标志
    // 0 TX FIFO中还有空间
    // 1 满
    #define TX_FULL     0

#define REG_OBSERVE_TX 0x08
    // 占7:4共4个比特位，丢包计数
    #define PLOS_CNT  4
    // 占3:0共4个比特位，重传计数
    #define ARC_CNT   0

// RSSI, Carrier Detect
// 最低比特位有效(0 接收信号小于-60dBm)
#define REG_RSSI 0x09

#define REG_RX_ADDR_P0 0x0A  // 39:0 默认0xE7E7E7E7E7
#define REG_RX_ADDR_P1 0x0B  // 39:0 默认0xC2C2C2C2C2
#define REG_RX_ADDR_P2 0x0C  //  7:0 默认0xC3
#define REG_RX_ADDR_P3 0x0D  //  7:0 默认0xC4
#define REG_RX_ADDR_P4 0x0E  //  7:0 默认0xC5
#define REG_RX_ADDR_P5 0x0F  //  7:0 默认0xC6
#define REG_TX_ADDR    0x10  // 39:0 默认0xE7E7E7E7E7


// 以下是一组表示各通道的静态负载长度
// 都是5:0为有效比特位
// 0 表示pipe未使用
// 1 1byte
// ...
// 32 32bytes
#define REG_RX_PW_P0 0x11
#define REG_RX_PW_P1 0x12
#define REG_RX_PW_P2 0x13
#define REG_RX_PW_P3 0x14
#define REG_RX_PW_P4 0x15
#define REG_RX_PW_P5 0x16


#define REG_FIFO_STATUS 0x17
    // 如果置1则重用上次传输的数据包
    // 只要CE为1，数据包就会不断地被重发
    // SPI命令REUSE_TX_PL对此位置1
    // SPI命令W_TX_PAYLOAD或FLUSH_TX对此位清0
    #define TX_REUSE    6
    // 0 TX FIFO尚有空间
    // 1 TX FIFO已满
    #define TX_FIFO_FULL    5
    // 0 TX FIFO中有数据
    // 1 TX FIFO中没有数据
    #define TX_FIFIO_EMPTY  4
    // 0 RX FIFO尚有空间
    // 1 RX FIFO已满
    #define RX_FIFO_FULL    1
    // 0 RX FIFO中有数据
    // 1 RX FIFO中没有数据
    #define RX_FIFO_EMPTY   0


// 启用相应data pip的动态负载长度
// 需要启用 EN_DPL 和 ENAA_Pn
#define REG_DYNPD 0x1C
    #define DPL_P5      5
    #define DPL_P4      4
    #define DPL_P3      3
    #define DPL_P2      2
    #define DPL_P1      1
    #define DPL_P0      0

#define REG_FEATURE 0x1D
    // Enable Dynamic Payload Length
    #define EN_DPL      2
    // Enable Payload with ACK
    #define EN_ACK_PAY  1
    // Enable W_TX_PAYLOAD_NOACK 命令
    #define EN_DYN_ACK  0


#define REGISTER_MASK 0x1F
#define CMD_R_REGISTER    0x00
#define CMD_W_REGISTER    0x20

#define CMD_R_RX_PAYLOAD  0b01100001
#define CMD_W_TX_PAYLOAD  0b10100000
#define CMD_FLUSH_TX      0b11100001
#define CMD_FLUSH_RX      0b11100010
#define CMD_REUSE_TX_PL   0b11100011
#define CMD_ACTIVATE      0b01010000
#define CMD_R_RX_PL_WID   0b01100000
#define CMD_W_ACK_PAYLOAD 0b10101000
#define CMD_W_TX_PAYLOAD_NOACK  0b10110000
#define CMD_NOP           0b11111111


#define _BV(n) (1<<(n))

void nrf_powerup();

void nrf_init();

uint8_t nrf_hal_test();


#endif /* NRF24L01_H_ */
