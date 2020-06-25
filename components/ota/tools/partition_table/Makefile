TARGET			?= ptbl

CC			:= gcc
LD			:= ld

OUT_ROOT		:= out
OBJ_DIR			:= obj
LIB_DIR			:= lib
TARGET_DIR		:= target
OUT_DIR			:= $(OUT_ROOT)/$(OBJ_DIR) $(OUT_ROOT)/$(LIB_DIR) $(OUT_ROOT)/$(TARGET_DIR)

NOECHO			:= @

INCDIRS 		:= \
				../../common/image			\
				../../common/crc				\
				../../common/partition		\
				../../recovery/include

SRCDIRS			:= \
				../../common/image			\
				../../common/crc				\
				src

INCLUDE			:= $(patsubst %, -I %, $(INCDIRS))

CFILES			:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

CFILENDIR		:= $(notdir $(CFILES))

COBJS			:= $(patsubst %, $(OUT_ROOT)/$(OBJ_DIR)/%, $(CFILENDIR:.c=.o))
OBJS			:= $(COBJS)

VPATH			:= $(SRCDIRS)

.PHONY: clean $(OUT_DIR)

$(TARGET): $(OUT_DIR) $(OBJS)
	@echo "LD $(TARGET)"
	$(NOECHO)$(CC) -o $(OUT_ROOT)/$(TARGET_DIR)/$(TARGET) $(OBJS)

$(OUT_DIR) :
	@mkdir -p $@

$(COBJS) : $(OUT_ROOT)/$(OBJ_DIR)/%.o : %.c
	@echo "CC $(notdir $<)"
	$(NOECHO)$(CC) -Wall -c -O2 $(INCLUDE) -o $@ $<

clean:
	@rm -rf $(OUT_ROOT)

