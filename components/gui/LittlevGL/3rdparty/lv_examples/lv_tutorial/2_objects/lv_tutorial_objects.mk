CSRCS += lv_tutorial_objects.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tutorial/2_objects
VPATH += :$(LVGL_DIR)/lv_examples/lv_tutorial/2_objects

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tutorial/2_objects"
