#ifndef NRF24L01_H_
#define NRF24L01_H_

#include "stdint.h"

typedef struct {
    int  (*init)(void *private);
    void (*csn)(uint8_t mode);
    void (*ce)(uint8_t mode);
    void (*spi_send)(uint8_t *buf, uint8_t len);
    void (*spi_recv)(uint8_t *buf, uint8_t len);
    void *private;
} nrf_init_t;

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
	#define AW_3BYTES	1
	#define AW_4BYTES	2
	#define AW_5BYTES	3



#define REG_SETUP_RETR 0x04
    // ARD, Auto Retransmit Delay占7:4共4个比特位
    // 0000 250us
    // 0001 500us
    // 0010 750us
    // ...
    // 1111 4000us
    #define ARD         4
    #define ARD_250us	0
    #define ARD_500us	1
    #define ARD_750us   2
    #define ARD_1000us	3
    #define ARD_1250us	4
    #define ARD_1500us	5
    #define ARD_1750us	6
    #define ARD_2000us	7
    #define ARD_2250us	8
    #define ARD_2500us	9
    #define ARD_2750us	10
    #define ARD_3000us	11
    #define ARD_3250us  12
    #define ARD_3500us	13
    #define ARD_3750us	14
    #define ARD_4000us	15

    // ARC, Auto Retransmit Count占3:0共4个比特
    // 0000 Re-Transmit disabled
    // 0001 最多重试一次
    // ...
    // 1111 最多重度15次
    #define ARC         0
	#define ARC_0		0
	#define ARC_1		1
	#define ARC_2		2
	#define ARC_3		3
	#define ARC_4		4
	#define ARC_5		5
	#define ARC_6		6
	#define ARC_7		7
	#define ARC_8		8
	#define ARC_9		9
	#define ARC_10		10
	#define ARC_11		11
	#define ARC_12		12
	#define ARC_13		13
	#define ARC_14		14
	#define ARC_15		15


// RF Channel选择
// 占6:0共7个比特
#define REG_RF_CH 0x05


#define REG_RF_SETUP 0x06
    #define PLL_LOCK    4 // 仅用在测试

    // Air Data Rate
    // 0 1Mbps
    // 1 2Mbps
    #define RF_DR       3
		#define NRF_1Mbps	0
		#define NRF_2Mbps	1
        // 对于250Kbps的配置需要特别注意
        // 因为nRF24L01P(也就是nRF24L01+)和nRF24L01在这里不一样
        // nRF24L01P只定义了第3比特位的RF_DR，0为1Mbps,1为2Mbps
        // nRF24L01则定义了第3比特位的RF_DR_HIGH和第5比特位的RF_DR_LOW，当RF_DR_HIGH==0 && RF_DR_LOW == 1时设置为250Kbps
        // 然而nRF24L01的第5比特位只允许为0，所以无法发送250Kbps
        // 因此为了避免两个不同的产品在250Kbps不能通信带来的疑惑和调式程序的麻烦，本程序直接删除了NRF_250Kbps的定义
        // 如果确实想用250Kbps，可以自己手动设置


    // 仅用在nRF24L01P芯片上
    #define RF_DR_LOW   5
    #define RF_DR_HIGH  3

    // PWR, 占2:1共2个比特，仅nRF24L01P、nRF24L01有效
    // 00 -18dBm
    // 01 -12dBm
    // 10  -6dBm
    // 11   0dBm
    #define RF_PWR      1
		#define RF_PWR_n18dBm	0
		#define RF_PWR_n12dBm   1
		#define RF_PWR_n6dBm	2
		#define RF_PWR_0dBm		3

    // 仅nRF24L01有效，nRF24L01P没有这位定义
    #define LNA_HCURR   0

    // 在SI24R1中，针对nRF24L01P没有使用这一位就直接拿来扩充RF_PWR的位数了
    // 因此在SI24R1中 RF_PWR == 0 占用0:2共3个比特位
    // 与nRF24L01P的对应关系如下
    // [2:0]    SI24R1      nRF24L01P
    //  000     -12dBm      -18dBm
    //  001      -6dBm
    //  010      -4dBm      -12dBm
    //  011       0dBm
    //  100       1dBm       -6dBm
    //  101       3dBm
    //  110       4dBm        0dBm
    //  111       7dBm
    // 以下定义仅SI24R1芯片有效
    #define SI24R1_PWR   0
        #define SI24R1_PWR_n12dBm   0
        #define SI24R1_PWR_n6dBm    1
        #define SI24R1_PWR_n4dBm    2
        #define SI24R1_PWR_0dBm     3
        #define SI24R1_PWR_1dBm     4
        #define SI24R1_PWR_3dBm     5
        #define SI24R1_PWR_4dBm     6
        #define SI24R1_PWR_7dBm     7


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

