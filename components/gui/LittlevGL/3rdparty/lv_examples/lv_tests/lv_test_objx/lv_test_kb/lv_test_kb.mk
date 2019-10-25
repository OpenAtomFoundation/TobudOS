CSRCS += lv_test_kb.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_kb
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_kb

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_kb"
