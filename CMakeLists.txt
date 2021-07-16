cmake_minimum_required(VERSION 3.13)
project(tcp_client_server)

find_package (Threads)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")

# Enable SERVER_EXAMPLE or CLIENT_EXAMPLE
# to compile one of the examples.
# Each example contains a main() function, so
# enable only one example, to avoid main() collision
add_definitions(
        -DSERVER_EXAMPLE
#        -DCLIENT_EXAMPLE
)

add_executable(tcp_client_server
        client_example.cpp
        server_example.cpp
        src/tcp_client.cpp
        src/tcp_server.cpp
        src/client.cpp include/common.h src/common.cpp src/pipe_ret_t.h.cpp include/client_event.h)

target_link_libraries (tcp_client_server ${CMAKE_THREAD_LIBS_INIT})
