.PHONY: run_demo_test demo_test_clean run_unit_test run_multi_thread_test \
        coap_run_demo_test coap_demo_test_clean

tests: run_demo_test run_unit_test run_multi_thread_test coap_run_demo_test
  