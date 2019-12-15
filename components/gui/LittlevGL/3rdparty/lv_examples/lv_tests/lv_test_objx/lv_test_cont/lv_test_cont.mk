CSRCS += lv_test_cont.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_cont
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_cont

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_cont"
