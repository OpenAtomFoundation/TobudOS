CSRCS += lv_test_list.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_list
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_list

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_list"
