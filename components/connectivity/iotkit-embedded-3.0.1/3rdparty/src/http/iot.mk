LIBA_TARGET     := libiot_http.a

HDR_REFS        += src/infra

DEPENDS         += wrappers
LDFLAGS         += -liot_sdk -liot_hal -liot_tls

TARGET          := http-example

LIB_SRCS_EXCLUDE        := examples/http_example.c
SRCS_http-example   	+= examples/http_example.c