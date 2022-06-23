# 移远SDK适配

1. 将该目录的内容拷贝到`ql-application\qcloud-iot-c-sdk`下，并将sdk放置在`ql-application\qcloud-iot-c-sdk\iot_c_sdk`目录下

```c
├── CMakeLists.txt
├── config
├── iot_c_sdk
└── qcloud_iot_mqtt_demo.c
```

2. 修改相应的编译脚本

- `core_export.list` 添加相应的函数

    ```c
    //md5
    mbedtls_md5_init
    mbedtls_md5_starts
    mbedtls_md5_update
    mbedtls_md5_finish

    // 
    mbedtls_ctr_drbg_free
    mbedtls_entropy_free
    mbedtls_ctr_drbg_init
    mbedtls_entropy_init
    mbedtls_ctr_drbg_seed
    mbedtls_entropy_func
    mbedtls_ctr_drbg_random
    ```

- `ql-application/CMakeLists.txt`修改

    ```cmake
    if(QL_APP_FEATURE_QCLOUD_IOT) 
    add_subdirectory_if_exist(qcloud-iot-c-sdk)
    endif()
    ```

- `ql-application/init/ql_init.c`修改

    ```c
    #ifdef QL_APP_FEATURE_QCLOUD_IOT
    #include "qcloud_iot_common.h"
    #include "qcloud_iot_hub.h"
    #include "qcloud_iot_explorer.h"
    #endif
    ```

    ```c
    #ifdef QL_APP_FEATURE_QCLOUD_IOT
    //your application entry
    extern int qcloud_iot_mqtt_demo(int argc, char **argv);
    qcloud_iot_mqtt_demo(0, NULL);
    #endif
    ```
