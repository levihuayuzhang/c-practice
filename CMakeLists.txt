cmake_minimum_required(VERSION 3.22.1)

project(c-practice CXX C)

SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(simple-threadpool-cpp)
add_subdirectory(hello)