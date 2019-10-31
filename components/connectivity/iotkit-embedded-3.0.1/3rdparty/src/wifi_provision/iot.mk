LIBA_TARGET     := libiot_awss.a

LIB_SRCS_PATTERN    := *.c

$(call Append_Conditional, LIB_SRCS_PATTERN, smartconfig/*.c, AWSS_SUPPORT_SMARTCONFIG)
$(call Append_Conditional, LIB_SRCS_PATTERN, p2p/*.c, AWSS_SUPPORT_SMARTCONFIG_WPS)
$(call Append_Conditional, LIB_SRCS_PATTERN, zero_config/*.c, AWSS_SUPPORT_ZEROCONFIG)
$(call Append_Conditional, LIB_SRCS_PATTERN, phone_ap/*.c, AWSS_SUPPORT_AHA)
$(call Append_Conditional, LIB_SRCS_PATTERN, router_ap/*.c, AWSS_SUPPORT_ADHA)
$(call Append_Conditional, LIB_SRCS_PATTERN, dev_ap/*.c, AWSS_SUPPORT_DEV_AP)
$(call Append_Conditional, LIB_SRCS_PATTERN, frameworks/*.c, AWSS_FRAMEWORKS)
$(call Append_Conditional, LIB_SRCS_PATTERN, frameworks/*/*.c, AWSS_FRAMEWORKS)
