
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)

# See https://github.com/google/googletest/blob/master/googletest/README.md
set(gtest_force_shared_crt ON CACHE BOOL "Use debug dll." FORCE)
set(BUILD_GMOCK ON CACHE BOOL "Build gmock." FORCE)
# After the following call, the CMake targets defined by googletest
# are available to the rest of the build.
FetchContent_MakeAvailable(googletest)

set(headers
  test_common.hpp
)

add_executable( smack_cpp_test
  main.cpp
  ../src/cli/cmdGenerateResourceBundle.cpp
  test_cli.cpp
  test_convert.cpp
  test_locale.cpp
  test_properties.cpp
  test_resource_bundle.cpp
  test_resource_bundle_generated.cpp
  test_resource_bundle_generator.cpp
  test_scheduler.cpp
  test_time_probe.cpp
  test_threadpool.cpp
  test_util.cpp
)

target_link_libraries( smack_cpp_test
  gtest
  gmock
  smack_cpp
  rb_smack
)

target_compile_definitions(smack_cpp_test PRIVATE
  TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

include(GoogleTest)
gtest_discover_tests(smack_cpp_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(cli-resource-bundle)
