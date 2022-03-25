file(GLOB src_cos ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(src_services ${src_services} ${src_cos} PARENT_SCOPE)

file(GLOB src_cos_download_sample ${CMAKE_CURRENT_SOURCE_DIR}/sample/cos_download_sample.c)
add_executable(cos_download_sample ${src_cos_download_sample})
target_link_libraries(cos_download_sample ${libsdk})
