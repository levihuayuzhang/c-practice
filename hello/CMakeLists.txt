cmake_minimum_required(VERSION 3.22)

project(hello)

SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)

aux_source_directory(. SRC)

add_executable(hello ${SRC})

# target_link_libraries(hello pthread)