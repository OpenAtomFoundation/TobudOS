CSRCS += lv_test_table.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_table
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_table

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_table"
