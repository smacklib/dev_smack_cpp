cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)

include(FetchContent)
	
FetchContent_Declare(
  abseil
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
  GIT_TAG       20210324.2
)

# After the following call, the CMake targets defined by abseil
# are available to the rest of the build.
FetchContent_MakeAvailable(abseil)

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

add_executable( cli_functional_2
    cli_functional_2.cpp
    ${headers}
)

target_link_libraries( cli_functional_2
  smack_cpp
  absl::strings
)
