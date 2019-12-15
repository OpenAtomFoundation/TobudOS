CSRCS += lv_test_btnm.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_btnm
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_btnm

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_btnm"
