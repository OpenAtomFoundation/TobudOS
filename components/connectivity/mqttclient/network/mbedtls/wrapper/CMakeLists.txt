aux_source_directory(. WARAPPER_SRCS)

add_library("wrapper" ${CMAKE_LIB_TYPE} ${WARAPPER_SRCS})
target_link_libraries("wrapper" "platform")



