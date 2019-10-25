CSRCS += tpcal.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_apps/tpcal
VPATH += :$(LVGL_DIR)/lv_examples/lv_apps/tpcal

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_apps/tpcal"
