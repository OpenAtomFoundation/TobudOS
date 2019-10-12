CSRCS += lv_tutorial_animations.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tutorial/8_animations
VPATH += :$(LVGL_DIR)/lv_examples/lv_tutorial/8_animations

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tutorial/8_animations"
