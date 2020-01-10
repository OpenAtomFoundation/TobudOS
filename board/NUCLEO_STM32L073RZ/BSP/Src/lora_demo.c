#include "lora_demo.h"
#include "HTS221.h"
#include "RHF76.h"

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

typedef struct device_data_st {
    uint8_t     temperature;
    uint8_t     humidity;
    uint16_t    period;
} __PACKED__ dev_data_t;

typedef struct device_data_wrapper_st {
    union {
        dev_data_t  dev_data;
        uint8_t     serialize[sizeof(dev_data_t)];
    } u;
} dev_data_wrapper_t;

dev_data_wrapper_t dev_data_wrapper;

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

void application_entry(void *arg)
{
    int16_t temperature;
    int16_t humidity;

    HTS221_Init();

    rhf76_lora_init(HAL_UART_PORT_1);
    tos_lora_module_recvcb_register(recv_callback);

    tos_lora_module_join_otaa("8cf957200000fa57", "8cf957200000fa572059aaaaad204a72");

    while (1) {
        HTS221_Get_Temperature(&temperature);
        HTS221_Get_Humidity(&humidity);

        printf("temperature: %2.1f\n", temperature / 10.0);
        printf("humidity   : %2.1f\n", humidity / 10.0);

        dev_data_wrapper.u.dev_data.temperature = temperature / 10;
        dev_data_wrapper.u.dev_data.humidity    = humidity / 10;
        dev_data_wrapper.u.dev_data.period      = report_period;

        tos_lora_module_send(dev_data_wrapper.u.serialize, sizeof(dev_data_t));
        tos_task_delay(report_period * 1000);
    }
}

