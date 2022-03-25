file(GLOB src_app ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_executable(app_data_template ${src_app})
target_link_libraries(app_data_template ${libsdk})
