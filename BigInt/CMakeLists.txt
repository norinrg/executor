cmake_minimum_required(VERSION 2.6)
project(bigint)

include_directories(../Catch/include)

add_executable(bigint
    #integer.cpp
    main.cpp
)

install(TARGETS bigint RUNTIME DESTINATION bin)
