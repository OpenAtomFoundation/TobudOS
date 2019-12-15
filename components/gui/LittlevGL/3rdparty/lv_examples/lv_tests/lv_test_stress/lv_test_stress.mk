CSRCS += lv_test_stress.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_stress
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_stress

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_stress"
