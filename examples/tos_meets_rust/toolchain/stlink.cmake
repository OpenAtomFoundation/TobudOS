# JLink functions
# Adds targets for stlink programmers and emulators


add_custom_target(flash
	COMMAND st-flash --reset write ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin 0x8000000
        DEPENDS ${PROJECT_NAME}
	)


add_custom_target(erase
	COMMAND st-flash erase
        DEPENDS ${PROJECT_NAME}
	)


add_custom_target(reset
	COMMAND st-flash reset
        DEPENDS ${PROJECT_NAME}
	)
