LIBA_TARGET := libiot_hal.a
CFLAGS      := $(filter-out -ansi,$(CFLAGS))

LIB_SRCS_PATTERN    += os/$(CONFIG_VENDOR)/*.c

$(call Append_Conditional, LIB_SRCS_PATTERN, tls/*.c, _PLATFORM_IS_LINUX_ SUPPORT_TLS)
$(call Append_Conditional, LIB_SRCS_PATTERN, tls/*.c, _PLATFORM_IS_LINUX_ COAP_DTLS_SUPPORT)
$(call Append_Conditional, LIB_SRCS_PATTERN, atm/at_tcp/mk3060.c, AT_TCP_ENABLED AT_TCP_HAL_MK3060)
$(call Append_Conditional, LIB_SRCS_PATTERN, atm/at_tcp/sim800.c, AT_TCP_ENABLED AT_TCP_HAL_SIM800)
$(call Append_Conditional, LIB_SRCS_PATTERN, atm/at_mqtt/mqtt_ica.c, AT_MQTT_ENABLED AT_MQTT_HAL_ICA)
$(call Append_Conditional, LIB_SRCS_PATTERN, atm/at_mqtt/mqtt_sim800.c, AT_MQTT_ENABLED AT_MQTT_HAL_SIM800)
$(call Append_Conditional, LIB_SRCS_EXCLUDE, os/ubuntu/HAL_UART_linux.c, , AT_PARSER_ENABLED)
