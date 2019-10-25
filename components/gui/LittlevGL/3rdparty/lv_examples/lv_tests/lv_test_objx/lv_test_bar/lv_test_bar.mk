CSRCS += lv_test_bar.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_bar
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_bar

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_bar"
