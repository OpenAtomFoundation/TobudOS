#ifndef _SYSCON_TYPES_H_
#define _SYSCON_TYPES_H_
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus



// PORT A ranges GPIOA_0 to GPIOA_15
#define GPIO_PWIDTH_A 16


/**
 * @brief enum of GPIO_Num.
 */
typedef enum
{
	GPIOA_0  =  0,//gpio port A
	GPIOA_1  =  1,
	GPIOA_2  =  2,
	GPIOA_3  =  3,
	GPIOA_4  =  4,
	GPIOA_5  =  5,
	GPIOA_6  =  6,
	GPIOA_7  =  7,
	GPIOA_8  =  8,
	GPIOA_9  =  9,
	GPIOA_10 = 10,
	GPIOA_11 = 11,
	GPIOA_12 = 12,
	GPIOA_13 = 13,
	GPIOA_14 = 14,
	GPIOA_15 = 15,
	GPIOB_0  = 16,//gpio port B
	GPIOB_1  = 17,
	GPIOB_2  = 18,
	GPIOB_3  = 19,
	GPIOB_4  = 20,
	GPIOB_5  = 21,
	GPIOB_6  = 22,
	GPIOB_7  = 23,
	GPIOB_8  = 24,
	GPIOB_9  = 25,
	GPIO_NUM_MAX = 26,
} GPIO_Num;

#define IS_PORTA_GPIO(num)      ( (num) == GPIOA_0  || \
                                    (num) == GPIOA_1  || \
                                    (num) == GPIOA_2  || \
                                    (num) == GPIOA_3  || \
                                    (num) == GPIOA_4  || \
                                    (num) == GPIOA_5  || \
                                    (num) == GPIOA_6  || \
                                    (num) == GPIOA_7  || \
                                    (num) == GPIOA_8  || \
                                    (num) == GPIOA_9  || \
                                    (num) == GPIOA_10 || \
                                    (num) == GPIOA_11 || \
                                    (num) == GPIOA_12 || \
                                    (num) == GPIOA_13 || \
                                    (num) == GPIOA_14 || \
                                    (num) == GPIOA_15 )


typedef enum
{
	GPIO_AF_I2C0_SCL     = 0, //I2C0,I2C1
	GPIO_AF_I2C0_SDA     = 1,
	GPIO_AF_I2C1_SCL     = 2,
	GPIO_AF_I2C1_SDA     = 3,
	GPIO_AF_UART0_TX     = 4, //UART0,UART1
	GPIO_AF_UART0_RX     = 5,
	GPIO_AF_UART0_RTS    = 6,
	GPIO_AF_UART0_CTS    = 7,
	GPIO_AF_UART1_TX     = 8,
	GPIO_AF_UART1_RX     = 9,
	GPIO_AF_TIMER0_PWM   = 10,//TIMERx_PWM,PWM
	GPIO_AF_TIMER1_PWM   = 11,
	GPIO_AF_TIMER2_PWM   = 12,
	GPIO_AF_TIMER3_PWM   = 13,
	GPIO_AF_PWM0         = 14,
	GPIO_AF_PWM1         = 15,
	GPIO_AF_PWM2         = 16,
	GPIO_AF_PWM3         = 17,
	GPIO_AF_PWM4         = 18,
	GPIO_AF_PWM5         = 19,
	GPIO_AF_PWM6         = 20,
	GPIO_AF_PWM7         = 21,
	GPIO_AF_PWM8         = 22,
	GPIO_AF_PWM9         = 23,
	GPIO_AF_PWM10        = 24,
	GPIO_AF_PWM11        = 25,
	GPIO_AF_SPI_M1_CLK   = 26,//SPI_Master1
	GPIO_AF_SPI_M1_CS    = 27,
	GPIO_AF_SPI_M1_MOSI  = 28,
	GPIO_AF_SPI_M1_MISO  = 29,
	GPIO_AF_SPI_S0_CLK   = 30,//SPI_Slaver0
	GPIO_AF_SPI_S0_CS    = 31,
	GPIO_AF_SPI_S0_MOSI  = 32,
	GPIO_AF_SPI_S0_MISO  = 33,
	GPIO_AF_SPI_M0_CLK   = 34,//SPI_Master0
	GPIO_AF_SPI_M0_CS    = 35,
	GPIO_AF_SPI_M0_MOSI  = 36,
	GPIO_AF_SPI_M0_MISO  = 37,
} GPIO_AltFunctionType;



