if (CONFIG_QCLOUD_LLSYNC_SUPPORT)
    set(srcs
            src/core/ble_qiot_llsync_data.c
            src/core/ble_qiot_llsync_device.c
            src/core/ble_qiot_llsync_event.c
            src/core/ble_qiot_service.c
            src/utils/ble_qiot_utils_base64.c
            src/utils/ble_qiot_utils_hmac.c
            src/utils/ble_qiot_utils_log.c
            src/utils/ble_qiot_utils_md5.c
            src/utils/ble_qiot_utils_sha1.c)

    list(APPEND srcs
            date_template/ble_qiot_template.c)

    list(APPEND srcs
            hal/ble_qiot_ble_device.c
            hal/ble_qiot_ble_service.c)

    set(includes inc src/internal_inc)
    set(priv_includes date_template src/internal_inc hal)

endif ()

idf_component_register(SRCS "${srcs}"
        INCLUDE_DIRS "${includes}"
        PRIV_INCLUDE_DIRS "${priv_includes}"
        REQUIRES bt)
