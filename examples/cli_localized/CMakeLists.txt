set( RB_NAME "cli_localized" )
set(RB_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/resourceBundle/${RB_NAME}.properties")
set(RB_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/rb_${RB_NAME}.h")

add_custom_command(
    OUTPUT ${RB_OUTPUT}
    COMMAND $<TARGET_FILE:smack> generate-resource-bundle ${RB_INPUT} ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS smack ${RB_INPUT}
    COMMENT "Generating rb_${RB_NAME}.h from ${RB_NAME}.properties"
    VERBATIM
)

add_library( rb_cli_localized INTERFACE )
target_sources( rb_cli_localized INTERFACE ${RB_OUTPUT} )
target_include_directories( rb_cli_localized INTERFACE ${CMAKE_CURRENT_BINARY_DIR} )

add_executable( cli_localized
    cli_localized.cpp
    rb_cli_localized
)
target_link_libraries( cli_localized
  rb_cli_localized
  smack_cpp
)