typedef enum
{
    GPIO_AF_IO_0  = 0,
    GPIO_AF_IO_1  = 1,
    GPIO_AF_IO_2  = 2,
    GPIO_AF_IO_3  = 3,
    GPIO_AF_IO_4  = 4,
    GPIO_AF_IO_5  = 5,
    GPIO_AF_IO_6  = 6,
    GPIO_AF_IO_7  = 7,
    GPIO_AF_IO_8  = 8,
    GPIO_AF_IO_9  = 9,
    GPIO_AF_IO_10 = 10,
    GPIO_AF_IO_11 = 11,
    GPIO_AF_IO_12 = 12,
    GPIO_AF_IO_13 = 13,
    GPIO_AF_IO_14 = 14,
    GPIO_AF_IO_15 = 15,
    GPIO_AF_IO_16 = 16,
    GPIO_AF_IO_17 = 17,
    GPIO_AF_IO_18 = 18,
    GPIO_AF_IO_19 = 19
} GPIO_AltFunctionIoIndex;

typedef enum        //reset only one device at a time
{
    SW_RST_CORE_PMU = 0,
    SW_RST_CORE_WIC = 1,
    SW_RST_CORE_RTC = 2,
    SW_RST_CORE_PHY = 3,
    SW_RST_CORE_ALL = 4
} SYSCON_SwRst_Core;

typedef enum       //reset only one device at a time
{
    SW_RST_PATCH  = 0,
    SW_RST_QSPI   = 1,
    SW_RST_ADCC   = 2,
    SW_RST_I2S    = 3,
    SW_RST_GPIO   = 4,
    SW_RST_SPIM   = 5,
    SW_RST_SPIS   = 6,
    SW_RST_I2C0   = 7,
    SW_RST_I2C1   = 8,
    SW_RST_UART0  = 9,
    SW_RST_UART1  = 10,
    SW_RST_SPIM2  = 11,
    SW_RST_WDT    = 12,
    SW_RST_TIMER  = 13,
    SW_RST_DBGH   = 14,
    SW_RST_SDIO   = 15,
    SW_RST_MAC    = 16,
    SW_RST_CACHE  = 17,
    SW_RST_DMA    = 18,
    SW_RST_RFREG  = 19
} SYSCON_SwRst_Peripheral;

typedef enum
{
    SW_CLKGATE_CORE_RTC    = 0,
    SW_CLKGATE_CORE_WIC    = 1,
    SW_CLKGATE_CORE_PHY    = 2,
    SW_CLKGATE_CORE_MAC20M = 3,
    SW_CLKGATE_CORE_MAC40M = 4,
    SW_CLKGATE_CORE_MAC80M = 5
} SYSCON_ClkGate_Core;

typedef enum
{
    SW_CLKGATE_QSPI    = 1,
    SW_CLKGATE_ADCC    = 2,
    SW_CLKGATE_I2S     = 3,
    SW_CLKGATE_GPIO    = 4,
    SW_CLKGATE_SPIM    = 5,
    SW_CLKGATE_SPIS    = 6,
    SW_CLKGATE_I2C0    = 7,
    SW_CLKGATE_I2C1    = 8,
    SW_CLKGATE_UART0   = 9,
    SW_CLKGATE_UART1   = 10,
    SW_CLKGATE_SPIM2   = 11,
    SW_CLKGATE_WDT     = 12,
    SW_CLKGATE_TIMER   = 13,
    SW_CLKGATE_TIMER_1 = 14,
    SW_CLKGATE_TIMER_2 = 15,
    SW_CLKGATE_TIMER_3 = 16,
    SW_CLKGATE_TIMER_4 = 17,
    SW_CLKGATE_DBGH    = 18,
    SW_CLKGATE_SDIO    = 19,
    SW_CLKGATE_MAC     = 20,
    SW_CLKGATE_CACHE   = 21,
    SW_CLKGATE_DMA     = 22,
    SW_CLKGATE_RFREG   = 23,
    SW_CLKGATE_PWM     = 24,
    SW_CLKGATE_EF      = 25,
    SW_CLKGATE_TRNG    = 26,
    SW_CLKGATE_AES     = 27,
} SYSCON_ClkGate_Peripheral;

typedef enum
{
    SPIM_IO_EN0 = 0,
    SPIM_IO_EN1 = 1,
    SPIM_IO_EN2 = 2
} SYSCON_SPIM_Index;

typedef enum
{
    I2S_IO_EN0 = 0,
    I2S_IO_EN1 = 1
} SYSCON_I2S_Index;

