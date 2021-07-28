cmake_minimum_required(VERSION 3.16)
project(sql_interpreter)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src/dbms)
add_subdirectory(src/exceptions)
add_subdirectory(src/logger)
add_subdirectory(src/rec_desc)
add_subdirectory(src/sockets)


add_executable(server src/server.cpp)
add_executable(client src/client.cpp)

target_link_libraries(client dbms exceptions logger runner parser scanner sockets)
