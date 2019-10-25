CSRCS += lv_test_cb.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_cb
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_cb

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_cb"
