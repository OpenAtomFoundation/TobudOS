#include "types.h"
#include "ln88xx.h"
#include "hal/hal_gpio.h"
#include "hal/simi2c.h"

#define I2C_NOACK   1
#define I2C_ACK     0

///////////////////////////  SCL, SDA Pin Configuration  //////////////////////

// 这两个GPIO模拟I2C是OK的
// 大板子丝印34，对应 GPIOB_11，配置为 SCL
// 大板子丝印33，对应 GPIOB_10，配置为 SDA
// #define SCL_PIN     GPIOB_11
// #define SDA_PIN     GPIOB_10

// 这两个GPIO模拟I2C是OK的
// 大板子丝印22，对应 GPIOB_13，配置为 SCL
// 大板子丝印21，对应 GPIOB_12，配置为 SDA
#define SCL_PIN     GPIOB_6
#define SDA_PIN     GPIOB_7



// 这两个GPIO模拟I2C是OK的
// 大板子丝印16，对应 GPIOA_20，配置为 SCL
// 大板子丝印15，对应 GPIOA_14，配置为 SDA
// #define SCL_PIN     GPIOA_20
// #define SDA_PIN     GPIOA_14

// 这两个GPIO模拟I2C是OK的, 恰好可以点亮 cloud, identity 两个灯
// 大板子丝印14，对应 GPIOA_13，配置为 SCL
// 大板子丝印13，对应 GPIOA_12，配置为 SDA
// #define SCL_PIN     GPIOA_13
// #define SDA_PIN     GPIOA_12


static void __switch_sda_output(void);
static void __switch_sda_input(void);
static uint8_t __sda_read(void);

#define I2C_SCL_HIGH    HAL_GPIO_WritePin(SCL_PIN, GPIO_VALUE_HIGH)
#define I2C_SCL_LOW     HAL_GPIO_WritePin(SCL_PIN, GPIO_VALUE_LOW)
#define I2C_SDA_HIGH    HAL_GPIO_WritePin(SDA_PIN, GPIO_VALUE_HIGH)
#define I2C_SDA_LOW     HAL_GPIO_WritePin(SDA_PIN, GPIO_VALUE_LOW)
#define I2C_SDA_READ    __sda_read()

/**
 * @brief 当前的配置I2C波特率为 112Kbps
 *
 * @param dly
 */
void I2C_Delay(uint16_t dly)
{
    uint16_t i = 0;
    for (i = dly*15; i > 0; i--);
}

/**
 * @brief 引脚初始化
 */
void I2C_Init(void)
{
    GPIO_InitTypeDef scl_config;
    GPIO_InitTypeDef sda_config;

    scl_config.dir = GPIO_OUTPUT;

    sda_config.dir = GPIO_OUTPUT;

    HAL_GPIO_Init(SCL_PIN, scl_config);
    HAL_GPIO_Init(SDA_PIN, sda_config);
    I2C_SCL_HIGH;
    I2C_SDA_HIGH;
    I2C_SDA_HIGH;
}

static void __switch_sda_output(void)
{
    GPIO_InitTypeDef sda_config;

    sda_config.dir = GPIO_OUTPUT;

    HAL_GPIO_Init(SDA_PIN, sda_config);
}

static void __switch_sda_input(void)
{
    GPIO_InitTypeDef sda_config;

    sda_config.dir = GPIO_INPUT;
    sda_config.debounce = GPIO_DEBOUNCE_YES;

    HAL_GPIO_Init(SDA_PIN, sda_config);
}

static uint8_t __sda_read(void)
{
    uint8_t value = 0;
    value = HAL_GPIO_ReadPin(SDA_PIN);
    return value;
}

/**
 * @brief 模拟起始信号
 */
void I2C_Start(void)
{
    I2C_SDA_HIGH;
    I2C_SCL_HIGH;
    I2C_Delay(10);

    I2C_Delay(10);
    I2C_SDA_LOW;
    I2C_Delay(10);

    // 钳住I2C总线，准备发送数据
    I2C_SCL_LOW;
    I2C_Delay(10);
}

/**
 * @brief 模拟结束信号
 */
void I2C_Stop(void)
{
    I2C_SCL_LOW;
    I2C_SDA_LOW;
    I2C_Delay(3);

    I2C_SCL_HIGH;
    I2C_Delay(10);

    I2C_SDA_HIGH;
    I2C_Delay(10);
}

// 仅作为 debug 用途，用来统计没有收到ACK的次数
static uint32_t ack_failed = 0;

/**
 * @brief 等待从机应答信号
 *
 * @return uint8_t
 */
uint8_t I2C_GetAck(void)
{
    uint8_t ack;

    I2C_SDA_HIGH;
    __switch_sda_input();
    I2C_Delay(10);

    I2C_SCL_HIGH;
    I2C_Delay(5);

    if (I2C_SDA_READ) {
        ack = I2C_NOACK;
    } else {
        ack = I2C_ACK;
    }
    __switch_sda_output();
    I2C_SCL_LOW;
    I2C_Delay(5);

    if (I2C_NOACK == ack) {
        ack_failed++;
    }
    return ack;
}

/**
 * @brief 主机产生应答信号
 *
 * @param ack 如果值为 I2C_ACK 则发送ACK；否则不回ACK。
 */
void I2C_PutAck(uint8_t ack)
{
    I2C_SCL_LOW;
    I2C_Delay(10);

    if (I2C_ACK == ack) {
        I2C_SDA_LOW;
    } else {
        I2C_SDA_HIGH;
    }
    I2C_Delay(10);

    I2C_SCL_HIGH;
    I2C_Delay(10);
    I2C_SCL_LOW;
    I2C_Delay(2);
}


