CSRCS += sysmon.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_apps/sysmon
VPATH += :$(LVGL_DIR)/lv_examples/lv_apps/sysmon

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_apps/sysmon"
