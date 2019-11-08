# README.md: coap cloud

## Contents

```shell
.
├── aos.mk
├── Cloud_CoAPDeserialize.h
├── Cloud_CoAPExport.h
├── Cloud_CoAPMessage.h
├── Cloud_CoAPNetwork.h
├── Cloud_CoAPPlatform.h
├── Cloud_CoAPSerialize.h
├── CMakeLists.txt
├── CoAPDeserialize.c
├── CoAPExport.c
├── CoAPMessage.c
├── CoAPNetwork.c
├── CoAPSerialize.c
├── Config.in
├── iot.mk
├── iotx_ca_cert.c
└── iotx_coap_api.c

```

## Introduction
Implementation of coap protocol and special customization for connecting alicloud iot platform


### Features



### Dependencies

- **hal**. osal and hal to shield different os and hardware
- **infra**. Authentication, net and so on tool set.

## API
Please refer to  [coap api](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/CoAP_Provides#iot_coap_init) for API details.
## Reference
Please refer to  [coap connect](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/CoAP_Connect) for example details.


