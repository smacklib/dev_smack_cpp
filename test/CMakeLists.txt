cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)

enable_testing()

option(INSTALL_GTEST "Include gtest in product install." OFF)
option(INSTALL_GMOCK "Include gmock in product install." OFF)

include(FetchContent)
	
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)

# After the following call, the CMake targets defined by googletest
# are available to the rest of the build.
FetchContent_MakeAvailable(googletest)

set(headers
  test_common.hpp
)

add_executable( smack_cpp_test
  main.cpp
  test_cli.cpp
  test_convert.cpp
  test_time_probe.cpp
  test_properties.cpp
  test_util.cpp
)

target_link_libraries( smack_cpp_test
  gtest
  smack_cpp
)

add_test(NAME smack_cpp_test COMMAND smack_cpp_test)
