cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)

# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/

# Use -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON for command debugging.
# Use -DCMAKE_BUILD_TYPE=Release

# Set the variables before the project definition to get their
# default values int the cmake cache.
set( CMAKE_BUILD_TYPE "Release" CACHE STRING "Default.")
set( CMAKE_INSTALL_PREFIX "./dist" CACHE STRING "Default.")

project(smack_cpp VERSION 0.1.0 LANGUAGES CXX)

option(ENABLE_TESTS "Enable testing" OFF)
option(ENABLE_EXAMPLES "Enable building examples" OFF)

#
# Common for all architectures..
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

add_subdirectory(src)

if (ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (ENABLE_TESTS)
    include(FetchContent)
	
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        release-1.10.0
    )

    # After the following call, the CMake targets defined by googletest
    # are available to the rest of the build.
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_subdirectory(test)
endif ()
