set(SUBDIRS "linux")

foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
endforeach()


