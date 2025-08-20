cmake_minimum_required(VERSION 3.10)

project(bmp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(../../default-options.cmake)

include_directories(include)
add_executable(bmp src/main.cpp src/image.cpp)

add_executable(bmp-test src/doctest_main.cpp src/image_test.cpp)
target_include_directories(bmp-test PRIVATE doctest)
