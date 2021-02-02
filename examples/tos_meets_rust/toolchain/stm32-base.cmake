function(stm32_configure_linker_addresses target)
    if(FLASH_ORIGIN)
        target_link_options(${target}
            PRIVATE "LINKER:--defsym=flash_origin=${FLASH_ORIGIN}"
            )
    else()
        # We always need to set FLASH_ORIGIN so that the script
        # toolchain/flash.in gets configured correctly.
        set(FLASH_ORIGIN 0x00000000 PARENT_SCOPE)
    endif()

    if(FLASH_LENGTH)
        target_link_options(${target}
            PRIVATE "LINKER:--defsym=flash_length=${FLASH_LENGTH}"
            )
    endif()

    if(RAM_ORIGIN)
        target_link_options(${target}
            PRIVATE "LINKER:--defsym=ram_origin=${RAM_ORIGIN}"
            )
    endif()

    if(RAM_LENGTH)
        target_link_options(${target}
            PRIVATE "LINKER:--defsym=ram_length=${RAM_LENGTH}"
            )
    endif()
endfunction(stm32_configure_linker_addresses)

if (NOT DEFINED DEVICE)
    message(FATAL_ERROR "No processor defined")
endif ()
message("Device: ${DEVICE}")

# Convert to upper case
string(TOUPPER ${DEVICE} DEVICE_U)
message("Processor: ${DEVICE_U}")

set(CPU_TYPE "m0plus")


# Set compiler flags
# Common arguments
add_definitions("-D${DEVICE}")
add_definitions("-DUSE_HAL_DRIVER")
set(COMMON_DEFINITIONS "-Wextra -Wall -Wno-unused-parameter -mcpu=cortex-${CPU_TYPE} -mthumb -fno-builtin -ffunction-sections -fdata-sections -fomit-frame-pointer -mfpu=fpv4-sp-d16 -mfloat-abi=soft ${OPTIONAL_DEBUG_SYMBOLS}")
set(DEPFLAGS "-MMD -MP")

# Enable FLTO optimization if required
if (USE_FLTO)
    set(OPTFLAGS "-Os -flto")
else ()
    set(OPTFLAGS "-Os")
endif ()

# Build flags
set(CMAKE_C_FLAGS "-std=gnu99 ${COMMON_DEFINITIONS} ${CPU_FIX} --specs=nano.specs ${DEPFLAGS}")
set(CMAKE_CXX_FLAGS "${COMMON_DEFINITIONS} ${CPU_FIX} --specs=nano.specs ${DEPFLAGS}")
set(CMAKE_ASM_FLAGS "${COMMON_DEFINITIONS} -x assembler-with-cpp -DLOOP_ADDR=0x8000")

# Set default inclusions
set(LIBS ${LIBS} -lgcc -lc -lnosys -lgcc -lc -lnosys)

# Debug Flags
set(COMMON_DEBUG_FLAGS "-O0 -g -gdwarf-2")
set(CMAKE_C_FLAGS_DEBUG "${COMMON_DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_DEBUG_FLAGS}")
set(CMAKE_ASM_FLAGS_DEBUG "${COMMON_DEBUG_FLAGS}")

# Release Flags
set(COMMON_RELEASE_FLAGS "${OPTFLAGS} -DNDEBUG=1 -DRELEASE=1")
set(CMAKE_C_FLAGS_RELEASE "${COMMON_RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_RELEASE_FLAGS}")
set(CMAKE_ASM_FLAGS_RELEASE "${COMMON_RELEASE_FLAGS}")

# Print debug info helper function
function(print_debug_info)
    message("COMPILER_PREFIX =${COMPILER_PREFIX}")
    message("CMAKE_SOURCE_DIR =${CMAKE_SOURCE_DIR}")
    message("CMAKE_C_COMPILER =${CMAKE_C_COMPILER}")
    message("CMAKE_C_FLAGS =${CMAKE_C_FLAGS}")
    message("CMAKE_C_LINK_EXECUTABLE =${CMAKE_C_LINK_EXECUTABLE}")
    message("CMAKE_EXE_LINKER_FLAGS =${CMAKE_EXE_LINKER_FLAGS}")
    message("CMAKE_AR =${CMAKE_AR}")

    message("Definitions: ")
    get_directory_property(defs DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS)
    foreach (def ${defs})
        message(STATUS "-D${def}")
    endforeach ()

    get_property(dirs TARGET ${PROJECT_NAME} PROPERTY INCLUDE_DIRECTORIES)
    message("Includes: ")
    foreach (dir ${dirs})
        message(STATUS "${dir}")
    endforeach ()

    get_property(libs TARGET ${PROJECT_NAME} PROPERTY LINK_LIBRARIES)
    message("Libraries:")
    foreach (libs ${libs})
        message(STATUS "${libs}")
    endforeach ()
endfunction()
