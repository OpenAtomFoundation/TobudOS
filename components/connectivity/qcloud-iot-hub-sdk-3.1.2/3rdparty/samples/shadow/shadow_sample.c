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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "qcloud_iot_export.h"

#ifdef AUTH_MODE_CERT
    static char sg_cert_file[PATH_MAX + 1];      // full path of device cert file
    static char sg_key_file[PATH_MAX + 1];       // full path of device key file
#endif


static DeviceInfo sg_devInfo;

static char sg_shadow_update_buffer[200];
size_t sg_shadow_update_buffersize = sizeof(sg_shadow_update_buffer) / sizeof(sg_shadow_update_buffer[0]);

static DeviceProperty sg_shadow_property;
static int sg_current_update_count = 0;
static bool sg_delta_arrived = false;

void OnDeltaCallback(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty) {
	int rc = IOT_Shadow_JSON_ConstructDesireAllNull(pClient, sg_shadow_update_buffer, sg_shadow_update_buffersize);

	if (rc == QCLOUD_RET_SUCCESS) {
		sg_delta_arrived = true;
	}
	else {
		Log_e("construct desire failed, err: %d", rc);
	}
}

void OnShadowUpdateCallback(void *pClient, Method method, RequestAck requestAck, const char *pJsonDocument, void *pUserdata) {
	Log_i("recv shadow update response, response ack: %d", requestAck);
}

static int _setup_connect_init_params(ShadowInitParams* initParams)
{
	int ret;
	
	ret = HAL_GetDevInfo((void *)&sg_devInfo);	
	if(QCLOUD_RET_SUCCESS != ret){
		return ret;
	}
	
	initParams->device_name = sg_devInfo.device_name;
	initParams->product_id = sg_devInfo.product_id;

#ifdef AUTH_MODE_CERT
	char certs_dir[PATH_MAX + 1] = "certs";
	char current_path[PATH_MAX + 1];
	char *cwd = getcwd(current_path, sizeof(current_path));
	if (cwd == NULL)
	{
		Log_e("getcwd return NULL");
		return QCLOUD_ERR_FAILURE;
	}
	sprintf(sg_cert_file, "%s/%s/%s", current_path, certs_dir, sg_devInfo.dev_cert_file_name);
	sprintf(sg_key_file, "%s/%s/%s", current_path, certs_dir, sg_devInfo.dev_key_file_name);

	initParams->cert_file = sg_cert_file;
	initParams->key_file = sg_key_file;
#else
	initParams->device_secret = sg_devInfo.device_secret;
#endif

	initParams->command_timeout = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
	initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;
	initParams->auto_connect_enable = 1;

    return QCLOUD_RET_SUCCESS;
}


int demo_device_shadow()
{
	int rc = QCLOUD_ERR_FAILURE;

	void* shadow_client = NULL;

	//init connection
	ShadowInitParams init_params = DEFAULT_SHAWDOW_INIT_PARAMS;		
    rc = _setup_connect_init_params(&init_params);
	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("init params err,rc=%d", rc);
		return rc;
	}

	shadow_client = IOT_Shadow_Construct(&init_params);
	if (shadow_client == NULL) {
		Log_e("shadow client constructed failed.");
		return QCLOUD_ERR_FAILURE;
	}

	// register delta property
	sg_shadow_property.key = "updateCount";
	sg_shadow_property.data = &sg_current_update_count;
	sg_shadow_property.type = JINT32;
	rc = IOT_Shadow_Register_Property(shadow_client, &sg_shadow_property, OnDeltaCallback);
	if (rc != QCLOUD_RET_SUCCESS) {
		rc = IOT_Shadow_Destroy(shadow_client);
		Log_e("register device shadow property failed, err: %d", rc);
		return rc;
	}

	// do get and sync operation before update
	rc = IOT_Shadow_Get_Sync(shadow_client, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("get device shadow failed, err: %d", rc);
		return rc;
	}

	while (IOT_Shadow_IsConnected(shadow_client) || QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT == rc ||
			QCLOUD_RET_MQTT_RECONNECTED == rc || QCLOUD_RET_SUCCESS == rc) {

		rc = IOT_Shadow_Yield(shadow_client, 200);

		if (QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT == rc) {
			HAL_SleepMs(1000);
			continue;
		}
		else if (rc != QCLOUD_RET_SUCCESS && rc != QCLOUD_RET_MQTT_RECONNECTED) {
			Log_e("exit with error: %d", rc);
			return rc;
		}

		if (sg_delta_arrived) {
			rc = IOT_Shadow_Update_Sync(shadow_client, sg_shadow_update_buffer, sg_shadow_update_buffersize, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
			sg_delta_arrived = false;
			if (rc == QCLOUD_RET_SUCCESS) 
				Log_i("shadow update success");
		}

		IOT_Shadow_JSON_ConstructReport(shadow_client, sg_shadow_update_buffer, sg_shadow_update_buffersize, 1, &sg_shadow_property);
		rc = IOT_Shadow_Update(shadow_client, sg_shadow_update_buffer, sg_shadow_update_buffersize, OnShadowUpdateCallback, NULL, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
		sg_current_update_count++;

		// sleep for some time in seconds
		HAL_SleepMs(1000);
	}

	Log_e("loop exit with error: %d", rc);

	rc = IOT_Shadow_Destroy(shadow_client);

	return rc;
}

int main()
{
	IOT_Log_Set_Level(eLOG_DEBUG);

	demo_device_shadow();

	return 0;
}
