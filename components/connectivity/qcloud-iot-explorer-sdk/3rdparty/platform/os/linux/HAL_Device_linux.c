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
#include <stdarg.h>
#include <memory.h>

#include "qcloud_iot_import.h"
#include "qcloud_iot_export.h"

#include "utils_param_check.h"

/* Enable this macro (also control by cmake) to use static string buffer to store device info */
/* To use specific storing methods like files/flash, disable this macro and implement dedicated methods */
//#define DEBUG_DEV_INFO_USED

#ifdef DEBUG_DEV_INFO_USED
/* product Id  */
static char sg_product_id[MAX_SIZE_OF_PRODUCT_ID + 1]    = "PRODUCT_ID";

/* device name */
static char sg_device_name[MAX_SIZE_OF_DEVICE_NAME + 1]  = "YOUR_DEV_NAME";

#ifdef DEV_DYN_REG_ENABLED
/* product secret for device dynamic Registration  */
static char sg_product_secret[MAX_SIZE_OF_PRODUCT_SECRET + 1]  = "YOUR_PRODUCT_SECRET";
#endif

#ifdef AUTH_MODE_CERT
/* public cert file name of certificate device */
static char sg_device_cert_file_name[MAX_SIZE_OF_DEVICE_CERT_FILE_NAME + 1]      = "YOUR_DEVICE_NAME_cert.crt";
/* private key file name of certificate device */
static char sg_device_privatekey_file_name[MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME + 1] = "YOUR_DEVICE_NAME_private.key";
#else
/* device secret of PSK device */
static char sg_device_secret[MAX_SIZE_OF_DEVICE_SECRET + 1] = "YOUR_IOT_PSK";
#endif

#ifdef GATEWAY_ENABLED
/* sub-device product id  */
//static char sg_sub_device_product_id[MAX_SIZE_OF_PRODUCT_ID + 1]   = "PRODUCT_ID";
/* sub-device device name */
//static char sg_sub_device_name[MAX_SIZE_OF_DEVICE_NAME + 1]  = "YOUR_SUB_DEV_NAME";

#ifdef DEBUG_DEV_INFO_USED
static const DeviceInfo sg_subdevList[] = {
    {.product_id = "WPDA0S6S08", .device_name = "dev001"},
    {.product_id = "WPDA0S6S08", .device_name = "dev002"},
    {.product_id = "WPDA0S6S08", .device_name = "dev003"},

    {.product_id = "Y8T6NB8DM0", .device_name = "test001"},
    {.product_id = "Y8T6NB8DM0", .device_name = "test002"},
    {.product_id = "Y8T6NB8DM0", .device_name = "test003"}
};
#endif

#endif


static int device_info_copy(void *pdst, void *psrc, uint8_t max_len)
{
    if (strlen(psrc) > max_len) {
        return QCLOUD_ERR_FAILURE;
    }
    memset(pdst, '\0', max_len);
    strncpy(pdst, psrc, max_len);
    return QCLOUD_RET_SUCCESS;
}

int HAL_SetDevInfoFile(const char *file_name)
{
    Log_w("device info is not stored in file!!");

    return QCLOUD_RET_SUCCESS;
}

#else
/*device info manage example method*/

#include "lite-utils.h"

#ifdef GATEWAY_ENABLED
#define MAX_DEV_INFO_FILE_LEN       10*1024
#else
#define MAX_DEV_INFO_FILE_LEN       1024
#endif
#define MAX_CONFIG_FILE_NAME        256


#define KEY_AUTH_MODE               "auth_mode"
#define KEY_PRODUCT_ID              "productId"
#define KEY_PRODUCT_SECRET          "productSecret"
#define KEY_DEV_NAME                "deviceName"
#define KEY_DEV_SECRET              "key_deviceinfo.deviceSecret"
#define KEY_DEV_CERT                "cert_deviceinfo.devCertFile"
#define KEY_DEV_PRIVATE_KEY         "cert_deviceinfo.devPrivateKeyFile"

