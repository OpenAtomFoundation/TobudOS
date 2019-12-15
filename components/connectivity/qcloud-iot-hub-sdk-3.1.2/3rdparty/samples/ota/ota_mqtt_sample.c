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
#include <string.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "lite-utils.h"


#ifdef AUTH_MODE_CERT
    static char sg_cert_file[PATH_MAX + 1];      // full path of device cert file
    static char sg_key_file[PATH_MAX + 1];       // full path of device key file
#endif


static DeviceInfo sg_devInfo;


#define OTA_BUF_LEN (5000)

static bool sg_pub_ack = false;
static int sg_packet_id = 0;

static void event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg) 
{	
	uintptr_t packet_id = (uintptr_t)msg->msg;

	switch(msg->event_type) {
		case MQTT_EVENT_UNDEF:
			Log_i("undefined event occur.");
			break;

		case MQTT_EVENT_DISCONNECT:
			Log_i("MQTT disconnect.");
			break;

		case MQTT_EVENT_RECONNECT:
			Log_i("MQTT reconnect.");
			break;

		case MQTT_EVENT_SUBCRIBE_SUCCESS:
			Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_SUBCRIBE_TIMEOUT:
			Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_SUBCRIBE_NACK:
			Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_SUCCESS:
			Log_i("publish success, packet-id=%u", (unsigned int)packet_id);
            if (sg_packet_id == packet_id)
                sg_pub_ack = true;
			break;

		case MQTT_EVENT_PUBLISH_TIMEOUT:
			Log_i("publish timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_NACK:
			Log_i("publish nack, packet-id=%u", (unsigned int)packet_id);
			break;
		default:
			Log_i("Should NOT arrive here.");
			break;
	}
}

/* demo of firmware info management in device side for resuming download from break point */
#define VERSION_FILE_PATH  	"./local_fw_info.json"
#define KEY_VER				"version"
#define KEY_MD5				"md5"
#define KEY_SIZE			"downloadSize"
#define KEY_STATE			"state"
#define KEY_PREVER			"running_version"
static char * get_local_fw_version(char **ver, char **md5, char **size)
{
#define INFO_FILE_MAX_LEN	256

	FILE *fp;
	int len;
	int rlen;
	char *preVer;
	char *reportVer = NULL;  
	
	fp = fopen(VERSION_FILE_PATH, "r");	
    if(NULL == fp) {
        Log_e("open file %s failed", VERSION_FILE_PATH);
		goto exit;
    }

	fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
	if(len > INFO_FILE_MAX_LEN){
		Log_e("%s is too big, pls check", VERSION_FILE_PATH);
		goto exit;
	}

	char *JsonDoc = (char *)HAL_Malloc(len + 10);
	if(NULL == JsonDoc){
		Log_e("malloc buffer for json file read fail"); 
		goto exit;
	}
	
	rewind(fp);
	rlen = fread(JsonDoc, 1, len, fp);
	
	if(len != rlen){
		Log_e("read data len (%d) less than needed (%d), %s", rlen, len, JsonDoc);
	}

	*ver = LITE_json_value_of(KEY_VER, JsonDoc);
	*md5 = LITE_json_value_of(KEY_MD5, JsonDoc);
	*size = LITE_json_value_of(KEY_SIZE, JsonDoc);
	preVer  = LITE_json_value_of(KEY_PREVER, JsonDoc);
	
	if((NULL != *ver) && (NULL != preVer) && (0 == strcmp(*ver, preVer))){
		reportVer = *ver;
		HAL_Free(preVer);		 
	}else{
		reportVer = preVer;
	}
		
exit:
	
	if(NULL != fp){
		fclose(fp);
	}

	return reportVer;
#undef INFO_FILE_MAX_LEN	
}

/* update local firmware info for resuming download from break point */
static int update_local_fw_info(const char *version, const char *preVer, const char *md5, uint32_t downloadedSize)
{
#define INFO_FILE_MAX_LEN	256

	FILE *fp;
	int wlen;
	int ret = QCLOUD_RET_SUCCESS;
	char dataBuff[INFO_FILE_MAX_LEN];

	memset(dataBuff, 0, INFO_FILE_MAX_LEN);
	HAL_Snprintf(dataBuff, INFO_FILE_MAX_LEN, "{\"%s\":\"%s\", \"%s\":\"%s\",\"%s\":%d,\"%s\":\"%s\"}",\
						KEY_VER, version, KEY_MD5, md5, KEY_SIZE, downloadedSize, \
						KEY_PREVER, (NULL == preVer)?"1.0.0": preVer); 

	fp = fopen(VERSION_FILE_PATH, "w");	
    if(NULL == fp) {
        Log_e("open file %s failed", VERSION_FILE_PATH);
		ret = QCLOUD_ERR_FAILURE;
		goto exit;
    }

	wlen = fwrite(dataBuff, 1, strlen(dataBuff), fp);
	if(wlen != strlen(dataBuff)){
		Log_e("save version to file err");
		ret = QCLOUD_ERR_FAILURE;
	} 
	
exit:
	
	if(NULL != fp){
		fclose(fp);
	}

	return ret;
#undef INFO_FILE_MAX_LEN	
}

/* get local firmware offset for resuming download from break point */
static int getFwOffset(void *h_ota, char *local_ver, char *local_md5, char *local_size, uint32_t *offset, uint32_t *size_file)
{
	char version[128], md5sum[33];
	uint32_t local_len;
	int Ret;

	Ret = IOT_OTA_Ioctl(h_ota, IOT_OTAG_VERSION, version, 128);
	Ret |= IOT_OTA_Ioctl(h_ota, IOT_OTAG_MD5SUM, md5sum, 33);
	Ret |= IOT_OTA_Ioctl(h_ota, IOT_OTAG_FILE_SIZE, size_file, 4);

	local_len = (NULL == local_size)?0:atoi(local_size);

	
	if((NULL == local_ver)||(NULL == local_md5)||(NULL == local_size)){
		*offset = 0;	
	}
	else if((0 != strcmp(local_ver, version))||(0 != strcmp(local_md5, md5sum))||(local_len > *size_file)){
		*offset = 0;
	}else {		
		*offset = local_len;	
	}
	 
	return Ret;
}

/* calculate left MD5 for resuming download from break point */
static int cal_exist_fw_md5(void *h_ota, FILE *fp, size_t size){
#define BUFF_LEN 1024

	 char buff[BUFF_LEN];
	 size_t rlen;
	 int Ret = QCLOUD_RET_SUCCESS;
	 		 
	 while((size > 0) && (!feof(fp))){
		rlen = (size > BUFF_LEN)?BUFF_LEN:size;
	 	if(rlen != fread(buff, 1, rlen, fp)){
			Log_e("read data len not expected");
			Ret = QCLOUD_ERR_FAILURE;
			break;
		}
		IOT_OTA_UpdateClientMd5(h_ota, buff, rlen);
		size -= rlen;
	 }
	 return Ret;
#undef 	BUFF_LEN 
}

static int _setup_connect_init_params(MQTTInitParams* initParams)
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
    initParams->event_handle.h_fp = event_handler;

    return QCLOUD_RET_SUCCESS;
}

