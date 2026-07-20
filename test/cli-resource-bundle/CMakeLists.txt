
set(RB_INPUT "${CMAKE_SOURCE_DIR}/test/resources/resourceBundle/smack.properties")
set(RB_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/rb_smack.h")

add_custom_command(
    OUTPUT ${RB_OUTPUT}
    COMMAND $<TARGET_FILE:smack> generate-resource-bundle ${RB_INPUT} ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS smack ${RB_INPUT}
    COMMENT "Generating rb_smack.h from smack.properties"
    VERBATIM
)

add_library( rb_smack INTERFACE )
target_sources( rb_smack INTERFACE ${RB_OUTPUT} )
target_include_directories( rb_smack INTERFACE ${CMAKE_CURRENT_BINARY_DIR} )

add_executable( rb-smack-test
  cli-resource-bundle.cpp
)

target_link_libraries( rb-smack-test
  rb_smack
  smack_cpp
)
