aux_source_directory(. SRC)

add_executable(client client.c)
add_executable(server server.c)
target_link_libraries(server pthread)