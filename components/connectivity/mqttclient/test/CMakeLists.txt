set(SUBDIRS "emqx" "onenet" "baidu" "ali")

foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
endforeach()
