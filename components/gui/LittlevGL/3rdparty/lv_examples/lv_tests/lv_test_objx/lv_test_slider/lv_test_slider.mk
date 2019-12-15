CSRCS += lv_test_slider.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_slider_bar
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_slider

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_slider"
