CSRCS += lv_test_ddlist.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_ddlist
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_ddlist

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_ddlist"
