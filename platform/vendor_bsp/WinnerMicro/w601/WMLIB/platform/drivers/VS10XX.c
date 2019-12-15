/**
 * @file    VS10XX.c
 *
 * @brief   VS10XX driver
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#include <string.h>
#include "VS10XX.h"
#include "wm_hostspi.h"
#include "wm_gpio.h"
#include "wm_config.h"

#if CODEC_VS1053
extern void delay_us(unsigned int time);
extern int gSpiCsFlag;
void codec_data_cs_low(void)
{
    while (0 == tls_gpio_read(SPI_CS_PIN))
    {
//      printf("\nflash is busy,codec data cs wait...\n");
        tls_os_time_delay(1);
    }
    gSpiCsFlag = 1;
    tls_gpio_cfg(VS_XDCS, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_write(VS_XDCS, 0);
}

void codec_data_cs_high(void)
{
    tls_gpio_cfg(VS_XDCS, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_write(VS_XDCS, 1);
    gSpiCsFlag = 0;
}

void codec_cmd_cs_low(void)
{
    while (0 == tls_gpio_read(SPI_CS_PIN))
    {
//      printf("\nflash is busy,codec cmd cs wait...\n");
        tls_os_time_delay(1);
    }
    gSpiCsFlag = 1;
    tls_gpio_cfg(VS_XCS, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_write(VS_XCS, 0);
}

void codec_cmd_cs_high(void)
{
    tls_gpio_cfg(VS_XCS, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_write(VS_XCS, 1);
    gSpiCsFlag = 0;
}

int VS_Wait_Timeout(void)
{
    u16 retry = 0;

    while ((tls_gpio_read(VS_DQ) == 0) && (retry < 0xFFFE))
    {
        retry++;
    // delay_us(50);
    }
    if (retry >= 0xFFFE)
    {
        printf("VS1053 wait timeout\r\n");
        return 1;
    }
    return 0;
}

const u16 wav_plugin[40] =      /* Compressed plugin */
{
    0x0007, 0x0001, 0x8010, 0x0006, 0x001c, 0x3e12, 0xb817, 0x3e14, /* 0 */
    0xf812, 0x3e01, 0xb811, 0x0007, 0x9717, 0x0020, 0xffd2, 0x0030, /* 8 */
    0x11d1, 0x3111, 0x8024, 0x3704, 0xc024, 0x3b81, 0x8024, 0x3101, /* 10 */
    0x8024, 0x3b81, 0x8024, 0x3f04, 0xc024, 0x2808, 0x4800, 0x36f1, /* 18 */
    0x9811, 0x0007, 0x0001, 0x8028, 0x0006, 0x0002, 0x2a00, 0x040e,
};

//vs10xx装载patch.
//patch：patch首地址
//len：patch长度
void VS_Load_Patch(u16 * patch, u16 len)
{
    u16 i;
    u16 addr, n, val;

    for (i = 0; i < len;)
    {
        addr = patch[i++];
        n = patch[i++];
        if (n & 0x8000U)        // RLE run, replicate n samples
        {
            n &= 0x7FFF;
            val = patch[i++];
            while (n--)
            {
                VS_WR_Cmd(addr, val);
            }
        }
        else                    // copy run, copy n sample
        {
            while (n--)
            {
                val = patch[i++];
                VS_WR_Cmd(addr, val);
            }
        }
    }
}

//硬复位MP3
u8 VS_HD_Reset(void)
{
    tls_gpio_write(VS_XDCS, 1); // 取消数据传输
    tls_gpio_write(VS_XCS, 1);
    tls_gpio_write(VS_RST, 0);
    tls_os_time_delay(10);
    tls_gpio_write(VS_RST, 1);
    return VS_Wait_Timeout();
}

//软复位VS10XX
void VS_Soft_Reset(void)
{
    tls_spi_setup(TLS_SPI_MODE_0, TLS_SPI_CS_LOW, VS_SPI_CLK_LOW);
    VS_WR_Cmd(SPI_MODE, 0x0816);
    VS_WR_Cmd(SPI_CLOCKF, 0x9800);
    VS_WR_Cmd(SPI_DECODE_TIME, 0x0000);
    VS_WR_Cmd(SPI_DECODE_TIME, 0x0000);
}

//向VS10XX写命令
//address:命令地址
//data:命令数据
void VS_WR_Cmd(u8 address, u16 data)
{
    u8 temp[5];

    if (VS_Wait_Timeout())
    {
        return;
    }
    codec_cmd_cs_low();
    temp[0] = VS_WRITE_COMMAND;
    temp[1] = address;
    temp[2] = data >> 8;
    temp[3] = data & 0xff;
    tls_spi_write(temp, 4);
    codec_cmd_cs_high();
}

