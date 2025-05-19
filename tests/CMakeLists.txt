# GoogleTestを検索
find_package(GTest REQUIRED)

# テスト実行ファイルのソース
set(TEST_SOURCES
  tcp_server_test.cpp
)

# テスト実行ファイルを作成
add_executable(tcp_server_test ${TEST_SOURCES})
target_link_libraries(tcp_server_test
  PRIVATE
    ${PROJECT_NAME}
    GTest::GTest
    GTest::Main
)

# テストをCTestに登録
add_test(NAME TcpServerTest COMMAND tcp_server_test) 