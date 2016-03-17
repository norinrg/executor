cmake_minimum_required(VERSION 2.8)

project(newpi)

include_directories(executor)

add_executable(newpi
    src/AsyncExecutor
    src/AsyncQueue
    src/AsyncTimedExecutor
    src/AsyncTimedQueue
    test/main
    src/SyncExecutor
)

##IF (DEFINED CMAKE_CODE_COVERAGE)
#    INCLUDE(CodeCoverage)
#    SET(CMAKE_CXX_FLAGS "-std=c++11 -g -O0 -fprofile-arcs -ftest-coverage")
#    SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
#
#    SETUP_TARGET_FOR_COVERAGE(
#        cover                # Name for custom target.
#        newpi                # Name of the test driver executable that runs the tests.
#                            # NOTE! This should always have a ZERO as exit code
#                            # otherwise the coverage generation will not complete.
#        /home/norbert/dev/newPi/build/CMakeFiles/cover.dir           # Name of output directory.
#    )
##ENDIF()

install(TARGETS newpi RUNTIME DESTINATION bin)

find_package (Threads)

target_link_libraries (newpi ${CMAKE_THREAD_LIBS_INIT})
