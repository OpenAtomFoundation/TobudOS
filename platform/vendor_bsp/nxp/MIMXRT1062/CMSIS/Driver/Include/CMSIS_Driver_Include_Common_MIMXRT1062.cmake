if(NOT CMSIS_DRIVER_INCLUDE_COMMON_MIMXRT1062_INCLUDED)
    
    set(CMSIS_DRIVER_INCLUDE_COMMON_MIMXRT1062_INCLUDED true CACHE BOOL "CMSIS_Driver_Include_Common component is included.")


    target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/.
    )

endif()
