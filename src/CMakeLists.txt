cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

set(headers
    util_console_app.hpp
    util_time_probe.hpp
)
set(implementation
    util_console_app.cpp
    util_time_probe.cpp
)

add_library(smack_cpp STATIC
    ${implementation}
    ${headers}
)

target_include_directories(smack_cpp INTERFACE ..)

install(TARGETS
  smack_cpp
  DESTINATION lib)
install(FILES
  ${headers}
  DESTINATION include/smack_cpp)
