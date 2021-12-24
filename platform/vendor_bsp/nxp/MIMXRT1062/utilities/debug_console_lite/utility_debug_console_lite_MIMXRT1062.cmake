if(NOT UTILITY_DEBUG_CONSOLE_LITE_MIMXRT1062_INCLUDED)
    
    set(UTILITY_DEBUG_CONSOLE_LITE_MIMXRT1062_INCLUDED true CACHE BOOL "utility_debug_console_lite component is included.")

    target_sources(${MCUX_SDK_PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/fsl_debug_console.c
    )


    target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/.
    )


    include(component_lpuart_adapter_MIMXRT1062)

    include(driver_common_MIMXRT1062)

endif()
