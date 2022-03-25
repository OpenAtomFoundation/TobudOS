file(GLOB src_app ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_executable(app_ota ${src_app})
target_link_libraries(app_ota ${libsdk})