#define KEY_SUBDEV_PRODUCT_ID       "sub_productId"
#define KEY_SUBDEV_NAME             "sub_devName"
#define KEY_SUBDEV_NUM              "subDev.subdev_num"
#define KEY_SUBDEV_LIST             "subDev.subdev_list"


#define STR_DEV_INFO                "key_deviceinfo"
#define STR_DEV_SECRET              "deviceSecret"
#define STR_DEV_CERT                "cert_deviceinfo"
#define STR_DEV_CERT_FILE           "devCertFile"
#define STR_DEV_KEY_FILE            "devPrivateKeyFile"

#define MAX_SIZE_OF_DEVICE_INFO_FILE    128
static char sg_device_info_file[MAX_SIZE_OF_DEVICE_INFO_FILE + 1] = {"./device_info.json"};

int HAL_SetDevInfoFile(const char *file_name)
{
    if (file_name == NULL || strlen(file_name) > MAX_SIZE_OF_DEVICE_INFO_FILE) {
        Log_e("illegal device info file name!");
        return QCLOUD_ERR_INVAL;
    }

    /* check if file exists */
    if (access(file_name, 0)) {
        Log_e("device info file \"%s\" not existed", file_name);
        return QCLOUD_ERR_FAILURE;
    }

    memset(sg_device_info_file, 0, sizeof(sg_device_info_file));
    strncpy(sg_device_info_file, file_name, MAX_SIZE_OF_DEVICE_INFO_FILE);
    return QCLOUD_RET_SUCCESS;
}

