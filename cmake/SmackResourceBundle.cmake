# smack_add_resource_bundle(NAME <name> INPUT <path_to_properties>)
#
# Creates an INTERFACE library target named rb_<name> that exposes the
# code-generated header rb_<name>.h produced from a .properties file.
#
# The smack tool (target: smack) must be available in the build tree.
#
# Parameters:
#   NAME   - Base name used for both the library target (rb_<name>) and
#             the generated header (rb_<name>.h).
#   INPUT  - Absolute path to the source .properties file.
#
function(smack_add_resource_bundle)
    cmake_parse_arguments(ARG "" "NAME;INPUT" "" ${ARGN})

    if(NOT ARG_NAME)
        message(FATAL_ERROR "smack_add_resource_bundle: NAME is required")
    endif()
    if(NOT ARG_INPUT)
        message(FATAL_ERROR "smack_add_resource_bundle: INPUT is required")
    endif()

    set(_rb_output "${CMAKE_CURRENT_BINARY_DIR}/rb_${ARG_NAME}.h")

    add_custom_command(
        OUTPUT  ${_rb_output}
        COMMAND $<TARGET_FILE:smack> generate-resource-bundle ${ARG_INPUT} ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS smack ${ARG_INPUT}
        COMMENT "Generating rb_${ARG_NAME}.h"
        VERBATIM
    )

    add_library(rb_${ARG_NAME} INTERFACE)
    target_sources(rb_${ARG_NAME} INTERFACE ${_rb_output})
    target_include_directories(rb_${ARG_NAME} INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
endfunction()
