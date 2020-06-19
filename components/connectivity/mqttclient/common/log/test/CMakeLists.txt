set(EXECUTABLE_OUTPUT_PATH "${PROJECT_ROOT_PATH}/${OUTDIRS}/bin/")

aux_source_directory(. CURRENT_DIR_SRCS)

add_executable(${TARGETS} ${CURRENT_DIR_SRCS})

foreach(findlib ${LIBNAMES})
    target_link_libraries(${TARGETS} ${findlib})
endforeach()

find_package("Threads")
target_link_libraries(${TARGETS} ${CMAKE_THREAD_LIBS_INIT})

