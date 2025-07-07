cmake_minimum_required(VERSION 3.10)
project(c_redis_server C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(src)

add_executable(c_redis_server main.c)
target_include_directories(c_redis_server PUBLIC src)
target_link_libraries(c_redis_server c_redis_server_lib)