static int iot_parse_devinfo_from_json_file(DeviceInfo *pDevInfo)
{
    FILE *fp;
    uint32_t len;
    uint32_t rlen;
    int ret = QCLOUD_RET_SUCCESS;

    fp = fopen(sg_device_info_file, "r");
    if (NULL == fp) {
        Log_e("open device info file \"%s\" failed", sg_device_info_file);
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }

    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    if (len > MAX_DEV_INFO_FILE_LEN) {
        Log_e("device info file \"%s\" is too big!", sg_device_info_file);
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }

    char *JsonDoc = (char *)HAL_Malloc(len + 10);
    memset(JsonDoc, 0, len + 10);
    if (NULL == JsonDoc) {
        Log_e("malloc buffer for json file read fail");
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }

    rewind(fp);
    rlen = fread(JsonDoc, 1, len, fp);

    if (len != rlen) {
        Log_e("read data len (%d) less than needed (%d), %s", rlen, len, JsonDoc);
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }

    /*Get device info*/
    char* authMode = LITE_json_value_of(KEY_AUTH_MODE, JsonDoc);
    if (NULL == authMode) {
        HAL_Free(JsonDoc);
        Log_e("read authcode from json file failed!");
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }

    char* productId = LITE_json_value_of(KEY_PRODUCT_ID, JsonDoc);
    if (NULL == productId) {
        HAL_Free(JsonDoc);
        HAL_Free(authMode);
        Log_e("read product id from json file failed!");
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }

    char* devName = LITE_json_value_of(KEY_DEV_NAME, JsonDoc);
    if (NULL == devName) {
        HAL_Free(JsonDoc);
        HAL_Free(authMode);
        HAL_Free(productId);
        Log_e("read device name from json file failed!");
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }

    strncpy(pDevInfo->product_id, productId, MAX_SIZE_OF_PRODUCT_ID);
    strncpy(pDevInfo->device_name, devName, MAX_SIZE_OF_DEVICE_NAME);

#ifdef DEV_DYN_REG_ENABLED
    char* productSecret = LITE_json_value_of(KEY_PRODUCT_SECRET, JsonDoc);
    if (NULL == productSecret) {
        HAL_Free(JsonDoc);
        HAL_Free(authMode);
        HAL_Free(productId);
        HAL_Free(devName);
        Log_e("read product secret key from json file failed!");
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }
    strncpy(pDevInfo->product_secret, productSecret, MAX_SIZE_OF_PRODUCT_SECRET);
#endif

#ifdef  AUTH_MODE_CERT
    char* devCrtFileName = LITE_json_value_of(KEY_DEV_CERT, JsonDoc);
    if (NULL == devCrtFileName) {
        HAL_Free(JsonDoc);
        HAL_Free(authMode);
        HAL_Free(productId);
        HAL_Free(devName);
#ifdef DEV_DYN_REG_ENABLED
        HAL_Free(productSecret);
#endif
        Log_e("read device crt file name from json file failed!");
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }
    char* devKeyFileName = LITE_json_value_of(KEY_DEV_PRIVATE_KEY, JsonDoc);
    if (NULL == devKeyFileName) {
        HAL_Free(JsonDoc);
        HAL_Free(authMode);
        HAL_Free(productId);
        HAL_Free(devName);
        HAL_Free(devCrtFileName);
#ifdef  DEV_DYN_REG_ENABLED
        HAL_Free(productSecret);
#endif
        Log_e("read device private key file name from json file failed!");
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }
    strncpy(pDevInfo->dev_cert_file_name, devCrtFileName, MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);
    strncpy(pDevInfo->dev_key_file_name, devKeyFileName, MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME);
    //Log_d("mode:%s, pid:%s, devName:%s, devCrtFileName:%s, devKeyFileName:%s", authMode, productId, devName,  devCrtFileName, devKeyFileName);
#else
    char* devSecret = LITE_json_value_of(KEY_DEV_SECRET, JsonDoc);
    if (NULL == devSecret) {
        HAL_Free(JsonDoc);
        HAL_Free(authMode);
        HAL_Free(productId);
        HAL_Free(devName);
#ifdef  DEV_DYN_REG_ENABLED
        HAL_Free(productSecret);
#endif
        Log_e("read device secret key from json file failed!");
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }
    strncpy(pDevInfo->device_secret, devSecret, MAX_SIZE_OF_DEVICE_SECRET);
    //Log_d("mode:%s, pid:%s, devName:%s, devSerect:%s", authMode, productId, devName,  devSecret);
#endif

    if (authMode) {
        HAL_Free(authMode);
    }

    if (productId) {
        HAL_Free(productId);
    }

    if (devName) {
        HAL_Free(devName);
    }

#ifdef DEV_DYN_REG_ENABLED
    if (productSecret) {
        HAL_Free(productSecret);
    }
#endif

#ifdef  AUTH_MODE_CERT
    if (devCrtFileName) {
        HAL_Free(devCrtFileName);
    }

    if (devKeyFileName) {
        HAL_Free(devKeyFileName);
    }
#else
    if (devSecret) {
        HAL_Free(devSecret);
    }
#endif

    if (NULL != JsonDoc) {
        HAL_Free(JsonDoc);
    }

exit:
    if (NULL != fp) {
        fclose(fp);
    }
    return ret;
}

#ifdef GATEWAY_ENABLED

static int iot_get_subdev_from_list(char* devList, DeviceInfo *pDevInfo, int *pDevNum)
{
#define MAX_LEN_DEV_INFO    (MAX_SIZE_OF_PRODUCT_ID + MAX_SIZE_OF_DEVICE_NAME + 128)
	int devNum = *pDevNum;
    int count = 0;
    int ret = QCLOUD_RET_SUCCESS;
    char *pNext;
    char TempBuff[MAX_LEN_DEV_INFO];

    if (devList == NULL || pDevInfo == NULL || devNum == 0) {
        return QCLOUD_ERR_INVAL;
    }

    pNext = (char *)strtok(devList, "}");
    while (pNext != NULL) {
        memset(TempBuff, '\0', MAX_LEN_DEV_INFO);
        HAL_Snprintf(TempBuff, MAX_LEN_DEV_INFO, "%s}", pNext);
        char *pos = strchr(TempBuff, '{');
		if(NULL == pos){
			*pDevNum = count;
			break;
		}

        char* productId = LITE_json_value_of(KEY_SUBDEV_PRODUCT_ID, pos);
        if (productId) {
            strncpy(pDevInfo[count].product_id, productId, MAX_SIZE_OF_PRODUCT_ID);
            HAL_Free(productId);
            productId = NULL;
        } else {
            ret = QCLOUD_ERR_INVAL;
            break;
        }

        char* devName = LITE_json_value_of(KEY_SUBDEV_NAME, pos);
        if (devName) {
            strncpy(pDevInfo[count].device_name, devName, MAX_SIZE_OF_DEVICE_NAME);
            HAL_Free(devName);
            devName = NULL;
        } else {
            ret = QCLOUD_ERR_INVAL;
            break;
        }

        count++;
        pNext = (char *)strtok(NULL, "}");

        if (count > (devNum - 1)) {
            break;
        }
    }

    return ret;
#undef   MAX_LEN_DEV_INFO
}

