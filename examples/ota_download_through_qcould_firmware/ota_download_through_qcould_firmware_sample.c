#include "mcu_init.h"
#include "esp8266_tencent_firmware.h"
#include "tencent_firmware_module_wrapper.h"

#include "tos_k.h"
#include "tos_kv.h"

#include "ota_flash.h"
#include "ota_partition.h"
#include "ota_env.h"

extern ota_flash_drv_t stm32l4_norflash_onchip_drv_ota;
extern ota_flash_prop_t stm32l4_norflash_onchip_prop_ota;

ota_fw_info_t ota_fw_info;

#define READ_OTA_DATA_BUF_LEN   256
uint8_t ota_fw_data_buffer[READ_OTA_DATA_BUF_LEN];

#define CUR_VERSION "0.1"

void ota_download_demo_task(void)
{
    mqtt_state_t state;
    
    int read_len = 0;
    uint8_t crc = 0, the_crc;
    ota_img_vs_t new_version;
    int remain_len = 0;

    char *product_id = "WDRRDCF1TE";
    char *device_name = "dev1";
    char *key = "ULtbpSxXtSQyaFyeaax6pw==";

    device_info_t dev_info;
    memset(&dev_info, 0, sizeof(device_info_t));
    
    printf("---------------------------\r\n");
    printf("|          ver:%s        |\r\n", CUR_VERSION);
    printf("---------------------------\r\n");
    

    esp8266_tencent_firmware_sal_init(HAL_UART_PORT_0);
    esp8266_tencent_firmware_join_ap("Mculover666", "mculover666");

    strncpy(dev_info.product_id, product_id, PRODUCT_ID_MAX_SIZE);
    strncpy(dev_info.device_name, device_name, DEVICE_NAME_MAX_SIZE);
    strncpy(dev_info.device_serc, key, DEVICE_SERC_MAX_SIZE);
    tos_tf_module_info_set(&dev_info, TLS_MODE_PSK);

	mqtt_param_t init_params = DEFAULT_MQTT_PARAMS;
	if (tos_tf_module_mqtt_conn(init_params) != 0) {
		printf("module mqtt conn fail\n");
	} else {
		printf("module mqtt conn success\n");
	}

    if (tos_tf_module_mqtt_state_get(&state) != -1) {
        printf("MQTT: %s\n", state == MQTT_STATE_CONNECTED ? "CONNECTED" : "DISCONNECTED");
    }

    uint32_t partition_addr = 0x0803f800;

    if (ota_env_init(OTA_UPDATE_IN_POSITION, partition_addr, &stm32l4_norflash_onchip_drv_ota, &stm32l4_norflash_onchip_prop_ota) < 0) {
        printf("env init failed!\n");
        return;
    }
    else {
        printf("env init success\r\n");
    }
    
    if(tos_tf_module_ota_set(OTA_ENABLE, CUR_VERSION)) {
        printf("module ota enable failed!\n");
        return;
    }
    else {
         printf("module ota enable success!\n");
    }
    
    /* read info until success */
    if(!tos_tf_module_ota_read_fwinfo(&ota_fw_info)) {
        printf("fw info in module read success, fw_version=%s, fw_size=%d!\n", ota_fw_info.fw_version, ota_fw_info.fw_size);
    }
    
    /* is the file too large? */
    if (ota_fw_info.fw_size > ota_partition_size(OTA_PARTITION_OTA)) {
        printf("OTA partition is too small, file size is %d bytes!", ota_fw_info.fw_size);
        return;
    }
    
    /* 1. make flash ready */
    uint32_t flash_addr = ota_partition_start(OTA_PARTITION_OTA);
    if (ota_flash_erase_blocks(flash_addr, ota_fw_info.fw_size) < 0) {
        printf("Erase Flash fail!\r\n");
        return;
    }
    
    /* 2. deal with image header */
    remain_len = ota_fw_info.fw_size;
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
    read_len = MIN(remain_len, READ_OTA_DATA_BUF_LEN);
    if(tos_tf_module_ota_read_fwdata(ota_fw_data_buffer, read_len) != read_len) {
        return;
    }
    
    if (read_len < sizeof(ota_img_hdr_t)) {
        /* read_len at least larger than ota_img_hdr_t */
        return;
    }

    if (ota_flash_write(flash_addr, ota_fw_data_buffer, ota_flash_write_size_aligned_get(read_len)) < 0) {
        return;
    }

    /* get original image header */
    the_crc = ((ota_img_hdr_t *)ota_fw_data_buffer)->patch_crc;
    /* patch version */
    new_version = ((ota_img_hdr_t *)ota_fw_data_buffer)->new_version;

    /* calculate image header crc */
    crc = ota_img_hdr_crc((ota_img_hdr_t *)ota_fw_data_buffer);
    /* calculate remain image body crc */
    crc = crc8(crc, ota_fw_data_buffer + sizeof(ota_img_hdr_t), read_len - sizeof(ota_img_hdr_t));

    flash_addr += read_len;
    remain_len -= read_len;
    
    /* 3. read file content and write to flash */
    while (remain_len > 0) {
        read_len = MIN(remain_len, sizeof(ota_fw_data_buffer));
        if (tos_tf_module_ota_read_fwdata(ota_fw_data_buffer, read_len) != read_len) {
            return;
        }

        if (ota_flash_write(flash_addr, ota_fw_data_buffer, ota_flash_write_size_aligned_get(read_len)) < 0) {
            return;
        }

        crc = crc8(crc, ota_fw_data_buffer, read_len);

        flash_addr += read_len;
        remain_len -= read_len;
    }
    
    /* 4. crc check */
    if (the_crc != crc) {
        return;
    }

    if (tos_kv_set("new_version", &new_version, sizeof(ota_img_vs_t)) != KV_ERR_NONE) {
        return;
    }

    return;
}


void application_entry(void *arg)
{
    ota_download_demo_task();
    while (1) {
        printf("Please reset your board to continue ota update!\r\n");
        tos_task_delay(1000);
    }
}

