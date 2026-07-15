cmake_minimum_required(VERSION 3.10)

project(Server)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

add_library(object_storage
    connection_handler.cpp
    protocols/HttpParser.cpp
    protocols/RespParser.cpp
    server.cpp
)

target_include_directories(
    object_storage
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    object_storage
    PUBLIC
    Boost::system
    Threads::Threads
)

add_executable(server
    main_server.cpp
)

target_link_libraries(
    server
    PRIVATE
    object_storage
)