static int iot_parse_subdevinfo_from_json_file(DeviceInfo *pDevInfo, int *subDevNum)
{
    FILE *fp;
    uint32_t len;
    uint32_t rlen;
    char *JsonDoc = NULL;
    int ret = QCLOUD_RET_SUCCESS;

    fp = fopen(sg_device_info_file, "r");
    if (NULL == fp) {
        Log_e("open device info file \"%s\" failed", sg_device_info_file);
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }

    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    if (len > MAX_DEV_INFO_FILE_LEN) {
        Log_e("device info file \"%s\" is too big!", sg_device_info_file);
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }

    JsonDoc = (char *)HAL_Malloc(len + 10);
    memset(JsonDoc, 0, len + 10);
    if (NULL == JsonDoc) {
        Log_e("malloc buffer for json file read fail");
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }

    rewind(fp);
    rlen = fread(JsonDoc, 1, len, fp);

    if (len != rlen) {
        Log_e("read data len (%d) less than needed (%d), %s", rlen, len, JsonDoc);
    }

    /*Get sub device num*/
    char* devNum = LITE_json_value_of(KEY_SUBDEV_NUM, JsonDoc);
    if (devNum) {
        *subDevNum = atoi(devNum);
        *subDevNum = (*subDevNum > MAX_NUM_SUB_DEV) ? MAX_NUM_SUB_DEV : (*subDevNum);
        HAL_Free(devNum);
    } else {
        Log_e("fail to parse dev num");
        ret =  QCLOUD_ERR_FAILURE;
        goto exit;
    }

    /*Get sub info*/
    char* devList = LITE_json_value_of(KEY_SUBDEV_LIST, JsonDoc);
    if (devList) {  //json utils not support array parse currently, we will update json utils
        ret = iot_get_subdev_from_list(devList, pDevInfo, subDevNum);
        HAL_Free(devList);
    } else {
        Log_e("fail to parse devList");
        ret =  QCLOUD_ERR_FAILURE;
    }

exit:

    if (JsonDoc) {
        HAL_Free(JsonDoc);
    }

    if (NULL != fp) {
        fclose(fp);
    }

    return ret;
}
#endif

static int iot_save_devinfo_to_json_file(DeviceInfo *pDevInfo)
{
    char JsonDoc[MAX_DEV_INFO_FILE_LEN] = {0};
    int32_t remain_size = MAX_DEV_INFO_FILE_LEN;
    int32_t rc_of_snprintf = 0;
    int32_t wlen;
    FILE *fp;

    rc_of_snprintf = HAL_Snprintf(JsonDoc, remain_size, "{\n");
    remain_size -= rc_of_snprintf;

    //auth_mode
#ifdef  AUTH_MODE_CERT
    rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size, \
                                  "\"%s\":\"%s\",\n", \
                                  KEY_AUTH_MODE, "CERT"
                                 );
    remain_size -= rc_of_snprintf;
#else
    rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size, \
                                  "\"%s\":\"%s\",\n", \
                                  KEY_AUTH_MODE, "KEY"
                                 );
    remain_size -= rc_of_snprintf;
