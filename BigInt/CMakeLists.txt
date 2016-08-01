cmake_minimum_required(VERSION 2.6)
project(bigint)

add_executable(bigint main.cpp)

install(TARGETS bigint RUNTIME DESTINATION bin)
