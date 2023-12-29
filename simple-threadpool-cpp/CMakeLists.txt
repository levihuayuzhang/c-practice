cmake_minimum_required(VERSION 3.22.1)

project(simple-threadPool-cpp CXX)

SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)

aux_source_directory(. SRC)

add_executable(tp ${SRC})
target_compile_features(tp PUBLIC cxx_std_17)
target_link_libraries(tp pthread)