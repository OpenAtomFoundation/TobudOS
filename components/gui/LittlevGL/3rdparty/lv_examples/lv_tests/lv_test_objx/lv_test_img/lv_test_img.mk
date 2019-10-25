CSRCS += lv_test_img.c
CSRCS += img_flower_icon.c 

DEPPATH += --dep-path $(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_img
VPATH += :$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_img

CFLAGS += "-I$(LVGL_DIR)/lv_examples/lv_tests/lv_test_objx/lv_test_img"
