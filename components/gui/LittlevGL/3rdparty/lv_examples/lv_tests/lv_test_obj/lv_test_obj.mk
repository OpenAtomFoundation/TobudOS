CSRCS += lv_test_obj.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_obj
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_obj

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_obj"
