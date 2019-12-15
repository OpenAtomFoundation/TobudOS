CSRCS += lv_test_sw.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_sw
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_sw

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_sw"
