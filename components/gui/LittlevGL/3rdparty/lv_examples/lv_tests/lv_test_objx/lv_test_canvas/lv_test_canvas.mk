CSRCS += lv_test_canvas.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_canvas
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_canvas

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_canvas"
