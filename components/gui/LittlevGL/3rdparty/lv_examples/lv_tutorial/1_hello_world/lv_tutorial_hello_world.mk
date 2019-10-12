CSRCS += lv_tutorial_hello_world.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tutorial/1_hello_world
VPATH += :$(LVGL_DIR)/lv_examples/lv_tutorial/1_hello_world

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tutorial/1_hello_world"
