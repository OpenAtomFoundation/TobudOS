LIBA_TARGET     := libiot_sign.a

HDR_REFS        := src/infra

DEPENDS         += wrappers
LDFLAGS         += -liot_sdk -liot_hal -liot_tls

DEPENDS         += external_libs/mbedtls

LIB_SRCS_EXCLUDE        := examples/dev_sign_example.c
SRCS_dev-sign-example   += examples/dev_sign_example.c

$(call Append_Conditional, TARGET, dev-sign-example, DEV_SIGN, BUILD_AOS NO_EXECUTABLES)

