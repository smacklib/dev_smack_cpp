add_subdirectory(cli)


configure_file(smack_version.h.in smack_version.h)

set(headers
    smack_locale.h
    smack_system.h
    smack_cli.hpp
    smack_convert.hpp
    smack_properties.hpp
	smack_resource_bundle.h
    smack_scheduler.h
	smack_threadpool.h
    smack_util.hpp
    smack_util_time_probe.hpp
)

set(implementation
smack_locale.cpp
)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND implementation smack_system_linux.cpp)
endif()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND implementation smack_system_win.cpp)
endif()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND implementation smack_system_darwin.cpp)
endif()

list(APPEND implementation
    smack_convert.cpp
    smack_properties.cpp
	smack_resource_bundle.cpp
    smack_util.cpp
    smack_util_time_probe.cpp
)

add_library(smack_cpp STATIC
    ${implementation}
    ${headers}
)

target_include_directories(smack_cpp INTERFACE .
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include/smack_cpp>
)

install(TARGETS
  smack_cpp
  DESTINATION lib)
install(FILES
  ${headers}
  DESTINATION include/smack_cpp)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/smack_version.h
  DESTINATION include/smack_cpp)
