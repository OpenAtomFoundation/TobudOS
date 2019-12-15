LIBA_TARGET     := libiot_alink.a

HDR_REFS        += src/infra
HDR_REFS		+= src/mqtt
HDR_REFS		+= src/dev_sign

DEPENDS         += wrappers
LDFLAGS         += -liot_sdk -liot_hal -liot_tls

LIB_SRCS_PATTERN     	:= *.c server/*.c client/*.c deprecated/*.c

LIB_SRCS_EXCLUDE     	  := examples/linkkit_example_solo.c examples/cJSON.c
SRCS_linkkit-example-solo := examples/linkkit_example_solo.c examples/cJSON.c

LIB_SRCS_EXCLUDE             += examples/linkkit_example_gateway.c examples/cJSON.c
SRCS_linkkit-example-gateway := examples/linkkit_example_gateway.c examples/cJSON.c

$(call Append_Conditional, LIB_SRCS_PATTERN, alcs/*.c, ALCS_ENABLED)

ifneq (,$(filter -DDEPRECATED_LINKKIT,$(CFLAGS)))
$(call Append_Conditional, TARGET, linkkit-example-solo, DEVICE_MODEL_ENABLED, BUILD_AOS NO_EXECUTABLES DEVICE_MODEL_GATEWAY)
$(call Append_Conditional, TARGET, linkkit-example-gateway, DEVICE_MODEL_ENABLED DEVICE_MODEL_GATEWAY, BUILD_AOS NO_EXECUTABLES)
else
$(call Append_Conditional, TARGET, linkkit-example-solo, DEVICE_MODEL_ENABLED, BUILD_AOS NO_EXECUTABLES)
$(call Append_Conditional, TARGET, linkkit-example-gateway, DEVICE_MODEL_GATEWAY, BUILD_AOS NO_EXECUTABLES)
endif

