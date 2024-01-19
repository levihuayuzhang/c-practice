aux_source_directory(./ SRC)

# add_executable(simple-threadpool-c ${SRC})
add_executable(simple-threadpool-c main.c threadpool.c)
target_link_libraries(simple-threadpool-c pthread)