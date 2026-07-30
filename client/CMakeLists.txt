cmake_minimum_required(VERSION 3.10)

project(Client)

find_package(Boost REQUIRED COMPONENTS system)

add_library(client_lib
    client.cpp
)

target_include_directories(client_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(client_lib
    PUBLIC
    Boost::system
    pthread
)

add_executable(client
    main_client.cpp
)

target_link_libraries(client
    PRIVATE
    client_lib
)
