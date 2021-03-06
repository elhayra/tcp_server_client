cmake_minimum_required(VERSION 3.13)
project(tcp_client_server)

find_package (Threads)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")

add_library(${PROJECT_NAME} 
        src/tcp_client.cpp
        src/tcp_server.cpp
        src/client.cpp)

option(SERVER_EXAMPLE "Build SERVER" ON)

if(SERVER_EXAMPLE)

        add_definitions(
                -DSERVER_EXAMPLE
        )

        add_executable(tcp_server server_example.cpp)

        target_link_libraries (tcp_server ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

endif()

option(CLIENT_EXAMPLE "Build CLIENT" ON)

if(CLIENT_EXAMPLE)

        add_definitions(
                -DCLIENT_EXAMPLE
        )

        add_executable(tcp_client client_example.cpp)

        target_link_libraries (tcp_client ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

endif()
