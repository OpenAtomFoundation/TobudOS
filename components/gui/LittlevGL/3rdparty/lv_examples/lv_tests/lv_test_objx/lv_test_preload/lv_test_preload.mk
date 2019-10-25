CSRCS += lv_test_preload.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_preload
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_preload

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_preload"
