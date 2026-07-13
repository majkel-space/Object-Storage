cmake_minimum_required(VERSION 3.10)

project(Client)

find_package(Boost REQUIRED COMPONENTS system)

add_executable(client.o
    client.cpp
    main_client.cpp
)

target_include_directories(client.o PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(client.o PRIVATE -pthread)
target_link_libraries(client.o PRIVATE Boost::system)
