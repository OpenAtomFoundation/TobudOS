aux_source_directory(. DIR_SRCS)

set(INCDIRS ${CMAKE_CURRENT_SOURCE_DIR})

add_executable("ali" ${DIR_SRCS})

foreach(findlib ${LIBNAMES})
    target_link_libraries("ali" ${findlib})
endforeach()

find_package("Threads")
target_link_libraries("ali" ${CMAKE_THREAD_LIBS_INIT})