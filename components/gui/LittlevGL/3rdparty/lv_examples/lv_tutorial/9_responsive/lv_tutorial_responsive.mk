CSRCS += lv_tutorial_responsive.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tutorial/9_responsive
VPATH += :$(LVGL_DIR)/lv_examples/lv_tutorial/9_responsive

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tutorial/9_responsive"
