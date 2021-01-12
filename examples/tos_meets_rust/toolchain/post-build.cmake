# ARM post build commands
# Copyright (c) 2016 Ryan Kurte
# This file is covered under the MIT license available at: https://opensource.org/licenses/MIT

# Create binary file
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${OBJCOPY} -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${OBJCOPY} -O ihex ${PROJECT_NAME} ${PROJECT_NAME}.hex)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${OBJDUMP} -d -S ${PROJECT_NAME} > ${PROJECT_NAME}.dmp)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${OBJSIZE} ${PROJECT_NAME})
