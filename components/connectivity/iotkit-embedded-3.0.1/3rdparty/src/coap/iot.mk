LIBA_TARGET         := libiot_coap.a
HDR_REFS            := src/infra
LIB_SRCS_PATTERN    := *.c

$(call Append_Conditional, LIB_SRCS_PATTERN, client/*.c, COAP_CLIENT)
$(call Append_Conditional, LIB_SRCS_PATTERN, CoAPPacket/*.c, COAP_PACKET)
$(call Append_Conditional, LIB_SRCS_PATTERN, server/*.c, COAP_SERVER)

LDFLAGS         	+= -liot_sdk -liot_hal -liot_tls

$(call Append_Conditional, LIB_SRCS_EXCLUDE, examples/coap_example.c, COAP_CLIENT)
$(call Append_Conditional, SRCS_coap-example, examples/coap_example.c, COAP_CLIENT)
$(call Append_Conditional, TARGET, coap-example, COAP_CLIENT)