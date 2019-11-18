LIBA_TARGET     := libiot_reset.a

HDR_REFS        := src/infra src/mqtt

DEPENDS         += wrappers
LDFLAGS         += -liot_sdk -liot_hal -liot_tls

LIB_SRCS_EXCLUDE         += examples/dev_reset_example.c
SRCS_dev-reset-example   := examples/dev_reset_example.c

$(call Append_Conditional, TARGET, dev-reset-example, DEV_RESET, BUILD_AOS NO_EXECUTABLES)
