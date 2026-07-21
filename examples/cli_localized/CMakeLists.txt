smack_add_resource_bundle(
    NAME  cli_localized
    INPUT "${CMAKE_CURRENT_SOURCE_DIR}/resourceBundle/cli_localized.properties"
)

add_executable( cli_localized
    cli_localized.cpp
    rb_cli_localized
)
target_link_libraries( cli_localized
  rb_cli_localized
  smack_cpp
)
