cmake_minimum_required(VERSION 3.10)

project(lab-calc C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(../../default-options.cmake)

include(CheckLibraryExists)
CHECK_LIBRARY_EXISTS(m sqrt "" HAVE_LIB_M)
if (HAVE_LIB_M)
    link_libraries(m)
endif (HAVE_LIB_M)

add_subdirectory(lib)  # Add only after all compilation options are specified.

add_executable(calc-test doctest_main.cpp calc_test.cpp)
target_link_libraries(calc-test calc)

add_executable(calc-test-c calc_test_c.c)
target_link_libraries(calc-test-c calc)

add_executable(calc-cli calc_cli.c)
target_link_libraries(calc-cli calc)

add_executable(tasks-info tasks_info.cpp)
