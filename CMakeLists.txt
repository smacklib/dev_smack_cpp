cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

# Use -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON for command debugging.
# Use -DCMAKE_BUILD_TYPE=Release

# Set the variables before the project definition to get their
# default values into the cmake cache.
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Default.")
set(CMAKE_INSTALL_PREFIX "./dist" CACHE STRING "Default.")

#
# Common for all architectures..
#
set(SMACK_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD ${SMACK_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(smack_cpp VERSION ${SMACK_CXX_STANDARD}.1.0 LANGUAGES CXX)

option(ENABLE_TESTS "Enable testing" OFF)
option(ENABLE_EXAMPLES "Enable building examples" OFF)

add_subdirectory(src)

if (ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (ENABLE_TESTS)
    set(CMAKE_CTEST_ARGUMENTS "--output-on-failure")
    enable_testing()
    add_subdirectory(test)
endif ()
