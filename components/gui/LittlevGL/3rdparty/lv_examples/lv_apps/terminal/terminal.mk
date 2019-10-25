CSRCS += terminal.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_apps/terminal
VPATH += :$(LVGL_DIR)/lv_examples/lv_apps/terminal

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_apps/terminal"
