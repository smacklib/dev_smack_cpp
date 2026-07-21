
smack_add_resource_bundle(
    NAME  smack
    INPUT "${CMAKE_SOURCE_DIR}/test/resources/resourceBundle/smack.properties"
)

add_executable( rb-smack-test
  cli-resource-bundle.cpp
)

target_link_libraries( rb-smack-test
  rb_smack
  smack_cpp
)
