CSRCS += lv_tutorial_themes.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tutorial/4_themes
VPATH += :$(LVGL_DIR)/lv_examples/lv_tutorial/4_themes

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tutorial/4_themes"
