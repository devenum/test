cmake_minimum_required(VERSION 3.10)

project(lab-vector-naive CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(../../default-options.cmake)

add_executable(vector-test vector_test.cpp doctest_main.cpp vector_extra_tu.cpp)

add_executable(vector-test-std vector_test.cpp doctest_main.cpp)
target_compile_definitions(vector-test-std PUBLIC -DTEST_STD_VECTOR)
