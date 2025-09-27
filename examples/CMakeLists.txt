
set(headers
  ../test/test_common.hpp
)

add_executable( cli_hello
    cli_hello.cpp
    ${headers}
)

target_link_libraries( cli_hello
  smack_cpp
)

add_executable( cli_hello_new_type
    cli_hello_new_type.cpp
    ${headers}
)

target_link_libraries( cli_hello_new_type
  smack_cpp
)

add_executable( cli_class
    cli_class.cpp
    ${headers}
)

target_link_libraries( cli_class
  smack_cpp
)

add_executable( cli_functional
    cli_functional.cpp
    ${headers}
)

target_link_libraries( cli_functional
  smack_cpp
)
