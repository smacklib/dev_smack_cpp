cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)

set(headers
    test_common.hpp
)

add_executable( tss_util_test
    main.cpp
	test_time_probe.cpp
)
target_link_libraries( tss_util_test
  gtest
  tss_tools
)

add_executable( cli_class
    cli_class.cpp
    ${headers}
)
target_link_libraries( cli_class
  tss_tools
)

add_executable( cli_functional
    cli_functional.cpp
    ${headers}
)
target_link_libraries( cli_functional
  tss_tools
)

add_test(NAME tss_util_test COMMAND tss_util_test)
