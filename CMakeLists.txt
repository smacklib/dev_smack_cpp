cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

# https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/

# Use -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON for command debugging.
# Use -DCMAKE_BUILD_TYPE=Release

# Set the variables before the project definition to get their
# default values int the cmake cache.
set( CMAKE_BUILD_TYPE "Release" CACHE STRING "Default.")
set( CMAKE_INSTALL_PREFIX "./dist" CACHE STRING "Default.")

project(tss_tools VERSION 0.1.0 LANGUAGES CXX)

enable_testing()

#
# Common for all architectures..
#
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14 -D_POSIX_SOURCE=1 -D_XOPEN_SOURCE=500")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib/googletest-release-1.8.1)
