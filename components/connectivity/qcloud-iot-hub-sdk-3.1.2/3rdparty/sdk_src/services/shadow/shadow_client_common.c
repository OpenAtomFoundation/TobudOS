/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "shadow_client_common.h"
#include "qcloud_iot_import.h"


static int _add_property_handle_to_list(Qcloud_IoT_Shadow *pShadow, DeviceProperty *pProperty, OnPropRegCallback callback)
{
    IOT_FUNC_ENTRY;

    PropertyHandler *property_handle = (PropertyHandler *)HAL_Malloc(sizeof(PropertyHandler));
    if (NULL == property_handle)
    {
        Log_e("run memory malloc is error!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    property_handle->callback = callback;
    property_handle->property = pProperty;

    ListNode *node = list_node_new(property_handle);
    if (NULL == node) {
        Log_e("run list_node_new is error!");
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }
    list_rpush(pShadow->inner_data.property_handle_list, node);

    IOT_FUNC_EXIT_RC(QCLOUD_RET_SUCCESS);
}

int shadow_common_check_property_existence(Qcloud_IoT_Shadow *pshadow, DeviceProperty *pProperty)
{
    ListNode *node;

    HAL_MutexLock(pshadow->mutex);
    node = list_find(pshadow->inner_data.property_handle_list, pProperty);
    HAL_MutexUnlock(pshadow->mutex);

    return (NULL != node);
}

int shadow_common_remove_property(Qcloud_IoT_Shadow *pshadow, DeviceProperty *pProperty)
{
    int rc = QCLOUD_RET_SUCCESS;

    ListNode *node;
    HAL_MutexLock(pshadow->mutex);
    node = list_find(pshadow->inner_data.property_handle_list, pProperty);
    if (NULL == node) {
        rc = QCLOUD_ERR_SHADOW_NOT_PROPERTY_EXIST;
        Log_e("Try to remove a non-existent property.");
    } else {
        list_remove(pshadow->inner_data.property_handle_list, node);
    }
    HAL_MutexUnlock(pshadow->mutex);
    
    return rc;
}

int shadow_common_register_property_on_delta(Qcloud_IoT_Shadow *pShadow, DeviceProperty *pProperty, OnPropRegCallback callback)
{
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(pShadow, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(callback, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pProperty, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pProperty->key, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pProperty->data, QCLOUD_ERR_INVAL);

    int rc;

    HAL_MutexLock(pShadow->mutex);
    rc = _add_property_handle_to_list(pShadow, pProperty, callback);
    HAL_MutexUnlock(pShadow->mutex);

    IOT_FUNC_EXIT_RC(rc);
}

#ifdef __cplusplus
}
#endif
