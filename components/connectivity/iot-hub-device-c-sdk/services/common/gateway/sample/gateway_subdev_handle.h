/**
 * @file gateway_subdev_handle.h
 * @author {hubert} ({hubertxxu@tencent.com})
 * @brief
 * @version 1.0
 * @date 2022-05-25
 *
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @par Change Log:
 * <table>
 * Date				Version		Author			Description
 * 2022-05-25		1.0			hubertxxu		first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_SERVICES_COMMON_GATEWAY_SAMPLE_GATEWAY_SUBDEV_HANDLE_H_
#define IOT_HUB_DEVICE_C_SDK_SERVICES_COMMON_GATEWAY_SAMPLE_GATEWAY_SUBDEV_HANDLE_H_

#ifdef __cplusplus
extern "C" {
#endif

int iot_gateway_init(void *client, int sub_dev_max);

int iot_gateway_deinit(void *client);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_SERVICES_COMMON_GATEWAY_SAMPLE_GATEWAY_SUBDEV_HANDLE_H_
