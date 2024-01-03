aux_source_directory(. SRC)

add_executable(socket ${SRC})
# target_compile_features(tp PUBLIC cxx_std_17)
target_link_libraries(socket pthread)