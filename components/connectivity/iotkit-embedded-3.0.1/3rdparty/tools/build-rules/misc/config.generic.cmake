CONFIG_ENV_CFLAGS   += \
    -Os -Wall \
    -D_PLATFORM_IS_HOST_ \

LDFLAGS             += -lpthread -lrt
