cmake_minimum_required(VERSION 3.13)
project(intercom)

find_package (Threads)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")


add_executable(intercom main.cpp
        src/tcp_client.cpp
        src/tcp_server.cpp
        src/client.cpp

        include/tcp_client.h
        include/tcp_server.h
        include/client_observer.h
        include/client.h
        include/pipe_ret_t.h
        include/server_observer.h src/client.cpp)

target_link_libraries (intercom ${CMAKE_THREAD_LIBS_INIT})
