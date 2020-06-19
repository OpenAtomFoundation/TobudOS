set(SUBDIRS "log")

aux_source_directory(. DIR_SRCS)

string(REGEX REPLACE ".*/(.*)" "\\1" LIB_NAME ${CMAKE_CURRENT_SOURCE_DIR}) 

if (DIR_SRCS)
    foreach(libname ${LIBNAMES})
        if (${LIB_NAME} STREQUAL ${libname})
            add_library(${libname} ${CMAKE_LIB_TYPE} ${DIR_SRCS})
        endif()
    endforeach()

else()
    message(WARNING "not find is src file!")
endif()


foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
endforeach()
