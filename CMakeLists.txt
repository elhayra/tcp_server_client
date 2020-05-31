cmake_minimum_required(VERSION 3.13)
project(tcp_client_server)

find_package (Threads)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")


add_executable(tcp_client_server
        main.cpp
        src/tcp_client.cpp
        src/tcp_server.cpp
        src/client.cpp
        src/client.cpp)

target_link_libraries (tcp_client_server ${CMAKE_THREAD_LIBS_INIT})
