cmake_minimum_required(VERSION 3.10)

project(Server)

find_package(Boost REQUIRED COMPONENTS system)

add_executable(server.o
    connection_handler.cpp
    server.cpp
    main_server.cpp
)

target_include_directories(server.o PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(server.o PRIVATE -lboost_system -pthread)
target_link_libraries(server.o PRIVATE Boost::system)
