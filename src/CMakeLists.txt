cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

set(headers
    Locale.h
	ResourceBundle.h
    smack_cli.hpp
    smack_convert.hpp
    smack_properties.hpp
    smack_util.hpp
    smack_util_time_probe.hpp
)
set(implementation
    Locale.cpp
	ResourceBundle.cpp
    smack_convert.cpp
    smack_properties.cpp
    smack_util.cpp
    smack_util_time_probe.cpp
)

add_library(smack_cpp STATIC
    ${implementation}
    ${headers}
)

target_include_directories(smack_cpp INTERFACE .)

install(TARGETS
  smack_cpp
  DESTINATION lib)
install(FILES
  ${headers}
  DESTINATION include/smack_cpp)
