CSRCS += lv_test_ta.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_ta
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_ta

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_ta"
