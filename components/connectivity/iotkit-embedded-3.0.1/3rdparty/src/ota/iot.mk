LIBA_TARGET     := libiot_ota.a

HDR_REFS        := src/infra

DEPENDS         += wrappers
LDFLAGS         += -liot_sdk -liot_hal -liot_tls

LIB_SRCS_PATTERN := *.c

LIB_SRCS_EXCLUDE        += examples/ota_example_mqtt.c
SRCS_ota-example-mqtt   := examples/ota_example_mqtt.c

$(call Append_Conditional, TARGET, ota-example-mqtt, OTA_ENABLED, BUILD_AOS NO_EXECUTABLES)
