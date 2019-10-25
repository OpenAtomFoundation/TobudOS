CSRCS += lv_test_gauge.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_gauge
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_gauge

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_gauge"
