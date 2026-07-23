# smack_add_resource_bundle(NAME <name> INPUT <path_to_properties>)
#
# Creates a STATIC library target named rb_<name> that exposes the
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
    set(_rb_stub   "${CMAKE_CURRENT_BINARY_DIR}/rb_${ARG_NAME}_stub.cpp")

    add_custom_command(
        OUTPUT  ${_rb_output}
        COMMAND $<TARGET_FILE:smack> generate-resource-bundle ${ARG_INPUT} ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS smack ${ARG_INPUT}
        COMMENT "Generating rb_${ARG_NAME}.h"
        VERBATIM
    )

    # Write a minimal stub at configure time so the STATIC library has a
    # compilable source.  Using STATIC (rather than INTERFACE) makes the
    # generated header a direct source-level dependency of a real build
    # artifact, which guarantees the custom command runs before any
    # consumer target regardless of parallel job count or make invocation.
    if(NOT EXISTS "${_rb_stub}")
        file(WRITE "${_rb_stub}" "// Auto-generated stub. Do not edit.\n")
    endif()

    add_library(rb_${ARG_NAME} STATIC ${_rb_stub} ${_rb_output})
    target_include_directories(rb_${ARG_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
endfunction()
