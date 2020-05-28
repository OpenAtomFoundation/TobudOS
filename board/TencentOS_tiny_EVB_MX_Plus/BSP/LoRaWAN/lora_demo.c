#include "lora_demo.h"
#include "RHF76.h"
#include "sensor_parser.h"

/*
 ==================================================================================
 data template:

 Type       Name            Token       DataType    RW          Attribute
 property   temperature     temperature integer     readonly    range: [-100, 155]
                                                                initial: 0
                                                                step: 1
                                                                unit: centigrade

 property   humidity        humidity    integer     readonly    range: [-0, 100]
                                                                initial: 0
                                                                step: 1
                                                                unit: %

 property   report_period   period      integer     read-write  range: [0, 3600]
                                                                initial: 0
                                                                step: 1
                                                                unit: second

 ==================================================================================
 up-link parser javascript:

 function RawToProtocol(fPort, bytes) {
  var data = {
      "method": "report",
      "clientToken" : new Date(),
      "params" : {}
  };
  data.params.temperature = bytes[0];
  data.params.humidity = bytes[1];
  data.params.period = bytes[2] | (bytes[3] << 8);
  return data;
 }

 ==================================================================================
 down-link parser javascript:

 function ProtocolToRaw(obj) {
     var data = new Array();
     data[0] = 5;// fport=5
     data[1] = 0;// unconfirmed mode
     data[2] = obj.params.period & 0x00FF;
     data[3] = (obj.params.period >> 8) & 0x00FF;
     return data;
 }

 */

uint16_t report_period = 10;

k_mail_q_t mail_q;
#define DATA_CNT        26
uint8_t mail_buf[DATA_CNT];

typedef struct device_data_st {
    uint8_t     data[DATA_CNT];
} __PACKED__ dev_data_t;

void recv_callback(uint8_t *data, uint8_t len)
{
    int i = 0;

    printf("len: %d\n", len);

    for (i = 0; i < len; ++i) {
        printf("data[%d]: %d\n", i, data[i]);
    }

    if (len == 1) {
        report_period = data[0];
    } else if (len >= 2) {
        report_period = data[0] | (data[1] << 8);
    }
    printf("report_period: %d\n", report_period);
}


#define CMD_LEN_MAX     1024
char cmd_buf[CMD_LEN_MAX];

void uart_output(const char *str)
{
    /* if using c lib printf through uart, a simpler one is: */
    printf(str);
}

dev_data_t dev_data;
uint8_t pool[DATA_CNT];

void application_entry(void *arg)
{
    int i = 0;

    rhf76_lora_init(HAL_UART_PORT_0);
    tos_lora_module_recvcb_register(recv_callback);

    tos_lora_module_join_otaa("8cf957200000025a", "8cf957211111125a1e29bbbbbd204a72");

    tos_mail_q_create(&mail_q, pool, DATA_CNT, sizeof(uint8_t));
    tos_shell_init(cmd_buf, sizeof(cmd_buf), uart_output);

    while (1) {
        size_t mail_size;
        tos_mail_q_pend(&mail_q, &dev_data.data, &mail_size, TOS_TIME_FOREVER);

        for (i = 0; i < mail_size; ++i) {
            printf("[%d] %x\n", i, dev_data.data[i]);
        }
        printf("\n\n");

        tos_lora_module_send(&dev_data.data, mail_size);
    }
}

