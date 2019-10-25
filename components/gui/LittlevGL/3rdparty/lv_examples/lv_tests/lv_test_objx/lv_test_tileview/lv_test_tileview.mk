CSRCS += lv_test_tileview.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_tileview
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_tileview

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_tileview"
