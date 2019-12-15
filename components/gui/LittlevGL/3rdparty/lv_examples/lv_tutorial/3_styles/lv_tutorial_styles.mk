CSRCS += lv_tutorial_styles.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tutorial/3_styles
VPATH += :$(LVGL_DIR)/lv_examples/lv_tutorial/3_styles

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tutorial/3_styles"
