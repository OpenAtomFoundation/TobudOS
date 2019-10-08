Generate a custom Profile in Bluetooth Developer Studio
    - Create a custom profile:
        - File->New;
        - Rename 'New Profile' to, for example 'Test Profile'
    - Create a custom service:
        - CUSTOM SERVICE;
        - Rename 'TEST PROFILE NEW SERVICE' to , for example 'Test Profile Test service'
    - Add a new characteristic:
        - Click on '+'
        - Rename the characteristic to, for example 'RX'
        - Set Read/Write/Notify/Indicate properties, for example, 'Write Without Response' to 'Mandatory'
        - Add Fields to the characteristic
            - Add one 'value' field with format '_VARIABLE'
    - Add as many characteristics as needed (for example add one more 'TX' variable with with Notify property to 'Mandatory' and a '_VARIABLE' field named 'value')
    - Generate code
        - Tools->Generate Code
        - Choose 'Server' and 'NORDIC SEMICONDUCTOR NRF51 PLUGIN'

Using a custom Profile in 'my_project'
    - Include the generated h file:
        #include "ble_test_profile_test_service.h"
    - If you want to use logging, define APP_LOG macro
        #define APPL_LOG NRF_LOG_INFO
    - Declare a service structure variable:
        static ble_test_profile_test_service_t      m_tests;
    - Create a service event handler function:
        static void on_test_evt(ble_test_profile_test_service_t * p_test_profile_test_service,
                                ble_test_profile_test_service_evt_t * p_evt)
        {
            switch (p_evt->evt_type)
            {
                case BLE_TEST_PROFILE_TEST_SERVICE_RX_EVT_WRITE:
                    APPL_LOG("Charact written with value %s. ", p_evt->params.rx.value.p_data);
                    break;

                default:
                    // No implementation needed.
                    break;
            }
        }
    - add the service initialization in services_init(void)
        uint32_t                             err_code;
        ble_test_profile_test_service_init_t tests_init;

        // Initialize XXX Service.
        memset(&tests_init, 0, sizeof(tests_init));

        tests_init.evt_handler                = on_test_evt;
        //tests_init.is_xxx_notify_supported    = true;
        tests_init.ble_test_profile_test_service_rx_initial_value.value.size = 20;
        tests_init.ble_test_profile_test_service_rx_initial_value.value.p_data = rx_value;
        tests_init.ble_test_profile_test_service_tx_initial_value.value.size = 20;
        tests_init.ble_test_profile_test_service_tx_initial_value.value.p_data = tx_value;

        err_code = ble_test_profile_test_service_init(&m_tests, &tests_init);
        APP_ERROR_CHECK(err_code);
    - add the call to the service's on_ble_event API in ble_evt_dispatch function
        ble_test_profile_test_service_on_ble_evt(&m_tests, p_ble_evt);
    - in the keil project, add the service source file (add existing file to group nRF_BLE, choose 'ble_test_profile_test_service.c')
