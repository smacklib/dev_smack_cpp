
add_executable( rb-smack-test
  cli-resource-bundle.cpp
)

target_link_libraries( rb-smack-test
  rb_smack
  smack_cpp
)
