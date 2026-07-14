add_executable(
    test_server
    test_server.cpp
)

target_link_libraries(
    test_server
    PRIVATE
    GTest::gtest_main
    object_storage
)

include(GoogleTest)
gtest_discover_tests(test_server)
