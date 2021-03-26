cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)

set(headers
    test_common.hpp
)

add_executable( smack_cpp_test
    main.cpp
    test_cli.cpp
    test_time_probe.cpp
	test_util.cpp
)
target_link_libraries( smack_cpp_test
  gtest
  smack_cpp
)

add_executable( cli_class
    cli_class.cpp
    ${headers}
)
target_link_libraries( cli_class
  smack_cpp
)

add_executable( cli_class_argcv
    cli_class_argcv.cpp
    ${headers}
)
target_link_libraries( cli_class_argcv
  smack_cpp
)

add_executable( cli_functional
    cli_functional.cpp
    ${headers}
)
target_link_libraries( cli_functional
  smack_cpp
)

add_executable( cli_functional_argcv
    cli_functional_argcv.cpp
    ${headers}
)
target_link_libraries( cli_functional_argcv
  smack_cpp
)

add_test(NAME smack_cpp_test COMMAND smack_cpp_test)