int main(int argc, char **argv) 
{
    IOT_Log_Set_Level(eLOG_DEBUG);
    int rc;
	uint32_t firmware_valid;
    char version[128], md5sum[33];
    uint32_t size_downloaded, size_file;
    int len;	
	int ota_over = 0;
    bool upgrade_fetch_success = true;
    char buf_ota[OTA_BUF_LEN];
	FILE *fp = NULL;
	uint32_t offset = 0;


    MQTTInitParams init_params = DEFAULT_MQTTINIT_PARAMS;
    rc = _setup_connect_init_params(&init_params);
	if (rc != QCLOUD_RET_SUCCESS) {
		Log_e("init params err,rc=%d", rc);
		return rc;
	}

    void *client = IOT_MQTT_Construct(&init_params);
    if (client != NULL) {
        Log_i("Cloud Device Construct Success");
    } else {
        Log_e("Cloud Device Construct Failed");
        return QCLOUD_ERR_FAILURE;
    }

    void *h_ota = IOT_OTA_Init(sg_devInfo.product_id, sg_devInfo.device_name, client);
    if (NULL == h_ota) {
        Log_e("initialize OTA failed");
        return QCLOUD_ERR_FAILURE;
    }

	IOT_MQTT_Yield(client, 1000);  //make sure subscribe success
	
	char *local_ver = NULL, *local_md5 = NULL, *local_size = NULL, *reportVer = NULL;
	reportVer = get_local_fw_version(&local_ver, &local_md5, &local_size);	
	Log_d("local_ver:%s local_md5:%s, local_size:%s", local_ver, local_md5, local_size);
	
    /* Must report version first */
    if (0 > IOT_OTA_ReportVersion(h_ota, (NULL == reportVer)?"1.0.0": reportVer)) {
        Log_e("report OTA version failed");
        goto exit;
    }

    do {
        Log_i("wait for ota upgrade command...");

        IOT_MQTT_Yield(client, 200);
        
        if (IOT_OTA_IsFetching(h_ota)) {
						
			/*check pre-download finished or not*/
			/*if version & MD5 is the same,then pre-download not finished,get breakpoint continue download.otherwise the version is new*/		
			rc = getFwOffset(h_ota, local_ver, local_md5, local_size, &offset, &size_file);
			if(QCLOUD_RET_SUCCESS != rc){
				Log_e("get fw offset err,rc:%d",rc);
				upgrade_fetch_success = false;
				break;
			}
			
			/*cal file md5*/
			//Log_d("Get offset:%d(%x)", offset, offset);
			if(offset > 0){
				if (NULL == (fp = fopen("ota.bin", "ab+"))) {
			        Log_e("open file failed");
					upgrade_fetch_success = false;
			        break;
			    }
				
				if(QCLOUD_RET_SUCCESS != cal_exist_fw_md5(h_ota, fp, offset)){
					Log_e("cal exist fw md5 failed");
					upgrade_fetch_success = false;
					break;
				}
			}else{
				if (NULL == (fp = fopen("ota.bin", "wb+"))) {
			        Log_e("open file failed");
					upgrade_fetch_success = false;
					break;
				}
			}
			
			/*set offset and start http connect*/		
			fseek(fp, offset, SEEK_SET);
			rc = IOT_OTA_StartDownload(h_ota, offset, size_file);
			if(QCLOUD_RET_SUCCESS != rc){
				Log_e("OTA download start err,rc:%d",rc);
				upgrade_fetch_success = false;
				break;
			}
	
            do {
                len = IOT_OTA_FetchYield(h_ota, buf_ota, OTA_BUF_LEN, 1);
                if (len > 0) {
                    if (1 != fwrite(buf_ota, len, 1, fp)) {
                        Log_e("write data to file failed");
                        upgrade_fetch_success = false;
                        break;
                    }
                } else if (len < 0) {
                    Log_e("download fail rc=%d", len);
                    upgrade_fetch_success = false;
                    break;
                }				
				fflush(fp);
				
                /* get OTA information and update local info */
                IOT_OTA_Ioctl(h_ota, IOT_OTAG_FETCHED_SIZE, &size_downloaded, 4);
                IOT_OTA_Ioctl(h_ota, IOT_OTAG_FILE_SIZE, &size_file, 4);
                IOT_OTA_Ioctl(h_ota, IOT_OTAG_MD5SUM, md5sum, 33);
                IOT_OTA_Ioctl(h_ota, IOT_OTAG_VERSION, version, 128);			
				rc = update_local_fw_info(version, reportVer, md5sum, size_downloaded);
				if(QCLOUD_RET_SUCCESS != rc){
					Log_e("update local fw info err,rc:%d", rc);
				}

                IOT_MQTT_Yield(client, 100);
				
            } while (!IOT_OTA_IsFetchFinish(h_ota));
			
			fclose(fp);
			fp = NULL;
	
            /* Must check MD5 match or not */ 
            if (upgrade_fetch_success) {
                IOT_OTA_Ioctl(h_ota, IOT_OTAG_CHECK_FIRMWARE, &firmware_valid, 4);
                if (0 == firmware_valid) {
                    Log_e("The firmware is invalid");
                    upgrade_fetch_success = false;
					rc = update_local_fw_info(NULL, reportVer, NULL, 0);
					if(QCLOUD_RET_SUCCESS != rc){
						Log_e("update local fw info err,rc:%d", rc);
					}
                } else {
                    Log_i("The firmware is valid");
                    upgrade_fetch_success = true;
                }
            }

            ota_over = 1;
        }

        HAL_SleepMs(2000);
    } while(!ota_over);


    if (upgrade_fetch_success)
    {
        /* begin execute OTA files, should report upgrade begin */ 
         sg_packet_id = IOT_OTA_ReportUpgradeBegin(h_ota);
         if (0 > sg_packet_id) {
             Log_e("report OTA begin failed error:%d", sg_packet_id);
             return QCLOUD_ERR_FAILURE;
         }
         while (!sg_pub_ack) {
             HAL_SleepMs(1000);
             IOT_MQTT_Yield(client, 200);
         }
         sg_pub_ack = false;
	
		 //* add your own upgrade logic here*//
		 //* fw_upgrade.....		 		

		if(QCLOUD_RET_SUCCESS == rc){
		    /* if upgrade success */
	        /* after execute OTA files, should report upgrade result */
	         sg_packet_id = IOT_OTA_ReportUpgradeSuccess(h_ota, NULL);
	         if (0 > sg_packet_id) {
	             Log_e("report OTA result failed error:%d", sg_packet_id);
	             return QCLOUD_ERR_FAILURE;
	         }
	         while (!sg_pub_ack) {
	             HAL_SleepMs(1000);
	             IOT_MQTT_Yield(client, 200);
	         }
			 rc = update_local_fw_info(version, version, md5sum, size_downloaded); // just for example, add your own logic 
	         sg_pub_ack = false;	

		}else{
			 /* if upgrade fail */
	        sg_packet_id = IOT_OTA_ReportUpgradeFail(h_ota, NULL); 
	        if (0 > sg_packet_id) {
	             Log_e("report OTA result failed error:%d", sg_packet_id);
	             return QCLOUD_ERR_FAILURE;
	         }
	         while (!sg_pub_ack) {
	             HAL_SleepMs(1000);
	             IOT_MQTT_Yield(client, 200);
	        }
			rc = update_local_fw_info(NULL, reportVer, NULL, 0); // just for example, add your own logic 
	        sg_pub_ack = false;	

		}        
    }

exit:

	if(NULL != fp){
		fclose(fp);
		fp = NULL;
	}

	if(NULL != local_ver){
		reportVer = (reportVer == local_ver)?NULL: reportVer;
		HAL_Free(local_ver);
		local_ver = NULL;
	}
	
	if(NULL != local_md5){
		HAL_Free(local_md5);
		local_md5 = NULL;
	}

	if(NULL != local_size){
		HAL_Free(local_size);
		local_size = NULL;
	}

	if(NULL != reportVer){
		HAL_Free(reportVer);
		reportVer = NULL;
	}

    IOT_OTA_Destroy(h_ota);

    IOT_MQTT_Destroy(&client);
    
    return 0;
}
