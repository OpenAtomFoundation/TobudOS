LIBA_TARGET         := libiot_dev_bind.a
LIB_SRCS_PATTERN    := *.c

$(call Append_Conditional, LIB_SRCS_PATTERN, impl/*.c, DEV_BIND_ENABLED)
$(call Append_Conditional, LIB_SRCS_PATTERN, impl/os/*.c, DEV_BIND_ENABLED)
$(call Append_Conditional, LIB_SRCS_PATTERN, impl/awss_reset/*.c, DEV_BIND_ENABLED)
