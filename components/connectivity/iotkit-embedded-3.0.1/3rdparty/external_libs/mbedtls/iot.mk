LIBA_TARGET := libiot_tls.a

HDR_REFS    += src/infra

CFLAGS := $(filter-out -Wdeclaration-after-statement,$(CFLAGS))
CFLAGS := $(filter-out -ansi,$(CFLAGS))
