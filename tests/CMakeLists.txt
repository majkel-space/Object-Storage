add_executable(
    test_server
    test_server.cpp
)

add_executable(
    integration_test
    integration_test.cpp
)

target_link_libraries(test_server
    PRIVATE
    GTest::gtest_main
    object_storage
)

target_link_libraries(integration_test
    PRIVATE
    GTest::gtest_main
    object_storage
    client_lib
)

include(GoogleTest)

gtest_discover_tests(test_server)
gtest_discover_tests(integration_test)