#endif

    //product id, device name
    rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size,
                                  "\"%s\":\"%s\",\n\"%s\":\"%s\",\n", \
                                  KEY_PRODUCT_ID, pDevInfo->product_id, \
                                  KEY_DEV_NAME, pDevInfo->device_name);
    remain_size -= rc_of_snprintf;

    //product secret
#ifdef DEV_DYN_REG_ENABLED
    rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size,
                                  "\"%s\":\"%s\",\n",
                                  KEY_PRODUCT_SECRET, pDevInfo->product_secret);
    remain_size -= rc_of_snprintf;
#endif

    // key device info or cert device info
#ifdef  AUTH_MODE_CERT
    rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size, "\"%s\":{\n", STR_DEV_CERT);
    remain_size -= rc_of_snprintf;
    rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size, \
                                  "\"%s\":\"%s\",\n\"%s\":\"%s\"\n}", \
                                  STR_DEV_CERT_FILE, pDevInfo->dev_cert_file_name, \
                                  STR_DEV_KEY_FILE, pDevInfo->dev_key_file_name \
                                 );
    remain_size -= rc_of_snprintf;
#else
    rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size, "\"%s\":{\n", STR_DEV_INFO);
    remain_size -= rc_of_snprintf;
    rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size,  \
                                  "\"%s\":\"%s\"\n}", \
                                  STR_DEV_SECRET, pDevInfo->device_secret);
    remain_size -= rc_of_snprintf;
#endif

    rc_of_snprintf = HAL_Snprintf(JsonDoc + strlen(JsonDoc), remain_size, "\n}");
    remain_size -= rc_of_snprintf;

    Log_d("JsonDoc(%d):%s", MAX_DEV_INFO_FILE_LEN - remain_size, JsonDoc);

    fp = fopen(sg_device_info_file, "w");
    if (NULL == fp) {
        Log_e("open file %s failed", sg_device_info_file);
        return  QCLOUD_ERR_FAILURE;
    }

    wlen = fwrite(JsonDoc, 1, strlen(JsonDoc), fp);
    if (wlen < (MAX_DEV_INFO_FILE_LEN - remain_size)) {
        Log_e("write len less than needed");
    }
    fclose(fp);
    return QCLOUD_RET_SUCCESS;
}
#endif

int HAL_SetDevInfo(void *pdevInfo)
{
    POINTER_SANITY_CHECK(pdevInfo, QCLOUD_ERR_DEV_INFO);
    int ret;
    DeviceInfo *devInfo = (DeviceInfo *)pdevInfo;

#ifdef DEBUG_DEV_INFO_USED
    ret  = device_info_copy(sg_product_id, devInfo->product_id, MAX_SIZE_OF_PRODUCT_ID);//set product ID
    ret |= device_info_copy(sg_device_name, devInfo->device_name, MAX_SIZE_OF_DEVICE_NAME);//set dev name

#ifdef  AUTH_MODE_CERT
    ret |= device_info_copy(sg_device_cert_file_name, devInfo->dev_cert_file_name, MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);//set dev cert file name
    ret |= device_info_copy(sg_device_privatekey_file_name, devInfo->dev_key_file_name, MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME);//set dev key file name
#else
    ret |= device_info_copy(sg_device_secret, devInfo->device_secret, MAX_SIZE_OF_DEVICE_SECRET);//set dev secret
#endif

#else
    ret = iot_save_devinfo_to_json_file(devInfo);
#endif

    if (QCLOUD_RET_SUCCESS != ret) {
        Log_e("Set device info err");
        ret = QCLOUD_ERR_DEV_INFO;
    }
    return ret;
}