typedef enum
{
    SYSCLK_SRC_XTAL = 0,
    SYSCLK_SRC_PLL
} SYSTEM_CLOCK_SRC;
typedef enum
{
    SYSTEM_EXT_INT0 = 0,
    SYSTEM_EXT_INT1 = 1,
    SYSTEM_EXT_INT2 = 2,
    SYSTEM_EXT_INT3 = 3,
    SYSTEM_EXT_INT4 = 4,
    SYSTEM_EXT_INT5 = 5,
    SYSTEM_EXT_INT6 = 6,
    SYSTEM_EXT_INT7 = 7,
    SYSTEM_EXT_INT_ERROR
}SYSTEM_EXT_INT_Wakeup_Index;
typedef enum
{
    SYSTEM_EXT_INT_TRIG_HIGH_LEVEL   = 0,
    SYSTEM_EXT_INT_TRIG_LOW_LEVEL    = 1,
    SYSTEM_EXT_INT_TRIG_RISING_EDGE  = 2,
    SYSTEM_EXT_INT_TRIG_FALLING_EDGE = 3,
}SYSTEM_EXT_INT_Triggle_Type;


////////////////////////////////////////  Macros  /////////////////////////////
#define IS_GPIO_AF_TYPE(type)   ((type) == GPIO_AF_I2C0_SCL    || \
                                   (type) == GPIO_AF_I2C0_SDA    || \
                                   (type) == GPIO_AF_I2C1_SCL    || \
                                   (type) == GPIO_AF_I2C1_SDA    || \
                                   (type) == GPIO_AF_UART0_TX    || \
                                   (type) == GPIO_AF_UART0_RX    || \
                                   (type) == GPIO_AF_UART0_RTS   || \
                                   (type) == GPIO_AF_UART0_CTS   || \
                                   (type) == GPIO_AF_UART1_TX    || \
                                   (type) == GPIO_AF_UART1_RX    || \
                                   (type) == GPIO_AF_TIMER0_PWM  || \
                                   (type) == GPIO_AF_TIMER1_PWM  || \
                                   (type) == GPIO_AF_TIMER2_PWM  || \
                                   (type) == GPIO_AF_TIMER3_PWM  || \
                                   (type) == GPIO_AF_PWM0        || \
                                   (type) == GPIO_AF_PWM1        || \
                                   (type) == GPIO_AF_PWM2        || \
                                   (type) == GPIO_AF_PWM3        || \
                                   (type) == GPIO_AF_PWM4        || \
                                   (type) == GPIO_AF_PWM5        || \
                                   (type) == GPIO_AF_PWM6        || \
                                   (type) == GPIO_AF_PWM7        || \
                                   (type) == GPIO_AF_PWM8        || \
                                   (type) == GPIO_AF_PWM9        || \
                                   (type) == GPIO_AF_PWM10       || \
                                   (type) == GPIO_AF_PWM11       || \
                                   (type) == GPIO_AF_SPI_M1_CLK  || \
                                   (type) == GPIO_AF_SPI_M1_CS   || \
                                   (type) == GPIO_AF_SPI_M1_MOSI || \
                                   (type) == GPIO_AF_SPI_M1_MISO || \
                                   (type) == GPIO_AF_SPI_S0_CLK  || \
                                   (type) == GPIO_AF_SPI_S0_CS   || \
                                   (type) == GPIO_AF_SPI_S0_MOSI || \
                                   (type) == GPIO_AF_SPI_S0_MISO || \
                                   (type) == GPIO_AF_SPI_M0_CLK  || \
                                   (type) == GPIO_AF_SPI_M0_CS   || \
                                   (type) == GPIO_AF_SPI_M0_MOSI || \
                                   (type) == GPIO_AF_SPI_M0_MISO)




#define IS_GPIO_AF_IO_INDEX(index)  ((index) == GPIO_AF_IO_0  || \
                                     (index) == GPIO_AF_IO_1  || \
                                     (index) == GPIO_AF_IO_2  || \
                                     (index) == GPIO_AF_IO_3  || \
                                     (index) == GPIO_AF_IO_4  || \
                                     (index) == GPIO_AF_IO_5  || \
                                     (index) == GPIO_AF_IO_6  || \
                                     (index) == GPIO_AF_IO_7  || \
                                     (index) == GPIO_AF_IO_8  || \
                                     (index) == GPIO_AF_IO_9  || \
                                     (index) == GPIO_AF_IO_10 || \
                                     (index) == GPIO_AF_IO_11 || \
                                     (index) == GPIO_AF_IO_12 || \
                                     (index) == GPIO_AF_IO_13 || \
                                     (index) == GPIO_AF_IO_14 || \
                                     (index) == GPIO_AF_IO_15 || \
                                     (index) == GPIO_AF_IO_16 || \
                                     (index) == GPIO_AF_IO_17 || \
                                     (index) == GPIO_AF_IO_18 || \
                                     (index) == GPIO_AF_IO_19)


