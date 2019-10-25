CSRCS += lv_tutorial_keyboard.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tutorial/10_keyboard
VPATH += :$(LVGL_DIR)/lv_examples/lv_tutorial/10_keyboard

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tutorial/10_keyboard"
