find_package(GTest REQUIRED)

set(TEST_SOURCES
  tcp_server_test.cpp
)

add_executable(tcp_server_test ${TEST_SOURCES})
target_link_libraries(tcp_server_test
  PRIVATE
    ${PROJECT_NAME}
    GTest::GTest
    GTest::Main
)

add_test(NAME TcpServerTest COMMAND tcp_server_test) 