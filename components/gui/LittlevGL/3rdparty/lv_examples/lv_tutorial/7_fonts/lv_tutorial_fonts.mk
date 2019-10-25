CSRCS += lv_tutorial_fonts.c
CSRCS += arial_20.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tutorial/7_fonts
VPATH += :$(LVGL_DIR)/lv_examples/lv_tutorial/7_fonts

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tutorial/7_fonts"
