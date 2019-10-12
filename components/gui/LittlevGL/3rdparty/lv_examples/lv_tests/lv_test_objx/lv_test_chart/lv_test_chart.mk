CSRCS += lv_test_chart.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_chart
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_chart

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_chart"
