set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(client client.c)
add_executable(server server.c)
# target_link_libraries(server pthread)
target_link_libraries(server PRIVATE Threads::Threads)