int HAL_GetDevInfo(void *pdevInfo)
{
    POINTER_SANITY_CHECK(pdevInfo, QCLOUD_ERR_DEV_INFO);
    int ret;
    DeviceInfo *devInfo = (DeviceInfo *)pdevInfo;
    memset((char *)devInfo, '\0', sizeof(DeviceInfo));

#ifdef DEBUG_DEV_INFO_USED
    ret  = device_info_copy(devInfo->product_id, sg_product_id, MAX_SIZE_OF_PRODUCT_ID);//get product ID
    ret |= device_info_copy(devInfo->device_name, sg_device_name, MAX_SIZE_OF_DEVICE_NAME);//get dev name

#ifdef DEV_DYN_REG_ENABLED
    ret |= device_info_copy(devInfo->product_secret, sg_product_secret, MAX_SIZE_OF_PRODUCT_SECRET );//get product ID
#endif

#ifdef  AUTH_MODE_CERT
    ret |= device_info_copy(devInfo->dev_cert_file_name, sg_device_cert_file_name, MAX_SIZE_OF_DEVICE_CERT_FILE_NAME);//get dev cert file name
    ret |= device_info_copy(devInfo->dev_key_file_name, sg_device_privatekey_file_name, MAX_SIZE_OF_DEVICE_SECRET_FILE_NAME);//get dev key file name
#else
    ret |= device_info_copy(devInfo->device_secret, sg_device_secret, MAX_SIZE_OF_DEVICE_SECRET);//get dev secret
#endif

#else
    ret = iot_parse_devinfo_from_json_file(devInfo);
#endif

    if (QCLOUD_RET_SUCCESS != ret) {
        Log_e("Get device info err");
        ret = QCLOUD_ERR_DEV_INFO;
    }
    return ret;
}

#ifdef GATEWAY_ENABLED
int HAL_GetGwDevInfo(void *pgwDeviceInfo)
{
    POINTER_SANITY_CHECK(pgwDeviceInfo, QCLOUD_ERR_DEV_INFO);
    int ret;
    int i;

    GatewayDeviceInfo *gwDevInfo = (GatewayDeviceInfo *)pgwDeviceInfo;
    memset((char *)gwDevInfo, 0, sizeof(GatewayDeviceInfo));

#ifdef DEBUG_DEV_INFO_USED
    ret  = HAL_GetDevInfo(&(gwDevInfo->gw_info));//get gw dev info
    if (sizeof(sg_subdevList[]) / sizeof(sg_subdevList[0]) > MAX_NUM_SUB_DEV) {
        gwDevInfo->sub_dev_num = MAX_NUM_SUB_DEV;
    } else {
        gwDevInfo->sub_dev_num = sizeof(sg_subdevList[]) / sizeof(sg_subdevList[0]);
    }

    for (i = 0; i < gwDevInfo->sub_dev_num; i++) {
        //copy sub dev info
        ret = device_info_copy(gwDevInfo->sub_dev_info[i].product_id, sg_subdevList[i].product_id, MAX_SIZE_OF_PRODUCT_ID);
        ret |= device_info_copy(gwDevInfo->sub_dev_info[i].device_name, sg_subdevList[i].device_name, MAX_SIZE_OF_DEVICE_NAME);
    }

#else
    ret = iot_parse_devinfo_from_json_file(&(gwDevInfo->gw_info));
    if (ret != QCLOUD_RET_SUCCESS) {
        return QCLOUD_ERR_FAILURE;
    }

    //copy sub dev info
    memset((char *)gwDevInfo->sub_dev_info, '\0', MAX_NUM_SUB_DEV * sizeof(DeviceInfo));
    ret = iot_parse_subdevinfo_from_json_file(gwDevInfo->sub_dev_info, &(gwDevInfo->sub_dev_num));

#endif

    if (QCLOUD_RET_SUCCESS != ret) {
        Log_e("Get gateway device info err");
        ret = QCLOUD_ERR_DEV_INFO;
    } else {
        Log_d("sub device num:%d", gwDevInfo->sub_dev_num);
        for (i = 0; i < gwDevInfo->sub_dev_num; i++) {
            Log_d("%dth subDevPid:%s subDevName:%s", i, gwDevInfo->sub_dev_info[i].product_id, gwDevInfo->sub_dev_info[i].device_name);
        }
    }
    return ret;
}
#endif
