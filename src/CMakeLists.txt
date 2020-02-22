cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

set(headers
    util_console_app.hpp
    util_time_probe.hpp
)
set(implementation
    util_console_app.cpp
    util_time_probe.cpp
)

add_library(tss_tools STATIC
    ${implementation}
    ${headers}
)

target_include_directories(tss_tools INTERFACE ..)

install(TARGETS
  tss_tools
  DESTINATION lib)
install(FILES
  ${headers}
  DESTINATION include/tss_tools)
