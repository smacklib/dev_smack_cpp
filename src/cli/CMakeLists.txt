
add_executable( smack
    cmdGenerateResourceBundle.cpp
    cmdGenerateResourceBundle.h
    smack.cpp
)

target_link_libraries( smack
  smack_cpp
)
