CSRCS += lv_test_line.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_line
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_line

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_line"
