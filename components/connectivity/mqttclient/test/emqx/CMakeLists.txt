aux_source_directory(. DIR_SRCS)

set(INCDIRS ${CMAKE_CURRENT_SOURCE_DIR})

add_executable("emqx" ${DIR_SRCS})

foreach(findlib ${LIBNAMES})
    target_link_libraries("emqx" ${findlib})
endforeach()

find_package("Threads")
target_link_libraries("emqx" ${CMAKE_THREAD_LIBS_INIT})