#define CMD_R_RX_PAYLOAD        0b01100001
#define CMD_W_TX_PAYLOAD        0b10100000
#define CMD_FLUSH_TX            0b11100001
#define CMD_FLUSH_RX            0b11100010
#define CMD_REUSE_TX_PL         0b11100011
#define CMD_ACTIVATE            0b01010000
#define CMD_R_RX_PL_WID         0b01100000
#define CMD_W_ACK_PAYLOAD       0b10101000
#define CMD_W_TX_PAYLOAD_NOACK  0b10110000
#define CMD_NOP                 0b11111111


#define _BV(n) (1<<(n))
#define _VV(v, n) ((v)<<(n))


int nrf_init(nrf_init_t *ni);

void nrf_flush_rx();

void nrf_flush_tx();

void nrf_delay(uint32_t delay);

int nrf_powerup();

int nrf_powerdown();

void nrf_enable_rx_irq();

void nrf_disable_rx_irq();

void nrf_enable_tx_irq();

void nrf_disable_tx_irq();

void nrf_enable_max_rt_irq();

void nrf_disable_max_rt_irq();

void nrf_set_rf_channel(uint8_t channel);

int nrf_set_rxaddr(uint8_t pipe, uint8_t *addr, uint8_t addrlen);

int nrf_get_rxaddr(uint8_t pipe, uint8_t *addr, uint8_t *addrlen);

int nrf_set_txaddr(uint8_t *addr, uint8_t addrlen);

int nrf_get_txaddr(uint8_t *addr, uint8_t *addrlen);

int nrf_enable_rxaddr(uint8_t pipe);

void nrf_reset_registers();

void nrf_set_standby_mode();

void nrf_set_receive_mode();

void nrf_set_send_mode();

void nrf_enable_autoack(uint8_t pipe);

void nrf_disable_autoack(uint8_t pipe);

void nrf_set_datarate(uint8_t dr);

int nrf_enable_dynamic_payload(uint8_t pipe);

int nrf_read_payload(uint8_t *buf, uint8_t *len, uint8_t *pipe);

int nrf_poll_read_payload(uint8_t *buf, uint8_t *len, uint8_t *pipe);

int nrf_write_payload(uint8_t *buf, uint8_t len);

void nrf_ce(uint8_t mode);

void nrf_csn(uint8_t mode);



int nrf_read_reg(uint8_t reg, uint8_t *buf, uint8_t len);
int nrf_read_reg_byte(uint8_t reg, uint8_t *v);
int nrf_write_reg(uint8_t reg, uint8_t *buf, uint8_t len);
int nrf_write_reg_byte(uint8_t reg, uint8_t byte);
int nrf_clear_reg_bit(uint8_t reg, uint8_t bit);
int nrf_set_reg_bit(uint8_t reg, uint8_t bit);

int nrf_cmd_read(uint8_t cmd, uint8_t *data, uint8_t len);
int nrf_cmd_read_byte(uint8_t cmd, uint8_t *data);
int nrf_write_cmd_and_data(uint8_t cmd, uint8_t *data, uint8_t len);
int nrf_write_cmd(uint8_t cmd);
#endif /* NRF24L01_H_ */