//读VS10XX的寄存器
//注意不要用倍速读取,会出错
u16 VS_RD_Reg(u8 address)
{
    u8 txbuf[2];
    u8 rxbuf[2];

    if (VS_Wait_Timeout())
    {
        return;
    }
    codec_cmd_cs_low();
    txbuf[0] = VS_READ_COMMAND;
    txbuf[1] = address;
    tls_spi_read_with_cmd(txbuf, 2, rxbuf, 2);
    codec_cmd_cs_high();
    return ((rxbuf[0] << 8) | rxbuf[1]);
}

//得到mp3的播放时间n sec
u16 VS_Get_DecodeTime(void)
{
    tls_spi_setup(TLS_SPI_MODE_0, TLS_SPI_CS_LOW, VS_SPI_CLK_LOW);
    return VS_RD_Reg(SPI_DECODE_TIME);
}

void vs_set_vol(int vol)
{
    int tmp;
    u16 volt = 0;

    tmp = vol;
    if (vol >= 0 && vol <= 100)
    {
        volt = tmp << 8;
        volt += tmp;
        printf("\nvolt==%x\n", volt);
        tls_spi_setup(TLS_SPI_MODE_0, TLS_SPI_CS_LOW, VS_SPI_CLK_LOW);
        VS_WR_Cmd(SPI_VOL, volt);   // 设音量
    }
}

void vs_mute(void)
{
    tls_spi_setup(TLS_SPI_MODE_0, TLS_SPI_CS_LOW, VS_SPI_CLK_LOW);
    VS_WR_Cmd(SPI_VOL, 0xfefe); // 设音量
}

//激活PCM 录音模式
//agc:0,自动增益.1024相当于1倍,512相当于0.5倍,最大值65535=64倍
void VS_Rec_Mode(u16 agc)
{
    tls_spi_setup(TLS_SPI_MODE_0, TLS_SPI_CS_LOW, VS_SPI_CLK_LOW);
    VS_WR_Cmd(SPI_BASS, 0x0000);
    VS_WR_Cmd(SPI_AICTRL0, 8000);   // 设置采样率,设置为8Khz
    VS_WR_Cmd(SPI_AICTRL1, agc);    // 设置增益,0,自动增益.1024相当于1倍,512相当于0.5倍,最大值65535=64倍
                                    //
    VS_WR_Cmd(SPI_AICTRL2, 0);  // 设置增益最大值,0,代表最大值65536=64X
    VS_WR_Cmd(SPI_AICTRL3, 6);  // 左通道(MIC单声道输入)
    VS_WR_Cmd(SPI_CLOCKF, 0X2000);  // 设置VS10XX的时钟,MULT:2倍频;ADD:不允许;CLK:12.288Mhz
    VS_WR_Cmd(SPI_MODE, 0x1804);    // MIC,录音激活
    tls_os_time_delay(1);       // 等待至少1.35ms
    VS_Load_Patch((u16 *) wav_plugin, 40);  // VS1053的WAV录音需要patch
}

int VS_Rec_GetData(char *recbuf)
{
    int datalen, idx;
    u16 data;

    tls_spi_setup(TLS_SPI_MODE_0, TLS_SPI_CS_LOW, VS_SPI_CLK_HIGH);
    datalen = VS_RD_Reg(SPI_HDAT1);
    if ((datalen >= VS_REC_PKG_SIZE) && (datalen < 896))
    {
        idx = 0;
        while (idx < VS_REC_PKG_SIZE)
        {
            data = VS_RD_Reg(SPI_HDAT0);
            recbuf[idx++] = data & 0XFF;
            recbuf[idx++] = data >> 8;
        }
        return 1;
    }
    else if (datalen >= 896)
    {
        printf("#%d\r\n", datalen);
    }
    return 0;
}

//初始化VS10XX的IO口
void VS_Init(void)
{
    tls_gpio_cfg(VS_RST, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_write(VS_RST, 1);

    tls_gpio_cfg(VS_XCS, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_write(VS_XCS, 1);

    tls_gpio_cfg(VS_XDCS, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_write(VS_XDCS, 1);

    tls_gpio_cfg(VS_DQ, WM_GPIO_DIR_INPUT, WM_GPIO_ATTR_PULLHIGH);
}

#endif
