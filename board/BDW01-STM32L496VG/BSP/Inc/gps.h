/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef __GPS_H__
#define __GPS_H__

#include "tos_at_gps.h"

//#define DEBUG_GPS_DATA 1

typedef struct
{
//	GSV_item item[5];
	char db1;//0-19
	char db2;//20-29
	char db3;//30-39
	char db4;//40-
}GSV_;


typedef enum
{
	ATGM336H_UNITE_GPS =1,
	ATGM336H_UNITE_BDS =2,
	ATGM336H_UNITE_GPS_BDS =3,
	ATGM336H_UNITE_GLONASS =4,
	ATGM336H_UNITE_GPS_GLONASS =5,
	ATGM336H_UNITE_BDS_GLONASS =6,
	ATGM336H_UNITE_GPS_BDS_GLONASS =7,
}Atgm336h_Unite;

int gps_read(int channel_id, void *buf, size_t len);
int gps_init(hal_uart_port_t uart_port);

#endif /* __GPS_H__ */

