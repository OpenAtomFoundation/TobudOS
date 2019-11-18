/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWS_LIB_H__
#define __AWS_LIB_H__

#include <stdint.h>

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/* auth type */
enum AWS_AUTH_TYPE {
    AWS_AUTH_TYPE_OPEN,
    AWS_AUTH_TYPE_SHARED,
    AWS_AUTH_TYPE_WPAPSK,
    AWS_AUTH_TYPE_WPA8021X,
    AWS_AUTH_TYPE_WPA2PSK,
    AWS_AUTH_TYPE_WPA28021X,
    AWS_AUTH_TYPE_WPAPSKWPA2PSK,
    AWS_AUTH_TYPE_MAX = AWS_AUTH_TYPE_WPAPSKWPA2PSK,
    AWS_AUTH_TYPE_INVALID = 0xff,
};

/* encry type */
enum AWS_ENC_TYPE {
    AWS_ENC_TYPE_NONE,
    AWS_ENC_TYPE_WEP,
    AWS_ENC_TYPE_TKIP,
    AWS_ENC_TYPE_AES,
    AWS_ENC_TYPE_TKIPAES,
    AWS_ENC_TYPE_MAX = AWS_ENC_TYPE_TKIPAES,
    AWS_ENC_TYPE_INVALID = 0xff,
};

/* link type */
enum AWS_LINK_TYPE {
    AWS_LINK_TYPE_NONE,
    AWS_LINK_TYPE_PRISM,
    AWS_LINK_TYPE_80211_RADIO,
    AWS_LINK_TYPE_80211_RADIO_AVS
};

#if 0
/* 将monitor模式下抓到的包传入该函数进行处理
 * 参数：
 *     buf: frame buffer
 *     length: frame len
 *     link_type: see enum AWS_LINK_TYPE
 *     with_fcs: frame include 80211 fcs field, the tailing 4bytes
 *
 * 说明：
 *     适配前执行以下命令, 检查link_type和with_fcs参数
 *     a) iwconfig wlan0 mode monitor    #进入monitor模式
 *     b) iwconfig wlan0 channel 6    #切换到信道6(以路由器信道为准)
 *     c) tcpdump -i wlan0 -s0 -w file.pacp    #抓包保存文件
 *     d) 用wireshark或者omnipeek打开，检查包头格式，及包尾是否包含FCS 4字节
 *
 *     常见的包头类型为：
 *     无额外的包头：AWS_LINK_TYPE_NONE
 *     radio header: hdr_len = *(uint16_t *)(buf + 2)
 *     avs header: hdr_len = *(unsigned long *)(buf + 4)
 *     prism header: hdr_len = 144
 */
int aws_80211_frame_handler(char *buf, int length,
                            int link_type, int with_fcs);
#endif

/* 启动一键配网服务, 该函数会block，直到配网成功或者超时退出,
 *     超时时间由aws_timeout_period_ms设置
 * 参数：
 *     pk: product key
 *     dn: device name
 *     ds: device security
 *     ps: product security
 */
void aws_start(char *pk, char *dn, char *ds, char *ps);
/* {该函数大致流程如下:
 *     init();
 *     platform_monitor_open();
 *     aws_main_thread_func();
 *     platform_monitor_close();
 *     destroy();
 * }

 * aws_start返回后，调用该函数，获取ssid和passwd等信息
 * aws成功时，ssid & passwd一定会返回非NULL字符串, 但bssid和auth, encry, channel
 *     有可能会返回NULL或者INVALID值(取决于是否能在wifi列表里搜索命中)
 * aws失败超时后，该函数会返回0, 且所有参数为NULL或INVALID VALUE
 *
 * auth defined by enum AWS_AUTH_TYPE
 * encry defined by enum AWS_ENC_TYPE
 *
 * 返回值：1--成功，0--失败
 */
int aws_get_ssid_passwd(char ssid[32 + 1], char passwd[64 + 1], uint8_t bssid[6],
                        char *auth, char *encry, uint8_t *channel);

/* 配网结束（成功或失败）后，调用该函数，释放配网库占用的资源 */
void aws_destroy(void);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
