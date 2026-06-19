
add_executable( smack
    cmdGenerateResourceBundle.cpp
    cmdGenerateResourceBundle.h
    cmdUsePrototype.cpp
    smack.cpp
)

target_link_libraries( smack
  smack_cpp
)
