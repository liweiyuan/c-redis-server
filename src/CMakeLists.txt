add_library(c_redis_server_lib
    server.c
    parser.c
    storage.c
)

target_link_libraries(c_redis_server_lib pthread)