// SCL 低电平时准备数据，在SCL为高电平时发送出去
void I2C_WriteByte(uint8_t data)
{
    uint8_t cnt = 0;

    for (cnt = 0; cnt < 8; cnt++) {
        I2C_SCL_LOW;
        I2C_Delay(5);

        if (data & 0x80) {
            I2C_SDA_HIGH;
        } else {
            I2C_SDA_LOW;
        }
        data <<= 1;
        I2C_Delay(5);

        I2C_SCL_HIGH;
        I2C_Delay(5);
    }

    I2C_SCL_LOW;
}


uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t cnt = 0;
    uint8_t data = 0;

    I2C_SCL_LOW;
    I2C_Delay(10);

    I2C_SDA_HIGH;

    for( cnt = 0; cnt < 8; cnt++) {
        I2C_SCL_HIGH;
        I2C_Delay(10);

        data <<= 1;
        if (I2C_SDA_READ) {
            data |= 0x01;
        }

        I2C_SCL_LOW;
        I2C_Delay(10);
    }

    return data;
}


//////////////////////////////////  OLED  /////////////////////////////////////

/**
 * @brief OLED device addr = 0x78
 */

#define OLED_DEV_ADDR       (0x78)

uint8_t OLED_Write_Command(uint8_t command)
{
    uint8_t ack;
    uint8_t ctrl = 0x00;
    uint8_t result = 0;

    I2C_Start();
    I2C_WriteByte(OLED_DEV_ADDR);
    ack = I2C_GetAck();
    if (I2C_NOACK == ack) {
        result = 1;
        return result;
    }
    I2C_WriteByte(ctrl);
    ack = I2C_GetAck();
    if (I2C_NOACK == ack) {
        result = 1;
        return result;
    }
    I2C_WriteByte(command);
    ack = I2C_GetAck();
    if (I2C_NOACK == ack) {
        result = 1;
        return result;
    }
    I2C_Stop();
    I2C_Delay(1);
    return result;
}

uint8_t OLED_Write_Data(uint8_t data)
{
    uint8_t ack;
    uint8_t ctrl = 0x40;
    uint8_t result = 0;

    I2C_Start();
    I2C_WriteByte(OLED_DEV_ADDR);
    ack = I2C_GetAck();
    if (I2C_NOACK == ack) {
        result = 1;
        return result;
    }
    I2C_WriteByte(ctrl);
    ack = I2C_GetAck();
    if (I2C_NOACK == ack) {
        result = 1;
        return result;
    }
    I2C_WriteByte(data);
    ack = I2C_GetAck();
    if (I2C_NOACK == ack) {
        result = 1;
        return result;
    }
    I2C_Stop();
    return result;
}

void OLED_Fill(uint8_t _8pixel)
{
    uint16_t m, n;

    for (m = 0; m < 8; m++) {
        // 128x64 bit
        OLED_Write_Command(0xb0 + m);   // page0-page7
        OLED_Write_Command(0x00);       // low column start address
        OLED_Write_Command(0x10);       // high column start address
        for (n = 0; n < 132; n++) {
            OLED_Write_Data(_8pixel);
        }
    }

    OLED_Write_Command(0xA4);   // GDRAM to display.
}

void OLED_ClearScreen(void)
{
    OLED_Fill(0x00);
}

void OLED_FillDot(void)
{
    OLED_Fill(0x01);
}

void OLED_On(void)
{
    OLED_Write_Command(0x8D);
    OLED_Write_Command(0x14);
    OLED_Write_Command(0xAF);
}

void OLED_Off(void)
{
    OLED_Write_Command(0x8D);
    OLED_Write_Command(0x10);
    OLED_Write_Command(0xAE);
}

void OLED_StartDisplay(void)
{
    OLED_Write_Command(0xAE);   // display off
    OLED_Write_Command(0x20);   // set memory addressing mode
    OLED_Write_Command(0x10);   // 00 -- horizontal addressing mode; 01 -- vertical addressing mode; 10 -- page addressing mode; 11 -- invalid
    OLED_Write_Command(0xB0);   // set page start address for page addressing mode, 0-7
    OLED_Write_Command(0xC8);   // set COM output scan direction
    OLED_Write_Command(0x00);   // set low column address
    OLED_Write_Command(0x10);   // set high column address
    OLED_Write_Command(0x40);   // set start line address
    OLED_Write_Command(0x81);   // set contrast control register
    OLED_Write_Command(0xDF);
    OLED_Write_Command(0xA1);   // set segment re-map 0 to 127
    OLED_Write_Command(0xA6);   // set normal display
    OLED_Write_Command(0xA8);   // set multiplex ratio (1 to 64)
    OLED_Write_Command(0x3F);
    OLED_Write_Command(0xA4);   // 0xA5 -- output follows RAM content; 0xA5 -- output ignores RAM content
    OLED_Write_Command(0xD3);   // set display offset
    OLED_Write_Command(0x00);   // not offset
    OLED_Write_Command(0xD5);   // set display clock divide ratio/oscillator frequency
    OLED_Write_Command(0xF0);   // set divide ratio
    OLED_Write_Command(0xD9);   // set pre-charge period
    OLED_Write_Command(0x22);
    OLED_Write_Command(0xDA);   // set COM pins hardware configuration
    OLED_Write_Command(0x12);
    OLED_Write_Command(0xDB);   // set Vcomh
    OLED_Write_Command(0x20);   // 0x20, 0x77 x Vcc
    OLED_Write_Command(0x8D);   // set DC-DC enable
    OLED_Write_Command(0x14);
    OLED_Write_Command(0xAF);   // turn on OLED panel
}
