cmake_minimum_required(VERSION 3.10)
project(c_redis_server C)

set(CMAKE_C_STANDARD 99)

add_executable(c_redis_server main.c src/config.c src/logger.c src/parser.c src/server.c src/storage.c)
target_include_directories(c_redis_server PUBLIC src)
target_link_libraries(c_redis_server pthread)
target_compile_options(c_redis_server PRIVATE -D_POSIX_C_SOURCE=200809L)