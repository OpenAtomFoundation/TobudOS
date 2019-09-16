/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @brief Boot API接口定义。
 * @file n32g020xx_boot.h
 * @author Nations Solution Team
 * @version v1.0.1
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 * @defgroup BOOT Boot API接口
 * @{
 */

#ifndef __N32G020XX_BOOT_H__
#define __N32G020XX_BOOT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define API_ENTRY 0x2001

#define BOOT_API_ERR        0xFFFFFFFF
#define BOOT_API_SUCCESS    0xFFFFFFF0

#define BOOT_BACK_KEY0      0x55555555
#define BOOT_BACK_KEY1      0xAAAAAAAA

#define BOOT_OSC_FREQ_GET   0xBBBBBBBB
#define BOOT_CHIP_INF_RD    0xEEEEEEEE
#define BOOT_CHIP_ID_RD     0xEEEEFFFF
#define BOOT_SWD_SET        0xEEEE1111
#define BOOT_SWD_GET        0xEEEE2222
#define BOOT_BACK           0xFFFFFFFF
#define BOOT_PARTITION_SET  0xEEEE3333
#define BOOT_USER_PROTECTION_SET    0xEEEE4444

typedef uint32_t (*Boot_APIFunc)(uint32_t, uint32_t, uint32_t);
#define BOOT_API_ENTRY ((Boot_APIFunc)API_ENTRY)

/**
 * @brief 取得OSC80M时钟的实测频率
 * @param freq 0表示80M的实测频率，1表示40M的实测频率
 * @return 实测频率
 */
#define Boot_APIOSCFreqGet(freq) BOOT_API_ENTRY(freq, 0, BOOT_OSC_FREQ_GET)

/**
 * @brief 取得芯片序列号
 * @param buf 存放SN的RAM地址，长度应为16字节
 * @return BOOT_API_SUCCESS
 */
#define Boot_APISNGet(buf) BOOT_API_ENTRY((uint32_t)buf, 1, BOOT_CHIP_INF_RD)

/**
 * @brief 取得芯片ID
 * @param buf 存放ID的RAM地址，长度应为8字节
 * @return BOOT_API_SUCCESS
 */
#define Boot_APIIDGet(buf) BOOT_API_ENTRY((uint32_t)buf, 0, BOOT_CHIP_ID_RD)

/**
 * @brief 退回Boot模式
 * @param key1 应为BOOT_BACK_KEY0
 * @param key2 应为BOOT_BACK_KEY1
 * @return BOOT_API_SUCCESS 或 BOOT_API_ERR
 *
 * 调用后复位芯片，则退回Boot模式
 */
#define Boot_APIBackBoot(key0, key1) BOOT_API_ENTRY(key0, key1, BOOT_BACK)

/**
 * @brief 开启SWD接口
 * @return  BOOT_API_SUCCESS
 * @note 在应用程序模式下调用此函数，芯片将会切换回Boot模式。
 */
#define Boot_APIEnableSWD()             BOOT_API_ENTRY(0, 1, BOOT_SWD_SET)

/**
 * @brief 禁用SWD接口
 * @return  BOOT_API_SUCCESS
 */
#define Boot_APIDisableSWD()            BOOT_API_ENTRY(0, 0, BOOT_SWD_SET)

/**
 * @brief 取得SWD状态
 * @return  0表示SWD被禁用，1表示SWD被启用
 */
#define Boot_APIGetSWDState()           BOOT_API_ENTRY(0, 0, BOOT_SWD_GET)

/**
 * @brief FLASH用户分区
 * 
 * @param userboot_size 用户BOOT分区大小
 * @param app1_size APP1分区大小
 * @return `BOOT_API_SUCCESS` 表示分区参数合法，`BOOT_API_ERR` 表示分区参数不合法
 * 
 * 上电只能通过BootAPI配置一次
 */
#define Boot_APISetPartition(userboot_size, app1_size) BOOT_API_ENTRY((uint32_t)userboot_size, (uint32_t)app1_size, BOOT_PARTITION_SET)


/** @brief 用户BOOT区域权限封闭 */
#define USERBOOT_PROTECTION_ENABLE   0x01
/** @brief APP1区域权限封闭 */
#define APP1_PROTECTION_ENABLE   0x02
/** @brief APP2区域权限封闭 */
#define APP2_PROTECTION_ENABLE   0x04

/**
 * @brief 用户权限封闭
 * 
 * @param flag 权限封闭的位设置，为 `USERBOOT_PROTECTION_ENABLE`, `APP1_PROTECTION_ENABLE` 和 `APP2_PROTECTION_ENABLE` 的组合
 */
#define Boot_APISetUserProtection(flag) BOOT_API_ENTRY(0, (uint32_t)flag, BOOT_USER_PROTECTION_SET)

#ifdef __cplusplus
}
#endif

#endif // __N32G020XX_BOOT_H__

/** @} */
