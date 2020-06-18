aux_source_directory(. DIR_SRCS)

set(INCDIRS ${CMAKE_CURRENT_SOURCE_DIR})

add_executable("onenet" ${DIR_SRCS})

foreach(findlib ${LIBNAMES})
    target_link_libraries("onenet" ${findlib})
endforeach()

find_package("Threads")
target_link_libraries("onenet" ${CMAKE_THREAD_LIBS_INIT})