#define IS_SWRST_CORE(core)     ((core) == SW_RST_CORE_PMU || \
                                 (core) == SW_RST_CORE_WIC || \
                                 (core) == SW_RST_CORE_RTC || \
                                 (core) == SW_RST_CORE_PHY || \
                                 (core) <= SW_RST_CORE_ALL)

#define IS_SWRST_PERIPHERAL(peri)   ((peri) == SW_RST_PATCH || \
                                     (peri) == SW_RST_QSPI  || \
                                     (peri) == SW_RST_ADCC  || \
                                     (peri) == SW_RST_I2S   || \
                                     (peri) == SW_RST_GPIO  || \
                                     (peri) == SW_RST_SPIM  || \
                                     (peri) == SW_RST_SPIS  || \
                                     (peri) == SW_RST_I2C0  || \
                                     (peri) == SW_RST_I2C1  || \
                                     (peri) == SW_RST_UART0 || \
                                     (peri) == SW_RST_UART1 || \
                                     (peri) == SW_RST_SPIM2 || \
                                     (peri) == SW_RST_WDT   || \
                                     (peri) == SW_RST_TIMER || \
                                     (peri) == SW_RST_DBGH  || \
                                     (peri) == SW_RST_SDIO  || \
                                     (peri) == SW_RST_MAC   || \
                                     (peri) == SW_RST_CACHE || \
                                     (peri) == SW_RST_DMA   || \
                                     (peri) == SW_RST_RFREG)

#define IS_CLKGATE_CORE(clkgate)    ((clkgate) == SW_CLKGATE_CORE_RTC    || \
                                     (clkgate) == SW_CLKGATE_CORE_WIC    || \
                                     (clkgate) == SW_CLKGATE_CORE_PHY    || \
                                     (clkgate) == SW_CLKGATE_CORE_MAC20M || \
                                     (clkgate) == SW_CLKGATE_CORE_MAC40M || \
                                     (clkgate) == SW_CLKGATE_CORE_MAC80M)

#define IS_CLKGATE_PERIPHERAL(clkgate)  ((clkgate) == SW_CLKGATE_QSPI    || \
                                         (clkgate) == SW_CLKGATE_ADCC    || \
                                         (clkgate) == SW_CLKGATE_I2S     || \
                                         (clkgate) == SW_CLKGATE_GPIO    || \
                                         (clkgate) == SW_CLKGATE_SPIM    || \
                                         (clkgate) == SW_CLKGATE_SPIS    || \
                                         (clkgate) == SW_CLKGATE_I2C0    || \
                                         (clkgate) == SW_CLKGATE_I2C1    || \
                                         (clkgate) == SW_CLKGATE_UART0   || \
                                         (clkgate) == SW_CLKGATE_UART1   || \
                                         (clkgate) == SW_CLKGATE_SPIM2   || \
                                         (clkgate) == SW_CLKGATE_WDT     || \
                                         (clkgate) == SW_CLKGATE_TIMER   || \
                                         (clkgate) == SW_CLKGATE_TIMER_1 || \
                                         (clkgate) == SW_CLKGATE_TIMER_2 || \
                                         (clkgate) == SW_CLKGATE_TIMER_3 || \
                                         (clkgate) == SW_CLKGATE_TIMER_4 || \
                                         (clkgate) == SW_CLKGATE_DBGH    || \
                                         (clkgate) == SW_CLKGATE_SDIO    || \
                                         (clkgate) == SW_CLKGATE_MAC     || \
                                         (clkgate) == SW_CLKGATE_CACHE   || \
                                         (clkgate) == SW_CLKGATE_DMA     || \
                                         (clkgate) == SW_CLKGATE_RFREG   || \
                                         (clkgate) == SW_CLKGATE_PWM     || \
                                         (clkgate) == SW_CLKGATE_EF      || \
                                         (clkgate) == SW_CLKGATE_TRNG    || \
                                         (clkgate) == SW_CLKGATE_AES)

#define IS_SPIM_INDEX(index)    ((index) == SPIM_IO_EN0 || (index) == SPIM_IO_EN1 || (index) == SPIM_IO_EN2)

#define IS_I2S_INDEX(index)     ((index) == I2S_IO_EN0 || (index) == I2S_IO_EN1)

#define IS_CLOCK_SRC(src)       ((src) == SYSCLK_SRC_XTAL || (src) == SYSCLK_SRC_PLL)


#ifdef __cplusplus
}
#endif // __cplusplus
#endif /* _SYSCON_TYPES_H_ */
