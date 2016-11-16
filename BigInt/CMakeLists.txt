cmake_minimum_required(VERSION 3.2)
project(bigint)

include_directories(../Catch/include)

add_executable(bigint
    #integer.cpp
    main.cpp
)

set_property(TARGET bigint PROPERTY CXX_STANDARD 14)

install(TARGETS bigint RUNTIME DESTINATION bin)
