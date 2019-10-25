CSRCS += lv_test_arc.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_arc
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_arc

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_arc"
